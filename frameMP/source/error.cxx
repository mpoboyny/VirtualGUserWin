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

// error.cxx

#include <mp.hxx>

#define DEF_MPERROR_STR	_T("Unknown error.")

namespace frameMP {
	
	const CError::ErrorEntry CError::s_stdErrors[]  = {
		{MPER_UNE, DEF_MPERROR_STR},
		{MPER_FNF, _T("File not found")},
		{MPER_OOM, _T("Out off memory")},
		{MPER_BPA, _T("Bad parameter value")}
	};
	
	CError::CError()
	{
		for (unsigned i = 0; i < MPER_MAX_ID - 1; ++i) {
			m_errMap[s_stdErrors[i].m_code] = s_stdErrors[i];
		}
	}

	CError::~CError()
	{
		m_errMap.clear();
	}

	void CError::AddError(ErrorEntry err)
	{
		if (err.m_code) {
			m_errMap[err.m_code] = err;
		}
		else {
			TrFu;
			Tr("err.m_code == 0, 0 is reserved for MP_OK ! CError not added !");
		}
	}

	const TCHAR* CError::LookUp(ErrorCode code)
	{
		ErrorMap::iterator iter = m_errMap.find(code);
		return iter==m_errMap.end()? DEF_MPERROR_STR: iter->second.m_str.c_str();
	}

}; // namespace frameMP
