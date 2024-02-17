//
// (w)ritten by Chuan-Liang Teng 2006, mailto:clteng@ms6.hinet.net
//
#include "util.h"
//
extern HINSTANCE _hInst;
extern HWND      _hDlg;
//
void ShowErrorMsg(HWND hWnd, const DWORD dwErrorCode, const char *szFunctionName)
{
    void* lpMsgBuf;
//
    if (!FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                       FORMAT_MESSAGE_FROM_SYSTEM | 
                       FORMAT_MESSAGE_IGNORE_INSERTS,
                       0L,
                       dwErrorCode,
                       MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                       (char*)&lpMsgBuf,
                       0,
                       0L))
        return;

    MessageBox(hWnd, (const char*)lpMsgBuf, szFunctionName, MB_ICONSTOP | MB_OK);
    LocalFree(lpMsgBuf);
};
//
void WaitCursor(const char bIsWait)
{
    HCURSOR hCur = 0L;
//
    hCur = (HCURSOR)LoadImage(_hInst, (bIsWait)?IDC_WAIT:IDC_ARROW,
                              IMAGE_CURSOR,
                              GetSystemMetrics(SM_CXSMICON),
                              GetSystemMetrics(SM_CYSMICON),
                              LR_DEFAULTCOLOR);
    if (hCur)
        SetCursor(hCur);
};
//
void DisableCloseBox(HWND hDlg)
{
    DWORD dwStyle = GetClassLongPtr(hDlg, GCL_STYLE);
    dwStyle |= CS_NOCLOSE;
    SetClassLongPtr(hDlg, GCL_STYLE, dwStyle);
};
//
/*********************************************************************/
/*                                ListView                           */
/*********************************************************************/
void ListViewDeleteItem(HWND hList, const short wItem)
{
    if (wItem == -1)
        SendMessage(hList, LVM_DELETEALLITEMS, 0, 0);
    else
        SendMessage(hList, LVM_DELETEITEM, (WPARAM)wItem, 0);
};
//
void ListViewSetExtStyle(HWND hListView, const DWORD dwStyle)
{
    DWORD style;
//
    style = (dwStyle) ? dwStyle :
            LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_EX_FLATSB;
    SendMessage(hListView, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)style);
};
//
void ListViewSetSelectItem(HWND hListView, const short iItem)
{
    LVITEM lvItem;
    short i;
//
    lvItem.iItem = iItem;
    lvItem.mask  = LVIF_STATE;
    lvItem.state = LVIS_SELECTED;
    i = (short)SendMessage(hListView, LVM_SETITEMSTATE, iItem, (LPARAM)&lvItem);
};
//
void ListViewInsertColumnText(HWND hListView, const short wIdx,
                              const int wFmt, char *pszText, const BOOL bFinal)
{
    LVCOLUMN   column;
    TEXTMETRIC text;
    int        nWidth;
    HDC        hDC = GetDC(_hDlg);
    int        fmt;
//
    GetTextMetrics(hDC, &text);
    nWidth = text.tmAveCharWidth*20;
    ReleaseDC(_hDlg, hDC);
//
    if (!wFmt)
        fmt = LVCFMT_LEFT;
    column.mask    = LVCF_TEXT|LVCF_FMT|LVCF_WIDTH;
    column.pszText = pszText;
    column.fmt     = fmt;
    if (!bFinal)
        column.cx = nWidth;
    else
        column.cx = nWidth*100;
    if (SendMessage(hListView, LVM_INSERTCOLUMN, wIdx, (LPARAM)(LPLVCOLUMN)&column) == -1)
        ShowErrorMsg(_hDlg, GetLastError(), "InitialListView");
};
//
void ListViewAddColumnImageText(HWND hWnd, const UINT nID, const short wIdx,
                                const int wFmt, char *pszText, const BOOL bFinal)
{
    LVCOLUMN   column;
    TEXTMETRIC text;
    int        nWidth;
    HWND       hListView = GetDlgItem(hWnd, nID);
    HDC        hDC = GetDC(hWnd);
    int        fmt;
//
    GetTextMetrics(hDC, &text);
    nWidth = text.tmAveCharWidth*20;
    ReleaseDC(hWnd, hDC);
//
    if (!wFmt)
        fmt = LVCFMT_LEFT;
    column.mask    = LVCF_TEXT|LVCF_FMT|LVCF_WIDTH|LVCF_IMAGE;
    column.pszText = pszText;
    column.fmt     = fmt;
    if (!bFinal)
        column.cx = nWidth;
    else
        column.cx = 700;
    if (SendMessage(hListView, LVM_INSERTCOLUMN, wIdx, (LPARAM)(LPLVCOLUMN)&column) == -1)
        ShowErrorMsg(hWnd, GetLastError(), "InitialListView");
};
//
void ListViewInsertItemText(HWND hListView, const int iItem,
                            const int iSubItem, char *pszText)
{
    LVITEM item;
//
    item.mask       = LVIF_TEXT;
    item.iItem      = iItem;
    item.iSubItem   = iSubItem;
    item.state      = 0;
    item.stateMask  = 0; 
    item.iImage     = 0;
    item.lParam     = 0;
    item.pszText    = pszText;
    item.cchTextMax = (int)strlen(pszText);
    if (!iSubItem)
        SendMessage(hListView, LVM_INSERTITEM, 0, (LPARAM)&item);
    else
        SendMessage(hListView, LVM_SETITEMTEXT, iItem, (LPARAM)&item);
};
//
int ListViewGetItemSelect(HWND hListView)
{
    return (int)SendMessage(hListView, LVM_GETNEXTITEM, -1, MAKELPARAM((UINT)LVNI_SELECTED, 0));
};
//
int ListViewGetItemCount(HWND hListView)
{
    return (int)SendMessage(hListView, LVM_GETITEMCOUNT, 0, 0);
};
//
void ListViewGetItemText(HWND hListView, const int iSubItem, char *pszText)
{
    LVITEM item;
    int    idx = ListViewGetItemSelect(hListView);
//
    if (idx == -1)
        return;
//
    item.iSubItem   = iSubItem;
    item.pszText    = pszText;
    item.cchTextMax = 255;
    SendMessage(hListView, LVM_GETITEMTEXT, idx, (LPARAM)&item);
};
//
void ListViewGetSpecItem(HWND hListView, const int iItem, const int iSubItem, char *pszText)
{
    LVITEM item;
//
    if (iItem == -1)// || iItem == 0)
        return;
//
    item.iSubItem   = iSubItem;
    item.pszText    = pszText;
    item.cchTextMax = 255;
    SendMessage(hListView, LVM_GETITEMTEXT, iItem, (LPARAM)&item);
};
//
UINT IsListViewDBClkEvent(const UINT nID, NMHDR* pnmh)
{
    if (pnmh->idFrom == nID)
    {
        if (pnmh->code == NM_DBLCLK)
            return nID;
    };
    return 0;
};
//
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
//
void ListViewRemoveAllItems(HWND hListView)
{
    short wCnt  = (short)SendMessage(hListView, LVM_GETITEMCOUNT, 0, 0);
    short wLoop;
//
    for (wLoop = 0; wLoop < wCnt; wLoop++)
         SendMessage(hListView, LVM_DELETEITEM, 0, 0);
    SendMessage(hListView, LVM_DELETEALLITEMS, 0, 0);
};
//
void ListViewRemoveItem(HWND hDlg, const UINT nID, const int iItem)
{
    SendMessage(GetDlgItem(hDlg, nID), LVM_DELETEITEM, (WPARAM)iItem, 0);
};
//
void ListViewRemoveColumn(HWND hDlg, const UINT nID, const int iCol)
{
    SendMessage(GetDlgItem(hDlg, nID), LVM_DELETECOLUMN, (WPARAM)iCol, 0);
};
//
BOOL UnicodeToAnsi(const wchar_t *Source, const short wLen, char *Destination, const short sLen)
{
    return WideCharToMultiByte(CP_ACP,
                               WC_COMPOSITECHECK,
                               Source,
                               wLen,
                               Destination,
                               sLen,
                               0L, 0L);
};
//
/*********************************************************************/
/*                                Unicode                            */
/*********************************************************************/
BOOL AnsiToUnicode(const char *Source, const short sLen, wchar_t *Destination, const short wLen)
{
    return MultiByteToWideChar(CP_ACP,
                               MB_PRECOMPOSED,
                               Source,
                               sLen,
                               Destination,
                               wLen);
};
//
/*********************************************************************/
/*                                TreeView                           */
/*********************************************************************/
HTREEITEM TreeViewInsertRootText(HWND hWnd, UINT nID,
                                 HTREEITEM hParent, HTREEITEM hAfter,
                                 char *pszText)
{
    TVINSERTSTRUCT tvInsert;
    HWND           hTree = GetDlgItem(hWnd, nID);
//
    if (!hParent && !hAfter)
    {
        tvInsert.hParent      = 0L;
        tvInsert.hInsertAfter = 0L;
        tvInsert.item.mask    = TVIF_TEXT;
        tvInsert.item.pszText = pszText;
        return (HTREEITEM)SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);
    };
//
    tvInsert.hParent      = hParent;
    tvInsert.hInsertAfter = hAfter;
    tvInsert.item.mask    = TVIF_TEXT;
    tvInsert.item.pszText = pszText;
    return (HTREEITEM)SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvInsert);
};
//
HTREEITEM TreeViewFindChild(HWND hTreeView, HTREEITEM hRoot,
                            const char *szBuf)
{
    HTREEITEM hChildItem  = 0L;
    HTREEITEM hParentItem = hRoot;
    char      szText[128] = {0};
    TVITEM    tvItem      = {0};
    int       iLoop;
    int       nIndex;
//
    tvItem.mask       = TVIF_TEXT|TVIF_HANDLE;
    tvItem.pszText    = szText;
    tvItem.cchTextMax = sizeof(char)*127;
    nIndex = (int)SendMessage(hTreeView, TVM_GETCOUNT, 0, 0);
    RtlZeroMemory(szText, sizeof(char)*128);
    for (iLoop = 0; iLoop < nIndex; iLoop++)
    {
        hChildItem   = (HTREEITEM)SendMessage(hTreeView,
                                             TVM_GETNEXTITEM,
                                             (iLoop) ? TVGN_NEXT : TVGN_CHILD,
                                             (iLoop) ? (LPARAM)hParentItem : (LPARAM)hRoot);
        tvItem.hItem = hChildItem;
        SendMessage(hTreeView, TVM_GETITEM, 0, (LPARAM)&tvItem);
        if (*szText)
        {
            if (!strcmp(szBuf, szText))
                return hChildItem;
        };
        hParentItem = hChildItem;
    };
    return 0;
};
//
HTREEITEM MakeDeviceRootTree(SP_CLASSIMAGELIST_DATA spImageData,
                             const UINT nIdTree, const UINT nIdBmp)
{
    TVINSERTSTRUCT tvStruct       = {0};
    HIMAGELIST     hImageListTree = 0L;
    HTREEITEM      hTreeItem      = 0L;
    HBITMAP        hBitmap        = 0L;
    int            nIndex         = MAX_PATH-1;
    char           szName[MAX_PATH] = {0};
//
    GetComputerName(szName, &nIndex);
    hBitmap = LoadBitmap(_hInst, MAKEINTRESOURCE(nIdBmp));
    nIndex = ImageList_Add(spImageData.ImageList, hBitmap, 0L);
    DeleteObject(hBitmap);
    SendMessage(GetDlgItem(_hDlg, nIdTree), TVM_SETIMAGELIST,
                TVSIL_NORMAL, (LPARAM)spImageData.ImageList);
    if (ImageList_GetImageCount(spImageData.ImageList) != -1)
    {
        tvStruct.hParent      = 0L;
        tvStruct.hInsertAfter = TVI_ROOT;
        tvStruct.item.mask    = TVIF_IMAGE | TVIF_TEXT | TVIF_SELECTEDIMAGE;
        tvStruct.item.pszText = szName;
        tvStruct.item.iImage  = nIndex;
        tvStruct.item.iSelectedImage = nIndex;
        hTreeItem = (HTREEITEM)SendMessage(GetDlgItem(_hDlg, nIdTree),
                                           TVM_INSERTITEM, 0, (LPARAM)&tvStruct);
        return hTreeItem;
    };
    return 0;
};
//
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
//
UINT IsTreeViewSelectChanged(const UINT nID, NMHDR* pnmh)
{
    if (pnmh->code == TVN_SELCHANGED && pnmh->idFrom == nID)
        return TVN_SELCHANGED;
    return 0;
};
//
void TreeViewExpand(HWND hTree, HTREEITEM hTreeItem, const BOOL bExpand)
{
    UINT flags = (bExpand) ? TVE_EXPAND : TVE_COLLAPSE;
//
    SendMessage(hTree, TVM_EXPAND, (WPARAM)flags, (LPARAM)hTreeItem);
};
//
void TreeViewGetSelectText(HWND hTree, NMHDR* pnmh, TVITEM* pItem)
{
    NMTREEVIEW *pnmView          = (NMTREEVIEW*)pnmh;
    char       pszText[MAX_PATH] = {0};
//
    pItem->mask = TVIF_TEXT;
    pItem->hItem = pnmView->itemNew.hItem;
    do
    {
        pItem->pszText    = pszText;
        pItem->cchTextMax = MAX_PATH-1;
//        SendMessage(hTree, TVM_GETNEXTITEM, TVGN_CARET, (LPARAM)pItem);
        SendMessage(hTree, TVM_GETITEM, 0, (LPARAM)pItem);
        if (strlen(pItem->pszText))
            break;
    } while (1);
};
//
void TreeViewRemoveAllNodes(HWND hDlg, const UINT nIdTree)
{
    HWND  hTree = GetDlgItem(hDlg, nIdTree);
    short wCnt  = (short)SendMessage(hDlg, TVM_GETCOUNT, 0, 0);
    short wLoop;
//
    for (wLoop = 0; wLoop < wCnt; wLoop)
        SendMessage(hTree, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
    SendMessage(hTree, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
};
//
/*********************************************************************/
/*                                String                             */
/*********************************************************************/
void FindComma(char *szData)
{
    short wLen = (short)strlen(szData);
    short wIdx;
    short wLoop; 
    char  szTmp[128] = {0};
//
    for (wIdx = 0, wLoop = 0; wLoop < wLen; wLoop++)
    {
        if (szData[wLoop] == ',')
            szData[wLoop] = '.';
        else if (szData[wLoop] == ' ')
            continue;
        szTmp[wIdx++] = szData[wLoop];
    };
    memcpy(szData, szTmp, wIdx*sizeof(char));
    szData[wIdx] = 0;
};
//

void StrRight(char *szData, short wCount)
{
    short wLen = (short)strlen(szData)-wCount;
//
    if (wCount < 0)
        wCount = 0;
    if (wCount >= (short)strlen(szData))
        return;
//
    memmove(szData, szData+wLen, wCount*sizeof(char));
    szData[wCount] = 0;
};                                     
//
void StrLTrim(char *szData)
{
    char *ptr = szData;
//
    while (isspace(*ptr))
        ptr = _tcsinc(ptr);
//
    if (strcmp(ptr, szData))
    {
        short wLen = (short)(strlen(szData) - (ptr - szData));
        memmove(szData, ptr, (wLen+1)*sizeof(char));
    };
};
//
void StrRTrim(char *szData)
{
    char *ptr  = szData;
    char *pTmp = 0L;
//
    while (*ptr != 0)
    {
        if (isspace(*ptr))
        {
            if (!pTmp)
                pTmp = ptr;
        }
        else
            pTmp = 0L;
        ptr = _tcsinc(ptr);
    };
//
    if (pTmp)
    {
        *pTmp = 0;
        memmove(szData, szData, strlen(szData)-strlen(pTmp));
    };
};
//
void ConvertGUIDToString(const GUID guid, char *pData)
{
    char  szData[30] = {0};
    char  szTmp[3]   = {0};
    short wLoop;
//
    sprintf(pData, "%04X-%02X-%02X-", guid.Data1,
                   guid.Data2, guid.Data3);
    for (wLoop = 0; wLoop < 8; wLoop++)
    {
         if (wLoop == 2)
             strcat(szData, "-");
         sprintf(szTmp, "%02X", guid.Data4[wLoop]);
         strcat(szData, szTmp);
    };
//         szTmp[wLoop] = guid.Data4[wLoop];
    memcpy(pData+strlen(pData), szData, strlen(szData));
};
//
/*********************************************************************/
/*                            ListView-Image                         */
/*********************************************************************/
void SetOverlayImage(HIMAGELIST hImage, const int iImage, const int iOverlay)
{
    ImageList_SetOverlayImage(hImage, iImage, iOverlay);
};
//
int GetImageCount(HIMAGELIST hImage)
{
    return ImageList_GetImageCount(hImage);
};
//
int ListViewSetImageList(HWND hList, const int nList, LPARAM lParam)
{
    WPARAM imageList = LVSIL_NORMAL;
//
    if (nList)
        imageList = nList;
    return (int)SendMessage(hList, LVM_SETIMAGELIST,
                            imageList, lParam);
};
