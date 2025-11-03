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

// prc.hxx

#include <mp.hxx>
#include <wnd.hxx>

#ifndef _MPPRC_HXX_
#define _MPPRC_HXX_

namespace frameMP 
{
class CPrc
{
    static const unsigned s_defIdleWaitSec = 1;
    PrcToken m_token;
    PrcId	 m_pid;
    inline bool CleanUp();
    public:
        CPrc();
        CPrc(PrcToken token);
        CPrc(PrcId pid);
        virtual ~CPrc();

        PrcId GetPid() { return m_pid; }
        PrcToken GetToken() { return m_token; }

		bool ExecuteFile(const Str &cmdLine);
		bool Create(const Str &cmdLine, const Str &param, const Str &file, bool waitForIdle = true);
        bool Create(const Str &cmdLine, const Str param[], unsigned numParam, BOOL inheritParentDescs);
        bool Close();
        bool IsRunning();
        SWinInfo GetProcessWinInfo();
        WinInfoList GetProcessAllWinInfo();
        bool Terminate(unsigned exitCode = 0);
        static void SleepMs(unsigned milliSec);
};

}; // namespace frameMP

#endif // _MPPRC_HXX_
