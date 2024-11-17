/* Copyright (C) 2012-2025 Stefan-Mihai MOGA
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

// SortListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliTask.h"
#include "SortListCtrl.h"

// CSortListCtrl

IMPLEMENT_DYNAMIC(CSortListCtrl, CMFCListCtrl)

CSortListCtrl::CSortListCtrl()
{
	m_pSystemSnapshot = nullptr;
}

CSortListCtrl::~CSortListCtrl()
{
}

BEGIN_MESSAGE_MAP(CSortListCtrl, CMFCListCtrl)
END_MESSAGE_MAP()

// CSortListCtrl message handlers

int CSortListCtrl::OnCompareItems(LPARAM lParam1, LPARAM lParam2, int iColumn)
{
	if ((-1 == iColumn) || (1 == iColumn))
	{
		if (m_pSystemSnapshot != nullptr)
		{
			CProcessData* pParam1 = m_pSystemSnapshot->GetProcessID((DWORD)lParam1);
			CProcessData* pParam2 = m_pSystemSnapshot->GetProcessID((DWORD)lParam2);
			if ((pParam1 != nullptr) && (pParam2 != nullptr))
			{
				CString strFileName1 = pParam1->GetFileName();
				CString strFileName2 = pParam2->GetFileName();
				const int nRetVal = strFileName1.CompareNoCase(strFileName2);
				if (nRetVal == 0)
				{
					return (pParam1->GetProcessID() - pParam2->GetProcessID());
				}
				else
				{
					return nRetVal;
				}
			}
		}
	}
	else
	{
		if (0 == iColumn)
		{
			if (m_pSystemSnapshot != nullptr)
			{
				CProcessData* pParam1 = m_pSystemSnapshot->GetProcessID((DWORD)lParam1);
				CProcessData* pParam2 = m_pSystemSnapshot->GetProcessID((DWORD)lParam2);
				if ((pParam1 != nullptr) && (pParam2 != nullptr))
				{
					return (pParam1->GetProcessID() - pParam2->GetProcessID());
				}
			}
		}
		else
		{
			if (2 == iColumn)
			{
				if (m_pSystemSnapshot != nullptr)
				{
					CProcessData* pParam1 = m_pSystemSnapshot->GetProcessID((DWORD)lParam1);
					CProcessData* pParam2 = m_pSystemSnapshot->GetProcessID((DWORD)lParam2);
					if ((pParam1 != nullptr) && (pParam2 != nullptr))
					{
						// return (int)(pParam1->GetProcessorUsage() - pParam2->GetProcessorUsage());
						return (pParam1->GetProcessorUsage() < pParam2->GetProcessorUsage()) ? -1 : 1;
					}
				}
			}
			else
			{
				if (3 == iColumn)
				{
					if (m_pSystemSnapshot != nullptr)
					{
						CProcessData* pParam1 = m_pSystemSnapshot->GetProcessID((DWORD)lParam1);
						CProcessData* pParam2 = m_pSystemSnapshot->GetProcessID((DWORD)lParam2);
						if ((pParam1 != nullptr) && (pParam2 != nullptr))
						{
							return (pParam1->GetMemoryUsage() - pParam2->GetMemoryUsage());
						}
					}
				}
				else
				{
					if (4 == iColumn)
					{
						if (m_pSystemSnapshot != nullptr)
						{
							CProcessData* pParam1 = m_pSystemSnapshot->GetProcessID((DWORD)lParam1);
							CProcessData* pParam2 = m_pSystemSnapshot->GetProcessID((DWORD)lParam2);
							if ((pParam1 != nullptr) && (pParam2 != nullptr))
							{
								CString strDescription1 = pParam1->GetDescription();
								CString strDescription2 = pParam2->GetDescription();
								const int nRetVal = strDescription1.CompareNoCase(strDescription2);
								if (nRetVal == 0)
								{
									return (pParam1->GetProcessID() - pParam2->GetProcessID());
								}
								else
								{
									return nRetVal;
								}
							}
						}
					}
					else
					{
						if (5 == iColumn)
						{
							if (m_pSystemSnapshot != nullptr)
							{
								CProcessData* pParam1 = m_pSystemSnapshot->GetProcessID((DWORD)lParam1);
								CProcessData* pParam2 = m_pSystemSnapshot->GetProcessID((DWORD)lParam2);
								if ((pParam1 != nullptr) && (pParam2 != nullptr))
								{
									CString strCompany1 = pParam1->GetCompany();
									CString strCompany2 = pParam2->GetCompany();
									const int nRetVal = strCompany1.CompareNoCase(strCompany2);
									if (nRetVal == 0)
									{
										return (pParam1->GetProcessID() - pParam2->GetProcessID());
									}
									else
									{
										return nRetVal;
									}
								}
							}
						}
						else
						{
							if (6 == iColumn)
							{
								if (m_pSystemSnapshot != nullptr)
								{
									CProcessData* pParam1 = m_pSystemSnapshot->GetProcessID((DWORD)lParam1);
									CProcessData* pParam2 = m_pSystemSnapshot->GetProcessID((DWORD)lParam2);
									if ((pParam1 != nullptr) && (pParam2 != nullptr))
									{
										CString strVersion1 = pParam1->GetVersion();
										CString strVersion2 = pParam2->GetVersion();
										const int nRetVal = strVersion1.CompareNoCase(strVersion2);
										if (nRetVal == 0)
										{
											return (pParam1->GetProcessID() - pParam2->GetProcessID());
										}
										else
										{
											return nRetVal;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return 0;
}
