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

// tools.hxx

#include <stdio.h>
#include <stack>

#ifndef _MPTOOLS_HXX_
#define _MPTOOLS_HXX_

#define DimOf(Array) (sizeof(Array) / sizeof(Array[0]))
#define Min(a, b) ((a < b) ? a : b)
#define Max(a, b) ((a > b) ? a : b)
#define Abs(a) ((a) < 0 ? -(a) : (a))

namespace frameMP
{

template <typename T> 
class TDel
{
    T *m_mem;
    TDel(const TDel& rhs);
    void operator=(const TDel& rhs);
    void *operator new(size_t sb);
    void operator delete(void *pvMem);
    public:
        TDel(T *mem):m_mem(mem){}
        ~TDel(){if (m_mem) {delete m_mem; m_mem = NULL;}}
        T* operator->() {return m_mem;}
        T* operator*() {return m_mem;}
};

typedef enum
{
    TSinglGet,
    TSinglDestroy
} ESingl;

template <class CLS> 
struct TSingl
{
    static CLS& Inst(ESingl what = TSinglGet)
    {
      static CLS *inst=0;
      if (what == TSinglDestroy) {
        delete inst;
        inst=0;
      } 
      else{
        if (!inst) {
            inst=new CLS();
        }
      }
      return *inst;
    }
};

template <class CLS> 
class TInstCounter
{
    enum ETodo
    {
        TInstCreate,
        TInstDestroy,
        TInstGet
    };
    unsigned Inst(ETodo what)
    {
        switch(what) {
            case TInstCreate:
                ++CLS::s_InstCount;
                break;
            case TInstDestroy:
                --CLS::s_InstCount;
                break;
        }
        return CLS::s_InstCount;
    }
    protected:
        TInstCounter()
        {
            Inst(TInstCreate);
        }
        TInstCounter(const TInstCounter& rhs)
        {
            Inst(TInstCreate);
        }
        virtual ~TInstCounter()
        {
            Inst(TInstDestroy);
        }
    public:
        unsigned GetInst()
        {
            return Inst(TInstGet);
        }
};

////
//class A : public TInstCounter<A>
//{
//	friend class TInstCounter<A>;
//	static unsigned s_InstCount;
//};
//unsigned A::s_InstCount = 0;

template <typename T> bool ReAlloc(T **ptr, size_t bytesPtr, size_t bytesNew)
{
    if (bytesNew < 1) {
        return false;
    }
    T *back = NULL;
    back = (T*)malloc(bytesNew);
    if (!back) {
        return false;
    }
    memset((void*)back, 0, bytesNew);
    if (*ptr) {
        memcpy((void*)back, (void*)*ptr, bytesPtr < bytesNew? bytesPtr: bytesNew);
        free((void*)*ptr);
    }
    *ptr = back;
    return true;
}

struct SMemCmpData
{
    char *m_pData1, *m_pData2;
    size_t m_dataSize;
};

template <typename T> bool MemCmp(T *&pData1, size_t dataSize1, T *&pData2, size_t dataSize2, double &percentEqual)
{
    percentEqual = 0.;
    size_t cmpSize = Min(dataSize1, dataSize2);
    size_t cmpSizeMax = Max(dataSize1, dataSize2);
    if (cmpSize < 1) {
        return false;
    }
    size_t equalBytes = 0;
    SMemCmpData block = {(char*)pData1, (char*)pData2, cmpSize};
    stack<SMemCmpData> cmpStack;
    cmpStack.push(block);
    while (!cmpStack.empty()) {
        SMemCmpData currBlock = cmpStack.top(); cmpStack.pop();
        if (!memcmp(currBlock.m_pData1, currBlock.m_pData2, currBlock.m_dataSize)) {
            equalBytes += currBlock.m_dataSize;
        }
        else if (currBlock.m_dataSize > 1){
            size_t newSize = currBlock.m_dataSize/2;
            SMemCmpData b1 = {currBlock.m_pData1, currBlock.m_pData2, newSize};
            cmpStack.push(b1);
            SMemCmpData b2 = {currBlock.m_pData1+newSize, currBlock.m_pData2+newSize, currBlock.m_dataSize%2? newSize+1: newSize};
            cmpStack.push(b2);
        }
    }
    percentEqual = ((double)equalBytes)/((double)cmpSize) * 100.;
    if (cmpSizeMax != cmpSize) {
        percentEqual -= ((double)(cmpSizeMax - cmpSize))/((double)cmpSizeMax) * 100.;
        if (percentEqual < 0.99999) percentEqual = 0.0;
    }
    return percentEqual > 99.99999;
}

}; // namespace frameMP

#endif //_MPTOOLS_HXX_
