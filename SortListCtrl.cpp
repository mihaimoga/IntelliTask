/* This file is part of IntelliFile application developed by Stefan-Mihai MOGA.

IntelliFile is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliFile is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliFile.  If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// SortListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliTask.h"
#include "SortListCtrl.h"

// CSortListCtrl

IMPLEMENT_DYNAMIC(CSortListCtrl, CMFCListCtrl)

CSortListCtrl::CSortListCtrl()
{
	m_pSystemSnapshot = NULL;
}

CSortListCtrl::~CSortListCtrl()
{
}

BEGIN_MESSAGE_MAP(CSortListCtrl, CMFCListCtrl)
END_MESSAGE_MAP()

// CSortListCtrl message handlers

int CSortListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	UNREFERENCED_PARAMETER(iColumn);
	if (m_pSystemSnapshot != NULL)
	{
		CProcessData* pParam1 = m_pSystemSnapshot->GetProcessID((DWORD)lParam1);
		CProcessData* pParam2 = m_pSystemSnapshot->GetProcessID((DWORD)lParam2);
		if ((pParam1 != NULL) && (pParam2 != NULL))
		{
			CString strFileName1 = pParam1->GetFileName();
			CString strFileName2 = pParam2->GetFileName();
			const int nRetVal = strFileName1.CompareNoCase(strFileName2);
			if (nRetVal == 0)
			{
				return (pParam1->GetProcessID() - pParam2->GetProcessID());
			}
			else
			{
				return nRetVal;
			}
		}
	}
	return 0;
}
