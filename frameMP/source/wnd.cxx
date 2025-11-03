// 
// This file is part of FrameMP project.
// Copyright ©2017 Mykhaylo Poboynyy <mpoboyny@googlemail.com>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; version 2.1 (LGPL-2.1) or later.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

// wnd.cxx

#include <prc.hxx>
#include <file.hxx>

namespace frameMP
{

CWndBase::DispList CWndBase::m_dispList;
unsigned CWndBase::s_InstCount = 0;
CWndBase::XKeyMap CWndBase::s_KeyNameToKey;

CWndBase CWndBase::s_staticInit;

CWndBase::CWndBase() : m_hWnd(NULL), m_hWndPar(NULL), m_iCompareCutPx(0)
{
    memset(&m_winInfo, 0, sizeof(SWinInfo));
    if (s_InstCount == 1)
    {
        TrVoid(!Init());
    }
}

CWndBase::CWndBase(const SWinInfo &info)
            : m_hWnd(info.m_hWnd), m_hWndPar(info.m_hPar), m_winInfo(info), m_iCompareCutPx(0)
{
    if (s_InstCount == 1)
    {
        TrVoid(!Init());
    }
}

CWndBase::~CWndBase()
{
    if (s_InstCount == 1)
    {
        Clean();
    }
}

HWND CWndBase::GetHwnd() const
{
    return m_hWnd;
}

HWND CWndBase::GetParHwnd()
{
    return m_hWndPar;
}

BOOL CWndBase::OnWndMsg(UINT &uMsg, WPARAM &wParam, LPARAM &lParam)
{
    return FALSE;
}


WinInfoList CWndBase::FindWindowByTitle(const TCHAR *title)
{
    return FindWindowByTitleText(title, eWithTitleExakt);
}

WinInfoList CWndBase::FindWindowByTextInTitle(const TCHAR *title)
{
    return FindWindowByTitleText(title, eWithTitleWithText);
}

BOOL CWndBase::SaveScreenShot(LPCTSTR filePath)
{
    TrFu;
	return SaveScreenShotEx(filePath, SRect());
}

BOOL CWndBase::SaveScreenShotEx(LPCTSTR filePath, const SRect& rc)
{
	TrFu;
	TrVar(filePath);
	BOOL back = FALSE;
	void* pData = NULL;
	size_t dataSize = 0, byteSaved = 0;
	int iFd = 0;
	back = GetScreenShot(pData, dataSize, CutWndRect(rc));
	if (back) {
		back = iFd = SFile::Open(filePath, MP_O_BINARY | MP_O_CREATE | MP_O_RDWR | MP_O_TRUNC, MP_S_WRITE);
	}
	if (iFd && dataSize) {
		byteSaved = SFile::Write(iFd, pData, dataSize);
		back = dataSize == byteSaved;
	}
	free(pData);
	SFile::Close(iFd);
	TrRet(!back, FALSE);
	return back;
}

int CWndBase::SetCompareCutPx(int val)
{
	TrFu;
	int back = m_iCompareCutPx;
	m_iCompareCutPx = val;
	TrVar(back);
	TrVar(m_iCompareCutPx);
	return back;
}

double CWndBase::CmpScreenShot(LPCTSTR filePath, BOOL soft)
{
    TrFu;
	return CmpScreenShotEx(filePath, SRect(), soft);
}

// is in wndWin.inl
extern double CompareBmpData(void* dataBmp1, size_t dataBmp1Size, void* dataBmp2, size_t dataBmp2Size);

double CWndBase::CmpScreenShotEx(LPCTSTR filePath, const SRect& rc, BOOL soft)
{
	TrFu;
	TrVar(filePath);
	TrVar(soft)
	CPath path(filePath);
	TrRet(!path.IsFile(), 0.);
	double percentEqual = 0.;
	TrVar(m_iCompareCutPx);
	int iFdRef = 0;
	size_t dataSize = 0, refFsize = 0, refBytesRead = 0;
	void* pData = NULL, * pRefBytes = NULL;
	iFdRef = SFile::Open(filePath);
	TrRet(!iFdRef, 0.);
	refFsize = SFile::GetFileSize(iFdRef);
	if (refFsize && GetScreenShot(pData, dataSize, CutWndRect(rc, m_iCompareCutPx)) && frameMP::ReAlloc(&pRefBytes, 0, refFsize)
		&& 0 < (refBytesRead = SFile::Read(iFdRef, pRefBytes, refFsize))) {
		if (soft) {
			percentEqual = CompareBmpData(pData, dataSize, pRefBytes, refBytesRead);
		}
		else {
			if (frameMP::MemCmp(pData, dataSize, pRefBytes, refFsize, percentEqual)) {
				percentEqual = 100.0;
			}
		}
		if (percentEqual < 100.0)
		{
			percentEqual *= pow(10., 5);
			percentEqual = ceil(percentEqual);
			percentEqual /= pow(10., 5);
		}
	}
	else {
		Tr("Check failed!");
	}
	TrVar(percentEqual);
	::free(pData);
	::free(pRefBytes);
	SFile::Close(iFdRef);
	//TrVar(percentEqual);
	return percentEqual;
}

static inline BOOL IsAppWindow(HWND hWnd)
{
    if (!IsWindowVisible(hWnd)) return FALSE;
    if (GetWindow(hWnd, GW_OWNER))return FALSE;
    LONG styleEx = GetWindowLong(hWnd, GWL_EXSTYLE);
    if (styleEx & WS_EX_APPWINDOW) return TRUE;
    if (styleEx & WS_EX_TOOLWINDOW) return FALSE;
    if (GetWindowLongPtr(hWnd, GWLP_USERDATA) == 0x49474541) return FALSE;
    return TRUE;
}

static inline void SetCursorPosTo(const HWND& hWnd, const SPoint& pt)
{
    POINT p = { pt.m_x, pt.m_y };
    ::ClientToScreen(hWnd, &p);
    ::SetCursorPos(p.x, p.y);
}

void CWndBase::InitKeyNameToKey()
{
    s_KeyNameToKey[_T("Shift")] = VK_SHIFT;
    s_KeyNameToKey[_T("Ctrl")] = VK_CONTROL;
    s_KeyNameToKey[_T("Alt")] = VK_MENU;
    s_KeyNameToKey[_T("LeftShift")] = VK_LSHIFT;
    s_KeyNameToKey[_T("RightShift")] = VK_RSHIFT;
    s_KeyNameToKey[_T("LeftCtrl")] = VK_LCONTROL;
    s_KeyNameToKey[_T("RightCtrl")] = VK_RCONTROL;
    s_KeyNameToKey[_T("LeftAlt")] = VK_LMENU;
    s_KeyNameToKey[_T("RightAlt")] = VK_RMENU;
    s_KeyNameToKey[_T("NumLock")] = VK_NUMLOCK;
    s_KeyNameToKey[_T("ScrolLock")] = VK_SCROLL;
    s_KeyNameToKey[_T("Tab")] = VK_TAB;
    s_KeyNameToKey[_T("Back")] = VK_BACK;
    s_KeyNameToKey[_T("Clear")] = VK_CLEAR;
    s_KeyNameToKey[_T("Return")] = VK_RETURN;
    s_KeyNameToKey[_T("Space")] = VK_SPACE;
    s_KeyNameToKey[_T("Pause")] = VK_PAUSE;
    s_KeyNameToKey[_T("Escape")] = VK_ESCAPE;
    s_KeyNameToKey[_T("PageUp")] = VK_PRIOR;
    s_KeyNameToKey[_T("PageDown")] = VK_NEXT;
    s_KeyNameToKey[_T("Home")] = VK_HOME;
    s_KeyNameToKey[_T("End")] = VK_END;
    s_KeyNameToKey[_T("KeyUp")] = VK_UP;
    s_KeyNameToKey[_T("KeyDown")] = VK_DOWN;
    s_KeyNameToKey[_T("KeyLeft")] = VK_LEFT;
    s_KeyNameToKey[_T("KeyRight")] = VK_RIGHT;
    s_KeyNameToKey[_T("Print")] = VK_PRINT;
    s_KeyNameToKey[_T("PrintScreen")] = VK_SNAPSHOT;
    s_KeyNameToKey[_T("CapsLock")] = VK_CAPITAL;
    s_KeyNameToKey[_T("CtrlBreak")] = VK_CANCEL;
    s_KeyNameToKey[_T("Insert")] = VK_INSERT;
    s_KeyNameToKey[_T("Delete")] = VK_DELETE;
    s_KeyNameToKey[_T("LeftWin")] = VK_LWIN;
    s_KeyNameToKey[_T("RightWin")] = VK_RWIN;
    s_KeyNameToKey[_T("Plus")] = VK_ADD;
    s_KeyNameToKey[_T("Minus")] = VK_SUBTRACT;
    TCHAR buf[3] = { 0 };
    buf[0] = _T('F');
    for (int i = 1; i < 13; ++i) {
        buf[1] = _T('0') + i;
        s_KeyNameToKey[buf] = 0x6F + i;
    }
}

BOOL CWndBase::Init()
{
    InitKeyNameToKey();
    return TRUE;
}

void CWndBase::Clean()
{
}

LRESULT WINAPI CWndBase::WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    CWndBase* mWnd = (CWndBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (mWnd && (mWnd->PumpMsg(uMsg, wParam, lParam) || mWnd->OnWndMsg(uMsg, wParam, lParam))) {
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

unsigned CWndBase::GetText(Str& back)
{
    unsigned cc = (unsigned)SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0);
    TrRet(cc < 1, 0);
    TCHAR* txt = (TCHAR*)calloc(cc + 16, sizeof(TCHAR));
    cc = (unsigned)SendMessage(m_hWnd, WM_GETTEXT, cc + 16, (LPARAM)txt);
    back = txt;
    free(txt);
    return cc;
}

BOOL CWndBase::GetTitle(Str& back)
{
    return IsTopWindow(m_winInfo) && GetText(back) > 0;
}

BOOL CWndBase::SetText(LPCTSTR txt)
{
    return !!SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM)txt);
}

BOOL CWndBase::TypeText(LPCTSTR txt)
{
    unsigned size = (unsigned)_tcslen(txt);
    TrRet(!size, FALSE);
    INPUT* inputs = (INPUT*)calloc(size * 2, sizeof(INPUT));
    unsigned keyCount = 0;
    for (unsigned i = 0; i < size * 2; i += 2) {
        inputs[i].type = INPUT_KEYBOARD;
        inputs[i].ki.wVk = 0;
        inputs[i].ki.wScan = txt[keyCount];
        inputs[i].ki.dwFlags = KEYEVENTF_UNICODE;
        inputs[i].ki.time = 0;
        inputs[i].ki.dwExtraInfo = ::GetMessageExtraInfo();
        ///////////////////////////////////////////////////////
        inputs[i + 1].type = INPUT_KEYBOARD;
        inputs[i + 1].ki.wVk = 0;
        inputs[i + 1].ki.wScan = txt[keyCount++];
        inputs[i + 1].ki.dwFlags = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        inputs[i + 1].ki.time = 0;
        inputs[i + 1].ki.dwExtraInfo = ::GetMessageExtraInfo();

    }
    BOOL back = ::SendInput(size * 2, inputs, sizeof(INPUT));
    free(inputs);
    return back;
}

BOOL CWndBase::KeyCodeDo(LPCTSTR code, unsigned flag)
{
    //TrFu;
    //TrVar(code);
    WORD vKey = 0;
    TrRet(!code, FALSE);
    xKeyMapIt = s_KeyNameToKey.find(code);
    if (xKeyMapIt != s_KeyNameToKey.end()) {
        vKey = xKeyMapIt->second;
    }
    else if (_tcslen(code) == 1) {
        vKey = (WORD)code[0];
    }
    else {
        TrRet(!vKey, FALSE);
    }
    INPUT ip;
    memset(&ip, 0, sizeof(ip));
    ip.type = INPUT_KEYBOARD;
    ip.ki.wVk = vKey;
    ip.ki.dwFlags = flag;
    SendInput(1, &ip, sizeof(INPUT));
    return TRUE;
}

BOOL CWndBase::KeyCodeType(LPCTSTR key)
{
    BOOL back = KeyCodeDo(key, 0);
    ::Sleep(20);
    return back && KeyCodeDo(key, KEYEVENTF_KEYUP);
}

BOOL CWndBase::KeyCodeDown(LPCTSTR key)
{
    ::Sleep(20);
    return KeyCodeDo(key, 0);
}

BOOL CWndBase::KeyCodeUp(LPCTSTR key)
{
    ::Sleep(20);
    return KeyCodeDo(key, KEYEVENTF_KEYUP);
}

BOOL CWndBase::Activate()
{
    ::ShowWindow(m_hWnd, SW_RESTORE);
    TrRet(!::SetForegroundWindow(m_hWnd), FALSE);
    return TRUE;
}

BOOL CWndBase::ActivateEx()
{
    SWinInfo inf;
    inf.m_hWnd = ::GetDesktopWindow();
    inf.m_isDeskTop = true;
    CWndBase wnd(inf);
    wnd.KeyCodeDown(_T("LeftAlt"));
    BOOL SetForegroundRes = ::SetForegroundWindow(m_hWnd);
    ::Sleep(20);
    wnd.KeyCodeUp(_T("LeftAlt"));
    TrRet(!SetForegroundRes, FALSE);
    return TRUE;
}

//BOOL CWndBase::ActivateEx()
//{
//	// It works like Alt + TAB window switching in Windows
//	// to unlock SetForegroundWindow we need to imitate Alt pressing
//	BYTE keyState[256] = { 0 };
//	if (::GetKeyboardState((LPBYTE)&keyState)) {
//		if (!(keyState[VK_MENU] & 0x80)) {
//			::keybd_event(VK_MENU, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);
//		}
//	}
//	BOOL SetForegroundRes = ::SetForegroundWindow(m_hWnd);
//	if (::GetKeyboardState((LPBYTE)&keyState)){
//		if (!(keyState[VK_MENU] & 0x80)){
//			::keybd_event(VK_MENU, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0);
//		}
//	}
//	TrRet(!SetForegroundRes, FALSE);
//	return TRUE;
//}

void CWndBase::Minimize()
{
    ::ShowWindow(m_hWnd, SW_MINIMIZE);
}

void CWndBase::Maximize()
{
    ::ShowWindow(m_hWnd, SW_MAXIMIZE);
}

void CWndBase::UpdateWindow()
{
    ::UpdateWindow(m_hWnd);
}

int CWndBase::GetWndWidth()
{
    RECT rc = { 0, 0, 0, 0 };
    ::GetClientRect(m_hWnd, &rc);
    return rc.right - rc.left;
}

int CWndBase::GetWndHeight()
{
    RECT rc = { 0, 0, 0, 0 };
    ::GetClientRect(m_hWnd, &rc);
    return rc.bottom - rc.top;
}

SRect CWndBase::GetWndRect()
{
    RECT rcWin;
    SRect rc;
    ::GetWindowRect(m_hWnd, &rcWin);
    rc.m_posX = rcWin.left;
    rc.m_posY = rcWin.top;
    rc.m_width = rcWin.right - rcWin.left;
    rc.m_height = rcWin.bottom - rcWin.top;
    return rc;
}

BOOL CWndBase::SetWndRect(const SRect& rc)
{
    return ::SetWindowPos(m_hWnd, 0, rc.m_posX, rc.m_posY, rc.m_width, rc.m_height, SWP_NOZORDER);
}

PrcId CWndBase::GetWindowPrcId()
{
    PrcId pid = 0;
    ::GetWindowThreadProcessId(m_hWnd, &pid);
    return pid;
}

WinInfoList CWndBase::GetAllChildByClassName(const TCHAR* title)
{
    TCHAR clName[256];
    WinInfoList backList;
    stack<HWND> wndStack;
    HWND hWnd = ::GetWindow(m_hWnd, GW_CHILD), currWnd;
    wndStack.push(hWnd);
    do {
        currWnd = wndStack.top();
        wndStack.pop();
        if (::GetClassName(currWnd, clName, 256) && !::_tcscmp(clName, title)) {
            SWinInfo inf;
            inf.m_hWnd = currWnd;
            backList.push_back(inf);
        }
        for (hWnd = ::GetWindow(currWnd, GW_CHILD); hWnd; hWnd = ::GetWindow(hWnd, GW_HWNDNEXT)) {
            wndStack.push(hWnd);
            if (hWnd == ::GetWindow(hWnd, GW_HWNDLAST))
            {
                break;
            }
        }
    } while (!wndStack.empty());
    return backList;
}

WinInfoList CWndBase::GetAllChild()
{
    WinInfoList backList;
    stack<HWND> wndStack;
    HWND hWnd = ::GetWindow(m_hWnd, GW_CHILD), currWnd;
    wndStack.push(hWnd);
    do {
        currWnd = wndStack.top();
        wndStack.pop();
        SWinInfo inf;
        inf.m_hWnd = currWnd;
        backList.push_back(inf);
        for (hWnd = ::GetWindow(currWnd, GW_CHILD); hWnd; hWnd = ::GetWindow(hWnd, GW_HWNDNEXT)) {
            wndStack.push(hWnd);
            if (hWnd == ::GetWindow(hWnd, GW_HWNDLAST))
            {
                break;
            }
        }
    } while (!wndStack.empty());
    return backList;
}

WinInfoList CWndBase::GetAllZOrder()
{
    WinInfoList backList;
    HWND hwnd = ::GetTopWindow(m_hWnd);
    while (hwnd) {
        hwnd = ::GetNextWindow(hwnd, GW_HWNDNEXT);
        SWinInfo inf;
        inf.m_hWnd = hwnd;
        backList.push_back(inf);
    }
    return backList;
}

BOOL CWndBase::OnCreate()
{
    return !SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
}


BOOL CWndBase::IsTopWindow(const SWinInfo& inf)
{
    if (::IsWindow(inf.m_hWnd) && ::GetDesktopWindow() == ::GetAncestor(inf.m_hWnd, GA_PARENT)) {
        return TRUE;
    }
    return FALSE;
}

BOOL CWndBase::IsWindow(HWND hWnd)
{
    return ::IsWindow(hWnd);
}

WinInfoList CWndBase::FindWindowByTitleText(const TCHAR* title, EFindWindowWhat eWhat)
{
    static const int tmpTitleSize = 1024;
    WinInfoList backList;
    TCHAR tmpTitle[tmpTitleSize] = { 0 };
    for (HWND hWnd = ::GetWindow(::GetDesktopWindow(), GW_CHILD); hWnd && hWnd != ::GetWindow(hWnd, GW_HWNDLAST); hWnd = ::GetWindow(hWnd, GW_HWNDNEXT)) {
        if (GetWindowText(hWnd, tmpTitle, tmpTitleSize - 1) > 1) {
            if (eWhat == eWithTitleExakt) {
                if (!_tcscmp(tmpTitle, title)) {
                    SWinInfo inf;
                    inf.m_hWnd = hWnd;
                    backList.push_back(inf);
                }
            }
            else
            {
                if (_tcsstr(tmpTitle, title)) {
                    SWinInfo inf;
                    inf.m_hWnd = hWnd;
                    backList.push_back(inf);
                }
            }
        }
    }
    return backList;
}

SWinInfo CWndBase::GetActivWindow()
{
    SWinInfo inf;
    inf.m_hWnd = ::GetForegroundWindow();
    return inf;
}

WinInfoList CWndBase::GetAllTopWindow()
{
    WinInfoList backList;
    HWND hFirst = GetTopWindow(NULL), hCurr = hFirst;
    do {
        hCurr = GetNextWindow(hCurr, GW_HWNDNEXT);
        if (!hCurr || hCurr == hFirst) {
            break;
        }
        SWinInfo inf;
        inf.m_hWnd = hCurr;
        backList.push_back(inf);
    } while (true);
    return backList;
}

void CWndBase::ClickL(unsigned delay, const SPoint& pt)
{
    //TrFu;
    SetCursorPosTo(m_hWnd, pt);
    TrVoid(!PostMessage(m_hWnd, WM_LBUTTONDOWN, MK_LBUTTON, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
    if (delay) {
        ::Sleep(delay);
    }
    TrVoid(!PostMessage(m_hWnd, WM_LBUTTONUP, 0, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
}

void CWndBase::ClickExL(const SPoint& pt)
{
    SetCursorPosTo(m_hWnd, pt);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
}

void CWndBase::ClickDL(const SPoint& pt)
{
    //TrFu;
    SetCursorPosTo(m_hWnd, pt);
    TrVoid(!PostMessage(m_hWnd, WM_LBUTTONDBLCLK, 0, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
}

void CWndBase::ClickExDL(const SPoint& pt)
{
    SetCursorPosTo(m_hWnd, pt);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
}

void CWndBase::ClickR(unsigned delay, const SPoint& pt)
{
    //TrFu;
    SetCursorPosTo(m_hWnd, pt);
    TrVoid(!PostMessage(m_hWnd, WM_RBUTTONDOWN, MK_RBUTTON, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
    if (delay) {
        ::Sleep(delay);
    }
    TrVoid(!PostMessage(m_hWnd, WM_RBUTTONUP, 0, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
}

void CWndBase::ClickExR(const SPoint& pt)
{
    SetCursorPosTo(m_hWnd, pt);
    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
}

void CWndBase::ClickExDR(const SPoint& pt)
{
    SetCursorPosTo(m_hWnd, pt);
    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, GetMessageExtraInfo());
    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, GetMessageExtraInfo());
}

void CWndBase::MoveMouseTo(const SPoint& pt)
{
    //TrFu;
    SetCursorPosTo(m_hWnd, pt);
    TrVoid(!PostMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
}

void CWndBase::MoveLMouse(const SPoint& pt)
{
    // TrFu;
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    Sleep(500); // 1 Sekunde Verzögerung
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (cursorPos.x + pt.m_x) * 65536 / GetSystemMetrics(SM_CXSCREEN), (cursorPos.y + pt.m_y) * 65536 / GetSystemMetrics(SM_CYSCREEN), 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void CWndBase::MoveRMouse(const SPoint& pt)
{
    // TrFu;
    POINT cursorPos;
    GetCursorPos(&cursorPos);
    mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
    Sleep(500); // 1 Sekunde Verzögerung
    mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (cursorPos.x + pt.m_x) * 65536 / GetSystemMetrics(SM_CXSCREEN), (cursorPos.y + pt.m_y) * 65536 / GetSystemMetrics(SM_CYSCREEN), 0, 0);
    mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}

SWinInfo CWndBase::GetWindowFromPoint(const SPoint& pt)
{
    SWinInfo inf;
    POINT ptWin = { pt.m_x, pt.m_y };
    HWND hWnd = ::WindowFromPoint(ptWin);
    inf.m_hWnd = hWnd;
    return inf;
}

Str CWndBase::GetClassName()
{
    Str back;
    TCHAR clName[256] = { 0 };
    ::GetClassName(m_hWnd, clName, 256);
    back = clName;
    return back;
}

WinInfoList CWndBase::GetAllParent()
{
    WinInfoList backList;
    HWND hFirst = GetTopWindow(NULL), hCurr = m_hWnd;
    do {
        hCurr = GetParent(hCurr);
        if (!hCurr || hCurr == hFirst) {
            break;
        }
        SWinInfo inf;
        inf.m_hWnd = hCurr;
        backList.push_back(inf);
    } while (true);
    return backList;
}

WinInfoList CWndBase::GetAllChildFromPoint(const SPoint& pt)
{
    //TrFu;
    WinInfoList backList;
    //TrVar(m_hWnd);
    POINT clPoint = { pt.m_x, pt.m_y };
    HWND wnd = m_hWnd;
    HWND currWnd = ::ChildWindowFromPointEx(wnd, clPoint,
        CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT);
    POINT parPoint = clPoint;
    ::ClientToScreen(wnd, &parPoint);
    ::ScreenToClient(currWnd, &parPoint);
    clPoint = parPoint;
    do {
        SWinInfo inf;
        inf.m_hWnd = currWnd;
        //TrVar(currWnd);
        backList.push_back(inf);
        wnd = currWnd;
        currWnd = ::ChildWindowFromPointEx(wnd, clPoint,
            CWP_SKIPINVISIBLE | CWP_SKIPDISABLED | CWP_SKIPTRANSPARENT);
        parPoint = clPoint;
        ::ClientToScreen(wnd, &parPoint);
        ::ScreenToClient(currWnd, &parPoint);
        clPoint = parPoint;
    } while (currWnd != wnd);
    return backList;
}

/* inline */
SRect CWndBase::CutWndRect(const SRect& rc, int defPix)
{
    TrFu;
    TrVar(defPix);
    SRect wndRc = GetWndRect();
    SRect res = wndRc;
    if (!rc.IsEmpty() && rc.IsValid()) {
        res = wndRc.Cut(rc, defPix);
        if (!res.IsValid()) {
            res = wndRc;
        }
    }
    return res;
}

//BOOL CWndBase::GetScreenShot(void*& pScreenData, size_t& screenDataSize, const SRect& wndRect)
//{
//	TrFu;
//
//	screenDataSize = 0;
//
//	// --- Resource Initialization ---
//	HDC hdcScreen = NULL;
//	HDC hdcMemDC = NULL;
//	HBITMAP hbmScreen = NULL;
//	HGDIOBJ hbmOld = NULL;
//
//	// Pointer to the pixel data buffer allocated by CreateDIBSection
//	char* lpDIBits = NULL;
//
//	BOOL back = FALSE;
//	DWORD dwBmpSize = 0;
//	DWORD dwSizeofDIB = 0;
//
//	// We no longer need GlobalAlloc/GlobalLock for the DIBits themselves, only for the final output
//	// The BITMAPFILEHEADER and BITMAPINFOHEADER remain necessary for the output BMP file format.
//	BITMAPFILEHEADER bmfHeader = { 0 };
//	BITMAPINFO bi = { 0 }; // Use BITMAPINFO to correctly pass to CreateDIBSection
//
//	// 1. Setup the BITMAPINFO structure for 32-bit BI_RGB
//	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	bi.bmiHeader.biWidth = wndRect.m_width;
//	// Negative height forces a top-down DIB, which is often easier to work with 
//	// and standard for most memory-mapped images.
//	bi.bmiHeader.biHeight = -wndRect.m_height;
//	bi.bmiHeader.biPlanes = 1;
//	bi.bmiHeader.biBitCount = 32;
//	bi.bmiHeader.biCompression = BI_RGB;
//	bi.bmiHeader.biSizeImage = 0;
//	// Other fields are zeroed correctly by the {0} initializer
//
//	// 2. Get the screen DC.
//	hdcScreen = GetDC(NULL);
//	if (!hdcScreen) {
//		Tr("GetDC(NULL) failed");
//		goto SaveScreenShot_ret;
//	}
//
//	// 3. Create a compatible DC
//	hdcMemDC = CreateCompatibleDC(hdcScreen);
//	if (!hdcMemDC) {
//		Tr("CreateCompatibleDC has failed");
//		goto SaveScreenShot_ret;
//	}
//
//	// 4. Create the DIB Section
//	// CRITICAL FIX: Use CreateDIBSection for a deterministic, device-independent format
//	// lpDIBits will receive the pointer to the allocated pixel data
//	hbmScreen = CreateDIBSection(
//		hdcMemDC,        // DC (can be NULL or memory DC)
//		&bi,             // Bitmap info structure
//		DIB_RGB_COLORS,  // Color table usage
//		(void**)&lpDIBits, // Address of a pointer to receive the DIB bit values
//		NULL,            // File mapping handle (not used)
//		0                // Offset (not used)
//	);
//
//	if (!hbmScreen || !lpDIBits) {
//		Tr("CreateDIBSection Failed");
//		// hbmScreen may be valid even if lpDIBits is NULL on failure, but cleanup handles both
//		goto SaveScreenShot_ret;
//	}
//
//	// 5. Select the DIB Section into the memory DC and save the original HBITMAP
//	hbmOld = SelectObject(hdcMemDC, hbmScreen);
//
//	// 6. Bit block transfer (screenshot)
//	if (!BitBlt(hdcMemDC,
//		0, 0,
//		wndRect.m_width, wndRect.m_height,
//		hdcScreen,
//		wndRect.m_posX, wndRect.m_posY,
//		SRCCOPY)) {
//		Tr("BitBlt has failed");
//		goto SaveScreenShot_ret;
//	}
//
//	// The pixel data is now in the memory pointed to by lpDIBits.
//
//	// 7. Calculate final sizes
//	// Calculate size of pixel data (32-bit, aligned to 4 bytes per row)
//	dwBmpSize = ((wndRect.m_width * 32 + 31) / 32) * 4 * wndRect.m_height;
//
//	// Total size includes headers
//	dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
//
//	// 8. Finalize BMP file header
//	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
//	bmfHeader.bfSize = dwSizeofDIB;
//	bmfHeader.bfType = 0x4D42; // 'BM'
//
//	// 9. Copy data to external buffer
//	screenDataSize = dwSizeofDIB;
//	if (screenDataSize > 0 && frameMP::ReAlloc(&pScreenData, 0, screenDataSize)) {
//		back = TRUE;
//		char* pCurrData = (char*)pScreenData;
//
//		// Copy File Header
//		memcpy(pCurrData, &bmfHeader, sizeof(BITMAPFILEHEADER));
//		pCurrData += sizeof(BITMAPFILEHEADER);
//		// Copy Info Header (only the BITMAPINFOHEADER part of bi)
//		memcpy(pCurrData, &bi.bmiHeader, sizeof(BITMAPINFOHEADER));
//		pCurrData += sizeof(BITMAPINFOHEADER);
//		// Copy Pixel Data (from the DIB Section's direct pointer)
//		memcpy(pCurrData, lpDIBits, dwBmpSize);
//	}
//
//SaveScreenShot_ret:
//	// --- Cleanup: ORDER MATTERS ---
//
//	// 1. Restore the original object to the memory DC
//	// This is necessary before deleting hdcMemDC or hbmScreen
//	if (hbmOld) SelectObject(hdcMemDC, hbmOld);
//
//	// 2. Delete GDI objects
//	// The memory pointed to by lpDIBits is freed when hbmScreen is deleted.
//	if (hbmScreen) DeleteObject(hbmScreen);
//	if (hdcMemDC) DeleteDC(hdcMemDC);
//
//	// 3. Release the screen DC
//	if (hdcScreen) ReleaseDC(NULL, hdcScreen);
//
//	TrRet(!back, FALSE);
//	return back;
//}

BOOL CWndBase::GetScreenShot(void*& pScreenData, size_t& screenDataSize, const SRect& wndRect)
{
    TrFu;

    screenDataSize = 0;

    // --- Resource Initialization ---
    HDC hdcScreen = NULL;
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    HGDIOBJ hbmOld = NULL; // Stores the original bitmap selected into hdcMemDC

    BOOL back = FALSE;
    DWORD dwSizeofDIB = 0;
    DWORD dwBmpSize = 0;
    HANDLE hDIB = NULL;
    char* lpbitmap = NULL;

    BITMAP bmpScreen;
    BITMAPFILEHEADER bmfHeader = { 0 };
    BITMAPINFOHEADER bi = { 0 };

    // 1. Get the screen DC. Use GetDC(NULL) for the screen.
    hdcScreen = GetDC(NULL);
    if (!hdcScreen) {
        Tr("GetDC(NULL) failed");
        goto SaveScreenShot_ret;
    }

    // 2. Create a compatible DC
    hdcMemDC = CreateCompatibleDC(hdcScreen);
    if (!hdcMemDC) {
        Tr("CreateCompatibleDC has failed");
        goto SaveScreenShot_ret;
    }

    // 3. Create a compatible bitmap
    hbmScreen = CreateCompatibleBitmap(hdcScreen, wndRect.m_width, wndRect.m_height);
    if (!hbmScreen) {
        Tr("CreateCompatibleBitmap Failed");
        goto SaveScreenShot_ret;
    }

    // 4. Select the compatible bitmap into the compatible memory DC and save the original
    // CRITICAL FIX: Save the HGDIOBJ returned by SelectObject
    hbmOld = SelectObject(hdcMemDC, hbmScreen);

    // 5. Bit block transfer (screenshot)
    if (!BitBlt(hdcMemDC,
        0, 0,
        wndRect.m_width, wndRect.m_height,
        hdcScreen,
        wndRect.m_posX, wndRect.m_posY,
        SRCCOPY)) {
        Tr("BitBlt has failed");
        goto SaveScreenShot_ret;
    }

    // 6. Get BITMAP structure and setup BITMAPINFOHEADER for 32-bit BI_RGB
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;    // Enforcing 32-bit format
    bi.biCompression = BI_RGB;
    // Size is calculated below, but GDI can ignore this for GetDIBits
    bi.biSizeImage = 0;
    // ... all other BI fields are zeroed correctly

    // Calculate the size of the pixel data buffer (32-bit, aligned to DWORD boundary)
    // Formula: ((W * Bits + 31) / 32) * 4 * H
    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // 7. Allocate buffer for pixel data
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    if (!hDIB) {
        Tr("GlobalAlloc failed for DIB");
        goto SaveScreenShot_ret;
    }
    lpbitmap = (char*)GlobalLock(hDIB);
    if (!lpbitmap) {
        Tr("GlobalLock failed");
        goto SaveScreenShot_ret;
    }

    // 8. Get the actual 32-bit DIBits into the buffer
    // CRITICAL NOTE: Using hdcScreen here is correct as it's a palette-independent DC
    if (!GetDIBits(hdcScreen, hbmScreen, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO*)&bi, DIB_RGB_COLORS)) {
        Tr("GetDIBits failed");
        goto SaveScreenShot_ret;
    }

    // 9. Finalize BMP structure sizes and headers
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    // Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
    // Size of the file
    bmfHeader.bfSize = dwSizeofDIB;
    // bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; // 'BM'

    // 10. Copy data to external buffer
    screenDataSize = dwSizeofDIB;
    if (screenDataSize > 0 && frameMP::ReAlloc(&pScreenData, 0, screenDataSize)) {
        back = TRUE;
        char* pCurrData = (char*)pScreenData;

        // Copy File Header
        memcpy(pCurrData, &bmfHeader, sizeof(BITMAPFILEHEADER));
        pCurrData += sizeof(BITMAPFILEHEADER);
        // Copy Info Header
        memcpy(pCurrData, &bi, sizeof(BITMAPINFOHEADER));
        pCurrData += sizeof(BITMAPINFOHEADER);
        // Copy Pixel Data
        memcpy(pCurrData, lpbitmap, dwBmpSize);
    }

SaveScreenShot_ret:
    // --- Cleanup: ORDER MATTERS ---

    // 1. Unlock and Free the DIB from the heap (if allocated/locked)
    if (lpbitmap) GlobalUnlock(hDIB);
    if (hDIB) GlobalFree(hDIB);

    // 2. Restore the original bitmap to the memory DC
    if (hbmOld) SelectObject(hdcMemDC, hbmOld);

    // 3. Delete GDI objects
    if (hbmScreen) DeleteObject(hbmScreen);
    if (hdcMemDC) DeleteDC(hdcMemDC); // Use DeleteDC for CreateCompatibleDC

    // 4. Release the screen DC
    // CRITICAL FIX: Use ReleaseDC(NULL, hdcScreen) for GetDC(NULL)
    if (hdcScreen) ReleaseDC(NULL, hdcScreen);
    TrRet(!back, FALSE);
    return back;
}

int CWndBase::GetMonitorCount()
{
    return ::GetSystemMetrics(SM_CMONITORS);
}

SWinInfo CWndBase::GetDesktopWindow()
{
    SWinInfo inf;
    inf.m_hWnd = ::GetDesktopWindow();
    inf.m_isDeskTop = true;
    return inf;
}

BOOL CWndBase::ShowDesktopWindow()
{
    // TrFu;
    SWinInfo inf;
    inf.m_hWnd = ::GetDesktopWindow();
    inf.m_isDeskTop = true;
    CWndBase wnd(inf);
    BOOL back = wnd.KeyCodeDown(_T("LeftWin")) && wnd.KeyCodeDown(_T("D"));
    // TrVar(back);
    if (back) {
        ::Sleep(20);
        back = wnd.KeyCodeUp(_T("D")) && wnd.KeyCodeUp(_T("LeftWin"));
    }
    TrRet(!back, FALSE);
    return back;
    // //
    // // This code produce issues on win7-32b. Desktop loss mouse input 
    // //
    ///* WinInfoList allTopWnd = GetAllTopWindow();
    // WINDOWPLACEMENT wndPl = {0};
    // wndPl.length = sizeof(WINDOWPLACEMENT);
    // for (WinInfoList::iterator it = allTopWnd.begin(), itEnd = allTopWnd.end(); it != itEnd; ++it) {
    //     if (IsAppWindow((*it).m_hWnd)) {
    //         ::GetWindowPlacement((*it).m_hWnd, &wndPl);
    //         wndPl.showCmd = SW_MINIMIZE;
    //         TrRet(!::SetWindowPlacement((*it).m_hWnd, &wndPl), FALSE);
    //     }
    // }
    // return TRUE;*/
}

int CWndBase::GetTextFromCL(StrA& back)
{
    TrFu;
    int res = 0;
    TrRet(!IsClipboardFormatAvailable(CF_TEXT), res);
    TrRet(!OpenClipboard(NULL), res);
    HGLOBAL hglb = GetClipboardData(CF_TEXT);
    if (hglb != NULL) {
        LPCSTR lpcstr = (LPCSTR)GlobalLock(hglb);
        TrVar(lpcstr);
        if (lpcstr != NULL) {
            back.assign(lpcstr);
            res = (int)back.length();
            GlobalUnlock(hglb);
        }
    }
    CloseClipboard();
    return res;
}

// Gemeni code <-

// Define the structure of a BMP file header (packed to 2 bytes)
#pragma pack(push, 2)
typedef struct tagBITMAPFILEHEADER_SMALL {
    WORD    bfType;
    DWORD   bfSize;
    WORD    bfReserved1;
    WORD    bfReserved2;
    DWORD   bfOffBits;
} BITMAPFILEHEADER_SMALL;
#pragma pack(pop)

#define MIN_BMP_HEADER_SIZE (sizeof(BITMAPFILEHEADER_SMALL) + sizeof(BITMAPINFOHEADER))
#define PIXEL_BYTE_COUNT 4 

// External helper function (assumed to be defined/available)
int CalculateBGRManhattanDistance(DWORD pixel1, DWORD pixel2)
{
    BYTE b1 = (BYTE)(pixel1 & 0xFF);
    BYTE g1 = (BYTE)((pixel1 >> 8) & 0xFF);
    BYTE r1 = (BYTE)((pixel1 >> 16) & 0xFF);

    BYTE b2 = (BYTE)(pixel2 & 0xFF);
    BYTE g2 = (BYTE)((pixel2 >> 8) & 0xFF);
    BYTE r2 = (BYTE)((pixel2 >> 16) & 0xFF);

    return abs(b1 - b2) + abs(g1 - g2) + abs(r1 - r2);
}

// --- Main Function: CompareBmpData with Size Determination and Cropping ---

double CompareBmpData(void* dataBmp1, size_t dataBmp1Size, void* dataBmp2, size_t dataBmp2Size)
{
    if (!dataBmp1 || dataBmp1Size < MIN_BMP_HEADER_SIZE || !dataBmp2 || dataBmp2Size < MIN_BMP_HEADER_SIZE) {
        return 0.0;
    }

    // --- Configuration ---
    const int PIXEL_TOLERANCE = 30;
    const DWORD BGR_MASK = 0x00FFFFFF;

    // --- Header and Size Determination ---
    const BITMAPFILEHEADER_SMALL* pFileHdr1 = (const BITMAPFILEHEADER_SMALL*)dataBmp1;
    const BITMAPINFOHEADER* pBmpInfo1 = (const BITMAPINFOHEADER*)((char*)dataBmp1 + sizeof(BITMAPFILEHEADER_SMALL));

    const BITMAPFILEHEADER_SMALL* pFileHdr2 = (const BITMAPFILEHEADER_SMALL*)dataBmp2;
    const BITMAPINFOHEADER* pBmpInfo2 = (const BITMAPINFOHEADER*)((char*)dataBmp2 + sizeof(BITMAPFILEHEADER_SMALL));

    size_t area1 = (size_t)pBmpInfo1->biWidth * abs(pBmpInfo1->biHeight);
    size_t area2 = (size_t)pBmpInfo2->biWidth * abs(pBmpInfo2->biHeight);

    // Determine target dimensions (smallest image's dimensions, regardless of parameter position)
    const BITMAPINFOHEADER* pSmallestInfo = (area1 <= area2) ? pBmpInfo1 : pBmpInfo2;
    const BITMAPINFOHEADER* pLargestInfo = (area1 <= area2) ? pBmpInfo2 : pBmpInfo1;

    int targetWidth = pSmallestInfo->biWidth;
    int targetHeight = abs(pSmallestInfo->biHeight);

    if (targetWidth <= 0 || targetHeight <= 0) {
        return 0.0;
    }

    // --- Pixel Data Pointers Setup ---

    // Determine which pointer points to the smallest image data (pRefData)
    // and which points to the largest/compared image data (pCmpData)
    const void* pRefData = (area1 <= area2) ? dataBmp1 : dataBmp2;
    const BITMAPFILEHEADER_SMALL* pRefFileHdr = (area1 <= area2) ? pFileHdr1 : pFileHdr2;
    const BYTE* pRefPixels = (const BYTE*)pRefData + pRefFileHdr->bfOffBits;

    const void* pCmpData = (area1 <= area2) ? dataBmp2 : dataBmp1;
    const BITMAPFILEHEADER_SMALL* pCmpFileHdr = (area1 <= area2) ? pFileHdr2 : pFileHdr1;
    const BYTE* pCmpPixels = (const BYTE*)pCmpData + pCmpFileHdr->bfOffBits;

    // Now, pRefData and pCmpData are correctly ordered based on size.
    // The width of the image referred to by pRefData is targetWidth.
    // The width of the image referred to by pCmpData is pLargestInfo->biWidth.

    int refWidth = targetWidth; // Width of the image currently in pRefPixels (guaranteed smallest)
    int cmpWidth = pLargestInfo->biWidth; // Width of the image currently in pCmpPixels

    // --- Comparison Logic (Cropping is handled by loop bounds) ---

    size_t equalPixels = 0;
    size_t comparedPixels = 0;

    for (int y = 0; y < targetHeight; ++y) {
        for (int x = 0; x < targetWidth; ++x) {
            {
                // Calculate pixel index (offset) based on the *actual* width of the image data.
                // NOTE: targetWidth is pSmallestInfo->biWidth, cmpWidth is pLargestInfo->biWidth.

                size_t ref_i = (size_t)y * refWidth + x;
                size_t cmp_i = (size_t)y * cmpWidth + x;

                // Read 4-byte pixel values
                DWORD refPixel = *(const DWORD*)(pRefPixels + ref_i * PIXEL_BYTE_COUNT);
                DWORD cmpPixel = *(const DWORD*)(pCmpPixels + cmp_i * PIXEL_BYTE_COUNT);

                // Check for EXCLUSION (Alpha = 0x00) - If this is not needed, remove this block
                // For screen capture, it's generally safe to remove if BGR data is sufficient.
                const DWORD ALPHA_MASK = 0xFF000000;
                if ((refPixel & ALPHA_MASK) == 0 || (cmpPixel & ALPHA_MASK) == 0) {
                    continue;
                }

                // 1. PIXEL IS VALID: Increment total compared count
                comparedPixels++;

                // 2. Perform ENHANCED PIXEL COMPARISON (Manhattan Distance on BGR components)
                DWORD refBGR = refPixel & BGR_MASK;
                DWORD cmpBGR = cmpPixel & BGR_MASK;

                int distance = CalculateBGRManhattanDistance(refBGR, cmpBGR);

                if (distance <= PIXEL_TOLERANCE) {
                    equalPixels++;
                }
            }
        }
    }

    // --- Calculate and Return Percentage ---

    if (comparedPixels == 0) {
        return 0.0;
    }

    return (double)equalPixels * 100.0 / comparedPixels;
}

// Gemeni code ->

}; // namespace frameMP

