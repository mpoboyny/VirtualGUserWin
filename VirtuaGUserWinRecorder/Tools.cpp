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
}
