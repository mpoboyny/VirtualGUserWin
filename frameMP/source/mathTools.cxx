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

// mathTools.cxx

#include <mp.hxx>
#include <mathTools.hxx>
#include <tools.hxx>
#include <file.hxx>

#include <assert.h>

namespace frameMP
{

// inline functions
void CMtx::initFrom(const CMtx &rhs)
{
    TrFu;
    assert(rhs.m_dataSize > 0);
    if (m_dataSize != rhs.m_dataSize) {
        m_data = (double*)calloc(rhs.m_Rows * rhs.m_Cols, sizeof(double));
    }
    m_Rows = rhs.m_Rows;
    m_Cols = rhs.m_Cols;
    m_dataSize = rhs.m_Rows * rhs.m_Cols *sizeof(double);
    memcpy(m_data, rhs.m_data, m_dataSize);
}

//////////////////////////////////////////////////////////
CMtx::CMtx(size_t rows, size_t cols, const double *data)
    : m_Rows(rows), m_Cols(cols), m_dataSize(0), m_data(0)
{
    assert(rows > 0 && cols > 0);
    m_dataSize =  m_Rows * m_Cols * sizeof(double);
    m_data = (double*)calloc(m_Rows * m_Cols, sizeof(double));
    assert(m_data);
    if (data) {
        memcpy(m_data, data, m_dataSize);
    }
}

CMtx::CMtx(const CMtx &rhs)
    : m_Rows(0), m_Cols(0), m_dataSize(0), m_data(0)
{
    initFrom(rhs);
}

CMtx::~CMtx()
{
    free(m_data);
}

CMtx& CMtx::operator=(const CMtx &rhs)
{
    initFrom(rhs);
    return *this;
}

CMtx CMtx::operator*(const CMtx &rhs) const
{
    assert(m_Cols == rhs.m_Rows);
    CMtx res(m_Rows, rhs.m_Cols);
    double *resRow;
    const double *myRow;
    size_t i, j, k;
    for (i = 0; i < res.m_Rows; ++i) {
        resRow = &res.m_data[i * res.m_Cols];
        myRow = &m_data[i * m_Cols];
        for (j = 0; j < res.m_Cols; ++j) {
            for (k = 0; k < rhs.m_Rows; ++k) {
                resRow[j] += myRow[k] * rhs.m_data[k * rhs.m_Cols + j];
            }
        }
    }
    return res;
}

CMtx CMtx::operator*(const double &rhs) const
{
    TrFu;
    CMtx res(m_Rows, m_Cols, m_data);
    TrP;
    double *resRow;
    const double *myRow;
    size_t i, j;
    for (i = 0; i < m_Rows; ++i) {
        resRow = &res.m_data[i * m_Cols];
        myRow = &m_data[i * m_Cols];
        for (j = 0; j < m_Cols; ++j) {
            resRow[j] = myRow[j] * rhs;
        }
    }
    return res;
}

CMtx CMtx::operator+(const CMtx &rhs) const
{
    assert(m_Cols == rhs.m_Cols && m_Rows == rhs.m_Rows);
    CMtx res(m_Rows, m_Cols, m_data);
    double *resRow;
    const double *myRow;
    const double *rhsRow;
    size_t i, j;
    for (i = 0; i < m_Rows; ++i) {
        resRow = &res.m_data[i * m_Cols];
        myRow = &m_data[i * m_Cols];
        rhsRow = &rhs.m_data[i * m_Cols];
        for (j = 0; j < m_Cols; ++j) {
            resRow[j] = myRow[j] + rhsRow[j];
        }
    }
    return res;
}

CMtx CMtx::operator-(const CMtx &rhs) const
{
    assert(m_Cols == rhs.m_Cols && m_Rows == rhs.m_Rows);
    CMtx res(m_Rows, m_Cols, m_data);
    double *resRow;
    const double *myRow;
    const double *rhsRow;
    size_t i, j;
    for (i = 0; i < m_Rows; ++i) {
        resRow = &res.m_data[i * m_Cols];
        myRow = &m_data[i * m_Cols];
        rhsRow = &rhs.m_data[i * m_Cols];
        for (j = 0; j < m_Cols; ++j) {
            resRow[j] = myRow[j] - rhsRow[j];
        }
    }
    return res;
}

double* CMtx::operator[](size_t ind)
{
    assert(ind < m_Rows);
    return &m_data[ind * m_Cols];
}

bool CMtx::Resize(size_t rows, size_t cols)
{
    TrFu;
    TrRet(rows < 1 || cols < 1, false);
    size_t newSize = rows * cols * sizeof(double);
    double *data = NULL;
    TrRet(!frameMP::ReAlloc(&data, 0, newSize), false);
    size_t minRows = Min(m_Rows, rows),
           minCols = Min(m_Cols, cols);
    double *newRow;
    const double *myRow;
    for (size_t i = 0; i < minRows; ++i) {
        myRow = &m_data[i * m_Cols];
        newRow = &data[i * cols];
        memcpy(newRow, myRow, sizeof(double) * minCols);
    }
    free(m_data);
    m_data = data;
    m_Rows = rows;
    m_Cols = cols;
    m_dataSize = newSize;
    return true;
}

CMtx& CMtx::Union(const CMtx &rhs)
{
    TrFu;
    size_t newRows = m_Rows + rhs.m_Rows,
           newCols = Max(m_Cols, rhs.m_Cols);
    size_t newSize = newRows * newCols * sizeof(double);
    if (frameMP::ReAlloc(&m_data, m_dataSize, newSize)) {
        double *myRow;
        const double *rhsRow;
        for (size_t i = m_Rows, k = 0; i < newRows; ++i) {
            myRow = &m_data[i * m_Cols];
            rhsRow = &rhs.m_data[k++ * rhs.m_Cols];
            memcpy(myRow, rhsRow, sizeof(double) * rhs.m_Cols);
        }
        m_Rows = newRows;
        m_Cols = newCols;
        m_dataSize = newSize;
    }
    return *this;
}

bool CMtx::WriteTo(int desc)
{
    int sizeData[3] = {int(m_Rows), int(m_Cols), int(m_dataSize)};
    size_t s1 = sizeof(int) * 3, writenBytes = 0;
    writenBytes = SFile::Write(desc, &sizeData[0], s1);
    writenBytes += SFile::Write(desc, &m_data[0], m_dataSize);
    return writenBytes == s1+m_dataSize;
}

bool CMtx::ReadFrom(int desc)
{
    TrFu;
    int sizeData[3] = {0};
    size_t s1 = sizeof(int) * 3, readedBytes = 0;
    readedBytes = SFile::Read(desc, &sizeData[0], s1);
    if (sizeData[2] < 1
                || readedBytes != s1
                || sizeData[2] != sizeof(double) * sizeData[0] * sizeData[1]) {
        return false;
    }
    if (frameMP::ReAlloc(&m_data, m_dataSize, sizeData[2])) {
        readedBytes += SFile::Read(desc, m_data, sizeData[2]);
        m_Rows = sizeData[0];
        m_Cols = sizeData[1];;
        m_dataSize = sizeData[2]; 
    }
    return readedBytes == s1+m_dataSize;
}

}; // namespace frameMP
