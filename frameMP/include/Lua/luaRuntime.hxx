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

// luaRuntime.hxx

#include "../mp.hxx"

#ifndef _MPLUARUNTIME_HXX_
#define _MPLUARUNTIME_HXX_

struct lua_State;

namespace frameMP 
{
	class CLuaRuntime
	{
		lua_State *m_lua_state;

		public:
			CLuaRuntime();
			virtual ~CLuaRuntime();
			
			inline lua_State* GetState() {return m_lua_state;}
			bool DoFile(const char *file);
			bool DoString(const char *strTodo, const char *chunkName, StrA &errMsg);

	};
	typedef TDel<CLuaRuntime> PCLuaRuntime;
}; // namespace frameMP

#endif // _MPLUARUNTIME_HXX_
