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

// ProcessView.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliTask.h"
#include "ProcessView.h"
#include "MainFrame.h"

// CProcessView

IMPLEMENT_DYNCREATE(CProcessView, CMFCListView)

// Constructor: Initialize member variables
CProcessView::CProcessView()
{
	m_bInitialized = false;
	m_pMainFrame = nullptr;
	m_nRefreshTimerID = 0;

	// Associate the system snapshot with the list control
	GetListCtrl().m_pSystemSnapshot = &m_pSystemSnapshot;
}

// Destructor
CProcessView::~CProcessView()
{
}

// Message map: Connect Windows messages to handler functions
BEGIN_MESSAGE_MAP(CProcessView, CMFCListView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_NOTIFY(NM_DBLCLK, ID_MFCLISTCTRL, OnDblClickEntry)
END_MESSAGE_MAP()

// CProcessView diagnostics

#ifdef _DEBUG
void CProcessView::AssertValid() const
{
	CMFCListView::AssertValid();
}

#ifndef _WIN32_WCE
void CProcessView::Dump(CDumpContext& dc) const
{
	CMFCListView::Dump(dc);
}
#endif
#endif //_DEBUG

// CProcessView message handlers

// Initialize the view: Set up columns, styles, and start refresh timer
void CProcessView::OnInitialUpdate()
{
	CMFCListView::OnInitialUpdate();

	// Only initialize once
	if (!m_bInitialized)
	{
		m_bInitialized = true;

		// Enable visual enhancements: double buffering, full row selection, and gridlines
		GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle()
			| LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		// Restore column widths from application settings
		const int nProcessID = theApp.GetInt(_T("ProcessID"), PID_COLUMN_LENGTH);
		const int nProcessName = theApp.GetInt(_T("ProcessName"), PROCESS_COLUMN_LENGTH);
		const int nCPU_Usage = theApp.GetInt(_T("CPU_Usage"), CPU_USAGE_COLUMN_LENGTH);
		const int nMEM_Usage = theApp.GetInt(_T("MEM_Usage"), MEM_USAGE_COLUMN_LENGTH);
		const int nCompany = theApp.GetInt(_T("Company"), COMPANY_COLUMN_LENGTH);
		const int nVersion = theApp.GetInt(_T("Version"), VERSION_COLUMN_LENGTH);

		// Calculate description column width to fill remaining space
		const int nDescription = rectClient.Width() - (nProcessID + nProcessName + nCPU_Usage + nMEM_Usage + nCompany + nVersion);

		// Create list view columns
		GetListCtrl().InsertColumn(0, _T("PID"), LVCFMT_LEFT, nProcessID);
		GetListCtrl().InsertColumn(1, _T("Process"), LVCFMT_LEFT, nProcessName);
		GetListCtrl().InsertColumn(2, _T("CPU Usage"), LVCFMT_CENTER, nCPU_Usage);
		GetListCtrl().InsertColumn(3, _T("Mem Usage"), LVCFMT_RIGHT, nMEM_Usage);
		GetListCtrl().InsertColumn(4, _T("Description"), LVCFMT_LEFT, nDescription);
		GetListCtrl().InsertColumn(5, _T("Company"), LVCFMT_LEFT, nCompany);
		GetListCtrl().InsertColumn(6, _T("Version"), LVCFMT_LEFT, nVersion);

		// Populate the list with initial process data
		VERIFY(Refresh());

		// Start timer to refresh process list every second (1000ms)
		m_nRefreshTimerID = (UINT)SetTimer(1, 1000, nullptr);
	}
}

// Clean up when the view is destroyed
void CProcessView::OnDestroy()
{
	// Stop the refresh timer
	VERIFY(KillTimer(m_nRefreshTimerID));

	CMFCListView::OnDestroy();
}

// Handle window resizing
void CProcessView::OnSize(UINT nType, int cx, int cy)
{
	CMFCListView::OnSize(nType, cx, cy);
	// Adjust column widths to fit the new window size
	ResizeListCtrl();
}

// Timer handler: Refresh the process list periodically
void CProcessView::OnTimer(UINT_PTR nIDEvent)
{
	CString strListItem;
	HANDLE hSnapshot = nullptr;
	PROCESSENTRY32 pe32 = { 0 };
	CUIntArray arrProcessID;

	if (nIDEvent == m_nRefreshTimerID)
	{
		// Temporarily stop the timer during refresh to prevent overlap
		KillTimer(m_nRefreshTimerID);

		// Remember the currently selected item
		const int nOldListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);

		// Disable redrawing during update for better performance
		GetListCtrl().SetRedraw(FALSE);

		// Take a snapshot of all running processes
		if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) != INVALID_HANDLE_VALUE)
		{
			DOUBLE nProcessorTotalUsage = 0.0;
			pe32.dwSize = sizeof(PROCESSENTRY32);

			// Iterate through all processes in the snapshot
			if (Process32First(hSnapshot, &pe32))
			{
				do
				{
					// Track process IDs to identify terminated processes later
					arrProcessID.Add(pe32.th32ProcessID);

					// Check if this is an existing process
					CProcessData* pProcessData = m_pSystemSnapshot.UpdateProcess(pe32.th32ProcessID);
					if (pProcessData != nullptr)
					{
						// Update existing process in the list
						const int nCount = GetListCtrl().GetItemCount();
						for (int nListItem = 0; nListItem < nCount; nListItem++)
						{
							const DWORD nProcessID = (DWORD)GetListCtrl().GetItemData(nListItem);
							if ((nProcessID != 0) && (nProcessID == pe32.th32ProcessID))
							{
								// Update CPU and memory usage for this process
								nProcessorTotalUsage += pProcessData->GetProcessorUsage();
								strListItem.Format(_T("%.2f%%"), pProcessData->GetProcessorUsage());
								GetListCtrl().SetItemText(nListItem, 2, strListItem);
								GetListCtrl().SetItemText(nListItem, 3, FormatSize(pProcessData->GetMemoryUsage()));
								break;
							}
						}
					}
					else // New process detected
					{
						pProcessData = m_pSystemSnapshot.InsertProcess(pe32);
						if (pProcessData != nullptr)
						{
							// Add new process to the list
							strListItem.Format(_T("%d"), pProcessData->GetProcessID());
							const int nNewListItem = GetListCtrl().InsertItem(GetListCtrl().GetItemCount(), strListItem);
							GetListCtrl().SetItemText(nNewListItem, 1, pProcessData->GetFileName());
							nProcessorTotalUsage += pProcessData->GetProcessorUsage();
							strListItem.Format(_T("%.2f%%"), pProcessData->GetProcessorUsage());
							GetListCtrl().SetItemText(nNewListItem, 2, strListItem);
							GetListCtrl().SetItemText(nNewListItem, 3, FormatSize(pProcessData->GetMemoryUsage()));
							GetListCtrl().SetItemText(nNewListItem, 4, pProcessData->GetDescription());
							GetListCtrl().SetItemText(nNewListItem, 5, pProcessData->GetCompany());
							GetListCtrl().SetItemText(nNewListItem, 6, pProcessData->GetVersion());
							GetListCtrl().SetItemData(nNewListItem, pProcessData->GetProcessID());
						}
					}
				} while (Process32Next(hSnapshot, &pe32));
			}
			VERIFY(CloseHandle(hSnapshot));

			// Update the System Idle Process (PID 0) with calculated idle time
			const int nCount = GetListCtrl().GetItemCount();
			for (int nListItem = 0; nListItem < nCount; nListItem++)
			{
				const DWORD nProcessID = (DWORD)GetListCtrl().GetItemData(nListItem);
				if (nProcessID == 0)
				{
					// Idle CPU = 100% - total CPU usage
					DOUBLE nIdleUsage = 100.0 - nProcessorTotalUsage;
					strListItem.Format(_T("%.2f%%"), nIdleUsage);
					GetListCtrl().SetItemText(nListItem, 2, strListItem);
					CProcessData* pProcessData = m_pSystemSnapshot.GetProcessID(0);
					if (pProcessData != nullptr)
					{
						pProcessData->SetProcessorUsage(nIdleUsage);
					}
				}
			}
		}

		// Remove processes that have terminated
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
				GetListCtrl().DeleteItem(nOldIndex);
				VERIFY(m_pSystemSnapshot.RemoveProcess(nOldProcessID));
				nOldIndex--; // Adjust index after deletion
			}
		}
		arrProcessID.RemoveAll();

		// Re-sort the list according to current sort settings
		const int nSortColumn = GetListCtrl().GetHeaderCtrl().GetSortColumn();
		const bool bIsAscending = GetListCtrl().GetHeaderCtrl().IsAscending();
		GetListCtrl().Sort(nSortColumn, bIsAscending, FALSE);

		// Restore the previously selected item
		GetListCtrl().SetItemState(nOldListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

		// Re-enable drawing and update the display
		GetListCtrl().SetRedraw(TRUE);
		GetListCtrl().UpdateWindow();
		ResizeListCtrl();

		// Restart the timer for the next refresh cycle
		m_nRefreshTimerID = (UINT)SetTimer(1, 1000, nullptr);
	}

	CMFCListView::OnTimer(nIDEvent);
}

// Handle double-click on a list item
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

// Resize list control columns to fit the window and save column widths
void CProcessView::ResizeListCtrl()
{
	HDITEM hdItem = { 0 };
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

// Show process properties dialog when double-clicking a process
void CProcessView::DoubleClickEntry(int nIndex)
{
	ASSERT(GetListCtrl().m_hWnd != nullptr);

	// Get the process data for the selected item
	CProcessData* pProcessData = m_pSystemSnapshot.GetProcessID((int)GetListCtrl().GetItemData(nIndex));
	ASSERT(pProcessData != nullptr);

	// Get the executable file path
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

// Format memory size in human-readable format (bytes, KB, MB, GB, TB)
CString CProcessView::FormatSize(ULONGLONG nFormatSize)
{
	CString strFormatSize;
	ULONGLONG nFormatRest = 0;

	// Less than 1 KB
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
			if (nFormatRest != 0) nFormatSize++; // Round up
			strFormatSize.Format(_T("%d KB"), (int)nFormatSize);
		}
		else
		{
			// Convert to MB
			nFormatRest = nFormatSize % 1024;
			nFormatSize = nFormatSize / 1024;
			if (nFormatSize < 1024)
			{
				if (nFormatRest != 0) nFormatSize++; // Round up
				strFormatSize.Format(_T("%d MB"), (int)nFormatSize);
			}
			else
			{
				// Convert to GB
				nFormatRest = nFormatSize % 1024;
				nFormatSize = nFormatSize / 1024;
				if (nFormatSize < 1024)
				{
					if (nFormatRest != 0) nFormatSize++; // Round up
					strFormatSize.Format(_T("%d GB"), (int)nFormatSize);
				}
				else
				{
					// Convert to TB
					nFormatRest = nFormatSize % 1024;
					nFormatSize = nFormatSize / 1024;
					if (nFormatRest != 0) nFormatSize++; // Round up
					strFormatSize.Format(_T("%d TB"), (int)nFormatSize);
				}
			}
		}
	}
	return strFormatSize;
}

// Refresh the entire process list from scratch
bool CProcessView::Refresh()
{
	CString strListItem;

	// Disable redrawing for better performance
	GetListCtrl().SetRedraw(FALSE);

	// Remember the currently selected item
	int nOldListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);

	// Clear all existing items
	VERIFY(GetListCtrl().DeleteAllItems());

	// Refresh the system snapshot with current process data
	bool bRetVal = m_pSystemSnapshot.Refresh();
	const int nSize = m_pSystemSnapshot.GetSize();

	// Populate the list with all processes
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_pSystemSnapshot.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);

		// Add process to the list with all its information
		strListItem.Format(_T("%d"), pProcessData->GetProcessID());
		const int nNewListItem = GetListCtrl().InsertItem(GetListCtrl().GetItemCount(), strListItem);
		GetListCtrl().SetItemText(nNewListItem, 1, pProcessData->GetFileName());
		strListItem.Format(_T("%.2f%%"), pProcessData->GetProcessorUsage());
		GetListCtrl().SetItemText(nNewListItem, 2, strListItem);
		GetListCtrl().SetItemText(nNewListItem, 3, FormatSize(pProcessData->GetMemoryUsage()));
		GetListCtrl().SetItemText(nNewListItem, 4, pProcessData->GetDescription());
		GetListCtrl().SetItemText(nNewListItem, 5, pProcessData->GetCompany());
		GetListCtrl().SetItemText(nNewListItem, 6, pProcessData->GetVersion());
		GetListCtrl().SetItemData(nNewListItem, pProcessData->GetProcessID());
	}

	// Sort by process name (column 1) in ascending order
	GetListCtrl().Sort(1, true, false);

	// Restore the previously selected item
	GetListCtrl().SetItemState(nOldListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

	// Re-enable drawing and update the display
	GetListCtrl().SetRedraw(true);
	GetListCtrl().UpdateWindow();
	ResizeListCtrl();

	return bRetVal;
}
