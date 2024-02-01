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

#include "SetupAPI.h"
#include "cfgmgr32.h"

// CEnumDevicesDlg dialog

class CEnumDevicesDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CEnumDevicesDlg)

public:
	CEnumDevicesDlg(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CEnumDevicesDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ENUMDEVICES_DIALOG };
#endif
	CListCtrl m_ctrlDevices;
public:
	HMACHINE m_hMachine;
	SP_CLASSIMAGELIST_DATA m_pImageListData;
	CImageList m_pImageList;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	void EnumDevices();
	void RetrieveSubNodes(DEVINST parent, DEVINST sibling, DEVNODE dn);
	CString GetDeviceName(DEVNODE DevNode);

	DECLARE_MESSAGE_MAP()
};
