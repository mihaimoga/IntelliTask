/* This file is part of IntelliTask application developed by Stefan-Mihai MOGA.

IntelliTask is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliTask is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliTask. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// EnumDevicesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliTask.h"
#include "afxdialogex.h"
#include "EnumDevicesDlg.h"

#pragma comment(lib, "Cfgmgr32.lib")
#pragma comment(lib, "Setupapi.lib")
#pragma comment(lib, "Rpcrt4.lib")

// CEnumDevicesDlg dialog

IMPLEMENT_DYNAMIC(CEnumDevicesDlg, CDialogEx)

CEnumDevicesDlg::CEnumDevicesDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ENUMDEVICES_DIALOG, pParent)
{
}

CEnumDevicesDlg::~CEnumDevicesDlg()
{
}

void CEnumDevicesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_DEVICES, m_ctrlDevices);
}

BEGIN_MESSAGE_MAP(CEnumDevicesDlg, CDialogEx)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CEnumDevicesDlg message handlers

BOOL CEnumDevicesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	EnumDevices();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CEnumDevicesDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	SetupDiDestroyClassImageList(&m_pImageListData);
}

void CEnumDevicesDlg::EnumDevices()
{
	TCHAR LocalComputer[MAX_PATH];
	DWORD Size = MAX_PATH - 2;

	GetComputerName(LocalComputer + 2, &Size);
	LocalComputer[0] = _T('\\');
	LocalComputer[1] = _T('\\');

	CONFIGRET cr;
	cr = CM_Connect_Machine(LocalComputer, &m_hMachine);

	if (cr != CR_SUCCESS)
	{
		TCHAR Text[MAX_PATH];
		wsprintf(Text, _T("Machine Connection failed, cr= %lx(hex)\n"), cr);
		::MessageBox(m_hWnd, Text, _T("Error"), MB_ICONSTOP | MB_OK);
		return;
	}

	//Set Image List 
	m_pImageListData.cbSize = sizeof(m_pImageListData);
	SetupDiGetClassImageList(&m_pImageListData);

	m_pImageList.Attach(m_pImageListData.ImageList);
	m_ctrlDevices.SetImageList(&m_pImageList, LVSIL_SMALL);
	m_ctrlDevices.SetImageList(&m_pImageList, LVSIL_NORMAL);

	DEVNODE dnRoot;
	CM_Locate_DevNode_Ex(&dnRoot, NULL, 0, m_hMachine);

	DEVNODE dnFirst;
	CM_Get_Child_Ex(&dnFirst, dnRoot, 0, m_hMachine);

	RetrieveSubNodes(dnRoot, NULL, dnFirst);

	CM_Disconnect_Machine(m_hMachine);
}

void CEnumDevicesDlg::RetrieveSubNodes(DEVINST parent, DEVINST sibling, DEVNODE dn)
{
	DEVNODE dnSibling, dnChild;

	do
	{
		CONFIGRET cr = CM_Get_Sibling_Ex(&dnSibling, dn, 0, m_hMachine);

		if (cr != CR_SUCCESS)
			dnSibling = NULL;

		TCHAR GuidString[MAX_GUID_STRING_LEN];
		ULONG Size = sizeof(GuidString);

		cr = CM_Get_DevNode_Registry_Property_Ex(dn, CM_DRP_CLASSGUID, NULL,
			GuidString, &Size, 0, m_hMachine);

		if (cr == CR_SUCCESS)
		{
			GUID Guid;
			GuidString[MAX_GUID_STRING_LEN - 2] = _T('\0');
			UuidFromString((RPC_WSTR)&GuidString[1], &Guid);
			int Index;
			if (SetupDiGetClassImageIndex(&m_pImageListData, &Guid, &Index))
			{
				CString DeviceName = GetDeviceName(dn);
				m_ctrlDevices.InsertItem(m_ctrlDevices.GetItemCount(), DeviceName, Index);
			}
		}

		cr = CM_Get_Child_Ex(&dnChild, dn, 0, m_hMachine);
		if (cr == CR_SUCCESS)
		{
			RetrieveSubNodes(dn, NULL, dnChild);
		}

		dn = dnSibling;

	} while (dn != NULL);

}


CString CEnumDevicesDlg::GetDeviceName(DEVNODE DevNode)
{
	CString	strType;
	CString	strValue;
	CString DisplayName;
	LPTSTR	Buffer;

	int  BufferSize = MAX_PATH + MAX_DEVICE_ID_LEN;
	ULONG  BufferLen = BufferSize * sizeof(TCHAR);

	Buffer = strValue.GetBuffer(BufferSize);
	if (CR_SUCCESS == CM_Get_DevNode_Registry_Property_Ex(DevNode,
		CM_DRP_FRIENDLYNAME, NULL,
		Buffer, &BufferLen, 0, m_hMachine))
	{
		DisplayName = Buffer;
	}
	else
	{
		BufferLen = BufferSize * sizeof(TCHAR);

		if (CR_SUCCESS == CM_Get_DevNode_Registry_Property_Ex(DevNode,
			CM_DRP_DEVICEDESC, NULL,
			Buffer, &BufferLen, 0, m_hMachine))
		{
			DisplayName = Buffer;
		}
		else
		{
			DisplayName = _T("Unknown Device");
		}
	}

	return DisplayName;
}
