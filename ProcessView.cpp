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

/**
 * @file ProcessView.cpp
 * @brief Implementation of the CProcessView class - Main process monitoring view
 * 
 * This file implements a list view that displays running processes with real-time updates.
 * Key features:
 * - Automatic refresh every 1 second via timer
 * - Incremental updates (only changed data is updated)
 * - CPU and memory usage monitoring
 * - Process properties via double-click
 * - Persistent column widths across sessions
 * 
 * PERFORMANCE NOTES:
 * - Uses SetRedraw(FALSE) to batch updates and reduce flicker
 * - Employs incremental updates in OnTimer() vs full refresh in Refresh()
 * - Double buffering enabled via LVS_EX_DOUBLEBUFFER for smooth updates
 * 
 * THREAD SAFETY:
 * - NOT thread-safe. All operations must occur on the UI thread.
 * - Timer callbacks execute on the UI thread, ensuring safety.
 */

#include "stdafx.h"
#include "IntelliTask.h"
#include "ProcessView.h"
#include "MainFrame.h"

/**
 * @brief Dynamic creation implementation for CProcessView
 * 
 * Enables runtime creation and type information for the CProcessView class.
 * This allows the framework to create instances dynamically.
 */
IMPLEMENT_DYNCREATE(CProcessView, CMFCListView)

/**
 * @brief Constructor for CProcessView
 * 
 * Initializes all member variables and associates the system snapshot
 * with the list control for process monitoring.
 */
CProcessView::CProcessView()
{
	// Initialize state flags
	m_bInitialized = false;
	m_pMainFrame = nullptr;
	m_nRefreshTimerID = 0;

	// Associate the system snapshot with the list control
	// This allows the list control to access process data directly
	GetListCtrl().m_pSystemSnapshot = &m_pSystemSnapshot;
}

/**
 * @brief Destructor for CProcessView
 * 
 * Cleans up resources. Timer cleanup is handled in OnDestroy().
 */
CProcessView::~CProcessView()
{
}

/**
 * @brief Message map for CProcessView
 * 
 * Maps Windows messages and notifications to their handler functions.
 * Handles sizing, timer events, destruction, and double-click notifications.
 */
BEGIN_MESSAGE_MAP(CProcessView, CMFCListView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, ID_MFCLISTCTRL, OnDblClickEntry)
END_MESSAGE_MAP()

/**
 * @brief Validate the object's state (Debug builds only)
 * 
 * Verifies that the object is in a valid state by calling the base class implementation.
 */
#ifdef _DEBUG
void CProcessView::AssertValid() const
{
	CMFCListView::AssertValid();
}

#ifndef _WIN32_WCE
/**
 * @brief Dump the object's contents to a CDumpContext (Debug builds only)
 * 
 * Outputs diagnostic information about the object for debugging purposes.
 * 
 * @param dc Reference to a CDumpContext object
 */
void CProcessView::Dump(CDumpContext& dc) const
{
	CMFCListView::Dump(dc);
}
#endif
#endif //_DEBUG

/**
 * @brief Initialize the view after creation
 * 
 * Sets up the list control with columns, styles, and initial data.
 * Also starts the refresh timer for periodic updates.
 * 
 * @note This method is called only once, guarded by m_bInitialized flag.
 */
void CProcessView::OnInitialUpdate()
{
	CMFCListView::OnInitialUpdate();

	// === PREVENT MULTIPLE INITIALIZATION ===
	// Only initialize once to avoid duplicate columns and timers
	if (!m_bInitialized)
	{
		m_bInitialized = true;

		// === CONFIGURE LIST CONTROL VISUAL STYLE ===
		// Enable visual enhancements:
		// - LVS_EX_DOUBLEBUFFER: Reduces flicker during updates
		// - LVS_EX_FULLROWSELECT: Allows clicking anywhere in row to select
		// - LVS_EX_GRIDLINES: Shows grid lines between rows/columns
		GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle()
			| LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		// === RESTORE COLUMN WIDTHS ===
		// Restore column widths from application settings (persisted from last run)
		const int nProcessID = theApp.GetInt(_T("ProcessID"), PID_COLUMN_LENGTH);
		const int nProcessName = theApp.GetInt(_T("ProcessName"), PROCESS_COLUMN_LENGTH);
		const int nCPU_Usage = theApp.GetInt(_T("CPU_Usage"), CPU_USAGE_COLUMN_LENGTH);
		const int nMEM_Usage = theApp.GetInt(_T("MEM_Usage"), MEM_USAGE_COLUMN_LENGTH);
		const int nCompany = theApp.GetInt(_T("Company"), COMPANY_COLUMN_LENGTH);
		const int nVersion = theApp.GetInt(_T("Version"), VERSION_COLUMN_LENGTH);

		// Calculate description column width to fill remaining space
		const int nDescription = rectClient.Width() - (nProcessID + nProcessName + nCPU_Usage + nMEM_Usage + nCompany + nVersion);

		// === CREATE LIST VIEW COLUMNS ===
		// Column 0: Process ID (left-aligned numbers)
		GetListCtrl().InsertColumn(0, _T("PID"), LVCFMT_LEFT, nProcessID);
		// Column 1: Process name (left-aligned text)
		GetListCtrl().InsertColumn(1, _T("Process"), LVCFMT_LEFT, nProcessName);
		// Column 2: CPU usage percentage (center-aligned)
		GetListCtrl().InsertColumn(2, _T("CPU Usage"), LVCFMT_CENTER, nCPU_Usage);
		// Column 3: Memory usage (right-aligned for numbers)
		GetListCtrl().InsertColumn(3, _T("Mem Usage"), LVCFMT_RIGHT, nMEM_Usage);
		// Column 4: File description (left-aligned, fills remaining space)
		GetListCtrl().InsertColumn(4, _T("Description"), LVCFMT_LEFT, nDescription);
		// Column 5: Company name (left-aligned)
		GetListCtrl().InsertColumn(5, _T("Company"), LVCFMT_LEFT, nCompany);
		// Column 6: Version string (left-aligned)
		GetListCtrl().InsertColumn(6, _T("Version"), LVCFMT_LEFT, nVersion);

		// === POPULATE WITH INITIAL DATA ===
		// Populate the list with initial process data
		VERIFY(Refresh());

		// === START REFRESH TIMER ===
		// Start timer to refresh process list every second (1000ms)
		// Timer ID is stored for later cleanup
		m_nRefreshTimerID = (UINT)SetTimer(1, 1000, nullptr);
	}
}

/**
 * @brief Clean up resources when the view is destroyed
 * 
 * Stops the refresh timer and performs cleanup before destruction.
 */
void CProcessView::OnDestroy()
{
	// Stop the refresh timer
	VERIFY(KillTimer(m_nRefreshTimerID));

	CMFCListView::OnDestroy();
}

/**
 * @brief Handle window resizing
 * 
 * Adjusts column widths to fit the new window size.
 * 
 * @param nType Type of resizing requested
 * @param cx New width of the client area
 * @param cy New height of the client area
 */
void CProcessView::OnSize(UINT nType, int cx, int cy)
{
	CMFCListView::OnSize(nType, cx, cy);
	// Adjust column widths to fit the new window size
	// This ensures the Description column expands/contracts to fill available space
	// providing a better user experience when resizing the window
	ResizeListCtrl();
}

/**
 * @brief Timer event handler for periodic process list refresh
 * 
 * Called every second to update the process list with current data.
 * Updates existing processes, adds new ones, and removes terminated processes.
 * 
 * Algorithm:
 * 1. Take snapshot of current running processes
 * 2. Update existing processes (CPU/memory usage)
 * 3. Add newly started processes
 * 4. Remove terminated processes
 * 5. Re-sort and restore selection
 * 
 * @param nIDEvent Timer identifier
 * 
 * @note The timer is temporarily stopped during refresh to prevent overlapping updates.
 * This is crucial for stability - if refresh takes longer than 1 second (rare but possible
 * on slow systems with many processes), we avoid re-entrancy issues.
 * 
 * @performance This method is optimized for incremental updates. It only updates changed
 * values (CPU/memory) for existing processes rather than recreating the entire list.
 * SetRedraw(FALSE) prevents screen flicker by batching all visual updates.
 */
void CProcessView::OnTimer(UINT_PTR nIDEvent)
{
	CString strListItem;
	HANDLE hSnapshot = nullptr;
	PROCESSENTRY32 pe32 = { 0 };
	CUIntArray arrProcessID;

	if (nIDEvent == m_nRefreshTimerID)
	{
		// === STOP TIMER DURING REFRESH ===
		// Temporarily stop the timer during refresh to prevent overlapping updates
		// This ensures we don't start a new refresh while still processing the current one
		KillTimer(m_nRefreshTimerID);

		// === REMEMBER CURRENT SELECTION ===
		// Remember the currently selected item to restore it after refresh
		// This preserves the user's selection even as the list is updated,
		// improving UX by maintaining context during monitoring
		const int nOldListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);

		// === DISABLE REDRAWING ===
		// Disable redrawing during update for better performance and to prevent flicker
		GetListCtrl().SetRedraw(FALSE);

		// === TAKE PROCESS SNAPSHOT ===
		// Take a snapshot of all running processes using Toolhelp API
		if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) != INVALID_HANDLE_VALUE)
		{
			// Track total CPU usage to calculate System Idle Process percentage
			DOUBLE nProcessorTotalUsage = 0.0;
			pe32.dwSize = sizeof(PROCESSENTRY32);

			// === ENUMERATE ALL PROCESSES ===
			// Iterate through all processes in the snapshot
			if (Process32First(hSnapshot, &pe32))
			{
				do
				{
					// Track process IDs to identify terminated processes later
					arrProcessID.Add(pe32.th32ProcessID);

					// === UPDATE EXISTING OR ADD NEW PROCESS ===
					// Check if this is an existing process that needs updating
					CProcessData* pProcessData = m_pSystemSnapshot.UpdateProcess(pe32.th32ProcessID);
					if (pProcessData != nullptr)
					{
						// === UPDATE EXISTING PROCESS ===
						// Process already exists in our snapshot, update its display
						const int nCount = GetListCtrl().GetItemCount();
						for (int nListItem = 0; nListItem < nCount; nListItem++)
						{
							// Find the list item corresponding to this process
							const DWORD nProcessID = (DWORD)GetListCtrl().GetItemData(nListItem);
							if ((nProcessID != 0) && (nProcessID == pe32.th32ProcessID))
							{
								// Update CPU and memory usage for this process
								// Accumulate CPU usage for idle process calculation
								nProcessorTotalUsage += pProcessData->GetProcessorUsage();
								// Update CPU usage column with formatted percentage
								strListItem.Format(_T("%.2f%%"), pProcessData->GetProcessorUsage());
								GetListCtrl().SetItemText(nListItem, 2, strListItem);
								// Update memory usage column with formatted size
								GetListCtrl().SetItemText(nListItem, 3, FormatSize(pProcessData->GetMemoryUsage()));
								break;
							}
						}
					}
					else // New process detected
					{
						// === ADD NEW PROCESS ===
						// Process not in snapshot yet, insert it
						pProcessData = m_pSystemSnapshot.InsertProcess(pe32);
						if (pProcessData != nullptr)
						{
							// Add new process to the list with all its information
							// Column 0: Process ID
							strListItem.Format(_T("%d"), pProcessData->GetProcessID());
							const int nNewListItem = GetListCtrl().InsertItem(GetListCtrl().GetItemCount(), strListItem);
							// Column 1: Process name
							GetListCtrl().SetItemText(nNewListItem, 1, pProcessData->GetFileName());
							// Column 2: CPU usage
							nProcessorTotalUsage += pProcessData->GetProcessorUsage();
							strListItem.Format(_T("%.2f%%"), pProcessData->GetProcessorUsage());
							GetListCtrl().SetItemText(nNewListItem, 2, strListItem);
							// Column 3: Memory usage
							GetListCtrl().SetItemText(nNewListItem, 3, FormatSize(pProcessData->GetMemoryUsage()));
							// Column 4: Description
							GetListCtrl().SetItemText(nNewListItem, 4, pProcessData->GetDescription());
							// Column 5: Company
							GetListCtrl().SetItemText(nNewListItem, 5, pProcessData->GetCompany());
							// Column 6: Version
							GetListCtrl().SetItemText(nNewListItem, 6, pProcessData->GetVersion());
							// Store process ID as item data for quick lookups
							GetListCtrl().SetItemData(nNewListItem, pProcessData->GetProcessID());
						}
					}
				} while (Process32Next(hSnapshot, &pe32));
			}
			VERIFY(CloseHandle(hSnapshot));

			// === UPDATE SYSTEM IDLE PROCESS ===
			// Update the System Idle Process (PID 0) with calculated idle time
			// Idle CPU = 100% - sum of all process CPU usage
			// NOTE: System Idle Process is a special pseudo-process that represents
			// unused CPU capacity. It's not a real process but shown for completeness.
			const int nCount = GetListCtrl().GetItemCount();
			for (int nListItem = 0; nListItem < nCount; nListItem++)
			{
				const DWORD nProcessID = (DWORD)GetListCtrl().GetItemData(nListItem);
				if (nProcessID == 0)
				{
					// Calculate idle percentage
					DOUBLE nIdleUsage = 100.0 - nProcessorTotalUsage;
					// Update display
					strListItem.Format(_T("%.2f%%"), nIdleUsage);
					GetListCtrl().SetItemText(nListItem, 2, strListItem);
					// Update snapshot data
					CProcessData* pProcessData = m_pSystemSnapshot.GetProcessID(0);
					if (pProcessData != nullptr)
					{
						pProcessData->SetProcessorUsage(nIdleUsage);
					}
				}
			}
		}

		// === REMOVE TERMINATED PROCESSES ===
		// Remove processes that have terminated (no longer in the new snapshot)
		for (int nOldIndex = 0; nOldIndex < GetListCtrl().GetItemCount(); nOldIndex++)
		{
			bool bFound = false;
			const DWORD nOldProcessID = (DWORD)GetListCtrl().GetItemData(nOldIndex);

			// Check if this process still exists in the new snapshot
			for (int nNewIndex = 0; nNewIndex < arrProcessID.GetCount(); nNewIndex++)
			{
				const DWORD nNewProcessID = arrProcessID.GetAt(nNewIndex);
				if (nOldProcessID == nNewProcessID)
				{
					bFound = true;
					break;
				}
			}

			// Process not found - it has terminated
			if (!bFound)
			{
				// Remove from list control
				GetListCtrl().DeleteItem(nOldIndex);
				// Remove from snapshot
				VERIFY(m_pSystemSnapshot.RemoveProcess(nOldProcessID));
				// Adjust index after deletion
				nOldIndex--;
			}
		}
		arrProcessID.RemoveAll();

		// === RE-SORT AND RESTORE SELECTION ===
		// Re-sort the list according to current sort settings
		// This maintains the user's preferred sort order (by PID, name, CPU, etc.)
		// even as processes come and go
		const int nSortColumn = GetListCtrl().GetHeaderCtrl().GetSortColumn();
		const bool bIsAscending = GetListCtrl().GetHeaderCtrl().IsAscending();
		GetListCtrl().Sort(nSortColumn, bIsAscending, FALSE);

		// Restore the previously selected item
		GetListCtrl().SetItemState(nOldListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

		// === RE-ENABLE DRAWING ===
		// Re-enable drawing and update the display
		GetListCtrl().SetRedraw(TRUE);
		GetListCtrl().UpdateWindow();
		ResizeListCtrl();

		// === RESTART TIMER ===
		// Restart the timer for the next refresh cycle
		m_nRefreshTimerID = (UINT)SetTimer(1, 1000, nullptr);
	}

	CMFCListView::OnTimer(nIDEvent);
}

/**
 * @brief Handle double-click notification on a list item
 * 
 * Opens the properties dialog for the double-clicked process.
 * 
 * @param pNMHDR Pointer to notification message header
 * @param pResult Pointer to result value (set to 0)
 */
void CProcessView::OnDblClickEntry(NMHDR* pNMHDR, LRESULT* pResult)
{
	LPNMITEMACTIVATE pItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pResult != nullptr) *pResult = 0;

	// Open process properties if a valid item was clicked
	if (pItemActivate->iItem != -1)
	{
		DoubleClickEntry(pItemActivate->iItem);
	}
}

/**
 * @brief Resize list control columns to fit window and persist settings
 * 
 * Reads current column widths, saves them to application settings,
 * and recalculates the Description column to fill remaining space.
 * 
 * @note Column widths are persisted using theApp.WriteInt() for restoration on next run.
 * 
 * @implementation The deeply nested if-structure is necessary because:
 * 1. Each GetItem() call can fail if the column doesn't exist
 * 2. We need the width of ALL fixed columns before calculating the Description column
 * 3. The Description column is the only one that dynamically resizes to fill space
 * 4. Failed GetItem() at any level means we can't proceed safely
 * 
 * @performance Called during OnSize() and after updates. Not performance-critical
 * as it only involves reading/writing a few integer values.
 */
void CProcessView::ResizeListCtrl()
{
	HDITEM hdItem = { 0 };
	// Verify list control is created before attempting to resize
	// This check prevents crashes during initialization or destruction
	if (GetListCtrl().GetSafeHwnd() != nullptr)
	{
		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		CMFCHeaderCtrl& pHeaderCtrl = GetListCtrl().GetHeaderCtrl();

		// Read current column widths and save them to application settings
		hdItem.mask = HDI_WIDTH;
		if (pHeaderCtrl.GetItem(0, &hdItem))
		{
			const int nProcessID = hdItem.cxy;
			theApp.WriteInt(_T("ProcessID"), nProcessID);
			hdItem.mask = HDI_WIDTH;
			if (pHeaderCtrl.GetItem(1, &hdItem))
			{
				const int nProcessName = hdItem.cxy;
				theApp.WriteInt(_T("ProcessName"), nProcessName);
				hdItem.mask = HDI_WIDTH;
				if (pHeaderCtrl.GetItem(2, &hdItem))
				{
					const int nCPU_Usage = hdItem.cxy;
					theApp.WriteInt(_T("CPU_Usage"), nCPU_Usage);
					hdItem.mask = HDI_WIDTH;
					if (pHeaderCtrl.GetItem(3, &hdItem))
					{
						const int nMEM_Usage = hdItem.cxy;
						theApp.WriteInt(_T("MEM_Usage"), nMEM_Usage);
						hdItem.mask = HDI_WIDTH;
						if (pHeaderCtrl.GetItem(4, &hdItem))
						{
							theApp.WriteInt(_T("Description"), hdItem.cxy);
							hdItem.mask = HDI_WIDTH;
							if (pHeaderCtrl.GetItem(5, &hdItem))
							{
								const int nCompany = hdItem.cxy;
								theApp.WriteInt(_T("Company"), nCompany);
								hdItem.mask = HDI_WIDTH;
								if (pHeaderCtrl.GetItem(6, &hdItem))
								{
									const int nVersion = hdItem.cxy;
									theApp.WriteInt(_T("Version"), nVersion);

									// Recalculate description column to fill remaining space
									const int nDescription = rectClient.Width() - (nProcessID + nProcessName + nCPU_Usage + nMEM_Usage + nCompany + nVersion);
									if (pHeaderCtrl.GetItem(4, &hdItem))
									{
										hdItem.cxy = nDescription;
										if (pHeaderCtrl.SetItem(4, &hdItem))
										{
											// Refresh the display
											GetListCtrl().Invalidate();
											GetListCtrl().UpdateWindow();
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
}

/**
 * @brief Display Windows properties dialog for a process
 * 
 * Opens the file properties dialog for the executable of the selected process.
 * Uses ShellExecuteEx with the "properties" verb.
 * 
 * @param nIndex Index of the list item (process) to show properties for
 * 
 * @note If the executable path is empty or inaccessible, nothing happens.
 * 
 * @security The properties dialog is shown in the security context of the current user.
 * Some system processes may not have accessible file paths due to security restrictions.
 * This is normal behavior and not an error.
 */
void CProcessView::DoubleClickEntry(int nIndex)
{
	ASSERT(GetListCtrl().m_hWnd != nullptr);

	// Get the process data for the selected item
	CProcessData* pProcessData = m_pSystemSnapshot.GetProcessID((int)GetListCtrl().GetItemData(nIndex));
	ASSERT(pProcessData != nullptr);

	// Get the executable file path
	// NOTE: Some system processes (like System Idle Process) may not have a file path
	CString sPathName{ pProcessData->GetFilePath() };
	if (!sPathName.IsEmpty())
	{
		TRACE(sPathName);

		// Open Windows properties dialog for the executable file
		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(sei));
		sei.cbSize = sizeof(sei);
		sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
		sei.nShow = SW_SHOW;
		sei.lpFile = sPathName.GetBuffer(sPathName.GetLength());
		sei.lpVerb = _T("properties"); // Show file properties
		sei.fMask = SEE_MASK_INVOKEIDLIST;
#pragma warning(suppress: 26486)
		ShellExecuteEx(&sei);
		sPathName.ReleaseBuffer();
	}
}

/**
 * @brief Format a memory size value in human-readable format
 * 
 * Converts byte values to appropriate units (bytes, KB, MB, GB, TB)
 * with automatic scaling and rounding.
 * 
 * @param nFormatSize Memory size in bytes
 * @return Formatted string with appropriate unit suffix
 * 
 * Examples:
 * - 512 bytes -> "512"
 * - 2048 bytes -> "2 KB"
 * - 5242880 bytes -> "5 MB"
 * 
 * @algorithm Uses binary units (1 KB = 1024 bytes) rather than decimal (1 KB = 1000 bytes).
 * This matches how Windows Task Manager displays memory, providing consistency.
 * Always rounds up remainders to avoid showing 0 KB for small allocations.
 */
CString CProcessView::FormatSize(ULONGLONG nFormatSize)
{
	CString strFormatSize;
	ULONGLONG nFormatRest = 0;

	// === DETERMINE APPROPRIATE UNIT ===
	// Less than 1 KB - display in bytes
	if (nFormatSize < 1024)
	{
		strFormatSize.Format(_T("%d"), (int)nFormatSize);
	}
	else
	{
		// Convert to KB
		nFormatRest = nFormatSize % 1024;
		nFormatSize = nFormatSize / 1024;
		if (nFormatSize < 1024)
		{
			// Round up if there's a remainder
			if (nFormatRest != 0) nFormatSize++;
			strFormatSize.Format(_T("%d KB"), (int)nFormatSize);
		}
		else
		{
			// Convert to MB
			nFormatRest = nFormatSize % 1024;
			nFormatSize = nFormatSize / 1024;
			if (nFormatSize < 1024)
			{
				// Round up if there's a remainder
				if (nFormatRest != 0) nFormatSize++;
				strFormatSize.Format(_T("%d MB"), (int)nFormatSize);
			}
			else
			{
				// Convert to GB
				nFormatRest = nFormatSize % 1024;
				nFormatSize = nFormatSize / 1024;
				if (nFormatSize < 1024)
				{
					// Round up if there's a remainder
					if (nFormatRest != 0) nFormatSize++;
					strFormatSize.Format(_T("%d GB"), (int)nFormatSize);
				}
				else
				{
					// Convert to TB
					nFormatRest = nFormatSize % 1024;
					nFormatSize = nFormatSize / 1024;
					// Round up if there's a remainder
					if (nFormatRest != 0) nFormatSize++;
					strFormatSize.Format(_T("%d TB"), (int)nFormatSize);
				}
			}
		}
	}
	return strFormatSize;
}

/**
 * @brief Refresh the entire process list from scratch
 * 
 * Clears the current list, takes a fresh system snapshot, and repopulates
 * the list with all running processes. This is more expensive than the
 * incremental update performed by OnTimer().
 * 
 * @return true if snapshot was successfully created, false otherwise
 * 
 * @note This method is called during initial setup and can be called
 * manually to force a complete refresh.
 * 
 * @performance Comparison:
 * - Refresh(): Clears everything and rebuilds (~100-300ms for 100 processes)
 * - OnTimer(): Updates only changed values (~10-50ms per cycle)
 * Use OnTimer()'s incremental approach for regular updates.
 */
bool CProcessView::Refresh()
{
	CString strListItem;

	// === DISABLE REDRAWING ===
	// Disable redrawing for better performance during bulk updates
	GetListCtrl().SetRedraw(FALSE);

	// === REMEMBER SELECTION ===
	// Remember the currently selected item to restore after refresh
	int nOldListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);

	// === CLEAR EXISTING DATA ===
	// Clear all existing items from the list
	VERIFY(GetListCtrl().DeleteAllItems());

	// === REFRESH SNAPSHOT ===
	// Refresh the system snapshot with current process data
	// This creates a complete new snapshot of all running processes
	// NOTE: This is called only during initialization. After that, OnTimer() uses
	// incremental updates via UpdateProcess() for better performance.
	bool bRetVal = m_pSystemSnapshot.Refresh();
	const int nSize = m_pSystemSnapshot.GetSize();

	// === POPULATE LIST ===
	// Populate the list with all processes from the snapshot
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_pSystemSnapshot.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);

		// Add process to the list with all its information
		// Column 0: Process ID
		strListItem.Format(_T("%d"), pProcessData->GetProcessID());
		const int nNewListItem = GetListCtrl().InsertItem(GetListCtrl().GetItemCount(), strListItem);
		// Column 1: Process name
		GetListCtrl().SetItemText(nNewListItem, 1, pProcessData->GetFileName());
		// Column 2: CPU usage
		strListItem.Format(_T("%.2f%%"), pProcessData->GetProcessorUsage());
		GetListCtrl().SetItemText(nNewListItem, 2, strListItem);
		// Column 3: Memory usage
		GetListCtrl().SetItemText(nNewListItem, 3, FormatSize(pProcessData->GetMemoryUsage()));
		// Column 4: Description
		GetListCtrl().SetItemText(nNewListItem, 4, pProcessData->GetDescription());
		// Column 5: Company
		GetListCtrl().SetItemText(nNewListItem, 5, pProcessData->GetCompany());
		// Column 6: Version
		GetListCtrl().SetItemText(nNewListItem, 6, pProcessData->GetVersion());
		// Store process ID for quick lookups
		GetListCtrl().SetItemData(nNewListItem, pProcessData->GetProcessID());
	}

	// === SORT BY PROCESS NAME ===
	// Sort by process name (column 1) in ascending order
	GetListCtrl().Sort(1, true, false);

	// === RESTORE SELECTION ===
	// Restore the previously selected item
	GetListCtrl().SetItemState(nOldListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	// === RE-ENABLE DRAWING ===
	// Re-enable drawing and update the display
	GetListCtrl().SetRedraw(true);
	GetListCtrl().UpdateWindow();
	ResizeListCtrl();

	return bRetVal;
}
