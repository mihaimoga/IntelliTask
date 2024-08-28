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

#pragma once

#include <windows.h>

class CpuUsage
{
public:
	CpuUsage(void);
	DOUBLE GetUsage();
	DWORD GetProcessID() { return m_dwProcessID; }
	void SetProcessID(DWORD dwProcessID) { m_dwProcessID = dwProcessID; }

private:
	ULONGLONG SubtractTimes(const FILETIME& ftA, const FILETIME& ftB);
	bool EnoughTimePassed();
	inline bool IsFirstRun() const { return (m_dwLastRun == 0); }

protected:
	DWORD m_dwProcessID;
	//system total times
	FILETIME m_ftPrevSysIdle;
	FILETIME m_ftPrevSysKernel;
	FILETIME m_ftPrevSysUser;
	//process times
	FILETIME m_ftPrevProcKernel;
	FILETIME m_ftPrevProcUser;
	// last results
	DOUBLE m_nCpuUsage;
	ULONGLONG m_dwLastRun;
	int m_nProcessors;

	volatile LONG m_lRunCount;
};
