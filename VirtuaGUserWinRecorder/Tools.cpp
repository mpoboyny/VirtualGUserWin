#include "pch.h"
#include "framework.h"
#include "Tools.h"
#include <windows.h>
#include <shlobj.h>

namespace Tools
{
    CString GetModulePath(HMODULE hModule)
    {
        TCHAR path[MAX_PATH] = {0};
        DWORD len = ::GetModuleFileName(hModule, path, MAX_PATH);
        return CString(path, len);
    }

    CString GetModuleDirectory(HMODULE hModule)
    {
        CString modulePath = GetModulePath(hModule);
        int lastBackslash = modulePath.ReverseFind('\\');
        if (lastBackslash != -1)
        {
            return modulePath.Left(lastBackslash);
        }
        return CString();
    }

    BOOL CreateDirectoryRecursive(const CString& path)
    {
        if (path.IsEmpty())
            return FALSE;
        TCHAR buffer[MAX_PATH] = {0};
        _tcscpy_s(buffer, path);
        for (TCHAR* p = buffer + 1; *p; p++)
        {
            if (*p == '\\' || *p == '/')
            {
                *p = 0;
                if (!CreateDirectory(buffer, NULL))
                {
                    if (GetLastError() != ERROR_ALREADY_EXISTS)
                        return FALSE;
                }
                *p = '\\';
            }
        }
        if (!CreateDirectory(buffer, NULL))
        {
            if (GetLastError() != ERROR_ALREADY_EXISTS)
                return FALSE;
        }
        return TRUE;
    }

    CString GetUserDocumentsPath()
    {
        CString path;
        PWSTR pszPath = nullptr;
        if (SUCCEEDED(SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pszPath)))
        {
            path = pszPath;
            CoTaskMemFree(pszPath);
        }
        return path;
    }

    BOOL IsFolderEmpty(const CString& folderPath)
    {
        WIN32_FIND_DATA findFileData;
        HANDLE hFind = INVALID_HANDLE_VALUE;

        CString searchPath = folderPath;
        if (!searchPath.IsEmpty() && searchPath.Right(1) != _T("\\"))
            searchPath += _T("\\");
        searchPath += _T("*");

        hFind = FindFirstFile(searchPath, &findFileData);

        if (hFind == INVALID_HANDLE_VALUE)
            return TRUE; // Ordner existiert nicht oder ist leer

        BOOL isEmpty = TRUE;
        do
        {
            CString fileName = findFileData.cFileName;
            if (fileName != _T(".") && fileName != _T(".."))
            {
                isEmpty = FALSE;
                break;
            }
        } while (FindNextFile(hFind, &findFileData) != 0);

        FindClose(hFind);
        return isEmpty;
    }

    BOOL IsFolderExist(const CString& folderPath)
    {
        DWORD attrib = GetFileAttributes(folderPath);
        return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
    }
}
