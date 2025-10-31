/// 
// This file is part of FrameMP project.
// Copyright �2017 Mykhaylo Poboynyy <mpoboyny@googlemail.com>

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

// trace.hxx
#ifndef _MPTRACE_HXX_
#define _MPTRACE_HXX_

#include "str.hxx"
#include <time.h>

#	if defined(_MPTRACE_) || defined(_DEBUG) || defined(DEBUG)
#       ifndef _MPTRACE_
#		    if defined(_DEBUG) || defined(DEBUG)
#			    define _MPTRACE_
#		    endif
#       endif
#       ifdef _UNICODE
#			define tcout wcout
#		else
#			define tcout cout
#       endif
		namespace frameMP {
#           define __TRFILE__ (strrchr(__FILE__, '\\')?strrchr(__FILE__, '\\'):__FILE__)
#			ifdef _WIN32 
#				include <windows.h>
#				ifdef _MSC_VER
#					define Tr(out) {OutStr log; log<<"["<<__TRFILE__<<":"<<__LINE__<<"] "<<out<<"\n"; OutputDebugString(log.str().c_str());}
#				else // no _MSC_VER
#					define Tr(out) {OutStr log; log<<"["<<__TRFILE__<<":"<<__LINE__<<"] "<<out; OutputDebugString(log.str().c_str());}
#				endif
#			else
#				define Tr(out) {OutStr log; log<<"["<<__TRFILE__<<":"<<__LINE__<<"] "<<out<<"\n"; tcout<<log.str().c_str();}
#			endif
#			define TrVar(var) {if ((var)) Tr(#var<<": "<<var) else Tr(#var<<": "<<0);}
#			define TrStr(var) {Tr(#var<<": "<<var);}
#			define TrFVar(var) {Tr(#var<<": "<<var);}
#			define _TRACE_TAB_PREF 1
			class TrScope 
			{
				StrA m_fileName, m_fuName, m_tabs;
				int m_line, m_tabCount;
#				ifdef _WIN32
					SYSTEMTIME m_locTime;
					LARGE_INTEGER m_tick, m_freq;
#				else
					struct timespec m_timeReq;
					struct tm *m_tmInfo;
#				endif
				static int SetCount(int val=0)
				{
					static int s_count =_TRACE_TAB_PREF;
					return s_count+=val;
				}
				inline void AddTime(OutStr &timeLog)
				{
#					ifdef _WIN32
						GetLocalTime(&m_locTime);
						QueryPerformanceCounter(&m_tick);
						timeLog<<" "<<m_locTime.wHour<<":"<<m_locTime.wMinute<<":"<<m_locTime.wSecond+(m_tick.QuadPart%m_freq.QuadPart)/double(m_freq.QuadPart)<<" ";
#					else
						clock_gettime(CLOCK_REALTIME, &m_timeReq);
						m_tmInfo = localtime((const time_t*)&m_timeReq.tv_sec);
						timeLog<<" "<<m_tmInfo->tm_hour<<":"<<m_tmInfo->tm_min<<":"<<m_tmInfo->tm_sec+m_timeReq.tv_nsec/1e9<<" ";
#					endif
				}
				public: 
					TrScope(const char *file, const char *fu, int line) : m_fileName(file), m_fuName(fu), m_tabs(""), m_line(line), m_tabCount(SetCount(1))
					{
						for (int i=0; i<m_tabCount; i++) { 
							m_tabs+="\t ";
						}
						OutStr log;
						log<<"["<<m_fileName.c_str()<<":"<<m_line<<"] "<<m_tabs.c_str();
#						ifdef _WIN32
							QueryPerformanceFrequency(&m_freq);
#						endif
						AddTime(log);
						log<<m_fuName.c_str()<<" ->";
#						ifdef _WIN32
							OutputDebugString(log.str().c_str());
#							ifdef _MSC_VER
								OutputDebugString(_T("\n"));
#							endif
#						else
							tcout<<log.str().c_str()<<endl;
#						endif
					} 
					~TrScope()
					{
						OutStr log;
						log<<"["<<m_fileName.c_str()<<":"<<m_line<<"] "<<m_tabs.c_str();
						AddTime(log);
						log<<m_fuName.c_str()<<" <-"; 
#						ifdef _WIN32 
							OutputDebugString(log.str().c_str());
#							ifdef _MSC_VER
								OutputDebugString(_T("\n"));
#							endif
#						else
							tcout<<log.str().c_str()<<endl;
#						endif
						if (m_tabCount > _TRACE_TAB_PREF) {
							SetCount(-1);
						}
					}
			};
#			define TrFu TrScope logScope(__TRFILE__, __FUNCTION__, __LINE__);
		}; // namespace frameMP
#	else
#		define Tr(out)
#		define TrVar(var)
#		define TrStr(var)
#		define TrFVar(var) var;
#		define TrFu
#	endif // _MPTRACE_
#	define TrRet(cond, val)  	if ((cond)){\
								Tr(__FUNCTION__<<" failed "<<#cond<<" return "<<#val);\
								return (val);\
							}
#	define TrVoid(cond)  	if ((cond)){\
							Tr(__FUNCTION__<<" failed "<<#cond);\
							return;\
                        }
#	define TrErr(val)  	{ErrorCode errCode=(val); if (errCode){\
						Tr(#val<<": "<<CError::Inst().LookUp(errCode));} \
						else { Tr(#val<<": MP_OK");}}
#	define TrP       	Tr("Trace point");

#endif // _MPTRACE_HXX_
