// Equipment2015Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Equipment2015.h"
#include "Equipment2015Dlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CEquipment2015Dlg::CEquipment2015Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EQUIPMENT2015_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEquipment2015Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CEquipment2015Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()


    ON_BN_CLICKED(IDC_RESUME_ALL, &CEquipment2015Dlg::OnBnClickedResumeAll)
    ON_BN_CLICKED(IDC_PAUSE_ALL, &CEquipment2015Dlg::OnBnClickedPauseAll)
    ON_BN_CLICKED(IDC_STOP_ALL, &CEquipment2015Dlg::OnBnClickedStopAll)
    ON_BN_CLICKED(IDC_RESUME_LOAD1, &CEquipment2015Dlg::OnBnClickedResumeLoad1)
    ON_BN_CLICKED(IDC_PAUSE_LOAD1, &CEquipment2015Dlg::OnBnClickedPauseLoad1)
    ON_BN_CLICKED(IDC_STOP_LOAD1, &CEquipment2015Dlg::OnBnClickedStopLoad1)
    ON_BN_CLICKED(IDC_RESUME_LOAD2, &CEquipment2015Dlg::OnBnClickedResumeLoad2)
    ON_BN_CLICKED(IDC_PAUSE_LOAD2, &CEquipment2015Dlg::OnBnClickedPauseLoad2)
    ON_BN_CLICKED(IDC_STOP_LOAD2, &CEquipment2015Dlg::OnBnClickedStopLoad2)
END_MESSAGE_MAP()

// CEquipment2015Dlg 메시지 처리기
BOOL CEquipment2015Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// ThreadsManager 초기화 — Load1, Load2 등록
	m_threadsMgr.Initialize();

	// 결과 Observer 등록 (UI 업데이트)
	m_threadsMgr.AddObserver([this](const std::string& name, int requestId, const std::vector<std::string>& results)
	{
		CString msg;
		msg.Format(_T("[%hs] completed (requestId=%d, results=%d)\r\n"),
			name.c_str(), requestId, (int)results.size());
		OutputDebugString(msg);
	});

	return TRUE;	// 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CEquipment2015Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{

	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CEquipment2015Dlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);	// 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CEquipment2015Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CEquipment2015Dlg::OnBnClickedResumeAll()
{
    OutputDebugString(_T("[UI] Resume All activities...\n"));
    m_threadsMgr.GetManager().ResumeAll();
    
}

void CEquipment2015Dlg::OnBnClickedPauseAll()
{
    OutputDebugString(_T("[UI] Pause All activities...\n"));
    m_threadsMgr.GetManager().PauseAll();
}

void CEquipment2015Dlg::OnBnClickedStopAll()
{
    OutputDebugString(_T("[UI] Stop All activities...\n"));
    m_threadsMgr.GetManager().StopAll();
}

void CEquipment2015Dlg::OnBnClickedResumeLoad1()
{
    OutputDebugString(_T("[UI] Resume Load1...\n"));
    //m_threadsMgr.GetManager().ResumeLoad1();
}

void CEquipment2015Dlg::OnBnClickedPauseLoad1()
{
    OutputDebugString(_T("[UI] Pause Load1...\n"));
    //m_threadsMgr.GetManager().PauseLoad1();
}

void CEquipment2015Dlg::OnBnClickedStopLoad1()
{
    OutputDebugString(_T("[UI] Stop Load1...\n"));
    //m_threadsMgr.GetManager().StopLoad1();
}

void CEquipment2015Dlg::OnBnClickedResumeLoad2()
{
    OutputDebugString(_T("[UI] Resume Load2...\n"));
    //m_threadsMgr.GetManager().ResumeLoad2();
}

void CEquipment2015Dlg::OnBnClickedPauseLoad2()
{
    OutputDebugString(_T("[UI] Pause Load2...\n"));
    //m_threadsMgr.GetManager().PauseLoad2();
}

void CEquipment2015Dlg::OnBnClickedStopLoad2()
{
    OutputDebugString(_T("[UI] Stop Load2...\n"));
    //m_threadsMgr.GetManager().StopLoad2();
}
