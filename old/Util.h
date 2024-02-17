#ifndef __ICE_UTILLIB_H
#define __ICE_UTILLIB_H
//
#pragma once
//
#ifdef __cplusplus
extern "C" {
#endif
//
#include <windows.h>
#include <setupapi.h>
#include <commctrl.h>
#include <stdio.h>
#include <tchar.h>
//#include "resource.h"
//
#pragma comment(lib, "comctl32.lib")
//
UINT IsListViewClkEvent(const UINT, NMHDR*);
UINT IsListViewDBClkEvent(const UINT, NMHDR*);
void ListViewDeleteItem(HWND hList, const short wItem);
void ListViewGetItemText(HWND, const int, char*);
void ListViewGetSpecItem(HWND, const int, const int, char*);
void ListViewInsertColumnText(HWND, const short, const int, char*, const BOOL);
void ListViewAddColumnImageText(HWND, const UINT, const short, const int, char*, const BOOL);
void ListViewInsertItemText(HWND, const int, const int, char*);
void ListViewSetExtStyle(HWND, const DWORD);
void ListViewSetSelectItem(HWND, const short);
void ListViewRemoveAllItems(HWND);
void ListViewRemoveItem(HWND, const UINT, const int);
void ListViewRemoveColumn(HWND, const UINT, const int);
int  ListViewGetItemSelect(HWND);
int  ListViewGetItemCount(HWND);
//
HTREEITEM TreeViewInsertRootText(HWND, UINT, HTREEITEM, HTREEITEM, char*);
HTREEITEM TreeViewFindChild(HWND, HTREEITEM, const char*);
HTREEITEM MakeDeviceRootTree(SP_CLASSIMAGELIST_DATA, const UINT, const UINT);
UINT IsTreeViewClkEvent(const UINT, NMHDR*);
UINT IsTreeViewSelectChanged(const UINT, NMHDR*);
void TreeViewGetSelectText(HWND, NMHDR*, TVITEM*);
void TreeViewExpand(HWND, HTREEITEM, const BOOL);
void TreeViewRemoveAllNodes(HWND, const UINT);
//
void SetOverlayImage(HIMAGELIST, const int, const int);
int  GetImageCount(HIMAGELIST);
int  ListViewSetImageList(HWND, const int, LPARAM);
//
void ShowErrorMsg(HWND, const DWORD, const char*);
void DisableCloseBox(HWND);
void WaitCursor(const char);
//
BOOL AnsiToUnicode(const char*, const short, wchar_t*, const short);
BOOL UnicodeToAnsi(const wchar_t*, const short, char*, const short);
//
void FindComma(char*);
void StrLTrim(char*);
void StrRTrim(char*);
void StrRight(char*, short);
void ConvertGUIDToString(const GUID, char*);
//
#ifdef __cplusplus
}
#endif
//
#endif
