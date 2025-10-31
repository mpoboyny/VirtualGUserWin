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

// fileWin.inl

bool CPath::IsFile()
{
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(m_orgPath, &ffd);
    TrRet(hFind == INVALID_HANDLE_VALUE, false);
    FindClose(hFind);
    return !(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

bool CPath::IsDir()
{
    WIN32_FIND_DATA ffd;
    HANDLE hFind = FindFirstFile(m_orgPath, &ffd);
    TrRet(hFind == INVALID_HANDLE_VALUE, false);
    FindClose(hFind);
    return !!(ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);
}

Str CPath::GetFullPath() const
{
    static const DWORD dBufsize = 4096;
    TCHAR  buffer[dBufsize]=TEXT(""); 
    TCHAR** lppPart={NULL};
    DWORD  retval = GetFullPathName(m_orgPath, dBufsize, buffer, lppPart);
    if (retval > 0) {
        return Str(buffer);
    }
    return Str();
}

Str CPath::GetTmpFilePath()
{
	Str res;
	TCHAR lpTempPathBuffer[MAX_PATH];
	TCHAR szTempFileName[MAX_PATH];
	DWORD dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
	TrRet((dwRetVal > MAX_PATH || dwRetVal == 0), res);
	UINT   uRetVal = GetTempFileName(lpTempPathBuffer, TMP_FILE_PREF, 0, szTempFileName);
	TrRet(uRetVal == 0, res);
	res = szTempFileName;
	return res;
}

void CPath::RemoveAllTmpFiles()
{
	TCHAR lpTempPathBuffer[MAX_PATH];
	DWORD dwRetVal = GetTempPath(MAX_PATH, lpTempPathBuffer);
	TrVoid((dwRetVal > MAX_PATH || dwRetVal == 0));
	TrVar(lpTempPathBuffer);
	Str searchPath = lpTempPathBuffer;
	searchPath += '*';
	WIN32_FIND_DATA findFileData;
	HANDLE hFind = FindFirstFile(searchPath.c_str(), &findFileData);
	TrVoid(hFind == INVALID_HANDLE_VALUE);
	do {
		if (_tcsnccmp(findFileData.cFileName, TMP_FILE_PREF, _tcslen(TMP_FILE_PREF)) == 0) {
			TCHAR fullPath[MAX_PATH];
			_sntprintf(fullPath, sizeof(fullPath), TEXT("%s\\%s"), lpTempPathBuffer, findFileData.cFileName);
			if (!DeleteFile(fullPath)) {
				Tr(TEXT("Error while deleting: ") << fullPath);
			}
		}
	} while (FindNextFile(hFind, &findFileData) != 0);

	FindClose(hFind);
}
