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

//str.cxx

#include <mp.hxx>

namespace frameMP 
{

StrA StrToStrA(const Str &inStr)
{
	StrA back;
	if (sizeof(TCHAR) == sizeof(char)) {
		back = (const char*)inStr.c_str();
	}
#if defined(MSWIN_OS)
	else {
		int nLenOfInStr = WideCharToMultiByte(CP_OEMCP, 0, (LPCWSTR)inStr.c_str(), -1, NULL, 0, NULL, NULL);
		char *pMultiByteStr = (char*)HeapAlloc(GetProcessHeap(), 0, nLenOfInStr);
		WideCharToMultiByte(CP_OEMCP, 0, (LPCWSTR)inStr.c_str(), -1, pMultiByteStr, nLenOfInStr, NULL, NULL);
		back = pMultiByteStr;
		HeapFree(GetProcessHeap(), 0, pMultiByteStr);
	}
#elif defined(LINUX_OS)
	else {
		int nLenOfInStr = wcstombs(NULL, (wchar_t*)inStr.c_str(), 0) + 1;
		char *pMultiByteStr = (char*)calloc(nLenOfInStr, nLenOfInStr * sizeof(char));
		wcstombs(pMultiByteStr, (wchar_t*)inStr.c_str(), nLenOfInStr * sizeof(char));
		back = pMultiByteStr;
		free(pMultiByteStr);
	}
#endif
	return back;
}

Str StrAToStr(const StrA &inStr)
{
	Str back;
	if (sizeof(TCHAR) == sizeof(char)) {
		back = (const TCHAR*)inStr.c_str();
	}
#if defined(MSWIN_OS)
	else {
		int nLenOfWideCharStr = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)inStr.c_str(), -1, NULL, 0);
		wchar_t *pWideCharStr = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, nLenOfWideCharStr * sizeof(wchar_t));
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)inStr.c_str(), -1, pWideCharStr, nLenOfWideCharStr);
		back = pWideCharStr;
		HeapFree(GetProcessHeap(), 0, pWideCharStr);
	}
#elif defined(LINUX_OS)
	else {
		int nLenOfWideCharStr = mbstowcs(NULL, (char*)inStr.c_str(), 0) + 1;
		wchar_t *pWideCharStr = (wchar_t*)calloc(nLenOfWideCharStr, nLenOfWideCharStr * sizeof(wchar_t));
		mbstowcs(pWideCharStr, (char*)inStr.c_str(), nLenOfWideCharStr * sizeof(wchar_t));
		back = (TCHAR*)pWideCharStr;
		free(pWideCharStr);
	}
#endif
	return back;
}

StrW StrToStrW(const Str &inStr)
{
	StrW back;
	if (sizeof(TCHAR) == sizeof(wchar_t)) {
		back = (const wchar_t*)inStr.c_str();
	}
#if defined(MSWIN_OS)
	else {
		int nLenOfWideCharStr = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)inStr.c_str(), -1, NULL, 0);
		wchar_t *pWideCharStr = (wchar_t*)HeapAlloc(GetProcessHeap(), 0, nLenOfWideCharStr * sizeof(wchar_t));
		MultiByteToWideChar(CP_ACP, 0, (LPCSTR)inStr.c_str(), -1, pWideCharStr, nLenOfWideCharStr);
		back = pWideCharStr;
		HeapFree(GetProcessHeap(), 0, pWideCharStr);
	}
#elif defined(LINUX_OS)
	else {
		int nLenOfWideCharStr = mbstowcs(NULL, (char*)inStr.c_str(), 0) + 1;
		wchar_t *pWideCharStr = (wchar_t*)calloc(nLenOfWideCharStr, nLenOfWideCharStr * sizeof(wchar_t));
		mbstowcs(pWideCharStr, (char*)inStr.c_str(), nLenOfWideCharStr * sizeof(wchar_t));
		back = pWideCharStr;
		free(pWideCharStr);
	}
#endif
	return back;
}

Str StrWToStr(const StrW &inStr)
{
	Str back;
	if (sizeof(TCHAR) == sizeof(wchar_t)) {
		back = (const TCHAR*)inStr.c_str();
	}
#if defined(MSWIN_OS)
	else {
		int nLenOfInStr = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)inStr.c_str(), -1, NULL, 0, NULL, NULL);
		char *pMultiByteStr = (char*)HeapAlloc(GetProcessHeap(), 0, nLenOfInStr);
		WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)inStr.c_str(), -1, pMultiByteStr, nLenOfInStr, NULL, NULL);
		back = (const TCHAR*) pMultiByteStr;
		HeapFree(GetProcessHeap(), 0, pMultiByteStr);
	}
#elif defined(LINUX_OS)
	else {
		int nLenOfInStr = wcstombs(NULL, (wchar_t*)inStr.c_str(), 0) + 1;
		char *pMultiByteStr = (char*)calloc(nLenOfInStr, nLenOfInStr * sizeof(char));
		wcstombs(pMultiByteStr, (wchar_t*)inStr.c_str(), nLenOfInStr * sizeof(char));
		back = (const TCHAR*) pMultiByteStr;
		free(pMultiByteStr);
	}
#endif
	return back;
}

}; //frameMP
