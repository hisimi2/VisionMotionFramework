// Equipment2015Dlg.h : 헤더 파일
//

#pragma once

#include "Orchestrator.h"
#include "VMFramework/Load1/VatAdapterLoad1.h"

// CEquipment2015Dlg 대화 상자
class CEquipment2015Dlg : public CDialogEx
{
    VMF::CVatEngineObserverAdapter m_engine;
    std::shared_ptr<Load1Parts> m_parts;
    std::shared_ptr<VMF_Load1::VatAdapterLoad1> m_adapter;

    // Observer subscription id (for safe unsubscribe when dialog is destroyed)
    VMF::CVatEngineObserverAdapter::ObserverId m_engineObserverId;

public:
	CEquipment2015Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_EQUIPMENT2015_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:
	HICON m_hIcon;

 void HandleVmfResult(const VMF::VisionResultPayload& payload);

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedVisionSequence();
};
