#ifndef __ICE_SYSSETUP_H
#define __ICE_SYSSETUP_H
//
#pragma once
//
#ifdef __cplusplus
extern "C" {
#endif
//
#include "devmgr.h"
#include <windows.h>
#include "util.h"
//
char StartDriver(const short, const short, const char*, const char*);
char StopDriver(const char*);
void* OpenDevice(const char*);
//
#ifdef __cplusplus
}
#endif
//
#endif