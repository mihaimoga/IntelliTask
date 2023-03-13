/* This file is part of IntelliTask application developed by Stefan-Mihai MOGA.

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
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "IntelliTask.h"
#include "MainFrame.h"

#include "VersionInfo.h"
#include "HyperlinkStatic.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CIntelliTaskApp

BEGIN_MESSAGE_MAP(CIntelliTaskApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CIntelliTaskApp::OnAppAbout)
END_MESSAGE_MAP()

// CIntelliTaskApp construction

CIntelliTaskApp::CIntelliTaskApp() : m_pInstanceChecker(_T("IntelliTask"))
{
	EnableHtmlHelp();

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("IntelliTask.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CIntelliTaskApp object

CIntelliTaskApp theApp;

// CIntelliTaskApp initialization

BOOL CIntelliTaskApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction(FALSE);

	// AfxInitRichEdit2() is required to use RichEdit control	
	AfxInitRichEdit2();

	//Check for the previous instance as soon as possible
	if (m_pInstanceChecker.PreviousInstanceRunning())
	{
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);

		AfxMessageBox(_T("Previous version detected, will now restore it..."), MB_OK | MB_ICONINFORMATION);
		m_pInstanceChecker.ActivatePreviousInstance(cmdInfo.m_strFileName);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Mihai Moga"));

	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// EnableFullAccess();

	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMainFrame* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create and load the frame with its resources
	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);

	// The one and only window has been initialized, so show and update it
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand

	// If this is the first instance of our App then track it so any other instances can find us
	m_pInstanceChecker.TrackFirstInstanceRunning(m_pMainWnd->GetSafeHwnd());

	return TRUE;
}

int CIntelliTaskApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

BOOL CIntelliTaskApp::EnableFullAccess()
{
	HANDLE hToken = NULL;

	// List of privileges, all of them, he he ;)
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

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		for (int nIndex = 0; lpctszPrivileges[nIndex]; ++nIndex)
		{
			::ZeroMemory(&tokenPrivilege, sizeof(tokenPrivilege));
			LookupPrivilegeValue(NULL,
				lpctszPrivileges[nIndex],
				&tokenPrivilege.Privileges[0].Luid);
			tokenPrivilege.PrivilegeCount = 1;
			tokenPrivilege.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			AdjustTokenPrivileges(hToken, FALSE, &tokenPrivilege, 0, 0, 0);
		}// End for

		CloseHandle(hToken);
	}

	return TRUE;
}

// CIntelliTaskApp message handlers

// CAboutDlg dialog used for App About

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
	CFont m_fontCourier;
	CStatic m_ctrlVersion;
	CEdit m_ctrlWarning;
	CVersionInfo m_pVersionInfo;
	CHyperlinkStatic m_ctrlWebsite;
	CHyperlinkStatic m_ctrlEmail;
	CHyperlinkStatic m_ctrlContributors;

	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_VERSION, m_ctrlVersion);
	DDX_Control(pDX, IDC_WARNING, m_ctrlWarning);
	DDX_Control(pDX, IDC_WEBSITE, m_ctrlWebsite);
	DDX_Control(pDX, IDC_EMAIL, m_ctrlEmail);
	DDX_Control(pDX, IDC_CONTRIBUTORS, m_ctrlContributors);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	TCHAR lpszDrive[_MAX_DRIVE];
	TCHAR lpszDirectory[_MAX_DIR];
	TCHAR lpszFilename[_MAX_FNAME];
	TCHAR lpszExtension[_MAX_EXT];
	TCHAR lpszFullPath[_MAX_PATH];

	VERIFY(0 == _tsplitpath_s(AfxGetApp()->m_pszHelpFilePath, lpszDrive, _MAX_DRIVE, lpszDirectory, _MAX_DIR, lpszFilename, _MAX_FNAME, lpszExtension, _MAX_EXT));
	VERIFY(0 == _tmakepath_s(lpszFullPath, _MAX_PATH, lpszDrive, lpszDirectory, lpszFilename, _T(".exe")));

	if (m_pVersionInfo.Load(lpszFullPath))
	{
		CString strName = m_pVersionInfo.GetProductName().c_str();
		CString strVersion = m_pVersionInfo.GetProductVersionAsString().c_str();
		strVersion.Replace(_T(" "), _T(""));
		strVersion.Replace(_T(","), _T("."));
		const int nFirst = strVersion.Find(_T('.'));
		const int nSecond = strVersion.Find(_T('.'), nFirst + 1);
		strVersion.Truncate(nSecond);
		m_ctrlVersion.SetWindowText(strName + _T(" version ") + strVersion);
	}

	// set up window's terminal fixed font
	LOGFONT logfont;
	memset(&logfont, 0, sizeof(logfont));
	logfont.lfCharSet = OEM_CHARSET;
	logfont.lfPitchAndFamily = FIXED_PITCH;
	// logfont.lfQuality = ANTIALIASED_QUALITY;
	logfont.lfOutPrecision = OUT_TT_PRECIS;
	logfont.lfWeight = FW_NORMAL;
	logfont.lfHeight = -MulDiv(6, GetDeviceCaps(::GetDC(NULL), LOGPIXELSY), 72);
	_tcscpy_s(logfont.lfFaceName, LF_FACESIZE, _T("Tahoma"));
	VERIFY(m_fontCourier.CreateFontIndirect(&logfont));

	m_ctrlWarning.SetFont(&m_fontCourier);
	m_ctrlWarning.SetWindowText(_T("THE SOFTWARE IS PROVIDED \"AS-IS\" AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL STEFAN-MIHAI MOGA BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND, OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE."));

	m_ctrlWebsite.SetHyperlink(_T("https://www.emvs.site/"));
	m_ctrlEmail.SetHyperlink(_T("mailto:contact@emvs.site"));
	m_ctrlContributors.SetHyperlink(_T("https://github.com/mihaimoga/IntelliTask/graphs/contributors"));

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnDestroy()
{
	CDialog::OnDestroy();

	VERIFY(m_fontCourier.DeleteObject());
}

// App command to run the dialog
void CIntelliTaskApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CIntelliTaskApp customization load/save methods

void CIntelliTaskApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CIntelliTaskApp::LoadCustomState()
{
}

void CIntelliTaskApp::SaveCustomState()
{
}

// CIntelliTaskApp message handlers
