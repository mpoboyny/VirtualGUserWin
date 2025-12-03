#include "pch.h"
#include "framework.h"
#include "RichEditLogCtrl.h"

#ifndef EM_GETREADONLY
#define EM_GETREADONLY 0x00FF
#endif

IMPLEMENT_DYNAMIC(CRichEditLogCtrl, CRichEditCtrl)

CRichEditLogCtrl::CRichEditLogCtrl()        
{
}

CRichEditLogCtrl::~CRichEditLogCtrl()
{
}

void CRichEditLogCtrl::PreSubclassWindow()
{
    CRichEditCtrl::PreSubclassWindow();

    // Sicherstellen, dass vertikales Scrollen verfügbar ist
    ModifyStyle(0, ES_MULTILINE | ES_AUTOVSCROLL | WS_VSCROLL);
    // Initiale Scrollbar-Anzeige anhand des aktuellen Inhalts setzen
    ShowScrollBar(SB_VERT, FALSE);

    // Standard: schreibgeschützt, damit Benutzer nicht aus Versehen tippt
    SetReadOnly(true);
    // Events, falls später benötigt
    SetEventMask(GetEventMask());
}

void CRichEditLogCtrl::AppendLine(LPCTSTR pszText)
{
    // Wenn Control schreibgeschützt ist, temporär beschreibbar machen
    BOOL bWasReadOnly = (BOOL)SendMessage(EM_GETREADONLY, 0, 0);
    if (bWasReadOnly)
        SendMessage(EM_SETREADONLY, 0, 0);

    // Alte Fokus-Windows merken
    HWND hPrevFocus = ::GetFocus();

    // An das Ende hängen
    int nLen = GetWindowTextLength();
    SetSel(nLen, nLen);
    CString s = pszText;
    s += _T("\r\n");
    ReplaceSel(s);

    // Neues Ende-Char/Pos ermitteln und Caret setzen
    int nNewPos = GetWindowTextLength();
    SetSel(nNewPos, nNewPos);

    // Sichtbare Zeilen berechnen
    CRect rc;
    GetClientRect(&rc);
    int visibleLines = 1;
    {
        // Ermittle Zeilenhöhe aus aktuellem Font
        CDC* pDC = GetDC();
        if (pDC)
        {
            CFont* pFont = GetFont();
            CFont* pOld = pDC->SelectObject(pFont);
            TEXTMETRIC tm;
            pDC->GetTextMetrics(&tm);
            pDC->SelectObject(pOld);
            ReleaseDC(pDC);
            int lineH = max(1, tm.tmHeight + tm.tmExternalLeading);
            visibleLines = max(1, rc.Height() / lineH);
        }
    }

    // Gesamtzeilen im Control
    int totalLines = (int)SendMessage(EM_GETLINECOUNT, 0, 0);

    // Scrollbar nur anzeigen, wenn mehr Zeilen vorhanden sind als sichtbar
    BOOL needScroll = (totalLines > visibleLines);
    ShowScrollBar(SB_VERT, needScroll);

    // Frame/Layout aktualisieren, damit die Scrollbar-Änderung sichtbar wird
    ::SetWindowPos(GetSafeHwnd(), NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);

    HWND hThis = GetSafeHwnd();
    if (hThis != NULL)
    {
        // Fokus kurz setzen damit Scroll-Operationen wirken
        ::SetFocus(hThis);

        // Versuche, zuverlässig ans Ende zu scrollen
        SendMessage(EM_SCROLLCARET, 0, 0);

        // Robust: bestimme erste sichtbare Zeile und die Zielzeile und linienweise scrollen
        int firstVisible = (int)SendMessage(EM_GETFIRSTVISIBLELINE, 0, 0);
        int lastLine = (int)SendMessage(EM_LINEFROMCHAR, (WPARAM)max(0, nNewPos - 1), 0);
        int linesToScroll = lastLine - firstVisible;
        if (linesToScroll > 0)
        {
            SendMessage(EM_LINESCROLL, 0, linesToScroll);
        }

        // Layout-Update erzwingen
        UpdateWindow();

        // vorherigen Fokus wiederherstellen
        if (hPrevFocus != NULL && ::IsWindow(hPrevFocus) && hPrevFocus != hThis)
            ::SetFocus(hPrevFocus);
    }

    // ReadOnly-Zustand wiederherstellen
    if (bWasReadOnly)
        SendMessage(EM_SETREADONLY, 1, 0);
}

void CRichEditLogCtrl::AppendTimestampedLine(LPCTSTR pszText)
{
    time_t t = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &t);
    TCHAR timeBuf[64];
    _tcsftime(timeBuf, _countof(timeBuf), _T("%Y-%m-%d %H:%M:%S - "), &localTime);
    CString s = timeBuf;
    s += pszText;
    AppendLine(s);
}

void CRichEditLogCtrl::Clear()
{
    // Inhalte löschen
    SetSel(0, -1);
    ReplaceSel(_T(""));

    // Scrollbar verbergen (keine Zeilen mehr)
    ShowScrollBar(SB_VERT, FALSE);
    ::SetWindowPos(GetSafeHwnd(), NULL, 0, 0, 0, 0,
        SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    UpdateWindow();
}

void CRichEditLogCtrl::SetReadOnly(bool bReadOnly)
{
    SendMessage(EM_SETREADONLY, bReadOnly ? 1 : 0, 0);
}

BEGIN_MESSAGE_MAP(CRichEditLogCtrl, CRichEditCtrl)
    ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void CRichEditLogCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    CMenu menu;
    menu.CreatePopupMenu();
    menu.AppendMenu(MF_STRING, 1, _T("Alle löschen"));
    menu.AppendMenu(MF_STRING, 2, _T("Kopieren"));
    int cmd = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    if (cmd == 1) Clear();
    else if (cmd == 2) Copy();
}