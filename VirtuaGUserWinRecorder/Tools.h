//
// Tools.h
//

#pragma once

namespace Tools
{
    CString GetModulePath(HMODULE hModule);
    CString GetModuleDirectory(HMODULE hModule);
    BOOL CreateDirectoryRecursive(const CString& path);
    CString GetUserDocumentsPath();
    BOOL IsFolderExist(const CString& folderPath);
    BOOL IsFolderEmpty(const CString& folderPath);
    CString TempFileName();
    CString TempOnlyFileName();
}
