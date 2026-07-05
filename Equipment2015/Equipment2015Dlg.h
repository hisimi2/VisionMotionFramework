// Equipment2015Dlg.h : 헤더 파일
//

#pragma once

#include "OperationThreads/ThreadsManager.h"
#include "Actuators/COPSwitch.h"

#include <memory> // std::shared_ptr
#include <vector> // std::vector

namespace VMF { class Orchestrator; }

// 사용자 정의 메시지 ID — Observer에서 PostMessage로 전달할 로그 메시지
#define WM_ACTIVITY_RESULT (WM_USER + 100)

// CEquipment2015Dlg 대화 상자
class CEquipment2015Dlg : public CDialogEx
{
    COPSwitch m_StartSwitch;
    COPSwitch m_StopSwitch;

    void InitOrchestratorExample();
    void RunOrchestratorSequenceExample();
    void StopOrchestratorSequenceExample();
    void AccessSequenceDataExample();

public:
	CEquipment2015Dlg(CWnd* pParent = NULL);	// 표준 생성자입니다.

    enum { IDD = IDD_EQUIPMENT2015_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.

protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT OnActivityResult(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
    OperationThread::ThreadsManager m_threadsMgr;
    std::shared_ptr<VMF::Orchestrator> m_orchestrator;
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	CMFCButton m_BtnStart;
	CMFCButton m_BtnStop;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CEdit m_LogEdit;
	afx_msg void OnBnClickedVmfStateMachine();
	afx_msg void OnBnClickedVmfDirect();
    afx_msg void OnBnClickedVmfStateMachineWithConnectionManager();
    afx_msg void OnBnClickedVmfMultiServerExample();

    void AppendLog(LPCTSTR msg);
    void AppendLogFormat(LPCTSTR fmt, ...);

    // Observer 헬퍼 — Orchestrator에 Observer 등록 (PostMessage로 로그 전달)
    void RegisterOrchestratorObserver(
        std::shared_ptr<VMF::Orchestrator> orchestrator,
        LPCTSTR activityName);

    std::vector<std::shared_ptr<VMF::Orchestrator>> m_multiServerOrchestrators;
};
