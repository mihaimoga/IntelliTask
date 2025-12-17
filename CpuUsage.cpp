/* Copyright (C) 2012-2026 Stefan-Mihai MOGA
This file is part of IntelliTask application developed by Stefan-Mihai MOGA.
IntelliTask is an alternative Windows version to the famous Task Manager!

IntelliTask is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliTask is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliTask. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

#include "StdAfx.h"
#include <windows.h>
#include "CpuUsage.h"

CpuUsage::CpuUsage(void)
	: m_dwProcessID(0)
	, m_nCpuUsage(-1)
	, m_dwLastRun(0)
	, m_lRunCount(0)
{
	ZeroMemory(&m_ftPrevSysKernel, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevSysUser, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevProcKernel, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevProcUser, sizeof(FILETIME));

	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	m_nProcessors = sysInfo.dwNumberOfProcessors;
}

/**********************************************
* CpuUsage::GetUsage
* returns the percent of the CPU that this process
* has used since the last time the method was called.
* If there is not enough information, -1 is returned.
* If the method is recalled to quickly, the previous value
* is returned.
***********************************************/
DOUBLE CpuUsage::GetUsage()
{
	HANDLE hProcess = nullptr;
	// create a local copy to protect against race conditions in setting the member variable
	DOUBLE nCpuCopy = m_nCpuUsage;
	if (::InterlockedIncrement(&m_lRunCount) == 1)
	{
		// If this is called too often, the measurement itself will greatly affect the results.
		if (!EnoughTimePassed())
		{
			::InterlockedDecrement(&m_lRunCount);
			return nCpuCopy;
		}

		FILETIME ftSysIdle, ftSysKernel, ftSysUser;
		FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;
		if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, m_dwProcessID)) != nullptr)
		{
			if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
				!GetProcessTimes(hProcess, &ftProcCreation,
				&ftProcExit, &ftProcKernel, &ftProcUser))
			{
				::InterlockedDecrement(&m_lRunCount);
				CloseHandle(hProcess);
				return nCpuCopy;
			}
			CloseHandle(hProcess);
			hProcess = nullptr;

			/* CPU usage is calculated by getting the total amount of time 
			the system has operated since the last measurement 
			(made up of kernel + user) and the total
			amount of time the process has run (kernel + user) */
			ULONGLONG ftSysIdleDiff = SubtractTimes(ftSysIdle, m_ftPrevSysIdle);
			ULONGLONG ftSysKernelDiff = SubtractTimes(ftSysKernel, m_ftPrevSysKernel);
			ULONGLONG ftSysUserDiff = SubtractTimes(ftSysUser, m_ftPrevSysUser);

			ULONGLONG ftProcKernelDiff = SubtractTimes(ftProcKernel, m_ftPrevProcKernel);
			ULONGLONG ftProcUserDiff = SubtractTimes(ftProcUser, m_ftPrevProcUser);

			ULONGLONG nTotalSys =  ftSysKernelDiff + ftSysUserDiff - ftSysIdleDiff;
			ULONGLONG nTotalProc = ftProcKernelDiff + ftProcUserDiff;

			if (nTotalSys > 0)
			{
				m_nCpuUsage = ((100.0 * (DOUBLE)nTotalProc) / (DOUBLE)nTotalSys) / (DOUBLE)m_nProcessors;
			}

			m_ftPrevSysIdle = ftSysIdle;
			m_ftPrevSysKernel = ftSysKernel;
			m_ftPrevSysUser = ftSysUser;
			m_ftPrevProcKernel = ftProcKernel;
			m_ftPrevProcUser = ftProcUser;

			m_dwLastRun = GetTickCount64();
			nCpuCopy = m_nCpuUsage;
		}
	}

	::InterlockedDecrement(&m_lRunCount);
	return nCpuCopy;
}

ULONGLONG CpuUsage::SubtractTimes(const FILETIME& ftA, const FILETIME& ftB)
{
	LARGE_INTEGER a, b;
	a.LowPart = ftA.dwLowDateTime;
	a.HighPart = ftA.dwHighDateTime;
	b.LowPart = ftB.dwLowDateTime;
	b.HighPart = ftB.dwHighDateTime;
	return a.QuadPart - b.QuadPart;
}

bool CpuUsage::EnoughTimePassed()
{
	const int minElapsedMS = 250; // milliseconds
	ULONGLONG dwCurrentTickCount = GetTickCount64();
	return (dwCurrentTickCount - m_dwLastRun) > minElapsedMS;
}

// http://www.philosophicalgeek.com/2009/01/03/determine-cpu-usage-of-current-process-c-and-c/
