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

// app.cxx

#include <app.hxx>
#include <file.hxx>

namespace frameMP {

CApp::CApp() : m_msgWnd(NULL)
{
}

CApp::~CApp()
{
}

Str CApp::GetArg(unsigned ind)
{
	if (ind < m_argList.size()) {
		list<Str>::iterator iter = m_argList.begin();
		advance(iter, ind);
		return *iter;
	}
	return Str();
}

Str CApp::GetAppDir()
{
	Str back;
	m_appPath.GetLastDirPath(back);
	return back;
}

Str CApp::GetAppName()
{
	return m_appPath.back();
}

const CApp::ArgList& CApp::GetAllArg()
{
	return m_argList;
}

bool CApp::Initialize(int argc, char *argv[])
{
    TrFu;
	TrRet(m_appPath, false);
	TrRet(argc < 1 || !argv, false);
	m_appPath = CPath(frameMP::StrAToStr(StrA(argv[0])).c_str()).GetFullPath();
    int i = 0;
	for (; i < 2 && i < argc; ++i) {
		m_argList.push_back(CPath(StrAToStr(argv[i]).c_str()).GetFullPath());
	}
    for (; i < argc; ++i) {
		m_argList.push_back(StrAToStr(argv[i]));
	}
	return true;
}

#if defined(MSWIN_OS)
#	include "appWin.inl"
#elif defined(LINUX_OS)
#	include "appLin.inl"
#endif

}; // namespace frameMP
