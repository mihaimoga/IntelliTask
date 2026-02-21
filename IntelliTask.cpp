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

// IntelliTask.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "IntelliTask.h"
#include "MainFrame.h"

#include "VersionInfo.h"
#include "HLinkCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Message map for the CIntelliTaskApp class
 */
BEGIN_MESSAGE_MAP(CIntelliTaskApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CIntelliTaskApp::OnAppAbout)
END_MESSAGE_MAP()

/**
 * @brief Constructor for the CIntelliTaskApp class
 * 
 * Initializes the application with HTML help support, Restart Manager support,
 * and sets up the application ID for proper Windows integration.
 */
CIntelliTaskApp::CIntelliTaskApp() : m_pInstanceChecker(_T("IntelliTask"))
{
	// Enable HTML Help support for context-sensitive help
	EnableHtmlHelp();

	// === RESTART MANAGER SUPPORT ===
	// Support Restart Manager to allow Windows to close and restart the app
	// during system updates without losing user data
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

#ifdef _MANAGED
	// === COMMON LANGUAGE RUNTIME (CLR) SUPPORT ===
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// === APPLICATION ID SETUP ===
	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	// This ID is used by Windows for application identification and settings storage
	SetAppID(_T("IntelliTask.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/**
 * @brief The one and only CIntelliTaskApp object
 * 
 * This is the global application object that manages the entire application lifecycle.
 * There should be exactly one instance of this object per application.
 */
CIntelliTaskApp theApp;

/**
 * @brief Initialize the application instance
 * 
 * This method performs all initialization tasks including:
 * - Common controls initialization
 * - Socket and OLE initialization
 * - Single instance checking
 * - Registry settings
 * - Main frame window creation
 * 
 * @return TRUE if initialization succeeds, FALSE otherwise
 */
BOOL CIntelliTaskApp::InitInstance()
{
	// === COMMON CONTROLS INITIALIZATION ===
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles. Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	// === SOCKET INITIALIZATION ===
	// Initialize Windows Sockets for network communication
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// === OLE INITIALIZATION ===
	// Initialize OLE libraries for COM support
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Disable taskbar interaction for this application
	EnableTaskbarInteraction(FALSE);

	// === RICH EDIT CONTROL INITIALIZATION ===
	// AfxInitRichEdit2() is required to use RichEdit control	
	AfxInitRichEdit2();

	// === SINGLE INSTANCE CHECK ===
	// Check for the previous instance as soon as possible
	if (m_pInstanceChecker.PreviousInstanceRunning())
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);

		// Notify user and activate the existing instance
		AfxMessageBox(_T("Previous version detected, will now restore it..."), MB_OK | MB_ICONINFORMATION);
		m_pInstanceChecker.ActivatePreviousInstance(cmdInfo.m_strFileName);
		return FALSE;
	}

	// === REGISTRY SETTINGS ===
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Mihai Moga"));

	// === UI MANAGERS INITIALIZATION ===
	InitContextMenuManager();
	InitKeyboardManager();

	// Initialize tooltip manager with visual theme
	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// EnableFullAccess();

	// === MAIN WINDOW CREATION ===
	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;

	// Create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME, WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, nullptr, nullptr);

	// === DISPLAY MAIN WINDOW ===
	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// pFrame->MoveWindow(CRect(0, 0, 1214, 907));
	// pFrame->CenterWindow();

	// === TRACK FIRST INSTANCE ===
	// If this is the first instance of our App then track it so any other instances can find us
	m_pInstanceChecker.TrackFirstInstanceRunning(m_pMainWnd->GetSafeHwnd());

	return TRUE;
}

/**
 * @brief Clean up and exit the application instance
 * 
 * Performs cleanup tasks including terminating OLE libraries.
 * 
 * @return The exit code returned by CWinAppEx::ExitInstance()
 */
int CIntelliTaskApp::ExitInstance()
{
	// TODO: handle additional resources you may have added
	// Terminate OLE libraries before exit
	AfxOleTerm(FALSE);

	// Call base class cleanup and return exit code
	return CWinAppEx::ExitInstance();
}

/**
 * @brief Enable full access privileges for the application
 * 
 * Attempts to acquire all available system privileges including:
 * - Token creation, security, and debugging privileges
 * - System profile and driver loading privileges
 * - Backup, restore, and shutdown privileges
 * 
 * @return true if the token handle was opened successfully, false otherwise
 */
bool CIntelliTaskApp::EnableFullAccess()
{
	HANDLE hToken = nullptr;

	// === PRIVILEGE LIST ===
	// List of privileges, all of them, he he ;)
	// This comprehensive list includes all standard Windows privileges
	LPCTSTR lpctszPrivileges[] = { SE_CREATE_TOKEN_NAME,
								   SE_ASSIGNPRIMARYTOKEN_NAME,
								   SE_LOCK_MEMORY_NAME,
								   SE_INCREASE_QUOTA_NAME,
								   SE_UNSOLICITED_INPUT_NAME,
								   SE_MACHINE_ACCOUNT_NAME,
								   SE_TCB_NAME,
								   SE_SECURITY_NAME,
								   SE_TAKE_OWNERSHIP_NAME,
								   SE_LOAD_DRIVER_NAME,
								   SE_SYSTEM_PROFILE_NAME,
								   SE_SYSTEMTIME_NAME,
								   SE_PROF_SINGLE_PROCESS_NAME,
								   SE_INC_BASE_PRIORITY_NAME,
								   SE_CREATE_PAGEFILE_NAME,
								   SE_CREATE_PERMANENT_NAME,
								   SE_BACKUP_NAME,
								   SE_RESTORE_NAME,
								   SE_SHUTDOWN_NAME,
								   SE_DEBUG_NAME,
								   SE_AUDIT_NAME,
								   SE_SYSTEM_ENVIRONMENT_NAME,
								   SE_CHANGE_NOTIFY_NAME,
								   SE_REMOTE_SHUTDOWN_NAME,
								   SE_UNDOCK_NAME,
								   SE_SYNC_AGENT_NAME,
								   SE_ENABLE_DELEGATION_NAME,
								   SE_MANAGE_VOLUME_NAME,
								   0 };

	TOKEN_PRIVILEGES tokenPrivilege = { 0 };

	// === OPEN PROCESS TOKEN ===
	// Open the current process token with adjust and query privileges
	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		// === ENABLE EACH PRIVILEGE ===
		// Iterate through all privileges and attempt to enable them
		for (int nIndex = 0; lpctszPrivileges[nIndex]; ++nIndex)
		{
			// Zero out the structure for each privilege
			::ZeroMemory(&tokenPrivilege, sizeof(tokenPrivilege));

			// Look up the LUID for this privilege
			LookupPrivilegeValue(nullptr,
				lpctszPrivileges[nIndex],
				&tokenPrivilege.Privileges[0].Luid);

			// Set privilege count and enable flag
			tokenPrivilege.PrivilegeCount = 1;
			tokenPrivilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

			// Adjust the token privileges
			AdjustTokenPrivileges(hToken, false, &tokenPrivilege, 0, 0, 0);
		}// End for

		// Close the token handle
		CloseHandle(hToken);
	}

	return true;
}

/**
 * @brief About dialog class for displaying application information
 * 
 * Displays version information, copyright notice, GPL license text,
 * and hyperlinks to the website, email, and contributors page.
 */
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();

protected:
	CStatic m_ctrlVersion;
	CEdit m_ctrlWarning;
	CVersionInfo m_pVersionInfo;
	CHLinkCtrl m_ctrlWebsite;
	CHLinkCtrl m_ctrlEmail;
	CHLinkCtrl m_ctrlContributors;

	DECLARE_MESSAGE_MAP()
};

/**
 * @brief Constructor for the About dialog
 */
CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

/**
 * @brief Exchange data between dialog controls and member variables
 * @param pDX Pointer to the CDataExchange object
 */
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION, m_ctrlVersion);
	DDX_Control(pDX, IDC_WARNING, m_ctrlWarning);
	DDX_Control(pDX, IDC_WEBSITE, m_ctrlWebsite);
	DDX_Control(pDX, IDC_EMAIL, m_ctrlEmail);
	DDX_Control(pDX, IDC_CONTRIBUTORS, m_ctrlContributors);
}

/**
 * @brief Message map for the CAboutDlg class
 */
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/**
 * @brief Get the full path of the current module (executable)
 * 
 * Retrieves the fully qualified path to the executable file.
 * Dynamically increases buffer size if needed.
 * 
 * @param pdwLastError Optional pointer to receive the last error code
 * @return The full path to the module, or empty string on failure
 */
CString GetModuleFileName(_Inout_opt_ DWORD* pdwLastError = nullptr)
{
	CString strModuleFileName;
	DWORD dwSize{ _MAX_PATH };

	// Loop until we have a buffer large enough to hold the full path
	while (true)
	{
		// Allocate buffer and attempt to get the module file name
		TCHAR* pszModuleFileName{ strModuleFileName.GetBuffer(dwSize) };
		const DWORD dwResult{ ::GetModuleFileName(nullptr, pszModuleFileName, dwSize) };

		if (dwResult == 0)
		{
			// Failed to get module file name
			if (pdwLastError != nullptr)
				*pdwLastError = GetLastError();
			strModuleFileName.ReleaseBuffer(0);
			return CString{};
		}
		else if (dwResult < dwSize)
		{
			// Success - buffer was large enough
			if (pdwLastError != nullptr)
				*pdwLastError = ERROR_SUCCESS;
			strModuleFileName.ReleaseBuffer(dwResult);
			return strModuleFileName;
		}
		else if (dwResult == dwSize)
		{
			// Buffer too small - double the size and try again
			strModuleFileName.ReleaseBuffer(0);
			dwSize *= 2;
		}
	}
}

/**
 * @brief Initialize the About dialog
 * 
 * Loads version information from the module, formats the version string,
 * sets up the license text, and configures hyperlinks.
 * 
 * @return TRUE unless focus is set to a control
 */
BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// === GET MODULE PATH ===
	// Retrieve the full path to the executable
	CString strFullPath{ GetModuleFileName() };
	if (strFullPath.IsEmpty())
#pragma warning(suppress: 26487)
		return FALSE;

	// === LOAD VERSION INFORMATION ===
	// Extract version info from the executable
	if (m_pVersionInfo.Load(strFullPath.GetString()))
	{
		// Get product name and version
		CString strName = m_pVersionInfo.GetProductName().c_str();
		CString strVersion = m_pVersionInfo.GetProductVersionAsString().c_str();

		// Format version string (remove spaces, replace commas with dots)
		strVersion.Replace(_T(" "), _T(""));
		strVersion.Replace(_T(","), _T("."));

		// Truncate to major.minor version only
		const int nFirst = strVersion.Find(_T('.'));
		const int nSecond = strVersion.Find(_T('.'), nFirst + 1);
		strVersion.Truncate(nSecond);

		// Add 32-bit or 64-bit indicator
#if _WIN32 || _WIN64
#if _WIN64
		m_ctrlVersion.SetWindowText(strName + _T(" version ") + strVersion + _T(" (64-bit)"));
#else
		m_ctrlVersion.SetWindowText(strName + _T(" version ") + strVersion + _T(" (32-bit)"));
#endif
#endif
	}

	// === SET LICENSE TEXT ===
	// Display GPL v3 license notice
	m_ctrlWarning.SetWindowText(_T("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. You should have received a copy of the GNU General Public License along with this program. If not, see <https://www.gnu.org/licenses/>."));

	// === CONFIGURE HYPERLINKS ===
	// Set up website, email, and contributors links
	m_ctrlWebsite.SetHyperLink(_T("https://www.moga.doctor/"));
	m_ctrlEmail.SetHyperLink(_T("mailto:stefan-mihai@moga.doctor"));
	m_ctrlContributors.SetHyperLink(_T("https://github.com/mihaimoga/IntelliTask/graphs/contributors"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Handle dialog destruction
 */
void CAboutDlg::OnDestroy()
{
	CDialog::OnDestroy();
}

/**
 * @brief Display the About dialog
 * 
 * Command handler that creates and shows the application's About dialog.
 */
void CIntelliTaskApp::OnAppAbout()
{
	// Create and display the About dialog modally
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/**
 * @brief Perform tasks before loading state from registry
 * 
 * Loads the edit menu name from resources and adds it to the context menu manager.
 */
void CIntelliTaskApp::PreLoadState()
{
	bool bNameValid;
	CString strName;

	// Load the edit menu string resource
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);

	// Register the edit menu with the context menu manager
	// This allows the menu to be used in popup contexts throughout the application
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

/**
 * @brief Load custom application state from registry
 * 
 * Override this method to load custom settings when the application starts.
 */
void CIntelliTaskApp::LoadCustomState()
{
	// TODO: Override this method to load custom application state
	// This is called after the standard state is loaded from the registry
	// Use this to restore application-specific settings, window positions, etc.
}

/**
 * @brief Save custom application state to registry
 * 
 * Override this method to save custom settings when the application exits.
 */
void CIntelliTaskApp::SaveCustomState()
{
	// TODO: Override this method to save custom application state
	// This is called before the standard state is saved to the registry
	// Use this to persist application-specific settings, window positions, etc.
}

// CIntelliTaskApp message handlers
