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

// luaWnd.cxx

#include <frameMP/Lua/luaRuntime.hxx>
#include <luaWnd.hxx>
#include <luaPrc.hxx>
#include <luaWndGrabber.hxx>
#include <frameMP/file.hxx>
#include <frameMP/prc.hxx>
#include <frameMP/tools.hxx>
#include <fstream>

#define LUAWND(data) ((CLuaWnd*)(data->m_luaObj))->m_wnd

const char CLuaWnd::s_className[] = "MPWindow";

const luaL_reg CLuaWnd::s_meta[] =
{
    {"__gc", CLuaWnd::GarbageCollector},
    {"__tostring", CLuaWnd::ToString},
    {0, 0}
};

const luaL_reg CLuaWnd::s_methods[] = {
  {"findWindowByTitle", CLuaWnd::FindWindowByTitle},
  {"findWindowByTextInTitle", CLuaWnd::FindWindowByTextInTitle},
  {"getWindowFromPoint", CLuaWnd::GetWindowFromPoint},
  {"activateWindow", CLuaWnd::ActivateWindow},
  {"activateWindowEx", CLuaWnd::ActivateWindowEx},
  {"minimizeWindow", CLuaWnd::Minimize},
  {"maximizeWindow", CLuaWnd::Maximize},
  {"getActivWindow", CLuaWnd::GetActivWindow},
  {"getTitle", CLuaWnd::GetTitle},
  {"getText", CLuaWnd::GetText},
  {"setText", CLuaWnd::SetText},
  {"printText", CLuaWnd::PrintText},
  {"getGeometry", CLuaWnd::GetGeometry},
  {"getId", CLuaWnd::GetId},
  {"getProcess", CLuaWnd::GetProcess},
  {"getChildByClassName", CLuaWnd::GetChildByClassName},
  {"getAllChild", CLuaWnd::GetAllChild},
  {"getAllZOrder", CLuaWnd::GetAllZOrder},
  {"getAllParent", CLuaWnd::GetAllParent},
  {"setGeometry", CLuaWnd::SetGeometry},
  {"clickWindowL", CLuaWnd::ClickWindowL},
  {"clickWindowExL", CLuaWnd::ClickWindowExL},
  {"clickWindowDL", CLuaWnd::ClickWindowDL},
  {"clickWindowExDL", CLuaWnd::ClickWindowExDL },
  {"clickWindowR", CLuaWnd::ClickWindowR},
  {"clickWindowExR", CLuaWnd::ClickWindowExR},
  {"clickWindowExDR", CLuaWnd::ClickWindowExDR },
  {"getClassName", CLuaWnd::GetClassName},
  {"startMouseRec", CLuaWnd::StartMouseRec},
  {"getAllChildFromPoint", GetAllChildFromPoint},
  {"getDesktopWindow", GetDesktopWindow},
  {"showDesktopWindow", ShowDesktopWindow},
  {"isDesktop", IsDesktop},
  {"mouseMove", MouseMove},
  {"mouseLMove", MouseLMove},
  {"mouseRMove", MouseRMove},
  {"keyType", KeyType},
  {"keyDown", KeyDown},
  {"keyUp", KeyUp},
  {"saveScreenShot", SaveScreenShot},
  {"cmpScreenShotWithBmp", CmpScreenShotWith},
  {"saveScreenShotEx", SaveScreenShotEx},
  {"cmpScreenShotWithBmpEx", CmpScreenShotWithEx},
  {"setTesseractPath", SetTesseractPath },
  {"getTextFromImgFile", GetTextFromImgFile },
  {"getScreenShotText", GetScreenShotText },
  {"getClipboardText", GetClipboardText },
  {"setCompareCut", SetCompareCut },
  {0, 0}
};

CLuaRuntime* CLuaWnd::s_runtime = NULL;

Str CLuaWnd::s_tesseractPath;
Str CLuaWnd::s_tesseractOpt;
bool CLuaWnd::s_tesseractAvaliable = false;

CLuaWnd::CLuaWnd(SWinInfo& inf) : CLuaObj(), m_wnd(new CWndBase(inf))
{
}

CLuaWnd::~CLuaWnd()
{
    delete m_wnd;
}

void CLuaWnd::WinInfoListToTable(WinInfoList &wndList, lua_State *state)
{
    lua_newtable(state);
    if (wndList.size()) {
        int luaInd = 1;
        WinInfoList::iterator itEnd = wndList.end();
        for (WinInfoList::iterator it = wndList.begin(); it != itEnd; ++it) {
            lua_pushnumber(state, luaInd++);
            SLuaObjData *newData = PushData(state, -2, s_className);
            TrVoid(!newData);
            newData->m_luaObj = new CLuaWnd(*it);
            lua_settable(state, -3);
        }
    }
}

void CLuaWnd::RectToTable(SRect &rc, lua_State *state)
{
    lua_newtable(state);
    lua_pushstring(state, "posX");
    lua_pushnumber(state, rc.m_posX);
    lua_settable(state, -3);
    lua_pushstring(state, "posY");
    lua_pushnumber(state, rc.m_posY);
    lua_settable(state, -3);
    lua_pushstring(state, "width");
    lua_pushnumber(state, rc.m_width);
    lua_settable(state, -3);
    lua_pushstring(state, "height");
    lua_pushnumber(state, rc.m_height);
    lua_settable(state, -3);
}

StrA CLuaWnd::ExecTesseract(const Str& imgFile)
{
	const int MaxWaitSteps = 20;
	StrA res;
	TrRet(!s_tesseractAvaliable, res);
	TrStr(imgFile);
	Str outFile = CPath::GetTmpFilePath();
	TrRet(outFile.empty(), res);
	TrStr(outFile);
	Str cmdWithOpt = s_tesseractPath;
	if (!s_tesseractOpt.empty())
	{
		cmdWithOpt += TEXT(' ') + s_tesseractOpt;
	}
	TrStr(cmdWithOpt);
	CPrc worker;
	TrRet(!worker.Create(cmdWithOpt, imgFile, outFile, false), res);
	for (int i = 0; i < MaxWaitSteps && worker.IsRunning(); ++i) {
#if defined(MSWIN_OS)
		::Sleep(500);
#elif defined(LINUX_OS)
		sleep(1);
#endif
	}
	if (worker.IsRunning())
	{
		Tr("Tesseract still running, terminate with error...");
		worker.Terminate();
		return res;
	}
	outFile += TEXT(".txt");
	TrStr(outFile);
	CPath pathOut(outFile);
	TrRet(!pathOut.IsFile(), res);
	SFile::ReadFileToStr(outFile, res);
	return res;
}

bool CLuaWnd::Register(CLuaRuntime *rt)
{
    TrRet(s_runtime, false);
    s_runtime = rt;
    return CLuaObj::Register(s_runtime->GetState(), s_meta, s_methods, s_className);
}

int CLuaWnd::ToString(lua_State *state)
{
    return CLuaObj::ToString(state, 1, s_className);
}

int CLuaWnd::GarbageCollector(lua_State *state)
{
    return CLuaObj::GarbageCollector(state, 1, s_className);
}

int CLuaWnd::FindWindowByTitle(lua_State *state)
{
    size_t titleSize = 0;
    const char *title = luaL_checklstring(state, 1, &titleSize);
    if (!title) {
        return 0;
    }
    WinInfoList wndList = CWndBase::FindWindowByTitle(StrAToStr(StrA(title)).c_str());
    WinInfoListToTable(wndList, state);
    return 1;
}

int CLuaWnd::FindWindowByTextInTitle(lua_State *state)
{
    size_t titleSize = 0;
    const char *title = luaL_checklstring(state, 1, &titleSize);
    if (!title) {
        return 0;
    }
    WinInfoList wndList = CWndBase::FindWindowByTextInTitle(StrAToStr(StrA(title)).c_str());
    WinInfoListToTable(wndList, state);
    return 1;
}

int CLuaWnd::GetWindowFromPoint(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 2, 0);
    SLuaObjData *newData = PushData(state, -2, s_className);
    TrRet(!newData, 0);
    SWinInfo inf = CWndBase::GetWindowFromPoint(SPoint(luaL_checknumber(state, 1), 
                                                   luaL_checknumber(state, 2)));
    newData->m_luaObj = new CLuaWnd(inf);
    return 1;
}

int CLuaWnd::ActivateWindow(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    lua_pushboolean(state, LUAWND(data)->Activate());
    return 1;
}

int CLuaWnd::ActivateWindowEx(lua_State* state)
{
	SLuaObjData* data = GetData(state, 1, s_className);
	TrRet(!data, 0);
	lua_pushboolean(state, LUAWND(data)->ActivateEx());
	return 1;
}

int CLuaWnd::Minimize(lua_State* state)
{
	TrFu;
	int argc = lua_gettop(state);  /* number of arguments */
	TrRet(argc != 1, 0);
	SLuaObjData* data = GetData(state, 1, s_className);
	TrRet(!data, 0);
	LUAWND(data)->Minimize();
	return 0;
}

int CLuaWnd::Maximize(lua_State* state)
{
	TrFu;
	int argc = lua_gettop(state);  /* number of arguments */
	TrRet(argc != 1, 0);
	SLuaObjData* data = GetData(state, 1, s_className);
	TrRet(!data, 0);
	LUAWND(data)->Maximize();
	return 0;
}

int CLuaWnd::GetTitle(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    Str text;
    LUAWND(data)->GetTitle(text);
    lua_pushstring(state, StrToStrA(text).c_str());
    return 1;
}

int CLuaWnd::GetText(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    Str text;
    LUAWND(data)->GetText(text);
    lua_pushstring(state, StrToStrA(text.c_str()).c_str());
    return 1;
}

int CLuaWnd::SetText(lua_State *state)
{
    TrFu;
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 2, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    size_t txtSize = 0;
    const char *txt = luaL_checklstring(state, 2, &txtSize);
    TrVar(txt);
    TrRet(!txt, 0);
    lua_pushboolean(state, LUAWND(data)->SetText(StrAToStr(txt).c_str()));
    return 1;
}

int CLuaWnd::PrintText(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 2, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    size_t txtSize = 0;
    const char *txt = luaL_checklstring(state, 2, &txtSize);
    TrRet(!txt, 0);
    lua_pushboolean(state, LUAWND(data)->TypeText(StrAToStr(txt).c_str()));
    return 1;
}

int CLuaWnd::GetGeometry(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    SRect rc = LUAWND(data)->GetWndRect();
    RectToTable(rc, state);
    return 1;
}

int CLuaWnd::GetId(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    lua_pushnumber(state, (lua_Number)(long)LUAWND(data)->GetHwnd());
    return 1;
}

int CLuaWnd::GetProcess(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    SLuaObjData *newData = PushData(state, -2, CLuaPrc::s_className);
    if (!newData)
    {
        lua_pushnil(state);
        return 1;
    }
    newData->m_luaObj = new CLuaPrc(LUAWND(data)->GetWindowPrcId());
    return 1;
}

int CLuaWnd::GetChildByClassName(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 2, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    size_t clNameSize = 0;
    const char *clName = luaL_checklstring(state, 2, &clNameSize);
    if (!clName) {
        return 0;
    }
    WinInfoList wndList = LUAWND(data)->GetAllChildByClassName(StrAToStr(StrA(clName)).c_str());
    WinInfoListToTable(wndList, state);
    return 1;
}

int CLuaWnd::GetAllChild(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    WinInfoList wndList = LUAWND(data)->GetAllChild();
    WinInfoListToTable(wndList, state);
    return 1;
}

int CLuaWnd::GetAllZOrder(lua_State *state)
{
	SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    WinInfoList wndList = LUAWND(data)->GetAllZOrder();
    WinInfoListToTable(wndList, state);
    return 1;
}

int CLuaWnd::GetAllParent(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    WinInfoList wndList = LUAWND(data)->GetAllParent();
    WinInfoListToTable(wndList, state);
    return 1;
}

int CLuaWnd::SetGeometry(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 5, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    lua_pushboolean(state, LUAWND(data)->SetWndRect(SRect(luaL_checknumber(state, 2),
                                                          luaL_checknumber(state, 3),
                                                          luaL_checknumber(state, 4),
                                                          luaL_checknumber(state, 5))));
    return 1;
}

int CLuaWnd::ClickWindowL(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc < 1 || argc > 4 || argc == 3, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    if (argc == 1) {
        LUAWND(data)->ClickL();
    }
    else if (argc == 2) {
        LUAWND(data)->ClickL(luaL_checknumber(state, 2));
    }
    else {
        LUAWND(data)->ClickL(luaL_checknumber(state, 2), SPoint(luaL_checknumber(state, 3),
                                                                luaL_checknumber(state, 4)));
    }
    return 0;
}

int CLuaWnd::ClickWindowExL(lua_State *state)
{
    // TrFu;
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 3 && argc != 1, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    CWndBase *topWnd = LUAWND(data);
    if (argc == 3) {
        SPoint pt(luaL_checknumber(state, 2), luaL_checknumber(state, 3));
        topWnd->ClickExL(pt);
    }
    else {
        topWnd->ClickExL();
    }
    return 0;
}

int CLuaWnd::ClickWindowExDL(lua_State *state)
{
	// TrFu;
	int argc = lua_gettop(state);  /* number of arguments */
	TrRet(argc != 3 && argc != 1, 0);
	SLuaObjData *data = GetData(state, 1, s_className);
	TrRet(!data, 0);
	CWndBase *topWnd = LUAWND(data);
	if (argc == 3) {
		SPoint pt(luaL_checknumber(state, 2), luaL_checknumber(state, 3));
		topWnd->ClickExDL(pt);
	}
	else {
		topWnd->ClickExDL();
	}
	return 0;
}

int CLuaWnd::ClickWindowDL(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc < 1 || argc > 3 || argc == 2, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    if (argc == 1) {
        LUAWND(data)->ClickDL();
    }
    else {
        LUAWND(data)->ClickDL(SPoint(luaL_checknumber(state, 2), luaL_checknumber(state, 3)));
    }
    return 0;
}

int CLuaWnd::ClickWindowR(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc < 1 || argc > 4 || argc == 3, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    if (argc == 1) {
        LUAWND(data)->ClickR();
    }
    else if (argc == 2) {
        LUAWND(data)->ClickR(luaL_checknumber(state, 2));
    }
    else {
        LUAWND(data)->ClickR(luaL_checknumber(state, 2), SPoint(luaL_checknumber(state, 3),
                                                                luaL_checknumber(state, 4)
                                                                ));
    }
    return 0;
}

int CLuaWnd::ClickWindowExR(lua_State *state)
{
    // TrFu;
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 3 && argc != 1, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    CWndBase *topWnd = LUAWND(data);
    if (argc == 3) {
        SPoint pt(luaL_checknumber(state, 2), luaL_checknumber(state, 3));
        topWnd->ClickExR(pt);
    }
    else {
        topWnd->ClickExR();
    }
    return 0;
}

int CLuaWnd::ClickWindowExDR(lua_State* state)
{
	// TrFu;
	int argc = lua_gettop(state);  /* number of arguments */
	TrRet(argc != 3 && argc != 1, 0);
	SLuaObjData* data = GetData(state, 1, s_className);
	TrRet(!data, 0);
	CWndBase* topWnd = LUAWND(data);
	if (argc == 3) {
		SPoint pt(luaL_checknumber(state, 2), luaL_checknumber(state, 3));
		topWnd->ClickExDR(pt);
	}
	else {
		topWnd->ClickExDR();
	}
	return 0;
}

int CLuaWnd::GetClassName(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    lua_pushstring(state, StrToStrA(LUAWND(data)->GetClassName()).c_str());
    return 1;
}

int CLuaWnd::StartMouseRec(lua_State *state)
{
    TrFu;
    static volatile bool s_isRecordStarted = false;
    BOOL res = FALSE;
    //TrVar(s_isRecordStarted);
    if (s_isRecordStarted) {
        return 0;
    }
    int argc = lua_gettop(state);
    TrVar(argc);
    TrRet(argc != 3 && argc != 2, 0);
    if (lua_isfunction(state, -1)) {
        SLuaObjData *data = GetData(state, 1, s_className);
        BOOL bActivate = TRUE;
        if (argc == 3) {
            bActivate = lua_toboolean(state, 2);
        }
        TrRet(!data, 0);
        s_isRecordStarted = true;
        CWndBase *wnd = LUAWND(data);
        PWndGrabber gr(wnd->GetSWinInfo().m_isDeskTop? new CLuaDesktopGrabber(wnd, bActivate): new CLuaWndGrabber(wnd, bActivate));
        SPoint clickPt;
        res = gr->GetMouseClick(clickPt);
        lua_pushnumber(state, clickPt.m_x); // push first argument to the function
        lua_pushnumber(state, clickPt.m_y); // push second argument to the function
        lua_pcall(state, 2, 0, 0); // call a function with two arguments and no return values
        s_isRecordStarted = false;
    }
    lua_pushboolean(state, res);
    //TrVar(res);
    return 1;
}

int CLuaWnd::GetAllChildFromPoint(lua_State *state)
{
    int argc = lua_gettop(state);
    TrRet(argc != 3, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    WinInfoList wndList = LUAWND(data)->GetAllChildFromPoint(SPoint(luaL_checknumber(state, 2), 
                                                          luaL_checknumber(state, 3)));
    WinInfoListToTable(wndList, state);
    return 1;
}

int CLuaWnd::GetDesktopWindow(lua_State *state)
{
    SLuaObjData *newData = PushData(state, -2, s_className);
    TrRet(!newData, 0);
    SWinInfo inf = CWndBase::GetDesktopWindow();
    newData->m_luaObj = new CLuaWnd(inf);
    return 1;
}

int CLuaWnd::ShowDesktopWindow(lua_State *state)
{
    lua_pushboolean(state, CWndBase::ShowDesktopWindow());
    return 1;
}

int CLuaWnd::IsDesktop(lua_State *state)
{
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    lua_pushboolean(state, LUAWND(data)->GetSWinInfo().m_isDeskTop);
    return 1;
}

int CLuaWnd::MouseMove(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 3, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    LUAWND(data)->MoveMouseTo(SPoint(luaL_checknumber(state, 2), luaL_checknumber(state, 3)));
    return 0;
}

int CLuaWnd::MouseLMove(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 3, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    LUAWND(data)->MoveLMouse(SPoint(luaL_checknumber(state, 2), luaL_checknumber(state, 3)));
    return 0;
}

int CLuaWnd::MouseRMove(lua_State *state)
{
    int argc = lua_gettop(state);  /* number of arguments */
    TrRet(argc != 3, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    LUAWND(data)->MoveRMouse(SPoint(luaL_checknumber(state, 2), luaL_checknumber(state, 3)));
    return 0;
}

int CLuaWnd::KeyType(lua_State *state)
{
    //TrFu;
    int argc = lua_gettop(state);
    //TrVar(argc);
    TrRet(argc != 2, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    TrRet(!lua_istable(state, 2), 0);
    const char *value;
    lua_pushnil(state);
    while (lua_next(state, -2)) {
        value = lua_tostring(state, -1);
        //TrVar(value);
        TrRet(!LUAWND(data)->KeyCodeType(StrAToStr(value).c_str()), 0);
        lua_pop(state, 1);
    }
    lua_pushboolean(state, 1);
    return 1;
}

int CLuaWnd::KeyDown(lua_State *state)
{
    //TrFu;
    int argc = lua_gettop(state);
    //TrVar(argc);
    TrRet(argc != 2, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    TrRet(!lua_istable(state, 2), 0);
    const char *value;
    lua_pushnil(state);
    while (lua_next(state, -2)) {
        value = lua_tostring(state, -1);
        //TrVar(value);
        TrRet(!LUAWND(data)->KeyCodeDown(StrAToStr(value).c_str()), 0);
        lua_pop(state, 1);
    }
    lua_pushboolean(state, 1);
    return 1;
}

int CLuaWnd::KeyUp(lua_State *state)
{
    //TrFu;
    int argc = lua_gettop(state);
    //TrVar(argc);
    TrRet(argc != 2, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    TrRet(!lua_istable(state, 2), 0);
    const char *value;
    lua_pushnil(state);
    while (lua_next(state, -2)) {
        value = lua_tostring(state, -1);
        //TrVar(value);
        TrRet(!LUAWND(data)->KeyCodeUp(StrAToStr(value).c_str()), 0);
        lua_pop(state, 1);
    }
    lua_pushboolean(state, 1);
    return 1;
}

int CLuaWnd::GetActivWindow(lua_State* state)
{
	SLuaObjData* newData = PushData(state, -2, s_className);
	TrRet(!newData, 0);
	SWinInfo inf = CWndBase::GetActivWindow();
	newData->m_luaObj = new CLuaWnd(inf);
	return 1;
}

int CLuaWnd::SaveScreenShot(lua_State *state)
{
    //TrFu;
    int argc = lua_gettop(state);
    //TrVar(argc);
    TrRet(argc != 2, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    size_t pathSize = 0;
    const char *pathStrA = luaL_checklstring(state, 2, &pathSize);
    TrRet(!pathStrA, 0);
    TrRet(!LUAWND(data)->SaveScreenShot(StrAToStr(pathStrA).c_str()), 0);
    lua_pushboolean(state, 1);
    return 1;
}

int CLuaWnd::SaveScreenShotEx(lua_State* state)
{
	TrFu;
	int argc = lua_gettop(state);
	//TrVar(argc);
	TrRet(argc != 6, 0);
	SLuaObjData* data = GetData(state, 1, s_className);
	TrRet(!data, 0);
	size_t pathSize = 0;
	const char* pathStrA = luaL_checklstring(state, 2, &pathSize);
	TrRet(!pathStrA, 0);
	TrRet(!LUAWND(data)->SaveScreenShotEx(StrAToStr(pathStrA).c_str(), SRect(luaL_checknumber(state, 3), luaL_checknumber(state, 4), luaL_checknumber(state, 5), luaL_checknumber(state, 6))), 0);
	lua_pushboolean(state, 1);
	return 1;
}

int CLuaWnd::CmpScreenShotWith(lua_State *state)
{
    TrFu;
    int argc = lua_gettop(state);
    TrVar(argc);
    TrRet(argc < 2 || argc > 3, 0);
    SLuaObjData *data = GetData(state, 1, s_className);
    TrRet(!data, 0);
    size_t pathSize = 0;
    const char *pathStrA = luaL_checklstring(state, 2, &pathSize);
    TrRet(!pathStrA, 0);
	BOOL soft = FALSE;
	if (argc == 3 && lua_isboolean(state, 3)) {
		soft = lua_toboolean(state, 3);
	}
    lua_pushnumber(state, LUAWND(data)->CmpScreenShot(StrAToStr(pathStrA).c_str(), soft));
    return 1;
}

int CLuaWnd::CmpScreenShotWithEx(lua_State* state)
{
	TrFu;
	int argc = lua_gettop(state);
	TrVar(argc);
	TrRet(argc < 6 || argc > 7, 0);
	SLuaObjData* data = GetData(state, 1, s_className);
	TrRet(!data, 0);
	size_t pathSize = 0;
	const char* pathStrA = luaL_checklstring(state, 2, &pathSize);
	TrRet(!pathStrA, 0);
	BOOL soft = FALSE;
	if (argc == 7 && lua_isboolean(state, 7)) {
		soft = lua_toboolean(state, 7);
	}
	lua_pushnumber(state, LUAWND(data)->CmpScreenShotEx(StrAToStr(pathStrA).c_str(), SRect(luaL_checknumber(state, 3), luaL_checknumber(state, 4), luaL_checknumber(state, 5), luaL_checknumber(state, 6)), soft));
	return 1;
}

int CLuaWnd::SetTesseractPath(lua_State* state)
{
	TrFu;
	s_tesseractAvaliable = false;
	s_tesseractPath.clear();
	s_tesseractOpt = TEXT("--psm 6");
	int argc = lua_gettop(state);
	TrVar(argc);
	TrRet((argc < 1 || argc > 2), 0);
	size_t pathSize = 0;
	const char* pathStrA = luaL_checklstring(state, 1, &pathSize);
	TrRet(!pathStrA, 0);
	Str path = CPath::NormalizePathSeps(StrAToStr(pathStrA).c_str());
	CPath testPath(path);
	TrRet(!testPath.IsFile(), 0);
	s_tesseractPath = path;
	if (argc == 2) {
		const char* op = luaL_checklstring(state, 2, &pathSize);
		if (op) {
			s_tesseractOpt = StrAToStr(op);
		}
	}
	TrStr(s_tesseractOpt);
	s_tesseractAvaliable = true;
	TrStr(s_tesseractPath);
	lua_pushboolean(state, 1);
	return 1;
}

int CLuaWnd::GetTextFromImgFile(lua_State* state)
{
	TrFu;
	int argc = lua_gettop(state);
	TrVar(argc);
	TrRet(argc != 1, 0);
	size_t pathSize = 0;
	const char* pathStrA = luaL_checklstring(state, 1, &pathSize);
	TrRet(!pathStrA, 0);
	Str path = CPath::NormalizePathSeps(StrAToStr(pathStrA).c_str());
	TrRet(!CPath(path).IsFile(), 0);
	lua_pushstring(state, ExecTesseract(CPath::GetFullPath(path)).c_str());
	return 1;
}

int CLuaWnd::GetScreenShotText(lua_State* state)
{
	TrFu;
	int argc = lua_gettop(state);
	TrVar(argc);
	TrRet(argc != 5, 0);
	SLuaObjData* data = GetData(state, 1, s_className);
	TrRet(!data, 0);
	Str imgPath = CPath::GetTmpFilePath() + L".bmp";
	TrRet(!LUAWND(data)->SaveScreenShotEx(imgPath.c_str(), SRect(luaL_checknumber(state, 2), luaL_checknumber(state, 3), luaL_checknumber(state, 4), luaL_checknumber(state, 5))), 0);
	lua_pushstring(state, ExecTesseract(CPath::GetFullPath(imgPath)).c_str());
	return 1;
}

int CLuaWnd::GetClipboardText(lua_State* state)
{
	TrFu;
	StrA txt;
	CWndBase::GetTextFromCL(txt);
	lua_pushstring(state, txt.c_str());
	return 1;
}

int CLuaWnd::SetCompareCut(lua_State* state)
{
	TrFu;
	int argc = lua_gettop(state);
	TrRet(argc != 2, 0);
	SLuaObjData* data = GetData(state, 1, s_className);
	TrRet(!data, 0);
	int back = LUAWND(data)->SetCompareCutPx(luaL_check_int(state, 2));
	lua_pushnumber(state, back);
	return 1;
}
