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

// lin.hxx

#ifndef _LIN_HXX_
#define _LIN_HXX_

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <limits.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

namespace frameMP
{
typedef unsigned long int HWND;
typedef unsigned int UINT;
typedef unsigned long WPARAM;
typedef unsigned long LPARAM;
typedef unsigned long LRESULT;
typedef int BOOL;
#define FALSE 0
#define TRUE  1
typedef void* HANDLE;
typedef HANDLE  PrcToken;
typedef pid_t  PrcId;
typedef unsigned short WORD;
}; // namespace frameMP

#endif //_LIN_HXX_

