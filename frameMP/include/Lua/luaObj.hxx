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

// luaObj.hxx

#include "../mp.hxx"
#include <lua.hxx>

#ifndef _LUAOBJ_HXX_
#define _LUAOBJ_HXX_

namespace frameMP
{

#define LUAOBJ(data) ((CLuaObj*)(data->m_luaObj))

struct SLuaObjData
{
	void *m_luaObj;
	inline void Init()
	{
		m_luaObj = NULL;
	}
};

class CLuaObj
{	
	protected:
		static bool Register(lua_State *state, const luaL_reg *metaMethods, const luaL_reg *methods, const char *className);
		
		static int ToString(lua_State *state, int index, const char *className);
		static int GarbageCollector(lua_State *state, int index, const char *className);

		static SLuaObjData* GetData(lua_State *state, int index, const char *className);
		static SLuaObjData* PushData(lua_State *state, int stackMinusIndex, const char *className);

        static inline int badParam(lua_State *state, int need, int have);
	public:
		CLuaObj(){}
		virtual ~CLuaObj(){}
};

#include "luaObj.inl"

}; // namespace frameMP

#endif //_LUAOBJ_HXX_
