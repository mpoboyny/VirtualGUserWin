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

// luaObj.cxx

#include <Lua/luaObj.hxx>
#include <Lua/luaRuntime.hxx>

namespace frameMP 
{

SLuaObjData* CLuaObj::GetData(lua_State *state, int index, const char *className)
{
	SLuaObjData *data = NULL;
	luaL_checktype(state, index, LUA_TUSERDATA);
	data = (SLuaObjData*)luaL_checkudata(state, index, className);
	if (!data) {
		Tr("bad data: 0x"<<hex<<data);
        luaL_typerror(state, index, className);
	}
	return data;
}

SLuaObjData* CLuaObj::PushData(lua_State *state, int stackMinusIndex, const char *className)
{
	SLuaObjData *data = (SLuaObjData*)lua_newuserdata(state, sizeof(SLuaObjData));
	if (data) {
		luaL_getmetatable(state, className);
		lua_setmetatable(state, stackMinusIndex);
		data->Init();
	}
	else {
		Tr("bad data: 0x"<<hex<<data);
        luaL_error(state, "null pointer to %s", className);
	}
	return data;
}

int CLuaObj::ToString(lua_State *state, int index, const char *className)
{
	SLuaObjData *data = GetData(state, index, className);
	if (data) {
		lua_pushfstring(state, "%s", className, LUAOBJ(data));
		return 1;
	}
	return 0;
}

int CLuaObj::GarbageCollector(lua_State *state, int index, const char *className)
{
	SLuaObjData *data = GetData(state, index, className);
	if (data) {
		delete LUAOBJ(data);
	}
	else
	{
		Tr("bad data: 0x"<<hex<<data);
		return 0;
	}
	return 1;
}

bool CLuaObj::Register(lua_State *state, const luaL_reg *metaMethods, const luaL_reg *methods, const char *className)
{
	luaL_openlib(state, className, methods, 0);
	luaL_newmetatable(state, className);          					
	luaL_openlib(state, 0, metaMethods, 0);    
	lua_pushliteral(state, "__index");
	lua_pushvalue(state, -3);               
	lua_rawset(state, -3);                  
	lua_pushliteral(state, "__metatable");
	lua_pushvalue(state, -3);               
	lua_rawset(state, -3);                  				
	lua_pop(state, 1);                 
	lua_pop(state, 1); //After register the methods are still on the stack, remove them.	
	return true;
}

}; // namespace frameMP
