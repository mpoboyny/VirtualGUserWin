#pragma once

class CRichEditLogCtrl : public CRichEditCtrl
{
    DECLARE_DYNAMIC(CRichEditLogCtrl)
public:
    enum LogType
    {
        LogType_Info,
        LogType_Warning,
        LogType_Error
    };
    CRichEditLogCtrl();
    virtual ~CRichEditLogCtrl();

    // Fügt eine Zeile ohne Timestamp an
    void AppendLine(LPCTSTR pszText, LogType type = LogType_Info);
    // Fügt eine Zeile mit Zeitstempel an
    void AppendTimestampedLine(LPCTSTR pszText, LogType type = LogType_Info);
    // Löscht den Inhalt
    void Clear();
    // Setzt den ReadOnly-Zustand
    void SetReadOnly(bool bReadOnly);

protected:
    virtual void PreSubclassWindow() override;

    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

    DECLARE_MESSAGE_MAP()
};