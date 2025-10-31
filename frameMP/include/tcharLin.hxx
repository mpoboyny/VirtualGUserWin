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

//tcharLin.hxx

#ifndef _TCHARLIN_HXX_
#define _TCHARLIN_HXX_

#ifdef _UNICODE
#    include <wchar.h>
// string
#	define _tcscat     wcscat
#	define _tcscpy     wcscpy
#	define _tcsncpy    wcsncpy
#	define _tcsdup     wcsdup
#	define _tcslen     wcslen   
#	define _tcsxfrm    wcsxfrm
#	define _tcserror   strerror
#	define _tcsrchr    wcsrchr
#	define _tcschr	   wcschr 
#	define _tcsstr	   wcsstr 
#else //not def _UNICODE
// string
#	define _tcscat     strcat
#	define _tcscpy     strcpy
#	define _tcsncpy    strncpy
#	define _tcsdup     strdup
#	define _tcslen     strlen
#	define _tcsxfrm    strxfrm
#	define _tcserror   strerror
#	define _tcsrchr    strrchr
#	define _tcschr	   strchr 
#	define _tcsstr	   strstr 
#endif //_UNICODE

#endif //_TCHARLIN_HXX_

