// 
// This file is part of ClickerMP project.
// Copyright ©2013 Mykhaylo Poboynyy <mpoboyny@googlemail.com>

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

// main.cxx
#include <main.hxx>
#include <frameMP/app.hxx>
#include <frameMP/file.hxx>
#include <frameMP/Lua/luaRuntime.hxx>
#include <frameMP/Lua/luaRtObj.hxx>
#include "luaWnd.hxx"
#include "luaPrc.hxx"


inline void ShowUsage()
{
    printf("\nUsage: %ls <LUA FILE> [-LUA FILE PARAMETER...]", StrToStrW(CApp::Inst().GetAppName()).c_str());
    printf("\n\tLUA FILE PARAMETER - is optional. It can be more than 1 if any.\n\n");
}

int main(int argc, char **argv)
{
    TrFu;
	TrFVar(setlocale(LC_CTYPE, ""));
    Str appDir, fileDir, luaFile;
    TrRet(!CApp::Inst().Initialize(argc, argv), 1);
    if (argc < 2 || (argc > 1 && !strncmp(argv[1], "--help", min((int)strlen(argv[1]), (int)6))))
    {
        ShowUsage();
#ifdef _MPTRACE_
    printf("\n\nPress any key to continue...");
    getchar();
#endif
        return 0;
    }
    appDir = CApp::Inst().GetAppDir();
    //TrStr(appDir);
    luaFile = CApp::Inst().GetArg(1);
    //TrStr(luaFile);
    CPath filePath(luaFile.c_str());
    TrRet(!filePath.GetLastDirPath(fileDir), 1);
    //TrStr(fileDir);
    CApp::Inst().SetCurrWorkDir(fileDir.c_str());
    PCLuaRuntime pRt(new CLuaRuntime);
    CLuaWnd::Register(*pRt);
    CLuaPrc::Register(*pRt);
	CLuaRtObj::Register(*pRt);
    pRt->DoFile(StrToStrA(luaFile).c_str());
//#ifdef _MPTRACE_
//    printf("\n\nPress any key to continue...");
//    getchar();
//#endif
#ifndef _MPTRACE_
	CPath::RemoveAllTmpFiles();
#endif
    CApp::Inst(TSinglDestroy);
    return 0;
}
