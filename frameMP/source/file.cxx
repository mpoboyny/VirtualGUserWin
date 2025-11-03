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

// file.cxx

#include <mp.hxx>
#include <file.hxx>

#pragma warning(disable: 4996)

#pragma push_macro("_CRT_SECURE_NO_WARNINGS")  
#undef _CRT_SECURE_NO_WARNINGS  

#define TMP_FILE_PREF TEXT("FMP")


using namespace std;

namespace frameMP {

CPath::CPath(const TCHAR *path, TCHAR sep) 
            : m_orgPath(NULL), m_pathListContainer(NULL), m_sep(sep), m_isRelPath(false)
{
    InitPath(path, sep);
}

CPath::CPath(const Str &path, TCHAR sep) 
            : m_orgPath(NULL), m_pathListContainer(NULL), m_sep(sep), m_isRelPath(false)
{
    InitPath(path.c_str(), sep);
}

CPath::CPath(const CPath& rhs) : m_orgPath(rhs.m_orgPath?_tcsdup(rhs.m_orgPath):NULL),
                                 m_pathListContainer(NULL), m_sep(rhs.m_sep), m_isRelPath(false)
{
    Init();
}

CPath::~CPath()
{
    free(m_pathListContainer);
    free((void*)m_orgPath);
}

Str CPath::NormalizePathSeps(const TCHAR *osPath)
{
    Str back;
    TCHAR replaceSep = MP_PATH_SEP;
    TCHAR searchSep = MP_NXPATH_SEP;
    TCHAR *newPath = _tcsdup(osPath);
    for (size_t i=0, iCount = _tcslen(newPath); i < iCount; ++i) {
        if (newPath[i] == searchSep) newPath[i] = replaceSep;
    }
    back = newPath? newPath: _T("ups, bad path format ?");
    free(newPath);
    return back;
}

void CPath::Init()
{
#define MP_CPATH_REL	if (!m_isRelPath) { \
                            tokLen = _tcslen(tokLast); \
                            if (tokLen && tokLen <3 && tokLast[0] == _T('.')) { \
                                if (tokLen == 1 || tokLast[1] == _T('.')) m_isRelPath = true; \
                            } \
                        }
    //////////////////////////
    if(!m_orgPath) {
        return;
    }
    size_t tokLen = 0;
    m_pathListContainer = _tcsdup(m_orgPath);
    TCHAR *tokLast = m_pathListContainer;
    for (TCHAR *tok = m_pathListContainer; *tok; ++tok) {
        if (*tok == m_sep) {
            *tok = _T('\0');
            if (tokLast) {
                push_back(tokLast);
                MP_CPATH_REL;
            }
            tokLast = tok +1;
        }
    }
    if (tokLast) {
        push_back(tokLast);
        MP_CPATH_REL;
    }
#undef MP_CPATH_REL
}

void CPath::InitPath(const TCHAR *path, TCHAR sep)
{
    if (path) {
        if (!_tcsrchr(path, m_sep)) {
            size_t len = _tcslen(path);
            TrVoid(!ReAlloc(&m_orgPath, len * sizeof(TCHAR), (len + 3) * sizeof(TCHAR)));
            ((TCHAR*)m_orgPath)[0] = _T('.'); ((TCHAR*)m_orgPath)[1] = m_sep;
            _tcscpy((TCHAR*)(m_orgPath + 2), path);
        }
        else {
            m_orgPath = _tcsdup(path);
        }
        Init();
    }
}

void CPath::operator=(const CPath& rhs)
{
    free(m_pathListContainer);
    free((void*)m_orgPath);
    m_orgPath = rhs.m_orgPath? _tcsdup(rhs.m_orgPath): NULL;
    m_sep = MP_PATH_SEP;
    m_isRelPath = false;
    Init();
}

void CPath::operator=(const Str& rhs)
{
    free(m_pathListContainer);
    free((void*)m_orgPath);
    m_orgPath = _tcsdup(rhs.c_str());
    m_isRelPath = false;
    Init();
}

void CPath::operator+=(const TCHAR *addPathPart)
{
    TCHAR sep = MP_PATH_SEP;
    Str normPart = NormalizePathSeps(addPathPart);
    size_t partLen = normPart.length();
    size_t oldLen = _tcslen(m_orgPath);
    size_t newLen = partLen + oldLen + 1;
    const TCHAR *oldPath = m_orgPath;
    m_orgPath = (TCHAR*)realloc((void*)m_orgPath, (newLen + 1) * sizeof(TCHAR));
    if (!m_orgPath)
    {
        m_orgPath = oldPath;
        TrVoid(m_orgPath == oldPath);
    }
    TCHAR *newPath = (TCHAR*)(&m_orgPath[oldLen]);
    if (normPart[0] != sep && *newPath != sep) {
        *newPath = sep;
        ++newPath;
    }
    else if (normPart[0] == sep && *newPath == sep)
    {
        --newPath;
    }
    *newPath = _T('\0');
    _tcscat(newPath, normPart.c_str());
    free(m_pathListContainer);
    m_isRelPath = false;
    Init();
}

bool CPath::GetLastDirPath(Str &dirPath)
{
    if (IsDir()) {
        dirPath = m_orgPath;
        return true;
    }
    if (IsFile()) {
        dirPath = m_orgPath;
        size_t lastSep = dirPath.find_last_of(m_sep);
        dirPath.erase(lastSep, Str::npos);
        return true;
    }
    return false;
}

Str CPath::GetFullPath(const Str &path)
{
    return CPath(path.c_str()).GetFullPath();
}

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
    TCHAR  buffer[dBufsize] = TEXT("");
    TCHAR** lppPart = { NULL };
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

int SFile::Open(const Str& fileToOpen, int oflag, int pmode)
{
    return _wopen(StrToStrW(fileToOpen).c_str(), oflag, pmode);
}

size_t SFile::Read(int fd, void* buffer, size_t count)
{
    return _read(fd, buffer, (unsigned)count);
}

int SFile::Write(int fd, void* buffer, size_t count)
{
    return _write(fd, buffer, (unsigned)count);
}

bool SFile::Close(int fd)
{
    return !!_close(fd);
}

size_t SFile::GetFileSize(int fd)
{
    size_t iSize = _lseek(fd, 0, SEEK_END);
    _lseek(fd, 0, SEEK_SET);
    return iSize;
}

size_t SFile::ReadFileToStr(const Str& filePath, StrA& str)
{
	int fd = Open(filePath.c_str());
	size_t fSize = GetFileSize(fd);
	TrRet(fd <= 0 || fSize < 1, 0);
	TDel<char> pFileBuf(new char[fSize + 1]);
	TrRet(SFile::Read(fd, *pFileBuf, fSize) < 1, 0);
	(*pFileBuf)[fSize] = '\0';
	str = *pFileBuf;
	Close(fd);
	return fSize;
}

}; // namespace frameMP

#pragma pop_macro("_CRT_SECURE_NO_WARNINGS")

