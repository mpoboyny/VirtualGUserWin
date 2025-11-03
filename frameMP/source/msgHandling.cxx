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

// msgHandling.cxx


#include <frameMP/msgHandling.hxx>

namespace frameMP
{

CMsgSource::CMsgSource()
{
}

CMsgSource::~CMsgSource()
{
}

bool CMsgSource::AddMsgHandler(unsigned msg, unsigned priority, IMsgHander *hdl)
{
	MsgHandlerMap::iterator found = m_msgHdlMap.find(msg);
	if (found == m_msgHdlMap.end()) {
		list <IMsgHander*> tmpList;
		tmpList.push_back(hdl);
		m_msgHdlMap[msg] = tmpList;
		return true;
	}
	list <IMsgHander*> ::iterator iter = found->second.begin();
	for (list <IMsgHander*>::const_iterator iterEnd = found->second.end(); iter != iterEnd && (*iter)->m_priority > priority; ++iter);
	TrRet(iter != found->second.end() && (*iter)->m_priority == priority, false);
	found->second.insert(iter, hdl);
	hdl->m_priority = priority;
	return true;
}

bool CMsgSource::DelMsgHandler(unsigned msg, IMsgHander *hdl)
{
	MsgHandlerMap::iterator found = m_msgHdlMap.find(msg);
	TrRet(found == m_msgHdlMap.end(), false);
	list <IMsgHander*> ::iterator iter = found->second.begin();
	for (list <IMsgHander*>::const_iterator iterEnd = found->second.end(); iter != iterEnd; ++iter) {
		if ((*iter) == hdl) {
			found->second.erase(iter);
			return true;
		}
	}
	return false;
}

void CMsgSource::DelAllMsgHandler(IMsgHander *hdl)
{
	MsgHandlerMap::iterator iterMsg = m_msgHdlMap.begin(),
							iterMsgEnd = m_msgHdlMap.end();
	for (; iterMsg != iterMsgEnd; ++iterMsg) {
		list <IMsgHander*> ::iterator iter = iterMsg->second.begin();
		for (list <IMsgHander*>::iterator iterEnd = iterMsg->second.end(); iter != iterEnd; ++iter) {
			if ((*iter) == hdl) {
				iterMsg->second.erase(iter);
				if (iterMsg->second.empty()) {
					break;
				}
				iter = iterMsg->second.begin();
				iterEnd = iterMsg->second.end();
			}
		}
	}
}

bool CMsgSource::PumpMsg(unsigned uMsg, WPARAM wParam, LPARAM lParam)
{
    TrRet(m_msgHdlMap.size() == 0, false);
	bool back = false;
	MsgHandlerMap::const_iterator iterMap = m_msgHdlMap.find(uMsg);
	if ( iterMap != m_msgHdlMap.end()) {
		for (list <IMsgHander*>::const_iterator iter = iterMap->second.begin(), iterEnd = iterMap->second.end(); iter != iterEnd; ++iter) {
			if ((*iter)->HandleMsg(uMsg, wParam, lParam)) {
				back = true;
				break;
			}
		}
	}
	return back;
}


}; // namespace frameMP
