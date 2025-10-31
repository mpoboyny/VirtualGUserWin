// 
// This file is part of VirtualGUser project.
// Copyright ©2015 Mykhaylo Poboynyy <mpoboyny@googlemail.com>

// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; version 2 of the License.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
//

// luaRt.hxx

#include "luaObj.hxx"


#ifndef _LUARTOBJ_HXX_
#define _LUARTOBJ_HXX_


namespace frameMP
{

class CLuaRuntime;
class CLuaRtObj  : public CLuaObj
{
	static const luaL_reg s_meta[];
	static const luaL_reg s_methods[];
	static CLuaRuntime *s_runtime;
public:
	static const char s_className[];

	CLuaRtObj();
	virtual ~CLuaRtObj();

	static bool Register(CLuaRuntime *rt);
	static int ToString(lua_State *state);
	static int GarbageCollector(lua_State *state);

	static int Sleep(lua_State *state);
	static int GetOStype(lua_State *state);

	static int GetAppName(lua_State *state);
	static int GetAppArgv(lua_State *state);

	static int SetLocale(lua_State* state);

};

}; // namespace frameMP

#endif // _LUARTOBJ_HXX_
