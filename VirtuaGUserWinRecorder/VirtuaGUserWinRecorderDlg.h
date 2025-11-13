
// VirtuaGUserWinRecorderDlg.h: Headerdatei
//

#pragma once


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

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
