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

// error.hxx

#if defined(MSWIN_OS)
#	include "win.hxx"
#elif defined(LINUX_OS)
#	include "lin.hxx"
#endif

#include "str.hxx"
#include "tools.hxx"
#include <map>

#ifndef _MPERROR_HXX_
#define _MPERROR_HXX_

#define MP_OK			0

namespace frameMP 
{

	typedef unsigned int ErrorCode;
	
	enum EStdErrorCode
	{
		MPER_UNE = MP_OK + 1,
		MPER_FNF,
		MPER_OOM,
		MPER_BPA,
		
		MPER_MAX_ID
	};
	
	class CError : public TSingl<CError>
	{
		friend struct TSingl<CError>;

		CError();
		~CError();

		public:
			
			struct ErrorEntry
			{
				ErrorCode m_code;
				Str		  m_str;
			};

			static const ErrorEntry s_stdErrors[];

			void AddError(ErrorEntry err);
			const TCHAR* LookUp(ErrorCode code);

		private:
			typedef map<ErrorCode, ErrorEntry> ErrorMap;
			ErrorMap m_errMap;
	};

}; // namespace frameMP

#endif // _MPERROR_HXX_
