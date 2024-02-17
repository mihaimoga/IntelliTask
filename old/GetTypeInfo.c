//
// (w)ritten by Chuan-Liang Teng 2006, mailto:clteng@ms6.hinet.net
//
#include "gettypeinfo.h"
//
extern HWND _hDlg;
//
void ShowDevPropertyInfo(HWND hListView, char *szItemName,
                         char *szValue)
{
    short  wCount    = ListViewGetItemCount(hListView);
    char   szTmp[64] = {0};
    LVITEM lvItem    = {0};
//
    lvItem.iSubItem   = 1;
    lvItem.pszText    = szTmp;
    lvItem.cchTextMax = 63;
    SendMessage(hListView, LVM_GETITEMTEXT, wCount-1, (LPARAM)&lvItem);
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
            ListViewInsertItemText(hListView, wCount-1, 1, szValue);
    }
    else
    {
        if (szItemName)
            ListViewInsertItemText(hListView, wCount, 0, szItemName);
        else
            ListViewInsertItemText(hListView, wCount, 0, "");
        if (szValue)
            ListViewInsertItemText(hListView, wCount, 1, szValue);
    };
};
//
void GetMoreInformation(HDEVINFO hDevInfo, SP_DEVINFO_DATA spDevInfoData,
                        const UINT nIDList1)
{
    HWND  hListView  = GetDlgItem(_hDlg, nIDList1);
    GUID  guid       = {0};
    short wCount     = ListViewGetItemCount(hListView);
    char  bIsBIG5    = (GetACP() == 950) ? 1 : 0;
    char  szName[64] = {0};
    DWORD dwAddr;
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_BUSNUMBER,
                                         0L,
                                         (PBYTE)&dwAddr, //szName,
                                         63,
                                         0))
    {
        sprintf(szName, "%X", dwAddr);
        wCount = ListViewGetItemCount(hListView);
        if (bIsBIG5)
        {
            ListViewInsertItemText(hListView, wCount, 0, "匯流排編號");
            ListViewInsertItemText(hListView, wCount, 1, szName);
        }
        else
        {
            ListViewInsertItemText(hListView, wCount, 0, "BUS Number");
            ListViewInsertItemText(hListView, wCount, 1, szName);
        };
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_CAPABILITIES,
                                         0L,
                                         (PBYTE)&dwAddr, //szName,
                                         63,
                                         0))
    {
        if (dwAddr)
        {
            if (bIsBIG5)
                ShowDevPropertyInfo(hListView, "功能(Capability)", 0L);
            else
                ShowDevPropertyInfo(hListView, "Capability", 0L);
//
            if (dwAddr & CM_DEVCAP_LOCKSUPPORTED)
                ShowDevPropertyInfo(hListView, 0L, "LockSupported");
            if (dwAddr & CM_DEVCAP_EJECTSUPPORTED)
                ShowDevPropertyInfo(hListView, 0L, "EjectSupported");
            if (dwAddr & CM_DEVCAP_REMOVABLE)
                ShowDevPropertyInfo(hListView, 0L, "Removable");
            if (dwAddr & CM_DEVCAP_DOCKDEVICE)
                ShowDevPropertyInfo(hListView, 0L, "DockDevice");
            if (dwAddr & CM_DEVCAP_UNIQUEID)
                ShowDevPropertyInfo(hListView, 0L, "UniqueID");
            if (dwAddr & CM_DEVCAP_SILENTINSTALL)
                ShowDevPropertyInfo(hListView, 0L, "SilentInstall");
            if (dwAddr & CM_DEVCAP_RAWDEVICEOK)
                ShowDevPropertyInfo(hListView, 0L, "RawDeviceOK");
            if (dwAddr & CM_DEVCAP_SURPRISEREMOVALOK)
                ShowDevPropertyInfo(hListView, 0L, "SurpriseRemovalOK");
            if (dwAddr & CM_DEVCAP_HARDWAREDISABLED)
                ShowDevPropertyInfo(hListView, 0L, "HardwareDisabled");
            if (dwAddr & CM_DEVCAP_NONDYNAMIC)
                ShowDevPropertyInfo(hListView, 0L, "NonDynamic");
        };
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_CHARACTERISTICS,
                                         0L,
                                         (PBYTE)&dwAddr, //szName,
                                         63,
                                         0))
    {
        if (dwAddr)
        {
            if (bIsBIG5)
                ShowDevPropertyInfo(hListView, "特性(Characters)", 0L);
            else
                ShowDevPropertyInfo(hListView, "Characters", 0L);
//
            if (dwAddr & FILE_DEVICE_SECURE_OPEN)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_SECURE_OPEN");
            if (dwAddr & FILE_FLOPPY_DISKETTE)
                ShowDevPropertyInfo(hListView, 0L, "FLOPPY_DISKETTE");
            if (dwAddr & FILE_READ_ONLY_DEVICE)
                ShowDevPropertyInfo(hListView, 0L, "READ_ONLY_DEVICE");
            if (dwAddr & FILE_REMOVABLE_MEDIA)
                ShowDevPropertyInfo(hListView, 0L, "REMOVABLE_MEDIA");
            if (dwAddr & FILE_WRITE_ONCE_MEDIA)
                ShowDevPropertyInfo(hListView, 0L, "WRITE_ONCE_MEDIA");
            if (dwAddr & FILE_REMOTE_DEVICE)
                ShowDevPropertyInfo(hListView, 0L, "REMOTE_DEVICE");
            if (dwAddr & FILE_DEVICE_IS_MOUNTED)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_IS_MOUNTED");
            if (dwAddr & FILE_VIRTUAL_VOLUME)
                ShowDevPropertyInfo(hListView, 0L, "VIRTUAL_VOLUME");
            if (dwAddr & FILE_AUTOGENERATED_DEVICE_NAME)
                ShowDevPropertyInfo(hListView, 0L, "AUTOGENERATED_DEVICE_NAME");
        };
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_DEVTYPE,
                                         0L,
                                         (PBYTE)&dwAddr, //szName,
                                         63,
                                         0))
    {
        if (dwAddr)
        {
            if (bIsBIG5)
                ShowDevPropertyInfo(hListView, "設備型態(Device Type)", 0L);
            else
                ShowDevPropertyInfo(hListView, "Device Type", 0L);
//
            if (dwAddr & FILE_DEVICE_BEEP)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_BEEP");
            if (dwAddr & FILE_DEVICE_CD_ROM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_CD_ROM");
            if (dwAddr & FILE_DEVICE_CD_ROM_FILE_SYSTEM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_CD_ROM_FILE_SYSTEM");
            if (dwAddr & FILE_DEVICE_CONTROLLER)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_CONTROLLER");
            if (dwAddr & FILE_DEVICE_DATALINK)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_DATALINK");
            if (dwAddr & FILE_DEVICE_DFS)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_DFS");
            if (dwAddr & FILE_DEVICE_DISK)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_DISK");
            if (dwAddr & FILE_DEVICE_DISK_FILE_SYSTEM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_DISK_FILE_SYSTEM");
            if (dwAddr & FILE_DEVICE_FILE_SYSTEM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_FILE_SYSTEM");
            if (dwAddr & FILE_DEVICE_INPORT_PORT)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_INPORT_PORT");
            if (dwAddr & FILE_DEVICE_KEYBOARD)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_KEYBOARD");
            if (dwAddr & FILE_DEVICE_MAILSLOT)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_MAILSLOT");
            if (dwAddr & FILE_DEVICE_MIDI_IN)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_MIDI_IN");
            if (dwAddr & FILE_DEVICE_MIDI_OUT)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_MIDI_OUT");
            if (dwAddr & FILE_DEVICE_MOUSE)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_MOUSE");
            if (dwAddr & FILE_DEVICE_MULTI_UNC_PROVIDER)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_MULTI_UNC_PROVIDER");
            if (dwAddr & FILE_DEVICE_NAMED_PIPE)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_NAMED_PIPE");
            if (dwAddr & FILE_DEVICE_NETWORK)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_NETWORK");
            if (dwAddr & FILE_DEVICE_NETWORK_BROWSER)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_NETWORK_BROWSER");
            if (dwAddr & FILE_DEVICE_NETWORK_FILE_SYSTEM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_NETWORK_FILE_SYSTEM");
            if (dwAddr & FILE_DEVICE_NULL)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_NULL");
            if (dwAddr & FILE_DEVICE_PARALLEL_PORT)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_PARALLEL_PORT");
            if (dwAddr & FILE_DEVICE_PHYSICAL_NETCARD)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_PHYSICAL_NETCARD");
            if (dwAddr & FILE_DEVICE_PRINTER)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_PRINTER");
            if (dwAddr & FILE_DEVICE_SCANNER)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_SCANNER");
            if (dwAddr & FILE_DEVICE_SERIAL_MOUSE_PORT)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_SERIAL_MOUSE_PORT");
            if (dwAddr & FILE_DEVICE_SERIAL_PORT)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_SERIAL_PORT");
            if (dwAddr & FILE_DEVICE_SCREEN)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_SCREEN");
            if (dwAddr & FILE_DEVICE_SOUND)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_SOUND");
            if (dwAddr & FILE_DEVICE_STREAMS)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_STREAMS");
            if (dwAddr & FILE_DEVICE_TAPE)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_TAPE");
            if (dwAddr & FILE_DEVICE_TAPE_FILE_SYSTEM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_TAPE_FILE_SYSTEM");
            if (dwAddr & FILE_DEVICE_TRANSPORT)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_TRANSPORT");
            if (dwAddr & FILE_DEVICE_UNKNOWN)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_UNKNOWN");
            if (dwAddr & FILE_DEVICE_VIDEO)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_VIDEO");
            if (dwAddr & FILE_DEVICE_VIRTUAL_DISK)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_VIRTUAL_DISK");
            if (dwAddr & FILE_DEVICE_WAVE_IN)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_WAVE_IN");
            if (dwAddr & FILE_DEVICE_WAVE_OUT)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_WAVE_OUT");
            if (dwAddr & FILE_DEVICE_8042_PORT)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_8042_PORT");
            if (dwAddr & FILE_DEVICE_NETWORK_REDIRECTOR)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_NETWORK_REDIRECTOR");
            if (dwAddr & FILE_DEVICE_BATTERY)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_BATTERY");
            if (dwAddr & FILE_DEVICE_BUS_EXTENDER)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_BUS_EXTENDER");
            if (dwAddr & FILE_DEVICE_MODEM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_MODEM");
            if (dwAddr & FILE_DEVICE_VDM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_VDM");
            if (dwAddr & FILE_DEVICE_MASS_STORAGE)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_MASS_STORAGE");
            if (dwAddr & FILE_DEVICE_SMB)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_SMB");
            if (dwAddr & FILE_DEVICE_KS)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_KS");
            if (dwAddr & FILE_DEVICE_CHANGER)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_CHANGER");
            if (dwAddr & FILE_DEVICE_SMARTCARD)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_SMARTCARD");
            if (dwAddr & FILE_DEVICE_ACPI)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_ACPI");
            if (dwAddr & FILE_DEVICE_DVD)
                ShowDevPropertyInfo(hListView, 0L, "FILE_DEVICE_DVD");
            if (dwAddr & FILE_DEVICE_FULLSCREEN_VIDEO)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_FULLSCREEN_VIDEO");
            if (dwAddr & FILE_DEVICE_DFS_FILE_SYSTEM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_DFS_FILE_SYSTEM");
            if (dwAddr & FILE_DEVICE_DFS_VOLUME)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_DFS_VOLUME");
            if (dwAddr & FILE_DEVICE_SERENUM)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_SERENUM");
            if (dwAddr & FILE_DEVICE_TERMSRV)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_TERMSRV");
            if (dwAddr & FILE_DEVICE_KSEC)
                ShowDevPropertyInfo(hListView, 0L, "DEVICE_KSEC");
        };
    };
//
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_DRIVER,
                                         0L,
                                         (PBYTE)szName,
                                         63,
                                         0))
    {
        if (bIsBIG5)
            ShowDevPropertyInfo(hListView, "驅動程式(Driver)", szName);
        else
            ShowDevPropertyInfo(hListView, "Driver", szName);
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_ENUMERATOR_NAME,
                                         0L,
                                         (PBYTE)szName,
                                         63,
                                         0))
    {
        if (bIsBIG5)
            ShowDevPropertyInfo(hListView, "列舉名稱(Enumerator)", szName);
        else
            ShowDevPropertyInfo(hListView, "Enumerator", szName);
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_HARDWAREID,
                                         0L,
                                         (PBYTE)szName,
                                         63,
                                         0))
    {
        char *p = szName;
//
        if (bIsBIG5)
            ShowDevPropertyInfo(hListView, "硬體識別碼", 0L);
        else
            ShowDevPropertyInfo(hListView, "Hardware ID", 0L);
//
        while (*p)
        {
            ShowDevPropertyInfo(hListView, 0L, p);
            p = strchr(p, 0);
            *p++;
        };
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_LOCATION_INFORMATION,
                                         0L,
                                         (PBYTE)szName,
                                         63,
                                         0))
    {
        if (bIsBIG5)
            ShowDevPropertyInfo(hListView, "位置資訊", szName);
        else
            ShowDevPropertyInfo(hListView, "Local Information", szName);
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_LOWERFILTERS,
                                         0L,
                                         (PBYTE)szName,
                                         63,
                                         0))
    {
        if (bIsBIG5)
            ShowDevPropertyInfo(hListView, "下層過濾器名稱", szName);
        else
            ShowDevPropertyInfo(hListView, "Lower Filter", szName);
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_UPPERFILTERS,
                                         0L,
                                         (PBYTE)szName,
                                         63,
                                         0))
    {
        if (bIsBIG5)
            ShowDevPropertyInfo(hListView, "上層過濾器名稱", szName);
        else
            ShowDevPropertyInfo(hListView, "Upper Filter", szName);
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_PHYSICAL_DEVICE_OBJECT_NAME,
                                         0L,
                                         (PBYTE)szName,
                                         63,
                                         0))
    {
        if (bIsBIG5)
            ShowDevPropertyInfo(hListView, "實體設備物件名稱", szName);
        else
            ShowDevPropertyInfo(hListView, "Physical Device Object Name", szName);
    };
//
    if (SetupDiGetDeviceRegistryProperty(hDevInfo,
                                         &spDevInfoData,
                                         SPDRP_SERVICE,
                                         0L,
                                         (PBYTE)szName,
                                         63,
                                         0))
    {
        char *p = szName;
//
        if (bIsBIG5)
            ShowDevPropertyInfo(hListView, "服務名稱(Service Name)", 0L);
        else
            ShowDevPropertyInfo(hListView, "Service Name", 0L);
        if (*p)
            GetDriverName(GetDlgItem(_hDlg, nIDList1), szName);
        while (*p)
        {
            ShowDevPropertyInfo(hListView, 0L, p);
            p = strchr(p, 0);
            *p++;
            while (strchr(p, '\\'))
            {
                p = strchr(p, 0);
                *p++;
            };
        };
            
    };
};
//
void GetDriverName(HWND hListView, const char *szServiceName)
{
    HKEY  hKey             = 0L;
    char  szSubKey[128]    = {"SYSTEM\\CurrentControlSet\\Services\\\0"};
    char  szPath[MAX_PATH] = {0};
    DWORD cbData;
    DWORD dwType;
    DWORD dwStart;
//
    strcat(szSubKey, szServiceName);
    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE,
                     szSubKey,
                     0,
                     KEY_ALL_ACCESS,
                     &hKey) != ERROR_SUCCESS)
    {
        ListViewInsertItemText(hListView, 9, 1, "N/A");
        ListViewInsertItemText(hListView, 12, 1, "N/A");
        ListViewInsertItemText(hListView, 13, 1, "N/A");
        return;
    };
//
    cbData = MAX_PATH-1;
    dwType = REG_EXPAND_SZ;
    if (RegQueryValueEx(hKey, "ImagePath", 0L,
                        &dwType,
                        (unsigned char*)szPath,
                        &cbData) != ERROR_SUCCESS)
        ListViewInsertItemText(hListView, 9, 1, "N/A");
    else
    {    
        char szRoot[MAX_PATH] = {0};
//
        GetWindowsDirectory(szRoot, MAX_PATH-1);
        strcat(szRoot, "\\");
        strcat(szRoot, szPath);
        ListViewInsertItemText(hListView, 9, 1, szRoot);
    };
//
    RtlZeroMemory(szPath, MAX_PATH);
    cbData = MAX_PATH-1;
    dwType = REG_SZ;
    if (RegQueryValueEx(hKey, "Group", 0L,
                        &dwType,
                        (unsigned char*)szPath,
                        &cbData) != ERROR_SUCCESS)
        ListViewInsertItemText(hListView, 12, 1, "N/A");
    else
        ListViewInsertItemText(hListView, 12, 1, szPath);
//
    dwStart = 0;
    cbData  = sizeof(DWORD);
    dwType  = REG_DWORD;
    if (RegQueryValueEx(hKey, "Start", 0L,
                        &dwType,
                        (unsigned char*)&dwStart,
                        &cbData) != ERROR_SUCCESS)
        ListViewInsertItemText(hListView, 13, 1, "N/A");
    else
    {
        switch(dwStart)
        {
            case SERVICE_BOOT_START:
                strcpy(szPath, "BOOT START");
            break;
//
            case SERVICE_SYSTEM_START:
                strcpy(szPath, "SYSTEM START");
            break;
//
            case SERVICE_AUTO_START:
                strcpy(szPath, "AUTO START");
            break;
//
            case SERVICE_DEMAND_START:
                strcpy(szPath, "DEMAND START");
            break;
//
            case SERVICE_DISABLED:
                strcpy(szPath, "DISABLED");
            break;
//
        };
        ListViewInsertItemText(hListView, 13, 1, szPath);
    };
    RegCloseKey(hKey);
};