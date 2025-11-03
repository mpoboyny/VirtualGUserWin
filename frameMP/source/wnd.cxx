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

// wnd.cxx

#include <frameMP/prc.hxx>
#include <frameMP/file.hxx>
#include <stack>
#include <map>
#include <math.h>

namespace frameMP
{

CWndBase::DispList CWndBase::m_dispList;
unsigned CWndBase::s_InstCount = 0;
CWndBase::XKeyMap CWndBase::s_KeyNameToKey;

CWndBase CWndBase::s_staticInit;

CWndBase::CWndBase() : m_hWnd(NULL), m_hWndPar(NULL), m_iCompareCutPx(0)
{
    memset(&m_winInfo, 0, sizeof(SWinInfo));
    if (s_InstCount == 1)
    {
        TrVoid(!Init());
    }
}

CWndBase::CWndBase(const SWinInfo &info)
            : m_hWnd(info.m_hWnd), m_hWndPar(info.m_hPar), m_winInfo(info), m_iCompareCutPx(0)
{
    if (s_InstCount == 1)
    {
        TrVoid(!Init());
    }
}

CWndBase::~CWndBase()
{
    if (s_InstCount == 1)
    {
        Clean();
    }
}

HWND CWndBase::GetHwnd() const
{
    return m_hWnd;
}

HWND CWndBase::GetParHwnd()
{
    return m_hWndPar;
}

BOOL CWndBase::OnWndMsg(UINT &uMsg, WPARAM &wParam, LPARAM &lParam)
{
    return FALSE;
}


WinInfoList CWndBase::FindWindowByTitle(const TCHAR *title)
{
    return FindWindowByTitleText(title, eWithTitleExakt);
}

WinInfoList CWndBase::FindWindowByTextInTitle(const TCHAR *title)
{
    return FindWindowByTitleText(title, eWithTitleWithText);
}

BOOL CWndBase::SaveScreenShot(LPCTSTR filePath)
{
    TrFu;
	return SaveScreenShotEx(filePath, SRect());
}

BOOL CWndBase::SaveScreenShotEx(LPCTSTR filePath, const SRect& rc)
{
	TrFu;
	TrVar(filePath);
	BOOL back = FALSE;
	void* pData = NULL;
	size_t dataSize = 0, byteSaved = 0;
	int iFd = 0;
	back = GetScreenShot(pData, dataSize, CutWndRect(rc));
	if (back) {
		back = iFd = SFile::Open(filePath, MP_O_BINARY | MP_O_CREATE | MP_O_RDWR | MP_O_TRUNC, MP_S_WRITE);
	}
	if (iFd && dataSize) {
		byteSaved = SFile::Write(iFd, pData, dataSize);
		back = dataSize == byteSaved;
	}
	free(pData);
	SFile::Close(iFd);
	TrRet(!back, FALSE);
	return back;
}

int CWndBase::SetCompareCutPx(int val)
{
	TrFu;
	int back = m_iCompareCutPx;
	m_iCompareCutPx = val;
	TrVar(back);
	TrVar(m_iCompareCutPx);
	return back;
}

double CWndBase::CmpScreenShot(LPCTSTR filePath, BOOL soft)
{
    TrFu;
	return CmpScreenShotEx(filePath, SRect(), soft);
}

// is in wndWin.inl
extern double CompareBmpData(void* dataBmp1, size_t dataBmp1Size, void* dataBmp2, size_t dataBmp2Size);

double CWndBase::CmpScreenShotEx(LPCTSTR filePath, const SRect& rc, BOOL soft)
{
	TrFu;
	TrVar(filePath);
	TrVar(soft)
	CPath path(filePath);
	TrRet(!path.IsFile(), 0.);
	double percentEqual = 0.;
	TrVar(m_iCompareCutPx);
	int iFdRef = 0;
	size_t dataSize = 0, refFsize = 0, refBytesRead = 0;
	void* pData = NULL, * pRefBytes = NULL;
	iFdRef = SFile::Open(filePath);
	TrRet(!iFdRef, 0.);
	refFsize = SFile::GetFileSize(iFdRef);
	if (refFsize && GetScreenShot(pData, dataSize, CutWndRect(rc, m_iCompareCutPx)) && frameMP::ReAlloc(&pRefBytes, 0, refFsize)
		&& 0 < (refBytesRead = SFile::Read(iFdRef, pRefBytes, refFsize))) {
		if (soft) {
			percentEqual = CompareBmpData(pData, dataSize, pRefBytes, refBytesRead);
		}
		else {
			if (frameMP::MemCmp(pData, dataSize, pRefBytes, refFsize, percentEqual)) {
				percentEqual = 100.0;
			}
		}
		if (percentEqual < 100.0)
		{
			percentEqual *= pow(10., 5);
			percentEqual = ceil(percentEqual);
			percentEqual /= pow(10., 5);
		}
	}
	else {
		Tr("Check failed!");
	}
	TrVar(percentEqual);
	::free(pData);
	::free(pRefBytes);
	SFile::Close(iFdRef);
	//TrVar(percentEqual);
	return percentEqual;
}

#if defined(MSWIN_OS)
#	include "wndWin.inl"
#elif defined(LINUX_OS)
#	include "wndLin.inl"
#endif

}; // namespace frameMP

