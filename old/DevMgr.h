#ifndef __ICE_DEVMGR_H
#define __ICE_DEVMGR_H
//
#ifdef __cplusplus
extern "C" {
#endif
//
#pragma once
//
#include <windows.h>
#include <dbt.h>
#include <stdio.h>
#include "util.h"
#include "setup.h"
#include "wdmsetup.h"
#include "syssetup.h"
#include "resource.h"
//
INT_PTR CALLBACK MainDialog(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK InstallDialog(HWND, UINT, WPARAM, LPARAM);
//
void DisableRadioBtn(HWND, const char);
void GetInstallOption(HWND, short*, short*);
char OpenINFFile(const char, char*);
void SetCharSet(const UINT, const UINT);
void DeleteINFFile(const char*);
BOOL EnablePrivilege(LPCTSTR);
//
#ifdef __cplusplus
}
#endif
//
#endif