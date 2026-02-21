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

// MainFrame.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "IntelliTask.h"

#include "MainFrame.h"
#include "EnumDevicesDlg.h"
#include "EnumProgramsDlg.h"
#include "WebBrowserDlg.h"
#include "CheckForUpdatesDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/**
 * @brief Dynamic class implementation for CMainFrame
 * 
 * This macro enables runtime type information for the CMainFrame class.
 */
IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

/**
 * @brief Message map for the CMainFrame class
 * 
 * Maps Windows messages and command IDs to their respective handler functions.
 * Includes handlers for help commands, UI updates, process management, and social media links.
 */
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, &CFrameWndEx::OnHelpFinder)
	ON_COMMAND(ID_HELP, &CFrameWndEx::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, &CFrameWndEx::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, &CFrameWndEx::OnHelpFinder)
	ON_COMMAND(ID_VIEW_CAPTION_BAR, &CMainFrame::OnViewCaptionBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_CAPTION_BAR, &CMainFrame::OnUpdateViewCaptionBar)
	ON_COMMAND(ID_TOOLS_OPTIONS, &CMainFrame::OnOptions)
	ON_COMMAND(ID_PROPERTIES, &CMainFrame::OnProperties)
	ON_UPDATE_COMMAND_UI(ID_PROPERTIES, &CMainFrame::OnUpdateProperties)
	ON_COMMAND(ID_KILL_PROCESS, &CMainFrame::OnKillProcess)
	ON_UPDATE_COMMAND_UI(ID_KILL_PROCESS, &CMainFrame::OnUpdateKillProcess)
	ON_COMMAND(IDC_DEVICES, &CMainFrame::OnDevices)
	ON_COMMAND(IDC_PROGRAMS, &CMainFrame::OnPrograms)
	ON_COMMAND(IDC_TWITTER, &CMainFrame::OnTwitter)
	ON_COMMAND(IDC_LINKEDIN, &CMainFrame::OnLinkedin)
	ON_COMMAND(IDC_FACEBOOK, &CMainFrame::OnFacebook)
	ON_COMMAND(IDC_INSTAGRAM, &CMainFrame::OnInstagram)
	ON_COMMAND(IDC_ISSUES, &CMainFrame::OnIssues)
	ON_COMMAND(IDC_DISCUSSIONS, &CMainFrame::OnDiscussions)
	ON_COMMAND(IDC_WIKI, &CMainFrame::OnWiki)
	ON_COMMAND(IDC_USER_MANUAL, &CMainFrame::OnUserManual)
	ON_COMMAND(IDC_CHECK_FOR_UPDATES, &CMainFrame::OnCheckForUpdates)
END_MESSAGE_MAP()

/**
 * @brief Constructor for the CMainFrame class
 * 
 * Initializes member variables to nullptr.
 */
CMainFrame::CMainFrame()
{
	// Initialize main button and process view pointers
	m_MainButton = nullptr;
	m_pProcessView = nullptr;
}

/**
 * @brief Destructor for the CMainFrame class
 * 
 * Cleans up resources allocated by the main frame.
 */
CMainFrame::~CMainFrame()
{
}

/**
 * @brief Handle the WM_CREATE message
 * 
 * Creates and initializes the ribbon bar, docking manager, and visual manager.
 * Sets up the application UI elements when the main frame is created.
 * 
 * @param lpCreateStruct Pointer to a CREATESTRUCT structure
 * @return 0 on success, -1 on failure
 */
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// Call base class implementation first
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	// === DOCKING MANAGER SETUP ===
	// Enable Visual Studio 2005 style docking window behavior
	CDockingManager::SetDockingMode(DT_SMART);
	// Enable Visual Studio 2005 style docking window auto-hide behavior
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// === VISUAL MANAGER SETUP ===
	// Set the visual manager used to draw all user interface elements
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// === RIBBON BAR CREATION ===
	// Create and load the ribbon bar from resources
	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	// === APPLICATION BUTTON SETUP ===
	// Create application button but keep it hidden
	m_MainButton = new CMFCRibbonApplicationButton;
	m_MainButton->SetVisible(FALSE);
	m_wndRibbonBar.SetApplicationButton(m_MainButton, CSize());

	/* === STATUS BAR (COMMENTED OUT) ===
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	bool bNameValid;
	CString strTitlePane;
	bNameValid = strTitlePane.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(
		ID_STATUSBAR_PANE1, strTitlePane, TRUE, nullptr,
		_T("012345678901234567890123456789012345678901234567890123456789")), strTitlePane); */

	/* === CAPTION BAR (COMMENTED OUT) ===
	Create a caption bar :
	if (!CreateCaptionBar())
	{
		TRACE0("Failed to create caption bar\n");
		return -1;      // fail to create
	} */

	return 0;
}

/**
 * @brief Create the client area of the frame window
 * 
 * Creates and initializes the CProcessView as the main view for displaying process information.
 * 
 * @param lpcs Pointer to a CREATESTRUCT structure
 * @param pContext Pointer to a CCreateContext structure
 * @return TRUE on success, FALSE on failure
 */
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// === CREATE CONTEXT FOR PROCESS VIEW ===
	// Set up a new creation context for the process view
	CCreateContext pProcessContext;
	pProcessContext.m_pCurrentDoc = nullptr;
	pProcessContext.m_pCurrentFrame = this;
	pProcessContext.m_pLastView = nullptr;
	pProcessContext.m_pNewDocTemplate = nullptr;
	pProcessContext.m_pNewViewClass = RUNTIME_CLASS(CProcessView);

	// === CREATE AND INITIALIZE PROCESS VIEW ===
	// Create the process view as the main client area
	if ((m_pProcessView = (CProcessView*) CreateView(&pProcessContext, AFX_IDW_PANE_FIRST)) != nullptr)
	{
		// Show the view and perform initial update
		m_pProcessView->ShowWindow(SW_SHOW);
		m_pProcessView->OnInitialUpdate();

		// Set back-reference from view to frame
		m_pProcessView->m_pMainFrame = this;
	}

	// Call base class implementation
	return CFrameWndEx::OnCreateClient(lpcs, pContext);
}

/**
 * @brief Modify window parameters before the window is created
 * 
 * Removes the client edge style and registers a custom window class.
 * 
 * @param cs Reference to a CREATESTRUCT structure to modify
 * @return TRUE if the window should be created, FALSE otherwise
 */
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// Call base class first
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;

	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	// Remove the 3D client edge for a flat appearance
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	// Register a custom window class
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

/**
 * @brief Create and configure the caption bar
 * 
 * Creates a caption bar with a button, text, and bitmap image.
 * The caption bar provides additional UI elements at the top of the frame.
 * 
 * @return true on success, false on failure
 */
bool CMainFrame::CreateCaptionBar()
{
	// === CREATE CAPTION BAR ===
	// Create the caption bar window
	if (!m_wndCaptionBar.Create(WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS, this, ID_VIEW_CAPTION_BAR, -1, TRUE))
	{
		TRACE0("Failed to create caption bar\n");
		return false;
	}

	bool bNameValid;
	CString strTemp, strTemp2;

	// === CONFIGURE BUTTON ===
	// Add button to the caption bar
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButton(strTemp, ID_TOOLS_OPTIONS, CMFCCaptionBar::ALIGN_LEFT, FALSE);
	// Set button tooltip
	bNameValid = strTemp.LoadString(IDS_CAPTION_BUTTON_TIP);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetButtonToolTip(strTemp);

	// === CONFIGURE TEXT ===
	// Add text to the caption bar
	bNameValid = strTemp.LoadString(IDS_CAPTION_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetText(strTemp, CMFCCaptionBar::ALIGN_LEFT);

	// === CONFIGURE BITMAP ===
	// Add bitmap image to the caption bar
	m_wndCaptionBar.SetBitmap(IDB_INFO, RGB(255, 255, 255), FALSE, CMFCCaptionBar::ALIGN_LEFT);
	// Set image tooltip
	bNameValid = strTemp.LoadString(IDS_CAPTION_IMAGE_TIP);
	ASSERT(bNameValid);
	bNameValid = strTemp2.LoadString(IDS_CAPTION_IMAGE_TEXT);
	ASSERT(bNameValid);
	m_wndCaptionBar.SetImageToolTip(strTemp, strTemp2);

	return true;
}

/**
 * @brief Validate the object's state (Debug builds only)
 * 
 * Verifies that the object is in a valid state by calling the base class implementation.
 */
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

/**
 * @brief Dump the object's contents to a CDumpContext (Debug builds only)
 * 
 * Outputs diagnostic information about the object for debugging purposes.
 * 
 * @param dc Reference to a CDumpContext object
 */
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG

/**
 * @brief Handle the WM_SETFOCUS message
 * 
 * Forwards focus to the process view window.
 * 
 * @param pOldWnd Pointer to the window that lost focus (unused)
 */
void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// Forward focus to the view window
	// This ensures keyboard input goes to the process list
	m_pProcessView->SetFocus();
}

/**
 * @brief Route command messages to the appropriate handler
 * 
 * Gives the process view first chance to handle commands before
 * using the default frame window handling.
 * 
 * @param nID Command ID
 * @param nCode Notification code
 * @param pExtra Pointer to extra data (depends on nCode)
 * @param pHandlerInfo Pointer to command handler information
 * @return TRUE if the message was handled, FALSE otherwise
 */
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// Give the view first crack at the command
	if (m_pProcessView->OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// Otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/**
 * @brief Toggle the visibility of the caption bar
 * 
 * Shows or hides the caption bar and recalculates the frame layout.
 */
void CMainFrame::OnViewCaptionBar()
{
	// Toggle caption bar visibility
	m_wndCaptionBar.ShowWindow(m_wndCaptionBar.IsVisible() ? SW_HIDE : SW_SHOW);
	// Recalculate layout to adjust for visibility change
	RecalcLayout(FALSE);
}

/**
 * @brief Update the UI state for the caption bar menu item
 * 
 * Sets the check mark on the menu item based on caption bar visibility.
 * 
 * @param pCmdUI Pointer to the command UI object
 */
void CMainFrame::OnUpdateViewCaptionBar(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(m_wndCaptionBar.IsVisible());
}

/**
 * @brief Display the ribbon customization dialog
 * 
 * Allows users to customize the ribbon bar appearance and commands.
 */
void CMainFrame::OnOptions()
{
	// Create the ribbon customization dialog
	CMFCRibbonCustomizeDialog *pOptionsDlg = new CMFCRibbonCustomizeDialog(this, &m_wndRibbonBar);
	ASSERT(pOptionsDlg != nullptr);

	// Display the dialog modally (blocks until user closes it)
	pOptionsDlg->DoModal();
	// Clean up the dynamically allocated dialog
	delete pOptionsDlg;
}

/**
 * @brief Display the properties dialog for the selected process
 * 
 * Shows the Windows properties dialog for the executable file of the currently selected process.
 */
void CMainFrame::OnProperties()
{
	// === GET SELECTED PROCESS ===
	// Get the active process view
	CProcessView* pProcessView = (CProcessView*)GetActiveView();
	ASSERT_VALID(pProcessView);

	// Get the selected list item
	const int nListItem = pProcessView->GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);

	// Get the process data for the selected item
	CProcessData* pProcessData = pProcessView->m_pSystemSnapshot.GetProcessID((int)pProcessView->GetListCtrl().GetItemData(nListItem));
	ASSERT(pProcessData != nullptr);

	// === DISPLAY PROPERTIES ===
	// Get the file path of the process executable
	CString sPathName{ pProcessData->GetFilePath() };
	if (!sPathName.IsEmpty())
	{
		TRACE(sPathName);

		// Set up shell execute info structure
		SHELLEXECUTEINFO sei;
		memset(&sei, 0, sizeof(sei));
		sei.cbSize = sizeof(sei);
		sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
		sei.nShow = SW_SHOW;
		sei.lpFile = sPathName.GetBuffer(sPathName.GetLength());
		sei.lpVerb = _T("properties");  // Show properties dialog
		sei.fMask = SEE_MASK_INVOKEIDLIST;

		// Execute the properties verb to show the properties dialog
#pragma warning(suppress: 26486)
		ShellExecuteEx(&sei);
		sPathName.ReleaseBuffer();
	}
}

/**
 * @brief Update the UI state for the Properties command
 * 
 * Enables the Properties menu item only when a process is selected.
 * 
 * @param pCmdUI Pointer to the command UI object
 */
void CMainFrame::OnUpdateProperties(CCmdUI *pCmdUI)
{
	// Get the active process view
	CProcessView* pProcessView = (CProcessView*)GetActiveView();
	if ((pProcessView != nullptr) && (pProcessView->GetSafeHwnd() != nullptr))
	{
		// Enable only if an item is selected
		const int nListItem = pProcessView->GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
		pCmdUI->Enable(nListItem != -1);
	}
	else
		pCmdUI->Enable(false);
}

/**
 * @brief Terminate the selected process
 * 
 * Opens the selected process with PROCESS_TERMINATE access and terminates it.
 * Displays a confirmation message upon successful termination.
 */
void CMainFrame::OnKillProcess()
{
	// === GET SELECTED PROCESS ===
	// Get the active process view
	CProcessView* pProcessView = (CProcessView*)GetActiveView();
	ASSERT_VALID(pProcessView);

	// Get the selected list item
	const int nListItem = pProcessView->GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);

	// Get the process data for the selected item
	CProcessData* pProcessData = pProcessView->m_pSystemSnapshot.GetProcessID((int)pProcessView->GetListCtrl().GetItemData(nListItem));

	// === TERMINATE PROCESS ===
	// Open the process with terminate access
	DWORD dwDesiredAccess = PROCESS_TERMINATE;
	BOOL  bInheritHandle = FALSE;
	HANDLE hProcess = OpenProcess(dwDesiredAccess, bInheritHandle, pProcessData->GetProcessID());

	if (hProcess != NULL)
	{
		// Attempt to terminate the process
		bool result = TerminateProcess(hProcess, (UINT)-1);
		if (result)
		{
			// Display success message
			CString strMessage;
			strMessage.Format(_T("%s has been terminated!"), static_cast<LPCTSTR>(pProcessData->GetFilePath()));
			MessageBox(strMessage, _T("IntelliTask"), MB_OK);
		}
		// Close the process handle
		CloseHandle(hProcess);
	}
}

/**
 * @brief Update the UI state for the Kill Process command
 * 
 * Enables the Kill Process menu item only when a process is selected.
 * 
 * @param pCmdUI Pointer to the command UI object
 */
void CMainFrame::OnUpdateKillProcess(CCmdUI *pCmdUI)
{
	// Get the active process view
	CProcessView* pProcessView = (CProcessView*)GetActiveView();
	if ((pProcessView != nullptr) && (pProcessView->GetSafeHwnd() != nullptr))
	{
		// Enable only if an item is selected
		const int nListItem = pProcessView->GetListCtrl().GetNextItem(-1, LVIS_SELECTED | LVIS_FOCUSED);
		pCmdUI->Enable(nListItem != -1);
	}
	else
		pCmdUI->Enable(false);
}

/**
 * @brief Display the Enumerate Devices dialog
 * 
 * Shows a dialog that lists all hardware devices installed on the system.
 */
void CMainFrame::OnDevices()
{
	// Create the devices enumeration dialog
	CEnumDevicesDlg pEnumDevicesDlg(this);
	// Display it as a modal dialog (blocks parent window)
	pEnumDevicesDlg.DoModal();
}

/**
 * @brief Display the Enumerate Programs dialog
 * 
 * Shows a dialog that lists all installed programs on the system.
 */
void CMainFrame::OnPrograms()
{
	// Create the programs enumeration dialog
	CEnumProgramsDlg pEnumProgramsDlg(this);
	// Display it as a modal dialog (blocks parent window)
	pEnumProgramsDlg.DoModal();
}

/**
 * @brief Open the developer's Twitter/X profile
 * 
 * Launches the default web browser to view the developer's X (formerly Twitter) profile.
 */
void CMainFrame::OnTwitter()
{
	// Use ShellExecute to open URL in default browser
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://x.com/stefanmihaimoga"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Open the developer's LinkedIn profile
 * 
 * Launches the default web browser to view the developer's LinkedIn profile.
 */
void CMainFrame::OnLinkedin()
{
	// Use ShellExecute to open URL in default browser
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.linkedin.com/in/stefanmihaimoga/"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Open the developer's Facebook profile
 * 
 * Launches the default web browser to view the developer's Facebook profile.
 */
void CMainFrame::OnFacebook()
{
	// Use ShellExecute to open URL in default browser
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.facebook.com/stefanmihaimoga"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Open the developer's Instagram profile
 * 
 * Launches the default web browser to view the developer's Instagram profile.
 */
void CMainFrame::OnInstagram()
{
	// Use ShellExecute to open URL in default browser
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://www.instagram.com/stefanmihaimoga/"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Open the project's GitHub Issues page
 * 
 * Launches the default web browser to view the project's issue tracker on GitHub.
 */
void CMainFrame::OnIssues()
{
	// Use ShellExecute to open GitHub issues page in default browser
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://github.com/mihaimoga/IntelliTask/issues"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Open the project's GitHub Discussions page
 * 
 * Launches the default web browser to view the project's discussions on GitHub.
 */
void CMainFrame::OnDiscussions()
{
	// Use ShellExecute to open GitHub discussions page in default browser
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://github.com/mihaimoga/IntelliTask/discussions"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Open the project's GitHub Wiki page
 * 
 * Launches the default web browser to view the project's documentation wiki on GitHub.
 */
void CMainFrame::OnWiki()
{
	// Use ShellExecute to open GitHub wiki page in default browser
	::ShellExecute(GetSafeHwnd(), _T("open"), _T("https://github.com/mihaimoga/IntelliTask/wiki"), nullptr, nullptr, SW_SHOW);
}

/**
 * @brief Display the User Manual in a web browser dialog
 * 
 * Shows a dialog with an embedded web browser to view the user manual.
 */
void CMainFrame::OnUserManual()
{
	// Create web browser dialog to display user manual
	CWebBrowserDlg dlgWebBrowser(this);
	// Show as modal dialog
	dlgWebBrowser.DoModal();
}

/**
 * @brief Check for application updates
 * 
 * Displays a dialog that checks for newer versions of the application.
 */
void CMainFrame::OnCheckForUpdates()
{
	// Create update checker dialog
	CCheckForUpdatesDlg dlgCheckForUpdates(this);
	// Show as modal dialog
	dlgCheckForUpdates.DoModal();
}
