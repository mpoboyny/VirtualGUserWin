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

// fileLin.cxx

#include <sys/types.h>
#include <dirent.h>
#include <limits.h> /* PATH_MAX */

inline bool IsDirectory(const char *path)
{
	DIR *currDir = opendir(path);
	return currDir? !closedir(currDir): false;
}

bool CPath::IsFile()
{
	if (!IsDirectory(StrToStrA(m_orgPath).c_str())) {
		FILE *currFile = fopen(StrToStrA(m_orgPath).c_str(), "r");
		return currFile? !fclose(currFile): false;
	}
	return false;
}

bool CPath::IsDir()
{
	return IsDirectory(StrToStrA(m_orgPath).c_str());
}

Str CPath::GetFullPath() const
{
	TrRet(!m_orgPath, Str());
	//if (m_orgPath[0] == _T('/')) {
	//	return Str(m_orgPath);
	//}
	char buf[PATH_MAX + 1]; /* not sure about the "+ 1" */
    char *res = realpath(StrToStrA(Str(m_orgPath)).c_str(), buf);
	if (res) {
		return StrAToStr(StrA(res));
	}
	return Str();
}

