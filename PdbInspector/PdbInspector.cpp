//	------------------------------------------------------------------------------------------------
//	File name:		PdbInspector.cpp
//	Author:			Marc Ochsenmeier
//	Email:			info@winitor.net
//	Web:			www.winitor.net - 
//	Updated:		27.06.2013
//
//	Description:	An MFC UI Application (PdbInspector.exe) that consumes our PdbParser.dll
//					to inspect the content of PDB files.
//
//	------------------------------------------------------------------------------------------------
#include "stdafx.h"

#include "PdbInspector.h"
#include "PdbInspectorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CPdbInspectorApp
BEGIN_MESSAGE_MAP( CPdbInspectorApp, CWinApp)
	ON_COMMAND(ID_HELP, &OnHelp)
END_MESSAGE_MAP()

// CPdbInspectorApp construction
CPdbInspectorApp::CPdbInspectorApp()
{
}

// The one and only CPdbInspectorApp object
CPdbInspectorApp theApp;

// CPdbInspectorApp initialization
BOOL CPdbInspectorApp::InitInstance()
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

	CWinApp::InitInstance();
	AfxEnableControlContainer();

	PdbInspectorDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
