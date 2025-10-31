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

// prc.cxx

#include <prc.hxx>
#include <file.hxx>

namespace frameMP 
{

CPrc::CPrc() : m_token(0), m_pid(0)
{
}

#if defined(MSWIN_OS)
#	include "prcWin.inl"
#elif defined(LINUX_OS)
#	include "prcLin.inl"
#endif

}; // namespace frameMP
