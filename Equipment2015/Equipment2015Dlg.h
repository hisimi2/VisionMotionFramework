// Equipment2015Dlg.h : 헤더 파일
//

#pragma once

#include "OperationThreads/ThreadsManager.h"
#include "Actuators/COPSwitch.h"

// PostMessage로 전달할 Activity 결과 데이터 구조체
struct ActivityResultData
{
    CString activityName;
    int     requestId = 0;
    CString detail;
};

// 사용자 정의 메시지 ID
#define WM_ACTIVITY_RESULT (WM_USER + 100)

// CEquipment2015Dlg 대화 상자
class CEquipment2015Dlg : public CDialogEx
{
    COPSwitch m_StartSwitch;
    COPSwitch m_StopSwitch;

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

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
    afx_msg LRESULT OnActivityResult(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
    // === ThreadsManager 제어 버튼 핸들러 ===


private:
    OperationThread::ThreadsManager m_threadsMgr;
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
	CMFCButton m_BtnStart;
	CMFCButton m_BtnStop;
    afx_msg void OnTimer(UINT_PTR nIDEvent);
    CEdit m_LogEdit;
	afx_msg void OnBnClickedVmfStateMachine();
};
