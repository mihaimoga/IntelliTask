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

/**
 * @brief Dynamic class implementation macro for CProcessData
 * 
 * Enables runtime type information (RTTI) for the CProcessData class.
 */
IMPLEMENT_DYNAMIC(CProcessData, CObject)

/**
 * @brief Dynamic class implementation macro for CSystemSnapshot
 * 
 * Enables runtime type information (RTTI) for the CSystemSnapshot class.
 */
IMPLEMENT_DYNAMIC(CSystemSnapshot, CObject)

///////////////////////////////////////////////////////////////////////////////
// CProcessData member functions
// 
// This class encapsulates information about a single running process,
// including identification, performance metrics, and version information.
///////////////////////////////////////////////////////////////////////////////

/**
 * @brief Default constructor for CProcessData
 * Initializes all process data members to zero/default values
 */
CProcessData::CProcessData()
{
	// Initialize process identifiers
	m_dwProcessID = 0;
	m_dwParentProcessID = 0;
	// Initialize process metrics
	m_dwPriority = 0;
	m_cpuUsage = 0;
	m_memUsage = 0;
	// String members are auto-initialized by CString constructor
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
	// Store process identification information
	m_dwProcessID = dwProcessID;
	m_dwParentProcessID = dwParentProcessID;

	// Store process performance metrics
	m_dwPriority = dwPriority;
	m_cpuUsage = cpuUsage;
	m_memUsage = memUsage;

	// Store process file information
	m_strFileName = strFileName;
	m_strFilePath = strFilePath;

	// Store version information
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
// 
// This class manages a snapshot of all running processes in the system.
// It provides functionality to enumerate, update, and query process information
// using the Windows Toolhelp API.
//
// THREAD SAFETY: This class is NOT thread-safe. Access should be synchronized
// if used from multiple threads.
//
// PERFORMANCE: Enumerating all processes can be expensive. Use Refresh() sparingly
// and prefer UpdateProcess() for individual process updates.
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
 * 
 * @note This method is called automatically by the destructor.
 * Manually calling this will invalidate all pointers to CProcessData objects.
 * 
 * @warning Ensure no other code is holding pointers to process data before calling this.
 * 
 * @return true on success
 */
bool CSystemSnapshot::RemoveAll()
{
	// Get the number of processes in the snapshot
	const int nSize = (int)m_arrProcessList.GetSize();

	// === ITERATE AND DELETE PROCESSES ===
	// Iterate through all processes and delete them
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		// Get the process data object
		CProcessData* pProcessData = m_arrProcessList.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);

		// Delete the process data object and set pointer to null
		delete pProcessData;
		pProcessData = nullptr;
	}

	// === CLEAR THE ARRAY ===
	// Clear the array (removes all elements)
	m_arrProcessList.RemoveAll();
	return true;
}

/**
 * @brief Refreshes the system snapshot by enumerating all running processes
 * Uses CreateToolhelp32Snapshot to get current process information
 * 
 * @note This operation can be expensive as it enumerates ALL running processes.
 * Consider using UpdateProcess() for updating individual processes instead.
 * 
 * @note Requires at least PROCESS_QUERY_INFORMATION access rights.
 * Some system processes may not be accessible without elevated privileges.
 * 
 * @return true if snapshot was successfully created, false otherwise
 */
bool CSystemSnapshot::Refresh()
{
	HANDLE hSnapshot = nullptr;
	PROCESSENTRY32 pe32 = { 0 };

	// === CLEAR EXISTING DATA ===
	// Clear existing process list before refreshing
	VERIFY(RemoveAll());

	// === CREATE PROCESS SNAPSHOT ===
	// Create a snapshot of all processes in the system using Toolhelp API
	// TH32CS_SNAPPROCESS flag includes all processes
	if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) != INVALID_HANDLE_VALUE)
	{
		// Initialize the size member before using the structure
		pe32.dwSize = sizeof(PROCESSENTRY32);

		// === GET FIRST PROCESS ===
		// Get the first process information from the snapshot
		if (!Process32First(hSnapshot, &pe32))
		{
			return false;
		}

		// === ENUMERATE ALL PROCESSES ===
		// Iterate through all processes in the snapshot
		do
		{
			// Insert each process into our internal list
			VERIFY(InsertProcess(pe32));
		} while (Process32Next(hSnapshot, &pe32));

		// === CLEANUP ===
		// Close the snapshot handle to free resources
		VERIFY(CloseHandle(hSnapshot));
	}
	else
	{
		// Failed to create snapshot
		return false;
	}
	return true;
}

/**
 * @brief Inserts a new process into the snapshot
 * Gathers detailed information about the process including CPU usage, memory, and version info
 * 
 * @param pe32 The PROCESSENTRY32 structure containing basic process information
 * 
 * @note OpenProcess may fail for system processes or processes running with higher privileges.
 * In such cases, basic information (PID, name) is still stored, but detailed info will be empty.
 * 
 * @note Version information extraction may fail for:
 * - Processes without version resources
 * - Native system processes
 * - Processes with inaccessible executables
 * 
 * @return Pointer to the newly created CProcessData object (always non-null)
 */
CProcessData* CSystemSnapshot::InsertProcess(PROCESSENTRY32& pe32)
{
	HANDLE hProcess = nullptr;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);

	// === CREATE PROCESS DATA OBJECT ===
	// Create new process data object and add it to the list
	CProcessData* pProcessData = new CProcessData();
	m_arrProcessList.Add(pProcessData);

	// === SET BASIC INFORMATION ===
	// Set basic process information from PROCESSENTRY32 structure
	pProcessData->SetProcessID(pe32.th32ProcessID);
	pProcessData->SetParentProcessID(pe32.th32ParentProcessID);
	pProcessData->SetFileName(pe32.szExeFile);

	// === OPEN PROCESS FOR DETAILED INFORMATION ===
	// Open process handle with query and read permissions
	// PROCESS_QUERY_INFORMATION: Required to get process info
	// PROCESS_VM_READ: Required to read process memory information
	// Note: OpenProcess may fail for system/protected processes - this is expected
	if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pe32.th32ProcessID)) != nullptr)
	{
		// === GET CPU USAGE ===
		// Initialize CPU usage monitoring for this process
		// Note: First call may return 0% as baseline is established
		pProcessData->m_pCpuUsage.SetProcessID(pe32.th32ProcessID);
		// Get current CPU usage percentage
		pProcessData->SetProcessorUsage(pProcessData->m_pCpuUsage.GetUsage());

		// === GET MEMORY USAGE ===
		// Query process memory counters
		if (GetProcessMemoryInfo(hProcess, &pmc, pmc.cb))
		{
			// Store the working set size (physical memory used)
			// Note: This is the current physical RAM usage, not virtual memory
			pProcessData->SetMemoryUsage((DWORD)pmc.WorkingSetSize);
		}

		// === GET EXECUTABLE PATH AND VERSION INFO ===
		// Get full path to the executable
		TCHAR lpszFullPath[MAX_PATH] = { 0 };
		if (GetModuleFileNameEx(hProcess, nullptr, lpszFullPath, MAX_PATH))
		{
			// Store the full file path
			pProcessData->SetFilePath(lpszFullPath);

			// === EXTRACT VERSION INFORMATION ===
			// Extract version information from the executable file
			// Note: This may fail for system processes or files without version resources
			CVersionInfo pVersionInfo;
			if (pVersionInfo.Load(lpszFullPath))
			{
				// Store file description (e.g., "Task Manager")
				pProcessData->SetDescription(pVersionInfo.GetFileDescription().c_str());
				// Store company name (e.g., "Microsoft Corporation")
				pProcessData->SetCompany(pVersionInfo.GetCompanyName().c_str());
				// Store version string (e.g., "10.0.19041.1")
				pProcessData->SetVersion(pVersionInfo.GetFileVersionAsString().c_str());
			}
		}

		// === CLEANUP ===
		// Close the process handle to free resources
		VERIFY(CloseHandle(hProcess));
	}
	return pProcessData;
}

/**
 * @brief Updates CPU and memory usage for an existing process
 * 
 * @param dwProcessID The process identifier to update
 * 
 * @note This method is more efficient than Refresh() when updating a single process.
 * 
 * @note Returns nullptr if:
 * - Process not found in the snapshot (may have terminated)
 * - Process handle cannot be opened (insufficient privileges)
 * 
 * @return Pointer to the updated CProcessData object, or nullptr if process not found
 */
CProcessData* CSystemSnapshot::UpdateProcess(DWORD dwProcessID)
{
	HANDLE hProcess = nullptr;
	PROCESS_MEMORY_COUNTERS pmc = { 0 };
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS);

	// === FIND EXISTING PROCESS ===
	// Find the process in the current snapshot
	CProcessData* pProcessData = GetProcessID(dwProcessID);
	if (pProcessData != nullptr)
	{
		// === OPEN PROCESS FOR UPDATES ===
		// Open process handle to query updated information
		if ((hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, dwProcessID)) != nullptr)
		{
			// === UPDATE CPU USAGE ===
			// Query and update current CPU usage
			pProcessData->SetProcessorUsage(pProcessData->m_pCpuUsage.GetUsage());

			// === UPDATE MEMORY USAGE ===
			// Query and update current memory usage
			if (GetProcessMemoryInfo(hProcess, &pmc, pmc.cb))
			{
				pProcessData->SetMemoryUsage((DWORD)pmc.WorkingSetSize);
			}

			// === CLEANUP ===
			// Close the process handle
			VERIFY(CloseHandle(hProcess));
		}
		return pProcessData;
	}
	// Process not found in snapshot
	return nullptr;
}

/**
 * @brief Removes a process from the snapshot
 * 
 * @param dwProcessID The process identifier to remove
 * 
 * @note This should be called when a process terminates to keep the snapshot current.
 * Use this in conjunction with process monitoring to maintain an accurate list.
 * 
 * @warning The CProcessData object is deleted. Do not use any pointers to it after calling this.
 * 
 * @return true if process was found and removed, false otherwise
 */
bool CSystemSnapshot::RemoveProcess(DWORD dwProcessID)
{
	// Get the number of processes in the snapshot
	const int nSize = (int)m_arrProcessList.GetSize();

	// === SEARCH FOR PROCESS ===
	// Search for the process in the list by process ID
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_arrProcessList.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);

		// === REMOVE IF FOUND ===
		// If process found, delete it and remove from array
		if (pProcessData->GetProcessID() == dwProcessID)
		{
			// Delete the process data object
			delete pProcessData;
			pProcessData = nullptr;
			// Remove from the array
			m_arrProcessList.RemoveAt(nIndex);
			return true;
		}
	}
	// Process not found
	return false;
}

/**
 * @brief Retrieves process data by process ID
 * 
 * @param dwProcessID The process identifier to search for
 * 
 * @note This performs a linear search through the process list.
 * For large snapshots with frequent lookups, consider implementing
 * a map-based lookup structure for O(1) access instead of O(n).
 * 
 * @return Pointer to the CProcessData object if found, nullptr otherwise
 */
CProcessData* CSystemSnapshot::GetProcessID(DWORD dwProcessID)
{
	// Get the number of processes in the snapshot
	const int nSize = (int)m_arrProcessList.GetSize();

	// === SEARCH FOR PROCESS ===
	// Search for the process in the list by process ID
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_arrProcessList.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);

		// === RETURN IF FOUND ===
		// Return the process data if process ID matches
		if (pProcessData->GetProcessID() == dwProcessID)
		{
			return pProcessData;
		}
	}
	// Process not found
	return nullptr;
}
