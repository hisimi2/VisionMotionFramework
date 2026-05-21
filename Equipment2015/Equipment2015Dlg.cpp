// Equipment2015Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Equipment2015.h"
#include "Equipment2015Dlg.h"
#include "afxdialogex.h"

#include "VMFramework/Load1/Strategies/CPickPlaceSequenceStrategy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// PostMessage 기반 결과 수신은 제거됨. (CVatEngineUiAdapter는 Observer로 결과를 통지)

// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CEquipment2015Dlg 대화 상자

CEquipment2015Dlg::CEquipment2015Dlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_EQUIPMENT2015_DIALOG, pParent)
 , m_engineObserverId(0)
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
	ON_BN_CLICKED(IDC_VISION_SEQUENCE, &CEquipment2015Dlg::OnBnClickedVisionSequence)
END_MESSAGE_MAP()

void CEquipment2015Dlg::HandleVmfResult(const VMF::VisionResultPayload& payload)
{
     // 주의: 이 함수는 시퀀스 실행 스레드에서 호출될 수 있습니다.
     // UI 컨트롤 접근이 필요하면 UI 스레드로 마샬링해야 합니다.
     CString msg;
     msg.Format(_T("VMF Result: requestId=%d, lines=%d\r\n"), payload.requestId, (int)payload.results.size());
     OutputDebugString(msg);
}

// CEquipment2015Dlg 메시지 처리기

BOOL CEquipment2015Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX &0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX <0xF000);

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

     // VMF 결과 옵저버 등록 (this를 캡처하여 멤버 핸들러로 전달)
     m_engineObserverId = m_engine.AddObserver([this](const VMF::VisionResultPayload& payload) {
                                                this->HandleVmfResult(payload);
                                              });

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_parts = std::make_shared<Load1Parts>();
	m_adapter = std::make_shared<VMF_Load1::VatAdapterLoad1>(m_parts.get());

	return TRUE;	// 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CEquipment2015Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID &0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
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

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()),0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon +1) /2;
		int y = (rect.Height() - cyIcon +1) /2;

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

using namespace VMF_Load1::Strategies;

void CEquipment2015Dlg::OnBnClickedVisionSequence()
{
	if (!m_adapter)
		return;

	m_engine.StartSequence<CPickPlaceSequenceStrategy>(m_adapter.get());
}
