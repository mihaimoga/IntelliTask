/* Copyright (C) 2012-2024 Stefan-Mihai MOGA
This file is part of IntelliTask application developed by Stefan-Mihai MOGA.

IntelliTask is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliTask is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliTask.  If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

#pragma once

#include "ProcessList.h"

// CSortListCtrl

class CSortListCtrl : public CMFCListCtrl
{
	DECLARE_DYNAMIC(CSortListCtrl)

public:
	CSortListCtrl();
	virtual ~CSortListCtrl();

protected:
	virtual int OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn);

public:
	CSystemSnapshot* m_pSystemSnapshot;

protected:
	DECLARE_MESSAGE_MAP()
};
