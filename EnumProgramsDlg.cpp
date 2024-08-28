/* Copyright (C) 2012-2024 Stefan-Mihai MOGA
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

#include "Dtwinver.h"

#if defined(COSVERSION_WIN16_OR_DOS)
#define _tcscat strcat
#define _stprintf sprintf
#endif //#if defined(COSVERSION_WIN16_OR_DOS)

#ifndef _In_
#define _In_
#endif //#ifndef _In_

#ifndef _In_opt_
#define _In_opt_
#endif //#ifndef _In_opt_

#ifndef DEVICEFAMILYINFOENUM_WINDOWS_8X
#define DEVICEFAMILYINFOENUM_WINDOWS_8X 0x00000001
#endif //#ifndef DEVICEFAMILYINFOENUM_WINDOWS_8X

#ifndef DEVICEFAMILYINFOENUM_WINDOWS_PHONE_8X
#define DEVICEFAMILYINFOENUM_WINDOWS_PHONE_8X 0x00000002
#endif //#ifndef DEVICEFAMILYINFOENUM_WINDOWS_PHONE_8X

#ifndef DEVICEFAMILYINFOENUM_DESKTOP
#define DEVICEFAMILYINFOENUM_DESKTOP 0x00000003
#endif //#ifndef DEVICEFAMILYINFOENUM_DESKTOP

#ifndef DEVICEFAMILYINFOENUM_MOBILE
#define DEVICEFAMILYINFOENUM_MOBILE 0x00000004
#endif //#ifndef DEVICEFAMILYINFOENUM_MOBILE

#ifndef DEVICEFAMILYINFOENUM_XBOX
#define DEVICEFAMILYINFOENUM_XBOX 0x00000005
#endif //#ifndef DEVICEFAMILYINFOENUM_XBOX

#ifndef DEVICEFAMILYINFOENUM_TEAM
#define DEVICEFAMILYINFOENUM_TEAM 0x00000006
#endif //#ifndef DEVICEFAMILYINFOENUM_TEAM

#ifndef DEVICEFAMILYINFOENUM_IOT
#define DEVICEFAMILYINFOENUM_IOT 0x00000007
#endif //#ifndef DEVICEFAMILYINFOENUM_IOT

#ifndef DEVICEFAMILYINFOENUM_IOT_HEADLESS
#define DEVICEFAMILYINFOENUM_IOT_HEADLESS 0x00000008
#endif //#ifndef DEVICEFAMILYINFOENUM_IOT_HEADLESS

#ifndef DEVICEFAMILYINFOENUM_SERVER
#define DEVICEFAMILYINFOENUM_SERVER 0x00000009
#endif //#ifndef DEVICEFAMILYINFOENUM_SERVER

#ifndef DEVICEFAMILYINFOENUM_HOLOGRAPHIC
#define DEVICEFAMILYINFOENUM_HOLOGRAPHIC 0x0000000A
#endif //#ifndef DEVICEFAMILYINFOENUM_HOLOGRAPHIC

#ifndef DEVICEFAMILYINFOENUM_XBOXSRA
#define DEVICEFAMILYINFOENUM_XBOXSRA 0x0000000B
#endif //#ifndef DEVICEFAMILYINFOENUM_XBOXSRA

#ifndef DEVICEFAMILYINFOENUM_XBOXERA
#define DEVICEFAMILYINFOENUM_XBOXERA 0x0000000C
#endif //#ifndef DEVICEFAMILYINFOENUM_XBOXERA

#ifndef DEVICEFAMILYINFOENUM_SERVER_NANO
#define DEVICEFAMILYINFOENUM_SERVER_NANO 0x0000000D
#endif //#ifndef DEVICEFAMILYINFOENUM_SERVER_NANO

#ifndef DEVICEFAMILYINFOENUM_8828080
#define DEVICEFAMILYINFOENUM_8828080 0x0000000E
#endif //#ifndef DEVICEFAMILYINFOENUM_8828080

#ifndef DEVICEFAMILYINFOENUM_7067329
#define DEVICEFAMILYINFOENUM_7067329 0x0000000F
#endif //#ifndef DEVICEFAMILYINFOENUM_7067329

#ifndef DEVICEFAMILYINFOENUM_WINDOWS_CORE
#define DEVICEFAMILYINFOENUM_WINDOWS_CORE 0x00000010
#endif //#ifndef DEVICEFAMILYINFOENUM_WINDOWS_CORE

#ifndef DEVICEFAMILYINFOENUM_WINDOWS_CORE_HEADLESS
#define DEVICEFAMILYINFOENUM_WINDOWS_CORE_HEADLESS 0x00000011
#endif //#ifndef DEVICEFAMILYINFOENUM_WINDOWS_CORE_HEADLESS

#ifndef DEVICEFAMILYDEVICEFORM_PHONE
#define DEVICEFAMILYDEVICEFORM_PHONE 0x00000001
#endif //#ifndef DEVICEFAMILYDEVICEFORM_PHONE

#ifndef DEVICEFAMILYDEVICEFORM_TABLET
#define DEVICEFAMILYDEVICEFORM_TABLET 0x00000002
#endif //#ifndef DEVICEFAMILYDEVICEFORM_TABLET

#ifndef DEVICEFAMILYDEVICEFORM_DESKTOP
#define DEVICEFAMILYDEVICEFORM_DESKTOP 0x00000003
#endif //#ifndef DEVICEFAMILYDEVICEFORM_DESKTOP

#ifndef DEVICEFAMILYDEVICEFORM_NOTEBOOK
#define DEVICEFAMILYDEVICEFORM_NOTEBOOK 0x00000004
#endif //#ifndef DEVICEFAMILYDEVICEFORM_NOTEBOOK

#ifndef DEVICEFAMILYDEVICEFORM_CONVERTIBLE
#define DEVICEFAMILYDEVICEFORM_CONVERTIBLE 0x00000005
#endif //#ifndef DEVICEFAMILYDEVICEFORM_CONVERTIBLE

#ifndef DEVICEFAMILYDEVICEFORM_DETACHABLE
#define DEVICEFAMILYDEVICEFORM_DETACHABLE 0x00000006
#endif //#ifndef DEVICEFAMILYDEVICEFORM_DETACHABLE

#ifndef DEVICEFAMILYDEVICEFORM_ALLINONE
#define DEVICEFAMILYDEVICEFORM_ALLINONE 0x00000007
#endif //#ifndef DEVICEFAMILYDEVICEFORM_ALLINONE

#ifndef DEVICEFAMILYDEVICEFORM_STICKPC
#define DEVICEFAMILYDEVICEFORM_STICKPC 0x00000008
#endif //#ifndef DEVICEFAMILYDEVICEFORM_STICKPC

#ifndef DEVICEFAMILYDEVICEFORM_PUCK
#define DEVICEFAMILYDEVICEFORM_PUCK 0x00000009
#endif //#ifndef DEVICEFAMILYDEVICEFORM_PUCK

#ifndef DEVICEFAMILYDEVICEFORM_LARGESCREEN
#define DEVICEFAMILYDEVICEFORM_LARGESCREEN 0x0000000A
#endif //#ifndef DEVICEFAMILYDEVICEFORM_LARGESCREEN

#ifndef DEVICEFAMILYDEVICEFORM_HMD
#define DEVICEFAMILYDEVICEFORM_HMD 0x0000000B
#endif //#ifndef DEVICEFAMILYDEVICEFORM_HMD

#ifndef DEVICEFAMILYDEVICEFORM_INDUSTRY_HANDHELD
#define DEVICEFAMILYDEVICEFORM_INDUSTRY_HANDHELD 0x0000000C
#endif //#ifndef DEVICEFAMILYDEVICEFORM_INDUSTRY_HANDHELD

#ifndef DEVICEFAMILYDEVICEFORM_INDUSTRY_TABLET
#define DEVICEFAMILYDEVICEFORM_INDUSTRY_TABLET 0x0000000D
#endif //#ifndef DEVICEFAMILYDEVICEFORM_INDUSTRY_TABLET

#ifndef DEVICEFAMILYDEVICEFORM_BANKING
#define DEVICEFAMILYDEVICEFORM_BANKING 0x0000000E
#endif //#ifndef DEVICEFAMILYDEVICEFORM_BANKING

#ifndef DEVICEFAMILYDEVICEFORM_BUILDING_AUTOMATION
#define DEVICEFAMILYDEVICEFORM_BUILDING_AUTOMATION 0x0000000F
#endif //#ifndef DEVICEFAMILYDEVICEFORM_BUILDING_AUTOMATION

#ifndef DEVICEFAMILYDEVICEFORM_DIGITAL_SIGNAGE
#define DEVICEFAMILYDEVICEFORM_DIGITAL_SIGNAGE 0x00000010
#endif //#ifndef DEVICEFAMILYDEVICEFORM_DIGITAL_SIGNAGE

#ifndef DEVICEFAMILYDEVICEFORM_GAMING
#define DEVICEFAMILYDEVICEFORM_GAMING 0x00000011
#endif //#ifndef DEVICEFAMILYDEVICEFORM_GAMING

#ifndef DEVICEFAMILYDEVICEFORM_HOME_AUTOMATION
#define DEVICEFAMILYDEVICEFORM_HOME_AUTOMATION 0x00000012
#endif //#ifndef DEVICEFAMILYDEVICEFORM_HOME_AUTOMATION

#ifndef DEVICEFAMILYDEVICEFORM_INDUSTRIAL_AUTOMATION
#define DEVICEFAMILYDEVICEFORM_INDUSTRIAL_AUTOMATION 0x00000013
#endif //#ifndef DEVICEFAMILYDEVICEFORM_INDUSTRIAL_AUTOMATION

#ifndef DEVICEFAMILYDEVICEFORM_KIOSK
#define DEVICEFAMILYDEVICEFORM_KIOSK 0x00000014
#endif //#ifndef DEVICEFAMILYDEVICEFORM_KIOSK

#ifndef DEVICEFAMILYDEVICEFORM_MAKER_BOARD
#define DEVICEFAMILYDEVICEFORM_MAKER_BOARD 0x00000015
#endif //#ifndef DEVICEFAMILYDEVICEFORM_MAKER_BOARD

#ifndef DEVICEFAMILYDEVICEFORM_MEDICAL
#define DEVICEFAMILYDEVICEFORM_MEDICAL 0x00000016
#endif //#ifndef DEVICEFAMILYDEVICEFORM_MEDICAL

#ifndef DEVICEFAMILYDEVICEFORM_NETWORKING
#define DEVICEFAMILYDEVICEFORM_NETWORKING 0x00000017
#endif //#ifndef DEVICEFAMILYDEVICEFORM_NETWORKING

#ifndef DEVICEFAMILYDEVICEFORM_POINT_OF_SERVICE
#define DEVICEFAMILYDEVICEFORM_POINT_OF_SERVICE 0x00000018
#endif //#ifndef DEVICEFAMILYDEVICEFORM_POINT_OF_SERVICE

#ifndef DEVICEFAMILYDEVICEFORM_PRINTING
#define DEVICEFAMILYDEVICEFORM_PRINTING 0x00000019
#endif //#ifndef DEVICEFAMILYDEVICEFORM_PRINTING

#ifndef DEVICEFAMILYDEVICEFORM_THIN_CLIENT
#define DEVICEFAMILYDEVICEFORM_THIN_CLIENT 0x0000001A
#endif //#ifndef DEVICEFAMILYDEVICEFORM_THIN_CLIENT

#ifndef DEVICEFAMILYDEVICEFORM_TOY
#define DEVICEFAMILYDEVICEFORM_TOY 0x0000001B
#endif //#ifndef DEVICEFAMILYDEVICEFORM_TOY

#ifndef DEVICEFAMILYDEVICEFORM_VENDING
#define DEVICEFAMILYDEVICEFORM_VENDING 0x0000001C
#endif //#ifndef DEVICEFAMILYDEVICEFORM_VENDING

#ifndef DEVICEFAMILYDEVICEFORM_INDUSTRY_OTHER
#define DEVICEFAMILYDEVICEFORM_INDUSTRY_OTHER 0x0000001D
#endif //#ifndef DEVICEFAMILYDEVICEFORM_INDUSTRY_OTHER

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_ONE
#define DEVICEFAMILYDEVICEFORM_XBOX_ONE 0x0000001E
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_ONE

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_ONE_S
#define DEVICEFAMILYDEVICEFORM_XBOX_ONE_S 0x0000001F
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_ONE_S

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_ONE_X
#define DEVICEFAMILYDEVICEFORM_XBOX_ONE_X 0x00000020
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_ONE_X

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_ONE_X_DEVKIT
#define DEVICEFAMILYDEVICEFORM_XBOX_ONE_X_DEVKIT 0x00000021
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_ONE_X_DEVKIT

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_SERIES_X
#define DEVICEFAMILYDEVICEFORM_XBOX_SERIES_X 0x00000022
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_SERIES_X

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_SERIES_X_DEVKIT
#define DEVICEFAMILYDEVICEFORM_XBOX_SERIES_X_DEVKIT 0x00000023
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_SERIES_X_DEVKIT

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_SERIES_S
#define DEVICEFAMILYDEVICEFORM_XBOX_SERIES_S 0x00000024
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_SERIES_S

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_01
#define DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_01 0x00000025
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_01

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_02
#define DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_02 0x00000026
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_02

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_03
#define DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_03 0x00000027
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_03

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_04
#define DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_04 0x00000028
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_04

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_05
#define DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_05 0x00000029
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_05

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_06
#define DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_06 0x0000002A
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_06

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_07
#define DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_07 0x0000002B
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_07

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_08
#define DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_08 0x0000002C
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_08

#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_09
#define DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_09 0x0000002D
#endif //#ifndef DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_09

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
	ON_BN_CLICKED(IDC_OS_VERSION, &CEnumProgramsDlg::OnBnClickedVersion)
	ON_BN_CLICKED(IDC_REFRESH, &CEnumProgramsDlg::OnClickedRefresh)
END_MESSAGE_MAP()

// CEnumProgramsDlg message handlers

BOOL CEnumProgramsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	OnClickedRefresh();

	VERIFY(m_pWindowResizer.Hook(this));
	VERIFY(m_pWindowResizer.SetAnchor(IDC_PROGRAMS, ANCHOR_LEFT | ANCHOR_RIGHT | ANCHOR_TOP | ANCHOR_BOTTOM));
	VERIFY(m_pWindowResizer.SetAnchor(IDC_OS_VERSION, ANCHOR_LEFT | ANCHOR_BOTTOM));
	VERIFY(m_pWindowResizer.SetAnchor(IDC_REFRESH, ANCHOR_LEFT | ANCHOR_BOTTOM));
	VERIFY(m_pWindowResizer.SetAnchor(IDCANCEL, ANCHOR_RIGHT | ANCHOR_BOTTOM));

	const int nWidth = theApp.GetInt(_T("Width"), -1);
	const int nHeight = theApp.GetInt(_T("Height"), -1);
	if ((-1 != nWidth) && (-1 != nHeight))
	{
		CRect pWndRect(0, 0, nWidth, nHeight);
		MoveWindow(pWndRect, FALSE);
		CenterWindow();
		UpdateWindow();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEnumProgramsDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	RECT pWndRect;
	GetWindowRect(&pWndRect);
	const int nWidth = pWndRect.right - pWndRect.left;
	const int nHeight = pWndRect.bottom - pWndRect.top;
	theApp.WriteInt(_T("Width"), nWidth);
	theApp.WriteInt(_T("Height"), nHeight);
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

void CEnumProgramsDlg::OnClickedRefresh()
{
	CWaitCursor pWaitCursor;
	m_ctrlPrograms.SetRedraw(FALSE);
	m_arrPrograms.RemoveAll();
	m_ctrlPrograms.DeleteAllItems();

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

	m_ctrlPrograms.SetRedraw(TRUE);
	m_ctrlPrograms.UpdateWindow();
}

void CEnumProgramsDlg::OnBnClickedVersion()
{
	COSVersion::OS_VERSION_INFO osvi;
	memset(&osvi, 0, sizeof(osvi));
#ifdef _WIN32
	TCHAR sText[2048]; //NOLINT(modernize-avoid-c-arrays)
	sText[0] = _T('\0');
	TCHAR sBuf[100]; //NOLINT(modernize-avoid-c-arrays)
	sBuf[0] = _T('\0');
#else
	char sText[2048];
	char sBuf[100];
#endif //#ifdef _WIN32

	COSVersion os;
	if (os.GetVersion(&osvi))
	{
#ifndef UNDER_CE
		_stprintf(sText, _T("Emulated OS: "));

		switch (osvi.EmulatedPlatform)
		{
			case COSVersion::Dos:
			{
				_tcscat(sText, _T("DOS")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::Windows3x:
			{
				_tcscat(sText, _T("Windows")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::WindowsCE:
			{
				//This code will never really be executed, but for the same of completeness include it here
				if (os.IsWindowsEmbeddedCompact(&osvi, FALSE))
					_tcscat(sText, _T("Windows Embedded Compact")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				else if (os.IsWindowsCENET(&osvi, FALSE))
					_tcscat(sText, _T("Windows CE .NET")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				else
					_tcscat(sText, _T("Windows CE")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::Windows9x:
			{
				if (os.IsWindows95(&osvi, FALSE))
					_stprintf(sBuf, _T("Windows 95"));
				else if (os.IsWindows95SP1(&osvi, FALSE))
					_stprintf(sBuf, _T("Windows 95 SP1"));
				else if (os.IsWindows95B(&osvi, FALSE))
					_stprintf(sBuf, _T("Windows 95 B [aka OSR2]"));
				else if (os.IsWindows95C(&osvi, FALSE))
					_stprintf(sBuf, _T("Windows 95 C [aka OSR2.5]"));
				else if (os.IsWindows98(&osvi, FALSE))
					_stprintf(sBuf, _T("Windows 98"));
				else if (os.IsWindows98SP1(&osvi, FALSE))
					_stprintf(sBuf, _T("Windows 98 SP1"));
				else if (os.IsWindows98SE(&osvi, FALSE))
					_stprintf(sBuf, _T("Windows 98 Second Edition"));
				else if (os.IsWindowsME(&osvi, FALSE))
					_stprintf(sBuf, _T("Windows Millenium Edition"));
				else
					_stprintf(sBuf, _T("Windows \?\?"));
				_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::WindowsNT:
			{
				if (os.IsNTPreWin2k(&osvi, FALSE))
				{
					_tcscat(sText, _T("Windows NT")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsNTWorkstation(&osvi, FALSE))
						_tcscat(sText, _T(" (Workstation)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsNTStandAloneServer(&osvi, FALSE))
						_tcscat(sText, _T(" (Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsNTPDC(&osvi, FALSE))
						_tcscat(sText, _T(" (Primary Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsNTBDC(&osvi, FALSE))
						_tcscat(sText, _T(" (Backup Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsNTDatacenterServer(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsNTEnterpriseServer(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows2000(&osvi, FALSE))
				{
					if (os.IsProfessional(&osvi))
						_tcscat(sText, _T("Windows 2000 (Professional)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows2000Server(&osvi, FALSE))
						_tcscat(sText, _T("Windows 2000 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows2000DomainController(&osvi, FALSE))
						_tcscat(sText, _T("Windows 2000 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows 2000")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsWindows2000DatacenterServer(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows2000AdvancedServer(&osvi, FALSE))
						_tcscat(sText, _T(", (Advanced Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsXPOrWindowsServer2003(&osvi, FALSE))
				{
					if (os.IsStarterEdition(&osvi))
						_tcscat(sText, _T("Windows XP (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsPersonal(&osvi))
						_tcscat(sText, _T("Windows XP (Personal)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsProfessional(&osvi))
						_tcscat(sText, _T("Windows XP (Professional)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServer2003(&osvi, FALSE))
						_tcscat(sText, _T("Windows Server 2003")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2003(&osvi, FALSE))
						_tcscat(sText, _T("Windows Server 2003 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServer2003R2(&osvi, FALSE))
						_tcscat(sText, _T("Windows Server 2003 R2")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2003R2(&osvi, FALSE))
						_tcscat(sText, _T("Windows Server 2003 R2 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows XP")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsDatacenterWindowsServer2003(&osvi, FALSE) || os.IsDatacenterWindowsServer2003R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2003(&osvi, FALSE) || os.IsEnterpriseWindowsServer2003R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2003(&osvi, FALSE) || os.IsWebWindowsServer2003R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2003(&osvi, FALSE) || os.IsStandardWindowsServer2003R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsVistaOrWindowsServer2008(&osvi, FALSE))
				{
					if (os.IsWindowsVista(&osvi, FALSE))
					{
						if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows Vista (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsHomeBasic(&osvi))
							_tcscat(sText, _T("Windows Vista (Home Basic)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsHomeBasicPremium(&osvi))
							_tcscat(sText, _T("Windows Vista (Home Premium)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsBusiness(&osvi))
							_tcscat(sText, _T("Windows Vista (Business)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows Vista (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsUltimate(&osvi))
							_tcscat(sText, _T("Windows Vista (Ultimate)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Vista")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsWindowsServer2008(&osvi, FALSE))
							_tcscat(sText, _T("Windows Server 2008")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2008(&osvi, FALSE))
							_tcscat(sText, _T("Windows Server 2008 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2008")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsDatacenterWindowsServer2008(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2008(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2008(&osvi, FALSE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2008(&osvi, FALSE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows7OrWindowsServer2008R2(&osvi, FALSE))
				{
					if (os.IsWindows7(&osvi, FALSE))
					{
						if (os.IsThinPC(&osvi))
							_tcscat(sText, _T("Windows 7 Thin PC")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows 7 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsHomeBasic(&osvi))
							_tcscat(sText, _T("Windows 7 (Home Basic)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsHomeBasicPremium(&osvi))
							_tcscat(sText, _T("Windows 7 (Home Premium)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProfessional(&osvi))
							_tcscat(sText, _T("Windows 7 (Professional)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows 7 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsUltimate(&osvi))
							_tcscat(sText, _T("Windows 7 (Ultimate)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows 7")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsWindowsServer2008R2(&osvi, FALSE))
							_tcscat(sText, _T("Windows Server 2008 R2")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2008R2(&osvi, FALSE))
							_tcscat(sText, _T("Windows Server 2008 R2 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2008")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsDatacenterWindowsServer2008R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2008R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2008R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2008R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows8OrWindowsServer2012(&osvi, FALSE))
				{
					if (os.IsWindows8(&osvi, FALSE))
					{
						if (os.IsThinPC(&osvi))
							_tcscat(sText, _T("Windows 8 Thin PC")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindowsRT(&osvi, FALSE))
							_tcscat(sText, _T("Windows 8 RT")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows 8 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProfessional(&osvi))
							_tcscat(sText, _T("Windows 8 (Pro)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows 8 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows 8")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsWindowsServer2012(&osvi, FALSE))
							_tcscat(sText, _T("Windows Server 2012")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2012(&osvi, FALSE))
							_tcscat(sText, _T("Windows Server 2012 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2012")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsDatacenterWindowsServer2012(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2012(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2012(&osvi, FALSE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2012(&osvi, FALSE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows8Point1OrWindowsServer2012R2(&osvi, FALSE))
				{
					if (os.IsWindows8Point1(&osvi, FALSE))
					{
						if (os.IsThinPC(&osvi))
							_tcscat(sText, _T("Windows 8.1 Thin PC")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindowsRT(&osvi, FALSE))
							_tcscat(sText, _T("Windows 8.1 RT")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows 8.1 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProfessional(&osvi))
							_tcscat(sText, _T("Windows 8.1 (Pro)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows 8.1 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows 8.1")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsWindowsServer2012R2(&osvi, FALSE))
							_tcscat(sText, _T("Windows Server 2012 R2")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2012R2(&osvi, FALSE))
							_tcscat(sText, _T("Windows Server 2012 R2 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2012 R2")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsCoreConnected(&osvi))
						_tcscat(sText, _T(", (with Bing / CoreConnected)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsWindows8Point1Or2012R2Update(&osvi))
						_tcscat(sText, _T(", (Update)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsDatacenterWindowsServer2012R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2012R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2012R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2012R2(&osvi, FALSE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows10OrWindowsServer2016(&osvi, FALSE))
				{
					if (os.IsWindows10(&osvi, FALSE))
					{
						if (os.IsThinPC(&osvi))
							_tcscat(sText, _T("Windows 10 Thin PC")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindowsRT(&osvi, FALSE))
							_tcscat(sText, _T("Windows 10 RT")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows 10 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsCore(&osvi))
							_tcscat(sText, _T("Windows 10 (Home)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProfessional(&osvi))
							_tcscat(sText, _T("Windows 10 (Pro)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProWorkstations(&osvi))
							_tcscat(sText, _T("Windows 10 (Pro for Workstations)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows 10 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsSEdition(&osvi))
							_tcscat(sText, _T("Windows 10 S")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindows10X(&osvi))
							_tcscat(sText, _T("Windows 10X")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows 10")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsNanoServer(&osvi))
							_tcscat(sText, _T("Windows Server 2016 Nano Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsARM64Server(&osvi))
							_tcscat(sText, _T("Windows Server 2016 ARM64 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2016(&osvi, FALSE))
							_tcscat(sText, _T("Windows Server 2016 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2016")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						if (os.IsWindowsServerVersion1709(&osvi, FALSE))
							_tcscat(sText, _T(", (version 1709)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindowsServerVersion1803(&osvi, FALSE))
							_tcscat(sText, _T(", (version 1803)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsWindows10Version1507(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1507)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1511(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1511)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1607(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1607)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1703(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1703)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1709(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1709)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1803(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1803)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1809(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1809)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1903(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1903)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1909(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1909)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version2004(&osvi, FALSE))
						_tcscat(sText, _T(", (version 2004)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version20H2(&osvi, FALSE))
						_tcscat(sText, _T(", (version 20H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version21H1(&osvi, FALSE))
						_tcscat(sText, _T(", (version 21H1)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version21H2(&osvi, FALSE))
						_tcscat(sText, _T(", (version 21H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version22H2(&osvi, FALSE))
						_tcscat(sText, _T(", (version 22H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10ActiveDevelopmentBranch(&osvi, FALSE))
						_tcscat(sText, _T(", (Active Development Branch)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsCoreConnected(&osvi))
						_tcscat(sText, _T(", (with Bing / CoreConnected)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsDatacenterWindowsServer2016(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2016(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2016(&osvi, FALSE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2016(&osvi, FALSE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows11(&osvi, FALSE))
				{
					if (os.IsStarterEdition(&osvi))
						_tcscat(sText, _T("Windows 11 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsCore(&osvi))
						_tcscat(sText, _T("Windows 11 (Home)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsProfessional(&osvi))
						_tcscat(sText, _T("Windows 11 (Pro)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsProWorkstations(&osvi))
						_tcscat(sText, _T("Windows 11 (Pro for Workstations)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterprise(&osvi))
						_tcscat(sText, _T("Windows 11 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsSEdition(&osvi))
						_tcscat(sText, _T("Windows 11 S")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows 11")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsWindows11Version21H2(&osvi, FALSE))
						_tcscat(sText, _T(", (version 21H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows11Version22H2(&osvi, FALSE))
						_tcscat(sText, _T(", (version 22H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows11Version23H2(&osvi, FALSE))
						_tcscat(sText, _T(", (version 23H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows11Version24H2(&osvi, FALSE))
						_tcscat(sText, _T(", (version 24H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows11ActiveDevelopmentBranch(&osvi, FALSE))
						_tcscat(sText, _T(", (Active Development Branch)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsServer2019(&osvi, FALSE))
				{
					if (os.IsNanoServer(&osvi))
						_tcscat(sText, _T("Windows Server 2019 Nano Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsARM64Server(&osvi))
						_tcscat(sText, _T("Windows Server 2019 ARM64 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2019(&osvi, FALSE))
						_tcscat(sText, _T("Windows Server 2019 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows Server 2019")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsWindowsServerVersion1809(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1809)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServerVersion1903(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1903)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServerVersion1909(&osvi, FALSE))
						_tcscat(sText, _T(", (version 1909)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServerVersion2004(&osvi, FALSE))
						_tcscat(sText, _T(", (version 2004)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServerVersion20H2(&osvi, FALSE))
						_tcscat(sText, _T(", (version 20H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServer2019ActiveDevelopmentBranch(&osvi, FALSE))
						_tcscat(sText, _T(", (vNext aka Active Development Branch)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsDatacenterWindowsServer2019(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2019(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2019(&osvi, FALSE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2019(&osvi, FALSE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsServer2022(&osvi, FALSE))
				{
					if (os.IsNanoServer(&osvi))
						_tcscat(sText, _T("Windows Server 2022 Nano Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsARM64Server(&osvi))
						_tcscat(sText, _T("Windows Server 2022 ARM64 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2022(&osvi, FALSE))
						_tcscat(sText, _T("Windows Server 2022 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows Server 2022")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsWindowsServerVersion23H2(&osvi, FALSE))
						_tcscat(sText, _T(", (version 23H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServer2022ActiveDevelopmentBranch(&osvi, FALSE))
						_tcscat(sText, _T(", (vNext aka Active Development Branch)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsDatacenterWindowsServer2022(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2022(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2022(&osvi, FALSE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2022(&osvi, FALSE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsServer2025(&osvi, FALSE))
				{
					if (os.IsNanoServer(&osvi))
						_tcscat(sText, _T("Windows Server 2025 Nano Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsARM64Server(&osvi))
						_tcscat(sText, _T("Windows Server 2025 ARM64 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2025(&osvi, FALSE))
						_tcscat(sText, _T("Windows Server 2025 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows Server 2025")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsDatacenterWindowsServer2025(&osvi, FALSE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2025(&osvi, FALSE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2025(&osvi, FALSE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2025(&osvi, FALSE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				break;
			}
			default:
			{
				_tcscat(sText, _T("Unknown OS")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
		}

#ifndef UNDER_CE
		switch (osvi.EmulatedProcessorType)
		{
			case COSVersion::X86_PROCESSOR:
			{
				_tcscat(sText, _T(", (x86 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::MIPS_PROCESSOR:
			{
				_tcscat(sText, _T(", (MIPS Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ALPHA_PROCESSOR:
			{
				_tcscat(sText, _T(", (Alpha Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::PPC_PROCESSOR:
			{
				_tcscat(sText, _T(", (PPC Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::IA64_PROCESSOR:
			{
				_tcscat(sText, _T(", (IA64 Itanium[2] Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::AMD64_PROCESSOR:
			{
				_tcscat(sText, _T(", (x64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ALPHA64_PROCESSOR:
			{
				_tcscat(sText, _T(", (Alpha64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::MSIL_PROCESSOR:
			{
				_tcscat(sText, _T(", (MSIL Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ARM_PROCESSOR:
			{
				_tcscat(sText, _T(", (ARM Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::SHX_PROCESSOR:
			{
				_tcscat(sText, _T(", (SHX Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::IA32_ON_WIN64_PROCESSOR:
			{
				_tcscat(sText, _T(", (IA32 on Win64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::NEUTRAL_PROCESSOR:
			{
				_tcscat(sText, _T(", (Neutral Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ARM64_PROCESSOR:
			{
				_tcscat(sText, _T(", (ARM64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ARM32_ON_WIN64_PROCESSOR:
			{
				_tcscat(sText, _T(", (ARM32 on Win64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::IA32_ON_ARM64_PROCESSOR:
			{
				_tcscat(sText, _T(", (IA32 on ARM64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::UNKNOWN_PROCESSOR: //deliberate fallthrough
			default:
			{
				_tcscat(sText, _T(", (Unknown Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
		}
#endif //#ifndef UNDER_CE
		_stprintf(sBuf, _T(" v%d."), (int)(osvi.dwEmulatedMajorVersion));
		_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (osvi.dwEmulatedMinorVersion % 10)
		{
			if (osvi.dwEmulatedMinorVersion > 9)
				_stprintf(sBuf, _T("%02d"), (int)(osvi.dwEmulatedMinorVersion));
			else
				_stprintf(sBuf, _T("%01d"), (int)(osvi.dwEmulatedMinorVersion));
		}
		else
			_stprintf(sBuf, _T("%01u"), osvi.dwEmulatedMinorVersion / 10); //NOLINT(clang-diagnostic-format)
		_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (osvi.dwEmulatedBuildNumber)
		{
			_stprintf(sBuf, _T(" Build:%d"), (int)(osvi.dwEmulatedBuildNumber));
			_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		}
		if (osvi.wEmulatedServicePackMajor)
		{
			if (osvi.wEmulatedServicePackMinor)
			{
				//Handle the special case of NT 4 SP 6a which Dtwinver treats as SP 6.1
				if (os.IsNTPreWin2k(&osvi, FALSE) && (osvi.wEmulatedServicePackMajor == 6) && (osvi.wEmulatedServicePackMinor == 1))
					_stprintf(sBuf, _T(" Service Pack: 6a"));
				//Handle the special case of XP SP 1a which Dtwinver treats as SP 1.1
				else if (os.IsWindowsXP(&osvi, FALSE) && (osvi.wEmulatedServicePackMajor == 1) && (osvi.wEmulatedServicePackMinor == 1))
					_stprintf(sBuf, _T(" Service Pack: 1a"));
				else
					_stprintf(sBuf, _T(" Service Pack:%d.%d"), (int)(osvi.wEmulatedServicePackMajor), (int)(osvi.wEmulatedServicePackMinor));
			}
			else
				_stprintf(sBuf, _T(" Service Pack:%d"), (int)(osvi.wEmulatedServicePackMajor));
			_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		}
		else
		{
			if (osvi.wEmulatedServicePackMinor)
				_stprintf(sBuf, _T(" Service Pack:0.%d"), (int)(osvi.wEmulatedServicePackMinor));
		}

		_tcscat(sText, _T("\n")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
#endif //#ifndef UNDER_CE

		//CE does not really have a concept of an emulated OS so
		//lets not bother displaying any info on this on CE
		if (osvi.UnderlyingPlatform == COSVersion::WindowsCE)
			_tcscpy(sText, _T("OS: "));
		else
			_tcscat(sText, _T("Underlying OS: ")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

		switch (osvi.UnderlyingPlatform)
		{
			case COSVersion::Dos:
			{
				_tcscat(sText, _T("DOS")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::Windows3x:
			{
				_tcscat(sText, _T("Windows")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::WindowsCE:
			{
				if (os.IsWindowsEmbeddedCompact(&osvi, TRUE))
					_tcscat(sText, _T("Windows Embedded Compact")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				else if (os.IsWindowsCENET(&osvi, TRUE))
					_tcscat(sText, _T("Windows CE .NET")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				else
					_tcscat(sText, _T("Windows CE")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::Windows9x:
			{
				if (os.IsWindows95(&osvi, TRUE))
					_stprintf(sBuf, _T("Windows 95"));
				else if (os.IsWindows95SP1(&osvi, TRUE))
					_stprintf(sBuf, _T("Windows 95 SP1"));
				else if (os.IsWindows95B(&osvi, TRUE))
					_stprintf(sBuf, _T("Windows 95 B [aka OSR2]"));
				else if (os.IsWindows95C(&osvi, TRUE))
					_stprintf(sBuf, _T("Windows 95 C [aka OSR2.5]"));
				else if (os.IsWindows98(&osvi, TRUE))
					_stprintf(sBuf, _T("Windows 98"));
				else if (os.IsWindows98SP1(&osvi, TRUE))
					_stprintf(sBuf, _T("Windows 98 SP1"));
				else if (os.IsWindows98SE(&osvi, TRUE))
					_stprintf(sBuf, _T("Windows 98 Second Edition"));
				else if (os.IsWindowsME(&osvi, TRUE))
					_stprintf(sBuf, _T("Windows Millenium Edition"));
				else
					_stprintf(sBuf, _T("Windows \?\?"));
				_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::WindowsNT:
			{
				if (os.IsNTPreWin2k(&osvi, TRUE))
				{
					_tcscat(sText, _T("Windows NT")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsNTWorkstation(&osvi, TRUE))
						_tcscat(sText, _T(" (Workstation)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsNTStandAloneServer(&osvi, TRUE))
						_tcscat(sText, _T(" (Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsNTPDC(&osvi, TRUE))
						_tcscat(sText, _T(" (Primary Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsNTBDC(&osvi, TRUE))
						_tcscat(sText, _T(" (Backup Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsNTDatacenterServer(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsNTEnterpriseServer(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows2000(&osvi, TRUE))
				{
					if (os.IsProfessional(&osvi))
						_tcscat(sText, _T("Windows 2000 (Professional)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows2000Server(&osvi, TRUE))
						_tcscat(sText, _T("Windows 2000 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows2000DomainController(&osvi, TRUE))
						_tcscat(sText, _T("Windows 2000 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows 2000")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsWindows2000DatacenterServer(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows2000AdvancedServer(&osvi, TRUE))
						_tcscat(sText, _T(", (Advanced Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsXPOrWindowsServer2003(&osvi, TRUE))
				{
					if (os.IsStarterEdition(&osvi))
						_tcscat(sText, _T("Windows XP (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsPersonal(&osvi))
						_tcscat(sText, _T("Windows XP (Personal)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsProfessional(&osvi))
						_tcscat(sText, _T("Windows XP (Professional)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServer2003(&osvi, TRUE))
						_tcscat(sText, _T("Windows Server 2003")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2003(&osvi, TRUE))
						_tcscat(sText, _T("Windows Server 2003 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServer2003R2(&osvi, TRUE))
						_tcscat(sText, _T("Windows Server 2003 R2")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2003R2(&osvi, TRUE))
						_tcscat(sText, _T("Windows Server 2003 R2 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows XP")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsDatacenterWindowsServer2003(&osvi, TRUE) || os.IsDatacenterWindowsServer2003R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2003(&osvi, TRUE) || os.IsEnterpriseWindowsServer2003(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2003(&osvi, TRUE) || os.IsWebWindowsServer2003R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2003(&osvi, TRUE) || os.IsStandardWindowsServer2003R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsVistaOrWindowsServer2008(&osvi, TRUE))
				{
					if (os.IsWindowsVista(&osvi, TRUE))
					{
						if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows Vista (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsHomeBasic(&osvi))
							_tcscat(sText, _T("Windows Vista (Home Basic)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsHomeBasicPremium(&osvi))
							_tcscat(sText, _T("Windows Vista (Home Premium)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsBusiness(&osvi))
							_tcscat(sText, _T("Windows Vista (Business)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows Vista (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsUltimate(&osvi))
							_tcscat(sText, _T("Windows Vista (Ultimate)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Vista")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsWindowsServer2008(&osvi, TRUE))
							_tcscat(sText, _T("Windows Server 2008")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2008(&osvi, TRUE))
							_tcscat(sText, _T("Windows Server 2008 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2008")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsDatacenterWindowsServer2008(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2008(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2008(&osvi, TRUE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2008(&osvi, TRUE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows7OrWindowsServer2008R2(&osvi, TRUE))
				{
					if (os.IsWindows7(&osvi, TRUE))
					{
						if (os.IsThinPC(&osvi))
							_tcscat(sText, _T("Windows 7 Thin PC")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows 7 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsHomeBasic(&osvi))
							_tcscat(sText, _T("Windows 7 (Home Basic)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsHomeBasicPremium(&osvi))
							_tcscat(sText, _T("Windows 7 (Home Premium)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProfessional(&osvi))
							_tcscat(sText, _T("Windows 7 (Professional)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows 7 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsUltimate(&osvi))
							_tcscat(sText, _T("Windows 7 (Ultimate)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows 7")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsWindowsServer2008R2(&osvi, TRUE))
							_tcscat(sText, _T("Windows Server 2008 R2")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2008R2(&osvi, TRUE))
							_tcscat(sText, _T("Windows Server 2008 R2 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2008 R2")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsDatacenterWindowsServer2008R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2008R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2008R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2008R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows8OrWindowsServer2012(&osvi, TRUE))
				{
					if (os.IsWindows8(&osvi, TRUE))
					{
						if (os.IsThinPC(&osvi))
							_tcscat(sText, _T("Windows 8 Thin PC")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindowsRT(&osvi, TRUE))
							_tcscat(sText, _T("Windows 8 RT")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows 8 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProfessional(&osvi))
							_tcscat(sText, _T("Windows 8 (Pro)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows 8 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows 8")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsWindowsServer2012(&osvi, TRUE))
							_tcscat(sText, _T("Windows Server 2012")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2012(&osvi, TRUE))
							_tcscat(sText, _T("Windows Server 2012 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2012")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsDatacenterWindowsServer2012(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2012(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2012(&osvi, TRUE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2012(&osvi, TRUE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows8Point1OrWindowsServer2012R2(&osvi, TRUE))
				{
					if (os.IsWindows8Point1(&osvi, TRUE))
					{
						if (os.IsThinPC(&osvi))
							_tcscat(sText, _T("Windows 8.1 Thin PC")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindowsRT(&osvi, TRUE))
							_tcscat(sText, _T("Windows 8.1 RT")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows 8.1 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProfessional(&osvi))
							_tcscat(sText, _T("Windows 8.1 (Pro)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows 8.1 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows 8.1")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsWindowsServer2012R2(&osvi, TRUE))
							_tcscat(sText, _T("Windows Server 2012 R2")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2012R2(&osvi, TRUE))
							_tcscat(sText, _T("Windows Server 2012 R2 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2012 R2")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsCoreConnected(&osvi))
						_tcscat(sText, _T(", (with Bing / CoreConnected)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsWindows8Point1Or2012R2Update(&osvi))
						_tcscat(sText, _T(", (Update)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsDatacenterWindowsServer2012R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2012R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2012R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2012R2(&osvi, TRUE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows10OrWindowsServer2016(&osvi, TRUE))
				{
					if (os.IsWindows10(&osvi, TRUE))
					{
						if (os.IsThinPC(&osvi))
							_tcscat(sText, _T("Windows 10 Thin PC")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindowsRT(&osvi, TRUE))
							_tcscat(sText, _T("Windows 10 RT")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsStarterEdition(&osvi))
							_tcscat(sText, _T("Windows 10 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsCore(&osvi))
							_tcscat(sText, _T("Windows 10 (Home)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProfessional(&osvi))
							_tcscat(sText, _T("Windows 10 (Pro)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsProWorkstations(&osvi))
							_tcscat(sText, _T("Windows 10 (Pro for Workstations)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsEnterprise(&osvi))
							_tcscat(sText, _T("Windows 10 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsSEdition(&osvi))
							_tcscat(sText, _T("Windows 10 S")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindows10X(&osvi))
							_tcscat(sText, _T("Windows 10X")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows 10")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}
					else
					{
						if (os.IsNanoServer(&osvi))
							_tcscat(sText, _T("Windows Server 2016 Nano Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsARM64Server(&osvi))
							_tcscat(sText, _T("Windows Server 2016 ARM64 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsDomainControllerWindowsServer2016(&osvi, TRUE))
							_tcscat(sText, _T("Windows Server 2016 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else
							_tcscat(sText, _T("Windows Server 2016")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						if (os.IsWindowsServerVersion1709(&osvi, TRUE))
							_tcscat(sText, _T(", (version 1709)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
						else if (os.IsWindowsServerVersion1803(&osvi, TRUE))
							_tcscat(sText, _T(", (version 1803)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					}

					if (os.IsWindows10Version1507(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1507)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1511(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1511)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1607(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1607)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1703(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1703)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1709(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1709)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1803(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1803)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1809(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1809)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1903(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1903)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version1909(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1909)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version2004(&osvi, TRUE))
						_tcscat(sText, _T(", (version 2004)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version20H2(&osvi, TRUE))
						_tcscat(sText, _T(", (version 20H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version21H1(&osvi, TRUE))
						_tcscat(sText, _T(", (version 21H1)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version21H2(&osvi, TRUE))
						_tcscat(sText, _T(", (version 21H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10Version22H2(&osvi, TRUE))
						_tcscat(sText, _T(", (version 22H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows10ActiveDevelopmentBranch(&osvi, TRUE))
						_tcscat(sText, _T(", (Active Development Branch)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsCoreConnected(&osvi))
						_tcscat(sText, _T(", (with Bing / CoreConnected)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsDatacenterWindowsServer2016(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2016(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2016(&osvi, TRUE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2016(&osvi, TRUE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindows11(&osvi, TRUE))
				{
					if (os.IsStarterEdition(&osvi))
						_tcscat(sText, _T("Windows 11 (Starter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsCore(&osvi))
						_tcscat(sText, _T("Windows 11 (Home)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsProfessional(&osvi))
						_tcscat(sText, _T("Windows 11 (Pro)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsProWorkstations(&osvi))
						_tcscat(sText, _T("Windows 11 (Pro for Workstations)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterprise(&osvi))
						_tcscat(sText, _T("Windows 11 (Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsSEdition(&osvi))
						_tcscat(sText, _T("Windows 11 S")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows 11")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

					if (os.IsWindows11Version21H2(&osvi, TRUE))
						_tcscat(sText, _T(", (version 21H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows11Version22H2(&osvi, TRUE))
						_tcscat(sText, _T(", (version 22H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows11Version23H2(&osvi, TRUE))
						_tcscat(sText, _T(", (version 23H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows11Version24H2(&osvi, TRUE))
						_tcscat(sText, _T(", (version 24H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindows11ActiveDevelopmentBranch(&osvi, TRUE))
						_tcscat(sText, _T(", (Active Development Branch)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsServer2019(&osvi, TRUE))
				{
					if (os.IsNanoServer(&osvi))
						_tcscat(sText, _T("Windows Server 2019 Nano Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsARM64Server(&osvi))
						_tcscat(sText, _T("Windows Server 2019 ARM64 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2019(&osvi, TRUE))
						_tcscat(sText, _T("Windows Server 2019 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows Server 2019")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsWindowsServerVersion1809(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1809)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServerVersion1903(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1903)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServerVersion1909(&osvi, TRUE))
						_tcscat(sText, _T(", (version 1909)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServerVersion2004(&osvi, TRUE))
						_tcscat(sText, _T(", (version 2004)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServerVersion20H2(&osvi, TRUE))
						_tcscat(sText, _T(", (version 20H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServer2019ActiveDevelopmentBranch(&osvi, TRUE))
						_tcscat(sText, _T(", (vNext aka Active Development Branch)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsDatacenterWindowsServer2019(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2019(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2019(&osvi, TRUE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2019(&osvi, TRUE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsServer2022(&osvi, TRUE))
				{
					if (os.IsNanoServer(&osvi))
						_tcscat(sText, _T("Windows Server 2022 Nano Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsARM64Server(&osvi))
						_tcscat(sText, _T("Windows Server 2022 ARM64 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2022(&osvi, TRUE))
						_tcscat(sText, _T("Windows Server 2022 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows Server 2022")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsWindowsServerVersion23H2(&osvi, TRUE))
						_tcscat(sText, _T(", (version 23H2)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWindowsServer2022ActiveDevelopmentBranch(&osvi, TRUE))
						_tcscat(sText, _T(", (vNext aka Active Development Branch)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsDatacenterWindowsServer2022(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2022(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2022(&osvi, TRUE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2022(&osvi, TRUE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				else if (os.IsWindowsServer2025(&osvi, TRUE))
				{
					if (os.IsNanoServer(&osvi))
						_tcscat(sText, _T("Windows Server 2025 Nano Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsARM64Server(&osvi))
						_tcscat(sText, _T("Windows Server 2025 ARM64 Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsDomainControllerWindowsServer2025(&osvi, TRUE))
						_tcscat(sText, _T("Windows Server 2025 (Domain Controller)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else
						_tcscat(sText, _T("Windows Server 2025")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					if (os.IsDatacenterWindowsServer2025(&osvi, TRUE))
						_tcscat(sText, _T(", (Datacenter Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsEnterpriseWindowsServer2025(&osvi, TRUE))
						_tcscat(sText, _T(", (Enterprise Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsWebWindowsServer2025(&osvi, TRUE))
						_tcscat(sText, _T(", (Web Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					else if (os.IsStandardWindowsServer2025(&osvi, TRUE))
						_tcscat(sText, _T(", (Standard Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				}
				break;
			}
			default:
			{
				_stprintf(sBuf, _T("Unknown OS"));
				_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
		}

#ifndef UNDER_CE
		switch (osvi.UnderlyingProcessorType)
		{
			case COSVersion::X86_PROCESSOR:
			{
				_tcscat(sText, _T(", (x86 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::MIPS_PROCESSOR:
			{
				_tcscat(sText, _T(", (MIPS Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ALPHA_PROCESSOR:
			{
				_tcscat(sText, _T(", (Alpha Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::PPC_PROCESSOR:
			{
				_tcscat(sText, _T(", (PPC Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::IA64_PROCESSOR:
			{
				_tcscat(sText, _T(", (IA64 Itanium[2] Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::AMD64_PROCESSOR:
			{
				_tcscat(sText, _T(", (x64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ALPHA64_PROCESSOR:
			{
				_tcscat(sText, _T(", (Alpha64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::MSIL_PROCESSOR:
			{
				_tcscat(sText, _T(", (MSIL Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ARM_PROCESSOR:
			{
				_tcscat(sText, _T(", (ARM Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::SHX_PROCESSOR:
			{
				_tcscat(sText, _T(", (SHX Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::IA32_ON_WIN64_PROCESSOR:
			{
				_tcscat(sText, _T(", (IA32 on Win64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::NEUTRAL_PROCESSOR:
			{
				_tcscat(sText, _T(", (Neutral Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ARM64_PROCESSOR:
			{
				_tcscat(sText, _T(", (ARM64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::ARM32_ON_WIN64_PROCESSOR:
			{
				_tcscat(sText, _T(", (ARM32 on Win64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::IA32_ON_ARM64_PROCESSOR:
			{
				_tcscat(sText, _T(", (IA32 on ARM64 Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
			case COSVersion::UNKNOWN_PROCESSOR: //deliberate fallthrough
			default:
			{
				_tcscat(sText, _T(", (Unknown Processor)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
				break;
			}
		}
#endif //#ifndef UNDER_CE
		_stprintf(sBuf, _T(" v%d."), (int)(osvi.dwUnderlyingMajorVersion));
		_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (osvi.dwUnderlyingMinorVersion % 10)
		{
			if (osvi.dwUnderlyingMinorVersion > 9)
				_stprintf(sBuf, _T("%02d"), (int)(osvi.dwUnderlyingMinorVersion));
			else
				_stprintf(sBuf, _T("%01d"), (int)(osvi.dwUnderlyingMinorVersion));
		}
		else
			_stprintf(sBuf, _T("%01d"), (int)(osvi.dwUnderlyingMinorVersion / 10));
		_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (osvi.dwUnderlyingBuildNumber)
		{
			//Report the UBR on Windows 10 / Server 2016 and later
			if (os.IsWindows10OrWindowsServer2016(&osvi, TRUE) || os.IsWindows11(&osvi, TRUE) || os.IsWindowsServer2019(&osvi, TRUE) || os.IsWindowsServer2022(&osvi, TRUE) || os.IsWindowsServer2025(&osvi, TRUE))
				_stprintf(sBuf, _T(" Build:%d.%d"), (int)(osvi.dwUnderlyingBuildNumber), (int)(osvi.dwUBR));
			else
				_stprintf(sBuf, _T(" Build:%d"), (int)(osvi.dwUnderlyingBuildNumber));
			_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		}
		if (osvi.wUnderlyingServicePackMajor)
		{
			if (osvi.wUnderlyingServicePackMinor)
			{
				//Handle the special case of NT 4 SP 6a which Dtwinver treats as SP 6.1
				if (os.IsNTPreWin2k(&osvi, TRUE) && (osvi.wUnderlyingServicePackMajor == 6) && (osvi.wUnderlyingServicePackMinor == 1))
					_stprintf(sBuf, _T(" Service Pack: 6a"));
				//Handle the special case of XP SP 1a which Dtwinver treats as SP 1.1
				else if (os.IsWindowsXP(&osvi, TRUE) && (osvi.wUnderlyingServicePackMajor == 1) && (osvi.wUnderlyingServicePackMinor == 1))
					_stprintf(sBuf, _T(" Service Pack: 1a"));
				else
					_stprintf(sBuf, _T(" Service Pack:%d.%d"), (int)(osvi.wUnderlyingServicePackMajor), (int)(osvi.wUnderlyingServicePackMinor));
			}
			else
				_stprintf(sBuf, _T(" Service Pack:%d"), (int)(osvi.wUnderlyingServicePackMajor));
			_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		}
		else
		{
			if (osvi.wUnderlyingServicePackMinor)
				_stprintf(sBuf, _T(" Service Pack:0.%d"), (int)(osvi.wUnderlyingServicePackMinor));
		}
#if defined(COSVERSION_WIN32) || defined(COSVERSION_WIN64)
		if (osvi.ullUAPInfo)
		{
			_stprintf(sBuf, _T(", RtlGetDeviceFamilyInfo UAPInfo:%u.%u.%u.%u"), (DWORD)((osvi.ullUAPInfo & 0xFFFF000000000000ui64) >> 48), (DWORD)((osvi.ullUAPInfo & 0x0000FFFF00000000ui64) >> 32), (DWORD)((osvi.ullUAPInfo & 0x00000000FFFF0000ui64) >> 16), (DWORD)(osvi.ullUAPInfo & 0x000000000000FFFFui64)); //NOLINT(clang-diagnostic-format)
			_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		}
#endif //#if defined(COSVERSION_WIN32) || defined(COSVERSION_WIN64)
		if (osvi.ulDeviceFamily)
		{
			_stprintf(sBuf, _T(", Device Family:0x%08X"), osvi.ulDeviceFamily); //NOLINT(clang-diagnostic-format)
			_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
			switch (osvi.ulDeviceFamily)
			{
				case DEVICEFAMILYINFOENUM_WINDOWS_8X:
				{
					_tcscat(sText, _T(", Windows 8x")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_WINDOWS_PHONE_8X:
				{
					_tcscat(sText, _T(", Windows Phone 8x")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_DESKTOP:
				{
					_tcscat(sText, _T(", Desktop")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_MOBILE:
				{
					_tcscat(sText, _T(", Mobile")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_XBOX:
				{
					_tcscat(sText, _T(", Xbox")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_TEAM:
				{
					_tcscat(sText, _T(", Team")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_IOT:
				{
					_tcscat(sText, _T(", IoT")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_IOT_HEADLESS:
				{
					_tcscat(sText, _T(", IoT Headless")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_SERVER:
				{
					_tcscat(sText, _T(", Server")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_HOLOGRAPHIC:
				{
					_tcscat(sText, _T(", Holographic")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_XBOXSRA:
				{
					_tcscat(sText, _T(", Xbox SRA")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_XBOXERA:
				{
					_tcscat(sText, _T(", Xbox ERA")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_SERVER_NANO:
				{
					_tcscat(sText, _T(", Server Nano")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_8828080:
				{
					_tcscat(sText, _T(", \"8828080\"")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_7067329:
				{
					_tcscat(sText, _T(", \"7067329\"")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_WINDOWS_CORE:
				{
					_tcscat(sText, _T(", Windows Core")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYINFOENUM_WINDOWS_CORE_HEADLESS:
				{
					_tcscat(sText, _T(", Windows Core Headless")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				default:
				{
					break;
				}
			}
		}
		if (osvi.ulDeviceForm)
		{
			_stprintf(sBuf, _T(", Device Form:0x%08X"), osvi.ulDeviceForm); //NOLINT(clang-diagnostic-format)
			_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
			switch (osvi.ulDeviceForm)
			{
				case DEVICEFAMILYDEVICEFORM_PHONE:
				{
					_tcscat(sText, _T(", Phone")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_TABLET:
				{
					_tcscat(sText, _T(", Tablet")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_DESKTOP:
				{
					_tcscat(sText, _T(", Desktop")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_NOTEBOOK:
				{
					_tcscat(sText, _T(", Notebook")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_CONVERTIBLE:
				{
					_tcscat(sText, _T(", Convertible")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_DETACHABLE:
				{
					_tcscat(sText, _T(", Detachable")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_ALLINONE:
				{
					_tcscat(sText, _T(", All In One")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_STICKPC:
				{
					_tcscat(sText, _T(", Stick PC")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_PUCK:
				{
					_tcscat(sText, _T(", Puck")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_LARGESCREEN:
				{
					_tcscat(sText, _T(", Large Screen")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_HMD:
				{
					_tcscat(sText, _T(", Head Mounted Display")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_INDUSTRY_HANDHELD:
				{
					_tcscat(sText, _T(", Handheld")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_INDUSTRY_TABLET:
				{
					_tcscat(sText, _T(", Industry Tablet")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_BANKING:
				{
					_tcscat(sText, _T(", Banking")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_BUILDING_AUTOMATION:
				{
					_tcscat(sText, _T(", Automation")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_DIGITAL_SIGNAGE:
				{
					_tcscat(sText, _T(", Digital Signage")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_GAMING:
				{
					_tcscat(sText, _T(", Gaming")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_HOME_AUTOMATION:
				{
					_tcscat(sText, _T(", Home Automation")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_INDUSTRIAL_AUTOMATION:
				{
					_tcscat(sText, _T(", Industrial Automation")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_KIOSK:
				{
					_tcscat(sText, _T(", Kiosk")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_MAKER_BOARD:
				{
					_tcscat(sText, _T(", Maker Board")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_MEDICAL:
				{
					_tcscat(sText, _T(", Medical")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_NETWORKING:
				{
					_tcscat(sText, _T(", Networking")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_POINT_OF_SERVICE:
				{
					_tcscat(sText, _T(", Point Of Service")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_PRINTING:
				{
					_tcscat(sText, _T(", Printing")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_THIN_CLIENT:
				{
					_tcscat(sText, _T(", Thin Client")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_TOY:
				{
					_tcscat(sText, _T(", Toy")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_VENDING:
				{
					_tcscat(sText, _T(", Vending")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_INDUSTRY_OTHER:
				{
					_tcscat(sText, _T(", Other Industry")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_ONE:
				{
					_tcscat(sText, _T(", XBox One")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_ONE_S:
				{
					_tcscat(sText, _T(", XBox One S")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_ONE_X:
				{
					_tcscat(sText, _T(", XBox One X")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_ONE_X_DEVKIT:
				{
					_tcscat(sText, _T(", XBox One X Devkit")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_SERIES_X:
				{
					_tcscat(sText, _T(", XBox Series X")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_SERIES_X_DEVKIT:
				{
					_tcscat(sText, _T(", XBox Series X Devkit")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_SERIES_S:
				{
					_tcscat(sText, _T(", XBox Series S")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_01:
				{
					_tcscat(sText, _T(", XBox Reserved 01")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_02:
				{
					_tcscat(sText, _T(", XBox Reserved 02")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_03:
				{
					_tcscat(sText, _T(", XBox Reserved 03")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_04:
				{
					_tcscat(sText, _T(", XBox Reserved 04")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_05:
				{
					_tcscat(sText, _T(", XBox Reserved 05")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_06:
				{
					_tcscat(sText, _T(", XBox Reserved 06")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_07:
				{
					_tcscat(sText, _T(", XBox Reserved 07")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_08:
				{
					_tcscat(sText, _T(", XBox Reserved 08")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				case DEVICEFAMILYDEVICEFORM_XBOX_RESERVED_09:
				{
					_tcscat(sText, _T(", XBox Reserved 09")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
					break;
				}
				default:
				{
					break;
				}
			}
		}
		_stprintf(sBuf, _T(", ProductType:0x%08X"), osvi.dwProductType); //NOLINT(clang-diagnostic-format)
		_tcscat(sText, sBuf); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsEnterpriseStorageServer(&osvi))
			_tcscat(sText, _T(", (Storage Server Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsExpressStorageServer(&osvi))
			_tcscat(sText, _T(", (Storage Server Express)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsStandardStorageServer(&osvi))
			_tcscat(sText, _T(", (Storage Server Standard)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsWorkgroupStorageServer(&osvi))
			_tcscat(sText, _T(", (Storage Server Workgroup)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsEssentialsStorageServer(&osvi))
			_tcscat(sText, _T(", (Storage Server Essentials)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsStorageServer(&osvi))
			_tcscat(sText, _T(", (Storage Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

		if (os.IsHomeServerPremiumEdition(&osvi))
			_tcscat(sText, _T(", (Home Server Premium Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsHomeServerEdition(&osvi))
			_tcscat(sText, _T(", (Home Server Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

		if (os.IsTerminalServices(&osvi))
			_tcscat(sText, _T(", (Terminal Services)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsEmbedded(&osvi))
			_tcscat(sText, _T(", (Embedded)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsTerminalServicesInRemoteAdminMode(&osvi))
			_tcscat(sText, _T(", (Terminal Services in Remote Admin Mode)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.Is64Bit(&osvi, TRUE))
			_tcscat(sText, _T(", (64 Bit Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsMediaCenter(&osvi))
			_tcscat(sText, _T(", (Media Center Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsTabletPC(&osvi))
			_tcscat(sText, _T(", (Tablet PC Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsComputeClusterServerEdition(&osvi))
			_tcscat(sText, _T(", (Compute Cluster Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsServerFoundation(&osvi))
			_tcscat(sText, _T(", (Foundation Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsMultipointServerPremiumEdition(&osvi))
			_tcscat(sText, _T(", (MultiPoint Premium Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsMultiPointServer(&osvi))
			_tcscat(sText, _T(", (MultiPoint Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsSecurityAppliance(&osvi))
			_tcscat(sText, _T(", (Security Appliance)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsBackOffice(&osvi))
			_tcscat(sText, _T(", (BackOffice)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsNEdition(&osvi))
			_tcscat(sText, _T(", (N Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsEEdition(&osvi))
			_tcscat(sText, _T(", (E Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsHyperVTools(&osvi))
			_tcscat(sText, _T(", (Hyper-V Tools)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsHyperVServer(&osvi))
			_tcscat(sText, _T(", (Hyper-V Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsServerCore(&osvi))
			_tcscat(sText, _T(", (Server Core)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsUniprocessorFree(&osvi))
			_tcscat(sText, _T(", (Uniprocessor Free)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsUniprocessorChecked(&osvi))
			_tcscat(sText, _T(", (Uniprocessor Checked)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsMultiprocessorFree(&osvi))
			_tcscat(sText, _T(", (Multiprocessor Free)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsMultiprocessorChecked(&osvi))
			_tcscat(sText, _T(", (Multiprocessor Checked)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsEssentialBusinessServerManagement(&osvi))
			_tcscat(sText, _T(", (Windows Essential Business Server Management Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsEssentialBusinessServerMessaging(&osvi))
			_tcscat(sText, _T(", (Windows Essential Business Server Messaging Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsEssentialBusinessServerSecurity(&osvi))
			_tcscat(sText, _T(", (Windows Essential Business Server Security Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsClusterServer(&osvi))
			_tcscat(sText, _T(", (Cluster Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsSmallBusiness(&osvi))
			_tcscat(sText, _T(", (Small Business)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsRestricted(&osvi))
			_tcscat(sText, _T(", (Restricted)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsSmallBusinessServerPremium(&osvi))
			_tcscat(sText, _T(", (Small Business Server Premium)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsServerForSBSolutions(&osvi))
			_tcscat(sText, _T(", (Server for SB Solutions)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsServerSolutions(&osvi))
			_tcscat(sText, _T(", (Server Solutions)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsPreRelease(&osvi))
			_tcscat(sText, _T(", (Prerelease)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsEvaluation(&osvi))
			_tcscat(sText, _T(", (Evaluation)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsAutomotive(&osvi))
			_tcscat(sText, _T(", (Automotive)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsChina(&osvi))
			_tcscat(sText, _T(", (China)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsSingleLanguage(&osvi))
			_tcscat(sText, _T(", (Single Language)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsWin32sInstalled(&osvi))
			_tcscat(sText, _T(", (Win32s)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsEducation(&osvi))
			_tcscat(sText, _T(", (Education)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsIndustry(&osvi))
			_tcscat(sText, _T(", (Industry)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsStudent(&osvi))
			_tcscat(sText, _T(", (Student)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsMobile(&osvi))
			_tcscat(sText, _T(", (Mobile)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsCloudHostInfrastructureServer(&osvi))
			_tcscat(sText, _T(", (Cloud Host Infrastructure Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsLTSB(&osvi))
			_tcscat(sText, _T(", (LTSB / LTSC)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsCloudStorageServer(&osvi))
			_tcscat(sText, _T(", (Cloud Storage Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsPPIPro(&osvi))
			_tcscat(sText, _T(", (PPI Pro)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsConnectedCar(&osvi))
			_tcscat(sText, _T(", (Connected Car)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsHandheld(&osvi))
			_tcscat(sText, _T(", (Handheld)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsHolographic(&osvi))
			_tcscat(sText, _T(", (Holographic)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsBusiness(&osvi))
			_tcscat(sText, _T(", (Business)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsSubscription(&osvi))
			_tcscat(sText, _T(", (Subscription)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsUtilityVM(&osvi))
			_tcscat(sText, _T(", (Utility VM)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsAzure(&osvi))
			_tcscat(sText, _T(", (Azure)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsEnterpriseG(&osvi))
			_tcscat(sText, _T(", (Enterprise G)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsServerRDSH(&osvi))
			_tcscat(sText, _T(", (ServerRRDSH)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsHubOS(&osvi))
			_tcscat(sText, _T(", (HubOS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsCommunicationsServer(&osvi))
			_tcscat(sText, _T(", (Communications Server)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsOneCoreUpdateOS(&osvi))
			_tcscat(sText, _T(", (One Core Update OS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsAndromeda(&osvi))
			_tcscat(sText, _T(", (Andromeda)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsIoTCommercial(&osvi))
			_tcscat(sText, _T(", (IoT Commercial)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsIoTCore(&osvi))
			_tcscat(sText, _T(", (IoT Core)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsIoTOS(&osvi))
			_tcscat(sText, _T(", (IoT OS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsIoTEdgeOS(&osvi))
			_tcscat(sText, _T(", (IoT EdgeOS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsIoTEnterpriseSK(&osvi))
			_tcscat(sText, _T(", (IoT Enterprise LTSC Subscription)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsIoTEnterpriseK(&osvi))
			_tcscat(sText, _T(", (IoT Enterprise Subscription)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsIoTEnterpriseEvaluation(&osvi))
			_tcscat(sText, _T(", (IoT Enterprise Evaluation)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		else if (os.IsIoTEnterprise(&osvi))
			_tcscat(sText, _T(", (IoT Enterprise)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsValidation(&osvi))
			_tcscat(sText, _T(", (Validation)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsXBoxSystemOS(&osvi))
			_tcscat(sText, _T(", (XBox SystemOS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsXBoxNativeOS(&osvi))
			_tcscat(sText, _T(", (XBox NativeOS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsXBoxGamesOS(&osvi))
			_tcscat(sText, _T(", (XBox GameOS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsXBoxEraOS(&osvi))
			_tcscat(sText, _T(", (XBox EraOS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsXBoxDurangoHostOS(&osvi))
			_tcscat(sText, _T(", (XBox DurangoHostOS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsXBoxScarlettHostOS(&osvi))
			_tcscat(sText, _T(", (XBox ScarlettHostOS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsXBoxKeystone(&osvi))
			_tcscat(sText, _T(", (XBox Keystone)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsAzureStackHCIServerCore(&osvi))
			_tcscat(sText, _T(", (Azure Stack HCI Server Core)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsAzureServerAgentBridge(&osvi))
			_tcscat(sText, _T(", (Azure Server Agent Bridge)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsAzureServerNanoHost(&osvi))
			_tcscat(sText, _T(", (Azure Server Nano Host)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsDatacenterServerAzureEdition(&osvi))
			_tcscat(sText, _T(", (Datacenter Server Azure Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsDatacenterServerCoreAzureEdition(&osvi))
			_tcscat(sText, _T(", (Datacenter Server Core Azure Edition)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsAzureServerCloudhost(&osvi))
			_tcscat(sText, _T(", (Azure Server Cloud Host)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsAzureServerCloudMOS(&osvi))
			_tcscat(sText, _T(", (Azure Server Cloud MOS)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsWindows365(&osvi))
			_tcscat(sText, _T(", (Windows 365)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsWNC(&osvi))
			_tcscat(sText, _T(", (WNC)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		if (os.IsUnlicensed(&osvi))
			_tcscat(sText, _T(", (Unlicensed)")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

		_tcscat(sText, _T("\n")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)

		//Some extra info for CE
#ifdef UNDER_CE
		if (osvi.UnderlyingPlatform == COSVersion::WindowsCE)
		{
			_tcscat(sText, _T("Model: ")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
			_tcscat(sText, osvi.szOEMInfo); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
			_tcscat(sText, _T("\nDevice Type: ")); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
			_tcscat(sText, osvi.szPlatformType); //NOLINT(clang-analyzer-security.insecureAPI.strcpy)
		}
#endif //#ifdef UNDER_CE
	}
	else
		_stprintf(sText, _T("Failed in call to GetOSVersion\n"));

#ifdef _WINDOWS
	MessageBox(sText, _T("Operating System details"), MB_OK); //NOLINT(modernize-use-nullptr)
#elif _WIN32_WCE
	MessageBox(sText, _T("Operating System details"), MB_OK); //NOLINT(modernize-use-nullptr)
#else
	printf("%s", sText);
#endif //#ifdef _WINDOWS
}
