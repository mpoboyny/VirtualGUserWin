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

// prcLin.inl

extern unsigned char *GetWindowAtomProperty(Display *dpy, Window wnd, Atom atom,
                                     unsigned long *itemNum, Atom *type, int *size);
extern BOOL FetchTitle(Display *dpy, Window w, char ** winname);

CPrc::CPrc(PrcToken token) : m_token(token), m_pid(0)
{
}

CPrc::CPrc(PrcId pid) : m_token(NULL), m_pid(pid)
{
}

CPrc::~CPrc()
{
}

bool CPrc::ExecuteFile(const Str &cmdLine)
{
	TrFu;
	Tr("Not implemented yet!");
}

bool CPrc::Create(const Str &cmdLine, const Str &param, const Str &file, bool waitForIdle)
{
    //TrFu;
    char *argv[4] = {NULL, NULL, NULL};
    if (cmdLine.length()) {
        argv[0] = strdup(StrToStrA(cmdLine).c_str());
    }
    if (param.length()) {
        argv[1] = strdup(StrToStrA(param).c_str());
    }
    if (file.length()) {
        argv[2] = strdup(StrToStrA(file).c_str());
    }
#ifndef _MPTRACE_
    for (int fd = 0, fdcount=getdtablesize(); fd < fdcount; ++fd) {
        if (0 < fcntl(fd, F_SETFD, FD_CLOEXEC)) {
            Tr("0 < fcntl(fd, F_SETFD, FD_CLOEXEC), fd: "<<fd);
        }
    }
#endif
    pid_t pid = fork();
    if (!pid) {
        if (execvp(argv[0], argv) == -1) {
            Tr("execvp == -1, cmdLine: "<<argv[0]);
        }
        return 0;
    }
    else {
        //TrVar(pid);
        TrRet(pid < 0, false);
        free(argv[0]);
        free(argv[1]);
        free(argv[2]);
        m_pid = pid;
        sleep(s_defIdleWaitSec);
        return true;
    }
    return false;
}

bool CPrc::Create(const Str &cmdLine, const Str param[], unsigned numParam, BOOL inheritParentDescs)
{
    size_t argc = 1 + numParam;
    // TrVar(argc);
    char **argv = (char**)calloc((argc + 1), sizeof(char*));
    argv[0] = strdup(StrToStrA(cmdLine).c_str());
    for (size_t i = 1, j= 0; i < argc; ++i, ++j) {
        argv[i] = strdup(StrToStrA(param[j]).c_str());
    }
    if (!inheritParentDescs) {
        for (int fd = 0, fdcount=getdtablesize(); fd < fdcount; ++fd) {
            if (0 < fcntl(fd, F_SETFD, FD_CLOEXEC)) {
                Tr("0 < fcntl(fd, F_SETFD, FD_CLOEXEC), fd: "<<fd);
            }
        }
    }
    pid_t pid = fork();
    if (!pid) {
        if (execvp(argv[0], argv) == -1) {
            Tr("execvp == -1, cmdLine: "<<argv[0]);
        }
        return 0;
    }
    else {
        // TrVar(pid);
        for (size_t i = 0; i < argc; ++i) {
            free(argv[i]);
        }
        free(argv);
        m_pid = pid;
        sleep(s_defIdleWaitSec);
        TrRet(pid < 0, false);
        return true;
    }
    return false;
}

bool CPrc::Close()
{
    TrFu;
    Tr("Not implemented!");
    return true;
}

bool CPrc::IsRunning()
{
    TrFu;
    static const int s_maxPath = 1024;
    TrVar(m_pid);
    OutStr pathOutStr;
    pathOutStr<<_T("/proc/")<<m_pid<<_T("/exe");
	TrStr(pathOutStr.str());
    CPath pathTest(CPath::GetFullPath(pathOutStr.str()));
    if (pathTest.IsFile())
    {
        char buf[s_maxPath] = {0};
        TrStr(pathOutStr.str());
        if (readlink(StrToStrA(pathOutStr.str()).c_str(), buf, s_maxPath) > 0) {
            return true;
        }
    }
    return false;
}
//
//bool isProcessRunning(pid_t pid) {
//	// Der Aufruf ist erfolgreich (0 zurückgegeben), wenn der Prozess existiert.
//	if (kill(pid, 0) == -1) {
//		// errno wird auf ESRCH gesetzt, wenn der Prozess nicht existiert.
//		return false;
//	}
//	return true;
//}

SWinInfo CPrc::GetProcessWinInfo()
{
    //TrFu;
    SWinInfo inf;
    TrRet(m_pid < 1, inf);
    Display *display = NULL;
    OutStr pathOutStr;
    pathOutStr<<"/proc/"<<m_pid<<"/environ";
    CPath pathTest(pathOutStr.str().c_str());
    //TrStr(pathOutStr.str());
    // x11 search
    Atom netAtomClList = 0, netAtomPid = 0, actualType = 0;
    Window rootWnd = 0;
    unsigned long numItems = 0, bytesAfter = 0, currItemCount = 0;
    int format = 0, pid = 0, defScreenNum = 0;
    unsigned char *data = NULL, *currData = NULL;
    unsigned long *wndArray = NULL;
    BOOL status = FALSE;
    //TrFVar(CWndBase::m_dispList.size());
    LOOP_DISP(iter) {
        display = *iter;
        netAtomClList = XInternAtom(display, "_NET_CLIENT_LIST" , true);
        defScreenNum = DefaultScreen(display);
        rootWnd = RootWindow(display, defScreenNum);
        status = XGetWindowProperty(display, rootWnd, netAtomClList, 0L, (~0L), false, AnyPropertyType,
                                    &actualType, &format, &numItems, &bytesAfter, &data);
        //TrVar(numItems);
        //TrVar(format);
        if (status >= Success  && numItems) {
            if (format != 32) {
                Tr("format != 32, we are continue !");
                continue;
            }
            wndArray = (unsigned long*) data;
            netAtomPid = XInternAtom(display, "_NET_WM_PID", true);
            for (unsigned i = 0; i < numItems; ++i) {
                Window currWnd = (Window)wndArray[i];
                if (!currWnd) {
                    Tr("!currWnd");
                    continue;
                }
                currData = GetWindowAtomProperty(display, currWnd, netAtomPid, &currItemCount, NULL, NULL);
                if (currItemCount) {
                    pid = (int) *((unsigned long *)currData);
                    //TrVar(pid);
                    free(currData);
                    if (pid == m_pid) {
                        inf = SWinInfo(display, defScreenNum, rootWnd, currWnd);
                        goto GetProcessWinInfo_X11_Success;
                    }
                }
            }
            free(data);
        }
    }
    //Tr("proc search");
    if (pathTest.IsFile())
    {
        int fHandle = SFile::Open(pathOutStr.str());
        if (fHandle != MP_BAD_FILE) {
            char buff[MP_MAX_PATH] = {0};
            int count = SFile::Read(fHandle, buff, MP_MAX_PATH - 1), winId = 0;
            while (count > 0) {
                for (int i = 0; i < count; i += strlen(&buff[i]) + 1) {
                    if (!winId && !memcmp(&buff[i], "WINDOWID=", 9)) {
                        winId = atoi(&buff[i + 9]);
                    }
                    else if (!display && !memcmp(&buff[i], "DISPLAY=", 8)) {
                        display = XOpenDisplay(&buff[i + 8]);
                    }
                    if (winId && display) {
                        char *winname = NULL;
                        inf.m_disp = display;
                        inf.m_scrNum = DefaultScreen(display);
                        inf.m_hRoot = RootWindow(display, inf.m_scrNum);
                        inf.m_hWnd = winId;

                        FetchTitle(inf.m_disp, inf.m_hWnd,&winname);
                        //TrVar(winname);
                        free(winname);

                        SFile::Close(fHandle);
                        return inf;
                    }
                }
                count = SFile::Read(fHandle, buff, MP_MAX_PATH - 1);
            }
            SFile::Close(fHandle);
        }
        else {
            Tr("Can't open: "<<pathOutStr.str());
        }
    }
    return inf;
    GetProcessWinInfo_X11_Success:
        free(data);
        return inf;
}

WinInfoList CPrc::GetProcessAllWinInfo()
{
	// Tr("NO IMPL");
    WinInfoList allWnd;
	return allWnd;
}

bool CPrc::Terminate(unsigned exitCode)
{
    return !kill(m_pid, 9);
}

void CPrc::SleepMs(unsigned milliSec)
{
    struct timeval tv = {milliSec / 1000, milliSec % 1000 * 1000}; 
    select(0, NULL, NULL, NULL, &tv);
}
