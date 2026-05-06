#pragma once

#include "CVatEngineUiAdapter.h" 
#include "DVH_VAT/Load1/VatAdapterLoad1.h"

#include "spreadsheet.h"
#include <queue>
#include <functional>

typedef std::function<bool()> SequenceFunc;

class CEquipmentSWDlg : public CDialogEx
{
public:
    CEquipmentSWDlg(CWnd* pParent = NULL);
    virtual ~CEquipmentSWDlg();

    enum { IDD = IDD_EQUIPMENTSW_DIALOG };

protected:
    virtual void DoDataExchange(CDataExchange* pDX);

protected:
    HICON m_hIcon;

private:
    afx_msg LRESULT OnDVH_VATResultMsg(WPARAM wParam, LPARAM lParam);

    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    afx_msg void OnDestroy();
    DECLARE_MESSAGE_MAP()

private:
    // 엔진 어댑터를 멤버로 소유 (composition)
    DVH_VAT::CVatEngineUiAdapter m_engine;
	boost::shared_ptr<VAT_LOAD1::Load1Parts> m_parts;
	boost::shared_ptr<VAT_LOAD1::VatAdapterLoad1> m_adapter;

	std::queue<SequenceFunc> m_seqQueue;
	bool m_isSequenceRunning;
public:
	CSpreadSheet m_spread_teachingpos;
	CSpreadSheet m_spread_pickercamdist;
	CSpreadSheet m_spread_pickerpos;
	CSpreadSheet m_spread_platejig_left;
	CSpreadSheet m_spread_platejig_right;

	void UpdateSpread();
	void StartSequenceQueue();

	void RunLoad1PlateJigSequence();
	void RunLoad1AfterPlateJigSequence();
	void RunLoad1TeachingSequence();

	afx_msg void OnBnClickedBtnFocusLeft();
	afx_msg void OnBnClickedBtnFocusRight();
	afx_msg void OnBnClickedBtnFovLeft();
	afx_msg void OnBnClickedBtnFovRight();
	afx_msg void OnBnClickedBtnPlateLeft();
	afx_msg void OnBnClickedBtnPlateRight();
	afx_msg void OnBnClickedBtnPlateLow();
	afx_msg void OnBnClickedBtnFovLow();
	afx_msg void OnBnClickedBtnLow();
	afx_msg void OnBnClickedBtnHandpitch();
	afx_msg void OnBnClickedBtnLeft();
	afx_msg void OnBnClickedBtnRight();
	afx_msg void OnBnClickedBtnPcd();
	afx_msg void OnBnClickedBtnBacklash();
	afx_msg void OnBnClickedBtnPlatejigInsp();
	afx_msg void OnBnClickedBtnAfterPlatejigInsp();
	afx_msg void OnBnClickedBtnTeachingInsp();
	afx_msg void OnBnClickedBtnVatStop();
};

