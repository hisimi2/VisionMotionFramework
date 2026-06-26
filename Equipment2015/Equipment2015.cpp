// Equipment2015.cpp : 응용 프로그램에 대한 클래스 동작을 정의합니다.
//

#include "stdafx.h"
#include "Equipment2015.h"
#include "Equipment2015Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CEquipment2015App

BEGIN_MESSAGE_MAP(CEquipment2015App, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEquipment2015App 생성

CEquipment2015App::CEquipment2015App()
{
    m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}


// 유일한 CEquipment2015App 개체입니다.

CEquipment2015App theApp;


// CEquipment2015App 초기화

BOOL CEquipment2015App::InitInstance()
{
    INITCOMMONCONTROLSEX InitCtrls;
    InitCtrls.dwSize = sizeof(InitCtrls);
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&InitCtrls);

    CWinApp::InitInstance();

    AfxEnableControlContainer();

    CShellManager* pShellManager = new CShellManager;
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

    SetRegistryKey(_T("Equipment2015"));

    CEquipment2015Dlg dlg;
    m_pMainWnd = &dlg;
    INT_PTR nResponse = dlg.DoModal();

    if (pShellManager)
        delete pShellManager;

#ifndef _AFXDLL
    ControlBarCleanUp();
#endif

    return FALSE;
}

