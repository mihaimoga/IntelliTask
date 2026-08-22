# AI Agent Guide for IntelliTask

This document provides essential context, architectural details, build instructions, and strict coding conventions for AI agents working on the **IntelliTask** solution.

---

## 1. Solution Overview

**IntelliTask** is an open-source Windows process and service manager built in C++ using MFC (Microsoft Foundation Classes), Win32 API, and the C++ Standard Library (STL). It provides an alternative to Windows Task Manager, focusing on efficiency, performance, and low CPU overhead.

### Projects in Solution

| Project | Type | Description | License |
|---|---|---|---|
| **IntelliTask** (`IntelliTask.vcxproj`) | MFC Application (`.exe`) | Main GUI application for monitoring processes, services, installed programs, devices, and CPU/memory usage. | GPL-3.0 |
| **genUp4win** (`genUp4win\genUp4win.vcxproj`) | Dynamic Link Library (`.dll`) | Generic updater library handling update checks, XML parsing, and download verification. | MIT |

---

## 2. Architecture & Key Components

### IntelliTask Core Modules

- **Application Entry & Lifecycle**:
  - `IntelliTask.h` / `IntelliTask.cpp` (`CIntelliTaskApp`): Application class handling initialization, single-instance enforcement (`CInstanceChecker`), privilege elevation (`EnableFullAccess`), and state management.
  - `MainFrame.h` / `MainFrame.cpp` (`CMainFrame`): Main application frame window hosting MFC ribbon / toolbars, status bars, and view routing.
  - `stdafx.h` / `stdafx.cpp`: Precompiled header containing Windows, MFC, and WIL headers.

- **Process & Performance Monitoring**:
  - `ProcessView.h` / `ProcessView.cpp` (`CProcessView`): Main view displaying running processes with periodic refresh timers.
  - `ProcessList.h` / `ProcessList.cpp` (`CProcessList`): Process snapshot enumeration, status retrieval, process termination, and metadata collection.
  - `CpuUsage.h` / `CpuUsage.cpp` (`CCpuUsage`): CPU usage calculation across processor cores.

- **Dialogs & Tools**:
  - `EnumDevicesDlg.h` / `EnumDevicesDlg.cpp` (`CEnumDevicesDlg`): Local hardware device manager enumeration.
  - `EnumProgramsDlg.h` / `EnumProgramsDlg.cpp` (`CEnumProgramsDlg`): Installed software enumeration dialog.
  - `CheckForUpdatesDlg.h` / `CheckForUpdatesDlg.cpp` (`CCheckForUpdatesDlg`): Update check dialog integrating `genUp4win`.
  - `WebBrowserDlg.h` / `WebBrowserDlg.cpp` (`CWebBrowserDlg`): Embedded web browser dialog leveraging Microsoft Edge WebView2 (`EdgeWebBrowser.h/.cpp`).

- **UI Helpers & Utilities**:
  - `MFCListView.h` / `MFCListView.cpp` (`CMFCListView`): Base list view extension.
  - `SortListCtrl.h` / `SortListCtrl.cpp` (`CSortListCtrl`): Column sorting support for list controls.
  - `WndResizer.h` / `WndResizer.cpp` (`CWndResizer`): Layout resizing helper for MFC dialogs.
  - `HLinkCtrl.h` / `HLinkCtrl.cpp` (`CHLinkCtrl`): Hyperlink control helper.
  - `VersionInfo.h` / `VersionInfo.cpp` (`CVersionInfo`): Version resource extraction helper.
  - `Dtwinver.h` / `Dtwinver.cpp`: Windows OS version detection.
  - `sinstance.h` / `sinstance.cpp` (`CInstanceChecker`): Single-instance application detection.

### Dependencies & Packages

- **MFC**: Static linking (`UseOfMfc: Static`).
- **NuGet Packages** (defined in `packages.config`):
  - `Microsoft.Web.WebView2` (Native WebView2 control for Edge browser dialog)
  - `Microsoft.Windows.ImplementationLibrary` (WIL)
- **Windows Target**: Windows 10+ SDK (`10.0`), Platform Toolset `v145` / `v143`.
- **C++ Standard**: C++ latest (`/std:c++latest`), C latest (`/std:clatest`).

---

## 3. Build & Run Instructions

### Visual Studio / MSBuild

Build the full solution using MSBuild:
```cmd
msbuild IntelliTask.sln /p:Configuration=Release /p:Platform=x64
```
Supported configurations:
- `Debug|Win32`, `Debug|x64`
- `Release|Win32`, `Release|x64`

### CMake (genUp4win only)
`genUp4win` can also be built independently with CMake:
```bash
cd genUp4win
mkdir build && cd build
cmake ..
cmake --build . --config Release
```

---

## 4. Coding Standards & Guidelines

AI agents modifying this codebase must adhere strictly to the repository's coding style from `CONTRIBUTING.md`.

### Formatting & Bracing
- **Indentation**: Use **Tabs** (tab width = 4 spaces), never spaces for indentation.
- **Braces**: Use Allman style (opening and closing braces on separate lines):
  ```cpp
  void MyClass::method1()
  {
	  if (aCondition)
	  {
		  // Do something
	  }
  }
  ```
  *Exception*: Single-line inline method definitions in header files may use same-line braces:
  ```cpp
  int getValue() { return _value; }
  ```
- **Spacing**:
  - Exactly one space around binary and ternary operators: `if (a == 10 && b == 42)`
  - Exactly one space after semicolons in `for` loops: `for (int i = 0; i != 10; ++i)`
  - No space between function name and opening parenthesis: `foo();`
  - Exactly one space between keywords and opening parenthesis: `if (condition)`, `while (condition)`, `switch (value)`
- **Switch Statements**:
  ```cpp
  switch (test)
  {
	  case 1:
	  {
		  // Do something
		  break;
	  }
	  default:
		  // Do something else
  }
  ```

### Naming Conventions
- **Classes / Structs**: PascalCase (e.g., `CIntelliTaskApp`, `CProcessList`).
- **Methods & Functions**: camelCase for custom methods (or standard MFC PascalCase overrides when extending MFC base classes).
- **Parameters & Local Variables**: camelCase with descriptive naming.
- **Member Variables**: Preceded by an underscore `_` (e.g., `_publicAttribute`, `_pAccount`) or standard MFC prefix `m_` in existing MFC UI classes.

### Modern C++ Best Practices
- Use uniform initialization with curly braces: `MyClass instance{10.4};`
- Check string emptiness with `.empty()` or `CString::IsEmpty()`, never comparing against `""`.
- Use C++ casts (`static_cast`, `reinterpret_cast`) instead of C-style casts `(type)val`.
- Logical operators: use `!`, `&&`, `||` (not text equivalents `not`, `and`, `or`).
- Prefer pre-increment `++i` over post-increment `i++`.
- Avoid raw `new`/pointers when automatic variables or smart pointers (`std::unique_ptr` preferred) can be used.
- Avoid placing `using namespace` directives in header files.
- Use C++ style single-line comments `//` rather than C style `/* ... */`.

---

## 5. Agent Workflow & Safety Checklist

When making changes:
1. **Preserve Tab Indentation**: Do not replace tabs with spaces or reformat untouched code.
2. **Include Precompiled Headers**: Ensure `stdafx.h` (or `pch.h` in genUp4win) is included first in `.cpp` translation units.
3. **Verify Build**: Always run `run_build` to verify that changes compile cleanly without warnings or errors across targeted platforms.
4. **Scope**: Keep changes minimal, focused, and aligned with the single feature or bug fix requested.
