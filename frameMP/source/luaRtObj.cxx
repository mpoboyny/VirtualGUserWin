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

// luaRt.cxx

#include <frameMP/Lua/luaRuntime.hxx>
#include <frameMP/Lua/luaRtObj.hxx>
#include <frameMP/app.hxx>
#include <frameMP/file.hxx>

// #define LUART(data) ((CLuaRtObj*)(data->m_luaObj))->m_prc

namespace frameMP 
{

const char CLuaRtObj::s_className[] = "MPRuntime";

const luaL_reg CLuaRtObj::s_meta[] =
{
	{"__gc",		CLuaRtObj::GarbageCollector},
	{"__tostring",	CLuaRtObj::ToString},
	{0, 0}
};

const luaL_reg CLuaRtObj::s_methods[] =
{
  {"sleep",			CLuaRtObj::Sleep},
  {"getOStype",		CLuaRtObj::GetOStype},
  {"getAppName",	CLuaRtObj::GetAppName},
  {"getAppArgv",	CLuaRtObj::GetAppArgv},
  {"setlocale",		CLuaRtObj::SetLocale },
  {0, 0}
};

CLuaRuntime* CLuaRtObj::s_runtime = NULL;

CLuaRtObj::CLuaRtObj() : CLuaObj()
{
}

CLuaRtObj::~CLuaRtObj()
{
}

bool CLuaRtObj::Register(CLuaRuntime *rt)
{
	TrRet(s_runtime, false);
	s_runtime = rt;
	return CLuaObj::Register(s_runtime->GetState(), s_meta, s_methods, s_className);
}

int CLuaRtObj::ToString(lua_State *state)
{
	return CLuaObj::ToString(state, 1, s_className);
}

int CLuaRtObj::GarbageCollector(lua_State *state)
{
	return CLuaObj::GarbageCollector(state, 1, s_className);
}

int CLuaRtObj::Sleep(lua_State *state)
{
	int iSec = luaL_checkint(state, 1);
	TrRet(iSec < 1, 0);
#if defined(MSWIN_OS)
	::Sleep(iSec * 1000);
#elif defined(LINUX_OS)
	sleep(iSec);
#endif
	return 0;
}

int CLuaRtObj::GetOStype(lua_State *state)
{
	// based on defines from file.hxx
	if (MP_PATH_SEP == MP_MSPATH_SEP) {
		lua_pushstring(state, "MSWINDOWS*");
	}
	else if (MP_PATH_SEP == MP_NXPATH_SEP) {
		lua_pushstring(state, "*NIX");
	}
	else {
		lua_pushnil(state);
	}
	return 1;
}

int CLuaRtObj::GetAppName(lua_State *state)
{
    TrFu;
	lua_pushstring(state, StrToStrA(CApp::Inst().GetAppName()).c_str());
	return 1;
}

int CLuaRtObj::GetAppArgv(lua_State *state)
{
    TrFu;
	const CApp::ArgList allArg = CApp::Inst().GetAllArg();
	lua_newtable(state);
    Tr("argc: "<<allArg.size());
	if (allArg.size()) {
		int luaInd = 1;
		CApp::ArgList::const_iterator itEnd = allArg.end();
		for (CApp::ArgList::const_iterator it = allArg.begin(); it != itEnd; ++it) {
			lua_pushnumber(state, luaInd++);
			lua_pushstring(state, StrToStrA(*it).c_str());
			lua_settable(state, -3);
		}
	}
	return 1;
}

int CLuaRtObj::SetLocale(lua_State* state)
{
	TrFu;
	int argc = lua_gettop(state);
	const char* res;
	TrVar(argc);
	if (argc == 0)
	{
		res = setlocale(LC_ALL, "");
		TrRet(!res, 0);
	}
	else
	{
		size_t locSize = 0;
		const char* loc = luaL_checklstring(state, 1, &locSize);
		res = setlocale(LC_ALL, "");
		TrRet(!res, 0);
	}
	lua_pushstring(state, res);
	return 1;
}


}; // namespace frameMP
