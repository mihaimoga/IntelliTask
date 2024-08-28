/* Copyright (C) 2012-2024 Stefan-Mihai MOGA
This file is part of IntelliTask application developed by Stefan-Mihai MOGA.
IntelliTask is an alternative Windows version to the famous Task Manager!

IntelliTask is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliTask is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliTask. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

// IntelliTask.h : main header file for the IntelliTask application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "sinstance.h"

// CIntelliTaskApp:
// See IntelliTask.cpp for the implementation of this class
//

class CIntelliTaskApp : public CWinAppEx
{
public:
	CIntelliTaskApp();

public:
	CInstanceChecker m_pInstanceChecker;

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	bool EnableFullAccess();

public:
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CIntelliTaskApp theApp;
