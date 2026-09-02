// Equipment2015Dlg.cpp : 구현 파일
//
#include "stdafx.h"
#include "Equipment2015.h"
#include "Equipment2015Dlg.h"
#include "afxdialogex.h"

#include "VisionMotionFramework\Orchestrator.h"
#include "VisionMotionFramework\IDataRepository.h"
#include "VMFEquipmentPlugin\SetPlate1PLVIStrategy.h"

#include "Actuators\Load1Parts.h"
#include "Actuators\Load2Parts.h"
#include "OperationThreads\Load1ActivityBuilder.h"
#include "OperationThreads\Load2ActivityBuilder.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//=============================================================================
// Observer 헬퍼 — Orchestrator에 Observer를 등록하고
// PostMessage로 UI 스레드에 간단한 CString 로그 전달
//=============================================================================
void CEquipment2015Dlg::RegisterOrchestratorObserver(
    std::shared_ptr<VMF::Orchestrator> orchestrator,
    LPCTSTR activityName)
{
    orchestrator->AddObserver([this, activityName](const VMF::VisionResultPayload& payload)
        {
            CString msg;
            msg.Format(_T("[%s] RequestId=%d\r\n"), activityName, payload.requestId);

            for (const auto& result : payload.results)
            {
                msg += CString(result.c_str()) + _T("\r\n");
            }

            ::PostMessage(m_hWnd, WM_ACTIVITY_RESULT, (WPARAM)new CString(msg), 0);
        });
}

//=============================================================================
// [예제] VMF 상태머신 모드 (State Machine) - Plugin DLL 기반
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfStateMachine()
{
    auto strategy = std::make_shared<VMF_PLUGIN::SetPlate1PLVIStrategy>();

    m_orchestrator = std::make_shared<VMF::Orchestrator>(
        strategy,
        VMF::VisionConnectionConfig("127.0.0.1", 5000, 5000),
        m_loadPPAdapter.get());

    if (!m_orchestrator)
    {
        AppendLog(_T("[StateMachine Mode] Failed to create Orchestrator.\r\n"));
        return;
    }

    RegisterOrchestratorObserver(m_orchestrator, _T("VMF_StateMachine"));

    bool started = m_orchestrator->RunSequence();

    AppendLog(started ?
        _T("[StateMachine Mode] Sequence started.\r\n") :
        _T("[StateMachine Mode] Failed to start sequence.\r\n")
    );
}

//=============================================================================
// [예제] VMF 직접 모드 (Direct Mode) - Plugin DLL 기반
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfDirect()
{
    auto strategy = std::make_shared<VMF_PLUGIN::SetPlate1PLVIStrategy>();

    m_orchestrator = std::make_shared<VMF::Orchestrator>(
        strategy,
        VMF::VisionConnectionConfig("127.0.0.1", 5000, 5000));

    if (!m_orchestrator)
    {
        AppendLog(_T("[Direct Mode] Failed to create Orchestrator.\r\n"));
        return;
    }

    RegisterOrchestratorObserver(m_orchestrator, _T("VMF_Direct"));

    VMF::StringMap params;
    params["ExtraParam"] = "DirectModeTest";
bool cmdOk = m_orchestrator->ExecuteDirectVisionCommand(VMF::VisionCommand::Measure, params);

    VMF::VisionProcessorPtr vp = m_orchestrator->GetVisionProcessor();
    if (vp)
    {
        auto latestData = vp->GetLatestData(VMF::VisionCommand::Measure);
    }
}


CEquipment2015Dlg::CEquipment2015Dlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_EQUIPMENT2015_DIALOG, pParent)
    , m_StartSwitch("StartSwitch")
    , m_StopSwitch("StopSwitch")
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_StartSwitch.setGroup(&m_StopSwitch).setOption(IOPSwitch::PUSH, false);
    m_StopSwitch.setGroup(&m_StartSwitch).setOption(IOPSwitch::PUSH, false);
    m_StopSwitch.setStatus(true);


    m_threadsMgr = std::make_shared<EC::ThreadsManager>();

    // ★ Builder 등록 (방식 2: 직접 Builder 인스턴스 등록 — 각 Builder에 전용 Parts 주입) ★
    {
        // EquipmentParts에서 Stacker 부품만 추출하여 StackerParts 생성
        auto load1Parts = std::make_shared<Load1Parts>();

        auto stackerBuilder = std::make_shared<EC::CLoad1ActivityBuilder>();
        stackerBuilder->SetParts(load1Parts);
        m_threadsMgr->RegisterBuilder("Load1Activity", stackerBuilder);
    }
    {
        // EquipmentParts에서 Sorter 부품만 추출하여 SorterParts 생성
        auto load2Parts = std::make_shared<Load2Parts>();
        auto sorterBuilder = std::make_shared<EC::CLoad2ActivityBuilder>();
        sorterBuilder->SetParts(load2Parts);
        m_threadsMgr->RegisterBuilder("Load2Activity", sorterBuilder);
    }

    m_threadsMgr->Initialize();

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
    ON_BN_CLICKED(IDC_VMF_CONNECTION_MANAGER, &CEquipment2015Dlg::OnBnClickedVmfStateMachineWithVisionConnectionManager)
    ON_BN_CLICKED(IDC_VMF_MULTI_SERVER, &CEquipment2015Dlg::OnBnClickedVmfMultiServerExample)
    ON_BN_CLICKED(IDC_VMF_DIRECT, &CEquipment2015Dlg::OnBnClickedVmfDirect)
END_MESSAGE_MAP()

// CEquipment2015Dlg 메시지 처리기
BOOL CEquipment2015Dlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));

    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    // Observer 등록 — PostMessage로 UI 스레드에 결과 전달
    m_threadsMgr->AddObserver([this](const std::string& name, int requestId, const std::vector<std::string>& results)
    {
        CString msg;
        msg.Format(_T("[%hs] RequestId=%d\r\n"), name.c_str(), requestId);

        for (const auto& result : results)
        {
            msg += CString(result.c_str()) + _T("\r\n");
        }

        ::PostMessage(m_hWnd, WM_ACTIVITY_RESULT, (WPARAM)new CString(msg), 0);
    });

    SetTimer(1, 1000, NULL);

    return TRUE;
}

void CEquipment2015Dlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 1)
    {
        if (m_StartSwitch.getStatus())
        {
            m_threadsMgr->RunAll();
            m_BtnStart.SetFaceColor(RGB(0, 180, 0), TRUE);
            m_BtnStop.SetFaceColor(RGB(240, 240, 240), TRUE);
        }
        else
        {
            m_threadsMgr->PauseAll();
            m_BtnStart.SetFaceColor(RGB(240, 240, 240), TRUE);
            m_BtnStop.SetFaceColor(RGB(200, 50, 50), TRUE);
        }
    }

    CDialogEx::OnTimer(nIDEvent);
}

void CEquipment2015Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) != IDM_ABOUTBOX)
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

void CEquipment2015Dlg::AppendLog(LPCTSTR msg)
{
    m_LogEdit.SetSel(m_LogEdit.GetWindowTextLength(), m_LogEdit.GetWindowTextLength());
    m_LogEdit.ReplaceSel(msg);
}

void CEquipment2015Dlg::AppendLogFormat(LPCTSTR fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    CString msg;
    msg.FormatV(fmt, args);
    va_end(args);
    AppendLog(msg);
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
    CString* pMsg = reinterpret_cast<CString*>(wParam);
    if (pMsg)
    {
        AppendLog(*pMsg);
        delete pMsg;
    }
    return 0;
}

//=============================================================================
// [예제] VMF 상태머신 모드 - VisionConnectionManager 사용
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfStateMachineWithVisionConnectionManager()
{
    /*
    m_orchestrator = CreateOrchestrator();
    if (!m_orchestrator)
    {
        AppendLog(_T("[CM StateMachine] Failed to create Orchestrator from Plugin.\r\n"));
        return;
    }

    RegisterOrchestratorObserver(m_orchestrator, _T("VMF_CM_StateMachine"));

    VMF::VisionConnectionConfig connConfig("192.168.1.100", 5000, 5000);

    bool started = m_orchestrator->RunSequence(nullptr , connConfig);

    if (started)
    {
        AppendLog(_T("[CM StateMachine] Sequence started with VisionConnectionManager.\r\n"));
        AppendLog(_T("  - Vision 서버: 192.168.1.100:5000 (단일 소켓 공유)\r\n"));
    }
    else
    {
        AppendLog(_T("[CM StateMachine] Failed to start sequence.\r\n"));
    }
    */
}

//=============================================================================
// [예제] 다중 서버 사용 예시 - 여러 Orchestrator가 다른 Vision 서버에 접속
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfMultiServerExample()
{
    /*
    CString logMsg;
    logMsg = _T("[Multi-Server Example] Starting...\r\n\r\n");

    m_multiServerOrchestrators.clear();

    auto makeOrchestrator = [this](LPCTSTR name) -> std::shared_ptr<VMF::Orchestrator>
        {
            auto orch = CreateOrchestrator();
            if (orch)
                RegisterOrchestratorObserver(orch, name);
            return orch;
        };

    // ---- Orchestrator #1: Server A ----
    logMsg += _T("--- Orchestrator #1 → Server A (192.168.1.100:5000) ---\r\n");
    VMF::VisionConnectionConfig configA("192.168.1.100", 5000, 5000);
    auto orch1 = makeOrchestrator(_T("ServerA"));
    bool started1 = orch1 ? orch1->RunSequence(nullptr, configA) : false;
    logMsg.AppendFormat(_T("  StartSequence: %s\r\n\r\n"), started1 ? _T("SUCCESS") : _T("FAILED"));
    m_multiServerOrchestrators.push_back(orch1);

    // ---- Orchestrator #2: Server A (같은 서버 - 소켓 공유!) ----
    logMsg += _T("--- Orchestrator #2 → Server A (192.168.1.100:5000) [Socket Shared!] ---\r\n");
    auto orch2 = makeOrchestrator(_T("ServerA-2"));
    bool started2 = orch2 ? orch2->RunSequence(nullptr, configA) : false;
    logMsg.AppendFormat(_T("  StartSequence: %s (Same socket reused!)\r\n\r\n"), started2 ? _T("SUCCESS") : _T("FAILED"));
    m_multiServerOrchestrators.push_back(orch2);

    // ---- Orchestrator #3: Server B (별도 소켓) ----
    logMsg += _T("--- Orchestrator #3 → Server B (10.0.0.50:6000) [Separate Socket] ---\r\n");
    VMF::VisionConnectionConfig configB("10.0.0.50", 6000, 5000);
    auto orch3 = makeOrchestrator(_T("ServerB"));
    bool started3 = orch3 ? orch3->RunSequence(nullptr, configB) : false;
    logMsg.AppendFormat(_T("  StartSequence: %s (New socket for Server B)\r\n"), started3 ? _T("SUCCESS") : _T("FAILED"));
    m_multiServerOrchestrators.push_back(orch3);

    logMsg += _T("\r\n[Multi-Server] Summary:\r\n");
    logMsg += _T("  Server A: 1 socket (shared by 2 Orchestrators)\r\n");
    logMsg += _T("  Server B: 1 socket (dedicated)\r\n");
    logMsg += _T("  Total: 2 sockets (not 3)\r\n");

    AppendLog(logMsg);
    */
}
