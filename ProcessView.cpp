/* Copyright (C) 2012-2025 Stefan-Mihai MOGA
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

CProcessView::CProcessView()
{
	m_bInitialized = false;
	m_pMainFrame = nullptr;
	m_nRefreshTimerID = 0;

	GetListCtrl().m_pSystemSnapshot = &m_pSystemSnapshot;
}

CProcessView::~CProcessView()
{
}

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

void CProcessView::OnInitialUpdate()
{
	CMFCListView::OnInitialUpdate();

	if (!m_bInitialized)
	{
		m_bInitialized = true;

		GetListCtrl().SetExtendedStyle(GetListCtrl().GetExtendedStyle()
			| LVS_EX_DOUBLEBUFFER | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		const int nProcessID = theApp.GetInt(_T("ProcessID"), PID_COLUMN_LENGTH);
		const int nProcessName = theApp.GetInt(_T("ProcessName"), PROCESS_COLUMN_LENGTH);
		const int nCPU_Usage = theApp.GetInt(_T("CPU_Usage"), CPU_USAGE_COLUMN_LENGTH);
		const int nMEM_Usage = theApp.GetInt(_T("MEM_Usage"), MEM_USAGE_COLUMN_LENGTH);
		const int nCompany = theApp.GetInt(_T("Company"), COMPANY_COLUMN_LENGTH);
		const int nVersion = theApp.GetInt(_T("Version"), VERSION_COLUMN_LENGTH);

		// theApp.GetInt(_T("Description"), DESCRIPTION_COLUMN_SIZE)
		const int nDescription = rectClient.Width() - (nProcessID + nProcessName + nCPU_Usage + nMEM_Usage + nCompany + nVersion);

		GetListCtrl().InsertColumn(0, _T("PID"), LVCFMT_LEFT, nProcessID);
		GetListCtrl().InsertColumn(1, _T("Process"), LVCFMT_LEFT, nProcessName);
		GetListCtrl().InsertColumn(2, _T("CPU Usage"), LVCFMT_CENTER, nCPU_Usage);
		GetListCtrl().InsertColumn(3, _T("Mem Usage"), LVCFMT_RIGHT, nMEM_Usage);
		GetListCtrl().InsertColumn(4, _T("Description"), LVCFMT_LEFT, nDescription);
		GetListCtrl().InsertColumn(5, _T("Company"), LVCFMT_LEFT, nCompany);
		GetListCtrl().InsertColumn(6, _T("Version"), LVCFMT_LEFT, nVersion);

		VERIFY(Refresh());

		m_nRefreshTimerID = (UINT)SetTimer(1, 1000, nullptr);
	}
}

void CProcessView::OnDestroy()
{
	VERIFY(KillTimer(m_nRefreshTimerID));

	CMFCListView::OnDestroy();
}

void CProcessView::OnSize(UINT nType, int cx, int cy)
{
	CMFCListView::OnSize(nType, cx, cy);
	ResizeListCtrl();
}

void CProcessView::OnTimer(UINT_PTR nIDEvent)
{
	CString strListItem;
	HANDLE hSnapshot = nullptr;
	PROCESSENTRY32 pe32 = { 0 };
	CUIntArray arrProcessID;

	if (nIDEvent == m_nRefreshTimerID)
	{
		KillTimer(m_nRefreshTimerID);
		const int nOldListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
		GetListCtrl().SetRedraw(FALSE);
		if ((hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0)) != INVALID_HANDLE_VALUE)
		{
			DOUBLE nProcessorTotalUsage = 0.0;
			pe32.dwSize = sizeof(PROCESSENTRY32);
			if (Process32First(hSnapshot, &pe32))
			{
				do
				{
					arrProcessID.Add(pe32.th32ProcessID);
					CProcessData* pProcessData = m_pSystemSnapshot.UpdateProcess(pe32.th32ProcessID);
					if (pProcessData != nullptr)
					{
						const int nCount = GetListCtrl().GetItemCount();
						for (int nListItem = 0; nListItem < nCount; nListItem++)
						{
							const DWORD nProcessID = (DWORD)GetListCtrl().GetItemData(nListItem);
							if ((nProcessID != 0) && (nProcessID == pe32.th32ProcessID))
							{
								nProcessorTotalUsage += pProcessData->GetProcessorUsage();
								strListItem.Format(_T("%.2f%%"), pProcessData->GetProcessorUsage());
								GetListCtrl().SetItemText(nListItem, 2, strListItem);
								GetListCtrl().SetItemText(nListItem, 3, FormatSize(pProcessData->GetMemoryUsage()));
								break;
							}
						}
					}
					else // new process
					{
						pProcessData = m_pSystemSnapshot.InsertProcess(pe32);
						if (pProcessData != nullptr)
						{
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
			// TRACE(_T("nProcessorTotalUsage = %.2f%%\n"), nProcessorTotalUsage);
			const int nCount = GetListCtrl().GetItemCount();
			for (int nListItem = 0; nListItem < nCount; nListItem++)
			{
				const DWORD nProcessID = (DWORD)GetListCtrl().GetItemData(nListItem);
				if (nProcessID == 0)
				{
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
		for (int nOldIndex = 0; nOldIndex < GetListCtrl().GetItemCount(); nOldIndex++)
		{
			bool bFound = false;
			const DWORD nOldProcessID = (DWORD)GetListCtrl().GetItemData(nOldIndex);
			for (int nNewIndex = 0; nNewIndex < arrProcessID.GetCount(); nNewIndex++)
			{
				const DWORD nNewProcessID = arrProcessID.GetAt(nNewIndex);
				if (nOldProcessID == nNewProcessID)
				{
					bFound = true;
					break;
				}
			}
			if (!bFound)
			{
				GetListCtrl().DeleteItem(nOldIndex); // remove process
				VERIFY(m_pSystemSnapshot.RemoveProcess(nOldProcessID));
				nOldIndex--;
			}
		}
		arrProcessID.RemoveAll();
		const int nSortColumn = GetListCtrl().GetHeaderCtrl().GetSortColumn();
		const bool bIsAscending = GetListCtrl().GetHeaderCtrl().IsAscending();
		GetListCtrl().Sort(nSortColumn, bIsAscending, FALSE);
		GetListCtrl().SetItemState(nOldListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		GetListCtrl().SetRedraw(TRUE);
		GetListCtrl().UpdateWindow();
		ResizeListCtrl();
		m_nRefreshTimerID = (UINT)SetTimer(1, 1000, nullptr);
	}

	CMFCListView::OnTimer(nIDEvent);
}

void CProcessView::OnDblClickEntry(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (pResult != nullptr) *pResult = 0;
	if (pItemActivate->iItem != -1)
	{
		DoubleClickEntry(pItemActivate->iItem);
	}
}

void CProcessView::ResizeListCtrl()
{
	HDITEM hdItem = { 0 };
	if (GetListCtrl().GetSafeHwnd() != nullptr)
	{
		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		CMFCHeaderCtrl& pHeaderCtrl = GetListCtrl().GetHeaderCtrl();
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

									const int nDescription = rectClient.Width() - (nProcessID + nProcessName + nCPU_Usage + nMEM_Usage + nCompany + nVersion);
									if (pHeaderCtrl.GetItem(4, &hdItem))
									{
										hdItem.cxy = nDescription;
										if (pHeaderCtrl.SetItem(4, &hdItem))
										{
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

void CProcessView::DoubleClickEntry(int nIndex)
{
	ASSERT(GetListCtrl().m_hWnd != nullptr);
	CProcessData* pProcessData = m_pSystemSnapshot.GetProcessID((int)GetListCtrl().GetItemData(nIndex));
	ASSERT(pProcessData != nullptr);
	CString sPathName{ pProcessData->GetFilePath() };
	if (!sPathName.IsEmpty())
	{
		TRACE(sPathName);
		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(sei));
		sei.cbSize = sizeof(sei);
		sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
		sei.nShow = SW_SHOW;
		sei.lpFile = sPathName.GetBuffer(sPathName.GetLength());
		sei.lpVerb = _T("properties");
		sei.fMask = SEE_MASK_INVOKEIDLIST;
#pragma warning(suppress: 26486)
		ShellExecuteEx(&sei);
		sPathName.ReleaseBuffer();
	}
}

CString CProcessView::FormatSize(ULONGLONG nFormatSize)
{
	CString strFormatSize;
	ULONGLONG nFormatRest = 0;
	if (nFormatSize < 1024)
	{
		strFormatSize.Format(_T("%d"), (int)nFormatSize);
	}
	else
	{
		nFormatRest = nFormatSize % 1024;
		nFormatSize = nFormatSize / 1024;
		if (nFormatSize < 1024)
		{
			if (nFormatRest != 0) nFormatSize++;
			strFormatSize.Format(_T("%d KB"), (int)nFormatSize);
		}
		else
		{
			nFormatRest = nFormatSize % 1024;
			nFormatSize = nFormatSize / 1024;
			if (nFormatSize < 1024)
			{
				if (nFormatRest != 0) nFormatSize++;
				strFormatSize.Format(_T("%d MB"), (int)nFormatSize);
			}
			else
			{
				nFormatRest = nFormatSize % 1024;
				nFormatSize = nFormatSize / 1024;
				if (nFormatSize < 1024)
				{
					if (nFormatRest != 0) nFormatSize++;
					strFormatSize.Format(_T("%d GB"), (int)nFormatSize);
				}
				else
				{
					nFormatRest = nFormatSize % 1024;
					nFormatSize = nFormatSize / 1024;
					if (nFormatRest != 0) nFormatSize++;
					strFormatSize.Format(_T("%d TB"), (int)nFormatSize);
				}
			}
		}
	}
	return strFormatSize;
}

bool CProcessView::Refresh()
{
	CString strListItem;
	GetListCtrl().SetRedraw(FALSE);
	int nOldListItem = GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
	VERIFY(GetListCtrl().DeleteAllItems());
	bool bRetVal = m_pSystemSnapshot.Refresh();
	const int nSize = m_pSystemSnapshot.GetSize();
	for (int nIndex = 0; nIndex < nSize; nIndex++)
	{
		CProcessData* pProcessData = m_pSystemSnapshot.GetAt(nIndex);
		ASSERT(pProcessData != nullptr);
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
	// const int nSortColumn = GetListCtrl().GetHeaderCtrl().GetSortColumn();
	// const bool bIsAscending = GetListCtrl().GetHeaderCtrl().IsAscending();
	GetListCtrl().Sort(1, true, false);
	GetListCtrl().SetItemState(nOldListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	GetListCtrl().SetRedraw(true);
	GetListCtrl().UpdateWindow();
	ResizeListCtrl();
	return bRetVal;
}
