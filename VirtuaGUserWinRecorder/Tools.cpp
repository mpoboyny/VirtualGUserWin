#include "pch.h"
#include "framework.h"
#include "Tools.h"
#include <windows.h>
#include <shlobj.h>
#include <atlstr.h>
#include <tchar.h>

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

    CString TempFileName()
    {
        TCHAR tempPath[MAX_PATH] = {0};
        if (GetTempPath(MAX_PATH, tempPath) == 0)
            return _T("");

        TCHAR tempFile[MAX_PATH] = {0};
        if (GetTempFileName(tempPath, _T("VGU"), 0, tempFile) == 0)
            return _T("");

        return CString(tempFile);
    }

    // Hilfsfunktion: gibt nur den Dateinamen (mit Erweiterung) aus einem Pfad zurück.
    CString GetFileNameOnly(const CString& path)
    {
        if (path.IsEmpty())
            return CString();

        int posBack = path.ReverseFind('\\');
        int posSlash = path.ReverseFind('/');
        int lastPos = posBack > posSlash ? posBack : posSlash;

        if (lastPos == -1)
            return path;    

        return path.Mid(lastPos + 1);
    }

    // Gibt nur den Dateinamen zurück (ohne Pfad). Die temporäre Datei wird dabei erstellt und gelöscht,
    // sodass nur der Name reserviert bleibt.
    CString TempOnlyFileName()
    {
        GUID guid;
        // CoCreateGuid liefert RPC_S_OK (== S_OK) bei Erfolg
        if (CoCreateGuid(&guid) != S_OK)
            return CString();

        // StringFromGUID2 schreibt einen Null-terminierten WCHAR-String im Format "{...}"
        OLECHAR wszGuid[40] = {0};
        if (StringFromGUID2(guid, wszGuid, _countof(wszGuid)) == 0)
            return CString();

        CString filename(wszGuid); // enthält z.B. "{xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}"
        // Entferne führende '{' und schließende '}'
        int len = filename.GetLength();
        if (len >= 2 && filename[0] == _T('{') && filename[len - 1] == _T('}'))
        {
            filename = filename.Mid(1, len - 2); // nur der GUID-Teil ohne Klammern
        }

        // Baue finalen Dateinamen: Präfix + GUID + .tmp
        CString result = CString(_T("VGU_")) + filename + CString(_T(".tmp"));
        return result;
    }
}
