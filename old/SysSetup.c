//
// (w)ritten by Chuan-Liang Teng 2006, mailto:clteng@ms6.hinet.net
//
#include "syssetup.h"
//
extern HWND _hDlg;
//
char StartDriver(const short wError, const short wStart,
                 const char* lpszDriverPath, const char* lpszServiceName)
{
    SC_HANDLE hSCManager = 0L;
    SC_HANDLE hService = 0L;
//
    hSCManager = OpenSCManager(0L,
                               0L,
                               SC_MANAGER_ALL_ACCESS);
  
    if (hSCManager)
    {
        hService = CreateService(hSCManager,
                                 lpszServiceName,
                                 lpszServiceName,
                                 SERVICE_ALL_ACCESS,
                                 SERVICE_KERNEL_DRIVER,
                                 wStart,
                                 wError,
                                 lpszDriverPath,
                                 0L,
                                 0L,
                                 0L,
                                 0L,
                                 0L);
  
        if (!hService)
        {
            DWORD dwErrorCode = GetLastError();
            if (dwErrorCode == ERROR_SERVICE_EXISTS)
            {
                hService = OpenService(hSCManager, lpszServiceName, SERVICE_ALL_ACCESS);
            }
            else
            {
                ShowErrorMsg(_hDlg, dwErrorCode, "CreateService");
                return 0;
            };
        };
//
        if (hService)
        {
            if (!StartService(hService, 0, 0L))
            {
                CloseServiceHandle(hService);
                ShowErrorMsg(_hDlg, GetLastError(), "StartService");
                return 0;
            };
        };
//
        CloseServiceHandle(hSCManager);
    };
//
    return 1;
};
//  
char StopDriver(const char* lpszServiceName)
{
    SC_HANDLE      hSCManager = 0L;
    SC_HANDLE      hService   = 0L;
    char           bResult    = 0;
    SERVICE_STATUS ServiceStatus;
  
    hSCManager = OpenSCManager(0L,
                               0L,
                               SC_MANAGER_ALL_ACCESS);
//
    if (hSCManager)
    {
        hService = OpenService(hSCManager,
                               lpszServiceName,
                               SERVICE_ALL_ACCESS);
        if (hService)
        {
            bResult = (char)ControlService(hService, SERVICE_CONTROL_STOP, &ServiceStatus);
            bResult = bResult && DeleteService(hService);
            CloseServiceHandle(hService);
        };
//
        CloseServiceHandle(hSCManager);
    };
//  
    return bResult;
};
//
void* OpenDevice(const char* lpszDevicePath)
{
    HANDLE hDevice = (HANDLE)-1;
//
    hDevice = CreateFile(lpszDevicePath,
                         GENERIC_READ | GENERIC_WRITE,
                         FILE_SHARE_READ | FILE_SHARE_WRITE,
                         0L,
                         OPEN_EXISTING,
                         0,
                         0L);
//
    return hDevice;
};
