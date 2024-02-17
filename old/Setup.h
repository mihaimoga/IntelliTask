#ifndef __ICE_SETUP_H
#define __ICE_SETUP_H
//
#ifdef __cplusplus
extern "C" {
#endif
//
#pragma once
//
#include "devmgr.h"
#include <cfgmgr32.h>
#include <newdev.h>
#include <setupapi.h>
#include <shlobj.h>
#include "gettypeinfo.h"
#include "util.h"
//
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "newdev.lib")
#pragma comment(lib, "shell32.lib")
//
typedef struct _tagDEVICE_ORDER
{
    char szDevName[LINE_LEN];
    struct _tagDEVICE_ORDER *pNext;
} DEVICE_ORDER, *PDEVICE_ORDER;
//
typedef struct _tagDEVLIST
{
    GUID               guid;
    short              wOrder;
    short              wIndex;
    char               szInstallID[LINE_LEN];
    char               szName[MAX_PATH];
    char               szPath[MAX_PATH];
    struct _tagDEVLIST *pNext;
}DEVICE_LIST, *PDEVICE_LIST;
//
void InitialImageData();
void ListImageDataInitial(const UINT);
char EnumWDMDriver(const UINT, const UINT);
void GetDriverDetialInfo(HTREEITEM, const UINT, const char *, const int, const int);
void GetDeviceInstanceID(HDEVINFO, SP_DEVINFO_DATA*, char*);
void GetDeviceInterfaceInfo(HDEVINFO, SP_DEVINFO_DATA, char*);
void GetOtherInfo(GUID, const short, const UINT, const UINT);
void FindSpecResource(const DEVINST, const DWORD, const short, const UINT);
//
HTREEITEM MakeRootTree();
//
char InitialDeviceOrder();
DEVICE_ORDER* AllocNewDeviceOrderNode();
char AddNewDeviceOrderNode(const char*);
short FindDeviceOrder(const char*);
void FreeAllDeviceOrderNode();
//
char InitialDeviceList();
DEVICE_LIST* AllocNewDeviceNode();
char AddNewDeviceNode(const GUID, const char*, const char*,
                      const char*, const int, const short);
char FindDeviceName(const char*, const UINT, const UINT);
void GetDeviceDetailInfo(DEVICE_LIST*);
void DisplayDriverDetailInfo(HTREEITEM, const UINT, 
                             const char*, const int, const int);
void FreeAllocDeviceNode();
//
void GetMemoryResource(MEM_DES*, const ULONG, const UINT);
void GetIOResource(IO_DES*, const ULONG, const UINT);
void GetDMAResource(DMA_DES*, const ULONG, const UINT);
void GetIRQResource(IRQ_DES*, const ULONG, const UINT);
//
void GetMoreInformation(HDEVINFO, SP_DEVINFO_DATA, const UINT);
//
#ifdef __cplusplus
}
#endif
//
#endif
