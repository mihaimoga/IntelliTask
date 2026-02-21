/* Copyright (C) 2012-2026 Stefan-Mihai MOGA
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

// CheckForUpdatesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "IntelliTask.h"
#include "CheckForUpdatesDlg.h"

#include "genUp4win/genUp4win.h"

// Link appropriate genUp4win library based on platform and configuration
#if _WIN64
// 64-bit builds
#ifdef _DEBUG
#pragma comment(lib, "x64/Debug/genUp4win.lib")
#else
#pragma comment(lib, "x64/Release/genUp4win.lib")
#endif
#else
// 32-bit builds
#ifdef _DEBUG
#pragma comment(lib, "Debug/genUp4win.lib")
#else
#pragma comment(lib, "Release/genUp4win.lib")
#endif
#endif

/**
 * @brief Implementation of the Check For Updates dialog
 */

IMPLEMENT_DYNAMIC(CCheckForUpdatesDlg, CDialogEx)

/**
 * @brief Constructor for the Check For Updates dialog
 * @param pParent Pointer to the parent window (default: nullptr)
 */
CCheckForUpdatesDlg::CCheckForUpdatesDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_CheckForUpdatesDlg, pParent)
{
	// Initialize update thread tracking variables
	m_nUpdateThreadID = 0;
	m_hUpdateThread = nullptr;
	m_nTimerID = 0;
}

/**
 * @brief Destructor for the Check For Updates dialog
 */
CCheckForUpdatesDlg::~CCheckForUpdatesDlg()
{
}

/**
 * @brief Exchange data between dialog controls and member variables
 * @param pDX Pointer to the CDataExchange object
 */
void CCheckForUpdatesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	// Map dialog controls to member variables
	DDX_Control(pDX, IDC_STATUS, m_ctrlStatusMessage);
	DDX_Control(pDX, IDC_PROGRESS, m_ctrlProgress);
}

BEGIN_MESSAGE_MAP(CCheckForUpdatesDlg, CDialogEx)
	ON_WM_TIMER()
END_MESSAGE_MAP()

/**
 * @brief Global pointer to the Check For Updates dialog instance
 */
CCheckForUpdatesDlg* g_dlgCheckForUpdates = nullptr;

/**
 * @brief Callback function to update UI with status messages during update check
 * @param Unused parameter (callback ID)
 * @param strMessage Status message to display to the user
 */
void UI_Callback(int, const std::wstring& strMessage)
{
	if (g_dlgCheckForUpdates != nullptr)
	{
		// Update the status message control with the current operation
		g_dlgCheckForUpdates->m_ctrlStatusMessage.SetWindowText(strMessage.c_str());
		// Force immediate redraw to show the message
		g_dlgCheckForUpdates->m_ctrlStatusMessage.UpdateWindow();
	}
}

/**
 * @brief Global flag indicating whether the update check thread is currently running
 */
bool g_bThreadRunning = false;

/**
 * @brief Global flag indicating whether a new update was found
 */
bool g_bNewUpdateFound = false;

/**
 * @brief Thread procedure that performs the update check operation
 * @param lpParam Pointer to thread parameters (unused)
 * @return Thread exit code (always 0)
 */
DWORD WINAPI UpdateThreadProc(LPVOID lpParam)
{
	UNREFERENCED_PARAMETER(lpParam);

	// Signal that the thread has started
	g_bThreadRunning = true;

	// Start the progress bar marquee animation
	if (g_dlgCheckForUpdates != nullptr)
	{
		g_dlgCheckForUpdates->m_ctrlProgress.SetMarquee(TRUE, 30);
	}

	// Get the full path to the current executable
	const DWORD nLength = 0x1000 /* _MAX_PATH */;
	TCHAR lpszFilePath[nLength] = { 0, };
	GetModuleFileName(nullptr, lpszFilePath, nLength);

	// Perform the actual update check and store the result
	g_bNewUpdateFound = CheckForUpdates(lpszFilePath, APPLICATION_URL, UI_Callback);

	// Stop the progress bar marquee animation
	if (g_dlgCheckForUpdates != nullptr)
	{
		g_dlgCheckForUpdates->m_ctrlProgress.SetMarquee(FALSE, 30);
	}

	// Signal that the thread is finishing
	g_bThreadRunning = false;

	::ExitThread(0);
	// return 0;
}

/**
 * @brief Initialize the dialog and start the update check thread
 * @return TRUE unless focus is set to a control
 */
BOOL CCheckForUpdatesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

#ifdef _DEBUG
	// In debug builds, write configuration file for testing purposes
	const DWORD nLength = 0x1000 /* _MAX_PATH */;
	TCHAR lpszFilePath[nLength] = { 0, };
	GetModuleFileName(nullptr, lpszFilePath, nLength);
	WriteConfigFile(lpszFilePath, INSTALLER_URL);
#endif

	// Store global pointer to this dialog for callback access
	g_dlgCheckForUpdates = this;

	// Create the update check thread
	m_hUpdateThread = ::CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)UpdateThreadProc, this, 0, &m_nUpdateThreadID);

	// Start timer to monitor thread completion (fires every 100ms)
	m_nTimerID = SetTimer(0x1234, 100, nullptr);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Handle dialog cancellation by waiting for the update thread to complete
 */
void CCheckForUpdatesDlg::OnCancel()
{
	// Wait for the update thread to finish before closing the dialog
	while (g_bThreadRunning)
		Sleep(1000);

	CDialogEx::OnCancel();
}

/**
 * @brief Handle timer events to monitor update thread completion
 * @param nIDEvent Timer identifier
 */
void CCheckForUpdatesDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialogEx::OnTimer(nIDEvent);

	// Check if this is our monitoring timer
	if (m_nTimerID == nIDEvent)
	{
		// Check if the update thread has completed
		if (!g_bThreadRunning)
		{
			// Stop the timer as we no longer need to monitor
			VERIFY(KillTimer(m_nTimerID));

			// Close the dialog
			CDialogEx::OnCancel();

			// If an update was found, exit the application to allow the update to proceed
			if (g_bNewUpdateFound)
			{
				PostQuitMessage(0);
			}
		}
	}
}
