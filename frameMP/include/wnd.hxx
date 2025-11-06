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

// wnd.hxx

#include <mp.hxx>
#include <msgHandling.hxx>

#ifndef _WND_HXX_
#define _WND_HXX_

namespace frameMP
{
struct SWinInfo
{
    Display *m_disp;
    int m_scrNum;
    bool m_isDeskTop;
    HWND m_hRoot, m_hWnd, m_hPar;
    SWinInfo(Display *disp = NULL, int scrNum = -1, HWND hRoot = NULL, HWND hWnd = NULL, 
                HWND hPar = NULL) 
        : m_disp(disp), m_scrNum(scrNum), m_isDeskTop(false), 
          m_hRoot(hRoot), m_hWnd(hWnd), m_hPar(hPar)
    {}
#ifdef _MPTRACE_
    void Trace()
    {
        TrFu;
        OutStr tr;
        tr<<endl;
        tr<<"m_disp     : "<<m_disp<<endl;
        tr<<"m_hPar     : "<<m_hPar<<endl;
        tr<<"m_hWnd     : "<<m_hWnd<<endl;
        tr<<"m_hRoot    : "<<m_hRoot<<endl;
        tr<<"m_scrNum   : "<<m_scrNum<<endl;
        tr<<"m_isDeskTop: "<<m_isDeskTop;
        Tr(tr.str());
    }
#endif
};
typedef list<SWinInfo> WinInfoList;

struct SPoint
{
    int m_x, m_y;
    SPoint(int x = 0, int y = 0)
        : m_x(x), m_y(y)
    {}
    inline void Reset() {m_x = m_y = 0;}
    inline SPoint operator+(const SPoint &rhs) const
    {
        return SPoint(m_x + rhs.m_x, m_y + rhs.m_y);
    }
    inline SPoint operator-(const SPoint &rhs) const
    {
        return SPoint(m_x - rhs.m_x, m_y - rhs.m_y);
    }
};

struct SRect
{
    int m_posX, m_posY, m_width, m_height;
    SRect(int posX = 0, int posY = 0, int width = 0, int height = 0)
        : m_posX(posX), m_posY(posY), m_width(width), m_height(height)
    {}
    inline BOOL IsPointIn(const SPoint &pt)
    {
        return pt.m_x > m_posX && pt.m_y > m_posY && pt.m_x < (m_posX + m_width) && pt.m_y < (m_posY + m_height);
    }
	inline BOOL IsValid() const
	{
		return m_posX > -1 && m_posY > -1 && m_width > 0 && m_height > 0;
	}
	inline BOOL IsEmpty() const
	{
		return 0 == m_width == m_height;
	}
	SRect Cut(const SRect rc, int defPix = 0)
	{
		return SRect(m_posX + defPix + rc.m_posX, m_posY + defPix + rc.m_posY, rc.m_width - defPix, rc.m_height - defPix);
	}
#ifdef _MPTRACE_
    void Trace() const
    {
        OutStr tr;
        tr<<"SRect: "<<endl;
        tr<<"m_posX  : "<<m_posX<<endl;
        tr<<"m_posY  : "<<m_posY<<endl;
        tr<<"m_width  : "<<m_width<<endl;
        tr<<"m_height : "<<m_height<<endl;
        Tr(tr.str());
    }
#endif
};

class CPrc;
class CWndBase : public CMsgSource, public TInstCounter<CWndBase>
{
    friend class TInstCounter<CWndBase>;
    friend class CPrc;
    static unsigned s_InstCount;

    enum EFindWindowWhat
    {
        eWithTitleExakt,
        eWithTitleWithText,
        eAllTop
    };
    
    static CWndBase s_staticInit;
    protected:
        // variables
        typedef list<Display*> DispList;
        typedef map<Str, WORD> XKeyMap;

        HWND	m_hWnd,
                m_hWndPar;
        static DispList m_dispList;
        Str		m_defaultText;
        SWinInfo m_winInfo;
        
        static XKeyMap s_KeyNameToKey;
        XKeyMap::const_iterator xKeyMapIt;

		int m_iCompareCutPx;

        //functions
        BOOL OnCreate();
        inline static WinInfoList FindWindowByTitleText(const TCHAR *title, EFindWindowWhat eWhat);

        static LRESULT WINAPI WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

        static inline void InitKeyNameToKey();
        inline BOOL KeyCodeDo(LPCTSTR code, unsigned flag); 

		inline SRect CutWndRect(const SRect& rc, int defPix = 0);
        inline BOOL GetScreenShot(void *&pScreenData, size_t &screenDataSize, const SRect &wndRect);

    public:
        static BOOL Init();
        static void Clean();

        CWndBase();
        explicit CWndBase(const SWinInfo &info);
        virtual ~CWndBase();
        inline operator HWND() {return m_hWnd;}
        HWND GetHwnd()const;
        HWND GetParHwnd();
        inline SWinInfo GetSWinInfo()const {return m_winInfo;}

        unsigned GetText(Str& back);
        BOOL GetTitle(Str& back);
        BOOL SetText(LPCTSTR txt);
        BOOL TypeText(LPCTSTR txt);

        BOOL KeyCodeType(LPCTSTR key);
        BOOL KeyCodeDown(LPCTSTR key);
        BOOL KeyCodeUp(LPCTSTR key);

        BOOL Activate();
		BOOL ActivateEx();
		void Minimize();
		void Maximize();
        void UpdateWindow();
        
        int GetWndWidth();
        int GetWndHeight();
        SRect GetWndRect();
        BOOL SetWndRect(const SRect &rc);

        PrcId GetWindowPrcId();
        
        virtual BOOL Create(LPCTSTR wndDefaultText, HWND hPar=NULL, LPCTSTR className=NULL){return FALSE;}
        virtual BOOL OnWndMsg(UINT &uMsg, WPARAM &wParam, LPARAM &lParam);

        virtual void ParentSizeChanged(unsigned state, int cx, int cy) {}
        WinInfoList GetAllChildByClassName(const TCHAR *title);
        WinInfoList GetAllChild();
        WinInfoList GetAllParent();
		WinInfoList GetAllZOrder();

        void ClickL(unsigned delay = 12, const SPoint &pt = SPoint(1, 1));
        void ClickExL(const SPoint &pt = SPoint(1, 1));
        void ClickDL(const SPoint &pt = SPoint(1, 1));
		void ClickExDL(const SPoint &pt = SPoint(1, 1));
        void ClickR(unsigned delay = 12, const SPoint &pt = SPoint(1, 1));
        void ClickExR(const SPoint &pt = SPoint(1, 1));
		void ClickExDR(const SPoint& pt = SPoint(1, 1));

        void MoveMouseTo(const SPoint &pt);
        void MoveLMouse(const SPoint &pt);
        void MoveRMouse(const SPoint &pt);

        Str GetClassName();
        WinInfoList GetAllChildFromPoint(const SPoint &pt);

        BOOL SaveScreenShot(LPCTSTR filePath);
		BOOL SaveScreenShotEx(LPCTSTR filePath, const SRect& rc);
		int SetCompareCutPx(int val);
        double CmpScreenShot(LPCTSTR filePath, BOOL soft);
		double CmpScreenShotEx(LPCTSTR filePath, const SRect& rc, BOOL soft);

        static BOOL IsWindow(HWND hWnd);
        static BOOL IsTopWindow(const SWinInfo &inf);

        static SWinInfo GetActivWindow();
        static WinInfoList GetAllTopWindow();
        static WinInfoList FindWindowByTitle(const TCHAR *title);
        static WinInfoList FindWindowByTextInTitle(const TCHAR *title);	
        static SWinInfo GetWindowFromPoint(const SPoint &pt);
        static int GetMonitorCount();
        static SWinInfo GetDesktopWindow();
        static BOOL ShowDesktopWindow();
		static int GetTextFromCL(StrA& back);
};

#define LOOP_DISP(iterName) for (CWndBase::DispList::const_iterator iterName = CWndBase::m_dispList.begin(), iterEnd = CWndBase::m_dispList.end(); iterName != iterEnd; ++iterName)

}; // namespace FrameMP

#endif //_WND_HXX_
