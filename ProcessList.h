/* This file is part of IntelliTask application developed by Stefan-Mihai MOGA.

IntelliTask is free software: you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Open
Source Initiative, either version 3 of the License, or any later version.

IntelliTask is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
IntelliTask. If not, see <http://www.opensource.org/licenses/gpl-3.0.html>*/

#pragma once

#include "CpuUsage.h"

///////////////////////////////////////////////////////////////////////////////
// CProcessData command target
///////////////////////////////////////////////////////////////////////////////

class CProcessData : public CObject
{
protected:
	DECLARE_DYNAMIC(CProcessData)
	
public:
	CProcessData();
	CProcessData(DWORD dwProcessID, DWORD dwParentProcessID, DWORD dwPriority, DOUBLE cpuUsage, DWORD memUsage,
		CString strFileName, CString strFilePath, CString strDescription, CString strCompany, CString strVersion);
	virtual ~CProcessData();

public:
	DWORD GetProcessID() { return m_dwProcessID; }
	void SetProcessID(DWORD dwProcessID) { m_dwProcessID = dwProcessID; }
	DWORD GetParentProcessID() { return m_dwParentProcessID; }
	void SetParentProcessID(DWORD dwParentProcessID) { m_dwParentProcessID = dwParentProcessID; }
	DWORD GetPriority() { return m_dwPriority; }
	void SetPriority(DWORD dwPriority) { m_dwPriority = dwPriority; }
	DOUBLE GetProcessorUsage() { return m_cpuUsage; }
	void SetProcessorUsage(DOUBLE cpuUsage) { m_cpuUsage = cpuUsage; }
	DWORD GetMemoryUsage() { return m_memUsage; }
	void SetMemoryUsage(DWORD memUsage) { m_memUsage = memUsage; }
	CString GetFileName() { return m_strFileName; }
	void SetFileName(CString strFileName) { m_strFileName = strFileName; }
	CString GetFilePath() { return m_strFilePath; }
	void SetFilePath(CString strFilePath) { m_strFilePath = strFilePath; }
	CString GetDescription() { return m_strDescription; }
	void SetDescription(CString strDescription) { m_strDescription = strDescription; }
	CString GetCompany() { return m_strCompany; }
	void SetCompany(CString strCompany) { m_strCompany = strCompany; }
	CString GetVersion() { return m_strVersion; }
	void SetVersion(CString strVersion) { m_strVersion = strVersion; }

public:
	CpuUsage m_pCpuUsage;
protected:
	DWORD m_dwProcessID;
	DWORD m_dwParentProcessID;
	DWORD m_dwPriority;
	DOUBLE m_cpuUsage;
	DWORD m_memUsage;
	CString m_strFileName;
	CString m_strFilePath;
	CString m_strDescription;
	CString m_strCompany;
	CString m_strVersion;
};

typedef CArray<CProcessData*> CProcessList;

///////////////////////////////////////////////////////////////////////////////
// CSystemSnapshot command target
///////////////////////////////////////////////////////////////////////////////

class CSystemSnapshot : public CObject
{
protected:
	DECLARE_DYNAMIC(CSystemSnapshot)
	
public:
	CSystemSnapshot();
	virtual ~CSystemSnapshot();

public:
	BOOL RemoveAll();
	int GetSize() { return (int)m_arrProcessList.GetSize(); }
	CProcessData* GetAt(int nIndex) { return m_arrProcessList.GetAt(nIndex); }

	BOOL Refresh();
	BOOL InsertProcess(PROCESSENTRY32& pe32);
	CProcessData* UpdateProcess(DWORD dwProcessID);
	CProcessData* GetProcessID(DWORD dwProcessID);

protected:
	CProcessList m_arrProcessList;
};
