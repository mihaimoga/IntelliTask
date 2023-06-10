/* This file is part of IntelliTask application developed by Stefan-Mihai MOGA.

IntelliTask is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliTask is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliTask. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

#pragma once

#include "MFCListView.h"
#include "ProcessList.h"

class CMainFrame;

// CProcessView view

class CProcessView : public CMFCListView
{
	DECLARE_DYNCREATE(CProcessView)

public:
	CProcessView();           // protected constructor used by dynamic creation
	virtual ~CProcessView();

public:
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDblClickEntry(NMHDR *pNMHDR, LRESULT *pResult);

public:
	void ResizeListCtrl();
	void DoubleClickEntry(int nIndex);
	CString FormatSize(ULONGLONG nFormatSize);
	bool Refresh();

public:
	bool m_bInitialized;
	CMainFrame* m_pMainFrame;
	CSystemSnapshot m_pSystemSnapshot;
	UINT m_nRefreshTimerID;

protected:
	DECLARE_MESSAGE_MAP()
};
