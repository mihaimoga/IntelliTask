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

// EnumDevicesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliTask.h"
#include "afxdialogex.h"
#include "EnumDevicesDlg.h"

// Link required system libraries for device enumeration
#pragma comment(lib, "Cfgmgr32.lib")  // Configuration Manager API
#pragma comment(lib, "Setupapi.lib")  // Setup API for device installation
#pragma comment(lib, "Rpcrt4.lib")    // RPC runtime for GUID operations

/**
 * @brief Implementation of the Enumerate Devices dialog
 */

IMPLEMENT_DYNAMIC(CEnumDevicesDlg, CDialogEx)

/**
 * @brief Constructor for the Enumerate Devices dialog
 * @param pParent Pointer to the parent window (default: nullptr)
 */
CEnumDevicesDlg::CEnumDevicesDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ENUMDEVICES_DIALOG, pParent)
{
}

/**
 * @brief Destructor for the Enumerate Devices dialog
 */
CEnumDevicesDlg::~CEnumDevicesDlg()
{
}

/**
 * @brief Exchange data between dialog controls and member variables
 * @param pDX Pointer to the CDataExchange object
 */
void CEnumDevicesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	// Map tree view and list view controls to member variables
	DDX_Control(pDX, IDC_DEVICES, m_ctrlDevices);
	DDX_Control(pDX, IDC_DETAILS, m_ctrlDetails);
}

BEGIN_MESSAGE_MAP(CEnumDevicesDlg, CDialogEx)
	ON_WM_DESTROY()
	ON_NOTIFY(TVN_SELCHANGED, IDC_DEVICES, &CEnumDevicesDlg::OnSelchangedDevices)
END_MESSAGE_MAP()

/**
 * @brief Forward declaration for ListView column insertion helper function
 */
void ListViewInsertColumnText(HWND hListView, const DWORD wIdx,
	int wFmt, const TCHAR* pszText, const BOOL bFinal, HWND hDlg);

/**
 * @brief Initialize the dialog and enumerate all devices
 * @return TRUE unless focus is set to a control
 */
BOOL CEnumDevicesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set up the details ListView with two columns
	ListViewInsertColumnText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 0, 0, _T("Field"), 0, GetSafeHwnd());
	ListViewInsertColumnText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 1, 0, _T("Descritption"), 1, GetSafeHwnd());

	// Enumerate all devices and populate the tree view
	EnumDevices();

	// Set up window resizing behavior for controls
	VERIFY(m_pWindowResizer.Hook(this));
	// Devices tree stays on left, resizes vertically
	VERIFY(m_pWindowResizer.SetAnchor(IDC_DEVICES, ANCHOR_LEFT | ANCHOR_TOP | ANCHOR_BOTTOM));
	// Details list resizes both horizontally and vertically
	VERIFY(m_pWindowResizer.SetAnchor(IDC_DETAILS, ANCHOR_LEFT | ANCHOR_RIGHT | ANCHOR_TOP | ANCHOR_BOTTOM));
	// Cancel button stays at bottom right
	VERIFY(m_pWindowResizer.SetAnchor(IDCANCEL, ANCHOR_RIGHT | ANCHOR_BOTTOM));

	// Restore previous window size if available
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

/**
 * @brief Clean up resources and save window size on dialog destruction
 */
void CEnumDevicesDlg::OnDestroy()
{
	// Free all allocated device list nodes
	FreeAllocDeviceNode();
	// Free all device order nodes
	FreeAllDeviceOrderNode();

	CDialogEx::OnDestroy();

	// Save current window dimensions for next time
	RECT pWndRect;
	GetWindowRect(&pWndRect);
	const int nWidth = pWndRect.right - pWndRect.left;
	const int nHeight = pWndRect.bottom - pWndRect.top;
	theApp.WriteInt(_T("Width"), nWidth);
	theApp.WriteInt(_T("Height"), nHeight);
}

/**
 * @brief Convert a GUID structure to its string representation
 * @param guid The GUID to convert
 * @param pData Output buffer to receive the GUID string
 */
void ConvertGUIDToString(const GUID guid, TCHAR* pData)
{
	TCHAR szData[30] = { 0 };
	TCHAR szTmp[3] = { 0 };
	DWORD wLoop;

	// Format the first three parts of the GUID (Data1-Data3)
	_stprintf(pData, _T("%04X-%02X-%02X-"), guid.Data1, guid.Data2, guid.Data3);

	// Format the Data4 array (8 bytes) as hex pairs
	for (wLoop = 0; wLoop < 8; wLoop++)
	{
		// Add separator after the first 2 bytes of Data4
		if (wLoop == 2)
			_tcscat(szData, _T("-"));
		_stprintf(szTmp, _T("%02X"), guid.Data4[wLoop]);
		_tcscat(szData, szTmp);
	};
	// Append the formatted Data4 string to the output
	_tcscpy(pData + _tcslen(pData), szData);
};

/**
 * @brief Display a system error message in a message box
 * @param hWnd Parent window handle for the message box
 * @param dwErrorCode System error code to format and display
 * @param szFunctionName Name of the function where the error occurred
 */
void ShowErrorMsg(HWND hWnd, const DWORD dwErrorCode, const TCHAR* szFunctionName)
{
	void* lpMsgBuf;

	// Format the system error message from the error code
	if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		0L,
		dwErrorCode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(TCHAR*)&lpMsgBuf,
		0,
		0L))
		return;

	// Display the error message in a message box
	MessageBox(hWnd, (const TCHAR*)lpMsgBuf, szFunctionName, MB_ICONSTOP | MB_OK);
	// Free the buffer allocated by FormatMessage
	LocalFree(lpMsgBuf);
};

/**
 * @brief Delete an item or all items from a ListView control
 * @param hList Handle to the ListView control
 * @param wItem Index of the item to delete, or -1 to delete all items
 */
void ListViewDeleteItem(HWND hList, const DWORD wItem)
{
	if (wItem == -1)
		SendMessage(hList, LVM_DELETEALLITEMS, 0, 0);
	else
		SendMessage(hList, LVM_DELETEITEM, (WPARAM)wItem, 0);
};

/**
 * @brief Set extended styles for a ListView control
 * @param hListView Handle to the ListView control
 * @param dwStyle Style flags to set, or 0 for default styles
 */
void ListViewSetExtStyle(HWND hListView, const DWORD dwStyle)
{
	DWORD style;
	//
	style = (dwStyle) ? dwStyle :
		LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_FLATSB;
	SendMessage(hListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)style);
};

/**
 * @brief Select a specific item in a ListView control
 * @param hListView Handle to the ListView control
 * @param iItem Index of the item to select
 */
void ListViewSetSelectItem(HWND hListView, const DWORD iItem)
{
	LVITEM lvItem;
	lvItem.iItem = iItem;
	lvItem.mask = LVIF_STATE;
	lvItem.state = LVIS_SELECTED;
	SendMessage(hListView, LVM_SETITEMSTATE, iItem, (LPARAM)&lvItem);
};

/**
 * @brief Insert a column with text into a ListView control
 * @param hListView Handle to the ListView control
 * @param wIdx Column index where to insert
 * @param wFmt Column format (alignment), or 0 for default left alignment
 * @param pszText Column header text
 * @param bFinal TRUE if this is the last column (uses wider width)
 * @param hDlg Handle to the parent dialog for text measurement
 */
void ListViewInsertColumnText(HWND hListView, const DWORD wIdx,
	int wFmt, const TCHAR* pszText, const BOOL bFinal, HWND hDlg)
{
	LVCOLUMN   column;
	TEXTMETRIC text;
	int        nWidth;
	HDC        hDC = GetDC(hDlg);

	// Calculate column width based on average character width
	GetTextMetrics(hDC, &text);
	nWidth = text.tmAveCharWidth * 20;
	ReleaseDC(hDlg, hDC);

	// Default to left alignment if not specified
	if (!wFmt)
		wFmt = LVCFMT_LEFT;

	// Set up column structure
	column.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	column.pszText = (LPWSTR)pszText;
	column.fmt = wFmt;
	// Final column gets extra width to fill remaining space
	if (!bFinal)
		column.cx = nWidth;
	else
		column.cx = nWidth * 20;

	// Insert the column and handle errors
	if (SendMessage(hListView, LVM_INSERTCOLUMN, wIdx, (LPARAM)(LPLVCOLUMN)&column) == -1)
		ShowErrorMsg(hDlg, GetLastError(), _T("InitialListView"));
};

/**
 * @brief Add a column with image and text to a ListView control
 * @param hWnd Handle to the parent window
 * @param nID Control ID of the ListView
 * @param wIdx Column index where to insert
 * @param wFmt Column format (alignment), or 0 for default left alignment
 * @param pszText Column header text
 * @param bFinal TRUE if this is the last column (uses wider width)
 */
void ListViewAddColumnImageText(HWND hWnd, const UINT nID, const DWORD wIdx,
	int wFmt, TCHAR* pszText, const BOOL bFinal)
{
	LVCOLUMN   column;
	TEXTMETRIC text;
	int        nWidth;
	HWND       hListView = GetDlgItem(hWnd, nID);
	HDC        hDC = GetDC(hWnd);
	// int        fmt;

	GetTextMetrics(hDC, &text);
	nWidth = text.tmAveCharWidth * 20;
	ReleaseDC(hWnd, hDC);
	//
	if (!wFmt)
		wFmt = LVCFMT_LEFT;
	column.mask = LVCF_TEXT | LVCF_FMT | LVCF_WIDTH | LVCF_IMAGE;
	column.pszText = pszText;
	column.fmt = wFmt;
	if (!bFinal)
		column.cx = nWidth;
	else
		column.cx = 700;
	if (SendMessage(hListView, LVM_INSERTCOLUMN, wIdx, (LPARAM)(LPLVCOLUMN)&column) == -1)
		ShowErrorMsg(hWnd, GetLastError(), _T("InitialListView"));
};

/**
 * @brief Insert or set text for a ListView item or subitem
 * @param hListView Handle to the ListView control
 * @param iItem Item index
 * @param iSubItem Subitem (column) index
 * @param pszText Text to insert or set
 */
void ListViewInsertItemText(HWND hListView, const int iItem,
	const int iSubItem, const TCHAR* pszText)
{
	LVITEM item;
	//
	item.mask = LVIF_TEXT;
	item.iItem = iItem;
	item.iSubItem = iSubItem;
	item.state = 0;
	item.stateMask = 0;
	item.iImage = 0;
	item.lParam = 0;
	item.pszText = (LPWSTR)pszText;
	item.cchTextMax = (int)_tcslen(pszText);
	if (!iSubItem)
		SendMessage(hListView, LVM_INSERTITEM, 0, (LPARAM)&item);
	else
		SendMessage(hListView, LVM_SETITEMTEXT, iItem, (LPARAM)&item);
};

/**
 * @brief Get the index of the currently selected item in a ListView
 * @param hListView Handle to the ListView control
 * @return Index of the selected item, or -1 if no item is selected
 */
int ListViewGetItemSelect(HWND hListView)
{
	return (int)SendMessage(hListView, LVM_GETNEXTITEM, (WPARAM) - 1, MAKELPARAM((UINT)LVNI_SELECTED, 0));
};

/**
 * @brief Get the total number of items in a ListView
 * @param hListView Handle to the ListView control
 * @return Number of items in the ListView
 */
int ListViewGetItemCount(HWND hListView)
{
	return (int)SendMessage(hListView, LVM_GETITEMCOUNT, 0, 0);
};

/**
 * @brief Get the text of a specific subitem from the selected item
 * @param hListView Handle to the ListView control
 * @param iSubItem Subitem (column) index
 * @param pszText Output buffer to receive the text
 */
void ListViewGetItemText(HWND hListView, const int iSubItem, TCHAR* pszText)
{
	LVITEM item;
	int    idx = ListViewGetItemSelect(hListView);
	//
	if (idx == -1)
		return;
	//
	item.iSubItem = iSubItem;
	item.pszText = pszText;
	item.cchTextMax = 255;
	SendMessage(hListView, LVM_GETITEMTEXT, idx, (LPARAM)&item);
};

/**
 * @brief Get the text of a specific item and subitem
 * @param hListView Handle to the ListView control
 * @param iItem Item index
 * @param iSubItem Subitem (column) index
 * @param pszText Output buffer to receive the text
 */
void ListViewGetSpecItem(HWND hListView, const int iItem, const int iSubItem, TCHAR* pszText)
{
	LVITEM item;
	//
	if (iItem == -1)// || iItem == 0)
		return;
	//
	item.iSubItem = iSubItem;
	item.pszText = pszText;
	item.cchTextMax = 255;
	SendMessage(hListView, LVM_GETITEMTEXT, iItem, (LPARAM)&item);
};

/**
 * @brief Check if a notification is a ListView double-click event
 * @param nID Control ID to check
 * @param pnmh Pointer to the notification message header
 * @return The control ID if it's a double-click event, 0 otherwise
 */
UINT IsListViewDBClkEvent(const UINT nID, NMHDR* pnmh)
{
	if (pnmh->idFrom == nID)
	{
		if (pnmh->code == NM_DBLCLK)
			return nID;
	};
	return 0;
};

/**
 * @brief Check if a notification is a ListView click event (left or right)
 * @param nID Control ID to check
 * @param pnmh Pointer to the notification message header
 * @return NM_CLICK for left click, NM_RCLICK for right click, 0 otherwise
 */
UINT IsListViewClkEvent(const UINT nID, NMHDR* pnmh)
{
	if (pnmh->idFrom == nID)
	{
		if (pnmh->code == NM_CLICK)
			return NM_CLICK;
		else if (pnmh->code == NM_RCLICK)
			return NM_RCLICK;
	};
	return 0;
};

/**
 * @brief Remove all items from a ListView control
 * @param hListView Handle to the ListView control
 */
void ListViewRemoveAllItems(HWND hListView)
{
	LRESULT wCnt = SendMessage(hListView, LVM_GETITEMCOUNT, 0, 0);
	DWORD wLoop;
	//
	for (wLoop = 0; wLoop < wCnt; wLoop++)
		SendMessage(hListView, LVM_DELETEITEM, 0, 0);
	SendMessage(hListView, LVM_DELETEALLITEMS, 0, 0);
};

/**
 * @brief Remove a specific item from a ListView control
 * @param hDlg Handle to the parent dialog
 * @param nID Control ID of the ListView
 * @param iItem Index of the item to remove
 */
void ListViewRemoveItem(HWND hDlg, const UINT nID, const int iItem)
{
	SendMessage(GetDlgItem(hDlg, nID), LVM_DELETEITEM, (WPARAM)iItem, 0);
};

/**
 * @brief Remove a specific column from a ListView control
 * @param hDlg Handle to the parent dialog
 * @param nID Control ID of the ListView
 * @param iCol Index of the column to remove
 */
void ListViewRemoveColumn(HWND hDlg, const UINT nID, const int iCol)
{
	SendMessage(GetDlgItem(hDlg, nID), LVM_DELETECOLUMN, (WPARAM)iCol, 0);
};

/**
 * @brief Remove all nodes from a TreeView control
 * @param hDlg Handle to the parent dialog
 * @param nIdTree Control ID of the TreeView
 */
void TreeViewRemoveAllNodes(HWND hDlg, const UINT nIdTree)
{
	HWND  hTree = GetDlgItem(hDlg, nIdTree);
	LRESULT wCnt = SendMessage(hDlg, TVM_GETCOUNT, 0, 0);
	DWORD wLoop;
	for (wLoop = 0; wLoop < wCnt; wLoop)
		SendMessage(hTree, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
	SendMessage(hTree, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
};

/**
 * @brief Global device class image list data
 */
SP_CLASSIMAGELIST_DATA _spImageData = { 0 };

/**
 * @brief Initialize the device class image list
 * 
 * Destroys any existing class image list and retrieves a new one from the system.
 */
void InitialImageData()
{
	// HIMAGELIST hImageListView = 0L;
	SetupDiDestroyClassImageList(&_spImageData);
	RtlZeroMemory(&_spImageData, sizeof(SP_CLASSIMAGELIST_DATA));
	_spImageData.cbSize = sizeof(SP_CLASSIMAGELIST_DATA);
	SetupDiGetClassImageList(&_spImageData);
};

/**
 * @brief Create the root tree item for the device tree
 * @param spImageData Device class image list data
 * @param nIdTree Control ID of the TreeView
 * @param nIdBmp Bitmap resource ID for the root icon
 * @param hDlg Handle to the parent dialog
 * @return Handle to the root tree item, or NULL on failure
 */
HTREEITEM MakeDeviceRootTree(SP_CLASSIMAGELIST_DATA spImageData, const UINT nIdTree, const UINT nIdBmp, HWND hDlg)
{
	TVINSERTSTRUCT tvStruct = { 0 };
	// HIMAGELIST     hImageListTree = 0L;
	HTREEITEM      hTreeItem = 0L;
	HBITMAP        hBitmap = 0L;
	DWORD          nIndex = MAX_PATH - 1;
	TCHAR          szName[MAX_PATH] = { 0 };
	//
	GetComputerName(szName, &nIndex);
	hBitmap = LoadBitmap(nullptr, MAKEINTRESOURCE(nIdBmp));
	nIndex = ImageList_Add(spImageData.ImageList, hBitmap, 0L);
	DeleteObject(hBitmap);
	SendMessage(GetDlgItem(hDlg, nIdTree), TVM_SETIMAGELIST,
		TVSIL_NORMAL, (LPARAM)spImageData.ImageList);
	if (ImageList_GetImageCount(spImageData.ImageList) != -1)
	{
		tvStruct.hParent = 0L;
		tvStruct.hInsertAfter = TVI_ROOT;
		tvStruct.item.mask = TVIF_IMAGE | TVIF_TEXT | TVIF_SELECTEDIMAGE;
		tvStruct.item.pszText = szName;
		tvStruct.item.iImage = nIndex;
		tvStruct.item.iSelectedImage = nIndex;
		hTreeItem = (HTREEITEM)SendMessage(GetDlgItem(hDlg, nIdTree),
			TVM_INSERTITEM, 0, (LPARAM)&tvStruct);
		return hTreeItem;
	};
	return 0;
};

/**
 * @brief Global pointer to the head of the device list
 */
DEVICE_LIST* _pHead = 0L;

/**
 * @brief Global pointer to the head of the device order list
 */
DEVICE_ORDER* _pOrderHead = 0L;

DEVICE_ORDER* AllocNewDeviceOrderNode(HWND hDlg);

/**
 * @brief Initialize the device order list
 * @param hDlg Handle to the parent dialog
 * @return 1 on success, 0 on failure
 */
char InitialDeviceOrder(HWND hDlg)
{
	_pOrderHead = AllocNewDeviceOrderNode(hDlg);
	return (_pOrderHead) ? 1 : 0;
};

/**
 * @brief Allocate a new device order node
 * @param hDlg Handle to the parent dialog for error messages
 * @return Pointer to the new node, or NULL on failure
 */
DEVICE_ORDER* AllocNewDeviceOrderNode(HWND hDlg)
{
	DEVICE_ORDER* pNew = (DEVICE_ORDER*)LocalAlloc(LPTR, sizeof(DEVICE_ORDER));
	//
	if (!pNew)
	{
		ShowErrorMsg(hDlg, GetLastError(), _T("LocalAlloc"));
		return 0;
	};
	RtlZeroMemory(pNew->szDevName, sizeof(TCHAR) * LINE_LEN);
	pNew->pNext = 0L;
	return pNew;
};

/**
 * @brief Add a new device order node to the list
 * @param szDevName Device name to store in the node
 * @param hDlg Handle to the parent dialog for error messages
 * @return 1 on success, 0 on failure
 */
char AddNewDeviceOrderNode(const TCHAR* szDevName, HWND hDlg)
{
	// Allocate a new device order node
	DEVICE_ORDER* pAdd = AllocNewDeviceOrderNode(hDlg);

	if (!pAdd)
		return 0;

	// Copy device name into the node
	_tcscpy(pAdd->szDevName, szDevName);
	// Insert at the head of the list (after the dummy head node)
	pAdd->pNext = _pOrderHead->pNext;
	_pOrderHead->pNext = pAdd;
	return 1;
};

/**
 * @brief Find the order/count of a device by name
 * @param szName Device name to search for
 * @return Number of times the device name appears in the order list
 */
DWORD FindDeviceOrder(const TCHAR* szName)
{
	DEVICE_ORDER* pList = _pOrderHead->pNext;
	DWORD        wOrder = 0;

	// Traverse the list and count how many times this device name appears
	// This determines the order index for devices with the same class
	while (pList)
	{
		if (!_tcscmp(pList->szDevName, szName))
			wOrder++;
		pList = pList->pNext;
	};
	return wOrder;
};

/**
 * @brief Free all device order nodes and cleanup the list
 */
void FreeAllDeviceOrderNode()
{
	DEVICE_ORDER* pDel = _pOrderHead->pNext;
	DEVICE_ORDER* pTmp = 0L;

	// Traverse the list and free each node
	while (pDel->pNext)
	{
		pTmp = pDel;
		pDel = pDel->pNext;
		LocalFree(pTmp);
	};
	// Free the dummy head node
	LocalFree(_pOrderHead);
	_pOrderHead = 0L;
};

/**
 * @brief Initialize the device list
 * @param hDlg Handle to the parent dialog for error messages
 * @return 1 on success, 0 on failure
 */
char InitialDeviceList(HWND hDlg)
{
	_pHead = AllocNewDeviceNode(hDlg);
	return (_pHead) ? 1 : 0;
};

/**
 * @brief Allocate a new device list node
 * @param hDlg Handle to the parent dialog for error messages
 * @return Pointer to the new node, or NULL on failure
 */
DEVICE_LIST* AllocNewDeviceNode(HWND hDlg)
{
	DEVICE_LIST* pNew = (DEVICE_LIST*)LocalAlloc(LPTR, sizeof(DEVICE_LIST));
	//
	if (!pNew)
	{
		ShowErrorMsg(hDlg, GetLastError(), _T("LocalAlloc"));
		return 0;
	};
	RtlZeroMemory(&pNew->guid, sizeof(GUID));
	RtlZeroMemory(pNew->szInstallID, sizeof(char) * LINE_LEN);
	RtlZeroMemory(pNew->szName, sizeof(char) * MAX_PATH);
	RtlZeroMemory(pNew->szPath, sizeof(char) * MAX_PATH);
	pNew->wOrder = (DWORD) - 1;
	pNew->wIndex = (DWORD) - 1;
	pNew->pNext = 0L;
	return pNew;
};

/**
 * @brief Add a new device node to the device list
 * @param guid Device class GUID
 * @param szName Device name
 * @param szInstallID Device installation ID
 * @param szPath Device path
 * @param wIndex Device index
 * @param wOrder Device order
 * @param hDlg Handle to the parent dialog for error messages
 * @return 1 on success, 0 on failure
 */
char AddNewDeviceNode(const GUID guid,
	const TCHAR* szName,
	const TCHAR* szInstallID,
	const TCHAR* szPath,
	const DWORD wIndex,
	const DWORD wOrder,
	HWND hDlg)
{
	DEVICE_LIST* pAdd = AllocNewDeviceNode(hDlg);
	//
	if (!pAdd)
		return 0;
	memcpy(&pAdd->guid, &guid, sizeof(GUID));
	_tcscpy(pAdd->szInstallID, szInstallID);
	_tcscpy(pAdd->szName, szName);
	_tcscpy(pAdd->szPath, szPath);
	pAdd->wIndex = wIndex;
	pAdd->wOrder = wOrder;
	pAdd->pNext = _pHead->pNext;
	_pHead->pNext = pAdd;
	return 1;
};

/**
 * @brief Free all device nodes and cleanup the device list
 */
void FreeAllocDeviceNode()
{
	DEVICE_LIST* pDel = _pHead->pNext;
	DEVICE_LIST* pTmp = 0L;
	//
	while (pDel->pNext)
	{
		pTmp = pDel;
		pDel = pDel->pNext;
		LocalFree(pTmp);
	};
	LocalFree(_pHead);
	_pHead = 0L;
};

/**
 * @brief Get and display detailed information about a device
 * @param pList Pointer to the device list node
 * @param hDlg Handle to the parent dialog
 */
void GetDeviceDetailInfo(DEVICE_LIST* pList, HWND hDlg)
{
	TCHAR szBuf[MAX_PATH] = { 0 };
	HWND  hList = GetDlgItem(hDlg, IDC_DETAILS);

	ConvertGUIDToString(pList->guid, szBuf);
	ListViewInsertItemText(hList, 0, 1, szBuf);
	ListViewInsertItemText(hList, 2, 1, pList->szInstallID);
	_stprintf(szBuf, _T("%d"), pList->wIndex);
	ListViewInsertItemText(hList, 10, 1, szBuf);
	ListViewInsertItemText(hList, 11, 1, pList->szPath);
};

/**
 * @brief Display driver detail information in the tree view
 * @param hTreeChild Handle to the parent tree item
 * @param nID Control ID of the TreeView
 * @param szBuf Text to display
 * @param iImageIdx Image index for the item
 * @param iSelectImage Selected image index (unused)
 * @param hDlg Handle to the parent dialog
 */
void DisplayDriverDetailInfo(HTREEITEM hTreeChild, const UINT nID, const TCHAR* szBuf, const int iImageIdx, const int /*iSelectImage*/, HWND hDlg)
{
	TVINSERTSTRUCT tvStruct = { 0 };
	HWND           hTree = GetDlgItem(hDlg, nID);

	tvStruct.hParent = hTreeChild;
	tvStruct.hInsertAfter = TVI_LAST;
	tvStruct.item.mask = TVIF_IMAGE | TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_HANDLE;
	tvStruct.item.mask |= TVIF_PARAM;
	tvStruct.item.lParam = 1;
	tvStruct.item.pszText = (LPWSTR)szBuf;
	tvStruct.item.iImage = iImageIdx;
	tvStruct.item.iSelectedImage = iImageIdx;
	SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvStruct);
};

/**
 * @brief Get the device instance ID
 * @param hDevInfo Device information set handle
 * @param pspDevInfoData Pointer to device information data
 * @param szInstanceID Output buffer for the instance ID
 * @param hDlg Handle to the parent dialog for error messages
 */
void GetDeviceInstanceID(HDEVINFO hDevInfo,
	SP_DEVINFO_DATA* pspDevInfoData,
	TCHAR* szInstanceID, HWND hDlg)
{
	if (!SetupDiGetDeviceInstanceId(hDevInfo,
		pspDevInfoData,
		szInstanceID,
		LINE_LEN,
		0))
		ShowErrorMsg(hDlg, GetLastError(), _T("SetupDiBuildDriverInfoList"));
};

/**
 * @brief Insert text into a TreeView as a root or child item
 * @param hWnd Handle to the parent window
 * @param nID Control ID of the TreeView
 * @param hParent Handle to the parent tree item, or NULL for root
 * @param hAfter Handle to the item after which to insert, or NULL
 * @param pszText Text to display in the tree item
 * @return Handle to the newly inserted tree item
 */
HTREEITEM TreeViewInsertRootText(HWND hWnd, UINT nID, HTREEITEM hParent, HTREEITEM hAfter, TCHAR* pszText)
{
	TVINSERTSTRUCT tvInsert;
	HWND           hTree = GetDlgItem(hWnd, nID);
	//
	if (!hParent && !hAfter)
	{
		tvInsert.hParent = 0L;
		tvInsert.hInsertAfter = 0L;
		tvInsert.item.mask = TVIF_TEXT;
		tvInsert.item.pszText = (LPWSTR)pszText;
		return (HTREEITEM)SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);
	};
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = hAfter;
	tvInsert.item.mask = TVIF_TEXT;
	tvInsert.item.pszText = pszText;
	return (HTREEITEM)SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);
};

/**
 * @brief Find a child item in a TreeView by text
 * @param hTreeView Handle to the TreeView control
 * @param hRoot Handle to the root item to search from
 * @param szBuf Text to search for
 * @return Handle to the found tree item, or NULL if not found
 */
HTREEITEM TreeViewFindChild(HWND hTreeView, HTREEITEM hRoot,
	const TCHAR* szBuf)
{
	HTREEITEM hChildItem = 0L;
	HTREEITEM hParentItem = hRoot;
	TCHAR     szText[128] = { 0 };
	TVITEM    tvItem = { 0 };
	int       iLoop;
	int       nIndex;

	tvItem.mask = TVIF_TEXT | TVIF_HANDLE;
	tvItem.pszText = szText;
	tvItem.cchTextMax = sizeof(char) * 127;
	nIndex = (int)SendMessage(hTreeView, TVM_GETCOUNT, 0, 0);
	RtlZeroMemory(szText, sizeof(char) * 128);
	for (iLoop = 0; iLoop < nIndex; iLoop++)
	{
		hChildItem = (HTREEITEM)SendMessage(hTreeView,
			TVM_GETNEXTITEM,
			(iLoop) ? TVGN_NEXT : TVGN_CHILD,
			(iLoop) ? (LPARAM)hParentItem : (LPARAM)hRoot);
		tvItem.hItem = hChildItem;
		SendMessage(hTreeView, TVM_GETITEM, 0, (LPARAM)&tvItem);
		if (*szText)
		{
			if (!_tcscmp(szBuf, szText))
				return hChildItem;
		};
		hParentItem = hChildItem;
	};
	return 0;
};

/**
 * @brief Check if a notification is a TreeView click event
 * @param nID Control ID to check
 * @param pnmh Pointer to the notification message header
 * @return NM_CLICK for left click, NM_RCLICK for right click, 0 otherwise
 */
UINT IsTreeViewClkEvent(const UINT nID, NMHDR* pnmh)
{
	if (pnmh->idFrom == nID)
	{
		if (pnmh->code == NM_CLICK)
			return NM_CLICK;
		else if (pnmh->code == NM_RCLICK)
			return NM_RCLICK;
	};
	return 0;
};

/**
 * @brief Check if a notification is a TreeView selection changed event
 * @param nID Control ID to check
 * @param pnmh Pointer to the notification message header
 * @return TVN_SELCHANGED if selection changed, 0 otherwise
 */
UINT IsTreeViewSelectChanged(const UINT nID, NMHDR* pnmh)
{
	if (pnmh->code == TVN_SELCHANGED && pnmh->idFrom == nID)
		return TVN_SELCHANGED;
	return 0;
};

/**
 * @brief Expand or collapse a TreeView item
 * @param hTree Handle to the TreeView control
 * @param hTreeItem Handle to the tree item
 * @param bExpand TRUE to expand, FALSE to collapse
 */
void TreeViewExpand(HWND hTree, HTREEITEM hTreeItem, const BOOL bExpand)
{
	UINT flags = (bExpand) ? TVE_EXPAND : TVE_COLLAPSE;
	//
	SendMessage(hTree, TVM_EXPAND, (WPARAM)flags, (LPARAM)hTreeItem);
};

/**
 * @brief Get the text of the selected TreeView item
 * @param hTree Handle to the TreeView control
 * @param pnmh Pointer to the notification message header
 * @param pItem Pointer to TVITEM structure to receive the item information
 */
void TreeViewGetSelectText(HWND hTree, NMHDR* pnmh, TVITEM* pItem)
{
	NMTREEVIEW* pnmView = (NMTREEVIEW*)pnmh;
	TCHAR       pszText[MAX_PATH] = { 0 };
	//
	pItem->mask = TVIF_TEXT;
	pItem->hItem = pnmView->itemNew.hItem;
	do
	{
		pItem->pszText = pszText;
		pItem->cchTextMax = MAX_PATH - 1;
		//        SendMessage(hTree, TVM_GETNEXTITEM, TVGN_CARET, (LPARAM)pItem);
		SendMessage(hTree, TVM_GETITEM, 0, (LPARAM)pItem);
		if (_tcslen(pItem->pszText))
			break;
	} while (1);
};

/**
 * @brief Enumerate all WDM (Windows Driver Model) drivers and populate the device tree
 * @param nIdTree Control ID of the TreeView
 * @param nIdBmp Bitmap resource ID for the root icon
 * @param hDlg Handle to the parent dialog
 * @return 1 on success, 0 on failure
 */
char EnumWDMDriver(const UINT nIdTree, const UINT nIdBmp, HWND hDlg)
{
	HDEVINFO        hDevInfo = 0L;
	SP_DEVINFO_DATA spDevInfoData = { 0 };
	DWORD           wIndex = 0;
	HTREEITEM       hTreeChild = 0L;
	//
	hTreeChild = MakeDeviceRootTree(_spImageData, nIdTree, nIdBmp, hDlg);
	if (!hTreeChild)
		return 0;
	//
	hDevInfo = SetupDiGetClassDevs(0L, 0L, hDlg, DIGCF_PRESENT |
		DIGCF_ALLCLASSES | DIGCF_PROFILE);
	if (hDevInfo == (void*)-1)
	{
		ShowErrorMsg(hDlg, GetLastError(), _T("SetupDiGetClassDevs"));
		return 0;
	};
	//
	wIndex = 0;
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	SendMessage(GetDlgItem(hDlg, nIdTree), TVM_SETIMAGELIST,
		TVSIL_NORMAL, (LPARAM)_spImageData.ImageList);
	//
	while (1)
	{
		if (SetupDiEnumDeviceInfo(hDevInfo,
			wIndex,
			&spDevInfoData))
		{
			TCHAR szBuf[MAX_PATH] = { 0 };
			int wImageIdx = 0;
			// DWORD wItem = 0;

			if (!SetupDiGetDeviceRegistryProperty(hDevInfo,
				&spDevInfoData,
				SPDRP_CLASS, //SPDRP_DEVICEDESC,
				0L,
				(PBYTE)szBuf,
				2048,
				0))
			{
				wIndex++;
				continue;
			};
			//
			if (SetupDiGetClassImageIndex(&_spImageData,
				&spDevInfoData.ClassGuid,
				&wImageIdx))
			{
				TVINSERTSTRUCT         tvStruct = { 0 };
				HWND                   hTree = GetDlgItem(hDlg, nIdTree);
				TCHAR                  szName[64] = { 0 };
				TCHAR                  szID[LINE_LEN] = { 0 };
				TCHAR                  szPath[MAX_PATH] = { 0 };
				HTREEITEM              hItem;
				DWORD                  dwRequireSize;
				DWORD                  wOrder;
				//
				if (!SetupDiGetClassDescription(&spDevInfoData.ClassGuid,
					szBuf,
					MAX_PATH,
					&dwRequireSize))
				{
					wIndex++;
					continue;
				};
				wOrder = FindDeviceOrder(szBuf);
				if (!AddNewDeviceOrderNode(szBuf, hDlg))
				{
					wIndex++;
					continue;
				};
				hItem = TreeViewFindChild(hTree, hTreeChild, szBuf);
				if (!hItem)
				{
					tvStruct.hParent = hTreeChild;
					tvStruct.hInsertAfter = TVI_LAST;
					tvStruct.item.mask = TVIF_IMAGE | TVIF_TEXT | TVIF_SELECTEDIMAGE;
					tvStruct.item.mask |= TVIF_PARAM;
					tvStruct.item.lParam = 1;
					tvStruct.item.pszText = szBuf;
					tvStruct.item.iImage = wImageIdx;
					tvStruct.item.iSelectedImage = wImageIdx;
					hItem = (HTREEITEM)SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvStruct);
					wOrder = 0;
				};
				GetDeviceInstanceID(hDevInfo, &spDevInfoData, szID, hDlg);
				// GetDeviceInterfaceInfo(hDevInfo, spDevInfoData, szPath, hDlg);

				if (SetupDiGetDeviceRegistryProperty(hDevInfo,
					&spDevInfoData,
					SPDRP_FRIENDLYNAME,
					0L,
					(PBYTE)szName,
					63,
					0))
				{
					DisplayDriverDetailInfo(hItem, nIdTree, szName, wImageIdx, wImageIdx, hDlg);
					AddNewDeviceNode(spDevInfoData.ClassGuid, szName, szID, szPath, wIndex, wOrder, hDlg);
				}
				else if (SetupDiGetDeviceRegistryProperty(hDevInfo,
					&spDevInfoData,
					SPDRP_DEVICEDESC,
					0L,
					(PBYTE)szName,
					63,
					0))
				{
					DisplayDriverDetailInfo(hItem, nIdTree, szName, wImageIdx, wImageIdx, hDlg);
					AddNewDeviceNode(spDevInfoData.ClassGuid, szName, szID, szPath, wIndex, wOrder, hDlg);
				};
			};
		}
		else
			break;
		wIndex++;
	};
	SendMessage(GetDlgItem(hDlg, nIdTree), TVM_EXPAND, TVE_EXPAND, (LPARAM)hTreeChild);
	SendMessage(GetDlgItem(hDlg, nIdTree), TVM_SORTCHILDREN, 0, (LPARAM)hTreeChild);
	TreeView_SetItemState(GetDlgItem(hDlg, nIdTree), hTreeChild, TVIS_SELECTED, TVIS_SELECTED);
	SetupDiDestroyDeviceInfoList(hDevInfo);
	return 1;
};

/**
 * @brief Get and display memory resource information for a device
 * @param pMemDes Pointer to memory resource descriptor
 * @param ulSize Size of the resource data
 * @param nID Control ID of the ListView to display results
 * @param hDlg Handle to the parent dialog
 */
void GetMemoryResource(MEM_DES* pMemDes, const ULONG ulSize, const UINT nID, HWND hDlg)
{
	TCHAR szBuf[128] = { 0 };
	HWND  hListView = GetDlgItem(hDlg, nID);
	DWORD wLoop = 0;
	DWORD wCnt = ListViewGetItemCount(hListView);

	_stprintf(szBuf, _T("%08X - "), (unsigned int)pMemDes->MD_Alloc_Base);
	_stprintf(szBuf + _tcslen(szBuf), _T("%08X"), (unsigned int)pMemDes->MD_Alloc_End);
	ListViewInsertItemText(hListView, wCnt, 0, _T("Memory Range"));
	ListViewInsertItemText(hListView, wCnt, 1, szBuf);

	if (pMemDes->MD_Count)
	{
		for (wLoop = 0; wLoop < (ulSize -
			(LONG)(sizeof(MEM_DES) / pMemDes->MD_Type)); wLoop++)
		{
			MEM_RANGE* pMemRange = (MEM_RANGE*)(pMemDes + 1);// + pMemDes->MD_Type);
			if (pMemRange->MR_Min != 0 && pMemRange->MR_Max != 0 &&
				(pMemRange->MR_Min < pMemRange->MR_Max))
			{
				_stprintf(szBuf, _T("%08X - "), (unsigned int)pMemRange->MR_Min);
				_stprintf(szBuf + _tcslen(szBuf), _T("%08X"), (unsigned int)pMemRange->MR_Max);
				ListViewInsertItemText(hListView, wCnt + wLoop + 1, 0, _T("Memory Range"));
				ListViewInsertItemText(hListView, wCnt + wLoop + 1, 1, szBuf);
			};
		};
	};
};

/**
 * @brief Get and display I/O port resource information for a device
 * @param pIODes Pointer to I/O resource descriptor
 * @param ulSize Size of the resource data
 * @param nID Control ID of the ListView to display results
 * @param hDlg Handle to the parent dialog
 */
void GetIOResource(IO_DES* pIODes, const ULONG ulSize, const UINT nID, HWND hDlg)
{
	TCHAR szBuf[128] = { 0 };
	HWND  hListView = GetDlgItem(hDlg, nID);
	DWORD wLoop = 0;
	DWORD wCnt = ListViewGetItemCount(hListView);

	_stprintf(szBuf, _T("%04X - "), (unsigned int)pIODes->IOD_Alloc_Base);
	_stprintf(szBuf + _tcslen(szBuf), _T("%04X"), (unsigned int)pIODes->IOD_Alloc_End);
	ListViewInsertItemText(hListView, wCnt, 0, _T("IO Range"));
	ListViewInsertItemText(hListView, wCnt, 1, szBuf);

	if (pIODes->IOD_Count)
	{
		for (wLoop = 0; wLoop < (ulSize -
			(ULONG)(sizeof(IO_DES) / pIODes->IOD_Type)); wLoop++)
		{
			IO_RANGE* pIORange = (IO_RANGE*)(pIODes + 1); // + (wLoop * pIODes->IOD_Type));
			if (pIORange->IOR_Min != 0 && pIORange->IOR_Max != 0 &&
				(pIORange->IOR_Min < pIORange->IOR_Max))
			{
				_stprintf(szBuf, _T("%04X - "), (unsigned int)pIORange->IOR_Min);
				_stprintf(szBuf + _tcslen(szBuf), _T("%04X"), (unsigned int)pIORange->IOR_Max);
				ListViewInsertItemText(hListView, wCnt, 0, _T("IO Range"));
				ListViewInsertItemText(hListView, wCnt + wLoop + 1, 1, szBuf);
			};
		};
	};
};

/**
 * @brief Get and display DMA (Direct Memory Access) resource information for a device
 * @param pDMADes Pointer to DMA resource descriptor
 * @param ulSize Size of the resource data
 * @param nID Control ID of the ListView to display results
 * @param hDlg Handle to the parent dialog
 */
void GetDMAResource(DMA_DES* pDMADes, const ULONG ulSize, const UINT nID, HWND hDlg)
{
	TCHAR szBuf[128] = { 0 };
	HWND  hListView = GetDlgItem(hDlg, nID);
	DWORD wLoop = 0;
	DWORD wCnt = ListViewGetItemCount(hListView);

	_stprintf(szBuf, _T("%02d"), pDMADes->DD_Alloc_Chan);
	ListViewInsertItemText(hListView, wCnt, 0, _T("DMA"));
	ListViewInsertItemText(hListView, wCnt, 1, szBuf);

	if (pDMADes->DD_Count)
	{
		for (wLoop = 0; wLoop < (ulSize -
			(ULONG)(sizeof(DMA_DES) / pDMADes->DD_Type)); wLoop++)
		{
			DMA_RANGE* pDMARange = (DMA_RANGE*)(pDMADes + 1); // + (wLoop * pIODes->IOD_Type));
			if (pDMARange->DR_Min != 0 && pDMARange->DR_Max != 0 &&
				(pDMARange->DR_Min < pDMARange->DR_Max))
			{
				_stprintf(szBuf, _T("%02d - "), pDMARange->DR_Min);
				_stprintf(szBuf + _tcslen(szBuf), _T("%02d"), pDMARange->DR_Max);
				ListViewInsertItemText(hListView, wCnt + wLoop + 1, 0, _T("DMA"));
				ListViewInsertItemText(hListView, wCnt + wLoop + 1, 1, szBuf);
			};
		};
	};
};

/**
 * @brief Get and display IRQ (Interrupt Request) resource information for a device
 * @param pIRQDes Pointer to IRQ resource descriptor
 * @param ulSize Size of the resource data
 * @param nID Control ID of the ListView to display results
 * @param hDlg Handle to the parent dialog
 */
void GetIRQResource(IRQ_DES* pIRQDes, const ULONG ulSize, const UINT nID, HWND hDlg)
{
	TCHAR szBuf[128] = { 0 };
	HWND  hListView = GetDlgItem(hDlg, nID);
	DWORD wLoop = 0;
	DWORD wCnt = ListViewGetItemCount(hListView);

	_stprintf(szBuf, _T("%02d"), pIRQDes->IRQD_Alloc_Num);
	ListViewInsertItemText(hListView, wCnt, 0, _T("IRQ"));
	ListViewInsertItemText(hListView, wCnt, 1, szBuf);

	if (pIRQDes->IRQD_Count)
	{
		for (wLoop = 0; wLoop < (ulSize -
			(ULONG)(sizeof(IRQ_DES) / pIRQDes->IRQD_Type)); wLoop++)
		{
			IRQ_RANGE* pIRQRange = (IRQ_RANGE*)(pIRQDes + 1); // + (wLoop * pIODes->IOD_Type));
			if (pIRQRange->IRQR_Min != 0 && pIRQRange->IRQR_Max != 0 &&
				(pIRQRange->IRQR_Min < pIRQRange->IRQR_Max))
			{
				_stprintf(szBuf, _T("%02d - "), pIRQRange->IRQR_Min);
				_stprintf(szBuf + _tcslen(szBuf), _T("%02d"), pIRQRange->IRQR_Max);
				ListViewInsertItemText(hListView, wCnt + wLoop + 1, 0, _T("IRQ"));
				ListViewInsertItemText(hListView, wCnt + wLoop + 1, 1, szBuf);
			};
		};
	};
};

/**
 * @brief Find and display specific hardware resource types for a device
 * @param DevInst Device instance handle
 * @param dwResType Resource type (Memory, I/O, DMA, or IRQ)
 * @param wOrder Device order (unused)
 * @param nID Control ID of the ListView to display results
 * @param hDlg Handle to the parent dialog
 */
void FindSpecResource(const DEVINST DevInst, const DWORD dwResType,
	const DWORD /*wOrder*/, const UINT nID, HWND hDlg)
{
	char* pBuf = 0L;
	DWORD wHeaderSize;
	ULONG ulSize;
	//
	CONFIGRET cmRet;
	LOG_CONF  firstLogConf;
	LOG_CONF  nextLogConf;
	RES_DES   rdPrevResDes;
	//
	switch (dwResType)
	{
		case ResType_Mem:
			wHeaderSize = sizeof(MEM_DES);
			break;
		case ResType_IO:
			wHeaderSize = sizeof(IO_DES);
			break;
		case ResType_DMA:
			wHeaderSize = sizeof(DMA_DES);
			break;
		case ResType_IRQ:
			wHeaderSize = sizeof(IRQ_DES);
			break;
	};
	//
	cmRet = CM_Get_First_Log_Conf(&firstLogConf, DevInst, ALLOC_LOG_CONF);
	if (cmRet != CR_SUCCESS)
	{
		cmRet = CM_Get_First_Log_Conf(&firstLogConf, DevInst, BOOT_LOG_CONF);
		if (cmRet != CR_SUCCESS)
			return;
	};
	//
	cmRet = CM_Get_Next_Res_Des(&nextLogConf,
		firstLogConf,
		dwResType,
		0L,
		0L);
	if (cmRet != CR_SUCCESS)
	{
		CM_Free_Res_Des_Handle(firstLogConf);
		return;
	};
	//
	while (1)
	{
		//
		ulSize = 0;
		cmRet = CM_Get_Res_Des_Data_Size(&ulSize, nextLogConf, 0L);
		if (cmRet != CR_SUCCESS)
		{
			CM_Free_Res_Des_Handle(nextLogConf);
			break;
		};
		//
		pBuf = (char*)LocalAlloc(LPTR, 2048);
		if (!pBuf)
		{
			ShowErrorMsg(hDlg, GetLastError(), _T("LocalAlloc"));
			CM_Free_Res_Des_Handle(nextLogConf);
			break;
		};
		//
		cmRet = CM_Get_Res_Des_Data(nextLogConf, pBuf, ulSize, 0L);
		if (cmRet != CR_SUCCESS)
		{
			CM_Free_Res_Des_Handle(nextLogConf);
			LocalFree(pBuf);
			break;
		};
		//
		switch (dwResType)
		{
			case ResType_Mem:
				GetMemoryResource((MEM_DES*)pBuf, ulSize, nID, hDlg);
				break;
			case ResType_IO:
				GetIOResource((IO_DES*)pBuf, ulSize, nID, hDlg);
				break;
			case ResType_DMA:
				GetDMAResource((DMA_DES*)pBuf, ulSize, nID, hDlg);
				break;
			case ResType_IRQ:
				GetIRQResource((IRQ_DES*)pBuf, ulSize, nID, hDlg);
				break;
		};
		rdPrevResDes = 0;
		cmRet = CM_Get_Next_Res_Des(&rdPrevResDes,
			nextLogConf,
			dwResType,
			0L,
			0L);
		LocalFree(pBuf);
		if (cmRet != CR_SUCCESS)
			break;
		else
		{
			CM_Free_Res_Des_Handle(nextLogConf);
			nextLogConf = rdPrevResDes;
		};
	};
	CM_Free_Res_Des_Handle(nextLogConf);
};

/**
 * @brief Get and display comprehensive device information
 * @param guid Device class GUID
 * @param wOrder Device order index
 * @param nIDList1 Control ID of the ListView to display results
 * @param hDlg Handle to the parent dialog
 */
void GetOtherInfo(GUID guid, const DWORD wOrder, const UINT nIDList1, HWND hDlg)
{
	HDEVINFO         hDevInfo = 0L;
	SP_DEVINFO_DATA  spDevInfoData = { 0 };

	hDevInfo = SetupDiGetClassDevs(&guid, 0L, hDlg, DIGCF_PRESENT);
	if (hDevInfo == (void*)-1)
	{
		ShowErrorMsg(hDlg, GetLastError(), _T("SetupDiGetClassDevs"));
		return;
	};

	RtlZeroMemory(&spDevInfoData, sizeof(spDevInfoData));
	spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
	if (SetupDiEnumDeviceInfo(hDevInfo,
		wOrder,
		&spDevInfoData))
	{
		SP_DRVINFO_DATA        spDrvInfoData = { 0 };
		SP_DRVINFO_DETAIL_DATA spDrvInfoDetail[2] = {0};
		TCHAR                  HardwareID[256] = { 0 };
		HWND                   hList = GetDlgItem(hDlg, nIDList1);
		DWORD                  dwRequireSize;
		DWORD                  wIdx;
		
		RtlZeroMemory(&spDrvInfoData, sizeof(spDrvInfoData));
		// RtlZeroMemory(&spDrvInfoDetail, sizeof(spDrvInfoDetail));
		GetMoreInformation(hDevInfo, &spDevInfoData, nIDList1, hDlg);
		/* Show Resource Information
		for (wIdx = ResType_Mem; wIdx <= ResType_IRQ; wIdx++)
			FindSpecResource(spDevInfoData.DevInst,
				wIdx, wOrder, nIDList2, hDlg);*/
		//
		if (!SetupDiBuildDriverInfoList(hDevInfo,
			&spDevInfoData,
			SPDIT_COMPATDRIVER))
			ShowErrorMsg(hDlg, GetLastError(), _T("SetupDiBuildDriverInfoList"));
		wIdx = 0;
		while (1)
		{
			spDrvInfoData.cbSize = sizeof(SP_DRVINFO_DATA);
			if (SetupDiEnumDriverInfo(hDevInfo,
				&spDevInfoData,
				SPDIT_COMPATDRIVER,
				wIdx++,
				&spDrvInfoData))
			{
				// BYTE szBuf[2048] = { 0 };
				// RtlZeroMemory(szBuf, sizeof(szBuf));
				RtlZeroMemory(&spDrvInfoDetail, sizeof(spDrvInfoDetail));
				spDrvInfoDetail[0].cbSize = sizeof(SP_DRVINFO_DETAIL_DATA);
				dwRequireSize = 0;
				if (SetupDiGetDriverInfoDetail(hDevInfo,
					&spDevInfoData,
					&spDrvInfoData,
					&spDrvInfoDetail[0],
					sizeof(spDrvInfoDetail),
					&dwRequireSize))
				{
					SYSTEMTIME sysTime = {0};
					TCHAR      szTmp[64] = { 0 };
					_tcscpy(HardwareID, spDrvInfoDetail[0].HardwareID);
					ListViewInsertItemText(hList, 1, 1, HardwareID);
					ListViewInsertItemText(hList, 3, 1, spDrvInfoData.MfgName);
					ListViewInsertItemText(hList, 4, 1, spDrvInfoData.ProviderName);
					ListViewInsertItemText(hList, 5, 1, spDrvInfoData.Description);
					FileTimeToSystemTime(&spDrvInfoData.DriverDate, &sysTime);
					_stprintf(szTmp, _T("%02d/%02d/%04d"), sysTime.wMonth,
						sysTime.wDay, sysTime.wYear);
					ListViewInsertItemText(hList, 7, 1, szTmp);
					ListViewInsertItemText(hList, 6, 1, spDrvInfoDetail[0].SectionName);
					ListViewInsertItemText(hList, 8, 1, spDrvInfoDetail[0].InfFileName);
				}
				else
					RtlZeroMemory(HardwareID, sizeof(HardwareID));
			}
			else
			{
				DWORD dwError = GetLastError();
				if (dwError != ERROR_NO_MORE_ITEMS)
					ShowErrorMsg(hDlg, dwError, _T("SetupDiEnumDriverInfo"));
				break;
			};
			SetupDiDestroyDriverInfoList(hDevInfo, &spDevInfoData, SPDIT_COMPATDRIVER);
		};
		SetupDiDestroyDeviceInfoList(hDevInfo);
	};
};

/**
 * @brief Get device interface information and path
 * @param hDevInfo Device information set handle
 * @param spDevInfoData Device information data structure
 * @param szPath Output buffer for the device path
 * @param hDlg Handle to the parent dialog for error messages
 */
void GetDeviceInterfaceInfo(HDEVINFO hDevInfo, SP_DEVINFO_DATA spDevInfoData, TCHAR* szPath, HWND hDlg)
{
	SP_DEVICE_INTERFACE_DATA spDevInterfaceData = { 0 };
	//
	spDevInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
	if (!SetupDiCreateDeviceInterface(hDevInfo,
		&spDevInfoData,
		&spDevInfoData.ClassGuid,
		0L,
		0L,
		&spDevInterfaceData))
		ShowErrorMsg(hDlg, GetLastError(), _T("SetupDiBuildDriverInfoList"));
	else
	{
		SP_DEVICE_INTERFACE_DETAIL_DATA* pspDevInterfaceDetail = 0L;
		DWORD                           dwRequire = 0L;
		//
		if (!SetupDiGetDeviceInterfaceDetail(hDevInfo,
			&spDevInterfaceData,
			0L,
			0,
			&dwRequire,
			0L))
		{
			DWORD dwError = GetLastError();
			//
			if (dwError != ERROR_INSUFFICIENT_BUFFER)
			{
				ShowErrorMsg(hDlg, dwError, _T("SetupDiBuildDriverInfoList"));
				return;
			};
		};
		//
		pspDevInterfaceDetail = (SP_DEVICE_INTERFACE_DETAIL_DATA*)LocalAlloc(LPTR,
			sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) * dwRequire);
		pspDevInterfaceDetail->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
		if (!SetupDiGetDeviceInterfaceDetail(hDevInfo,
			&spDevInterfaceData,
			pspDevInterfaceDetail,
			dwRequire,
			&dwRequire,
			0L))
		{
			DWORD dwError = GetLastError();
			//
			if (dwError != ERROR_INSUFFICIENT_BUFFER)
				ShowErrorMsg(hDlg, dwError, _T("SetupDiBuildDriverInfoList"));
		}
		else
		{
			_tcscpy(szPath, pspDevInterfaceDetail->DevicePath);
		};
		//
		if (pspDevInterfaceDetail)
			LocalFree(pspDevInterfaceDetail);
	};
};

/**
 * @brief Display device property information in a ListView
 * @param hListView Handle to the ListView control
 * @param szItemName Property name to display (can be NULL)
 * @param szValue Property value to display (can be NULL)
 */
void ShowDevPropertyInfo(HWND hListView, const TCHAR* szItemName, const TCHAR* szValue)
{
	DWORD  wCount = ListViewGetItemCount(hListView);
	TCHAR  szTmp[64] = { 0 };
	LVITEM lvItem = { 0 };
	//
	lvItem.iSubItem = 1;
	lvItem.pszText = szTmp;
	lvItem.cchTextMax = 63;
	SendMessage(hListView, LVM_GETITEMTEXT, wCount - 1, (LPARAM)&lvItem);
	if (!*szTmp)
	{
		if (szItemName && szValue)
		{
			ListViewInsertItemText(hListView, wCount, 0, szItemName);
			ListViewInsertItemText(hListView, wCount, 1, szValue);
		}
		else if (szItemName && !szValue)
			ListViewInsertItemText(hListView, wCount, 0, szItemName);
		else if (!szItemName && szValue)
			ListViewInsertItemText(hListView, wCount - 1, 1, szValue);
	}
	else
	{
		if (szItemName)
			ListViewInsertItemText(hListView, wCount, 0, szItemName);
		else
			ListViewInsertItemText(hListView, wCount, 0, _T(""));
		if (szValue)
			ListViewInsertItemText(hListView, wCount, 1, szValue);
	};
};

/**
 * @brief Get and display driver name and service information from registry
 * @param hListView Handle to the ListView control to display results
 * @param szServiceName Name of the service to query
 */
void GetDriverName(HWND hListView, const TCHAR* szServiceName)
{
	HKEY  hKey = 0L;
	TCHAR szSubKey[128] = { _T("SYSTEM\\CurrentControlSet\\Services\\\0") };
	TCHAR szPath[MAX_PATH] = { 0 };
	DWORD cbData;
	DWORD dwType;
	DWORD dwStart;
	//
	_tcscat(szSubKey, szServiceName);
	if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		szSubKey,
		0,
		KEY_ALL_ACCESS,
		&hKey) != ERROR_SUCCESS)
	{
		ListViewInsertItemText(hListView, 9, 1, _T("N/A"));
		ListViewInsertItemText(hListView, 12, 1, _T("N/A"));
		ListViewInsertItemText(hListView, 13, 1, _T("N/A"));
		return;
	};
	//
	cbData = MAX_PATH - 1;
	dwType = REG_EXPAND_SZ;
	if (RegQueryValueEx(hKey, _T("ImagePath"), 0L,
		&dwType,
		(unsigned char*)szPath,
		&cbData) != ERROR_SUCCESS)
		ListViewInsertItemText(hListView, 9, 1, _T("N/A"));
	else
	{
		TCHAR szRoot[MAX_PATH] = { 0 };
		//
		GetWindowsDirectory(szRoot, MAX_PATH - 1);
		_tcscat(szRoot, _T("\\"));
		_tcscat(szRoot, szPath);
		ListViewInsertItemText(hListView, 9, 1, szRoot);
	};
	//
	RtlZeroMemory(szPath, MAX_PATH);
	cbData = MAX_PATH - 1;
	dwType = REG_SZ;
	if (RegQueryValueEx(hKey, _T("Group"), 0L,
		&dwType,
		(unsigned char*)szPath,
		&cbData) != ERROR_SUCCESS)
		ListViewInsertItemText(hListView, 12, 1, _T("N/A"));
	else
		ListViewInsertItemText(hListView, 12, 1, szPath);
	//
	dwStart = 0;
	cbData = sizeof(DWORD);
	dwType = REG_DWORD;
	if (RegQueryValueEx(hKey, _T("Start"), 0L,
		&dwType,
		(unsigned char*)&dwStart,
		&cbData) != ERROR_SUCCESS)
		ListViewInsertItemText(hListView, 13, 1, _T("N/A"));
	else
	{
		switch (dwStart)
		{
			case SERVICE_BOOT_START:
				_tcscpy(szPath, _T("BOOT START"));
				break;
			case SERVICE_SYSTEM_START:
				_tcscpy(szPath, _T("SYSTEM START"));
				break;
			case SERVICE_AUTO_START:
				_tcscpy(szPath, _T("AUTO START"));
				break;
			case SERVICE_DEMAND_START:
				_tcscpy(szPath, _T("DEMAND START"));
				break;
			case SERVICE_DISABLED:
				_tcscpy(szPath, _T("DISABLED"));
				break;
		};
		ListViewInsertItemText(hListView, 13, 1, szPath);
	};
	RegCloseKey(hKey);
};

/**
 * @brief Get comprehensive device properties and capabilities
 * @param hDevInfo Device information set handle
 * @param spDevInfoData Pointer to device information data structure
 * @param nIDList1 Control ID of the ListView to display results
 * @param hDlg Handle to the parent dialog
 */
void GetMoreInformation(HDEVINFO hDevInfo, SP_DEVINFO_DATA* spDevInfoData, const UINT nIDList1, HWND hDlg)
{
	HWND  hListView = GetDlgItem(hDlg, nIDList1);
	GUID  guid = { 0 };
	DWORD wCount = ListViewGetItemCount(hListView);
	TCHAR szName[64] = { 0 };
	DWORD dwAddr;

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_BUSNUMBER,
		0L,
		(PBYTE)&dwAddr, //szName,
		sizeof(dwAddr),
		0))
	{
		_stprintf(szName, _T("%X"), dwAddr);
		wCount = ListViewGetItemCount(hListView);
		ListViewInsertItemText(hListView, wCount, 0, _T("BUS Number"));
		ListViewInsertItemText(hListView, wCount, 1, szName);
	};
	//
	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_CAPABILITIES,
		0L,
		(PBYTE)&dwAddr, //szName,
		sizeof(dwAddr),
		0))
	{
		if (dwAddr)
		{
			ShowDevPropertyInfo(hListView, _T("Capability"), 0L);

			if (dwAddr & CM_DEVCAP_LOCKSUPPORTED)
				ShowDevPropertyInfo(hListView, 0L, _T("LockSupported"));
			if (dwAddr & CM_DEVCAP_EJECTSUPPORTED)
				ShowDevPropertyInfo(hListView, 0L, _T("EjectSupported"));
			if (dwAddr & CM_DEVCAP_REMOVABLE)
				ShowDevPropertyInfo(hListView, 0L, _T("Removable"));
			if (dwAddr & CM_DEVCAP_DOCKDEVICE)
				ShowDevPropertyInfo(hListView, 0L, _T("DockDevice"));
			if (dwAddr & CM_DEVCAP_UNIQUEID)
				ShowDevPropertyInfo(hListView, 0L, _T("UniqueID"));
			if (dwAddr & CM_DEVCAP_SILENTINSTALL)
				ShowDevPropertyInfo(hListView, 0L, _T("SilentInstall"));
			if (dwAddr & CM_DEVCAP_RAWDEVICEOK)
				ShowDevPropertyInfo(hListView, 0L, _T("RawDeviceOK"));
			if (dwAddr & CM_DEVCAP_SURPRISEREMOVALOK)
				ShowDevPropertyInfo(hListView, 0L, _T("SurpriseRemovalOK"));
			if (dwAddr & CM_DEVCAP_HARDWAREDISABLED)
				ShowDevPropertyInfo(hListView, 0L, _T("HardwareDisabled"));
			if (dwAddr & CM_DEVCAP_NONDYNAMIC)
				ShowDevPropertyInfo(hListView, 0L, _T("NonDynamic"));
		};
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_CHARACTERISTICS,
		0L,
		(PBYTE)&dwAddr, //szName,
		sizeof(dwAddr),
		0))
	{
		if (dwAddr)
		{
			ShowDevPropertyInfo(hListView, _T("Characters"), 0L);

			if (dwAddr & FILE_DEVICE_SECURE_OPEN)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_SECURE_OPEN"));
			if (dwAddr & FILE_FLOPPY_DISKETTE)
				ShowDevPropertyInfo(hListView, 0L, _T("FLOPPY_DISKETTE"));
			if (dwAddr & FILE_READ_ONLY_DEVICE)
				ShowDevPropertyInfo(hListView, 0L, _T("READ_ONLY_DEVICE"));
			if (dwAddr & FILE_REMOVABLE_MEDIA)
				ShowDevPropertyInfo(hListView, 0L, _T("REMOVABLE_MEDIA"));
			if (dwAddr & FILE_WRITE_ONCE_MEDIA)
				ShowDevPropertyInfo(hListView, 0L, _T("WRITE_ONCE_MEDIA"));
			if (dwAddr & FILE_REMOTE_DEVICE)
				ShowDevPropertyInfo(hListView, 0L, _T("REMOTE_DEVICE"));
			if (dwAddr & FILE_DEVICE_IS_MOUNTED)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_IS_MOUNTED"));
			if (dwAddr & FILE_VIRTUAL_VOLUME)
				ShowDevPropertyInfo(hListView, 0L, _T("VIRTUAL_VOLUME"));
			if (dwAddr & FILE_AUTOGENERATED_DEVICE_NAME)
				ShowDevPropertyInfo(hListView, 0L, _T("AUTOGENERATED_DEVICE_NAME"));
		};
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_DEVTYPE,
		0L,
		(PBYTE)&dwAddr, //szName,
		sizeof(dwAddr),
		0))
	{
		if (dwAddr)
		{
			ShowDevPropertyInfo(hListView, _T("Device Type"), 0L);

			if (dwAddr & FILE_DEVICE_BEEP)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_BEEP"));
			if (dwAddr & FILE_DEVICE_CD_ROM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_CD_ROM"));
			if (dwAddr & FILE_DEVICE_CD_ROM_FILE_SYSTEM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_CD_ROM_FILE_SYSTEM"));
			if (dwAddr & FILE_DEVICE_CONTROLLER)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_CONTROLLER"));
			if (dwAddr & FILE_DEVICE_DATALINK)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_DATALINK"));
			if (dwAddr & FILE_DEVICE_DFS)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_DFS"));
			if (dwAddr & FILE_DEVICE_DISK)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_DISK"));
			if (dwAddr & FILE_DEVICE_DISK_FILE_SYSTEM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_DISK_FILE_SYSTEM"));
			if (dwAddr & FILE_DEVICE_FILE_SYSTEM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_FILE_SYSTEM"));
			if (dwAddr & FILE_DEVICE_INPORT_PORT)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_INPORT_PORT"));
			if (dwAddr & FILE_DEVICE_KEYBOARD)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_KEYBOARD"));
			if (dwAddr & FILE_DEVICE_MAILSLOT)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_MAILSLOT"));
			if (dwAddr & FILE_DEVICE_MIDI_IN)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_MIDI_IN"));
			if (dwAddr & FILE_DEVICE_MIDI_OUT)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_MIDI_OUT"));
			if (dwAddr & FILE_DEVICE_MOUSE)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_MOUSE"));
			if (dwAddr & FILE_DEVICE_MULTI_UNC_PROVIDER)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_MULTI_UNC_PROVIDER"));
			if (dwAddr & FILE_DEVICE_NAMED_PIPE)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_NAMED_PIPE"));
			if (dwAddr & FILE_DEVICE_NETWORK)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_NETWORK"));
			if (dwAddr & FILE_DEVICE_NETWORK_BROWSER)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_NETWORK_BROWSER"));
			if (dwAddr & FILE_DEVICE_NETWORK_FILE_SYSTEM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_NETWORK_FILE_SYSTEM"));
			if (dwAddr & FILE_DEVICE_NULL)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_NULL"));
			if (dwAddr & FILE_DEVICE_PARALLEL_PORT)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_PARALLEL_PORT"));
			if (dwAddr & FILE_DEVICE_PHYSICAL_NETCARD)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_PHYSICAL_NETCARD"));
			if (dwAddr & FILE_DEVICE_PRINTER)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_PRINTER"));
			if (dwAddr & FILE_DEVICE_SCANNER)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_SCANNER"));
			if (dwAddr & FILE_DEVICE_SERIAL_MOUSE_PORT)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_SERIAL_MOUSE_PORT"));
			if (dwAddr & FILE_DEVICE_SERIAL_PORT)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_SERIAL_PORT"));
			if (dwAddr & FILE_DEVICE_SCREEN)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_SCREEN"));
			if (dwAddr & FILE_DEVICE_SOUND)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_SOUND"));
			if (dwAddr & FILE_DEVICE_STREAMS)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_STREAMS"));
			if (dwAddr & FILE_DEVICE_TAPE)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_TAPE"));
			if (dwAddr & FILE_DEVICE_TAPE_FILE_SYSTEM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_TAPE_FILE_SYSTEM"));
			if (dwAddr & FILE_DEVICE_TRANSPORT)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_TRANSPORT"));
			if (dwAddr & FILE_DEVICE_UNKNOWN)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_UNKNOWN"));
			if (dwAddr & FILE_DEVICE_VIDEO)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_VIDEO"));
			if (dwAddr & FILE_DEVICE_VIRTUAL_DISK)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_VIRTUAL_DISK"));
			if (dwAddr & FILE_DEVICE_WAVE_IN)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_WAVE_IN"));
			if (dwAddr & FILE_DEVICE_WAVE_OUT)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_WAVE_OUT"));
			if (dwAddr & FILE_DEVICE_8042_PORT)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_8042_PORT"));
			if (dwAddr & FILE_DEVICE_NETWORK_REDIRECTOR)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_NETWORK_REDIRECTOR"));
			if (dwAddr & FILE_DEVICE_BATTERY)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_BATTERY"));
			if (dwAddr & FILE_DEVICE_BUS_EXTENDER)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_BUS_EXTENDER"));
			if (dwAddr & FILE_DEVICE_MODEM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_MODEM"));
			if (dwAddr & FILE_DEVICE_VDM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_VDM"));
			if (dwAddr & FILE_DEVICE_MASS_STORAGE)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_MASS_STORAGE"));
			if (dwAddr & FILE_DEVICE_SMB)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_SMB"));
			if (dwAddr & FILE_DEVICE_KS)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_KS"));
			if (dwAddr & FILE_DEVICE_CHANGER)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_CHANGER"));
			if (dwAddr & FILE_DEVICE_SMARTCARD)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_SMARTCARD"));
			if (dwAddr & FILE_DEVICE_ACPI)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_ACPI"));
			if (dwAddr & FILE_DEVICE_DVD)
				ShowDevPropertyInfo(hListView, 0L, _T("FILE_DEVICE_DVD"));
			if (dwAddr & FILE_DEVICE_FULLSCREEN_VIDEO)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_FULLSCREEN_VIDEO"));
			if (dwAddr & FILE_DEVICE_DFS_FILE_SYSTEM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_DFS_FILE_SYSTEM"));
			if (dwAddr & FILE_DEVICE_DFS_VOLUME)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_DFS_VOLUME"));
			if (dwAddr & FILE_DEVICE_SERENUM)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_SERENUM"));
			if (dwAddr & FILE_DEVICE_TERMSRV)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_TERMSRV"));
			if (dwAddr & FILE_DEVICE_KSEC)
				ShowDevPropertyInfo(hListView, 0L, _T("DEVICE_KSEC"));
		};
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_DRIVER,
		0L,
		(PBYTE)szName,
		63,
		0))
	{
		ShowDevPropertyInfo(hListView, _T("Driver"), szName);
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_ENUMERATOR_NAME,
		0L,
		(PBYTE)szName,
		63,
		0))
	{
		ShowDevPropertyInfo(hListView, _T("Enumerator"), szName);
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_HARDWAREID,
		0L,
		(PBYTE)szName,
		63,
		0))
	{
		TCHAR* p = szName;
		ShowDevPropertyInfo(hListView, _T("Hardware ID"), 0L);
		while (*p)
		{
			ShowDevPropertyInfo(hListView, 0L, p);
			p = _tcschr(p, 0);
			*p++;
		};
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_LOCATION_INFORMATION,
		0L,
		(PBYTE)szName,
		63,
		0))
	{
		ShowDevPropertyInfo(hListView, _T("Local Information"), szName);
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_LOWERFILTERS,
		0L,
		(PBYTE)szName,
		63,
		0))
	{
		ShowDevPropertyInfo(hListView, _T("Lower Filter"), szName);
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_UPPERFILTERS,
		0L,
		(PBYTE)szName,
		63,
		0))
	{
		ShowDevPropertyInfo(hListView, _T("Upper Filter"), szName);
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,
		0L,
		(PBYTE)szName,
		63,
		0))
	{
		ShowDevPropertyInfo(hListView, _T("Physical Device Object Name"), szName);
	};

	if (SetupDiGetDeviceRegistryProperty(hDevInfo,
		spDevInfoData,
		SPDRP_SERVICE,
		0L,
		(PBYTE)szName,
		63,
		0))
	{
		TCHAR* p = szName;
		ShowDevPropertyInfo(hListView, _T("Service Name"), 0L);
		if (*p)
			GetDriverName(GetDlgItem(hDlg, nIDList1), szName);
		while (*p)
		{
			ShowDevPropertyInfo(hListView, 0L, p);
			p = _tcschr(p, 0);
			*p++;
			while (_tcschr(p, '\\'))
			{
				p = _tcschr(p, 0);
				*p++;
			};
		};

	};
};

/**
 * @brief Enumerate all devices and populate the tree and list views
 * 
 * This method initializes the device enumeration process, clearing existing
 * data and rebuilding the complete device tree with all hardware information.
 */
void CEnumDevicesDlg::EnumDevices()
{
	// Show wait cursor during enumeration
	CWaitCursor pWaitCursor;

	// Disable redrawing for better performance during updates
	m_ctrlDevices.SetRedraw(FALSE);
	m_ctrlDetails.SetRedraw(FALSE);

	// Initialize data structures for device tracking
	InitialDeviceList(GetSafeHwnd());
	InitialDeviceOrder(GetSafeHwnd());

	// Clear existing tree view nodes
	TreeViewRemoveAllNodes(GetSafeHwnd(), IDC_DEVICES);

	// Load device class images
	InitialImageData();

	// Enumerate all WDM drivers and populate the tree
	EnumWDMDriver(IDC_DEVICES, IDB_COMPUTER, GetSafeHwnd());

	// Re-enable redrawing and update the display
	m_ctrlDevices.SetRedraw(TRUE);
	m_ctrlDetails.SetRedraw(TRUE);
	m_ctrlDevices.UpdateWindow();
	m_ctrlDetails.UpdateWindow();
}

/**
 * @brief Find a device by name and display its detailed information
 * @param szName Device name to search for
 * @param nIDList1 Control ID of the ListView to display results
 * @param hDlg Handle to the parent dialog
 * @return 1 if device found and displayed, 0 if not found
 */
char FindDeviceName(const TCHAR* szName, const UINT nIDList1, HWND hDlg)
{
	DEVICE_LIST* pList = _pHead->pNext;
	//
	while (pList)
	{
		if (!_tcscmp(pList->szName, szName))
		{
			GetOtherInfo(pList->guid, pList->wOrder, nIDList1, hDlg);
			GetDeviceDetailInfo(pList, hDlg);
			return 1;
		};
		pList = pList->pNext;
	};
	return 0;
};

/**
 * @brief Handle TreeView selection change event to display device details
 * @param pNMHDR Pointer to notification message header
 * @param pResult Pointer to result value
 */
void CEnumDevicesDlg::OnSelchangedDevices(NMHDR* pNMHDR, LRESULT* pResult)
{
	// LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	TVITEM tvItem = { 0 };
	TreeViewGetSelectText(GetDlgItem(IDC_DEVICES)->GetSafeHwnd(), pNMHDR, &tvItem);
	if (tvItem.pszText)
	{
		TCHAR szData[LINE_LEN] = { 0 };
		_tcscpy(szData, tvItem.pszText/*, _tcslen(tvItem.pszText)*/);
		ListViewRemoveAllItems(GetDlgItem(IDC_DETAILS)->GetSafeHwnd());
		// ListViewRemoveAllItems(GetDlgItem(GetSafeHwnd(), IDC_LIST2));
		// ListViewInsertColumnText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 0, 0, _T("Field"), 0, GetSafeHwnd());
		// ListViewInsertColumnText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 1, 0, _T("Descritption"), 0, GetSafeHwnd());
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 0, 0, _T("GUID"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 1, 0, _T("Hardware ID"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 2, 0, _T("Install ID"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 3, 0, _T("Manufacturer"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 4, 0, _T("Provider"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 5, 0, _T("Driver Description"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 6, 0, _T("Section Name"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 7, 0, _T("INF Date"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 8, 0, _T("INF Path"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 9, 0, _T("Image Path"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 10, 0, _T("Index"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 11, 0, _T("Device Path"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 12, 0, _T("Group"));
		ListViewInsertItemText(GetDlgItem(IDC_DETAILS)->GetSafeHwnd(), 13, 0, _T("Start"));
		// SetCharSet(IDC_LIST1, IDC_LIST2);
		if (!FindDeviceName(szData, IDC_DETAILS, GetSafeHwnd()))
		{
			ListViewRemoveAllItems(GetDlgItem(IDC_DETAILS)->GetSafeHwnd());
		};
	};
	*pResult = 0;
}
