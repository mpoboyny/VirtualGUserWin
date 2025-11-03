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

#if defined(MSWIN_OS)
#	include "luaWndGrabberWin.inl"
#elif defined(LINUX_OS)
#	include "luaWndGrabberLin.inl"
#endif
