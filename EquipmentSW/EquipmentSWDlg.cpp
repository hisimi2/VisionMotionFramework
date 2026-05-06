#include "stdafx.h"
#include "EquipmentSW.h"
#include "EquipmentSWDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "DVH_VAT/Load1/Strategies/CLoad1LeftPlateJIGFocusCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1LeftPlateJigFOVCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1LeftPlateJigCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1LowCamFOVCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1LowCamCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1HandPitchCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1LeftUpperCamCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1RightPlateJIGFocusCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1RightJigFOVCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1RightPlateJigCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1RightUpperCamCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1LowPlateJIGCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1LowCamPlateJIGFocusCheckSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1PCDSequenceStrategy.h"
#include "DVH_VAT/Load1/Strategies/CLoad1BacklashSequenceStrategy.h"

using namespace VAT_LOAD1::Strategies;

// RegisterWindowMessage를 파일 스코프에서 얻어 메시지 맵에 사용
// 이름 충돌 회피를 위해 클래스 내부 정적 멤버명과 다른 변수명을 사용합니다.
static UINT g_VisionResultMsgId = DVH_VAT::CVatEngineUiAdapter::GetVisionResultMsgId();

CEquipmentSWDlg::CEquipmentSWDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(IDD_EQUIPMENTSW_DIALOG, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_parts = boost::make_shared<VAT_LOAD1::Load1Parts>();
	m_adapter = boost::make_shared<VAT_LOAD1::VatAdapterLoad1>(m_parts.get());
}
CEquipmentSWDlg::~CEquipmentSWDlg()
{
}
void CEquipmentSWDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SPREAD1, m_spread_teachingpos);
	DDX_Control(pDX, IDC_SPREAD2, m_spread_pickerpos);
	DDX_Control(pDX, IDC_SPREAD3, m_spread_pickercamdist);
	DDX_Control(pDX, IDC_SPREAD4, m_spread_platejig_left);
	DDX_Control(pDX, IDC_SPREAD5, m_spread_platejig_right);
}
BEGIN_MESSAGE_MAP(CEquipmentSWDlg, CDialogEx)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_DESTROY()
    ON_REGISTERED_MESSAGE(g_VisionResultMsgId, OnDVH_VATResultMsg)
	ON_BN_CLICKED(IDC_BTN_FOCUS_LEFT, &CEquipmentSWDlg::OnBnClickedBtnFocusLeft)
	ON_BN_CLICKED(IDC_BTN_FOCUS_RIGHT, &CEquipmentSWDlg::OnBnClickedBtnFocusRight)
	ON_BN_CLICKED(IDC_BTN_FOV_LEFT, &CEquipmentSWDlg::OnBnClickedBtnFovLeft)
	ON_BN_CLICKED(IDC_BTN_FOV_RIGHT, &CEquipmentSWDlg::OnBnClickedBtnFovRight)
	ON_BN_CLICKED(IDC_BTN_PLATE_LEFT, &CEquipmentSWDlg::OnBnClickedBtnPlateLeft)
	ON_BN_CLICKED(IDC_BTN_PLATE_RIGHT, &CEquipmentSWDlg::OnBnClickedBtnPlateRight)
	ON_BN_CLICKED(IDC_BTN_PLATE_LOW, &CEquipmentSWDlg::OnBnClickedBtnPlateLow)
	ON_BN_CLICKED(IDC_BTN_FOV_LOW, &CEquipmentSWDlg::OnBnClickedBtnFovLow)
	ON_BN_CLICKED(IDC_BTN_LOW, &CEquipmentSWDlg::OnBnClickedBtnLow)
	ON_BN_CLICKED(IDC_BTN_HANDPITCH, &CEquipmentSWDlg::OnBnClickedBtnHandpitch)
	ON_BN_CLICKED(IDC_BTN_LEFT, &CEquipmentSWDlg::OnBnClickedBtnLeft)
	ON_BN_CLICKED(IDC_BTN_RIGHT, &CEquipmentSWDlg::OnBnClickedBtnRight)
	ON_BN_CLICKED(IDC_BTN_PCD, &CEquipmentSWDlg::OnBnClickedBtnPcd)
	ON_BN_CLICKED(IDC_BTN_BACKLASH, &CEquipmentSWDlg::OnBnClickedBtnBacklash)
	ON_BN_CLICKED(IDC_BTN_PLATEJIG_INSP, &CEquipmentSWDlg::OnBnClickedBtnPlatejigInsp)
	ON_BN_CLICKED(IDC_BTN_AFTER_PLATEJIG_INSP, &CEquipmentSWDlg::OnBnClickedBtnAfterPlatejigInsp)
	ON_BN_CLICKED(IDC_BTN_TEACHING_INSP, &CEquipmentSWDlg::OnBnClickedBtnTeachingInsp)
	ON_BN_CLICKED(IDC_BTN_VAT_STOP, &CEquipmentSWDlg::OnBnClickedBtnVatStop)
END_MESSAGE_MAP()

void CEquipmentSWDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
    {
       // CAboutDlg dlgAbout;
       // dlgAbout.DoModal();
    }
    else
    {
        CDialogEx::OnSysCommand(nID, lParam);
    }
}
void CEquipmentSWDlg::OnPaint()
{
    if (IsIconic())
    {
        CPaintDC dc(this); 

        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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
HCURSOR CEquipmentSWDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

BOOL CEquipmentSWDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // CVatEngineUiAdapter 사용을 위해 소유자 등록 (composition)
    m_engine.SetOwner(this);

    return TRUE;
}
void CEquipmentSWDlg::OnDestroy()
{
    // 다이얼로그 파괴 전에 시퀀스 정지
    m_engine.StopVatSequence();
    CDialogEx::OnDestroy();
}

/*
LRESULT CEquipmentSWDlg::OnDVH_VATResultMsg(WPARAM wParam, LPARAM / *lParam* /)
{
    typedef boost::shared_ptr<DVH_VAT::VisionResultPayload>* HeapSpPtr;

    HeapSpPtr pHeapSp = reinterpret_cast<HeapSpPtr>(wParam);
    if (!pHeapSp)
        return 0;

    // 힙에 있던 shared_ptr을 지역으로 복사해 소유권 확보하고 즉시 포인터 해제
    boost::shared_ptr<DVH_VAT::VisionResultPayload> payload = *pHeapSp;
    delete pHeapSp;

    if (!payload)
    {
        OutputDebugString(_T("Vision Result Recv: empty payload\r\n"));
    }

    return 0;
}*/

LRESULT CEquipmentSWDlg::OnDVH_VATResultMsg(WPARAM wParam, LPARAM)
{
	typedef boost::shared_ptr<DVH_VAT::VisionResultPayload>* HeapSpPtr;

	HeapSpPtr pHeapSp = reinterpret_cast<HeapSpPtr>(wParam);
	if (!pHeapSp)
		return 0;

	boost::shared_ptr<DVH_VAT::VisionResultPayload> payload = *pHeapSp;
	delete pHeapSp;

	if (!payload)
		return 0;

	// 결과 파싱
	std::string status;
	for (size_t i = 0; i < payload->results.size(); ++i)
	{
		if (payload->results[i].find("Status:") != std::string::npos)
		{
			status = payload->results[i];
			break;
		}
	}

	// 완료된 경우만 다음 실행
	if (status.find("completed") != std::string::npos)
	{
		if (!m_seqQueue.empty())
		{
			StartSequenceQueue();
		}
		else
		{
			m_isSequenceRunning = false;
		}
	}
	else if (status.find("aborted") != std::string::npos)
	{
		// 실패 시 큐 비우기
		while (!m_seqQueue.empty()) m_seqQueue.pop();
		m_isSequenceRunning = false;
	}

	return 0;
}

void CEquipmentSWDlg::StartSequenceQueue()
{
	if (m_seqQueue.empty())
		return;

	m_isSequenceRunning = true;

	SequenceFunc func = m_seqQueue.front();
	m_seqQueue.pop();

	if (!func())
	{
		m_isSequenceRunning = false;
	}
}

void CEquipmentSWDlg::RunLoad1PlateJigSequence()
{
	while (!m_seqQueue.empty()) m_seqQueue.pop();

	//Focus Left, Right, Low, FOV Left, Right, ... Low FOV, Low

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1LeftPlateJIGFocusCheckSequenceStrategy>(m_adapter.get()); // Left Cam Focus
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1RightPlateJIGFocusCheckSequenceStrategy>(m_adapter.get()); // Right Cam Focus
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1LowCamPlateJIGFocusCheckSequenceStrategy>(m_adapter.get()); // Low Cam Focus
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1LeftPlateJigFOVCheckSequenceStrategy>(m_adapter.get()); // Left Cam FOV
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1RightJigFOVCheckSequenceStrategy>(m_adapter.get()); // Right Cam FOV
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1LeftPlateJigCheckSequenceStrategy>(m_adapter.get());
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1RightPlateJigCheckSequenceStrategy>(m_adapter.get());
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1LowPlateJIGCheckSequenceStrategy>(m_adapter.get());
	});

	StartSequenceQueue();
}

void CEquipmentSWDlg::RunLoad1AfterPlateJigSequence()
{
	while (!m_seqQueue.empty()) m_seqQueue.pop();

	//Focus Left, Right, Low, FOV Left, Right, ... Low FOV, Low

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1LowCamFOVCheckSequenceStrategy>(m_adapter.get()); // Left Cam Focus
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1LowCamCheckSequenceStrategy>(m_adapter.get()); // Right Cam Focus
	});


	StartSequenceQueue();
}

void CEquipmentSWDlg::RunLoad1TeachingSequence()
{
	while (!m_seqQueue.empty()) m_seqQueue.pop();

	//Focus Left, Right, Low, FOV Left, Right, ... Low FOV, Low

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1HandPitchCheckSequenceStrategy>(m_adapter.get()); // Left Cam Focus
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1LeftUpperCamCheckSequenceStrategy>(m_adapter.get()); // Right Cam Focus
	});

	m_seqQueue.push([this]() {
		return m_engine.StartVatSequence<CLoad1RightUpperCamCheckSequenceStrategy>(m_adapter.get()); // Right Cam Focus
	});


	StartSequenceQueue();
}

void CEquipmentSWDlg::OnBnClickedBtnFocusLeft()
{
	if (!m_engine.StartVatSequence<CLoad1LeftPlateJIGFocusCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnFocusRight()
{
	if (!m_engine.StartVatSequence<CLoad1RightPlateJIGFocusCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnFovLeft()
{
	if (!m_engine.StartVatSequence<CLoad1LeftPlateJigFOVCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnFovRight()
{
	if (!m_engine.StartVatSequence<CLoad1RightJigFOVCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnPlateLeft()
{
	if (!m_engine.StartVatSequence<CLoad1LeftPlateJigCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnPlateRight()
{
	if (!m_engine.StartVatSequence<CLoad1RightPlateJigCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnPlateLow()
{
	if (!m_engine.StartVatSequence<CLoad1LowPlateJIGCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnFovLow()
{
	if (!m_engine.StartVatSequence<CLoad1LowCamFOVCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnLow()
{
	if (!m_engine.StartVatSequence<CLoad1LowCamCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnHandpitch()
{
	if (!m_engine.StartVatSequence<CLoad1HandPitchCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnLeft()
{
	if (!m_engine.StartVatSequence<CLoad1LeftUpperCamCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnRight()
{
	if (!m_engine.StartVatSequence<CLoad1RightUpperCamCheckSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnPcd()
{
	if (!m_engine.StartVatSequence<CLoad1PCDSequenceStrategy>(m_adapter.get())) { /* ... */ }
}


void CEquipmentSWDlg::OnBnClickedBtnBacklash()
{
	if (!m_engine.StartVatSequence<CLoad1BacklashSequenceStrategy>(m_adapter.get())) { /* ... */ }
}

void CEquipmentSWDlg::OnBnClickedBtnPlatejigInsp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RunLoad1PlateJigSequence();
}


void CEquipmentSWDlg::OnBnClickedBtnAfterPlatejigInsp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RunLoad1AfterPlateJigSequence();
}


void CEquipmentSWDlg::OnBnClickedBtnTeachingInsp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	RunLoad1TeachingSequence();
}

void CEquipmentSWDlg::OnBnClickedBtnVatStop()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_engine.StopVatSequence();
}


void CEquipmentSWDlg::UpdateSpread()
{
	struct TeachingPoint
	{
		int locateId;
		double xP;
		double yP;
		double zP;
		double focus;
	};

	struct CamPickerDistance
	{
		double narrowX;
		double narrowY;
		double wideX;
		double wideY;
	};

	TeachingPoint Ctray1;
	TeachingPoint Ctray2;
	TeachingPoint LoadTable1;
	TeachingPoint LoadTable2;

	TeachingPoint Picker_Narrow;
	TeachingPoint Picker_Wide;

	TeachingPoint Target_A_Left;
	TeachingPoint Target_B_Left;
	TeachingPoint Upper_Target_Left;

	TeachingPoint Target_A_Right;
	TeachingPoint Target_B_Right;
	TeachingPoint Upper_Target_Right;

	TeachingPoint Lower_Target;

	CamPickerDistance CPD_LEFT;
	CamPickerDistance CPD_RIGHT;

	int col = 0;
	int row = 0;
	int col2 = 4;
	int row2 = 5;
	int option = 1;

	float fontSize = 8.0;

	m_spread_teachingpos.ClearRange(col, row, col2, row2, option);
	m_spread_teachingpos.SetFontSize(fontSize);
	m_spread_teachingpos.SetColWidth(0, 11);

	m_spread_teachingpos.SetMaxRows(4);
	m_spread_teachingpos.SetMaxCols(4);

	m_spread_teachingpos.SetText(0, 0, _variant_t("Load1 PP"));

	m_spread_teachingpos.SetText(1, 0, _variant_t("X"));
	m_spread_teachingpos.SetText(2, 0, _variant_t("Y"));
	m_spread_teachingpos.SetText(3, 0, _variant_t("Z"));
	m_spread_teachingpos.SetText(4, 0, _variant_t("Focus"));

	m_spread_teachingpos.SetText(0, 1, _variant_t("CTray1"));
	m_spread_teachingpos.SetText(0, 2, _variant_t("CTray2"));
	m_spread_teachingpos.SetText(0, 3, _variant_t("LoadTable1"));
	m_spread_teachingpos.SetText(0, 4, _variant_t("LoadTable2"));


	///////////////////
	m_spread_pickerpos.ClearRange(col, row, col2, row2, option);
	m_spread_pickerpos.SetFontSize(fontSize);
	m_spread_pickerpos.SetColWidth(0, 11);
	m_spread_pickerpos.SetRowHeight(1, 20);
	m_spread_pickerpos.SetRowHeight(2, 20);

	m_spread_pickerpos.SetMaxRows(2);
	m_spread_pickerpos.SetMaxCols(3);

	m_spread_pickerpos.SetText(0, 0, _variant_t("Load1 PP"));

	m_spread_pickerpos.SetText(1, 0, _variant_t("X"));
	m_spread_pickerpos.SetText(2, 0, _variant_t("Y"));
	m_spread_pickerpos.SetText(3, 0, _variant_t("Focus"));

	m_spread_pickerpos.SetText(0, 1, _variant_t("Lower Cam Pos Wide"));
	m_spread_pickerpos.SetText(0, 2, _variant_t("Lower Cam Pos Narrow"));

	///////////////////
	m_spread_pickercamdist.ClearRange(col, row, col2, row2, option);
	m_spread_pickercamdist.SetFontSize(fontSize);
	m_spread_pickercamdist.SetColWidth(0, 11);
	m_spread_pickercamdist.SetRowHeight(1, 20);
	m_spread_pickercamdist.SetRowHeight(2, 20);


	m_spread_pickercamdist.SetMaxRows(2);
	m_spread_pickercamdist.SetMaxCols(2);

	m_spread_pickercamdist.SetText(0, 0, _variant_t("Load1 PP"));

	m_spread_pickercamdist.SetText(1, 0, _variant_t("X"));
	m_spread_pickercamdist.SetText(2, 0, _variant_t("Y"));

	m_spread_pickercamdist.SetText(0, 1, _variant_t("UpperCam1 Pos"));
	m_spread_pickercamdist.SetText(0, 2, _variant_t("UpperCam2 Pos"));

	///////////////////
	m_spread_platejig_left.ClearRange(col, row, col2, row2, option);
	m_spread_platejig_left.SetFontSize(fontSize);

	m_spread_platejig_left.SetMaxRows(4);
	m_spread_platejig_left.SetMaxCols(3);
	m_spread_platejig_left.SetColWidth(0, 11);

	m_spread_platejig_left.SetText(0, 0, _variant_t("Load1 PP L"));

	m_spread_platejig_left.SetText(1, 0, _variant_t("X"));
	m_spread_platejig_left.SetText(2, 0, _variant_t("Y"));
	m_spread_platejig_left.SetText(3, 0, _variant_t("Focus"));

	m_spread_platejig_left.SetText(0, 1, _variant_t("TargetA"));
	m_spread_platejig_left.SetText(0, 2, _variant_t("TargetB"));
	m_spread_platejig_left.SetText(0, 3, _variant_t("UpperTarget"));
	m_spread_platejig_left.SetText(0, 4, _variant_t("LowerTarget"));


	///////////////////
	m_spread_platejig_right.ClearRange(col, row, col2, row2, option);
	m_spread_platejig_right.SetFontSize(fontSize);

	m_spread_platejig_right.SetMaxRows(4);
	m_spread_platejig_right.SetMaxCols(3);
	m_spread_platejig_right.SetColWidth(0, 11);

	m_spread_platejig_right.SetText(0, 0, _variant_t("Load1 PP R"));

	m_spread_platejig_right.SetText(1, 0, _variant_t("X"));
	m_spread_platejig_right.SetText(2, 0, _variant_t("Y"));
	m_spread_platejig_right.SetText(3, 0, _variant_t("Focus"));

	m_spread_platejig_right.SetText(0, 1, _variant_t("TargetA"));
	m_spread_platejig_right.SetText(0, 2, _variant_t("TargetB"));
	m_spread_platejig_right.SetText(0, 3, _variant_t("UpperTarget"));
	m_spread_platejig_right.SetText(0, 4, _variant_t("LowerTarget"));

	Ctray1.locateId = 5;
	Ctray2.locateId = 6;

	LoadTable1.locateId = 12;
	LoadTable2.locateId = 13;

	Picker_Narrow.locateId = 20;
	Picker_Wide.locateId = 21;

	Target_A_Left.locateId = 1;
	Target_B_Left.locateId = 2;
	Upper_Target_Left.locateId = 3;

	Target_A_Right.locateId = 1;
	Target_B_Right.locateId = 2;
	Upper_Target_Right.locateId = 3;

	Lower_Target.locateId = 4;

	auto repo = m_engine.getDataRepository();

	if (repo->LoadTeachingResult(1, LoadTable1.locateId, 1, 0, LoadTable1.xP, LoadTable1.yP, LoadTable1.zP) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadTeachingResult(1, LoadTable2.locateId, 1, 0, LoadTable2.xP, LoadTable2.yP, LoadTable2.zP) != DVH_VAT::StorageSuccess)
		return;

	if (repo->LoadTeachingResult(1, Ctray1.locateId, 1, 0, Ctray1.xP, Ctray1.yP, Ctray1.zP) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadTeachingResult(1, Ctray2.locateId, 1, 0, Ctray2.xP, Ctray2.yP, Ctray2.zP) != DVH_VAT::StorageSuccess)
		return;

	double dummy;

	if (repo->LoadInspInitPos(6, LoadTable1.locateId, 1, dummy, dummy, LoadTable1.focus) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadInspInitPos(6, LoadTable2.locateId, 1, dummy, dummy, LoadTable2.focus) != DVH_VAT::StorageSuccess)
		return;

	if (repo->LoadInspInitPos(6, Ctray1.locateId, 1, dummy, dummy, Ctray1.focus) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadInspInitPos(7, Ctray2.locateId, 1, dummy, dummy, Ctray2.focus) != DVH_VAT::StorageSuccess)
		return;


	if (repo->LoadInspInitPos(1, Picker_Narrow.locateId, 1, Picker_Narrow.xP, Picker_Narrow.yP, Picker_Narrow.zP) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadInspInitPos(1, Picker_Wide.locateId, 1, Picker_Wide.xP, Picker_Wide.yP, Picker_Wide.zP) != DVH_VAT::StorageSuccess)
		return;

	if (repo->LoadPickerCamDistance(6, 1, CPD_LEFT.narrowX, CPD_LEFT.narrowY, CPD_LEFT.wideX, CPD_LEFT.wideY) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadPickerCamDistance(7, 1, CPD_RIGHT.narrowX, CPD_RIGHT.narrowY, CPD_RIGHT.wideX, CPD_RIGHT.wideY) != DVH_VAT::StorageSuccess)
		return;

	if (repo->LoadInspInitPos(6, Target_A_Left.locateId, 1, Target_A_Left.xP, Target_A_Left.yP, Target_A_Left.zP) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadInspInitPos(6, Target_B_Left.locateId, 1, Target_B_Left.xP, Target_B_Left.yP, Target_B_Left.zP) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadInspInitPos(6, Upper_Target_Left.locateId, 1, Upper_Target_Left.xP, Upper_Target_Left.yP, Upper_Target_Left.zP) != DVH_VAT::StorageSuccess)
		return;

	if (repo->LoadInspInitPos(7, Target_A_Right.locateId, 1, Target_A_Right.xP, Target_A_Right.yP, Target_A_Right.zP) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadInspInitPos(7, Target_B_Right.locateId, 1, Target_B_Right.xP, Target_B_Right.yP, Target_B_Right.zP) != DVH_VAT::StorageSuccess)
		return;
	if (repo->LoadInspInitPos(7, Upper_Target_Right.locateId, 1, Upper_Target_Right.xP, Upper_Target_Right.yP, Upper_Target_Right.zP) != DVH_VAT::StorageSuccess)
		return;

	if (repo->LoadInspInitPos(1, Lower_Target.locateId, 1, Lower_Target.xP, Lower_Target.yP, Lower_Target.zP) != DVH_VAT::StorageSuccess)
		return;

	CString str;

	// CTray1
	str.Format(_T("%.3f"), Ctray1.xP);
	m_spread_teachingpos.SetText(1, 1, _variant_t(str));

	str.Format(_T("%.3f"), Ctray1.yP);
	m_spread_teachingpos.SetText(2, 1, _variant_t(str));

	str.Format(_T("%.3f"), Ctray1.zP);
	m_spread_teachingpos.SetText(3, 1, _variant_t(str));

	str.Format(_T("%.3f"), Ctray1.focus);
	m_spread_teachingpos.SetText(4, 1, _variant_t(str));


	// CTray2
	str.Format(_T("%.3f"), Ctray2.xP);
	m_spread_teachingpos.SetText(1, 2, _variant_t(str));

	str.Format(_T("%.3f"), Ctray2.yP);
	m_spread_teachingpos.SetText(2, 2, _variant_t(str));

	str.Format(_T("%.3f"), Ctray2.zP);
	m_spread_teachingpos.SetText(3, 2, _variant_t(str));

	str.Format(_T("%.3f"), Ctray2.focus);
	m_spread_teachingpos.SetText(4, 2, _variant_t(str));


	// LoadTable1
	str.Format(_T("%.3f"), LoadTable1.xP);
	m_spread_teachingpos.SetText(1, 3, _variant_t(str));

	str.Format(_T("%.3f"), LoadTable1.yP);
	m_spread_teachingpos.SetText(2, 3, _variant_t(str));

	str.Format(_T("%.3f"), LoadTable1.zP);
	m_spread_teachingpos.SetText(3, 3, _variant_t(str));

	str.Format(_T("%.3f"), LoadTable1.focus);
	m_spread_teachingpos.SetText(4, 3, _variant_t(str));

	// LoadTable2
	str.Format(_T("%.3f"), LoadTable2.xP);
	m_spread_teachingpos.SetText(1, 4, _variant_t(str));

	str.Format(_T("%.3f"), LoadTable2.yP);
	m_spread_teachingpos.SetText(2, 4, _variant_t(str));

	str.Format(_T("%.3f"), LoadTable2.zP);
	m_spread_teachingpos.SetText(3, 4, _variant_t(str));

	str.Format(_T("%.3f"), LoadTable2.focus);
	m_spread_teachingpos.SetText(4, 4, _variant_t(str));

	// Picker Narrow
	str.Format(_T("%.3f"), Picker_Narrow.xP);
	m_spread_pickerpos.SetText(1, 1, _variant_t(str));

	str.Format(_T("%.3f"), Picker_Narrow.yP);
	m_spread_pickerpos.SetText(2, 1, _variant_t(str));

	str.Format(_T("%.3f"), Picker_Narrow.zP);
	m_spread_pickerpos.SetText(3, 1, _variant_t(str));

	// PickerWide
	str.Format(_T("%.3f"), Picker_Wide.xP);
	m_spread_pickerpos.SetText(1, 2, _variant_t(str));

	str.Format(_T("%.3f"), Picker_Wide.yP);
	m_spread_pickerpos.SetText(2, 2, _variant_t(str));

	str.Format(_T("%.3f"), Picker_Wide.zP);
	m_spread_pickerpos.SetText(3, 2, _variant_t(str));

	// PickerCam Distance
	str.Format(_T("%.3f"), CPD_LEFT.narrowX);
	m_spread_pickercamdist.SetText(1, 1, _variant_t(str));

	str.Format(_T("%.3f"), CPD_LEFT.narrowY);
	m_spread_pickercamdist.SetText(2, 1, _variant_t(str));

	str.Format(_T("%.3f"), CPD_RIGHT.narrowX);
	m_spread_pickercamdist.SetText(1, 2, _variant_t(str));

	str.Format(_T("%.3f"), CPD_RIGHT.narrowY);
	m_spread_pickercamdist.SetText(2, 2, _variant_t(str));

	// PlateJig Left
	str.Format(_T("%.3f"), Target_A_Left.xP);
	m_spread_platejig_left.SetText(1, 1, _variant_t(str));

	str.Format(_T("%.3f"), Target_A_Left.yP);
	m_spread_platejig_left.SetText(2, 1, _variant_t(str));

	str.Format(_T("%.3f"), Target_A_Left.zP);
	m_spread_platejig_left.SetText(3, 1, _variant_t(str));

	str.Format(_T("%.3f"), Target_B_Left.xP);
	m_spread_platejig_left.SetText(1, 2, _variant_t(str));

	str.Format(_T("%.3f"), Target_B_Left.yP);
	m_spread_platejig_left.SetText(2, 2, _variant_t(str));

	str.Format(_T("%.3f"), Target_B_Left.zP);
	m_spread_platejig_left.SetText(3, 2, _variant_t(str));

	str.Format(_T("%.3f"), Upper_Target_Left.xP);
	m_spread_platejig_left.SetText(1, 3, _variant_t(str));

	str.Format(_T("%.3f"), Upper_Target_Left.yP);
	m_spread_platejig_left.SetText(2, 3, _variant_t(str));

	str.Format(_T("%.3f"), Upper_Target_Left.zP);
	m_spread_platejig_left.SetText(3, 3, _variant_t(str));

	str.Format(_T("%.3f"), Lower_Target.xP);
	m_spread_platejig_left.SetText(1, 4, _variant_t(str));

	str.Format(_T("%.3f"), Lower_Target.yP);
	m_spread_platejig_left.SetText(2, 4, _variant_t(str));

	str.Format(_T("%.3f"), Lower_Target.zP);
	m_spread_platejig_left.SetText(3, 4, _variant_t(str));


	// PlateJig Right
	str.Format(_T("%.3f"), Target_A_Right.xP);
	m_spread_platejig_right.SetText(1, 1, _variant_t(str));

	str.Format(_T("%.3f"), Target_A_Right.yP);
	m_spread_platejig_right.SetText(2, 1, _variant_t(str));

	str.Format(_T("%.3f"), Target_A_Right.zP);
	m_spread_platejig_right.SetText(3, 1, _variant_t(str));

	str.Format(_T("%.3f"), Target_B_Right.xP);
	m_spread_platejig_right.SetText(1, 2, _variant_t(str));

	str.Format(_T("%.3f"), Target_B_Right.yP);
	m_spread_platejig_right.SetText(2, 2, _variant_t(str));

	str.Format(_T("%.3f"), Target_B_Right.zP);
	m_spread_platejig_right.SetText(3, 2, _variant_t(str));

	str.Format(_T("%.3f"), Upper_Target_Right.xP);
	m_spread_platejig_right.SetText(1, 3, _variant_t(str));

	str.Format(_T("%.3f"), Upper_Target_Right.yP);
	m_spread_platejig_right.SetText(2, 3, _variant_t(str));

	str.Format(_T("%.3f"), Upper_Target_Right.zP);
	m_spread_platejig_right.SetText(3, 3, _variant_t(str));

	str.Format(_T("%.3f"), Lower_Target.xP);
	m_spread_platejig_right.SetText(1, 4, _variant_t(str));

	str.Format(_T("%.3f"), Lower_Target.yP);
	m_spread_platejig_right.SetText(2, 4, _variant_t(str));

	str.Format(_T("%.3f"), Lower_Target.zP);
	m_spread_platejig_right.SetText(3, 4, _variant_t(str));
}

