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

// luaWndGrabber.hxx

#include <main.hxx>
#include <frameMP/wnd.hxx>

#ifndef _LUAWNDGRABBER_HXX_
#define _LUAWNDGRABBER_HXX_

class CLuaWndGrabber
{
protected:
    CWndBase* m_wnd;
    BOOL m_bActivateWnd;
    inline BOOL CreateGrabberWnd();
public:
    CLuaWndGrabber(CWndBase* wnd, BOOL bActivateWnd = TRUE);
    virtual ~CLuaWndGrabber();

    virtual BOOL GetMouseClick(SPoint &clickPt);
};

class CLuaDesktopGrabber : public CLuaWndGrabber
{
public:
    CLuaDesktopGrabber(CWndBase* wnd, BOOL bActivateWnd = TRUE);
    ~CLuaDesktopGrabber();

    virtual BOOL GetMouseClick(SPoint &clickPt);
};
typedef TDel<CLuaWndGrabber> PWndGrabber;

#endif //_LUAWNDGRABBER_HXX_
