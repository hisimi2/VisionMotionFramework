// Equipment2015Dlg.cpp : 구현 파일
//

#include "stdafx.h"
#include "Equipment2015.h"
#include "Equipment2015Dlg.h"
#include "afxdialogex.h"

#include "Orchestrator.h"
#include "IActuator.h"
#include "ComponentSetupBase.h"   // DLL Plugin에서 반환되는 Strategy 객체 타입
#include "RepositoryFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// ============================================================================
// SetupStrategy Factory Interface
// VMFEquipmentPlugin.dll의 import library(.lib)를 암시적 링크한 후,
// CreateSetupStrategy() / DestroySetupStrategy()를 직접 호출하여 사용합니다.
// DLL은 import library를 통해 자동으로 로드되므로 LoadLibrary 불필요.
// ============================================================================
#include "PluginFactory.h"

CEquipment2015Dlg::CEquipment2015Dlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_EQUIPMENT2015_DIALOG, pParent)
    , m_StartSwitch("StartSwitch")
    , m_StopSwitch("StopSwitch")
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_StartSwitch.setGroup(&m_StopSwitch).setOption(IOPSwitch::PUSH, false);
    m_StopSwitch.setGroup(&m_StartSwitch).setOption(IOPSwitch::PUSH, false);
    m_StopSwitch.setStatus(true);
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
    ON_BN_CLICKED(IDC_VMF_CONNECTION_MANAGER, &CEquipment2015Dlg::OnBnClickedVmfStateMachineWithConnectionManager)
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

    m_threadsMgr.Initialize();

    // Observer 등록 — PostMessage로 UI 스레드에 결과 전달
    m_threadsMgr.AddObserver([this](const std::string& name, int requestId, const std::vector<std::string>& results)
    {
        ActivityResultData* pData = new ActivityResultData();
        pData->activityName = name.c_str();
        pData->requestId = requestId;

        for (const auto& result : results)
        {
            pData->detail += CString(result.c_str()) + _T("\r\n");
        }

        ::PostMessage(m_hWnd, WM_ACTIVITY_RESULT, (WPARAM)pData, 0);
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
            m_threadsMgr.GetManager().RunAll();
            m_BtnStart.SetFaceColor(RGB(0, 180, 0), TRUE);
            m_BtnStop.SetFaceColor(RGB(240, 240, 240), TRUE);
        }
        else
        {
            m_threadsMgr.GetManager().PauseAll();
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
    ActivityResultData* pData = reinterpret_cast<ActivityResultData*>(wParam);
    if (pData)
    {
        CString msg;
        msg.Format(_T("[%s] completed (requestId=%d)\r\n%s"),
            pData->activityName.GetString(),
            pData->requestId,
            pData->detail.GetString());

        AppendLog(msg);
        delete pData;
    }
    return 0;
}


//=============================================================================
// 공통 Observer 헬퍼 — Orchestrator에 Observer를 등록하고
// PostMessage로 UI 스레드에 결과 전달
//=============================================================================
static void AttachObserverToOrchestrator(
    std::shared_ptr<VMF::Orchestrator> orchestrator,
    HWND hWnd,
    LPCTSTR activityName)
{
    std::wstring activityNameW(activityName);
    orchestrator->AddObserver([hWnd, activityNameW](const VMF::VisionResultPayload& payload)
    {
        CString msg;
        msg.Format(_T("[%s] RequestId=%d\r\n"), activityNameW.c_str(), payload.requestId);

        for (const auto& result : payload.results)
        {
            msg += CString(result.c_str()) + _T("\r\n");
        }

        ActivityResultData* pData = new ActivityResultData();
        pData->activityName = activityNameW.c_str();
        pData->requestId = payload.requestId;
        pData->detail = msg;
        ::PostMessage(hWnd, WM_ACTIVITY_RESULT, (WPARAM)pData, 0);
    });
}

//=============================================================================
// [예제] VMF 상태머신 모드 (State Machine) - DLL Plugin 방식
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfStateMachine()
{
    m_orchestrator = std::make_shared<VMF::Orchestrator>();
    AttachObserverToOrchestrator(m_orchestrator, m_hWnd, _T("VMF_StateMachine"));

    // 정적 라이브러리 함수 직접 호출
    VMF::ComponentSetupBase* pStrategy = CreateSetupStrategy();
    if (!pStrategy)
    {
        AppendLog(_T("[StateMachine Mode] Failed to create strategy.\r\n"));
        return;
    }

    // Strategy를 shared_ptr로 관리 (자동 소멸)
    auto strategyPtr = std::shared_ptr<VMF::ComponentSetupBase>(pStrategy,
        [](VMF::ComponentSetupBase* ptr) {
            DestroySetupStrategy(ptr);
        });

    // 시퀀스 실행 (※ 실제 하드웨어 연결 시 Actuator 교체 필요)
    bool started = m_orchestrator->StartSequenceFromStrategy(
        strategyPtr, nullptr /* actuator */);

    if (started)
    {
        AppendLog(_T("[StateMachine Mode] Sequence started via static library.\r\n"));
    }
    else
    {
        AppendLog(_T("[StateMachine Mode] Failed to start sequence.\r\n"));
    }
}

//=============================================================================
// [예제] VMF 상태머신 모드 - ConnectionManager 사용
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfStateMachineWithConnectionManager()
{
    m_orchestrator = std::make_shared<VMF::Orchestrator>();
    AttachObserverToOrchestrator(m_orchestrator, m_hWnd, _T("VMF_CM_StateMachine"));

    // 정적 라이브러리 함수 직접 호출
    VMF::ComponentSetupBase* pStrategy = CreateSetupStrategy();
    if (!pStrategy)
    {
        AppendLog(_T("[CM StateMachine] Failed to create strategy.\r\n"));
        return;
    }

    auto strategyPtr = std::shared_ptr<VMF::ComponentSetupBase>(pStrategy,
        [](VMF::ComponentSetupBase* ptr) {
            DestroySetupStrategy(ptr);
        });

    VMF::VisionConnectionConfig connConfig("192.168.1.100", 5000, 5000);

    bool started = m_orchestrator->StartSequenceFromStrategy(
        strategyPtr, nullptr /* actuator */, connConfig);

    if (started)
    {
        AppendLog(_T("[CM StateMachine] Sequence started with ConnectionManager.\r\n"));
        AppendLog(_T("  - Vision 서버: 192.168.1.100:5000 (단일 소켓 공유)\r\n"));
    }
    else
    {
        AppendLog(_T("[CM StateMachine] Failed to start sequence.\r\n"));
    }
}

//=============================================================================
// [예제] 정적 라이브러리 Helper - Orchestrator 생성 및 Strategy 로드
//=============================================================================
static std::shared_ptr<VMF::Orchestrator> CreatePluginOrchestrator(
    HWND hWnd,
    LPCTSTR name,
    const VMF::VisionConnectionConfig& config,
    std::shared_ptr<VMF::ComponentSetupBase>& outStrategy)
{
    auto orchestrator = std::make_shared<VMF::Orchestrator>();
    AttachObserverToOrchestrator(orchestrator, hWnd, name);

    // 정적 라이브러리 함수 직접 호출
    // 정적 라이브러리 함수 직접 호출
    VMF::ComponentSetupBase* pStrategy = CreateSetupStrategy();
    if (!pStrategy)
    {
        return nullptr;
    }

    outStrategy = std::shared_ptr<VMF::ComponentSetupBase>(pStrategy,
        [](VMF::ComponentSetupBase* ptr) {
            DestroySetupStrategy(ptr);
        });

    return orchestrator;
}

//=============================================================================
// [예제] 다중 서버 사용 예시 - 여러 Orchestrator가 다른 Vision 서버에 접속
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfMultiServerExample()
{
    CString logMsg;
    logMsg = _T("[Multi-Server Example] Starting...\r\n\r\n");

    m_multiServerOrchestrators.clear();

    // ---- Orchestrator #1: Server A (DLL Plugin) ----
    logMsg += _T("--- Orchestrator #1 → Server A (192.168.1.100:5000) [DLL Plugin] ---\r\n");
    VMF::VisionConnectionConfig configA("192.168.1.100", 5000, 5000);
    std::shared_ptr<VMF::ComponentSetupBase> strategy1;
    auto orch1 = CreatePluginOrchestrator(m_hWnd, _T("ServerA"), configA, strategy1);
    bool started1 = false;
    if (orch1 && strategy1)
    {
        started1 = orch1->StartSequenceFromStrategy(strategy1, nullptr, configA);
    }
    logMsg.AppendFormat(_T("  StartSequence: %s\r\n\r\n"), started1 ? _T("SUCCESS") : _T("FAILED"));
    if (orch1) m_multiServerOrchestrators.push_back(orch1);

    // ---- Orchestrator #2: Server A (같은 서버 - DLL Plugin + 소켓 공유!) ----
    logMsg += _T("--- Orchestrator #2 → Server A (192.168.1.100:5000) [Socket Shared!] ---\r\n");
    std::shared_ptr<VMF::ComponentSetupBase> strategy2;
    auto orch2 = CreatePluginOrchestrator(m_hWnd, _T("ServerA-2"), configA, strategy2);
    bool started2 = false;
    if (orch2 && strategy2)
    {
        started2 = orch2->StartSequenceFromStrategy(strategy2, nullptr, configA);
    }
    logMsg.AppendFormat(_T("  StartSequence: %s (Same socket reused!)\r\n\r\n"), started2 ? _T("SUCCESS") : _T("FAILED"));
    if (orch2) m_multiServerOrchestrators.push_back(orch2);

    // ---- Orchestrator #3: Server B (별도 소켓) ----
    logMsg += _T("--- Orchestrator #3 → Server B (10.0.0.50:6000) [Separate Socket] ---\r\n");
    VMF::VisionConnectionConfig configB("10.0.0.50", 6000, 5000);
    std::shared_ptr<VMF::ComponentSetupBase> strategy3;
    auto orch3 = CreatePluginOrchestrator(m_hWnd, _T("ServerB"), configB, strategy3);
    bool started3 = false;
    if (orch3 && strategy3)
    {
        started3 = orch3->StartSequenceFromStrategy(strategy3, nullptr, configB);
    }
    logMsg.AppendFormat(_T("  StartSequence: %s (New socket for Server B)\r\n"), started3 ? _T("SUCCESS") : _T("FAILED"));
    if (orch3) m_multiServerOrchestrators.push_back(orch3);

    logMsg += _T("\r\n[Multi-Server] Summary:\r\n");
    logMsg += _T("  Server A: 1 socket (shared by 2 Orchestrators)\r\n");
    logMsg += _T("  Server B: 1 socket (dedicated)\r\n");
    logMsg += _T("  Total: 2 sockets (not 3)\r\n");

    AppendLog(logMsg);
}

//=============================================================================
// [예제] VMF 직접 모드 (Direct Mode) - 클릭 핸들러
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfDirect()
{
    // 정적 라이브러리 함수 직접 호출
    VMF::ComponentSetupBase* pStrategy = CreateSetupStrategy();
    if (!pStrategy)
    {
        AppendLog(_T("[Direct Mode] Failed to create strategy.\r\n"));
        return;
    }

    auto strategyPtr = std::shared_ptr<VMF::ComponentSetupBase>(pStrategy,
        [](VMF::ComponentSetupBase* ptr) {
            DestroySetupStrategy(ptr);
        });

    m_orchestrator = std::make_shared<VMF::Orchestrator>();
    AttachObserverToOrchestrator(m_orchestrator, m_hWnd, _T("VMF_Direct"));

    bool ok = m_orchestrator->InitializeDirect(strategyPtr);

    if (!ok)
    {
        AfxMessageBox(_T("[Direct Mode] InitializeDirect failed.\r\n")
                      _T("  - Check DLL Plugin implementation.\r\n"));
        return;
    }

    CString logMsg;
    logMsg = _T("[Direct Mode] DLL Plugin Strategy로 컴포넌트 조립 완료.\r\n\r\n");

// Measure 명령 실행
    bool cmdOk = m_orchestrator->ExecuteDirectVisionCommand(VMF::Measure);
    logMsg.AppendFormat(_T("[Direct Mode] Execute(Measure) → %s\r\n"),
                        cmdOk ? _T("SUCCESS") : _T("FAILED"));

    // 파라미터와 함께 명령 실행
    VMF::StringMap params;
    params["ExtraParam"] = "DirectModeTest";
    cmdOk = m_orchestrator->ExecuteDirectVisionCommand(VMF::SetCok, params);
    logMsg.AppendFormat(_T("[Direct Mode] Execute(SetCok, params) → %s\r\n"),
                        cmdOk ? _T("SUCCESS") : _T("FAILED"));

    // 최신 데이터 조회
    VMF::VisionProcessorPtr vp = m_orchestrator->GetVisionProcessor();
    if (vp)
    {
        auto latestData = vp->GetLatestData(VMF::Measure);
        logMsg += _T("  - GetLatestData(Measure) keys: ");
        for (const auto& kv : latestData)
        {
            logMsg += CString(kv.first.c_str()) + _T("=") + CString(kv.second.c_str()) + _T(" ");
        }
        logMsg += _T("\r\n");
    }

    // Context 통해 Task별 파라미터 확인
    VMF::VisionContextPtr ctx = m_orchestrator->GetOrCreateContext();
    if (ctx)
    {
        VMF::VisionParams moveParams = ctx->GetTaskParams("SampleMoveToStartPositionTask");
        VMF::VisionParams focusParams = ctx->GetTaskParams("SamplePerformFocusScanningTask");
        logMsg.AppendFormat(_T("  - Context: MoveTaskParams.size=%zu, FocusTaskParams.size=%zu\r\n"),
                            moveParams.visionParams.size(), focusParams.visionParams.size());
    }

    AppendLog(logMsg);
}
