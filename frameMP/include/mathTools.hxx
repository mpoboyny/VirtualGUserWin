// 
// This file is part of FrameMP project.
// Copyright ©2016 Mykhaylo Poboynyy <mpoboyny@googlemail.com>

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

#ifndef _MATHTOOLS_HXX_
#define _MATHTOOLS_HXX_

namespace frameMP
{

static double const s_e        = 2.71828182845904523536;
static double const s_log2e    = 1.44269504088896340736;
static double const s_log10e   = 0.434294481903251827651;
static double const s_ln2      = 0.693147180559945309417;
static double const s_ln10     = 2.30258509299404568402;
static double const s_pi       = 3.14159265358979323846;
static double const s_pi_2     = 1.57079632679489661923;
static double const s_pi_4     = 0.785398163397448309616;
static double const s_1_pi     = 0.318309886183790671538;
static double const s_2_pi     = 0.636619772367581343076;
static double const s_2_sqrtpi = 1.12837916709551257390;
static double const s_sqrt2    = 1.41421356237309504880;
static double const s_sqrt1_2  = 0.707106781186547524401;

template<typename T> T Deg2Rad(T deg) {return s_pi*deg/180.;}
template<typename T> T Rad2Deg(T rad) {return 180.*rad/s_pi;}

class CMtx
{
    size_t m_Rows;
    size_t m_Cols;
    size_t m_dataSize;
    double *m_data;
    
    inline void initFrom(const CMtx &rhs);
public:
    explicit CMtx(size_t rows = 1, size_t cols = 1, const double *data = 0);
    CMtx(const CMtx &rhs);
    virtual ~CMtx();
    CMtx& operator=(const CMtx &rhs);
    CMtx operator*(const CMtx &rhs) const;
    CMtx operator*(const double &rhs) const;
    CMtx operator+(const CMtx &rhs) const;
    CMtx operator-(const CMtx &rhs) const;
    double* operator[](size_t ind);
    bool Resize(size_t rows, size_t cols);
    CMtx& Union(const CMtx &rhs);
    inline size_t NumRows() const {return m_Rows;}
    inline size_t NumCols() const {return m_Cols;}
    bool WriteTo(int desc);
    bool ReadFrom(int desc);
};

}; // namespace frameMP

#endif // _MATHTOOLS_HXX_