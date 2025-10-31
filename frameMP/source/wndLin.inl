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

// wndLin.inl

#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>


struct TypeCode
{
	KeyCode m_key;
	KeyCode m_keyEx;
    KeyCode m_keyEx2;
	TypeCode(KeyCode k = 0, KeyCode kEx = 0, KeyCode kEx2 = 0) : m_key(k), m_keyEx(kEx), m_keyEx2(kEx2) 
	{}
};

map<char, TypeCode> GetCharToTypeCodeMap(Display* disp)
{
	TrFu;
	map<char, TypeCode> charToTypeCodeMap;
    charToTypeCodeMap.insert(pair<char, TypeCode>('\n', TypeCode(XKeysymToKeycode(disp, XK_Return))));
    charToTypeCodeMap.insert(pair<char, TypeCode>('\x1b', TypeCode(XKeysymToKeycode(disp, XK_Escape))));
    charToTypeCodeMap.insert(pair<char, TypeCode>('\x08', TypeCode(XKeysymToKeycode(disp, XK_BackSpace))));
    charToTypeCodeMap.insert(pair<char, TypeCode>('\t', TypeCode(XKeysymToKeycode(disp, XK_Tab))));
    charToTypeCodeMap.insert(pair<char, TypeCode>('\xff', TypeCode(XKeysymToKeycode(disp, XK_Delete))));
    charToTypeCodeMap.insert(pair<char, TypeCode>('\x14', TypeCode(XKeysymToKeycode(disp, XK_Scroll_Lock))));
	/////////////////////////////////////////////////////////////////////////////////////
	const int AltGrMask = Mod5Mask;
    int minKeyCode, maxKeyCode;
	XDisplayKeycodes(disp, &minKeyCode, &maxKeyCode);
	XKeyEvent ev;
	ev.display = disp;
    ev.type = KeyPress;
	KeySym currSym;
	int haveChars;
	char buf[32];
	const int vKeysMask[3] = {0, ShiftMask, AltGrMask};
	for (int keyc = minKeyCode; keyc <= maxKeyCode; ++keyc) {
		ev.keycode = keyc;
		for (int i = 0; i < 3; ++i) {
			currSym = 0;
			ev.state = vKeysMask[i];
			haveChars = XLookupString(&ev, buf, sizeof(buf), &currSym, NULL);
			if (currSym && haveChars && char(currSym) == buf[0]) {
				charToTypeCodeMap.insert(pair<char, TypeCode>(buf[0], TypeCode(keyc, vKeysMask[i])));
			}
		}
	}
	TrFVar(charToTypeCodeMap.size());
	return charToTypeCodeMap;
}

BOOL FetchTitle(Display *dpy, Window w, char ** winname)
{
    TrFu;
    int    status;
    XTextProperty text_prop = {0};
    char **list;
    int    num;

    status = XGetWMName(dpy, w, &text_prop);
    if (!status || !text_prop.value || !text_prop.nitems) {
        return FALSE;
    }
    status = XmbTextPropertyToTextList(dpy, &text_prop, &list, &num);
    if (status < Success || !num || !*list) {
        XFree(text_prop.value);
        XFreeStringList(list);
        return FALSE;
    }
    *winname = (char *)strdup(*list);
    XFree(text_prop.value);
    XFreeStringList(list);
    return TRUE;
}

inline unsigned GetSize(Display *dpy, Window wnd, Window root, bool bWidthOrHeight)
{
    int x, y;
    unsigned int width, height;
    unsigned int border_width, depth;

    if (!XGetGeometry(dpy, wnd, &root, &x, &y, &width, &height, &border_width, &depth)) {
        return 0;
    }
    return bWidthOrHeight? width: height;
}

unsigned char *GetWindowAtomProperty(Display *dpy, Window wnd, Atom atom,
                                     unsigned long *itemNum, Atom *type = NULL, int *size = NULL) {
    Atom actual_type;
    int actual_format = 0, status = 0;
    unsigned long nItems = 0, bytes_after = 0;
    unsigned char *prop = NULL;

    status = XGetWindowProperty(dpy, wnd, atom, 0, (~0L), False, AnyPropertyType, &actual_type,
                              &actual_format, &nItems, &bytes_after, &prop);

    TrRet(status == BadWindow || status != Success, NULL);
    *itemNum = nItems;
    if (type) {
        *type = actual_type;
    }
    if (size) {
        *size = actual_format;
    }
    return prop;
}

inline void Click(unsigned delay, const SPoint &pt, const SWinInfo& wi, unsigned but)
{
    // TrFu;
    XButtonEvent xbpe = {0};
    int dummy;
    unsigned mask = 0;
    Window dummyWin;
    int rootWndX = 0, rootWndY = 0, wndX = pt.m_x, wndY = pt.m_y;
    TrVoid(!XQueryPointer(wi.m_disp, wi.m_hRoot, &dummyWin, &dummyWin, &dummy, &dummy, &dummy, &dummy, &mask));
    TrVoid(!XTranslateCoordinates(wi.m_disp, wi.m_hWnd, wi.m_hRoot,
                            wndX, wndY, &rootWndX, &rootWndY, &dummyWin));
    /*XSetInputFocus(wi.m_disp, wi.m_hWnd, RevertToPointerRoot, CurrentTime);
    XSync(wi.m_disp, False);*/
    XWarpPointer(wi.m_disp, None, wi.m_hRoot, 0, 0, 0, 0, rootWndX, rootWndY);
    XFlush(wi.m_disp);
    switch(but) {
        case 1:  xbpe.state = Button1MotionMask; break;
        case 2:  xbpe.state = Button2MotionMask; break;
        case 3:  xbpe.state = Button3MotionMask; break;
        case 4:  xbpe.state = Button4MotionMask; break;
        case 5:  xbpe.state = Button5MotionMask; break;
    }
    xbpe.window = wi.m_hWnd;
    xbpe.button = but;
    xbpe.display = wi.m_disp;
    xbpe.root = wi.m_hRoot;
    xbpe.x = wndX;
    xbpe.y = wndY;
    xbpe.x_root = rootWndX;
    xbpe.y_root = rootWndY;
    xbpe.same_screen = True;
    xbpe.subwindow = None;
    xbpe.time = CurrentTime;
    xbpe.type = ButtonPress;
    TrVoid(!XSendEvent(wi.m_disp, wi.m_hWnd, True, ButtonPressMask, (XEvent *)&xbpe));
    XFlush(wi.m_disp);
    CPrc::SleepMs(delay);
    xbpe.type = ButtonRelease;
    switch(but) {
        case 1: xbpe.state |= Button1MotionMask; break;
        case 2: xbpe.state |= Button2MotionMask; break;
        case 3: xbpe.state |= Button3MotionMask; break;
        case 4: xbpe.state |= Button4MotionMask; break;
        case 5: xbpe.state |= Button5MotionMask; break;
    }
    // xbpe.state = mask;
    TrVoid(!XSendEvent(wi.m_disp, wi.m_hWnd, True, ButtonPressMask, (XEvent *)&xbpe));
    XFlush(wi.m_disp);
}

inline void pressedMove(unsigned delay, const SPoint &pt, const SWinInfo& wi, unsigned but)
{
	XTestFakeButtonEvent(wi.m_disp, but, True, CurrentTime);
	CPrc::SleepMs(delay);
	int dummy;
    unsigned mask = 0;
    Window dummyWin;
    int rootWndX = 0, rootWndY = 0;
    TrVoid(!XQueryPointer(wi.m_disp, wi.m_hRoot, &dummyWin, &dummyWin, &dummy, &dummy, &rootWndX, &rootWndY, &mask));
	rootWndX += pt.m_x;
	rootWndY += pt.m_y;
	XWarpPointer(wi.m_disp, None, wi.m_hRoot, 0, 0, 0, 0, rootWndX, rootWndY);
	CPrc::SleepMs(delay);
	XTestFakeButtonEvent(wi.m_disp, but, False, CurrentTime);
}

void CWndBase::InitKeyNameToKey()
{
    s_KeyNameToKey[_T("Shift")] =       XK_Shift_L; 
    s_KeyNameToKey[_T("Ctrl")] =        XK_Control_L;
    s_KeyNameToKey[_T("Alt")] =         XK_Alt_L;   
    s_KeyNameToKey[_T("LeftShift")] =   XK_Shift_L;
    s_KeyNameToKey[_T("RightShift")] =  XK_Shift_R;
    s_KeyNameToKey[_T("LeftCtrl")] =    XK_Control_L;
    s_KeyNameToKey[_T("RightCtrl")] =   XK_Control_R;
    s_KeyNameToKey[_T("LeftAlt")] =     XK_Alt_L;
    s_KeyNameToKey[_T("RightAlt")] =    XK_Alt_R;
    s_KeyNameToKey[_T("NumLock")] =     XK_Num_Lock;
    s_KeyNameToKey[_T("ScrolLock")] =   XK_Scroll_Lock;
    s_KeyNameToKey[_T("Tab")] =         XK_Tab;
    s_KeyNameToKey[_T("Back")] =        XK_BackSpace;
    s_KeyNameToKey[_T("Clear")] =       XK_Clear;
    s_KeyNameToKey[_T("Return")] =      XK_Return;
    s_KeyNameToKey[_T("Space")] =       XK_space;
    s_KeyNameToKey[_T("Pause")] =       XK_Pause;
    s_KeyNameToKey[_T("Escape")] =      XK_Escape;
    s_KeyNameToKey[_T("PageUp")] =      XK_Page_Up;
    s_KeyNameToKey[_T("PageDown")] =    XK_Page_Down;
    s_KeyNameToKey[_T("Home")] =        XK_Home;
    s_KeyNameToKey[_T("End")] =         XK_End;
    s_KeyNameToKey[_T("KeyUp")] =       XK_Up;
    s_KeyNameToKey[_T("KeyDown")] =     XK_Down;
    s_KeyNameToKey[_T("KeyLeft")] =     XK_Left;
    s_KeyNameToKey[_T("KeyRight")] =    XK_Right;
    s_KeyNameToKey[_T("Print")] =       XK_Print;
    s_KeyNameToKey[_T("PrintScreen")] = XK_Print;
    s_KeyNameToKey[_T("CapsLock")] =    XK_Caps_Lock;
    s_KeyNameToKey[_T("CtrlBreak")] =   XK_Cancel;
    s_KeyNameToKey[_T("Insert")] =      XK_Insert;
    s_KeyNameToKey[_T("Delete")] =      XK_Delete;
    s_KeyNameToKey[_T("Plus")] =        XK_plus;
    s_KeyNameToKey[_T("Minus")] =       XK_minus;
    TCHAR buf[3] = {0};
    buf[0] = _T('F');
    for (int i = 1; i < 13; ++i) {
        buf[1] = _T('0') + i;
        s_KeyNameToKey[buf] = 0xffbd + i;
    }
}

BOOL CWndBase::Init()
{
    // TrFu;
    InitKeyNameToKey();
    char *display_name = getenv("DISPLAY");
    Display* disp = XOpenDisplay(display_name);
    TrRet(!disp, FALSE);
    m_dispList.push_back(disp);
    return TRUE;
}

void CWndBase::Clean()
{
    //TrFu;
    LOOP_DISP(iter) {
        XCloseDisplay(*iter);
    }
}

unsigned CWndBase::GetText(Str& back)
{
    //TrFu;
    XSync(m_winInfo.m_disp, False);
    Atom xa_utf8_string = XInternAtom(m_winInfo.m_disp, "UTF8_STRING", False);
    Atom mp_atom = XInternAtom(m_winInfo.m_disp, "MP_DATA_ATOM", False);
    unsigned cc = 0;
    int format, result;
    unsigned long len, bytesLeft, dummy;
    unsigned char *data;
    Atom type;
    XEvent ev;
    TypeCode currCode;
    list<TypeCode>::iterator keyCodesIter, keyCodesIterEnd;
    list<TypeCode> keyCodes;
    memset(&currCode, 0, sizeof(TypeCode));
    currCode.m_keyEx = XKeysymToKeycode(m_winInfo.m_disp, XK_Control_L);
    currCode.m_key = XKeysymToKeycode(m_winInfo.m_disp, XK_Home);
    keyCodes.push_back(currCode);
    memset(&currCode, 0, sizeof(TypeCode));
    currCode.m_keyEx2 = XKeysymToKeycode(m_winInfo.m_disp, XK_Control_L);
    currCode.m_keyEx = XKeysymToKeycode(m_winInfo.m_disp, XK_Shift_L);
    currCode.m_key = XKeysymToKeycode(m_winInfo.m_disp, XK_End);
    keyCodes.push_back(currCode);
    //
    keyCodesIterEnd = keyCodes.end();
    for (keyCodesIter = keyCodes.begin(); keyCodesIter != keyCodesIterEnd; ++keyCodesIter) {
        if ((*keyCodesIter).m_keyEx2) {
            XTestFakeKeyEvent(m_winInfo.m_disp, (*keyCodesIter).m_keyEx2, True, CurrentTime);
            XFlush(m_winInfo.m_disp);
        }
        if ((*keyCodesIter).m_keyEx) {
            XTestFakeKeyEvent(m_winInfo.m_disp, (*keyCodesIter).m_keyEx, True, CurrentTime);
            XFlush(m_winInfo.m_disp);
        }
        XTestFakeKeyEvent(m_winInfo.m_disp, (*keyCodesIter).m_key, True, CurrentTime);
        XFlush(m_winInfo.m_disp);
        if ((*keyCodesIter).m_keyEx2) {
            XTestFakeKeyEvent(m_winInfo.m_disp, (*keyCodesIter).m_keyEx2, False, CurrentTime);
            XFlush(m_winInfo.m_disp);
        }
        if ((*keyCodesIter).m_keyEx) {
            XTestFakeKeyEvent(m_winInfo.m_disp, (*keyCodesIter).m_keyEx, False, CurrentTime);
            XFlush(m_winInfo.m_disp);
        }
        XTestFakeKeyEvent(m_winInfo.m_disp, (*keyCodesIter).m_key, False, CurrentTime);
        XFlush(m_winInfo.m_disp);
    }
    XSync(m_winInfo.m_disp, False);
    CPrc::SleepMs(500);
    // It is a magic, why i need this ??? if i have it ???
    Display *dpy = XOpenDisplay(NULL);
    XSync(dpy, False);
    Window wnd = XCreateWindow(dpy, DefaultRootWindow(dpy), -10, -10, 1, 1, 0, 0,
                               InputOnly, CopyFromParent, 0, NULL);
    XSelectInput(dpy, wnd, StructureNotifyMask);
    XFlush(dpy);
    XSelectInput(dpy, wnd, StructureNotifyMask+ExposureMask);
    XFlush(dpy);
    Window primSelWnd = XGetSelectionOwner(dpy, XA_PRIMARY);
    if (primSelWnd == None) {
        Tr("primSelWnd == None");
        goto GetText_exit;
    }
    XConvertSelection(dpy, XA_PRIMARY, xa_utf8_string, mp_atom, wnd, CurrentTime);
    XFlush(dpy);
    XNextEvent(dpy, &ev);
    if (ev.type != SelectionNotify || ev.xselection.property != mp_atom) {
        Tr("ev.type != SelectionNotify || ev.xselection.property != mp_atom");
        Tr("ev.xselection.property: "<<(int)ev.xselection.property<<" mp_atom: "<<(int)mp_atom);
        goto GetText_exit;
    }
    XGetWindowProperty(dpy, wnd, mp_atom, 0, 0, 0, AnyPropertyType,
                       &type, &format, &len, &bytesLeft, &data);
    //TrVar(bytesLeft);
    if (bytesLeft > 0) {
        result = XGetWindowProperty(dpy, wnd, ev.xselection.property, 0, bytesLeft,
                                    0, AnyPropertyType, &type, &format, &len, &dummy, &data);
        if (result == Success) {
            back = StrAToStr(StrA((const char*)data));
            cc = bytesLeft;
        }
        if (data) {
            XFree(data);
        }
    }
GetText_exit:
    XDestroyWindow(dpy, wnd);
    XCloseDisplay(dpy);
    memset(&currCode, 0, sizeof(TypeCode));
    currCode.m_key = XKeysymToKeycode(m_winInfo.m_disp, XK_Right);
    XTestFakeKeyEvent(m_winInfo.m_disp, currCode.m_key, True, CurrentTime);
    XFlush(m_winInfo.m_disp);
    XTestFakeKeyEvent(m_winInfo.m_disp, currCode.m_key, False, CurrentTime);
    XFlush(m_winInfo.m_disp);
    return cc;
}

BOOL CWndBase::SetText(LPCTSTR txt)
{
    TrFu;
    Tr("Not implemented!");
    return FALSE;
}

BOOL CWndBase::TypeText(LPCTSTR txt)
{
	TrFu;
	static const map<char, TypeCode> s_charToTypeCodeMap = GetCharToTypeCodeMap(m_winInfo.m_disp);
	StrA txtStrA = StrToStrA(txt);
    size_t txtLen = txtStrA.length();
    if (!txtLen) {
        return FALSE;
    }
	TrVar(txtLen);
	XKeyEvent event;
	event.display     = m_winInfo.m_disp;
    event.window      = m_winInfo.m_hWnd;
    event.root        = m_winInfo.m_hRoot;
    event.subwindow   = None;
    event.time        = CurrentTime;
    event.x           = 1;
    event.y           = 1;
    event.x_root      = 1;
    event.y_root      = 1;
	event.same_screen = True;
	map<char, TypeCode>::const_iterator it, itEnd = s_charToTypeCodeMap.end();
    for (size_t i = 0; i < txtLen; ++i) {
		it = s_charToTypeCodeMap.find(txtStrA[i]);
		if (it != itEnd) {
			const TypeCode &currCode = it->second;
			event.keycode = currCode.m_key;
			event.state   = currCode.m_keyEx;
			event.type = KeyPress;
			XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
			event.type = KeyRelease;
			XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
			XFlush(m_winInfo.m_disp);
		}
	}
    return TRUE;
}

BOOL CWndBase::KeyCodeDo(LPCTSTR code, unsigned flag)
{
    //TrFu;
    //Tr("code: "<<code<<", flag: "<<flag);
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
    BOOL back = XTestFakeKeyEvent(m_winInfo.m_disp, XKeysymToKeycode(m_winInfo.m_disp, vKey), flag, CurrentTime);
    XFlush(m_winInfo.m_disp);
    TrRet(!back, FALSE);
    return TRUE;
}

BOOL CWndBase::KeyCodeType(LPCTSTR key)
{
    BOOL back = KeyCodeDo(key, True);
    CPrc::SleepMs(20);
    return back && KeyCodeDo(key, False);
}

BOOL CWndBase::KeyCodeDown(LPCTSTR key)
{
  CPrc::SleepMs(20);
  return KeyCodeDo(key, True);
}

BOOL CWndBase::KeyCodeUp(LPCTSTR key)
{
    CPrc::SleepMs(20);
    return KeyCodeDo(key, False);
}

BOOL CWndBase::GetTitle(Str& back)
{
    TrRet(!m_hWnd, FALSE);
    char * winname = NULL;
    back.clear();
    if (FetchTitle(m_winInfo.m_disp, m_winInfo.m_hWnd, &winname)) {
        back = StrAToStr(StrA(winname));
        free(winname);
        return TRUE;
    }
    return FALSE;
}

BOOL CWndBase::Activate()
{
    TrRet(!m_hWnd, FALSE);
    XEvent xEvent;
    XWindowAttributes xWinAttr;
    memset(&xEvent, 0, sizeof(xEvent));
    xEvent.type = ClientMessage;
    xEvent.xclient.display = m_winInfo.m_disp;
    xEvent.xclient.window = m_winInfo.m_hWnd;
    xEvent.xclient.message_type = XInternAtom(m_winInfo.m_disp, "_NET_ACTIVE_WINDOW", False);
    xEvent.xclient.format = 32;
    xEvent.xclient.data.l[0] = 2L; /* 2 == Message from a window pager */
    xEvent.xclient.data.l[1] = CurrentTime;
    XGetWindowAttributes(m_winInfo.m_disp, m_winInfo.m_hWnd, &xWinAttr);
    
    if (XSendEvent(m_winInfo.m_disp, xWinAttr.screen->root, False,
                   SubstructureNotifyMask | SubstructureRedirectMask, &xEvent)) {
        XFlush(m_winInfo.m_disp);
        return TRUE;
    }
    // Tr("XSendEvent return false!");
    return FALSE;
}

void CWndBase::UpdateWindow()
{
}

int CWndBase::GetWndWidth()
{
    TrRet(!m_hWnd, 0);
    return (int)GetSize(m_winInfo.m_disp, m_winInfo.m_hWnd, m_winInfo.m_hRoot, true);
}

int CWndBase::GetWndHeight()
{
    TrRet(!m_hWnd, 0);
    return (int)GetSize(m_winInfo.m_disp, m_winInfo.m_hWnd, m_winInfo.m_hRoot, false);
}

SRect CWndBase::GetWndRect()
{
    SRect rc;
    XWindowAttributes attr;
    Window unused_child;
    TrRet(!XGetWindowAttributes(m_winInfo.m_disp, m_winInfo.m_hWnd, &attr), rc);
    rc.m_width = attr.width;
    rc.m_height = attr.height;
    TrRet(!m_winInfo.m_isDeskTop && attr.root != m_winInfo.m_hRoot, rc);
    XTranslateCoordinates(m_winInfo.m_disp, m_winInfo.m_hWnd, attr.root,
                            attr.x, attr.y, &rc.m_posX, &rc.m_posY, &unused_child);
    return rc;
}

BOOL CWndBase::SetWndRect(const SRect &rc)
{
    XUnmapWindow(m_winInfo.m_disp, m_winInfo.m_hWnd);
    XMapWindow(m_winInfo.m_disp, m_winInfo.m_hWnd);
    TrRet(!XMoveResizeWindow(m_winInfo.m_disp, m_winInfo.m_hWnd, rc.m_posX, rc.m_posY, rc.m_width, rc.m_height), FALSE);
    XSync(m_winInfo.m_disp, False);
    return TRUE;
}

PrcId CWndBase::GetWindowPrcId()
{
    //TrFu;
    TrRet(!m_hWnd, 0);
    PrcId pid = 0;
    unsigned long itemsCount = 0;
    Atom atom_NET_WM_PID = XInternAtom(m_winInfo.m_disp, "_NET_WM_PID", False);
    unsigned char *data = GetWindowAtomProperty(m_winInfo.m_disp, m_winInfo.m_hWnd, 
                                                atom_NET_WM_PID, &itemsCount);
    //TrVar(itemsCount);
    if (data && itemsCount > 0) {
        pid = (int) *((unsigned long *)data);
        free(data);
        return pid;
    }
    //TrVar(m_winInfo.m_hWnd);
    StrA envPathStr, currDir;
    DIR *dirHandle = opendir("/proc");
    struct dirent *dirEntry = NULL;
    int fHandle = 0;
    TrRet(!dirHandle, 0);
    while (0 != (dirEntry = readdir(dirHandle))) {
        envPathStr = "/proc/";
        envPathStr += dirEntry->d_name;
        envPathStr += "/environ";
        // TrVar(dirEntry->d_name);
        CPath envPath(StrAToStr(envPathStr).c_str());
        if (envPath.IsFile()) {
            // TrVar((const TCHAR*)envPath);
            fHandle = SFile::Open((const TCHAR*)envPath);
            // TrStr(envPathStr);
            if (fHandle != MP_BAD_FILE) {
                char buff[MP_MAX_PATH] = {0};
                int count = SFile::Read(fHandle, buff, MP_MAX_PATH - 1);
                HWND winId = 0;
                while (count > 0) {
                    for (int i = 0; i < count; i += strlen(&buff[i]) + 1) {
                        if (!winId && !memcmp(&buff[i], "WINDOWID=", 9)) {
                            winId = (HWND)atoi(&buff[i + 9]);
                        }
                        if (winId) {
                             // TrStr(envPathStr);
                             // TrVar(winId);
                            // winId = 0;
                            if (winId == m_winInfo.m_hWnd) {
                                TrP;
                                pid = atoi(dirEntry->d_name);
                                goto GetWindowPrcId__Success;
                            }
                            else {
                                goto GetWindowPrcId_nextDir;
                            }
                        }
                    }
                    count = SFile::Read(fHandle, buff, MP_MAX_PATH - 1);
                }
                GetWindowPrcId_nextDir:
                    SFile::Close(fHandle);
            }
        }
    }
    closedir(dirHandle);
    // TrVar(pid);
    return pid;
    GetWindowPrcId__Success:
        SFile::Close(fHandle);
        closedir(dirHandle);
        // Tr("GetWindowPrcId__Success, pid: "<<pid);
        return pid;
}

BOOL CWndBase::OnCreate()
{
    return FALSE;
}

BOOL CWndBase::IsTopWindow(const SWinInfo &inf)
{
    int    status;
    XTextProperty text_prop = {0};
    char **list;
    int    num;

    status = XGetWMName(inf.m_disp, inf.m_hWnd, &text_prop);
    if (!status || !text_prop.value || !text_prop.nitems) {
        return FALSE;
    }
    status = XmbTextPropertyToTextList(inf.m_disp, &text_prop, &list, &num);
    if (status < Success || !num || !*list) {
        XFree(text_prop.value);
        XFreeStringList(list);
        return FALSE;
    }
    XFree(text_prop.value);
    XFreeStringList(list);
    return TRUE;
}

BOOL CWndBase::IsWindow(HWND hWnd)
{
    TrFu;
    Tr("Not implemented!");
    return FALSE;
}

WinInfoList CWndBase::FindWindowByTitleText(const TCHAR *title, EFindWindowWhat eWhat)
{
    TrFu;
    StrA sTitle = StrToStrA(Str(title));
    WinInfoList backList;
    Display* display = NULL;
    Atom a = 0;
    Atom actualType = 0;
    int format = 0;
    unsigned long numItems = 0, bytesAfter = 0;
    unsigned char *data =0;
    Window rootWnd = 0;
    int defScreenNum = 0;
    char *winname = NULL;
    LOOP_DISP(iter) {
        display = *iter;
        a = XInternAtom(display, "_NET_CLIENT_LIST" , true);
        defScreenNum = DefaultScreen(display);
        rootWnd = RootWindow(display, defScreenNum);
        BOOL status = XGetWindowProperty(display, rootWnd, a, 0L, (~0L), false, AnyPropertyType,
                                    &actualType, &format, &numItems, &bytesAfter, &data);
        //TrVar(numItems);
        if (status >= Success && numItems)
        {
            // success - we have data: Format should always be 32:
            if (format != 32)
            {
                Tr("format != 32, we are continue !");
                continue;
            }
            // cast to proper format, and iterate through values:
            unsigned long *array = (unsigned long*) data;
            for (unsigned k = 0; k < numItems; ++k)
            {
                winname = NULL;
                // get window Id:
                Window w = (Window) array[k];
                if (!w)
                {
                    Tr("!w");
                    continue;
                }
                if (eWhat == eWithTitleExakt && FetchTitle(display, w, &winname)) {
                    if (!strcmp(winname, sTitle.c_str())) {
                        SWinInfo inf(display, defScreenNum, rootWnd, w);
#ifdef _MPTRACE_
                        inf.Trace();
#endif	
                        backList.push_back(inf);
                    }
                }
                else if (eWhat == eWithTitleWithText && FetchTitle(display, w, &winname)) {
                    if (strstr(winname, sTitle.c_str())) {
                        SWinInfo inf(display, defScreenNum, rootWnd, w);
                        backList.push_back(inf);
                    }
                }
                else if (FetchTitle(display, w, &winname)) {
                    // TrVar(winname);
                    SWinInfo inf(display, defScreenNum, rootWnd, w);
                    backList.push_back(inf);
                }
                TrVar(winname);
                free(winname);
            }
        }
    }
    return backList;
}

SWinInfo CWndBase::GetActivWindow()
{
    TrFu;
    SWinInfo inf;
    Window window;
    int revertTo = 0;
    Display *display = *CWndBase::m_dispList.begin();
    TrRet(!XGetInputFocus(display, &window, &revertTo), inf);
    Screen *screen = ScreenOfDisplay(display, 0);
    inf = SWinInfo(display, 0, RootWindowOfScreen(screen), window, 0);
    CWndBase tmpWnd(inf);
    WinInfoList allPar = tmpWnd.GetAllParent();
    SWinInfo infAct;
    TrRet(!allPar.size(), infAct);
    infAct = *allPar.rbegin();
    return infAct;
}

WinInfoList CWndBase::GetAllTopWindow()
{
    TrFu;
    return FindWindowByTitleText(NULL, eAllTop);
}

SWinInfo CWndBase::GetWindowFromPoint(const SPoint &pt)
{
    SWinInfo inf;
    Display *display = NULL;
    Window window, root;
    int dummyInt;
    unsigned dummyUInt;
    LOOP_DISP(iter) {
        display = *iter;
        int screencount = ScreenCount(display);
        for (int i = 0; i < screencount; i++) {
            Screen *screen = ScreenOfDisplay(display, i);
            if (XQueryPointer(display, RootWindowOfScreen(screen), &root, &window,
                              &dummyInt, &dummyInt, &dummyInt, &dummyInt, &dummyUInt)) {
                inf = SWinInfo(display, i, root, window, 0);
                break;
            }
        }
    }
    return inf;
}

WinInfoList CWndBase::GetAllChildByClassName(const TCHAR *title)
{
    TrFu;
    WinInfoList lst;
    Tr("Not implemented!");
    return lst;
}

WinInfoList CWndBase::GetAllChild()
{
    //TrFu;
    WinInfoList backList;
    stack<HWND> wndStack;
    HWND currWnd = NULL;
    Display *currDisplay = m_winInfo.m_disp;
    int currScrNum = m_winInfo.m_scrNum;
    Window *currChilds = NULL, currRoot = NULL, currParent = NULL;
    unsigned currChildsCount = 0;
    wndStack.push(m_hWnd);
    XFlush(currDisplay);
    while(!wndStack.empty()) {
        // TrP;
        currWnd = wndStack.top();
        wndStack.pop();
        // TrVar(currWnd);
        if (XQueryTree(currDisplay, currWnd, &currRoot, &currParent, &currChilds, &currChildsCount)) {
            SWinInfo inf(currDisplay, currScrNum, currRoot, currWnd, currParent);
//#ifdef _MPTRACE_
//			inf.Trace();
//
//#endif	
            backList.push_back(inf);
            // TrVar(currChildsCount);
            if (currChildsCount > 0) {
                for (unsigned i = 0; i < currChildsCount; ++i) {
                    wndStack.push(currChilds[i]);
                }
                XFree(currChilds);
            }
        }
    }
    //TrFVar(backList.size());
    return backList;
}

WinInfoList CWndBase::GetAllZOrder()
{
	WinInfoList lst;
    Tr("Not implemented!");
    return lst;
}

WinInfoList CWndBase::GetAllParent()
{
    //TrFu;
    //TrVar(m_winInfo.m_hWnd);
    //TrVar(m_winInfo.m_hRoot);
    WinInfoList backList;
    Display *currDisplay = m_winInfo.m_disp;
    Window *currChilds = NULL, currRoot = NULL, currParent = NULL, currWnd =NULL;
    unsigned currChildsCount = 0;
    XFlush(currDisplay);
    TrRet(!XQueryTree(currDisplay,  m_winInfo.m_hWnd, &currRoot, &currParent, &currChilds, &currChildsCount) && currChildsCount > 0, backList);
    XFree(currChilds);
    TrRet(currRoot == currParent, backList);
    currWnd = currParent;
    TrRet(!XQueryTree(currDisplay,  currWnd, &currRoot, &currParent, &currChilds, &currChildsCount) && currChildsCount > 0, backList);
    XFree(currChilds);
    do {
        SWinInfo inf(currDisplay, m_winInfo.m_scrNum, currRoot, currWnd, currParent);
        backList.push_back(inf);
        //TrVar(currWnd);
        currWnd = currParent;
        if (XQueryTree(currDisplay, currWnd, &currRoot, &currParent, &currChilds, &currChildsCount)
                && currChildsCount > 0) {
            XFree(currChilds);
        }
        else {
            break;
        }
    } while (currWnd != m_winInfo.m_hRoot);
    return backList;
}

void CWndBase::ClickL(unsigned delay, const SPoint &pt)
{
    Click(delay, pt, m_winInfo, 1);
}

void CWndBase::ClickExL(const SPoint &pt)
{
	//TrFu;
	XTestFakeButtonEvent(m_winInfo.m_disp, 1, True, CurrentTime);
    CPrc::SleepMs(100);
	XTestFakeButtonEvent(m_winInfo.m_disp, 1, False, CurrentTime);
}

void CWndBase::ClickR(unsigned delay, const SPoint &pt)
{
    Click(delay, pt, m_winInfo, 3);
}

void CWndBase::ClickExR(const SPoint &pt)
{
	XTestFakeButtonEvent(m_winInfo.m_disp, 3, True, CurrentTime);
    CPrc::SleepMs(100);
	XTestFakeButtonEvent(m_winInfo.m_disp, 3, False, CurrentTime);
}

void CWndBase::ClickDL(const SPoint &pt)
{
    //TrFu;
    Click(100, pt, m_winInfo, 1);
    Click(100, pt, m_winInfo, 1);
    //For a double click to occur, the second button press must occur within 1/4 of a second of the first. 
    //For a triple click to occur, the third button press must also occur within 1/2 second of the first button press.
}

void CWndBase::ClickExDL(const SPoint &pt)
{
	//TrFu;
	XTestFakeButtonEvent(m_winInfo.m_disp, 1, True, CurrentTime);
    CPrc::SleepMs(50);
	XTestFakeButtonEvent(m_winInfo.m_disp, 1, False, CurrentTime);
	CPrc::SleepMs(50);
	XTestFakeButtonEvent(m_winInfo.m_disp, 1, True, CurrentTime);
    CPrc::SleepMs(50);
	XTestFakeButtonEvent(m_winInfo.m_disp, 1, False, CurrentTime);
}

void  CWndBase::MoveMouseTo(const SPoint &pt)
{
    //TrFu;
    Window dummyWin;
    int rootWndX = 0, rootWndY = 0, wndX = pt.m_x, wndY = pt.m_y;
    TrVoid(!XTranslateCoordinates(m_winInfo.m_disp, m_winInfo.m_hWnd, m_winInfo.m_hRoot,
                            wndX, wndY, &rootWndX, &rootWndY, &dummyWin));
    XWarpPointer(m_winInfo.m_disp, None, m_winInfo.m_hRoot, 0, 0, 0, 0, rootWndX, rootWndY);
    XFlush(m_winInfo.m_disp);
    XSync(m_winInfo.m_disp, False);
}

void CWndBase::MoveLMouse(const SPoint &pt)
{
	//TrFu;
	pressedMove(500, pt, m_winInfo, 1);
}

void CWndBase::MoveRMouse(const SPoint &pt)
{
	//TrFu;
	pressedMove(500, pt, m_winInfo, 3);
}

Str CWndBase::GetClassName()
{
    Str back = _T("Unknown");
    XWindowAttributes attr;
    TrRet(!XGetWindowAttributes(m_winInfo.m_disp, m_winInfo.m_hWnd, &attr), back);
    if (attr.c_class == InputOutput) {
        back = _T("InputOutput");
    }
    else if (attr.c_class == InputOnly) {
        back = _T("InputOnly");
    }
    else {
        back = _T("Unknown");
    }

    return back;
}

WinInfoList CWndBase::GetAllChildFromPoint(const SPoint &pt)
{
    //TrFu;
    //TrVar(m_winInfo.m_hWnd);
    WinInfoList backList;
    Window root_return, child_return;
    int dummyInt;
    unsigned dummyUInt;
    bool queryBack = false;
    backList.push_back(m_winInfo);
    SRect myWndRect = GetWndRect();
    XWarpPointer(m_winInfo.m_disp, m_winInfo.m_hWnd, m_winInfo.m_hWnd, 
                 myWndRect.m_posX, myWndRect.m_posY, myWndRect.m_width, myWndRect.m_height,
                 pt.m_x, pt.m_y);
    queryBack = XQueryPointer(m_winInfo.m_disp, m_winInfo.m_hWnd, &root_return, &child_return,
                              &dummyInt, &dummyInt, &dummyInt, &dummyInt, &dummyUInt);
    Display *currDisp = m_winInfo.m_disp;
    int currScrNum = m_winInfo.m_scrNum;
    Window currWnd = m_winInfo.m_hWnd;
    while (child_return != None && queryBack) {
        SWinInfo inf = SWinInfo(currDisp, currScrNum, root_return, child_return, currWnd);
        //TrVar(child_return);
        backList.push_back(inf);
        currWnd = child_return;
        queryBack = XQueryPointer(currDisp, currWnd, &root_return, &child_return,
                              &dummyInt, &dummyInt, &dummyInt, &dummyInt, &dummyUInt);
    }
    return backList;
}

BOOL CWndBase::GetScreenShot(void *&pScreenData, size_t &screenDataSize)
{
    TrFu;
    TrRet(pScreenData, FALSE);
    SRect wndRect = GetWndRect();
    TrRet(wndRect.m_width < 1 || wndRect.m_height < 1, FALSE);
    XImage *pImg = XGetImage(m_winInfo.m_disp, m_winInfo.m_hWnd, 0, 0, 
                wndRect.m_width, wndRect.m_height, AllPlanes, ZPixmap);
    TrRet(!pImg, FALSE);
    BOOL back = FALSE;
    int width = wndRect.m_width, height = wndRect.m_height;
    int x = 0, y = 0;
    unsigned char *img = NULL, *pCurrData = NULL;
    int filesize = 54 + 3*width*height + 3*(height-1);
    screenDataSize = (size_t)filesize;
    //TrVar(filesize);  
    unsigned long red_mask = pImg->red_mask;
    unsigned long green_mask = pImg->green_mask;
    unsigned long blue_mask = pImg->blue_mask;
    unsigned long pixel;
    unsigned char blue, green, red;
    unsigned char bmpfileheader[14] = {'B','M',0,0,0,0,0,0,0,0,54,0,0,0};
    unsigned char bmpinfoheader[40] = {40,0,0,0,0,0,0,0,0,0,0,0,1,0,24,0,0,0,0,0,
                                        0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0};
    unsigned char bmppad[3] = {0,0,0};
    bmpfileheader[ 2] = (unsigned char)(filesize    );
    bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
    bmpfileheader[ 4] = (unsigned char)(filesize>>16);
    bmpfileheader[ 5] = (unsigned char)(filesize>>24);

    bmpinfoheader[ 4] = (unsigned char)(width    );
    bmpinfoheader[ 5] = (unsigned char)(width>> 8);
    bmpinfoheader[ 6] = (unsigned char)(width>>16);
    bmpinfoheader[ 7] = (unsigned char)(width>>24);
    bmpinfoheader[ 8] = (unsigned char)(height    );
    bmpinfoheader[ 9] = (unsigned char)(height>> 8);
    bmpinfoheader[10] = (unsigned char)(height>>16);
    bmpinfoheader[11] = (unsigned char)(height>>24);
    if (frameMP::ReAlloc(&img, 0, 3*width*height) && frameMP::ReAlloc(&pScreenData, 0, screenDataSize)) {
        for(int i = 0; i < width; ++i) {
            for(int j = 0; j < height; ++j) {
                x=i; 
                y=(height-1)-j;
                pixel = XGetPixel(pImg, x, y);
                blue = pixel & blue_mask;
                green = (pixel & green_mask) >> 8; 
                red = (pixel & red_mask) >> 16;
                img[(x+y*width)*3+0] = blue;
                img[(x+y*width)*3+1] = green;
                img[(x+y*width)*3+2] = red;
            }
        }
        TrVar(screenDataSize);
        pCurrData = (unsigned char*)pScreenData;
        memcpy(pCurrData, &bmpfileheader[0], 14);
        pCurrData += 14;
        memcpy(pCurrData, &bmpinfoheader[0], 40);
        pCurrData += 40;
        size_t bmppadSize = 0;
        for(int i = 0; i < height; ++i) {
            memcpy(pCurrData, img+(width*(height-i-1)*3), 3*width);
            pCurrData += 3*width;
            bmppadSize = (4-(width*3)%4)%4;
            memcpy(pCurrData, bmppad, bmppadSize);
            pCurrData += bmppadSize;
            
        }
        back = TRUE;
    }
    free(img);
    TrRet(!back, FALSE);
    return back;
}

SWinInfo CWndBase::GetDesktopWindow()
{
    Display* display = *m_dispList.begin();
    int defScreenNum = DefaultScreen(display);
    Window rootWnd = RootWindow(display, defScreenNum);
    SWinInfo inf(display, defScreenNum, rootWnd, rootWnd);
    inf.m_isDeskTop = true;
    return inf;
}

BOOL CWndBase::ShowDesktopWindow()
{
    XEvent xEvent;
    XWindowAttributes xWinAttr;
    SWinInfo deskTop = GetDesktopWindow();
    memset(&xEvent, 0, sizeof(xEvent));
    xEvent.type = ClientMessage;
    xEvent.xclient.display = deskTop.m_disp;
    xEvent.xclient.window = deskTop.m_hWnd;
    xEvent.xclient.message_type = XInternAtom(deskTop.m_disp, "_NET_SHOWING_DESKTOP", False);
    xEvent.xclient.format = 32;
    xEvent.xclient.data.l[0] = 1; /* 1 == Show */
    XGetWindowAttributes(deskTop.m_disp, deskTop.m_hWnd, &xWinAttr);
    BOOL back = XSendEvent(deskTop.m_disp, xWinAttr.screen->root, False,
                   SubstructureNotifyMask | SubstructureRedirectMask, &xEvent);
    if (back) {
        XFlush(deskTop.m_disp);
        return TRUE;
    }
    TrRet(!back, FALSE);
    return TRUE;
}
