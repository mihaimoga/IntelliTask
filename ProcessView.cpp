/* This file is part of IntelliTask application developed by Stefan-Mihai MOGA.

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
			| LVS_EX_DOUBLEBUFFER |LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);

		GetListCtrl().InsertColumn(0, _T("PID"), LVCFMT_LEFT, PID_COLUMN_LENGTH);
		GetListCtrl().InsertColumn(1, _T("Process"), LVCFMT_LEFT, PROCESS_COLUMN_LENGTH);
		GetListCtrl().InsertColumn(2, _T("CPU Usage"), LVCFMT_CENTER, CPU_USAGE_COLUMN_LENGTH);
		GetListCtrl().InsertColumn(3, _T("Mem Usage"), LVCFMT_RIGHT, MEM_USAGE_COLUMN_LENGTH);
		GetListCtrl().InsertColumn(4, _T("Description"), LVCFMT_LEFT, 100); // nDescriptionColumnLength
		GetListCtrl().InsertColumn(5, _T("Company"), LVCFMT_LEFT, COMPANY_COLUMN_LENGTH);
		GetListCtrl().InsertColumn(6, _T("Version"), LVCFMT_LEFT, VERSION_COLUMN_LENGTH);

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
							const DWORD nProcessID = (DWORD) GetListCtrl().GetItemData(nListItem);
							if (nProcessID == pe32.th32ProcessID)
							{
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
		GetListCtrl().Sort(1, TRUE, FALSE);
		GetListCtrl().SetItemState(nOldListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
		GetListCtrl().SetRedraw(TRUE);
		GetListCtrl().UpdateWindow();
		ResizeListCtrl();
		m_nRefreshTimerID = (UINT)SetTimer(1, 1000, nullptr);
	}

	CMFCListView::OnTimer(nIDEvent);
}

void CProcessView::ResizeListCtrl()
{
	HDITEM hdItem = { 0 };
	hdItem.cxy = 0;
	hdItem.mask = HDI_WIDTH;
	if (GetListCtrl().GetSafeHwnd() != nullptr)
	{
		CRect rectClient;
		GetListCtrl().GetClientRect(&rectClient);

		const int nDescriptionColumnLength = rectClient.Width() - PID_COLUMN_LENGTH - PROCESS_COLUMN_LENGTH -
			CPU_USAGE_COLUMN_LENGTH - MEM_USAGE_COLUMN_LENGTH - COMPANY_COLUMN_LENGTH - VERSION_COLUMN_LENGTH;

		CMFCHeaderCtrl& pHeaderCtrl = GetListCtrl().GetHeaderCtrl();
		if (pHeaderCtrl.GetItem(4, &hdItem))
		{
			hdItem.cxy = nDescriptionColumnLength;
			if (pHeaderCtrl.SetItem(4, &hdItem))
			{
				GetListCtrl().Invalidate();
				GetListCtrl().UpdateWindow();
			}
		}
	}
}

void CProcessView::DoubleClickEntry(int nIndex)
{
	UNREFERENCED_PARAMETER(nIndex);
}

CString CProcessView::FormatSize(ULONGLONG nFormatSize)
{
	CString strFormatSize;
	ULONGLONG nFormatRest = 0;
	if (nFormatSize < 1024)
	{
		strFormatSize.Format(_T("%d"), (int) nFormatSize);
	}
	else
	{
		nFormatRest = nFormatSize % 1024;
		nFormatSize = nFormatSize / 1024;
		if (nFormatSize < 1024)
		{
			if (nFormatRest != 0) nFormatSize++;
			strFormatSize.Format(_T("%d KB"), (int) nFormatSize);
		}
		else
		{
			nFormatRest = nFormatSize % 1024;
			nFormatSize = nFormatSize / 1024;
			if (nFormatSize < 1024)
			{
				if (nFormatRest != 0) nFormatSize++;
				strFormatSize.Format(_T("%d MB"), (int) nFormatSize);
			}
			else
			{
				nFormatRest = nFormatSize % 1024;
				nFormatSize = nFormatSize / 1024;
				if (nFormatSize < 1024)
				{
					if (nFormatRest != 0) nFormatSize++;
					strFormatSize.Format(_T("%d GB"), (int) nFormatSize);
				}
				else
				{
					nFormatRest = nFormatSize % 1024;
					nFormatSize = nFormatSize / 1024;
					if (nFormatRest != 0) nFormatSize++;
					strFormatSize.Format(_T("%d TB"), (int) nFormatSize);
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
	GetListCtrl().Sort(1, TRUE, FALSE);
	GetListCtrl().SetItemState(nOldListItem, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	GetListCtrl().SetRedraw(TRUE);
	GetListCtrl().UpdateWindow();
	ResizeListCtrl();
	return bRetVal;
}
