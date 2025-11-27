// VirtuaGUserWinRecorderDlg.h: Headerdatei
//

#pragma once

// Pseudocode / Plan (detailliert):
// 1. Erweiterung von CMyEditBrowseCtrl um ein CToolTipCtrl-Feld `m_tooltip`.
// 2. Initialisiere das Tooltip in `PreSubclassWindow`, weil das Control dann Subclassed/erzeugt ist.
// 3. Implementiere `InitTooltip()` zur Erstellung, Hinzufügung des Tools (this) und Aktivierung.
// 4. Überschreibe `PreTranslateMessage` und leite Events an `m_tooltip.RelayEvent` weiter, damit das Tooltip korrekt angezeigt wird.
// 5. Belasse vorhandene `OnBrowse()`-Signatur; Tooltip verwendet den vorhandenen `m_strTitle` als Text.
// 6. Implementiere die kleinen Helfer inline im Header (keine zusätzliche CPP-Datei nötig), kompakt und sicher für C++14.
// 7. Keine Änderungen am Dialog-Owner/Initialisierung hier — der Dialog sollte das Control weiterhin wie bisher im Konstruktor initialisieren.

// Zusätzlicher Planpunkt (aktuelle Änderung):
// - Verhindere Schließen auf IDCANCEL/ESC für `CVirtuaGUserWinRecorderDlg` durch Überschreiben von `OnCancel()`.
// - Implementierung inline: leer lassen (kein Aufruf von Basisklasse).

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

	virtual BOOL PreTranslateMessage(MSG* pMsg);

	// Generierte Funktionen für die Meldungstabellen
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	virtual void OnCancel() override;
    virtual void OnOK() override;

	DECLARE_MESSAGE_MAP();
public:
	afx_msg void OnBnClickedChkCleanWorkFolder();
};
