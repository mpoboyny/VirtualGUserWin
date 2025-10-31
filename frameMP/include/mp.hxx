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

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#ifndef __GNUC__
#	pragma warning( disable : 4267 )
#	pragma warning( disable : 4244 )
#	pragma warning( disable : 4311 )
#	pragma warning( disable : 4312 )
#	pragma warning( disable : 4996 )
#endif // __GNUC__

namespace frameMP 
{
}; // namespace frameMP

#if defined(MSWIN_OS)
#	include "win.hxx"
#elif defined(LINUX_OS)
#	include "lin.hxx"
#endif

#include "str.hxx"
#include "trace.hxx"
#include "error.hxx"
#include "tools.hxx"

#endif //_MP_HXX_
