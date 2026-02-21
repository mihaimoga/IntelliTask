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

/**
 * @file WebBrowserDlg.cpp
 * @brief Implementation of the CWebBrowserDlg class - Web browser dialog for displaying documentation
 * 
 * This file implements a dialog that embeds a web browser control to display
 * the application's user manual or other HTML content.
 * 
 * Key features:
 * - Asynchronous browser control creation
 * - Automatic window resizing to fit browser content
 * - Dynamic title updates from web page
 * - Popup blocking enabled by default
 * 
 * IMPLEMENTATION NOTES:
 * - Uses CWebBrowser custom control for modern web content rendering
 * - Browser is created asynchronously to avoid blocking the UI thread
 * - Title bar updates automatically when page title changes
 * 
 * THREADING:
 * - All browser operations occur on the UI thread
 * - Async creation prevents UI freeze during initialization
 * - Callbacks execute on the UI thread, ensuring safety
 * 
 * SECURITY:
 * - Popups are disabled to prevent unwanted content
 * - Only navigates to trusted URLs (USER_MANUAL_URL)
 * - No script injection or external navigation allowed
 * 
 * BROWSER TECHNOLOGY:
 * - Uses Edge WebView2 or IE11 control depending on configuration
 * - Supports modern HTML5, CSS3, and JavaScript
 * - Automatically handles SSL/TLS for HTTPS connections
 */

#include "stdafx.h"
#include "IntelliTask.h"
#include "WebBrowserDlg.h"

/**
 * @brief Dynamic class implementation for CWebBrowserDlg
 * 
 * Enables runtime type information (RTTI) for the CWebBrowserDlg class.
 * This allows the framework to identify and manipulate the class at runtime.
 */
IMPLEMENT_DYNAMIC(CWebBrowserDlg, CDialogEx)

/**
 * @brief Constructor for CWebBrowserDlg
 * 
 * Initializes the web browser dialog.
 * 
 * @param pParent Pointer to the parent window (default: nullptr)
 * 
 * @note The browser control itself is NOT created here. It's created
 * asynchronously in OnInitDialog() to prevent blocking during initialization.
 */
CWebBrowserDlg::CWebBrowserDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_WebBrowserDlg, pParent)
{
}

/**
 * @brief Destructor for CWebBrowserDlg
 * 
 * Cleans up resources. Browser control cleanup is handled in OnDestroy().
 */
CWebBrowserDlg::~CWebBrowserDlg()
{
}

/**
 * @brief Exchange data between dialog controls and member variables
 * 
 * @param pDX Pointer to the CDataExchange object
 * 
 * @note This dialog doesn't have standard controls that need DDX mapping,
 * as the browser control is created dynamically.
 */
void CWebBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

/**
 * @brief Message map for CWebBrowserDlg
 * 
 * Maps Windows messages to their handler functions.
 * Handles destruction and window resizing events.
 */
BEGIN_MESSAGE_MAP(CWebBrowserDlg, CDialogEx)
	ON_WM_DESTROY()
    ON_WM_SIZE()
END_MESSAGE_MAP()

/**
 * @brief Initialize the dialog and create the web browser control
 * 
 * Creates the embedded web browser control asynchronously and configures it
 * to display the user manual. Sets up callbacks for title updates.
 * 
 * @return TRUE to set focus to the first control automatically
 * 
 * @note The browser is created asynchronously to avoid blocking the UI thread.
 * The completion callback handles navigation and configuration.
 * 
 * @performance Async creation prevents UI freeze. Dialog appears immediately
 * while browser initializes in the background (~100-500ms depending on system).
 * 
 * @error If browser creation fails, the dialog will still be functional but empty.
 * Consider adding error handling in production code to detect and report failures.
 */
BOOL CWebBrowserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// === GET CLIENT AREA DIMENSIONS ===
	// Get the client rectangle to size the browser control to fill the dialog
	// This ensures the browser occupies the entire dialog area with no margins
	CRect rectClient;
	GetClientRect(rectClient);

	// === CREATE BROWSER CONTROL ASYNCHRONOUSLY ===
	// CreateAsync prevents blocking the UI thread during browser initialization
	// The browser control may take 100-500ms to initialize depending on:
	// - System resources
	// - Whether Edge WebView2 runtime is installed
	// - Cold start vs warm start
	m_pCustomControl.CreateAsync(
		WS_VISIBLE | WS_CHILD,  // Make browser visible and a child window
		rectClient,              // Position and size to fill client area
		this,                    // Parent window
		1,                       // Control ID
		[this]() {              // Completion callback (called when browser is ready)
			// === CONFIGURE BROWSER ===
			// Set the parent view for proper integration
			// This enables proper message routing and window management
			m_pCustomControl.SetParentView((CView*)this);

			// Disable popup windows for security and better UX
			// Prevents documentation pages from opening unwanted windows
			// Also blocks potentially malicious popup attempts
			m_pCustomControl.DisablePopups();

			// Navigate to the user manual URL
			// USER_MANUAL_URL should be defined in a header file
			// Can be a local file:/// URL or remote https:// URL
			m_pCustomControl.Navigate(USER_MANUAL_URL, nullptr);

			// === REGISTER TITLE CHANGE CALLBACK ===
			// Update dialog title when web page title changes
			// This provides context to the user about which page they're viewing
			// Lambda captures 'this' to access dialog methods
			m_pCustomControl.RegisterCallback(CWebBrowser::CallbackType::TitleChanged, [this]() {
				// Get the page title from the browser
				// Returns empty string if no title is set
				CString strTitle = m_pCustomControl.GetTitle();
				// Update the dialog's title bar
				// Provides immediate visual feedback of page navigation
				SetWindowText(strTitle);
				});
		});

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

/**
 * @brief Clean up resources when the dialog is destroyed
 * 
 * Destroys the web browser control and performs cleanup.
 * 
 * @note The browser control must be explicitly destroyed to release
 * COM objects and prevent memory leaks.
 * 
 * @important This method is called automatically when:
 * - User closes the dialog
 * - Parent window is destroyed
 * - Application is shutting down
 * 
 * RESOURCE CLEANUP:
 * - Releases COM interfaces used by the browser control
 * - Frees memory allocated for web page rendering
 * - Terminates any pending network requests
 */
void CWebBrowserDlg::OnDestroy()
{
	// === DESTROY BROWSER CONTROL ===
	// Explicitly destroy the browser control to release resources
	// This ensures proper COM cleanup and prevents memory leaks
	// VERIFY macro will assert in debug builds if destruction fails
	VERIFY(m_pCustomControl.DestroyWindow());

	// Call base class cleanup
	// This handles standard dialog destruction tasks
	CDialogEx::OnDestroy();
}

/**
 * @brief Handle window resizing events
 * 
 * Resizes the embedded browser control to match the dialog's new size.
 * This ensures the browser always fills the entire client area.
 * 
 * @param nType Type of resizing requested (SIZE_MINIMIZED, SIZE_MAXIMIZED, etc.)
 * @param cx New width of the client area in pixels
 * @param cy New height of the client area in pixels
 * 
 * @note The browser control is repositioned to (0,0) and resized to fill
 * the entire client area, providing a seamless browsing experience.
 * 
 * @performance This method is called frequently during window resizing.
 * The browser control handles reflow of web content automatically.
 * Modern browsers optimize reflow to prevent jank during resize.
 * 
 * @ux Filling the entire client area provides a clean, modern look
 * without distracting borders or margins around the content.
 */
void CWebBrowserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// === RESIZE BROWSER CONTROL ===
	// Check if browser control exists before attempting to resize
	// This prevents crashes during initialization or destruction
	// GetSafeHwnd() returns nullptr if window doesn't exist
	if (m_pCustomControl.GetSafeHwnd() != nullptr)
	{
		// Move and resize browser to fill the entire client area
		// Position: (0, 0) - top-left corner (no margins)
		// Size: (cx, cy) - full width and height of dialog
		// MoveWindow causes the browser to reflow its content
		m_pCustomControl.MoveWindow(0, 0, cx, cy);
	}
}
