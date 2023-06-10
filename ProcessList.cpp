/* This file is part of IntelliTask application developed by Stefan-Mihai MOGA.

IntelliTask is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliTask is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliTask. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

#include "stdafx.h"
#include "IntelliTask.h"
#include "ProcessList.h"
#include "VersionInfo.h"

IMPLEMENT_DYNAMIC(CProcessData, CObject)

IMPLEMENT_DYNAMIC(CSystemSnapshot, CObject)

///////////////////////////////////////////////////////////////////////////////
// CProcessData member functions
///////////////////////////////////////////////////////////////////////////////

CProcessData::CProcessData()
{
	m_dwProcessID = 0;
	m_dwParentProcessID = 0;
	m_dwPriority = 0;
	m_cpuUsage = 0;
	m_memUsage = 0;
}

CProcessData::CProcessData(DWORD dwProcessID, DWORD dwParentProcessID, DWORD dwPriority, DOUBLE cpuUsage, DWORD memUsage,
		CString strFileName, CString strFilePath, CString strDescription, CString strCompany, CString strVersion)
{
	m_dwProcessID = dwProcessID;
	m_dwParentProcessID = dwParentProcessID;
	m_dwPriority = dwPriority;
	m_cpuUsage = cpuUsage;
	m_memUsage = memUsage;
	m_strFileName = strFileName;
	m_strFilePath = strFilePath;
	m_strDescription = strDescription;
	m_strCompany = strCompany;
	m_strVersion = strVersion;
}

CProcessData::~CProcessData()
{
}

///////////////////////////////////////////////////////////////////////////////
// CSystemSnapshot member functions
///////////////////////////////////////////////////////////////////////////////

CSystemSnapshot::CSystemSnapshot()
{
}

CSystemSnapshot::~CSystemSnapshot()
{
	VERIFY(RemoveAll());
}

bool CSystemSnapshot::RemoveAll()
{
	const int nSize = (int)m_arrProcessList.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_arrProcessList.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);
		delete pProcessData;
		pProcessData = nullptr;
	}
	m_arrProcessList.RemoveAll();
	return true;
}

bool CSystemSnapshot::Refresh()
{
	HANDLE hSnapshot = nullptr;
	PROCESSENTRY32 pe32 = { 0 };

	VERIFY(RemoveAll());
	if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);
		if (!Process32First(hSnapshot, &pe32))
		{
			return false;
		}

		do
		{
			VERIFY(InsertProcess(pe32));
		} while (Process32Next(hSnapshot, &pe32));

		VERIFY(CloseHandle(hSnapshot));
	}
	else
	{
		return false;
	}
	return true;
}

CProcessData* CSystemSnapshot::InsertProcess(PROCESSENTRY32& pe32)
{
	HANDLE hProcess = nullptr;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);

	CProcessData* pProcessData = new CProcessData();
	m_arrProcessList.Add(pProcessData);
	pProcessData->SetProcessID(pe32.th32ProcessID);
	pProcessData->SetParentProcessID(pe32.th32ParentProcessID);
	pProcessData->SetFileName(pe32.szExeFile);

	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pe32.th32ProcessID)) != nullptr)
	{
		pProcessData->m_pCpuUsage.SetProcessID(pe32.th32ProcessID);
		pProcessData->SetProcessorUsage(pProcessData->m_pCpuUsage.GetUsage());
		if (GetProcessMemoryInfo(hProcess, &pmc, pmc.cb))
		{
			pProcessData->SetMemoryUsage((DWORD)pmc.WorkingSetSize);
		}

		TCHAR lpszFullPath[MAX_PATH] = { 0 };
		if (GetModuleFileNameEx(hProcess, nullptr, lpszFullPath, MAX_PATH))
		{
			pProcessData->SetFilePath(lpszFullPath);
			CVersionInfo pVersionInfo;
			if (pVersionInfo.Load(lpszFullPath))
			{
				pProcessData->SetDescription(pVersionInfo.GetFileDescription().c_str());
				pProcessData->SetCompany(pVersionInfo.GetCompanyName().c_str());
				pProcessData->SetVersion(pVersionInfo.GetFileVersionAsString().c_str());
			}
		}
		VERIFY(CloseHandle(hProcess));
	}
	return pProcessData;
}

CProcessData* CSystemSnapshot::UpdateProcess(DWORD dwProcessID)
{
	HANDLE hProcess = nullptr;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);

	CProcessData* pProcessData = GetProcessID(dwProcessID);
	if (pProcessData != nullptr)
	{
		if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, dwProcessID)) != nullptr)
		{
			pProcessData->SetProcessorUsage(pProcessData->m_pCpuUsage.GetUsage());
			if (GetProcessMemoryInfo(hProcess, &pmc, pmc.cb))
			{
				pProcessData->SetMemoryUsage((DWORD)pmc.WorkingSetSize);
			}
			VERIFY(CloseHandle(hProcess));
		}
		return pProcessData;
	}
	return nullptr;
}

bool CSystemSnapshot::RemoveProcess(DWORD dwProcessID)
{
	const int nSize = (int)m_arrProcessList.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_arrProcessList.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);
		if (pProcessData->GetProcessID() == dwProcessID)
		{
			delete pProcessData;
			pProcessData = nullptr;
			m_arrProcessList.RemoveAt(nIndex);
			return true;
		}
	}
	return false;
}

CProcessData* CSystemSnapshot::GetProcessID(DWORD dwProcessID)
{
	const int nSize = (int)m_arrProcessList.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_arrProcessList.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);
		if (pProcessData->GetProcessID() == dwProcessID)
		{
			return pProcessData;
		}
	}
	return nullptr;
}
