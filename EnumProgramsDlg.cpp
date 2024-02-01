/* This file is part of IntelliTask application developed by Stefan-Mihai MOGA.

IntelliTask is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliTask is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliTask. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

#include "afxdialogex.h"
#include "afxdialogex.h"

// EnumProgramsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliTask.h"
#include "afxdialogex.h"
#include "EnumProgramsDlg.h"

#include <afxcoll.h>

#define	IS_KEY		_T("Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall")
#define	IS_KEY_LEN	256
#define IS_DISPLAY	_T("DisplayName")

// CEnumProgramsDlg dialog

IMPLEMENT_DYNAMIC(CEnumProgramsDlg, CDialogEx)

CEnumProgramsDlg::CEnumProgramsDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ENUMPROGRAMS_DIALOG, pParent)
{
}

CEnumProgramsDlg::~CEnumProgramsDlg()
{
}

void CEnumProgramsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRAMS, m_ctrlPrograms);
}

BEGIN_MESSAGE_MAP(CEnumProgramsDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CEnumProgramsDlg message handlers

BOOL CEnumProgramsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	BuildList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEnumProgramsDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
}

void CSortStringArray::Sort()
{
	BOOL bNotDone = TRUE;

	while (bNotDone)
	{
		bNotDone = FALSE;
		for (int pos = 0; pos < GetUpperBound(); pos++)
			bNotDone |= CompareAndSwap(pos);
	}
}
BOOL CSortStringArray::CompareAndSwap(int pos)
{
	CString temp;
	int posFirst = pos;
	int posNext = pos + 1;

	if (GetAt(posFirst).CompareNoCase(GetAt(posNext)) > 0)
	{
		temp = GetAt(posFirst);
		SetAt(posFirst, GetAt(posNext));
		SetAt(posNext, temp);
		return TRUE;

	}
	return FALSE;
}

void CEnumProgramsDlg::BuildList()
{
	// Build a list of installed applications by enumerating
	//	HKLM\Software\Microsoft\Windows\CurrentVersion\Uninstall
	//	and fetching "DisplayName" entry

	HKEY hKey;
	if (::RegOpenKeyEx(HKEY_LOCAL_MACHINE, IS_KEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		return;

	DWORD dwIndex = 0;
	LONG lRet;
	DWORD cbName = IS_KEY_LEN;
	TCHAR szSubKeyName[IS_KEY_LEN];

	while ((lRet = ::RegEnumKeyEx(hKey, dwIndex, szSubKeyName, &cbName, NULL,
		NULL, NULL, NULL)) != ERROR_NO_MORE_ITEMS)
	{
		// Do we have a key to open?
		if (lRet == ERROR_SUCCESS)
		{
			// Open the key and get the value
			HKEY hItem;
			if (::RegOpenKeyEx(hKey, szSubKeyName, 0, KEY_READ, &hItem) != ERROR_SUCCESS)
				continue;
			// Opened - look for "DisplayName"
			TCHAR szDisplayName[IS_KEY_LEN];
			DWORD dwSize = sizeof(szDisplayName);
			DWORD dwType;

			if (::RegQueryValueEx(hItem, IS_DISPLAY, NULL, &dwType,
				(LPBYTE)&szDisplayName, &dwSize) == ERROR_SUCCESS)
			{
				// Add to the main array
				m_arrPrograms.Add(szDisplayName);
			}
			::RegCloseKey(hItem);
		}
		dwIndex++;
		cbName = IS_KEY_LEN;
	}
	::RegCloseKey(hKey);

	m_arrPrograms.Sort();

	for (int nIndex = 0; nIndex <= m_arrPrograms.GetUpperBound(); nIndex++)
		m_ctrlPrograms.InsertItem(m_ctrlPrograms.GetItemCount(), m_arrPrograms[nIndex], 0);
}
