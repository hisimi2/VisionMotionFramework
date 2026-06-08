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
    , m_StartSwitch("StartSwitch")
    , m_StopSwitch("StopSwitch")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_StartSwitch.setGroup(&m_StopSwitch).setOption(IOPSwitch::PUSH, false);
    m_StopSwitch.setGroup(&m_StartSwitch).setOption(IOPSwitch::PUSH, false);
    m_StopSwitch.setStatus(true); // 초기 상태는 Stop

}

void CEquipment2015Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_START, m_BtnStart);
    DDX_Control(pDX, IDC_STOP, m_BtnStop);
    DDX_Control(pDX, IDC_LOG_EDIT, m_LogEdit);
}

BEGIN_MESSAGE_MAP(CEquipment2015Dlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CEquipment2015Dlg::OnBnClickedStart)
    ON_BN_CLICKED(IDC_STOP, &CEquipment2015Dlg::OnBnClickedStop)
    ON_WM_TIMER()
    ON_MESSAGE(WM_ACTIVITY_RESULT, &CEquipment2015Dlg::OnActivityResult)
    ON_BN_CLICKED(IDC_VMF_STATE_MACHINE, &CEquipment2015Dlg::OnBnClickedVmfStateMachine)
END_MESSAGE_MAP()

// CEquipment2015Dlg 메시지 처리기
BOOL CEquipment2015Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    // Windows 기본 비주얼 매니저로 복원
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));

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

    // Observer에서 UI로 결과 출력 — PostMessage로 UI 스레드에 전달
    m_threadsMgr.AddObserver([this](const std::string& name, int requestId, const std::vector<std::string>& results)
    {
        ActivityResultData* pData = new ActivityResultData();
        pData->activityName = name.c_str();
        pData->requestId = requestId;

        for (const auto& result : results)
        {
            pData->detail += CString(result.c_str()) + _T("\r\n");
        }

        // UI 스레드로 메시지 전송 (비동기, 스레드 안전)
        ::PostMessage(this->m_hWnd, WM_ACTIVITY_RESULT, (WPARAM)pData, (LPARAM)0);
    });

    SetTimer(1, 1000, NULL); // 1초마다 타이머 이벤트 발생

	return TRUE;	// 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}


void CEquipment2015Dlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
    switch (nIDEvent)
    {
    case 1:
        if (m_StartSwitch.getStatus())
        {
            OutputDebugString(_T("[UI] Start All activities...\n"));
            m_threadsMgr.GetManager().RunAll();

            m_BtnStart.SetFaceColor(RGB(0, 180, 0), TRUE);
            m_BtnStop.SetFaceColor(RGB(240, 240, 240), TRUE);
        }
        else 
        {
            OutputDebugString(_T("[UI] Pause All activities...\n"));
            m_threadsMgr.GetManager().PauseAll();

            m_BtnStart.SetFaceColor(RGB(240, 240, 240), TRUE);
            m_BtnStop.SetFaceColor(RGB(200, 50, 50), TRUE);
        }
        break;
    }

    CDialogEx::OnTimer(nIDEvent);
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

void CEquipment2015Dlg::OnBnClickedStart()
{
    m_StartSwitch.setStatus(true);
}

void CEquipment2015Dlg::OnBnClickedStop()
{
    m_StopSwitch.setStatus(true);
}

// OnActivityResult — UI 스레드에서 실행됨 (PostMessage 수신)
LRESULT CEquipment2015Dlg::OnActivityResult(WPARAM wParam, LPARAM lParam)
{
    ActivityResultData* pData = reinterpret_cast<ActivityResultData*>(wParam);
    if (pData)
    {
        CString msg;
        msg.Format(_T("[%s] completed (requestId=%d)\r\n%s"),
            pData->activityName.GetString(),
            pData->requestId,
            pData->detail.GetString());

        m_LogEdit.SetSel(m_LogEdit.GetWindowTextLength(), m_LogEdit.GetWindowTextLength());
        m_LogEdit.ReplaceSel(msg);

        delete pData;
}
    return 0;
}


//=============================================================================
// [예제] Orchestrator + Strategy 패턴을 사용한 시퀀스 실행
// - VMF::Orchestrator를 생성하고 CLoad1LeftPlateJIGFocusCheckSequenceStrategy를
//   실행하는 방법을 보여줍니다.
// - 옵저버를 등록하여 실행 결과를 UI 로그에 출력합니다.
// - 실제 하드웨어가 없는 경우 CMockActuator를 사용할 수 있습니다.
//=============================================================================


// ============================================================
// [필요 헤더 includes]
// ============================================================
#include "Orchestrator.h"
#include "IActuator.h"
#include "VMFComposition/Load1/Strategies/CLoad1LeftPlateJIGFocusCheckSequenceStrategy.h"

// 실행에 필요한 Actuator 구현 (실제 하드웨어 또는 Mock)
#include "VMFComposition/Load1/VatAdapterLoad1.h"

// 직접 모드에서 필요한 헤더
#include "RepositoryFactory.h"
#include "VMFComposition/Mock/CMockVisionEventHandler.h"

// ============================================================
// 1. Orchestrator 생성 및 옵저버 등록 (OnInitDialog 등에서)
// ============================================================
void CEquipment2015Dlg::InitOrchestratorExample()
{
    // 1-1. Orchestrator 인스턴스 생성
    m_orchestrator = std::make_shared<VMF::Orchestrator>();

    // 1-2. 옵저버 등록: 시퀀스 실행 결과를 UI 로그에 표시
    m_orchestrator->AddObserver([this](const VMF::VisionResultPayload& payload)
    {
        CString msg;
        msg.Format(_T("[Orchestrator] RequestId=%d\r\n"), payload.requestId);

        for (const auto& result : payload.results)
        {
            msg += CString(result.c_str()) + _T("\r\n");
        }

// UI 스레드로 전달 (PostMessage 사용)
        ActivityResultData* pData = new ActivityResultData();
        pData->activityName = _T("Orchestrator");
        pData->requestId = payload.requestId;
        pData->detail = msg;
        ::PostMessage(m_hWnd, WM_ACTIVITY_RESULT, (WPARAM)pData, 0);
    });
}

// ============================================================
// 2. Actuator 생성 및 시퀀스 실행
// ============================================================
void CEquipment2015Dlg::RunOrchestratorSequenceExample()
{
    if (!m_orchestrator)
    {
        AfxMessageBox(_T("Orchestrator가 초기화되지 않았습니다."));
        return;
    }

    // 2-1. Actuator 생성 (실제 하드웨어 어댑터)
// 실제 장비 연결 시:
    // auto* actuator = new VMF_Load1::VatAdapterLoad1(하드웨어 초기화 파라미터);

    // Mock 객체 사용 (개발/테스트용):
    // auto* actuator = new VMF::CMockActuator();

    // ※ 실제 Actuator 주입은 Equipment 프로젝트의 하드웨어 초기화 이후 수행
    VMF::IActuator* actuator = nullptr; // 실제 구현 시 하드웨어 어댑터로 교체

    // 2-2. 템플릿 메서드를 통해 Strategy 타입을 전달하여 시퀀스 시작
    bool started = m_orchestrator->StartSequence<
        VMF_Load1::CLoad1LeftPlateJIGFocusCheckSequenceStrategy
    >(actuator);

    if (started)
    {
        m_LogEdit.SetSel(m_LogEdit.GetWindowTextLength(), m_LogEdit.GetWindowTextLength());
        m_LogEdit.ReplaceSel(_T("[Orchestrator] Sequence started successfully.\r\n"));
    }
    else
    {
        m_LogEdit.SetSel(m_LogEdit.GetWindowTextLength(), m_LogEdit.GetWindowTextLength());
        m_LogEdit.ReplaceSel(_T("[Orchestrator] Failed to start sequence.\r\n"));
    }
}

// ============================================================
// 3. 시퀀스 중단
// ============================================================
void CEquipment2015Dlg::StopOrchestratorSequenceExample()
{
    if (m_orchestrator)
    {
        m_orchestrator->StopSequence();
        m_LogEdit.SetSel(m_LogEdit.GetWindowTextLength(), m_LogEdit.GetWindowTextLength());
        m_LogEdit.ReplaceSel(_T("[Orchestrator] Sequence stopped.\r\n"));
    }
}

// ============================================================
// 4. 실행 결과 데이터 접근 (Orchestrator -> Context -> Repository)
// ============================================================
void CEquipment2015Dlg::AccessSequenceDataExample()
{
    if (!m_orchestrator) return;

    // Orchestrator를 통해 Repository 접근
    VMF::DataRepositoryPtr repo = m_orchestrator->GetDataRepository();
    if (repo)
    {
        std::string value;
        if (repo->LoadParam("Recipe1", "CameraIndex", value) == VMF::StorageSuccess)
        {
            CString msg;
            msg.Format(_T("[Repository] Loaded param CameraIndex = %s\r\n"),
                       CString(value.c_str()));

            m_LogEdit.SetSel(m_LogEdit.GetWindowTextLength(), m_LogEdit.GetWindowTextLength());
            m_LogEdit.ReplaceSel(msg);
        }
    }
}

void CEquipment2015Dlg::OnBnClickedVmfStateMachine()
{
    // === 직접 모드 (상태머신 없이) 예제 ===
    // 1. Repository 생성
    auto repo = VMF::RepositoryFactory::CreateRepository(
        "sqlite", "Data\\VAT_DATABASE.db;Data\\Images");
    if (!repo)
    {
        AfxMessageBox(_T("Repository 생성 실패"));
        return;
    }
    repo->Initialize();

    // 2. VisionProcessor 생성
    auto vp = std::make_shared<VMF::CMockVisionEventHandler>();
    vp->Initialize(VMF::VisionConnectionConfig("127.0.0.1", 8080, 3000));

    // 3. Orchestrator 초기화 (직접 모드)
    m_orchestrator = std::make_shared<VMF::Orchestrator>();
    m_orchestrator->SetDataRepository(std::move(repo));
    m_orchestrator->SetVisionProcessor(vp);

    // 4. Context 획득 및 파라미터 설정 (Strategy::ConfigureParams와 동일)
    auto ctx = m_orchestrator->GetOrCreateContext();

    VMF::VatParams params;
    params.seqParams["CameraIndex"] = "6";
    params.seqParams["HandID"] = "1";
    params.seqParams["PkgID"] = "1";
    params.seqParams["InspectionType"] = "6";

    // Repository에서 데이터 로드 → VisionPosition 추가
    auto loadedRepo = m_orchestrator->GetDataRepository();
    double posX, posY, focusZ;
    if (loadedRepo && loadedRepo->LoadInspInitPos(6, 3, 1, posX, posY, focusZ) == VMF::StorageSuccess)
    {
        std::vector<double> pos = { posX, posY, focusZ };
        params.visionPositions.push_back(
            VMF::VisionPosition(pos, 3, 6));
    }
    ctx->SetVatParams(params);

    // 5. 직접 비전 명령 실행
    bool ok = m_orchestrator->ExecuteDirectVisionCommand(VMF::Measure);

    CString msg;
    msg.Format(_T("[DirectMode] Measure command %s\r\n"), ok ? _T("SUCCESS") : _T("FAILED"));
    m_LogEdit.SetSel(m_LogEdit.GetWindowTextLength(), m_LogEdit.GetWindowTextLength());
    m_LogEdit.ReplaceSel(msg);
}
