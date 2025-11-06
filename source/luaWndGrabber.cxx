// 
// This file is part of VirtualGUser project.
// Copyright ©2017 Mykhaylo Poboynyy <mpoboyny@googlemail.com>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; version 2.1 (LGPL-2.1) of the License.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

// luaWndGrabber.cxx

#include <luaWndGrabber.hxx>
#include <process.h>

#define GRABWINDOWCLASS _T("ClickerMPMouseGrabWindow")
#define CLICKERMPGRABWNDPROC  _T("ClickerMPGrabWndProc")

static SPoint s_clickPt;
static BOOL s_lButClicked = FALSE;

CLuaWndGrabber::CLuaWndGrabber(CWndBase* wnd, BOOL bActivateWnd) 
            : m_wnd(wnd), m_bActivateWnd(bActivateWnd)
{}

CLuaWndGrabber::~CLuaWndGrabber()
{}

CLuaDesktopGrabber::CLuaDesktopGrabber(CWndBase* wnd, BOOL bActivateWnd) 
            : CLuaWndGrabber(wnd, bActivateWnd)
{}

CLuaDesktopGrabber::~CLuaDesktopGrabber()
{}

static LRESULT CALLBACK ClickerMPGrabWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_LBUTTONDOWN: {
		Tr("WM_LBUTTONDOWN");
		s_clickPt.m_x = LOWORD(lParam);
		s_clickPt.m_y = HIWORD(lParam);
		TrP;
		TrVar(s_clickPt.m_x);
		TrVar(s_clickPt.m_y);
		s_lButClicked = TRUE;
		PostQuitMessage(0);
		return 0;
	}
	case WM_KILLFOCUS: {
		Tr("WM_KILLFOCUS");
		if (!s_lButClicked) {
			PostQuitMessage(0);
		}
		break;
	}
	case WM_CHAR: {
		if (wParam == VK_ESCAPE) {
			Tr("WM_KILLFOCUS + VK_ESCAPE");
			PostQuitMessage(0);
			return 0;
		}
	}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);;
}

static ATOM RegGrabWnd(HINSTANCE hin)
{
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = 0;
	wcex.lpfnWndProc = (WNDPROC)ClickerMPGrabWndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 32;
	wcex.hInstance = hin;
	wcex.hIcon = NULL;
	wcex.hCursor = ::LoadCursor(NULL, IDC_CROSS);
	wcex.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = GRABWINDOWCLASS;
	wcex.hIconSm = NULL;
	return RegisterClassEx(&wcex);
}

BOOL CLuaWndGrabber::GetMouseClick(SPoint& clickPt)
{
	TrFu;
	static HINSTANCE s_hin = (HINSTANCE)GetModuleHandle(NULL);
	static ATOM s_regCls = RegGrabWnd(s_hin);
	TrRet(!s_regCls || !s_hin, FALSE);
	BOOL back = FALSE;
	if (m_bActivateWnd) {
		m_wnd->Activate();
	}
	SRect wndRt = m_wnd->GetWndRect();
	HWND currWnd = ::CreateWindow(GRABWINDOWCLASS, CLICKERMPGRABWNDPROC, WS_POPUP | WS_VISIBLE,
		wndRt.m_posX, wndRt.m_posY, wndRt.m_width, wndRt.m_height,
		NULL, NULL, s_hin, NULL);
	TrRet(!currWnd, FALSE);
	if (!::SetForegroundWindow(currWnd)) {
		Tr("SetForegroundWindow - FAILED, use SetWindowPos");
		::SetWindowPos(currWnd, HWND_TOP, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
		::SetWindowPos(*m_wnd, currWnd, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	s_lButClicked = FALSE;
	s_clickPt.Reset();
	::SetWindowLong(currWnd, GWL_EXSTYLE, GetWindowLong(currWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	// Make this window 30% alpha
	SetLayeredWindowAttributes(currWnd, 0, (255 * 30) / 100, LWA_ALPHA);
	BOOL bRet = FALSE;
	MSG  msg = { 0 };
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (bRet == -1) {
			DWORD err = GetLastError();
			Tr("Main loop error: " << err);
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	TrVar(msg.message);
	POINT pt = { s_clickPt.m_x, s_clickPt.m_y };
	if (!s_lButClicked) {
		goto GetMouseClick_exit;
	}
	::ClientToScreen(currWnd, &pt);
	RECT clRt;
	::GetWindowRect(*m_wnd, &clRt);
	back = PtInRect(&clRt, pt);
	::ScreenToClient(*m_wnd, &pt);
	clickPt.m_x = pt.x;
	clickPt.m_y = pt.y;
GetMouseClick_exit:
	::DestroyWindow(currWnd);
	TrVar(back);
	return back;
}

BOOL CLuaDesktopGrabber::GetMouseClick(SPoint& clickPt)
{
	TrFu;
	static HINSTANCE s_hin = (HINSTANCE)GetModuleHandle(NULL);
	static ATOM s_regCls = RegGrabWnd(s_hin);
	TrRet(!s_regCls || !s_hin, FALSE);
	BOOL back = FALSE;
	if (m_bActivateWnd) {
		m_wnd->Activate();
	}
	SRect wndRt = m_wnd->GetWndRect();
	HWND currWnd = ::CreateWindow(GRABWINDOWCLASS, CLICKERMPGRABWNDPROC, WS_POPUP | WS_VISIBLE,
		wndRt.m_posX, wndRt.m_posY, wndRt.m_width, wndRt.m_height,
		NULL, NULL, s_hin, NULL);
	TrRet(!currWnd, FALSE);
	::SetWindowLong(currWnd, GWL_EXSTYLE, GetWindowLong(currWnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	// Make this window 30% alpha
	::SetLayeredWindowAttributes(currWnd, 0, (255 * 30) / 100, LWA_ALPHA);
	// ?????????????????
	::SetForegroundWindow(currWnd);
	//TrRet(!::SetForegroundWindow(currWnd), FALSE);
	s_lButClicked = FALSE;
	s_clickPt.Reset();
	BOOL bRet = FALSE;
	MSG  msg = { 0 };
	while ((bRet = GetMessage(&msg, NULL, 0, 0)) != 0) {
		if (bRet == -1) {
			DWORD err = GetLastError();
			Tr("Main loop error: " << err);
		}
		else {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	TrVar(msg.message);
	POINT pt = { s_clickPt.m_x, s_clickPt.m_y };
	if (!s_lButClicked) {
		goto GetMouseClick_exit;
	}
	TrP;
	::ClientToScreen(currWnd, &pt);
	TrVar(pt.x);
	TrVar(pt.y);
	RECT clRt;
	::GetWindowRect(*m_wnd, &clRt);
	back = PtInRect(&clRt, pt);
	TrVar(back);
	::ScreenToClient(*m_wnd, &pt);
	clickPt.m_x = pt.x;
	clickPt.m_y = pt.y;
	TrVar(clickPt.m_x);
	TrVar(clickPt.m_y);
GetMouseClick_exit:
	::DestroyWindow(currWnd);
	TrVar(back);
	return back;
}
