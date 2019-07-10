#include "pch.h"
#include "framework.h"
#include "hMSLog2Ban.h"
#include "hMSLog2BanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ChMSLog2BanApp

BEGIN_MESSAGE_MAP(ChMSLog2BanApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)	
END_MESSAGE_MAP()


// ChMSLog2BanApp construction
ChMSLog2BanApp::ChMSLog2BanApp() {

}


ChMSLog2BanApp theApp;

BOOL ChMSLog2BanApp::InitInstance() {
	
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();
	CShellManager *pShellManager = new CShellManager;
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization	
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	ChMSLog2BanDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)	{

	}
	else if (nResponse == IDCANCEL)
	{
		
	}
	else if (nResponse == -1)
	{
		
	}

	// Delete the shell manager created above.
	if (pShellManager != nullptr)
	{
		delete pShellManager;
	}

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}


