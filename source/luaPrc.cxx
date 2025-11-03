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

// luaPrc.cxx

#include <frameMP/Lua/luaRuntime.hxx>
#include <frameMP/prc.hxx>
#include <luaPrc.hxx>
#include <luaWnd.hxx>


#define LUAPRC(data) ((CLuaPrc*)(data->m_luaObj))->m_prc

const char CLuaPrc::s_className[] = "MPProcess";

const luaL_reg CLuaPrc::s_meta[] =
{
    {"__gc", CLuaPrc::GarbageCollector},
    {"__tostring", CLuaPrc::ToString},
    {0, 0}
};

const luaL_reg CLuaPrc::s_methods[] = 
{
  {"getPrcId",			    CLuaPrc::GetPrcId},
  {"executeFile",   	    CLuaPrc::Execute},
  {"createPrc",			    CLuaPrc::Create},
  {"isPrcRunning",		    CLuaPrc::IsRunning},
  {"getPrcMainWindow",	    CLuaPrc::GetProcessWin},
  {"getPrcAllTopWindows",   CLuaPrc::GetPrcAllTopWindows},
  {"terminatePrc",		    CLuaPrc::Terminate},
  {"close",		            CLuaPrc::Close},
  {0, 0}
};

CLuaRuntime* CLuaPrc::s_runtime = NULL;

CLuaPrc::CLuaPrc(CPrc *prc) : CLuaObj(), m_prc(prc)
{
}

CLuaPrc::CLuaPrc(PrcId id) : CLuaObj(), m_prc(new CPrc(id))
{
}

CLuaPrc::~CLuaPrc()
{
    delete m_prc;
}

bool CLuaPrc::Register(CLuaRuntime *rt)
{
    TrRet(s_runtime, false);
    s_runtime = rt;
    return CLuaObj::Register(s_runtime->GetState(), s_meta, s_methods, s_className);
}

int CLuaPrc::ToString(lua_State *state)
{
    return CLuaObj::ToString(state, 1, s_className);
}

int CLuaPrc::GarbageCollector(lua_State *state)
{
    return CLuaObj::GarbageCollector(state, 1, s_className);
}

int CLuaPrc::GetPrcId(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    lua_pushnumber(state, LUAPRC(data)->GetPid());
    return 1;
}

/*static*/
int CLuaPrc::Execute(lua_State *state)
{
	size_t cmdLineSize = 0;
	const char *cmdLine = luaL_checklstring(state, 1, &cmdLineSize);
    if (!cmdLine)
    {
        lua_pushnil(state);
        return 1;
    }
	SLuaObjData *newData = PushData(state, -2, s_className);
    if (!newData)
    {
        lua_pushnil(state);
        return 1;
    }
    CPrc *pCPrc =  new CPrc();
    if (!pCPrc->ExecuteFile(StrAToStr(cmdLine)))
    {
        delete pCPrc;
        lua_pushnil(state);
        return 1;
    }
    newData->m_luaObj = new CLuaPrc(pCPrc);
    return 1;
}

int CLuaPrc::Create(lua_State *state)
{
    size_t cmdLineSize = 0, paramSize = 0, fileSize = 0;
    const char *cmdLine = luaL_checklstring(state, 1, &cmdLineSize);
    if (!cmdLine)
    {
        lua_pushnil(state);
        return 1;
    }
    const char *param = luaL_checklstring(state, 2, &paramSize);
    if (!param)
    {
        lua_pushnil(state);
        return 1;
    }
    const char *file = luaL_checklstring(state, 3, &fileSize);
    if (!file)
    {
        lua_pushnil(state);
        return 1;
    }
    SLuaObjData *newData = PushData(state, -2, s_className);
    if (!newData)
    {
        lua_pushnil(state);
        return 1;
    }
    CPrc *pCPrc =  new CPrc();
    if (!pCPrc->Create(StrAToStr(cmdLine), StrAToStr(param), StrAToStr(file)))
    {
        delete pCPrc;
        lua_pushnil(state);
        return 1;
    }
    newData->m_luaObj = new CLuaPrc(pCPrc);
    return 1;
}

int CLuaPrc::Close(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    lua_pushboolean(state, LUAPRC(data)->Close());
    return 1;
}

int CLuaPrc::IsRunning(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    lua_pushboolean(state, LUAPRC(data)->IsRunning());
    return 1;
}

int CLuaPrc::GetProcessWin(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    SWinInfo inf =  LUAPRC(data)->GetProcessWinInfo();
    SLuaObjData *newData = PushData(state, -2, CLuaWnd::s_className);
    TrRet(!newData, 0);
    newData->m_luaObj = new CLuaWnd(inf);
    return 1;
}

int CLuaPrc::GetPrcAllTopWindows(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    WinInfoList allInf =  LUAPRC(data)->GetProcessAllWinInfo();
    CLuaWnd::WinInfoListToTable(allInf, state);
    return 1;
}

int CLuaPrc::Terminate(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    lua_pushboolean(state, LUAPRC(data)->Terminate());
    return 1;
}
