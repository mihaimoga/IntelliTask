//
// (w)ritten by Chuan-Liang Teng 2006, mailto:clteng@ms6.hinet.net
//
#include "devmgr.h"
//
HINSTANCE _hInst = 0L;
HWND      _hDlg  = 0L;
//
BOOL EnablePrivilege(LPCTSTR PrviName)
{
    HANDLE           hToken = 0L;
    TOKEN_PRIVILEGES Newtp;
    BOOL             bRtn = FALSE;
//
    OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
    if (LookupPrivilegeValue(NULL,PrviName, &Newtp.Privileges[0].Luid))
    {
        Newtp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        Newtp.PrivilegeCount = 1;
        if (AdjustTokenPrivileges(hToken, FALSE, &Newtp, sizeof(Newtp), NULL, NULL))
            bRtn = TRUE;
    };
//
    CloseHandle(hToken);
//
    return bRtn;
};
char OpenINFFile(const char bType, char *pszName)
{
    char         szFile[MAX_PATH] = {0};
    char         szTitle[64]      = {0L};
    OPENFILENAME openFile         = {0L};
//
    openFile.lStructSize    = sizeof(OPENFILENAME);
    openFile.hwndOwner      = _hDlg;
    openFile.hInstance      = _hInst;
    if (bType)
        openFile.lpstrFilter    = "INF File(*.inf)\0*.inf\0";
    else
        openFile.lpstrFilter    = "System File(*.sys)\0*.sys\0";
    openFile.lpstrFile      = szFile;
    openFile.lpstrTitle     = szTitle;
    openFile.nMaxFileTitle  = 64;
    openFile.nMaxFile       = MAX_PATH;
    openFile.Flags          = OFN_FILEMUSTEXIST | OFN_EXPLORER;
    openFile.Flags         |= OFN_HIDEREADONLY; //  | OFN_ENABLETEMPLATE;
    if (GetOpenFileName(&openFile))
    {
        strcpy(pszName, szFile);
        return 1;
    };
    return 0;
};
//
void SetCharSet(const UINT nIDList1, const UINT nIDList2)
{
    HWND hListView1 = GetDlgItem(_hDlg, nIDList1);
    HWND hListView2 = GetDlgItem(_hDlg, nIDList2);
//
    if (GetACP() == 950)
    {
        ListViewInsertColumnText(hListView1, 0, 0, "欄位(Field)", 0);
        ListViewInsertColumnText(hListView1, 1, 0, "說明(Descritption)", 1);
        ListViewInsertItemText(hListView1, 0, 0, "GUID");
        ListViewInsertItemText(hListView1, 1, 0, "硬體識別碼");
        ListViewInsertItemText(hListView1, 2, 0, "安裝識別碼");
        ListViewInsertItemText(hListView1, 3, 0, "廠商");
        ListViewInsertItemText(hListView1, 4, 0, "供應者");
        ListViewInsertItemText(hListView1, 5, 0, "說明");
        ListViewInsertItemText(hListView1, 6, 0, "區段名稱");
        ListViewInsertItemText(hListView1, 7, 0, "INF 日期");
        ListViewInsertItemText(hListView1, 8, 0, "INF 路徑");
        ListViewInsertItemText(hListView1, 9, 0, "程式路徑");
        ListViewInsertItemText(hListView1, 10, 0, "索引值");
        ListViewInsertItemText(hListView1, 11, 0, "設備路徑");
        ListViewInsertItemText(hListView1, 12, 0, "群組");
        ListViewInsertItemText(hListView1, 13, 0, "啟動方式");
//
        ListViewInsertColumnText(hListView2, 0, 0, "資源型態", 0);
        ListViewInsertColumnText(hListView2, 1, 0, "設定", 1);
    }
    else
    {
        ListViewInsertColumnText(hListView1, 0, 0, "Field", 0);
        ListViewInsertColumnText(hListView1, 1, 0, "Descritption", 1);
        ListViewInsertItemText(hListView1, 0, 0, "GUID");
        ListViewInsertItemText(hListView1, 1, 0, "Hardware ID");
        ListViewInsertItemText(hListView1, 2, 0, "Install ID");
        ListViewInsertItemText(hListView1, 3, 0, "Manufacturer");
        ListViewInsertItemText(hListView1, 4, 0, "Provider");
        ListViewInsertItemText(hListView1, 5, 0, "Driver Description");
        ListViewInsertItemText(hListView1, 6, 0, "Section Name");
        ListViewInsertItemText(hListView1, 7, 0, "INF Date");
        ListViewInsertItemText(hListView1, 8, 0, "INF Path");
        ListViewInsertItemText(hListView1, 9, 0, "Image Path");
        ListViewInsertItemText(hListView1, 10, 0, "Index");
        ListViewInsertItemText(hListView1, 11, 0, "Device Path");
        ListViewInsertItemText(hListView1, 12, 0, "Group");
        ListViewInsertItemText(hListView1, 13, 0, "Start");
//
        ListViewInsertColumnText(hListView2, 0, 0, "Resource Type", 0);
        ListViewInsertColumnText(hListView2, 1, 0, "Setting", 1);
    };
};
//
void DisableRadioBtn(HWND hDlg, const char bEnable)
{
    short wLoop;
//
    for (wLoop = IDC_PARAMOPT1; wLoop <= IDC_PARAMOPT9; wLoop++)
         EnableWindow(GetDlgItem(hDlg, wLoop), bEnable);
    SendMessage(GetDlgItem(hDlg, IDC_PARAMOPT3), BM_SETCHECK, BST_CHECKED, 0);
    SendMessage(GetDlgItem(hDlg, IDC_PARAMOPT8), BM_SETCHECK, BST_CHECKED, 0);
};
//
void GetInstallOption(HWND hDlg, short *wError, short *wStart)
{
    HWND  hOption = 0L;
    short wLoop;
//
    for (wLoop = IDC_PARAMOPT1; wLoop <= IDC_PARAMOPT4; wLoop++)
    {
        hOption = GetDlgItem(hDlg, wLoop);
        if (SendMessage(hOption, BM_GETCHECK, 0, 0) == BST_CHECKED)
        {
            switch(wLoop)
            {
                case IDC_PARAMOPT1:
                    *wError = SERVICE_ERROR_CRITICAL;
                break;
//
                case IDC_PARAMOPT2:
                    *wError = SERVICE_ERROR_IGNORE;
                break;
//
                case IDC_PARAMOPT3:
                    *wError = SERVICE_ERROR_NORMAL;
                break;
//
                case IDC_PARAMOPT4:
                    *wError = SERVICE_ERROR_SEVERE;
                break;
//
            };
            break;
        };
    };
    if (wLoop > IDC_PARAMOPT4)
        *wError = -1;
    for (wLoop = IDC_PARAMOPT5; wLoop <= IDC_PARAMOPT9; wLoop++)
    {
        hOption = GetDlgItem(hDlg, wLoop);
        if (SendMessage(hOption, BM_GETCHECK, 0, 0) == BST_CHECKED)
        {
            switch(wLoop)
            {
                case IDC_PARAMOPT5:
                    *wStart = SERVICE_BOOT_START;
                break;
//
                case IDC_PARAMOPT6:
                    *wStart = SERVICE_SYSTEM_START;
                break;
//
                case IDC_PARAMOPT7:
                    *wStart = SERVICE_AUTO_START;
                break;
//
                case IDC_PARAMOPT8:
                    *wStart = SERVICE_DEMAND_START;
                break;
//
                case IDC_PARAMOPT9:
                    *wStart = SERVICE_DISABLED;
                break;
            };
            break;
        };
    };
    if (wLoop > IDC_PARAMOPT9)
        *wStart = -1;
};
//
INT_PTR CALLBACK InstallDialog(HWND hDlg, UINT uMsg,
                               WPARAM wParam, LPARAM lParam)
{
    static FILE *pf             = 0L;    
    static char szINF[MAX_PATH] = {0};
    static char bType;
//
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            DisableCloseBox(hDlg);
            DisableRadioBtn(hDlg, 0);
            bType = (!lParam) ? 0 : 1;
            EnableWindow(GetDlgItem(hDlg, IDC_INSTALLBTN), 0);
            EnableWindow(GetDlgItem(hDlg, IDC_RESETBTN), 0);
            EnableWindow(GetDlgItem(hDlg, IDC_DRVNAMETXT), 0);
            EnableWindow(GetDlgItem(hDlg, IDC_ENABLEDRVBTN), 0);
            SetFocus(GetDlgItem(hDlg, IDC_OPENINFBTN));
        };
        break;
//
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_OPENINFBTN:
                {
                    if (pf)
                        fclose(pf);
                    InitialGlobalVar();
                    pf = 0L;
                    if (!OpenINFFile(bType, szINF))
                        break;
                    SendMessage(GetDlgItem(hDlg, IDC_INFLBL), WM_SETTEXT,
                                0, (LPARAM)szINF);
                    pf = fopen(szINF, "r");
                    if (!pf)
                    {
                        sprintf(szINF, "Erro Open File!Code:%d", errno);
                        MessageBox(hDlg, szINF, "ERROR", MB_ICONSTOP);
                        break;
                    };
                    EnableWindow(GetDlgItem(hDlg, IDC_INSTALLBTN), 1);
                    EnableWindow(GetDlgItem(hDlg, IDC_RESETBTN), 1);
                    if (!bType)
                    {
                        DisableRadioBtn(hDlg, 1);
                        EnableWindow(GetDlgItem(hDlg, IDC_DRVNAMETXT), 1);
                        SetFocus(GetDlgItem(hDlg, IDC_DRVNAMETXT));
                    };
                };
                break;
//
                case IDC_INSTALLBTN:
                {
                    if (bType)
                    {
                        if (pf)
                            GetINFData(pf);
                        EnableWindow(GetDlgItem(hDlg, IDC_INSTALLBTN), 0);
                        EnableWindow(GetDlgItem(hDlg, IDC_RESETBTN), 0);
                        if (!StartInstallWDMDriver(szINF))
                        {
                            EnableWindow(GetDlgItem(hDlg, IDC_INSTALLBTN), 0);
                            EnableWindow(GetDlgItem(hDlg, IDC_RESETBTN), 0);
                            SetFocus(GetDlgItem(hDlg, IDC_OPENINFBTN));
                        }
                        else
                        {
                            EnableWindow(GetDlgItem(hDlg, IDC_INSTALLBTN), 1);
                            EnableWindow(GetDlgItem(hDlg, IDC_RESETBTN), 1);
                        };
                    }
                    else
                    {
                        short wError;
                        short wStart;
                        char  szDevName[64] = {0};
                        char szName[MAX_PATH] = {"\\\\.\\\0"};
//
                        GetInstallOption(hDlg, &wError, &wStart);
                        if (!wError || !wStart)
                            break;
                        SendMessage(GetDlgItem(hDlg, IDC_DRVNAMETXT), WM_GETTEXT,
                                    sizeof(char)*63, (LPARAM)szDevName);
                        if (!*szDevName)
                        {
                            RtlZeroMemory(szINF, sizeof(char)*MAX_PATH);
                            if (pf)
                                fclose(pf);
                            pf = 0L;
                            SendMessage(GetDlgItem(hDlg, IDC_INFLBL), WM_SETTEXT,
                                        0, 0L);
                            EnableWindow(GetDlgItem(hDlg, IDC_DRVNAMETXT), 0);
                            for (wStart = IDC_PARAMOPT1; wStart <= IDC_PARAMOPT9; wStart++)
                                 SendMessage(GetDlgItem(hDlg, wStart), BM_SETCHECK, BST_UNCHECKED, 0);
                            SetFocus(GetDlgItem(hDlg, IDC_OPENINFBTN));
                        }
                        else
                        {
                            strcat(szName, szDevName);
                            StartDriver(wError, wStart, szINF, szDevName);
                        };
                    };
                };
                break;
//
                case IDC_RESETBTN:
                {
                    short wLoop;
//
                    if (pf)
                        fclose(pf);
                    pf = 0L;
                    SendMessage(GetDlgItem(hDlg, IDC_INFLBL), WM_SETTEXT,
                                0, (LPARAM)"");
                    RtlZeroMemory(szINF, sizeof(char)*MAX_PATH);
                    SetFocus(GetDlgItem(hDlg, IDC_OPENINFBTN));
                    for (wLoop = IDC_PARAMOPT1; wLoop <= IDC_PARAMOPT9; wLoop++)
                        SendMessage(GetDlgItem(hDlg, wLoop), BM_SETCHECK, BST_UNCHECKED, 0);
                };
                break;
//
                case IDC_IEXIT:
                {
                    EndDialog(hDlg, 0);
                };
                break;
            };
        };
        break;
    };
    return 0;                     
};
//
void DeleteINFFile(const char *szINF)
{
    WIN32_FIND_DATA w32Data = {0};
    HANDLE          hFind   = 0L;
    char            szName[MAX_PATH] = {0};
    char            szDel[MAX_PATH] = {0};
//
    strcpy(szName, szINF);
    strcat(szName, ".*");
//
    hFind = FindFirstFile(szName, &w32Data);
    if (hFind != INVALID_HANDLE_VALUE)
    {
        do
        {
            _splitpath(szINF, szDel, szName, 0L, 0L);
            strcat(szDel, szName);
            strcat(szDel, w32Data.cFileName);
            _unlink(szDel);
        } while (FindNextFile(hFind, &w32Data));
    };
};
//
INT_PTR CALLBACK MainDialog(HWND hDlg, UINT uMsg,
                            WPARAM wParam, LPARAM lParam)
{
    static CRITICAL_SECTION criticalSection = {0};
//
    switch(uMsg)
    {
        case WM_INITDIALOG:
        {
            HWND hListView = GetDlgItem(hDlg, IDC_LIST1);
//
            _hDlg = hDlg;
            WaitCursor(1);
            EnablePrivilege(SE_DEBUG_NAME);
            EnablePrivilege(SE_LOAD_DRIVER_NAME);
            DisableCloseBox(hDlg);
            if (!InitialDeviceList() || !InitialDeviceOrder())
                EndDialog(hDlg, 0);
            ListViewSetExtStyle(GetDlgItem(hDlg, IDC_LIST1), 0);
            ListViewSetExtStyle(GetDlgItem(hDlg, IDC_LIST2), 0);
            SetCharSet(IDC_LIST1, IDC_LIST2);
            TreeViewRemoveAllNodes(hDlg, IDC_TREE1);
            InitialImageData();
            EnumWDMDriver(IDC_TREE1, IDB_MYCOMP);
            SendMessage(GetDlgItem(hDlg, IDC_RADIO2), BM_SETCHECK, BST_CHECKED, 0);
            EnableWindow(GetDlgItem(hDlg, IDC_DELBTN), 0);
            EnableWindow(GetDlgItem(hDlg, IDC_COPYPATHBTN), 0);
            WaitCursor(0);
            InitializeCriticalSection(&criticalSection);
        };
        break;
//
        case WM_COMMAND:
        {
            switch(LOWORD(wParam))
            {
                case IDC_ADDBTN:
                {
                    short wLegacy = 0;
//
                    if (SendMessage(GetDlgItem(hDlg, IDC_RADIO2),
                                    BM_GETCHECK, 0, 0) == BST_CHECKED)
                        wLegacy = 1;
                    if (DialogBoxParam(_hInst, MAKEINTRESOURCE(IDD_DIALOG2),
                                        hDlg, InstallDialog, wLegacy) == -1)
                        ShowErrorMsg(hDlg, GetLastError(), "DialogBox");
                };
                break;
//
                case IDC_DELBTN:
                {
                    char  szHardware[64]   = {0};
                    char  szPath[MAX_PATH] = {0};
                    short wIndex;
//
                    ListViewGetSpecItem(GetDlgItem(hDlg, IDC_LIST1), 10, 1, szHardware);
                    wIndex = atoi(szHardware);
                    RtlZeroMemory(szHardware, sizeof(char)*64);
                    ListViewGetSpecItem(GetDlgItem(hDlg, IDC_LIST1), 1, 1, szHardware);
                    ListViewGetSpecItem(GetDlgItem(hDlg, IDC_LIST1), 8, 1, szPath);
                    UninstallWDMDriver(wIndex, szHardware);
                    {
                        char *p              = strchr(szPath, '.');
                        char szTmp[MAX_PATH] = {0};
                        memcpy(szTmp, szPath, p-szPath);
                        DeleteINFFile(szTmp);
                    };
                    ListViewGetSpecItem(GetDlgItem(hDlg, IDC_LIST1), 9, 1, szPath);
                    _unlink(szPath);
                    EnableWindow(GetDlgItem(hDlg, IDC_DELBTN), 0);
                    EnableWindow(GetDlgItem(hDlg, IDC_COPYPATHBTN), 0);
                };
                break;
//
                case IDC_REFBTN:
                {
                    SendMessage(hDlg, WM_DEVICECHANGE, 0, 0);
                };
                break;
//
                case IDC_COPYPATHBTN:
                {
                    HGLOBAL hMem            = 0L;
                    char    *p              = 0L;
                    char    szBuf[MAX_PATH] = {0};
//
                    if (!IsClipboardFormatAvailable(CF_TEXT))
                        break;
                    if (!OpenClipboard(hDlg))
                    {
                        ShowErrorMsg(hDlg, GetLastError(), "OpenClipboard");
                        break;
                    };
                    EmptyClipboard();
                    ListViewGetSpecItem(GetDlgItem(hDlg, IDC_LIST1), 11, 1, szBuf);
                    if (!*szBuf)
                        break;
                    hMem = LocalAlloc(LMEM_MOVEABLE, sizeof(char)*strlen(szBuf)+1);
                    if (!hMem)
                    {
                        ShowErrorMsg(hDlg, GetLastError(), "LocalAlloc");
                        break;
                    };
                    p = LocalLock(hMem);
                    memcpy(p, szBuf, strlen(szBuf));
                    p[strlen(szBuf)] = 0;
                    LocalUnlock(hMem);
                    SetClipboardData(CF_TEXT, p);
                    LocalFree(hMem);
                    CloseClipboard();
                };
                break;
//
                case IDC_EXIT:
                    FreeAllocDeviceNode();
                    FreeAllDeviceOrderNode();
                    DeleteCriticalSection(&criticalSection);
                    EndDialog(hDlg, 0);
                break;
            };
            break;
//
        };
        break;
//
        case WM_DEVICECHANGE:
        {
            EnterCriticalSection(&criticalSection);
            WaitCursor(1);
            FreeAllocDeviceNode();
            FreeAllDeviceOrderNode();
            if (!InitialDeviceList() || !InitialDeviceOrder())
                EndDialog(hDlg, 0);
            ListViewRemoveAllItems(GetDlgItem(hDlg, IDC_LIST1));
            ListViewRemoveAllItems(GetDlgItem(hDlg, IDC_LIST2));
            SetCharSet(IDC_LIST1, IDC_LIST2);
            TreeViewRemoveAllNodes(hDlg, IDC_TREE1);
            InitialImageData();
            EnumWDMDriver(IDC_TREE1, IDB_MYCOMP);
            WaitCursor(0);
            LeaveCriticalSection(&criticalSection);
        };
        break;
//
        case WM_NOTIFY:
        {
            NMHDR *pnmh = (NMHDR*)lParam;
//
            switch(pnmh->idFrom)
            {
                case IDC_LIST1:
                {
                    if (pnmh->code == NM_CLICK)
                    {
                        char  szBuf[MAX_PATH] = {0};
                        HWND  hList           = GetDlgItem(hDlg, IDC_LIST1);
//
                        if (ListViewGetItemSelect(hList) == 10 ||
                            ListViewGetItemSelect(hList) == 11)
                        {
                            ListViewGetSpecItem(hList, 10, 1, szBuf);
                            if (*szBuf)
                                EnableWindow(GetDlgItem(hDlg, IDC_DELBTN), 1);
                            ListViewGetSpecItem(hList, 11, 1, szBuf);
                            if (*szBuf)
                                EnableWindow(GetDlgItem(hDlg, IDC_COPYPATHBTN), 1);
                        }
                        else
                        {
                            EnableWindow(GetDlgItem(hDlg, IDC_DELBTN), 0);
                            EnableWindow(GetDlgItem(hDlg, IDC_COPYPATHBTN), 0);
                        };
                    };
                };
                break;
//
                case IDC_TREE1:
                {
                    if (pnmh->code == TVN_SELCHANGED)
                    {
                        TVITEM     tvItem = {0};
                        NMTREEVIEW *pnmhTree = (NMTREEVIEW*)lParam;
//
                        TreeViewGetSelectText(GetDlgItem(hDlg, IDC_TREE1),
                                              (NMHDR*)lParam, &tvItem);
                        if (tvItem.pszText)
                        {
                            char szData[LINE_LEN] = {0};
//
                            memcpy(szData, tvItem.pszText, strlen(tvItem.pszText));
                            ListViewRemoveAllItems(GetDlgItem(_hDlg, IDC_LIST1));
                            ListViewRemoveAllItems(GetDlgItem(_hDlg, IDC_LIST2));
                            SetCharSet(IDC_LIST1, IDC_LIST2);
                            FindDeviceName(szData, IDC_LIST1, IDC_LIST2);
                        };
                    };
                };
                break;
            };
        };
        break;
    };
    return 0;
};
//
int APIENTRY WinMain(HINSTANCE hInst, HINSTANCE hPrevInst,
                     LPSTR lpCmdLine, int nShowCmd)
{
    InitCommonControls();
    _hInst = hInst;
    if (DialogBoxParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1),
                       0L, MainDialog, 0L) == -1)
        ShowErrorMsg(0L, GetLastError(), "DialogBoxParam");
    ExitProcess(0);
    return 0;
};
