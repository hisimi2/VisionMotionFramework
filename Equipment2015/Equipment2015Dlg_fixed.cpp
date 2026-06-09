// Equipment2015Dlg.cpp : êµ¬í˜„ ?Œì¼
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
    m_StopSwitch.setStatus(true); // ì´ˆê¸° ?íƒœ??Stop

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
    ON_BN_CLICKED(IDC_VMF_DIRECT, &CEquipment2015Dlg::OnBnClickedVmfDirect)
END_MESSAGE_MAP()

// CEquipment2015Dlg ë©”ì‹œì§€ ì²˜ë¦¬ê¸?BOOL CEquipment2015Dlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

    // Windows ê¸°ë³¸ ë¹„ì£¼??ë§¤ë‹ˆ?€ë¡?ë³µì›
    CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));

	// ?œìŠ¤??ë©”ë‰´??"?•ë³´..." ë©”ë‰´ ??ª©??ì¶”ê??©ë‹ˆ??

	// IDM_ABOUTBOX???œìŠ¤??ëª…ë ¹ ë²”ìœ„???ˆì–´???©ë‹ˆ??
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

	// ???€???ì???„ì´ì½˜ì„ ?¤ì •?©ë‹ˆ??
	SetIcon(m_hIcon, TRUE);			// ???„ì´ì½˜ì„ ?¤ì •?©ë‹ˆ??
	SetIcon(m_hIcon, FALSE);		// ?‘ì? ?„ì´ì½˜ì„ ?¤ì •?©ë‹ˆ??

	// ThreadsManager ì´ˆê¸°????Load1, Load2 ?±ë¡
	m_threadsMgr.Initialize();

    // Observer?ì„œ UIë¡?ê²°ê³¼ ì¶œë ¥ ??PostMessageë¡?UI ?¤ë ˆ?œì— ?„ë‹¬
    m_threadsMgr.AddObserver([this](const std::string& name, int requestId, const std::vector<std::string>& results)
    {
        ActivityResultData* pData = new ActivityResultData();
        pData->activityName = name.c_str();
        pData->requestId = requestId;

        for (const auto& result : results)
        {
            pData->detail += CString(result.c_str()) + _T("\r\n");
        }

        // UI ?¤ë ˆ?œë¡œ ë©”ì‹œì§€ ?„ì†¡ (ë¹„ë™ê¸? ?¤ë ˆ???ˆì „)
        ::PostMessage(this->m_hWnd, WM_ACTIVITY_RESULT, (WPARAM)pData, (LPARAM)0);
    });

    SetTimer(1, 1000, NULL); // 1ì´ˆë§ˆ???€?´ë¨¸ ?´ë²¤??ë°œìƒ

	return TRUE;	// ?¬ì»¤?¤ë? ì»¨íŠ¸ë¡¤ì— ?¤ì •?˜ì? ?Šìœ¼ë©?TRUEë¥?ë°˜í™˜?©ë‹ˆ??
}


void CEquipment2015Dlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: ?¬ê¸°??ë©”ì‹œì§€ ì²˜ë¦¬ê¸?ì½”ë“œë¥?ì¶”ê? ë°??ëŠ” ê¸°ë³¸ê°’ì„ ?¸ì¶œ?©ë‹ˆ??
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
		CPaintDC dc(this);	// ê·¸ë¦¬ê¸°ë? ?„í•œ ?”ë°”?´ìŠ¤ ì»¨í…?¤íŠ¸?…ë‹ˆ??

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ?´ë¼?´ì–¸???¬ê°?•ì—???„ì´ì½˜ì„ ê°€?´ë°??ë§ì¶¥?ˆë‹¤.
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

// OnActivityResult ??UI ?¤ë ˆ?œì—???¤í–‰??(PostMessage ?˜ì‹ )
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
// [?ˆì œ] Orchestrator + Strategy ?¨í„´???¬ìš©???œí€€???¤í–‰
// - VMF::Orchestratorë¥??ì„±?˜ê³  CLoad1LeftPlateJIGFocusCheckSequenceStrategyë¥?//   ?¤í–‰?˜ëŠ” ë°©ë²•??ë³´ì—¬ì¤ë‹ˆ??
// - ?µì?ë²„ë? ?±ë¡?˜ì—¬ ?¤í–‰ ê²°ê³¼ë¥?UI ë¡œê·¸??ì¶œë ¥?©ë‹ˆ??
// - ?¤ì œ ?˜ë“œ?¨ì–´ê°€ ?†ëŠ” ê²½ìš° CMockActuatorë¥??¬ìš©?????ˆìŠµ?ˆë‹¤.
//=============================================================================


// ============================================================
// [?„ìš” ?¤ë” includes]
// ============================================================
#include "Orchestrator.h"
#include "IActuator.h"
#include "VMFComposition/Load1/Strategies/CLoad1LeftPlateJIGFocusCheckSequenceStrategy.h"

// ?¤í–‰???„ìš”??Actuator êµ¬í˜„ (?¤ì œ ?˜ë“œ?¨ì–´ ?ëŠ” Mock)
#include "VMFComposition/Load1/VatAdapterLoad1.h"

// ì§ì ‘ ëª¨ë“œ?ì„œ ?„ìš”???¤ë”
#include "RepositoryFactory.h"
#include "VMFComposition/Mock/CMockVisionEventHandler.h"

// ============================================================
// 1. Orchestrator ?ì„± ë°??µì?ë²??±ë¡ (OnInitDialog ?±ì—??
// ============================================================
void CEquipment2015Dlg::InitOrchestratorExample()
{
    // 1-1. Orchestrator ?¸ìŠ¤?´ìŠ¤ ?ì„±
    m_orchestrator = std::make_shared<VMF::Orchestrator>();

    // 1-2. ?µì?ë²??±ë¡: ?œí€€???¤í–‰ ê²°ê³¼ë¥?UI ë¡œê·¸???œì‹œ
    m_orchestrator->AddObserver([this](const VMF::VisionResultPayload& payload)
    {
        CString msg;
        msg.Format(_T("[Orchestrator] RequestId=%d\r\n"), payload.requestId);

        for (const auto& result : payload.results)
        {
            msg += CString(result.c_str()) + _T("\r\n");
        }

// UI ?¤ë ˆ?œë¡œ ?„ë‹¬ (PostMessage ?¬ìš©)
        ActivityResultData* pData = new ActivityResultData();
        pData->activityName = _T("Orchestrator");
        pData->requestId = payload.requestId;
        pData->detail = msg;
        ::PostMessage(m_hWnd, WM_ACTIVITY_RESULT, (WPARAM)pData, 0);
    });
}

// ============================================================
// 2. Actuator ?ì„± ë°??œí€€???¤í–‰
// ============================================================
void CEquipment2015Dlg::RunOrchestratorSequenceExample()
{
    if (!m_orchestrator)
    {
        AfxMessageBox(_T("Orchestratorê°€ ì´ˆê¸°?”ë˜ì§€ ?Šì•˜?µë‹ˆ??"));
        return;
    }

    // 2-1. Actuator ?ì„± (?¤ì œ ?˜ë“œ?¨ì–´ ?´ëŒ‘??
// ?¤ì œ ?¥ë¹„ ?°ê²° ??
    // auto* actuator = new VMF_Load1::VatAdapterLoad1(?˜ë“œ?¨ì–´ ì´ˆê¸°???Œë¼ë¯¸í„°);

    // Mock ê°ì²´ ?¬ìš© (ê°œë°œ/?ŒìŠ¤?¸ìš©):
    // auto* actuator = new VMF::CMockActuator();

    // ???¤ì œ Actuator ì£¼ì…?€ Equipment ?„ë¡œ?íŠ¸???˜ë“œ?¨ì–´ ì´ˆê¸°???´í›„ ?˜í–‰
    VMF::IActuator* actuator = nullptr; // ?¤ì œ êµ¬í˜„ ???˜ë“œ?¨ì–´ ?´ëŒ‘?°ë¡œ êµì²´

    // 2-2. ?œí”Œë¦?ë©”ì„œ?œë? ?µí•´ Strategy ?€?…ì„ ?„ë‹¬?˜ì—¬ ?œí€€???œì‘
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
// 3. ?œí€€??ì¤‘ë‹¨
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
// 4. ?¤í–‰ ê²°ê³¼ ?°ì´???‘ê·¼ (Orchestrator -> Context -> Repository)
// ============================================================
void CEquipment2015Dlg::AccessSequenceDataExample()
{
    if (!m_orchestrator) return;

    // Orchestratorë¥??µí•´ Repository ?‘ê·¼
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

//=============================================================================
// [?ˆì œ] VMF ?íƒœë¨¸ì‹  ëª¨ë“œ (State Machine) - ?´ë¦­ ?¸ë“¤??// - Orchestrator::StartSequence<T>() ?¬ìš©
// - Strategy ??VP/Repo/Builder ??RunController ??AsyncExecutor(ë³„ë„ ?¤ë ˆ?? ?¤í–‰
// - ê²°ê³¼??Observer ì½œë°± ??PostMessage ??UI ë¡œê·¸ ì¶œë ¥
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfStateMachine()
{
    // --- 1. Orchestrator ?ì„± ---
    m_orchestrator = std::make_shared<VMF::Orchestrator>();

    // --- 2. Observer ?±ë¡ (?œí€€???¤í–‰ ê²°ê³¼ë¥?UI ë¡œê·¸??ì¶œë ¥) ---
    m_orchestrator->AddObserver([this](const VMF::VisionResultPayload& payload)
    {
        CString msg;
        msg.Format(_T("[StateMachine Mode] RequestId=%d\r\n"), payload.requestId);

        for (const auto& result : payload.results)
        {
            msg += CString(result.c_str()) + _T("\r\n");
        }

        ActivityResultData* pData = new ActivityResultData();
        pData->activityName = _T("VMF_StateMachine");
        pData->requestId = payload.requestId;
        pData->detail = msg;
        ::PostMessage(m_hWnd, WM_ACTIVITY_RESULT, (WPARAM)pData, 0);
    });

    // --- 3. Actuator ?ì„± ---
    // ???¤ì œ ?˜ë“œ?¨ì–´ ?°ê²° ??VatAdapterLoad1(ì´ˆê¸°?”íŒŒ?¼ë??? ë¡?êµì²´
    VMF::IActuator* actuator = nullptr;

    // --- 4. Strategy ?œí”Œë¦¿ìœ¼ë¡??íƒœë¨¸ì‹  ?œì‘ ---
    //    StartSequence<T>() ?´ë? ?™ì‘:
    //    ??CLoad1LeftPlateJIGFocusCheckSequenceStrategy ?ì„±
    //    ??CreateRepository() ??SqliteDataRepository ?ì„± ë°?Initialize()
    //    ??CreateVisionProcessor() ??CMockVisionEventHandler ?ì„± ë°?Initialize(config)
    //    ??CreateContext(vp, repo) ??Context ?ì„±, VP/Repo ?°ê²°
    //    ??ConfigureParams(ctx) ??SetParam + LoadInspInitPos + AddVisionPoint
    //    ??CreateBuilder() ??CLoad1ZFocusSequenceBuilder ?ì„±
    //    ??RunController(builder, ctx, actuator) ?ì„±
    //    ??AsyncExecutor ?ì„± ??SetResultSink(this) ??SetRunner
    //    ??RunSequence("Load1ZFocus") ??ë³„ë„ ?¤ë ˆ?œì—??Sequence::Execute() ?¤í–‰
    //    ?¤í–‰ ?„ë£Œ ??AsyncExecutor ??IResultSink::NotifyVisionResult ??Observer ì½œë°±
    bool started = m_orchestrator->StartSequence<
        VMF_Load1::CLoad1LeftPlateJIGFocusCheckSequenceStrategy
    >(actuator);

    CString msg;
    if (started)
    {
        msg = _T("[StateMachine Mode] Sequence 'Load1ZFocus' started successfully. (ë¹„ë™ê¸??¤í–‰)\r\n");
        msg += _T("  - ë³„ë„ ?¤ë ˆ?œì—??Sequence::Execute() ?¤í–‰ ì¤?r\n");
        msg += _T("  - ?„ë£Œ ??Observer ì½œë°±?¼ë¡œ ê²°ê³¼ ?˜ì‹ \r\n");
    }
    else
    {
        msg = _T("[StateMachine Mode] Failed to start sequence.\r\n");
    }

    m_LogEdit.SetSel(m_LogEdit.GetWindowTextLength(), m_LogEdit.GetWindowTextLength());
    m_LogEdit.ReplaceSel(msg);
}

//=============================================================================
// [?ˆì œ] VMF ì§ì ‘ ëª¨ë“œ (Direct Mode) - ?´ë¦­ ?¸ë“¤??// - Orchestrator::InitializeDirectWithStrategy<T>() ?¬ìš©
// - Strategy??ì»´í¬?ŒíŠ¸ ì¡°ë¦½ ë¡œì§(CreateRepository, CreateVisionProcessor, ConfigureParams)ë§??¬ì‚¬??// - ?íƒœë¨¸ì‹ (RunController + AsyncExecutor)?€ ?¤í–‰?˜ì? ?ŠìŒ
// - ExecuteDirectVisionCommand()ë¡??™ê¸°??ë¹„ì „ ëª…ë ¹ ?¤í–‰
//=============================================================================
void CEquipment2015Dlg::OnBnClickedVmfDirect()
{
    // --- 1. Orchestrator ?ì„± ---
    m_orchestrator = std::make_shared<VMF::Orchestrator>();

    // --- 2. Observer ?±ë¡ (ì§ì ‘ ëª¨ë“œ?ì„œ??Observer ê°€?? ---
    m_orchestrator->AddObserver([this](const VMF::VisionResultPayload& payload)
    {
        CString msg;
        msg.Format(_T("[Direct Mode] RequestId=%d\r\n"), payload.requestId);

        for (const auto& result : payload.results)
        {
            msg += CString(result.c_str()) + _T("\r\n");
        }

        ActivityResultData* pData = new ActivityResultData();
        pData->activityName = _T("VMF_Direct");
        pData->requestId = payload.requestId;
        pData->detail = msg;
        ::PostMessage(m_hWnd, WM_ACTIVITY_RESULT, (WPARAM)pData, 0);
    });

    // --- 3. Strategyë¥??µí•´ ì»´í¬?ŒíŠ¸ ?ì„±/ì¡°ë¦½ (ì§ì ‘ ëª¨ë“œ) ---
    //    InitializeDirectWithStrategy<T>() ?´ë? ?™ì‘:
    //    ??CLoad1LeftPlateJIGFocusCheckSequenceStrategy ?ì„±
    //    ??CreateRepository() ??SqliteDataRepository ?ì„± ë°?Initialize()
    //    ??CreateVisionProcessor() ??CMockVisionEventHandler ?ì„± ë°?Initialize(config)
    //    ??CreateContext(vp, repo) ??Context ?ì„±, VP/Repo ?°ê²°
    //    ??ConfigureParams(ctx) ??SetParam + LoadInspInitPos + AddVisionPoint
    //    ??VP/Repo/Contextë¥?m_directXXX???€??(RunController/AsyncExecutor ë¯¸ì‚¬??
    bool ok = m_orchestrator->InitializeDirectWithStrategy<
        VMF_Load1::CLoad1LeftPlateJIGFocusCheckSequenceStrategy
    >(nullptr);

    if (!ok)
    {
        AfxMessageBox(_T("[Direct Mode] InitializeDirectWithStrategy failed.\r\n"
                         "  - Check database path, migration_v1.sql, or VisionProcessor connection."));
        return;
    }

    // --- 4. ì§ì ‘ ë¹„ì „ ëª…ë ¹ ?¤í–‰ (?™ê¸°?? ---
    CString logMsg;
    logMsg = _T("[Direct Mode] Strategyë¡?ì»´í¬?ŒíŠ¸ ì¡°ë¦½ ?„ë£Œ.\r\n");
    logMsg += _T("  - Repository: SqliteDataRepository\r\n");
    logMsg += _T("  - VisionProcessor: CMockVisionEventHandler\r\n");
    logMsg += _T("  - ConfigureParamsë¡??Œë¼ë¯¸í„° ?¤ì • ?„ë£Œ\r\n\r\n");

    // 4-1. Measure ëª…ë ¹ ?¤í–‰
    bool cmdOk = m_orchestrator->ExecuteDirectVisionCommand(VMF::Measure);
    logMsg.AppendFormat(_T("[Direct Mode] ExecuteDirectVisionCommand(Measure) ??%s\r\n"),
                        cmdOk ? _T("SUCCESS") : _T("FAILED"));

    // 4-2. ?Œë¼ë¯¸í„°ë¥?ì¶”ê?ë¡??¤ì •?˜ì—¬ ?¤ë¥¸ ëª…ë ¹ ?¤í–‰
    VMF::StringMap params;
    params["ExtraParam"] = "DirectModeTest";
    cmdOk = m_orchestrator->ExecuteDirectVisionCommand(VMF::SetCok, params);
    logMsg.AppendFormat(_T("[Direct Mode] ExecuteDirectVisionCommand(SetCok, params) ??%s\r\n"),
                        cmdOk ? _T("SUCCESS") : _T("FAILED"));

    // 4-3. VisionProcessor?ì„œ ìµœì‹  ?°ì´??ì¡°íšŒ
    VMF::VisionProcessorPtr vp = m_orchestrator->GetVisionProcessor();
    if (vp)
    {
        VMF::IVisionProcessor::DataMap latestData = vp->GetLatestData(VMF::Measure);
        logMsg += _T("  - GetLatestData(Measure) keys: ");
        for (const auto& kv : latestData)
        {
            logMsg += CString(kv.first.c_str()) + _T("=") + CString(kv.second.c_str()) + _T(" ");
        }
        logMsg += _T("\r\n");
    }

    // --- 5. Contextë¥??µí•´ ?€?¥ëœ ?Œë¼ë¯¸í„° ?•ì¸ ---
    VMF::VisionContextPtr ctx = m_orchestrator->GetOrCreateContext();
    if (ctx)
    {
        std::string recipe = ctx->GetSeqParam("Recipe");
        int camIdx = ctx->GetSeqParamAs<int>("CameraIndex", -1);
        logMsg.AppendFormat(_T("  - Context seqParams: Recipe=%s, CameraIndex=%d\r\n"),
                            CString(recipe.c_str()), camIdx);
    }

    m_LogEdit.SetSel(m_LogEdit.GetWindowTextLength(), m_LogEdit.GetWindowTextLength());
    m_LogEdit.ReplaceSel(logMsg);
}
