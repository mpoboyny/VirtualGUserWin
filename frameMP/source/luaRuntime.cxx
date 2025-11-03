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

// luaRuntime.cxx

#include <frameMP/Lua/luaRuntime.hxx>
#include <frameMP/Lua/luaObj.hxx>
#include <lua.hxx>

namespace frameMP {
    
    CLuaRuntime::CLuaRuntime() : m_lua_state(lua_open())
    {
        TrVoid(!m_lua_state);
        luaopen_base(m_lua_state);
        luaopen_table(m_lua_state);
        luaopen_io(m_lua_state);
        luaopen_string(m_lua_state);
        luaopen_math(m_lua_state);
        luaopen_debug(m_lua_state);
    }

    CLuaRuntime::~CLuaRuntime()
    {
        if (m_lua_state) {
            lua_close(m_lua_state);
        }
    }

    bool CLuaRuntime::DoFile(const char *file)
    {
        TrFu;
        return !lua_dofile(m_lua_state, file);
    }
    
    bool CLuaRuntime::DoString(const char *strTodo, const char *chunkName, StrA &errMsg)
    {
        //TrFu;
        int res = luaL_loadbuffer(m_lua_state, strTodo, strlen(strTodo), chunkName);
        if (!res) {
            res = lua_pcall(m_lua_state, 0, LUA_MULTRET, 0);
        }
        if (res) {
            if(lua_isstring(m_lua_state, -1)) {
                errMsg = lua_tostring(m_lua_state, -1);
            }
            else {
                errMsg = "Unknow error :/";
            }
        }
        //TrVar(res);
        return !res;
    }
}; // namespace frameMP
