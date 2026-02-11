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

#include "stdafx.h"
#include "IntelliTask.h"
#include "ProcessList.h"
#include "VersionInfo.h"

IMPLEMENT_DYNAMIC(CProcessData, CObject)

IMPLEMENT_DYNAMIC(CSystemSnapshot, CObject)

///////////////////////////////////////////////////////////////////////////////
// CProcessData member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Default constructor for CProcessData
 * Initializes all process data members to zero/default values
 */
	CProcessData::CProcessData()
{
	m_dwProcessID = 0;
	m_dwParentProcessID = 0;
	m_dwPriority = 0;
	m_cpuUsage = 0;
	m_memUsage = 0;
}

/**
 * @brief Parameterized constructor for CProcessData
 * @param dwProcessID The process identifier
 * @param dwParentProcessID The parent process identifier
 * @param dwPriority The process priority level
 * @param cpuUsage The CPU usage percentage
 * @param memUsage The memory usage in bytes
 * @param strFileName The executable file name
 * @param strFilePath The full path to the executable
 * @param strDescription The file description from version info
 * @param strCompany The company name from version info
 * @param strVersion The version string from version info
 */
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

/**
 * @brief Destructor for CProcessData
 */
CProcessData::~CProcessData()
{
}

///////////////////////////////////////////////////////////////////////////////
// CSystemSnapshot member functions
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Default constructor for CSystemSnapshot
 */
CSystemSnapshot::CSystemSnapshot()
{
}

/**
 * @brief Destructor for CSystemSnapshot
 * Cleans up all allocated process data objects
 */
CSystemSnapshot::~CSystemSnapshot()
{
	VERIFY(RemoveAll());
}

/**
 * @brief Removes all process data from the snapshot
 * Deletes all CProcessData objects and clears the array
 * @return true on success
 */
bool CSystemSnapshot::RemoveAll()
{
	const int nSize = (int)m_arrProcessList.GetSize();
	// Iterate through all processes and delete them
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_arrProcessList.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);
		delete pProcessData;
		pProcessData = nullptr;
	}
	// Clear the array
	m_arrProcessList.RemoveAll();
	return true;
}

/**
 * @brief Refreshes the system snapshot by enumerating all running processes
 * Uses CreateToolhelp32Snapshot to get current process information
 * @return true if snapshot was successfully created, false otherwise
 */
bool CSystemSnapshot::Refresh()
{
	HANDLE hSnapshot = nullptr;
	PROCESSENTRY32 pe32 = { 0 };

	// Clear existing process list
	VERIFY(RemoveAll());

	// Create a snapshot of all processes in the system
	if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) != INVALID_HANDLE_VALUE)
	{
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// Get the first process information
		if (!Process32First(hSnapshot, &pe32))
		{
			return false;
		}

		// Iterate through all processes in the snapshot
		do
		{
			VERIFY(InsertProcess(pe32));
		} while (Process32Next(hSnapshot, &pe32));

		// Close the snapshot handle
		VERIFY(CloseHandle(hSnapshot));
	}
	else
	{
		return false;
	}
	return true;
}

/**
 * @brief Inserts a new process into the snapshot
 * Gathers detailed information about the process including CPU usage, memory, and version info
 * @param pe32 The PROCESSENTRY32 structure containing basic process information
 * @return Pointer to the newly created CProcessData object
 */
CProcessData* CSystemSnapshot::InsertProcess(PROCESSENTRY32& pe32)
{
	HANDLE hProcess = nullptr;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);

	// Create new process data object and add it to the list
	CProcessData* pProcessData = new CProcessData();
	m_arrProcessList.Add(pProcessData);

	// Set basic process information from PROCESSENTRY32
	pProcessData->SetProcessID(pe32.th32ProcessID);
	pProcessData->SetParentProcessID(pe32.th32ParentProcessID);
	pProcessData->SetFileName(pe32.szExeFile);

	// Open process handle to query additional information
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pe32.th32ProcessID)) != nullptr)
	{
		// Initialize CPU usage monitoring
		pProcessData->m_pCpuUsage.SetProcessID(pe32.th32ProcessID);
		pProcessData->SetProcessorUsage(pProcessData->m_pCpuUsage.GetUsage());

		// Get memory usage information
		if (GetProcessMemoryInfo(hProcess, &pmc, pmc.cb))
		{
			pProcessData->SetMemoryUsage((DWORD)pmc.WorkingSetSize);
		}

		// Get full path to the executable
		TCHAR lpszFullPath[MAX_PATH] = { 0 };
		if (GetModuleFileNameEx(hProcess, nullptr, lpszFullPath, MAX_PATH))
		{
			pProcessData->SetFilePath(lpszFullPath);

			// Extract version information from the executable
			CVersionInfo pVersionInfo;
			if (pVersionInfo.Load(lpszFullPath))
			{
				pProcessData->SetDescription(pVersionInfo.GetFileDescription().c_str());
				pProcessData->SetCompany(pVersionInfo.GetCompanyName().c_str());
				pProcessData->SetVersion(pVersionInfo.GetFileVersionAsString().c_str());
			}
		}

		// Close the process handle
		VERIFY(CloseHandle(hProcess));
	}
	return pProcessData;
}

/**
 * @brief Updates CPU and memory usage for an existing process
 * @param dwProcessID The process identifier to update
 * @return Pointer to the updated CProcessData object, or nullptr if process not found
 */
CProcessData* CSystemSnapshot::UpdateProcess(DWORD dwProcessID)
{
	HANDLE hProcess = nullptr;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);

	// Find the process in the snapshot
	CProcessData* pProcessData = GetProcessID(dwProcessID);
	if (pProcessData != nullptr)
	{
		// Open process handle to query updated information
		if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, dwProcessID)) != nullptr)
		{
			// Update CPU usage
			pProcessData->SetProcessorUsage(pProcessData->m_pCpuUsage.GetUsage());

			// Update memory usage
			if (GetProcessMemoryInfo(hProcess, &pmc, pmc.cb))
			{
				pProcessData->SetMemoryUsage((DWORD)pmc.WorkingSetSize);
			}

			// Close the process handle
			VERIFY(CloseHandle(hProcess));
		}
		return pProcessData;
	}
	return nullptr;
}

/**
 * @brief Removes a process from the snapshot
 * @param dwProcessID The process identifier to remove
 * @return true if process was found and removed, false otherwise
 */
bool CSystemSnapshot::RemoveProcess(DWORD dwProcessID)
{
	const int nSize = (int)m_arrProcessList.GetSize();
	// Search for the process in the list
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_arrProcessList.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);

		// If process found, delete it and remove from array
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

/**
 * @brief Retrieves process data by process ID
 * @param dwProcessID The process identifier to search for
 * @return Pointer to the CProcessData object if found, nullptr otherwise
 */
CProcessData* CSystemSnapshot::GetProcessID(DWORD dwProcessID)
{
	const int nSize = (int)m_arrProcessList.GetSize();
	// Search for the process in the list
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_arrProcessList.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);

		// Return the process data if process ID matches
		if (pProcessData->GetProcessID() == dwProcessID)
		{
			return pProcessData;
		}
	}
	return nullptr;
}
