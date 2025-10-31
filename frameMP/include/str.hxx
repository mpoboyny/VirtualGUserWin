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

// str.hxx

#ifndef _MPSTR_HXX_
#define _MPSTR_HXX_

#include <string>
#include <sstream>
#include <cstring>
#include <list>

namespace frameMP 
{
	using namespace std;
#	ifdef _WIN32 
#		include <tchar.h>
#   else
#   	include "tcharLin.hxx"
#	endif
#	ifndef TCHAR
#		ifdef _UNICODE
#			define TCHAR wchar_t
#		else
#			define TCHAR char
#		endif
#	endif
#   ifndef LPCTSTR
#		define LPCTSTR const TCHAR*
#	endif 
	typedef basic_string <wchar_t>        StrW;
	typedef basic_istringstream<wchar_t>  InStrW;
	typedef basic_ostringstream<wchar_t>  OutStrW;

	typedef basic_string <char>       StrA;
	typedef basic_istringstream<char> InStrA;
	typedef basic_ostringstream<char> OutStrA;

	typedef basic_string <TCHAR>       Str;
	typedef basic_istringstream<TCHAR> InStr;
	typedef basic_ostringstream<TCHAR> OutStr;
	
#	ifndef _T
#		ifdef _UNICODE
#			define _T(str) L##str
#		else
#			define _T(str) (str)
#		endif
#	endif

	StrA StrToStrA(const Str &inStr);
	Str StrAToStr(const StrA &inStr);
	StrW StrToStrW(const Str &inStr);
	Str StrWToStr(const StrW &inStr);

    typedef list<Str> StrList;

}; // namespace frameMP

#endif //_MPTOOLS_HXX_
