//
// (w)ritten by Chuan-Liang Teng 2006, mailto:clteng@ms6.hinet.net
//
#include "setup.h"
#include "resource.h"
//
SP_CLASSIMAGELIST_DATA _spImageData = {0};
//
extern HINSTANCE _hInst;
extern HWND      _hDlg;
//
DEVICE_LIST  *_pHead      = 0L;
DEVICE_ORDER *_pOrderHead = 0L;
//
char InitialDeviceOrder()
{
    _pOrderHead = AllocNewDeviceOrderNode();
    return (_pOrderHead) ? 1 : 0;
};
//
DEVICE_ORDER* AllocNewDeviceOrderNode()
{
    DEVICE_ORDER* pNew = (DEVICE_ORDER*)LocalAlloc(LPTR, sizeof(DEVICE_ORDER));
//
    if (!pNew)
    {
        ShowErrorMsg(_hDlg, GetLastError(), "LocalAlloc");
        return 0;
    };
    RtlZeroMemory(pNew->szDevName, sizeof(char)*LINE_LEN);
    pNew->pNext = 0L;
    return pNew;
};
//
char AddNewDeviceOrderNode(const char* szDevName)
{
    DEVICE_ORDER* pAdd = AllocNewDeviceOrderNode();
//
    if (!pAdd)
        return 0;
    memcpy(pAdd->szDevName, szDevName, strlen(szDevName));
    pAdd->pNext = _pOrderHead->pNext;
    _pOrderHead->pNext = pAdd;
    return 1;
};
//
short FindDeviceOrder(const char* szName)
{
    DEVICE_ORDER *pList = _pOrderHead->pNext;
    short        wOrder = 0;
//
    while (pList)
    {
        if (!strcmp(pList->szDevName, szName))
            wOrder++;
        pList = pList->pNext;
    };
    return wOrder;
};
//
void FreeAllDeviceOrderNode()
{
    DEVICE_ORDER *pDel = _pOrderHead->pNext;
    DEVICE_ORDER *pTmp = 0L;
//
    while (pDel->pNext)
    {
        pTmp = pDel;
        pDel = pDel->pNext;
        LocalFree(pTmp);
    };
    LocalFree(_pOrderHead);
    _pOrderHead = 0L;
};
//
char InitialDeviceList()
{
    _pHead = AllocNewDeviceNode();
    return (_pHead) ? 1 : 0;
};
//
DEVICE_LIST* AllocNewDeviceNode()
{
    DEVICE_LIST* pNew = (DEVICE_LIST*)LocalAlloc(LPTR, sizeof(DEVICE_LIST));
//
    if (!pNew)
    {
        ShowErrorMsg(_hDlg, GetLastError(), "LocalAlloc");
        return 0;
    };
    RtlZeroMemory(&pNew->guid, sizeof(GUID));
    RtlZeroMemory(pNew->szInstallID, sizeof(char)*LINE_LEN);
    RtlZeroMemory(pNew->szName, sizeof(char)*MAX_PATH);
    RtlZeroMemory(pNew->szPath, sizeof(char)*MAX_PATH);
    pNew->wOrder = -1;
    pNew->wIndex = -1;
    pNew->pNext = 0L;
    return pNew;
};
//
char AddNewDeviceNode(const GUID guid,
                      const char* szName,
                      const char* szInstallID,
                      const char* szPath,
                      const int wIndex,
                      const short wOrder)
{
    DEVICE_LIST* pAdd = AllocNewDeviceNode();
//
    if (!pAdd)
        return 0;
    memcpy(&pAdd->guid, &guid, sizeof(GUID));
    memcpy(pAdd->szInstallID, szInstallID, strlen(szInstallID));
    memcpy(pAdd->szName, szName, strlen(szName));
    memcpy(pAdd->szPath, szPath, strlen(szPath));
    pAdd->wIndex = wIndex;
    pAdd->wOrder = wOrder;
    pAdd->pNext = _pHead->pNext;
    _pHead->pNext = pAdd;
    return 1;
};
//
char FindDeviceName(const char* szName, const UINT nIDList1,
                    const UINT nIDList2)
{
    DEVICE_LIST *pList = _pHead->pNext;
//
    while (pList)
    {
        if (!strcmp(pList->szName, szName))
        {
            GetOtherInfo(pList->guid, pList->wOrder, nIDList1, nIDList2);
            GetDeviceDetailInfo(pList);
            return 1;
        };
        pList = pList->pNext;
    };
    return 0;
};
//
void FreeAllocDeviceNode()
{
    DEVICE_LIST *pDel = _pHead->pNext;
    DEVICE_LIST *pTmp = 0L;
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
//
void GetDeviceDetailInfo(DEVICE_LIST* pList)
{
    char       szBuf[MAX_PATH] = {0};
    HWND       hList           = GetDlgItem(_hDlg, IDC_LIST1);
//
    ConvertGUIDToString(pList->guid, szBuf);
    ListViewInsertItemText(hList, 0, 1, szBuf);
    ListViewInsertItemText(hList, 2, 1, pList->szInstallID);
    sprintf(szBuf, "%d", pList->wIndex);
    ListViewInsertItemText(hList, 10, 1, szBuf);
    ListViewInsertItemText(hList, 11, 1, pList->szPath);
};
//
void InitialImageData()
{
    HIMAGELIST hImageListView  = 0L;
//
    SetupDiDestroyClassImageList(&_spImageData);
    RtlZeroMemory(&_spImageData, sizeof(SP_CLASSIMAGELIST_DATA));
    _spImageData.cbSize = sizeof(SP_CLASSIMAGELIST_DATA);
    SetupDiGetClassImageList(&_spImageData);
};
//
void DisplayDriverDetailInfo(HTREEITEM hTreeChild, const UINT nID, 
                             const char *szBuf, const int iImageIdx, const int iSelectImage)
{
    TVINSERTSTRUCT tvStruct = {0};
    HWND           hTree    = GetDlgItem(_hDlg, nID);
//
    tvStruct.hParent      = hTreeChild;
    tvStruct.hInsertAfter = TVI_LAST;
    tvStruct.item.mask    = TVIF_IMAGE | TVIF_TEXT | TVIF_SELECTEDIMAGE | TVIF_HANDLE;
    tvStruct.item.mask   |= TVIF_PARAM;
    tvStruct.item.lParam  = 1;
    tvStruct.item.pszText = (char*)szBuf;
    tvStruct.item.iImage  = iImageIdx;
    tvStruct.item.iSelectedImage = iImageIdx;
    SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvStruct);
};
//
void GetDeviceInstanceID(HDEVINFO hDevInfo,
                         SP_DEVINFO_DATA* pspDevInfoData,
                         char *szInstanceID)
{
    if (!SetupDiGetDeviceInstanceId(hDevInfo,
                                    pspDevInfoData,
                                    szInstanceID,
                                    LINE_LEN,
                                    0))
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiBuildDriverInfoList");
};
//
char EnumWDMDriver(const UINT nIdTree, const UINT nIdBmp)
{
    HDEVINFO        hDevInfo         = 0L;
    SP_DEVINFO_DATA spDevInfoData    = {0};
    short           wIndex           = 0;
    HTREEITEM       hTreeChild       = 0L;
//
    hTreeChild = MakeDeviceRootTree(_spImageData, nIdTree, nIdBmp);
    if (!hTreeChild)
        return 0;
//
    hDevInfo = SetupDiGetClassDevs(0L, 0L, _hDlg, DIGCF_PRESENT |
                                   DIGCF_ALLCLASSES | DIGCF_PROFILE);
    if (hDevInfo == (void*)-1)
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiGetClassDevs");
        return 0;
    };
//
    wIndex = 0;
    spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    SendMessage(GetDlgItem(_hDlg, nIdTree), TVM_SETIMAGELIST,
                TVSIL_NORMAL, (LPARAM)_spImageData.ImageList);
//
    while (1)
    {
        if (SetupDiEnumDeviceInfo(hDevInfo,
                                  wIndex,
                                  &spDevInfoData))
        {
            char  szBuf[MAX_PATH] = {0};
            short wImageIdx       = 0;
            short wItem           = 0;
//
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
                                          (int*)&wImageIdx))
            {
                TVINSERTSTRUCT         tvStruct        = {0};
                HWND                   hTree           = GetDlgItem(_hDlg, nIdTree);
                char                   szName[64]      = {0};
                char                   szID[LINE_LEN]  = {0};
                char                   szPath[MAX_PATH] = {0};
                HTREEITEM              hItem;
                DWORD                  dwRequireSize;
                short                  wOrder;
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
                if (!AddNewDeviceOrderNode(szBuf))
                {
                    wIndex++;
                    continue;
                };
                hItem = TreeViewFindChild(hTree, hTreeChild, szBuf);
                if (!hItem)
                {
                    tvStruct.hParent      = hTreeChild;
                    tvStruct.hInsertAfter = TVI_LAST;
                    tvStruct.item.mask    = TVIF_IMAGE | TVIF_TEXT | TVIF_SELECTEDIMAGE;
                    tvStruct.item.mask   |= TVIF_PARAM;
                    tvStruct.item.lParam  = 1;
                    tvStruct.item.pszText = szBuf;
                    tvStruct.item.iImage  = wImageIdx;
                    tvStruct.item.iSelectedImage = wImageIdx;
                    hItem = (HTREEITEM)SendMessage(hTree, TVM_INSERTITEM, 0, (LPARAM)&tvStruct);
                    wOrder = 0;
                };
                GetDeviceInstanceID(hDevInfo, &spDevInfoData, szID);
                GetDeviceInterfaceInfo(hDevInfo, spDevInfoData, szPath);
//
                if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                                     &spDevInfoData,
                                                     SPDRP_FRIENDLYNAME,
                                                     0L,
                                                     (PBYTE)szName,
                                                     63,
                                                     0))
                {
                    DisplayDriverDetailInfo(hItem, nIdTree, szName, wImageIdx, wImageIdx);
                    AddNewDeviceNode(spDevInfoData.ClassGuid, szName, szID, szPath, wIndex, wOrder);
                }
                else if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                                     &spDevInfoData,
                                                     SPDRP_DEVICEDESC,
                                                     0L,
                                                     (PBYTE)szName,
                                                     63,
                                                     0))
                {
                    DisplayDriverDetailInfo(hItem, nIdTree, szName, wImageIdx, wImageIdx);
                    AddNewDeviceNode(spDevInfoData.ClassGuid, szName, szID, szPath, wIndex, wOrder);
//                    if (!GetFirmwareEnvironmentVariable(szName, (LPCSTR)&spDevInfoData.ClassGuid, szBuf, 127))
//                        ShowErrorMsg(_hDlg, GetLastError(), "GetFirmwareEnvironmentVariable");
                };
            };
//            SetupDiDestroyDriverInfoList(hDevInfo, &spDevInfoData, SPDIT_COMPATDRIVER);
        }
        else
            break;
        wIndex++;
    };
    SendMessage(GetDlgItem(_hDlg, nIdTree), TVM_EXPAND,
                TVE_EXPAND, (LPARAM)hTreeChild);
    SendMessage(GetDlgItem(_hDlg, nIdTree), TVM_SORTCHILDREN,
                0, (LPARAM)hTreeChild);
    TreeView_SetItemState(GetDlgItem(_hDlg, nIdTree), hTreeChild, TVIS_SELECTED, TVIS_SELECTED);
//    SendMessage(GetDlgItem(_hDlg, nIdTree), TVM_SELECTITEM,
//                TVGN_FIRSTVISIBLE, (LPARAM)hTreeChild);
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return 1;
};
//
void GetMemoryResource(MEM_DES* pMemDes, const ULONG ulSize, const UINT nID)
{
    char  szBuf[128] = {0};
    HWND  hListView  = GetDlgItem(_hDlg, nID);
    short wLoop      = 0;
    short wCnt       = ListViewGetItemCount(hListView);
//
    sprintf(szBuf, "%08X - ", pMemDes->MD_Alloc_Base);
    sprintf(szBuf+strlen(szBuf), "%08X", pMemDes->MD_Alloc_End);
    if (GetACP() == 950)
        ListViewInsertItemText(hListView, wCnt, 0, "記憶體範圍");
    else
        ListViewInsertItemText(hListView, wCnt, 0, "Memory Range");
    ListViewInsertItemText(hListView, wCnt, 1, szBuf);
//
    if (pMemDes->MD_Count)
    {
        for (wLoop = 0; wLoop < (short)(ulSize - 
                               (LONG)(sizeof(MEM_DES) / pMemDes->MD_Type)); wLoop++)
        {
            MEM_RANGE *pMemRange = (MEM_RANGE*)(pMemDes + 1);// + pMemDes->MD_Type);
            if (pMemRange->MR_Min != 0 && pMemRange->MR_Max != 0 &&
                (pMemRange->MR_Min < pMemRange->MR_Max))
            {
                sprintf(szBuf, "%08X - ", pMemRange->MR_Min);
                sprintf(szBuf+strlen(szBuf), "%08X", pMemRange->MR_Max);
                if (GetACP() == 950)
                    ListViewInsertItemText(hListView, wCnt+wLoop+1, 0, "記憶體範圍");
                else
                    ListViewInsertItemText(hListView, wCnt+wLoop+1, 0, "Memory Range");
                ListViewInsertItemText(hListView, wCnt+wLoop+1, 1, szBuf);
            };
        };
    };
};
//
void GetIOResource(IO_DES *pIODes, const ULONG ulSize, const UINT nID)
{
    char  szBuf[128] = {0};
    HWND  hListView  = GetDlgItem(_hDlg, nID);
    short wLoop      = 0;
    short wCnt       = ListViewGetItemCount(hListView);
//
    sprintf(szBuf, "%04X - ", pIODes->IOD_Alloc_Base);
    sprintf(szBuf+strlen(szBuf), "%04X", pIODes->IOD_Alloc_End);
//
    if (GetACP() == 950)
        ListViewInsertItemText(hListView, wCnt, 0, "輸出/輸入範圍");
    else
        ListViewInsertItemText(hListView, wCnt, 0, "IO Range");
    ListViewInsertItemText(hListView, wCnt, 1, szBuf);
//
    if (pIODes->IOD_Count)
    {
        for (wLoop = 0; wLoop < (short)(ulSize -
                            (ULONG)(sizeof(IO_DES) / pIODes->IOD_Type)); wLoop++)
        {
            IO_RANGE *pIORange = (IO_RANGE*)(pIODes + 1); // + (wLoop * pIODes->IOD_Type));
            if (pIORange->IOR_Min != 0 && pIORange->IOR_Max != 0 &&
                (pIORange->IOR_Min < pIORange->IOR_Max))
            {
                sprintf(szBuf, "%04X - ", pIORange->IOR_Min);
                sprintf(szBuf+strlen(szBuf), "%04X", pIORange->IOR_Max);
                if (GetACP() == 950)
                    ListViewInsertItemText(hListView, wCnt+wLoop+1, 0, "輸出/輸入範圍");
                else
                    ListViewInsertItemText(hListView, wCnt, 0, "IO Range");
                ListViewInsertItemText(hListView, wCnt+wLoop+1, 1, szBuf);
            };
        };
    };
};
//
void GetDMAResource(DMA_DES* pDMADes, const ULONG ulSize, const UINT nID)
{
    char  szBuf[128] = {0};
    HWND  hListView  = GetDlgItem(_hDlg, nID);
    short wLoop      = 0;
    short wCnt       = ListViewGetItemCount(hListView);
//
    sprintf(szBuf, "%02d", pDMADes->DD_Alloc_Chan);
    ListViewInsertItemText(hListView, wCnt, 0, "DMA");
    ListViewInsertItemText(hListView, wCnt, 1, szBuf);
//
    if (pDMADes->DD_Count)
    {
        for (wLoop = 0; wLoop < (short)(ulSize -
                            (ULONG)(sizeof(DMA_DES) / pDMADes->DD_Type)); wLoop++)
        {
            DMA_RANGE *pDMARange = (DMA_RANGE*)(pDMADes + 1); // + (wLoop * pIODes->IOD_Type));
            if (pDMARange->DR_Min != 0 && pDMARange->DR_Max != 0 &&
                (pDMARange->DR_Min < pDMARange->DR_Max))
            {
                sprintf(szBuf, "%02d - ", pDMARange->DR_Min);
                sprintf(szBuf+strlen(szBuf), "%02d", pDMARange->DR_Max);
                ListViewInsertItemText(hListView, wCnt+wLoop+1, 0, "DMA");
                ListViewInsertItemText(hListView, wCnt+wLoop+1, 1, szBuf);
            };
        };
    };
};
//
void GetIRQResource(IRQ_DES* pIRQDes, const ULONG ulSize, const UINT nID)
{
    char  szBuf[128] = {0};
    HWND  hListView  = GetDlgItem(_hDlg, nID);
    short wLoop      = 0;
    short wCnt       = ListViewGetItemCount(hListView);
//
    sprintf(szBuf, "%02d", pIRQDes->IRQD_Alloc_Num);
    ListViewInsertItemText(hListView, wCnt, 0, "IRQ");
    ListViewInsertItemText(hListView, wCnt, 1, szBuf);
//
    if (pIRQDes->IRQD_Count)
    {
        for (wLoop = 0; wLoop < (short)(ulSize -
                            (ULONG)(sizeof(IRQ_DES) / pIRQDes->IRQD_Type)); wLoop++)
        {
            IRQ_RANGE *pIRQRange = (IRQ_RANGE*)(pIRQDes + 1); // + (wLoop * pIODes->IOD_Type));
            if (pIRQRange->IRQR_Min != 0 && pIRQRange->IRQR_Max != 0 &&
                (pIRQRange->IRQR_Min < pIRQRange->IRQR_Max))
            {
                sprintf(szBuf, "%02d - ", pIRQRange->IRQR_Min);
                sprintf(szBuf+strlen(szBuf), "%02d", pIRQRange->IRQR_Max);
                ListViewInsertItemText(hListView, wCnt+wLoop+1, 0, "IRQ");
                ListViewInsertItemText(hListView, wCnt+wLoop+1, 1, szBuf);
            };
        };
    };
};
//
void FindSpecResource(const DEVINST DevInst, const DWORD dwResType,
                      const short wOrder, const UINT nID)
{
    char  *pBuf  = 0L;
    short wHeaderSize;
    ULONG ulSize;
//
    CONFIGRET cmRet;
    LOG_CONF  firstLogConf;
    LOG_CONF  nextLogConf;
    RES_DES   rdPrevResDes;
//
    switch(dwResType)
    {
        case ResType_Mem:
            wHeaderSize = sizeof(MEM_DES);
        break;
//
        case ResType_IO:
            wHeaderSize = sizeof(IO_DES);
        break;
//
        case ResType_DMA:
            wHeaderSize = sizeof(DMA_DES);
        break;
//
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
            ShowErrorMsg(_hDlg, GetLastError(), "LocalAlloc");
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
        switch(dwResType)
        {
            case ResType_Mem:
                GetMemoryResource((MEM_DES*)pBuf, ulSize, nID);
            break;
//
            case ResType_IO:
                GetIOResource((IO_DES*)pBuf, ulSize, nID);
            break;
//
            case ResType_DMA:
                GetDMAResource((DMA_DES*)pBuf, ulSize, nID);
            break;
//
            case ResType_IRQ: 
                GetIRQResource((IRQ_DES*)pBuf, ulSize, nID);
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
//
void GetOtherInfo(GUID guid, const short wOrder,
          const UINT nIDList1, const UINT nIDList2)
{
    HDEVINFO         hDevInfo      = 0L;
    SP_DEVINFO_DATA  spDevInfoData = {0};
//
    hDevInfo = SetupDiGetClassDevs(&guid, 0L, _hDlg, DIGCF_PRESENT);
    if (hDevInfo == (void*)-1)
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiGetClassDevs");
        return;
    };
//
//    wIndex = 0;
    spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    if (SetupDiEnumDeviceInfo(hDevInfo,
                              wOrder,
                              &spDevInfoData))
    {
        SP_DRVINFO_DATA        spDrvInfoData      = {0};
        SP_DRVINFO_DETAIL_DATA spDrvInfoDetail    = {0};
        char                   szHardware[128]    = {0};
        HWND                   hList              = GetDlgItem(_hDlg, nIDList1);
        DWORD                  dwRequireSize;
        short                  wIdx;
//
        GetMoreInformation(hDevInfo, spDevInfoData, nIDList1);
        // Show Resource Information  
        for (wIdx = ResType_Mem; wIdx <= ResType_IRQ; wIdx++)
            FindSpecResource(spDevInfoData.DevInst,
                             wIdx, wOrder, nIDList2);
//
        if (!SetupDiBuildDriverInfoList(hDevInfo,
                                        &spDevInfoData,
                                        SPDIT_COMPATDRIVER))
            ShowErrorMsg(_hDlg, GetLastError(), "SetupDiBuildDriverInfoList");
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
                char szBuf[2048] = {0};
//
                memcpy(&spDrvInfoDetail, szBuf, sizeof(SP_DRVINFO_DETAIL_DATA));
                spDrvInfoDetail.cbSize = sizeof(SP_DRVINFO_DETAIL_DATA);
                dwRequireSize = 0;
                if (SetupDiGetDriverInfoDetail(hDevInfo,
                                               &spDevInfoData,
                                               &spDrvInfoData,
                                               &spDrvInfoDetail,
                                               2048,
                                               &dwRequireSize))
                {
                    SYSTEMTIME sysTime   = {0};
                    char       szTmp[64] = {0};
//
                    memcpy(szHardware, spDrvInfoDetail.HardwareID,
                           strlen(spDrvInfoDetail.HardwareID));
                    ListViewInsertItemText(hList, 1, 1, szHardware);
                    ListViewInsertItemText(hList, 3, 1, spDrvInfoData.MfgName);
                    ListViewInsertItemText(hList, 4, 1, spDrvInfoData.ProviderName);
                    ListViewInsertItemText(hList, 5, 1, spDrvInfoData.Description);
                    FileTimeToSystemTime(&spDrvInfoData.DriverDate, &sysTime);
                    sprintf(szTmp, "%02d/%02d/%04d", sysTime.wMonth,
                             sysTime.wDay, sysTime.wYear);
                    ListViewInsertItemText(hList, 7, 1, szTmp);
                    ListViewInsertItemText(hList, 6, 1, spDrvInfoDetail.SectionName);
                    ListViewInsertItemText(hList, 8, 1, spDrvInfoDetail.InfFileName);
                }
                else
                    RtlZeroMemory(szHardware, sizeof(char)*128);
            }
            else
            {
                DWORD dwError = GetLastError();
//
                if (dwError != ERROR_NO_MORE_ITEMS)
                    ShowErrorMsg(_hDlg, dwError, "SetupDiEnumDriverInfo");
                break;
            };
            SetupDiDestroyDriverInfoList(hDevInfo, &spDevInfoData, SPDIT_COMPATDRIVER);
        };
        SetupDiDestroyDeviceInfoList(hDevInfo);
    };
};
//
void GetDeviceInterfaceInfo(HDEVINFO hDevInfo, SP_DEVINFO_DATA spDevInfoData,
                            char *szPath)
{
    SP_DEVICE_INTERFACE_DATA spDevInterfaceData = {0};
//
    spDevInterfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    if (!SetupDiCreateDeviceInterface(hDevInfo,
                                      &spDevInfoData,
                                      &spDevInfoData.ClassGuid,
                                      0L,
                                      0L,
                                      &spDevInterfaceData))
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiBuildDriverInfoList");
    else
    {
        SP_DEVICE_INTERFACE_DETAIL_DATA *pspDevInterfaceDetail = 0L;
        DWORD                           dwRequire              = 0L;
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
                ShowErrorMsg(_hDlg, dwError, "SetupDiBuildDriverInfoList");
                return;
            };
        };
//
        pspDevInterfaceDetail = (SP_DEVICE_INTERFACE_DETAIL_DATA*)LocalAlloc(LPTR,
                                    sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA)*dwRequire);
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
                ShowErrorMsg(_hDlg, dwError, "SetupDiBuildDriverInfoList");
        }
        else
        {
            memcpy(szPath, pspDevInterfaceDetail->DevicePath,
                   strlen(pspDevInterfaceDetail->DevicePath));
//            switch(spDevInterfaceData.                    
        };
//
        if (pspDevInterfaceDetail)
            LocalFree(pspDevInterfaceDetail);
    };
};