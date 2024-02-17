//
// (w)ritten by Chuan-Liang Teng 2006, mailto:clteng@ms6.hinet.net
//
#include "wdmsetup.h"
//
extern HINSTANCE       _hInst;
extern HWND            _hDlg;
//
HDEVINFO               _hDevInfo           = 0L;
short                  _wVender            = 0;
short                  _wHardware          = 0;
char                   _szVender[20][64]   = {0};
char                   _szHardware[20][64] = {0};
char                   _szHID[MAX_PATH+1]  = {0};
//
void InitialGlobalVar()
{
    short wLoop;
//
    _wVender = _wHardware = 0;
    for (wLoop = 0; wLoop < 20; wLoop++)
    {
        RtlZeroMemory(_szVender[wLoop], sizeof(char)*64);
        RtlZeroMemory(_szHardware[wLoop], sizeof(char)*64);
    };
};
//
char FindSectionName(FILE *pFile, const char *szKey)
{
    char szData[256] = {0};
//
    if (!pFile)
        return 0;
//
    rewind(pFile);
    while (!feof(pFile))
    {
        fgets(szData, 255, pFile);
        szData[strlen(szData)-1] = 0;
        if (!strcmp(szKey, szData))
            return 1;
    };
    return 0;
};
//
char GetSectionData(FILE* pFile, const char* szKey, const char bIsVender)
{
    char szData[128] = {0};
//
    if (bIsVender)
        strcpy(szData, szKey);
    else
    {
        szData[0] = '[';
        strcat(szData, szKey);
        strcat(szData, "]");
    };
    if (!FindSectionName(pFile, szData))
        return 0;
//
    RtlZeroMemory(szData, sizeof(char)*128);
    while (!feof(pFile))
    {
        char *str = 0L;
//
        fgets(szData, 127, pFile);
        szData[strlen(szData)-1] = 0;
        StrLTrim(szData);
        StrRTrim(szData);
        if (!*szData)
            continue;
        if (szData[0] == ';')
            continue;
//
        if (strchr(szData, '['))
        {
            StrLTrim(szData);
            if (szData[0] != ';')
                return 1;
            else
                continue;
        };
//
        if (bIsVender)
            str = strchr(szData, '=');
        else
            str = strchr(szData, ',');
//
        if (*str)
        {
            char  szTmp[128] = {0};
            short pos = (short)(str - szData + 1);
//
            StrRight(szData, (short)(strlen(szData)-pos));
            StrLTrim(szData);
            StrRTrim(szData);
            FindComma(szData);
            if (bIsVender)
                memmove(_szVender[_wVender++], szData, strlen(szData));
            else
                memmove(_szHardware[_wHardware++], szData, strlen(szData));
        };
    };
    return 1;
};
//
char GetINFData(FILE *pFile)
{
    short wLoop;
//
    if (!_wVender || !_wHardware)
        InitialGlobalVar();
    if (!GetSectionData(pFile, "[Manufacturer]", 1))
        return 0;
//
    for (wLoop = 0; wLoop < _wVender; wLoop++)
        GetSectionData(pFile, _szVender[wLoop], 0);
    if (_wHardware)
    {
        if (IsInstalled())
            return 0;
        else
            return 1;
    };
    return 0;
};
//
char IsInstalled()
{
    HDEVINFO        hDevInfo      = 0L;
    SP_DEVINFO_DATA spDevInfoData = {0L};
    short           wIdx;
    char            bIsFound;
//
    hDevInfo = SetupDiGetClassDevs(0L, 0, 0, 
                                   DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (hDevInfo == (void*)-1)
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiGetClassDevs");
        return 0;
    };
//
    spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    wIdx = 0;
    bIsFound = 0;
    while (++wIdx)
    {
        if (SetupDiEnumDeviceInfo(hDevInfo, wIdx, &spDevInfoData))
        {
            char *ptr;
            char *pBuffer = 0L;
            DWORD dwData  = 0L;
            DWORD dwRetVal;
            DWORD dwBufSize = 0L;
//
            while (1)
            {
                dwRetVal = SetupDiGetDeviceRegistryProperty(hDevInfo,
                                                            &spDevInfoData,
                                                            SPDRP_HARDWAREID,
                                                            &dwData,
                                                            (PBYTE)pBuffer,
                                                            dwBufSize,
                                                            &dwBufSize);
                if (!dwRetVal)
                    dwRetVal = GetLastError();
                else
                    break;
                if (dwRetVal == ERROR_INVALID_DATA)
                    break;
                else if (dwRetVal == ERROR_INSUFFICIENT_BUFFER)
                {
                    if (pBuffer)
                        LocalFree(pBuffer);
                    pBuffer = (char*)LocalAlloc(LPTR, dwBufSize);
                }
                else
                {
                    ShowErrorMsg(_hDlg, dwRetVal, "SetupDiGetDeviceRegistryProperty");
                    SetupDiDestroyDeviceInfoList(hDevInfo);
                    return 0;
                };
            };
//
            if (dwRetVal == ERROR_INVALID_DATA) 
                continue;
//
            for (ptr = pBuffer; *ptr && (ptr < &pBuffer[dwBufSize]);
                 ptr += lstrlen(ptr)+sizeof(char))
            {
                short wLoop;
//
                for (wLoop = 0; wLoop < _wHardware; wLoop++)
                {
                    if (!strcmp(_szHardware[wLoop], ptr))
                    {
                        bIsFound = 1;
                        break;
                    };
                };
            };
            if (pBuffer)
                LocalFree(pBuffer);
            if (bIsFound)
                break;
        };
    };
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return bIsFound;
};
//
char StartInstallWDMDriver(const char *szInfName)
{
    HDEVINFO             hDevInfo  = 0L;
    GUID                 guid      = {0L};
    SP_DEVINSTALL_PARAMS spDevInst = {0L};
//
    char  szClass[MAX_CLASS_NAME_LEN] = {0L};
//
    if (!SetupDiGetINFClass(szInfName, &guid, szClass, MAX_CLASS_NAME_LEN, 0))
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiGetINFClass");
        return 0;
    };
//
    hDevInfo = SetupDiGetClassDevs(&guid, 0L, 0L,
                                   DIGCF_PRESENT | 
                                   DIGCF_ALLCLASSES | DIGCF_PROFILE);
    if (!hDevInfo)
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiGetClassDevs");
        return 0;
    };
//
    spDevInst.cbSize = sizeof(SP_DEVINSTALL_PARAMS);
    if (!SetupDiGetDeviceInstallParams(hDevInfo, 0L, &spDevInst))
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiGetDeviceInstallParams");
        return 0;
    };
//
    spDevInst.Flags   = DI_ENUMSINGLEINF;
    spDevInst.FlagsEx = DI_FLAGSEX_ALLOWEXCLUDEDDRVS;
    strcpy(spDevInst.DriverPath, szInfName);
    if (!SetupDiSetDeviceInstallParams(hDevInfo, 0, &spDevInst))
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiSetDeviceInstallParams");
        return 0;
    };
//
    if (!SetupDiBuildDriverInfoList(hDevInfo, 0, SPDIT_CLASSDRIVER))
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiDeviceInstallParams");
        return 0;
    };
//
    SetupDiDestroyDeviceInfoList(hDevInfo);
    return InstallClassDriver(szInfName);
};
//
char InstallClassDriver(const char *szINFName)
{
    GUID            guid                          = {0};
    SP_DEVINFO_DATA spDevData                     = {0};
    HDEVINFO        hDevInfo                      = 0L;
    char            className[MAX_CLASS_NAME_LEN] = {0};
    char            *pHID                         = 0L;
    short           wLoop;
    BOOL            bRebootRequired;
//
    if (!SetupDiGetINFClass(szINFName, &guid, className, MAX_CLASS_NAME_LEN, 0))
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiGetINFClass");
        return 0;
    };
//
    hDevInfo = SetupDiCreateDeviceInfoList(&guid, 0);
    if (hDevInfo == (void*)-1)
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiCreateDeviceInfoList");
        return 0;
    };
//
    spDevData.cbSize = sizeof(SP_DEVINFO_DATA);
    if (!SetupDiCreateDeviceInfo(hDevInfo,
                                 className,
                                 &guid,
                                 0L, 0L, DICD_GENERATE_ID,
                                 &spDevData))
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiCreateDeviceInfo");
        SetupDiDestroyDeviceInfoList(hDevInfo);
        return 0;
    };
//
    for (wLoop = 0; wLoop < _wHardware; wLoop++)
    {
        if (pHID)
            LocalFree(pHID);
//
        pHID = (char*)LocalAlloc(LPTR, strlen(_szHardware[wLoop])*2*sizeof(char));
        if (!pHID)
        {
            ShowErrorMsg(_hDlg, GetLastError(), "LocalAlloc");
            SetupDiDestroyDeviceInfoList(hDevInfo);
            return 0;
        };
//
        strcpy(pHID, _szHardware[wLoop]);
        if (!SetupDiSetDeviceRegistryProperty(hDevInfo,
                                              &spDevData,
                                              SPDRP_HARDWAREID,
                                              (PBYTE)pHID,
                                              (DWORD)(strlen(_szHardware[wLoop])*2*sizeof(char))))
        {
            ShowErrorMsg(_hDlg, GetLastError(), "SetupDiSetDeviceRegistryProperty");
            SetupDiDestroyDeviceInfoList(hDevInfo);
            LocalFree(pHID);
            return 0;
        };
//
        if (!SetupDiCallClassInstaller(DIF_REGISTERDEVICE,
                                       hDevInfo, &spDevData))
        {
            ShowErrorMsg(_hDlg, GetLastError(), "SetupDiCallClassInstaller");
            SetupDiDestroyDeviceInfoList(hDevInfo);
            LocalFree(pHID);
            return 0;
        };
//
        bRebootRequired = 0;
        if (!UpdateDriverForPlugAndPlayDevices(0L, _szHardware[wLoop],
                                               szINFName,
                                               INSTALLFLAG_FORCE,
                                               &bRebootRequired))
        {
            DWORD dwErrorCode = GetLastError();
//
            if (!SetupDiCallClassInstaller(DIF_REMOVE, hDevInfo, &spDevData))
                ShowErrorMsg(_hDlg, GetLastError(), "SetupDiCallClassInstaller(Remove)");
            ShowErrorMsg(_hDlg, dwErrorCode, "UpdateDriverForPlugAndPlayDevices");
            SetupDiDestroyDeviceInfoList(hDevInfo);
            LocalFree(pHID);
            return 0;
        };
        LocalFree(pHID);
        pHID = 0L;
    };
//    FindInstalledIndex(guid, pIndex);
    SetupDiDestroyDeviceInfoList(hDevInfo);
    MessageBox(_hDlg, "Install Successed!", "Information", MB_ICONINFORMATION);
    return 1;
};
//
int FindInstalledIndex(GUID guid, int *pIndex)
{
    int             nIdx          = 0;
    HDEVINFO        hDevInfo      = 0L;
    SP_DEVINFO_DATA spDevInfoData = {0};
//
    hDevInfo = SetupDiGetClassDevs(&guid, 0L, 0L,
                                   DIGCF_PRESENT | 
                                   DIGCF_ALLCLASSES | DIGCF_PROFILE);
    if (!hDevInfo)
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiGetClassDevs");
        return 0;
    };
//
    nIdx = 0;
    while(1)
    {
        spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        if (SetupDiEnumDeviceInfo(hDevInfo, nIdx, &spDevInfoData))
        {
            char  szBuf[2000] = {0};
            short wLoop;
            char  bFind;
//
            SetupDiGetDeviceRegistryProperty(hDevInfo,
                                             &spDevInfoData,
                                             SPDRP_HARDWAREID,
                                             0L,
                                             (PBYTE)szBuf,
                                             2000,
                                             0L);
            for (bFind = 0, wLoop = 0; wLoop < _wHardware; wLoop++)
            {
                if (!strcmp(_szHardware[wLoop], szBuf))
                    break;
            };
            if (bFind)
                break;
        }
        else
            break;
        nIdx++;
    };
    *pIndex = nIdx;
    return nIdx;
};
//
void UninstallWDMDriver(const int nIndex, const char *szHardware)
{
    SP_DEVINFO_DATA spDevInfoData = {0};
    HDEVINFO        hDevInfo      = 0L;
    int             nIdx;    
//
    hDevInfo = SetupDiGetClassDevs(0L,
                                   0L,
                                   _hDlg, 
                                   DIGCF_ALLCLASSES | DIGCF_PRESENT);
    if (hDevInfo == (void*)-1)
    {
        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiGetClassDevs");
        return;
    };
//
    nIdx = 0;
    while (1)
    {
        spDevInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
        if (SetupDiEnumDeviceInfo(hDevInfo, nIdx, &spDevInfoData))
        {
            char szBuf[2048] = {0};
//
            if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                                &spDevInfoData,
                                                SPDRP_HARDWAREID,
                                                0L,
                                                (PBYTE)szBuf,
                                                2048,
                                                0L))
            {
                if (!stricmp(szHardware, szBuf))
                {
                    if (!SetupDiRemoveDevice(hDevInfo, &spDevInfoData))
                        ShowErrorMsg(_hDlg, GetLastError(), "SetupDiRemoveDevice");
                    MessageBox(_hDlg, "Successed Uninstall!", "Notice", MB_ICONINFORMATION);
                    break;
                };
            };
        }
        else
            break;
        nIdx++;
    };
    SetupDiDestroyDeviceInfoList(hDevInfo);
    InitialGlobalVar();
    return;
};