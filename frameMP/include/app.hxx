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

// app.hxx

#include "mp.hxx"
#include "file.hxx"


#ifndef _APP_HXX_
#define _APP_HXX_


namespace frameMP 
{

class CWndBase;

class CApp  : public TSingl<CApp>
{
	friend struct TSingl<CApp>;
public:
	typedef list<Str> ArgList;
	bool Initialize(int argc, char *argv[]);
	Str GetAppDir();
	Str GetAppName();
	Str GetArg(unsigned ind);
	bool GetCurrWorkDir(Str &dir);
	bool SetCurrWorkDir(const TCHAR *dir);
	const ArgList& GetAllArg();

private:
	static CWndBase* m_sMsgOnlyWnd;
	CApp();
	~CApp();
	CPath m_appPath;
	ArgList m_argList;
	CWndBase* m_msgWnd;
};

}; // namespace FrameMP

#endif //_APP_HXX_
