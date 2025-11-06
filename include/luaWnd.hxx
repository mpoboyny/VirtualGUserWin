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

// luaWnd.hxx

#include <main.hxx>
#include <Lua/luaObj.hxx>
#include <wnd.hxx>

#ifndef _LUAWND_HXX_
#define _LUAWND_HXX_

class frameMP::CLuaRuntime;
class frameMP::CWndBase;
class CLuaWnd  : public CLuaObj
{
        // variables
        static const luaL_reg s_meta[];
        static const luaL_reg s_methods[];
        static frameMP::CLuaRuntime *s_runtime;
        CWndBase *m_wnd;

		static Str s_tesseractPath;
		static Str s_tesseractOpt;
		static bool s_tesseractAvaliable;

        inline static void RectToTable(SRect &rc, lua_State *state);
		inline static StrA ExecTesseract(const Str &imgFile);
    public:
        static const char s_className[];

        CLuaWnd(SWinInfo& inf);
        virtual ~CLuaWnd();

        static void WinInfoListToTable(WinInfoList &wndList, lua_State *state);

        static bool Register(frameMP::CLuaRuntime *rt);

        static int ToString(lua_State *state);
        static int GarbageCollector(lua_State *state);

        static int FindWindowByTitle(lua_State *state);
        static int FindWindowByTextInTitle(lua_State *state);
        static int GetWindowFromPoint(lua_State *state);
        static int ActivateWindow(lua_State *state);
		static int ActivateWindowEx(lua_State* state);
		static int Minimize(lua_State* state);
		static int Maximize(lua_State* state);
        static int GetTitle(lua_State *state);
        static int GetText(lua_State *state);
        static int SetText(lua_State *state);
        static int PrintText(lua_State *state);
        static int GetGeometry(lua_State *state);
        static int GetId(lua_State *state);
        static int GetProcess(lua_State *state);
        static int GetChildByClassName(lua_State *state);
        static int GetAllChild(lua_State *state);
		static int GetAllZOrder(lua_State *state);
        static int GetAllParent(lua_State *state);
        static int SetGeometry(lua_State *state);
        static int ClickWindowL(lua_State *state);
        static int ClickWindowExL(lua_State *state);
        static int ClickWindowDL(lua_State *state);
		static int ClickWindowExDL(lua_State *state);
        static int ClickWindowR(lua_State *state);
        static int ClickWindowExR(lua_State *state);
		static int ClickWindowExDR(lua_State* state);
        static int GetClassName(lua_State *state);
        static int StartMouseRec(lua_State *state);
        static int GetAllChildFromPoint(lua_State *state);
        static int GetDesktopWindow(lua_State *state);
        static int ShowDesktopWindow(lua_State *state);
        static int IsDesktop(lua_State *state);
        static int MouseMove(lua_State *state);
        static int MouseLMove(lua_State *state);
        static int MouseRMove(lua_State *state);
        static int KeyType(lua_State *state);
        static int KeyDown(lua_State *state);
        static int KeyUp(lua_State *state);
		static int GetActivWindow(lua_State* state);
		static int SaveScreenShot(lua_State *state);
        static int CmpScreenShotWith(lua_State *state);
		static int SaveScreenShotEx(lua_State* state);
		static int CmpScreenShotWithEx(lua_State* state);
		static int SetTesseractPath(lua_State* state);
		static int GetTextFromImgFile(lua_State* state);
		static int GetScreenShotText(lua_State* state);
		static int GetClipboardText(lua_State* state);
		static int SetCompareCut(lua_State* state);
};
typedef TDel<CLuaWnd> PCLuaWnd;

#endif // _LUAWND_HXX_
