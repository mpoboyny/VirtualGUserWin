// VirtuaGUserWinRecorderDlg.h: Headerdatei
//

#pragma once

#include "RichEditLogCtrl.h"

// MyEditBrowseCtrl
class CMyEditBrowseCtrl : public CMFCEditBrowseCtrl
{
public:
		CMyEditBrowseCtrl(const CString title) 
		: m_strTitle(title){}

        virtual ~CMyEditBrowseCtrl() {}

protected:
	CString m_strTitle;
	virtual void OnBrowse();

	// Tooltip-Unterstützung
	CToolTipCtrl m_tooltip;

	// Initialisierung des Tooltips, Aufruf in PreSubclassWindow
	void InitTooltip();

	// Relay von Nachrichten an Tooltip
	virtual BOOL PreTranslateMessage(MSG* pMsg) override;
	virtual void PreSubclassWindow() override;
	virtual void OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsHighlighted) override;

	afx_msg void OnDropFiles(HDROP hDropInfo);
	DECLARE_MESSAGE_MAP()

};

// CVirtuaGUserWinRecorderDlg-Dialogfeld
class CVirtuaGUserWinRecorderDlg : public CDialogEx
{
// Konstruktion
public:
	CVirtuaGUserWinRecorderDlg(CWnd* pParent = nullptr);	// Standardkonstruktor

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_VIRTUAGUSERWINRECORDER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV-Unterstützung


// Implementierung
protected:
	HICON m_hIcon;
	CMyEditBrowseCtrl m_wndFolderEdit;
	CButton m_chkCleanWrkFolder;
	CStatic m_stcOverwrite;
	CEdit m_edWorkName;
	CRichEditLogCtrl m_wndLog;

protected:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedChkCleanWorkFolder();
	virtual void OnCancel() override;
    virtual void OnOK() override;

	DECLARE_MESSAGE_MAP();
	
};
