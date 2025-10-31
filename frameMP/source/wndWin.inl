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

// wndWin.inl


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

static inline void SetCursorPosTo(const HWND &hWnd, const SPoint &pt)
{
     POINT p = {pt.m_x, pt.m_y};
    ::ClientToScreen(hWnd, &p);
    ::SetCursorPos(p.x, p.y);
}

void CWndBase::InitKeyNameToKey()
{
    s_KeyNameToKey[_T("Shift")] =       VK_SHIFT; 
    s_KeyNameToKey[_T("Ctrl")] =        VK_CONTROL;
    s_KeyNameToKey[_T("Alt")] =         VK_MENU;   
    s_KeyNameToKey[_T("LeftShift")] =   VK_LSHIFT;
    s_KeyNameToKey[_T("RightShift")] =  VK_RSHIFT;
    s_KeyNameToKey[_T("LeftCtrl")] =    VK_LCONTROL;
    s_KeyNameToKey[_T("RightCtrl")] =   VK_RCONTROL;
    s_KeyNameToKey[_T("LeftAlt")] =     VK_LMENU;
    s_KeyNameToKey[_T("RightAlt")] =    VK_RMENU;
    s_KeyNameToKey[_T("NumLock")] =     VK_NUMLOCK;
    s_KeyNameToKey[_T("ScrolLock")] =   VK_SCROLL;
    s_KeyNameToKey[_T("Tab")] =         VK_TAB;
    s_KeyNameToKey[_T("Back")] =        VK_BACK;
    s_KeyNameToKey[_T("Clear")] =       VK_CLEAR;
    s_KeyNameToKey[_T("Return")] =      VK_RETURN;
    s_KeyNameToKey[_T("Space")] =       VK_SPACE;
    s_KeyNameToKey[_T("Pause")] =       VK_PAUSE;
    s_KeyNameToKey[_T("Escape")] =      VK_ESCAPE;
    s_KeyNameToKey[_T("PageUp")] =      VK_PRIOR;
    s_KeyNameToKey[_T("PageDown")] =    VK_NEXT;
    s_KeyNameToKey[_T("Home")] =        VK_HOME;
    s_KeyNameToKey[_T("End")] =         VK_END;
    s_KeyNameToKey[_T("KeyUp")] =       VK_UP;
    s_KeyNameToKey[_T("KeyDown")] =     VK_DOWN;
    s_KeyNameToKey[_T("KeyLeft")] =     VK_LEFT;
    s_KeyNameToKey[_T("KeyRight")] =    VK_RIGHT;
    s_KeyNameToKey[_T("Print")] =       VK_PRINT;
    s_KeyNameToKey[_T("PrintScreen")] = VK_SNAPSHOT;
    s_KeyNameToKey[_T("CapsLock")] =    VK_CAPITAL;
    s_KeyNameToKey[_T("CtrlBreak")] =   VK_CANCEL;
    s_KeyNameToKey[_T("Insert")] =      VK_INSERT;
    s_KeyNameToKey[_T("Delete")] =      VK_DELETE;
    s_KeyNameToKey[_T("LeftWin")] =     VK_LWIN;
    s_KeyNameToKey[_T("RightWin")] =    VK_RWIN;
    s_KeyNameToKey[_T("Plus")] =        VK_ADD;
    s_KeyNameToKey[_T("Minus")] =       VK_SUBTRACT;
    TCHAR buf[3] = {0};
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
    CWndBase *mWnd=(CWndBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
    if (mWnd && (mWnd->PumpMsg(uMsg, wParam, lParam) || mWnd->OnWndMsg(uMsg, wParam, lParam))) {
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

unsigned CWndBase::GetText(Str& back)
{
    unsigned cc = (unsigned)SendMessage(m_hWnd, WM_GETTEXTLENGTH, 0, 0);
    TrRet(cc < 1, 0);
    TCHAR *txt = (TCHAR*)calloc(cc+16, sizeof(TCHAR));
    cc = SendMessage(m_hWnd, WM_GETTEXT, cc+16, (LPARAM)txt);
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
    return SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM)txt);
}

BOOL CWndBase::TypeText(LPCTSTR txt)
{
    unsigned size = _tcslen(txt);
    TrRet(!size, FALSE);
    INPUT *inputs = (INPUT*)calloc(size*2, sizeof(INPUT));
    unsigned keyCount = 0;
    for (unsigned i = 0; i < size*2; i+=2) {
        inputs[i].type			  = INPUT_KEYBOARD;
        inputs[i].ki.wVk          = 0;
        inputs[i].ki.wScan        = txt[keyCount];
        inputs[i].ki.dwFlags      = KEYEVENTF_UNICODE;
        inputs[i].ki.time         = 0;
        inputs[i].ki.dwExtraInfo  = ::GetMessageExtraInfo();
        ///////////////////////////////////////////////////////
        inputs[i + 1].type			  = INPUT_KEYBOARD;
        inputs[i + 1].ki.wVk          = 0;
        inputs[i + 1].ki.wScan        = txt[keyCount++];
        inputs[i + 1].ki.dwFlags      = KEYEVENTF_UNICODE | KEYEVENTF_KEYUP;
        inputs[i + 1].ki.time         = 0;
        inputs[i + 1].ki.dwExtraInfo  = ::GetMessageExtraInfo();

    }
    BOOL back = ::SendInput(size*2, inputs, sizeof(INPUT));
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
    RECT rc = {0, 0, 0, 0};
    ::GetClientRect(m_hWnd, &rc);
    return rc.right - rc.left;
}

int CWndBase::GetWndHeight()
{
    RECT rc = {0, 0, 0, 0};
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

BOOL CWndBase::SetWndRect(const SRect &rc)
{
    return ::SetWindowPos(m_hWnd, 0, rc.m_posX, rc.m_posY, rc.m_width, rc.m_height, SWP_NOZORDER);
}

PrcId CWndBase::GetWindowPrcId()
{
    PrcId pid = 0;
    ::GetWindowThreadProcessId(m_hWnd, &pid);
    return pid;
}

WinInfoList CWndBase::GetAllChildByClassName(const TCHAR *title)
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
    } while(!wndStack.empty());
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
    } while(!wndStack.empty());
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


BOOL CWndBase::IsTopWindow(const SWinInfo &inf)
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

WinInfoList CWndBase::FindWindowByTitleText(const TCHAR *title, EFindWindowWhat eWhat)
{
    static const int tmpTitleSize = 1024;
    WinInfoList backList;
    TCHAR tmpTitle[tmpTitleSize] = {0};
    for (HWND hWnd = ::GetWindow(::GetDesktopWindow(), GW_CHILD);hWnd && hWnd != ::GetWindow(hWnd, GW_HWNDLAST); hWnd = ::GetWindow(hWnd, GW_HWNDNEXT)) {
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

void CWndBase::ClickL(unsigned delay, const SPoint &pt)
{
    //TrFu;
    SetCursorPosTo(m_hWnd, pt);
    TrVoid(!PostMessage(m_hWnd, WM_LBUTTONDOWN, MK_LBUTTON, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
    if (delay) {
        ::Sleep(delay);
    }
    TrVoid(!PostMessage(m_hWnd, WM_LBUTTONUP, 0, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
}

void CWndBase::ClickExL(const SPoint &pt)
{
    SetCursorPosTo(m_hWnd, pt);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
}

void CWndBase::ClickDL(const SPoint &pt)
{
    //TrFu;
    SetCursorPosTo(m_hWnd, pt);
    TrVoid(!PostMessage(m_hWnd, WM_LBUTTONDBLCLK, 0, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
}

void CWndBase::ClickExDL(const SPoint &pt)
{
	SetCursorPosTo(m_hWnd, pt);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, GetMessageExtraInfo());
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, GetMessageExtraInfo());
}

void CWndBase::ClickR(unsigned delay, const SPoint &pt)
{
    //TrFu;
    SetCursorPosTo(m_hWnd, pt);
    TrVoid(!PostMessage(m_hWnd, WM_RBUTTONDOWN, MK_RBUTTON, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
    if (delay) {
        ::Sleep(delay);
    }
    TrVoid(!PostMessage(m_hWnd, WM_RBUTTONUP, 0, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
}

void CWndBase::ClickExR(const SPoint &pt)
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

void CWndBase::MoveMouseTo(const SPoint &pt)
{
    //TrFu;
    SetCursorPosTo(m_hWnd, pt);
    TrVoid(!PostMessage(m_hWnd, WM_MOUSEMOVE, 0, (LPARAM)MAKELPARAM(pt.m_x, pt.m_y)));
}

void CWndBase::MoveLMouse(const SPoint &pt)
{
    // TrFu;
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
	Sleep(500); // 1 Sekunde Verzögerung
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (cursorPos.x + pt.m_x) * 65536 / GetSystemMetrics(SM_CXSCREEN), (cursorPos.y + pt.m_y) * 65536 / GetSystemMetrics(SM_CYSCREEN), 0, 0);
	mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void CWndBase::MoveRMouse(const SPoint &pt)
{
	// TrFu;
	POINT cursorPos;
	GetCursorPos(&cursorPos);
	mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
	Sleep(500); // 1 Sekunde Verzögerung
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, (cursorPos.x + pt.m_x) * 65536 / GetSystemMetrics(SM_CXSCREEN), (cursorPos.y + pt.m_y) * 65536 / GetSystemMetrics(SM_CYSCREEN), 0, 0);
	mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
}

SWinInfo CWndBase::GetWindowFromPoint(const SPoint &pt)
{
    SWinInfo inf;
    POINT ptWin = {pt.m_x, pt.m_y};
    HWND hWnd = ::WindowFromPoint(ptWin);
    inf.m_hWnd = hWnd;
    return inf;
}

Str CWndBase::GetClassName()
{
    Str back;
    TCHAR clName[256] = {0};
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

WinInfoList CWndBase::GetAllChildFromPoint(const SPoint &pt)
{
    //TrFu;
    WinInfoList backList;
    //TrVar(m_hWnd);
    POINT clPoint = {pt.m_x, pt.m_y};
    HWND wnd = m_hWnd;
    HWND currWnd = ::ChildWindowFromPointEx(wnd, clPoint,
                                        CWP_SKIPINVISIBLE|CWP_SKIPDISABLED|CWP_SKIPTRANSPARENT);
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
                                        CWP_SKIPINVISIBLE|CWP_SKIPDISABLED|CWP_SKIPTRANSPARENT);
        parPoint = clPoint;
        ::ClientToScreen(wnd, &parPoint);
        ::ScreenToClient(currWnd, &parPoint);
        clPoint = parPoint;
    } while (currWnd != wnd);
    return backList;
}

/* inline */
SRect CWndBase::CutWndRect(const SRect& rc)
{
	SRect wndRc = GetWndRect();
	SRect res = wndRc;
	if (!rc.IsEmpty() && rc.IsValid()) {
		res = wndRc.Cut(rc);
		if (!res.IsValid()) {
			res = wndRc;
		}
	}
	return res;
}

BOOL CWndBase::GetScreenShot(void *&pScreenData, size_t &screenDataSize, const SRect& wndRect)
{
    TrFu;
    //TrRet(pScreenData, FALSE);
	// wndRect.Trace();

	screenDataSize = 0;
    HDC hdcMemDC = NULL, hdcScreen = GetDC(NULL);
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    BOOL back = FALSE;
    DWORD dwSizeofDIB = 0, dwBmpSize = 0;
    HANDLE hDIB = NULL;
    char *lpbitmap = NULL;
    BITMAPFILEHEADER   bmfHeader = {0};    
    BITMAPINFOHEADER   bi = {0}; 

    // Create a compatible DC which is used in a BitBlt from the window DC
    hdcMemDC = CreateCompatibleDC(hdcScreen); 

    if (!hdcMemDC) {
        Tr("CreateCompatibleDC has failed");
        goto SaveScreenShot_ret;
    }

    // Create a compatible bitmap from the Window DC
    hbmScreen = CreateCompatibleBitmap(hdcScreen, wndRect.m_width, wndRect.m_height);
    
    if (!hbmScreen) {
        Tr("CreateCompatibleBitmap Failed");
        goto SaveScreenShot_ret;
    }

    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(hdcMemDC,hbmScreen);
    
    // Bit block transfer into our compatible memory DC.
    if (!BitBlt(hdcMemDC, 
               0,0, 
               wndRect.m_width, wndRect.m_height, 
               hdcScreen, 
               wndRect.m_posX, wndRect.m_posY,
               SRCCOPY)) {
        Tr("BitBlt has failed");
        goto SaveScreenShot_ret;
    }

    // Get the BITMAP from the HBITMAP
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);
      
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = bmpScreen.bmWidth;    
    bi.biHeight = bmpScreen.bmHeight;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;  
    bi.biXPelsPerMeter = 0;    
    bi.biYPelsPerMeter = 0;    
    bi.biClrUsed = 0;    
    bi.biClrImportant = 0;

    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
    // have greater overhead than HeapAlloc.
    hDIB = GlobalAlloc(GHND,dwBmpSize); 
    lpbitmap = (char *)GlobalLock(hDIB);    

    // Gets the "bits" from the bitmap and copies them into a buffer 
    // which is pointed to by lpbitmap.
    GetDIBits(hdcScreen, hbmScreen, 0,
        (UINT)bmpScreen.bmHeight,
        lpbitmap,
        (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    // Add the size of the headers to the size of the bitmap to get the total file size
    dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

    //Offset to where the actual bitmap bits start.
    bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER); 
    
    //Size of the file
    bmfHeader.bfSize = dwSizeofDIB; 
    
    //bfType must always be BM for Bitmaps
    bmfHeader.bfType = 0x4D42; //BM   

    screenDataSize = dwSizeofDIB;
    //TrVar(screenDataSize);
    if (screenDataSize > 0 && frameMP::ReAlloc(&pScreenData, 0, screenDataSize)) {
        back = TRUE;
        char *pCurrData = (char*)pScreenData;
        memcpy(pCurrData, &bmfHeader, sizeof(BITMAPFILEHEADER));
        pCurrData += sizeof(BITMAPFILEHEADER);
        memcpy(pCurrData, &bi, sizeof(BITMAPINFOHEADER));
        pCurrData += sizeof(BITMAPINFOHEADER);
        memcpy(pCurrData, lpbitmap, dwBmpSize);
    }

 //Unlock and Free the DIB from the heap
    GlobalUnlock(hDIB);    
    GlobalFree(hDIB);

SaveScreenShot_ret:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    ReleaseDC(m_hWnd, hdcScreen);

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
