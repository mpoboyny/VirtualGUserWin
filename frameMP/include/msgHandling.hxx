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

// msgHandling.hxx

#ifndef _MSGHANDLING_HXX_
#define _MSGHANDLING_HXX_

#include "mp.hxx"
#include <map>
#include <list>


namespace frameMP
{

class IMsgHander;

class CMsgSource
{
		typedef map <unsigned, list <IMsgHander*> > MsgHandlerMap;
		MsgHandlerMap m_msgHdlMap;
	public:
		CMsgSource();
		virtual ~CMsgSource();
		bool AddMsgHandler(unsigned msg, unsigned priority, IMsgHander *hdl);
		bool DelMsgHandler(unsigned msg, IMsgHander *hdl);
		void DelAllMsgHandler(IMsgHander *hdl);
		bool PumpMsg(unsigned uMsg, WPARAM wParam, LPARAM lParam);
};

class IMsgHander
{
	friend class CMsgSource;
	unsigned m_priority;
	public:
		IMsgHander() : m_priority(0) {}
		virtual ~IMsgHander(){}
		virtual bool HandleMsg(unsigned uMsg, WPARAM wParam, LPARAM lParam) = 0;
};

}; // frameMP

#endif //_MSGHANDLING_HXX_
