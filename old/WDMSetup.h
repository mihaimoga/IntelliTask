#ifndef __ICE_SETUPLIB_H
#define __ICE_SETUPLIB_H
//
#pragma once
//
#ifdef __cplusplus
extern "C" {
#endif
//
#include <windows.h>
//#include <cfgmgr32.h>
#include <newdev.h>
#include <setupapi.h>
#include <tchar.h>
#include <stdio.h>
#include "util.h"
//
#pragma comment(lib, "setupapi.lib")
#pragma comment(lib, "newdev.lib")
//#pragma comment(lib, "comctl32.lib")
//
#ifndef MAX_DEVICE_ID_LEN
#define MAX_DEVICE_ID_LEN     200
#define MAX_DEVNODE_ID_LEN    MAX_DEVICE_ID_LEN
#define MAX_GUID_STRING_LEN   39          // 38 chars + terminator null
#define MAX_CLASS_NAME_LEN    32
#endif
//
void InitialGlobalVar();
//
char FindSectionName(FILE*, const char*);
char IsInstalled();
char GetSectionData(FILE*, const char*, const char);
char GetINFData(FILE*);
char StartInstallWDMDriver(const char*);
char InstallClassDriver(const char*);
void UninstallWDMDriver(const int, const char*);
//int  FindInstalledIndex(GUID, int*);
//
#ifdef __cplusplus
}
#endif
//
#endif
