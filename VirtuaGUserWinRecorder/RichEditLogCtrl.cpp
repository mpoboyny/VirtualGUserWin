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

void CRichEditLogCtrl::AppendLine(LPCTSTR pszText, LogType type)
{
    // Temporär beschreibbar machen, falls ReadOnly gesetzt ist
    BOOL bWasReadOnly = (BOOL)SendMessage(EM_GETREADONLY, 0, 0);
    if (bWasReadOnly)
        SendMessage(EM_SETREADONLY, 0, 0);

    HWND hPrevFocus = ::GetFocus();

    // Bestimme Prefix und Ziel-Farbe
    CString prefix;
    CHARFORMAT2 cfNew = {};
    cfNew.cbSize = sizeof(cfNew);
    cfNew.dwMask = CFM_COLOR;
    switch (type)
    {
    case LogType_Warning:
        prefix = _T("[WARNING]:   ");
        cfNew.crTextColor = RGB(140, 80, 70); // dunkelrot / angepasst
        break;
    case LogType_Error:
        prefix = _T("[ERROR]     :   ");
        cfNew.crTextColor = RGB(255, 0, 0); // rot
        break;
    case LogType_Info:
    default:
        prefix = _T("[INFO]        :   ");
        cfNew.crTextColor = RGB(0, 0, 0); // schwarz
        break;
    }

    // Position am Ende setzen
    int nStart = GetWindowTextLength();
    SetSel(nStart, nStart);

    // Aktuelles Einfügeformat sichern
    CHARFORMAT2 cfOld = {};
    cfOld.cbSize = sizeof(cfOld);
    SendMessage(EM_GETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfOld);

    // Neues Format setzen (für die Einfügung)
    SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfNew);

    // Text einfügen (wird mit aktuellem Selektionsformat eingefügt)
    CString s = prefix;
    s += pszText ? pszText : _T("");
    s += _T("\r\n");
    ReplaceSel(s);

    // Caret ans Ende setzen
    int nEnd = nStart + s.GetLength();
    SetSel(nEnd, nEnd);

    // Falls die Einfügung das Format nicht vollständig übernehmen sollte,
    // selektieren wir den Bereich nochmal und stellen das Format sicher.
    SetSel(nStart, nEnd);
    SetSelectionCharFormat(cfNew);

    // Selection wieder auf Caret (keine Auswahl)
    SetSel(nEnd, nEnd);

    // Layout/Zeilenberechnung aktualisieren
    UpdateWindow();
    RedrawWindow(nullptr, nullptr, RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);

    HWND hThis = GetSafeHwnd();
    if (hThis != NULL)
    {
        ::SetFocus(hThis);
        SendMessage(EM_SCROLLCARET, 0, 0);
        SendMessage(WM_VSCROLL, SB_BOTTOM, 0);

        // Scrollbar nur anzeigen, wenn nötig
        SCROLLINFO si = {};
        si.cbSize = sizeof(si);
        si.fMask = SIF_PAGE | SIF_RANGE | SIF_POS;
        if (::GetScrollInfo(hThis, SB_VERT, &si))
            ShowScrollBar(SB_VERT, (si.nMax > (int)si.nPage));
        else
            ShowScrollBar(SB_VERT, TRUE);

        UpdateWindow();

        if (hPrevFocus != NULL && ::IsWindow(hPrevFocus) && hPrevFocus != hThis)
            ::SetFocus(hPrevFocus);
    }

    // Vorheriges Einfügeformat wiederherstellen,
    // damit nachfolgende Einfügungen nicht die Farbe "kleben" lassen.
    SendMessage(EM_SETCHARFORMAT, SCF_SELECTION, (LPARAM)&cfOld);

    // ReadOnly-Zustand wiederherstellen
    if (bWasReadOnly)
        SendMessage(EM_SETREADONLY, 1, 0);
}

void CRichEditLogCtrl::AppendTimestampedLine(LPCTSTR pszText, LogType type)
{
    time_t t = time(nullptr);
    tm localTime;
    localtime_s(&localTime, &t);
    TCHAR timeBuf[64];
    _tcsftime(timeBuf, _countof(timeBuf), _T("%Y-%m-%d %H:%M:%S - "), &localTime);
    CString s = timeBuf;
    s += pszText ? pszText : _T("");
    AppendLine(s, type);
}

void CRichEditLogCtrl::Clear()
{
    // Inhalte löschen
    SetSel(0, -1);
    ReplaceSel(_T(""));

    // Layout aktualisieren und Scrollbar verbergen
    UpdateWindow();
    RedrawWindow(nullptr, nullptr, RDW_UPDATENOW | RDW_ERASE | RDW_FRAME);
    ShowScrollBar(SB_VERT, FALSE);
}

void CRichEditLogCtrl::SetReadOnly(bool bReadOnly)
{
    SendMessage(EM_SETREADONLY, bReadOnly ? 1 : 0, 0);
}

BEGIN_MESSAGE_MAP(CRichEditLogCtrl, CRichEditCtrl)
    ON_WM_CONTEXTMENU()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

BOOL CRichEditLogCtrl::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
    // Immer Standard-Pfeil setzen
    ::SetCursor(::LoadCursor(NULL, IDC_ARROW));
    return TRUE; // Nachricht behandelt
}

void CRichEditLogCtrl::OnContextMenu(CWnd* /*pWnd*/, CPoint point)
{
    CMenu menu;
    if (!menu.CreatePopupMenu())
        return;

    const UINT ID_DELETE_ALL = 1;
    const UINT ID_COPY_ALL   = 2;

    menu.AppendMenu(MF_STRING, ID_DELETE_ALL, _T("Delete all"));
    menu.AppendMenu(MF_STRING, ID_COPY_ALL,   _T("Copy all"));

    // Disable both items when there is no text
    int len = GetWindowTextLengthW();
    UINT state = (len > 0) ? MF_ENABLED : (MF_GRAYED | MF_DISABLED);
    menu.EnableMenuItem(ID_DELETE_ALL, MF_BYCOMMAND | state);
    menu.EnableMenuItem(ID_COPY_ALL,   MF_BYCOMMAND | state);

    int cmd = menu.TrackPopupMenu(TPM_RETURNCMD | TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
    if (cmd == ID_DELETE_ALL && len > 0)
    {
        Clear();
    }
    else if (cmd == ID_COPY_ALL && len > 0)
    {
        CString text;
        GetWindowText(text);

        if (!::OpenClipboard(GetSafeHwnd()))
            return;

        ::EmptyClipboard();

        SIZE_T bytes = (text.GetLength() + 1) * sizeof(wchar_t);
        HGLOBAL hMem = ::GlobalAlloc(GMEM_MOVEABLE, bytes);
        if (hMem)
        {
            void* pMem = ::GlobalLock(hMem);
            if (pMem)
            {
                memcpy(pMem, (LPCWSTR)text.GetString(), bytes);
                ::GlobalUnlock(hMem);

                if (::SetClipboardData(CF_UNICODETEXT, hMem) == NULL)
                {
                    ::GlobalFree(hMem);
                }
                // bei Erfolg übernimmt die Zwischenablage das HGLOBAL
            }
            else
            {
                ::GlobalFree(hMem);
            }
        }

        ::CloseClipboard();
    }
}