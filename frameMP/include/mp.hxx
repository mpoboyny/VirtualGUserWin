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

// mp.hxx

#ifndef _MP_HXX_
#define _MP_HXX_

#define WINVER 0x0601       // Specifies Windows 7
#define _WIN32_WINNT 0x0601 // Specifies Windows 7

#ifndef UNICODE
#	define UNICODE
#endif
#ifndef _UNICODE
#	define _UNICODE
#endif


#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>
#include <direct.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>

#include "str.hxx"
#include "trace.hxx"
#include "error.hxx"
#include "tools.hxx"


namespace frameMP 
{
}; // namespace frameMP

#endif //_MP_HXX_
