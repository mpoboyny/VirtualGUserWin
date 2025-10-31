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

CPrc::CPrc(HANDLE token) : m_token(NULL), m_pid(0)
{
	CleanUp();
	TrVoid(!::DuplicateHandle(token, token, token, &m_token, 0, TRUE, DUPLICATE_SAME_ACCESS));
	m_pid = ::GetProcessId(m_token);
}

CPrc::CPrc(DWORD pid) : m_token(NULL), m_pid(0)
{
	CleanUp();
	m_token = ::OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, pid);
	TrVoid(!m_token);
	m_pid = pid;
}

CPrc::~CPrc()
{
	CleanUp();
}

bool CPrc::CleanUp()
{
	bool back = true;
	if (m_token)
	{
		back = !!::CloseHandle(m_token);
	}
	m_pid = 0;
	m_token = NULL;
	return back;
}

bool CPrc::ExecuteFile(const Str& cmdLine)
{
	TrFu;
	static const DWORD s_waitStep = 100;
	static const int waitCount = (s_defIdleWaitSec * 1000) / s_waitStep;
	LPTSTR szCommandLine = _tcsdup(cmdLine.c_str());
	SHELLEXECUTEINFO info;
	ZeroMemory(&info, sizeof(info));
	info.cbSize = sizeof(info);
	info.fMask = SEE_MASK_NOCLOSEPROCESS | SEE_MASK_FLAG_DDEWAIT;
	info.lpFile = szCommandLine;
	info.nShow = SW_SHOWNORMAL;
	BOOL bRes = ShellExecuteEx(&info);
	if (bRes) {
		DWORD waiteRes = ::WaitForInputIdle(info.hProcess, s_waitStep);
		for (int i = 0; i < waitCount && WAIT_TIMEOUT == s_waitStep; ++i) {
			waiteRes = ::WaitForInputIdle(info.hProcess, s_waitStep);
		}
		if (WAIT_FAILED == waiteRes) {
			Tr("WAIT_FAILED == waiteRes");
			bRes = FALSE;
			::CloseHandle(info.hProcess);
		}
		else {
			m_token = info.hProcess;
			m_pid = GetProcessId(info.hProcess);
		}

	}
	free(szCommandLine);
	TrVar(bRes);
	return !!bRes;
}

bool CPrc::Create(const Str& cmdLine, const Str& param, const Str& file, bool waitForIdle)
{
	TrFu;
	static const DWORD s_waitStep = 100;
	static const int waitCount = (s_defIdleWaitSec * 1000) / s_waitStep;

	Str cmd = cmdLine + _T(" \"") + param + _T("\" \"") + file + _T("\"");
	TrStr(cmd);
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	LPTSTR szCommandLine = _tcsdup(cmd.c_str());
	TrRet(!::CreateProcess(NULL, szCommandLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi), false);
	if (waitForIdle) {
		DWORD waiteRes = ::WaitForInputIdle(pi.hProcess, s_waitStep);
		for (int i = 0; i < waitCount && WAIT_TIMEOUT == s_waitStep; ++i) {
			waiteRes = ::WaitForInputIdle(pi.hProcess, s_waitStep);
		}
		if (WAIT_FAILED == waiteRes) {
			Tr("WAIT_FAILED == waiteRes");
			::CloseHandle(pi.hProcess);
			::CloseHandle(pi.hThread);
			free(szCommandLine);
			return false;
		}
	}
	m_token = pi.hProcess;
	m_pid = pi.dwProcessId;
	::CloseHandle(pi.hThread);
	free(szCommandLine);
	return true;
}

bool CPrc::Create(const Str& cmdLine, const Str param[], unsigned numParam, BOOL inheritParentDescs)
{
	static const DWORD s_waitStep = 100;
	static const int waitCount = (s_defIdleWaitSec * 1000) / s_waitStep;

	Str newParam = param[0];
	LPCTSTR blank = _T(" ");
	for (unsigned i = 0; i < numParam; ++i) {
		newParam += blank;
		newParam += param[i];
	}
	Str cmd = cmdLine + _T(" ") + newParam;
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;
	LPTSTR szCommandLine = _tcsdup(cmd.c_str());
	TrRet(!::CreateProcess(NULL, szCommandLine, NULL, NULL, inheritParentDescs, 0, NULL, NULL, &si, &pi), false);
	DWORD waiteRes = ::WaitForInputIdle(pi.hProcess, s_waitStep);
	for (int i = 0; i < waitCount && WAIT_TIMEOUT == s_waitStep; ++i) {
		waiteRes = ::WaitForInputIdle(pi.hProcess, s_waitStep);
	}
	if (WAIT_FAILED == waiteRes) {
		Tr("WAIT_FAILED == waiteRes");
		::CloseHandle(pi.hProcess);
		::CloseHandle(pi.hThread);
		free(szCommandLine);
		return false;
	}
	m_token = pi.hProcess;
	m_pid = pi.dwProcessId;
	::CloseHandle(pi.hThread);
	free(szCommandLine);
	return true;
}

bool CPrc::Close()
{
	return CleanUp();
}

bool CPrc::IsRunning()
{
	TrFu;
	TrVar(m_pid);
	HANDLE hPrc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, m_pid);
	TrVar(hPrc);
	if (!hPrc) {
		Tr("OpenProcess - FAILED");
		return false;
	}
	DWORD exitCode;
	if (!GetExitCodeProcess(hPrc, &exitCode)) {
		Tr("GetExitCodeProcess - FAILED, is running");
		CloseHandle(hPrc);
		return true;
	}
	TrVar(exitCode);
	::CloseHandle(hPrc);
	return exitCode == STILL_ACTIVE;
}

SWinInfo CPrc::GetProcessWinInfo()
{
	SWinInfo info;
	HWND hFirst = GetTopWindow(NULL), hCurr = hFirst;
	DWORD currPid = 0;
	do {
		if (::IsWindowVisible(hCurr)) {
			GetWindowThreadProcessId(hCurr, &currPid);
			if (m_pid == currPid) {
				info.m_hWnd = hCurr;
				break;
			}
		}
		hCurr = GetNextWindow(hCurr, GW_HWNDNEXT);
	} while (hCurr && hCurr != hFirst);
	return info;
}

WinInfoList CPrc::GetProcessAllWinInfo()
{
	WinInfoList allWnd;
	HWND hFirst = GetTopWindow(NULL), hCurr = hFirst;
	DWORD currPid = 0;
	do {
		if (::IsWindowVisible(hCurr)) {
			GetWindowThreadProcessId(hCurr, &currPid);
			if (m_pid == currPid) {
				SWinInfo info;
				info.m_hWnd = hCurr;
				allWnd.push_back(info);
			}
		}
		hCurr = GetNextWindow(hCurr, GW_HWNDNEXT);
	} while (hCurr && hCurr != hFirst);
	return allWnd;
}

bool CPrc::Terminate(unsigned exitCode)
{
	return !!::TerminateProcess(m_token, exitCode);
}

void CPrc::SleepMs(unsigned milliSec)
{
	::Sleep(milliSec);
}

}; // namespace frameMP
