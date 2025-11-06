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

// luaPrc.hxx

#include <main.hxx>
#include <Lua/luaObj.hxx>

#ifndef _LUAPRC_HXX_
#define _LUAPRC_HXX_

class frameMP::CLuaRuntime;
class frameMP::CPrc;
class CLuaPrc  : public CLuaObj
{
    static const luaL_reg s_meta[];
    static const luaL_reg s_methods[];
    static frameMP::CLuaRuntime *s_runtime;

    frameMP::CPrc *m_prc;
public:
    static const char s_className[];

    CLuaPrc(CPrc *prc);
    CLuaPrc(PrcId id);
    virtual ~CLuaPrc();

    static bool Register(frameMP::CLuaRuntime *rt);
    static int ToString(lua_State *state);
    static int GarbageCollector(lua_State *state);

    static int GetPrcId(lua_State *state);
	static int Execute(lua_State *state);
    static int Create(lua_State *state);
    static int Close(lua_State *state);
    static int IsRunning(lua_State *state);
    static int GetProcessWin(lua_State *state);
    static int GetPrcAllTopWindows(lua_State *state);
    static int Terminate(lua_State *state);
};

#endif // _LUAPRC_HXX_
