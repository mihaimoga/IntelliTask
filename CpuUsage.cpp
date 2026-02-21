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

/**
 * @brief Constructor for the CpuUsage class
 * 
 * Initializes CPU usage tracking by zeroing out previous timing values
 * and retrieving the number of processors in the system.
 */
CpuUsage::CpuUsage()
	: m_dwProcessID(0)
	, m_nCpuUsage(-1)
	, m_dwLastRun(0)
	, m_lRunCount(0)
{
	// Zero out all previous FILETIME structures to ensure clean initial state
	ZeroMemory(&m_ftPrevSysIdle, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevSysKernel, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevSysUser, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevProcKernel, sizeof(FILETIME));
	ZeroMemory(&m_ftPrevProcUser, sizeof(FILETIME));

	// Get the number of processors to normalize CPU usage across multiple cores
	SYSTEM_INFO sysInfo;
	GetSystemInfo(&sysInfo);
	m_nProcessors = sysInfo.dwNumberOfProcessors;
}

/**
 * @brief Calculate the CPU usage percentage for the monitored process
 * 
 * Returns the percent of the CPU that this process has used since the last
 * time the method was called. The calculation is based on the difference in
 * system time and process time between measurements.
 * 
 * @return CPU usage as a percentage (0.0-100.0), or -1 if there is not enough
 *         information. If called too quickly, returns the previous measurement.
 * 
 * @note This method uses thread-safe counters to prevent concurrent execution
 *       which could affect measurement accuracy.
 */
DOUBLE CpuUsage::GetUsage()
{
	HANDLE hProcess = nullptr;
	// Create a local copy to protect against race conditions in setting the member variable
	DOUBLE nCpuCopy = m_nCpuUsage;

	// Use interlocked increment to ensure only one thread executes the measurement at a time
	if (::InterlockedIncrement(&m_lRunCount) == 1)
	{
		// If this is called too often, the measurement itself will greatly affect the results.
		// Check if enough time has passed since the last measurement (minimum 250ms)
		if (!EnoughTimePassed())
		{
			::InterlockedDecrement(&m_lRunCount);
			return nCpuCopy;
		}

		FILETIME ftSysIdle, ftSysKernel, ftSysUser;
		FILETIME ftProcCreation, ftProcExit, ftProcKernel, ftProcUser;

		// Open the process with query permissions to read timing information
		if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, m_dwProcessID)) != nullptr)
		{
			// Get system-wide CPU times (idle, kernel, user) and process-specific times
			// If either call fails, return the cached value
			if (!GetSystemTimes(&ftSysIdle, &ftSysKernel, &ftSysUser) ||
				!GetProcessTimes(hProcess, &ftProcCreation,
				&ftProcExit, &ftProcKernel, &ftProcUser))
			{
				::InterlockedDecrement(&m_lRunCount);
				CloseHandle(hProcess);
				return nCpuCopy;
			}
			// Close the handle immediately after getting the timing information
			CloseHandle(hProcess);
			hProcess = nullptr;

			/* CPU usage is calculated by getting the total amount of time 
			the system has operated since the last measurement 
			(made up of kernel + user) and the total
			amount of time the process has run (kernel + user) */

			// Calculate the difference in system times since the last measurement
			ULONGLONG ftSysIdleDiff = SubtractTimes(ftSysIdle, m_ftPrevSysIdle);
			ULONGLONG ftSysKernelDiff = SubtractTimes(ftSysKernel, m_ftPrevSysKernel);
			ULONGLONG ftSysUserDiff = SubtractTimes(ftSysUser, m_ftPrevSysUser);

			// Calculate the difference in process times since the last measurement
			ULONGLONG ftProcKernelDiff = SubtractTimes(ftProcKernel, m_ftPrevProcKernel);
			ULONGLONG ftProcUserDiff = SubtractTimes(ftProcUser, m_ftPrevProcUser);

			// Total system time is kernel + user time, minus idle time
			ULONGLONG nTotalSys =  ftSysKernelDiff + ftSysUserDiff - ftSysIdleDiff;
			// Total process time is kernel + user time for this specific process
			ULONGLONG nTotalProc = ftProcKernelDiff + ftProcUserDiff;

			// Calculate the percentage: (process time / system time) * 100
			// Divide by number of processors to get per-core usage
			if (nTotalSys > 0)
			{
				m_nCpuUsage = ((100.0 * (DOUBLE)nTotalProc) / (DOUBLE)nTotalSys) / (DOUBLE)m_nProcessors;
			}

			// Store current values as previous values for the next measurement
			m_ftPrevSysIdle = ftSysIdle;
			m_ftPrevSysKernel = ftSysKernel;
			m_ftPrevSysUser = ftSysUser;
			m_ftPrevProcKernel = ftProcKernel;
			m_ftPrevProcUser = ftProcUser;

			// Update the last run timestamp
			m_dwLastRun = GetTickCount64();
			// Update the local copy with the new calculation
			nCpuCopy = m_nCpuUsage;
		}
	}

	// Decrement the run count to allow future calls
	::InterlockedDecrement(&m_lRunCount);
	return nCpuCopy;
}

/**
 * @brief Subtract two FILETIME values to get the time difference
 * 
 * Converts FILETIME structures to 64-bit integers and calculates the difference.
 * 
 * @param ftA The first (later) FILETIME value
 * @param ftB The second (earlier) FILETIME value to subtract from ftA
 * @return The difference in 100-nanosecond intervals
 */
ULONGLONG CpuUsage::SubtractTimes(const FILETIME& ftA, const FILETIME& ftB)
{
	LARGE_INTEGER a, b;
	// Convert FILETIME structures to 64-bit integers for arithmetic
	// FILETIME consists of two 32-bit values (low and high parts)
	a.LowPart = ftA.dwLowDateTime;
	a.HighPart = ftA.dwHighDateTime;
	b.LowPart = ftB.dwLowDateTime;
	b.HighPart = ftB.dwHighDateTime;
	// Subtract the QuadPart values (64-bit integer representation)
	return a.QuadPart - b.QuadPart;
}

/**
 * @brief Check if enough time has passed since the last CPU measurement
 * 
 * Ensures a minimum time interval between measurements to prevent the
 * measurement process itself from affecting the results.
 * 
 * @return true if at least 250 milliseconds have passed since the last run,
 *         false otherwise
 */
bool CpuUsage::EnoughTimePassed()
{
	// Minimum time that must elapse between measurements to ensure accuracy
	const int minElapsedMS = 250; // milliseconds

	// Get the current system tick count
	ULONGLONG dwCurrentTickCount = GetTickCount64();

	// Return true if more than 250ms have passed since the last measurement
	return (dwCurrentTickCount - m_dwLastRun) > minElapsedMS;
}

// http://www.philosophicalgeek.com/2009/01/03/determine-cpu-usage-of-current-process-c-and-c/
