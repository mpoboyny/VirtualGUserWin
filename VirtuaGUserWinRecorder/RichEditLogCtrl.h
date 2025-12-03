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
    void AppendLine(LPCTSTR pszText);
    // Fügt eine Zeile mit Zeitstempel an
    void AppendTimestampedLine(LPCTSTR pszText);
    // Löscht den Inhalt
    void Clear();
    // Setzt den ReadOnly-Zustand
    void SetReadOnly(bool bReadOnly);

protected:
    virtual void PreSubclassWindow() override;

    afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);

    DECLARE_MESSAGE_MAP()
};