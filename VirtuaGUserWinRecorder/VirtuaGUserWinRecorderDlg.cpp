// VirtuaGUserWinRecorderDlg.cpp: Implementierungsdatei
//

#include "pch.h"
#include "framework.h"
#include "VirtuaGUserWinRecorder.h"
#include "VirtuaGUserWinRecorderDlg.h"
#include "afxdialogex.h"
#include "Resource.h"
#include <shellapi.h> // Für DragQueryFile, DragFinish

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg-dialogfeld für Anwendungsbefehl "Info"

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialogfelddaten
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung

// Implementierung
protected:
	BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
public:
	CString m_ExeName;
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
, m_ExeName(_T(""))
{
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	this->SetWindowText(L"Information about " TARGETNAMESTR);
	UpdateData(TRUE);
	m_ExeName = TARGETNAMESTR;
	UpdateData(FALSE);
	return TRUE;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_ABOUT, m_ExeName);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMyEditBrowseCtrl

void CMyEditBrowseCtrl::InitTooltip()
{
	// Erstelle Tooltip falls noch nicht vorhanden, binde das gesamte Control
	if (!m_tooltip.m_hWnd)
	{
		// Create an empty tooltip control as a child of this control
		if (m_tooltip.Create(this))
		{
			// Add the tool for the whole control; verwendet m_strTitle als Text
			m_tooltip.AddTool(this, L"Drag and drop folder path hier");
			m_tooltip.Activate(TRUE);
		}
	}
}

void CMyEditBrowseCtrl::PreSubclassWindow()
{
	CMFCEditBrowseCtrl::PreSubclassWindow();
	// Control ist jetzt gebunden/subclassed => Tooltip initialisieren
	InitTooltip();

	// Drag & Drop für dieses Control aktivieren (Dateien/Ordner per Drag & Drop)
	::DragAcceptFiles(GetSafeHwnd(), TRUE);
}

inline BOOL CMyEditBrowseCtrl::PreTranslateMessage(MSG* pMsg)
{
	// Leite Nachrichten an das Tooltip weiter (z.B. WM_MOUSEMOVE)
	if (m_tooltip.m_hWnd)
	{
		m_tooltip.RelayEvent(pMsg);
	}
	return CMFCEditBrowseCtrl::PreTranslateMessage(pMsg);
}

void CMyEditBrowseCtrl::OnDrawBrowseButton(CDC* pDC, CRect rect, BOOL bIsButtonPressed, BOOL bIsHighlighted)
{
	// Ensure the image list is valid
	if (!m_ImageBrowse.GetSafeHandle())
	{
		return;
	}

	const int iconSize = 16;
	const int x = rect.left + (rect.Width() - iconSize) / 2;
	const int y = rect.top + (rect.Height() - iconSize) / 2;

	UINT drawStyle = ILD_TRANSPARENT;

	// Check if the control is disabled
	if (!IsWindowEnabled()) {
		drawStyle = ILD_BLEND50;  // Draw with 50% blend for a "disabled" effect
	}
	m_ImageBrowse.Draw(pDC, 0, CPoint(x, y), drawStyle);
}

void CMyEditBrowseCtrl::OnBrowse()
{
	// --- Step 1: Prepare the BROWSEINFO structure ---

	BROWSEINFO bi = { 0 };
	TCHAR szPath[MAX_PATH];
	
	// Set the flags for the dialog:
	// BIF_EDITBOX: Ensures an edit box is present for typing the path.
	// BIF_USENEWUI: Preferable, uses a resizable dialog with better features.
	// BIF_RETURNONLYFSDIRS: Restricts selection to file system directories.
	// BIF_NEWDIALOGSTYLE: Recommended for Win2K/XP and later.
	bi.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

	// Set the owner window and title
	bi.hwndOwner = GetSafeHwnd();
	bi.lpszTitle = m_strTitle.GetString();

	// Setting lpfn to NULL effectively disables the callback, as requested.
	bi.lpfn = NULL;

	// --- Step 2: Call the Win32 function ---
	LPITEMIDLIST pidl = SHBrowseForFolder(&bi);

	// --- Step 3: Process the result ---
	if (pidl != NULL)
	{
		// Convert the ITEMIDLIST to a file system path (using TCHAR types)
		if (SHGetPathFromIDList(pidl, szPath))
		{
			// Update the edit control with the selected path
			SetWindowText(szPath);
		}
		// Free the PIDL memory
		CoTaskMemFree(pidl);
	}

	// The base class needs to be notified of the update if needed (optional)
	// OnAfterUpdate(); 
}

// Message-Map für CMyEditBrowseCtrl
BEGIN_MESSAGE_MAP(CMyEditBrowseCtrl, CMFCEditBrowseCtrl)
	ON_WM_DROPFILES()
END_MESSAGE_MAP()

// Handler für Drag & Drop
void CMyEditBrowseCtrl::OnDropFiles(HDROP hDropInfo)
{
	if (hDropInfo == NULL)
	{
		CMFCEditBrowseCtrl::OnDropFiles(hDropInfo);
		return;
	}

	// Anzahl der gedroppten Elemente ermitteln
	UINT uCount = ::DragQueryFile(hDropInfo, 0xFFFFFFFF, NULL, 0);
	if (uCount == 0)
	{
		::DragFinish(hDropInfo);
		CMFCEditBrowseCtrl::OnDropFiles(hDropInfo);
		return;
	}

	// Wir verarbeiten nur das erste Element
	TCHAR szPath[MAX_PATH] = { 0 };
	if (::DragQueryFile(hDropInfo, 0, szPath, sizeof(szPath)/sizeof(szPath[0])))
	{
		// Prüfen, ob es ein Verzeichnis ist
		DWORD dwAttr = ::GetFileAttributes(szPath);
		if (dwAttr != INVALID_FILE_ATTRIBUTES && (dwAttr & FILE_ATTRIBUTE_DIRECTORY))
		{
			// Direktes Verzeichnis übernehmen
			SetWindowText(szPath);
		}
		else
		{
			// Ist eine Datei: Parent-Ordner extrahieren und übernehmen
			CString strPath(szPath);
			int nPos = strPath.ReverseFind('\\');
			if (nPos != -1)
			{
				strPath = strPath.Left(nPos);
				SetWindowText(strPath);
			}
			else
			{
				// Fallback: ganze Zeichenkette übernehmen
				SetWindowText(szPath);
			}
		}
	}

	::DragFinish(hDropInfo);

	// Basisverhalten aufrufen
	CMFCEditBrowseCtrl::OnDropFiles(hDropInfo);
}

// CVirtuaGUserWinRecorderDlg

CVirtuaGUserWinRecorderDlg::CVirtuaGUserWinRecorderDlg(CWnd* pParent /*=nullptr*/)
: CDialogEx(IDD_VIRTUAGUSERWINRECORDER_DIALOG, pParent)
, m_wndFolderEdit(L"Select work folder")
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVirtuaGUserWinRecorderDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFC_EDIT_BROWSE_WORK_FOLDER, m_wndFolderEdit);
	DDX_Control(pDX, IDC_CHK_CLEAN_WORK_FOLDER, m_chkCleanWrkFolder);
	DDX_Control(pDX, IDC_STC_OVERWRITE, m_stcOverwrite);
	DDX_Control(pDX, IDC_ED_WORK_NAME, m_edWorkName);
	DDX_Control(pDX, IDC_RICHEDIT_LOG, m_wndLog);
}

BEGIN_MESSAGE_MAP(CVirtuaGUserWinRecorderDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_CHK_CLEAN_WORK_FOLDER, &CVirtuaGUserWinRecorderDlg::OnBnClickedChkCleanWorkFolder)
END_MESSAGE_MAP()


// CVirtuaGUserWinRecorderDlg-Meldungshandler

BOOL CVirtuaGUserWinRecorderDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Hinzufügen des Menübefehls "Info..." zum Systemmenü.

	// IDM_ABOUTBOX muss sich im Bereich der Systembefehle befinden.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
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

	// Symbol für dieses Dialogfeld festlegen.  Wird automatisch erledigt
	//  wenn das Hauptfenster der Anwendung kein Dialogfeld ist
	SetIcon(m_hIcon, TRUE);			// Großes Symbol verwenden
	SetIcon(m_hIcon, FALSE);		// Kleines Symbol verwenden

	// TODO: Hier zusätzliche Initialisierung einfügen
	this->SetWindowText(TARGETNAMESTR);

	m_wndFolderEdit.EnableFolderBrowseButton();
    CEdit *FolderEdit = static_cast<CEdit*>(&m_wndFolderEdit);
    HWND hWndFolderEdit = FolderEdit->GetSafeHwnd();
	::DragAcceptFiles(hWndFolderEdit, TRUE);

    m_chkCleanWrkFolder.SetCheck(BST_CHECKED);

	m_stcOverwrite.ShowWindow(SW_HIDE);

	m_edWorkName.SetWindowTextW(L"VGU_REC");

	return TRUE;  // TRUE zurückgeben, wenn der Fokus nicht auf ein Steuerelement gesetzt wird
}

void CVirtuaGUserWinRecorderDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

BOOL CVirtuaGUserWinRecorderDlg::PreTranslateMessage(MSG* pMsg)
{
	// Check if the message is a key down event
	if (pMsg->message == WM_KEYDOWN)
	{
		// Check if the key pressed is the ENTER key (VK_RETURN)
		if (pMsg->wParam == VK_RETURN)
		{
			// Do NOT process the message, effectively stopping the Enter key 
			// from triggering the default OnOK/dialog closure behavior.
			return TRUE;
		}

		// Optional: Also check the ESCAPE key (VK_ESCAPE) 
		// as a secondary way to block the closure.
		if (pMsg->wParam == VK_ESCAPE)
		{
			return TRUE;
		}
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}

void CVirtuaGUserWinRecorderDlg::OnCancel()
{
    CDialogEx::OnCancel();
}

void CVirtuaGUserWinRecorderDlg::OnOK()
{
	m_wndLog.AppendTimestampedLine(L"I do it!");
}

HBRUSH CVirtuaGUserWinRecorderDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	// Rufe Standardverhalten zuerst ab
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// Sicherstellen, dass das Fenster gültig ist und mit unserem Control übereinstimmt
	if (pWnd != nullptr && pWnd->GetSafeHwnd() == m_stcOverwrite.GetSafeHwnd())
	{
		// Textfarbe auf Rot setzen
		pDC->SetTextColor(RGB(255, 0, 0));
		// Hintergrund transparent zeichnen, damit Dialoghintergrund durchscheint
		pDC->SetBkMode(TRANSPARENT);

		// Rückgabe eines Null-Pinsel für transparenten Hintergrund
		return (HBRUSH)::GetStockObject(NULL_BRUSH);
	}

	// Für alle anderen Controls das Standardverhalten beibehalten
	return hbr;
}

void CVirtuaGUserWinRecorderDlg::OnBnClickedChkCleanWorkFolder()
{
	m_stcOverwrite.ShowWindow(m_chkCleanWrkFolder.GetCheck()? SW_HIDE: SW_SHOW);
}
