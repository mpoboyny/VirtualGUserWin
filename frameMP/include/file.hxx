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

// file.hxx

#ifndef _MPFILE_HXX_
#define _MPFILE_HXX_

#define MP_PATH_SEP _T('\\')
#define MP_MSPATH_SEP _T('\\')
#define MP_NXPATH_SEP _T('/')

#define MP_MAX_PATH 1024

#define MP_BAD_FILE -1
#define MP_S_READ _S_IREAD
#define MP_S_WRITE _S_IWRITE

#define MP_O_BINARY _O_BINARY
#define MP_O_RDONLY _O_RDONLY
#define MP_O_RDWR _O_RDWR
#define MP_O_CREATE _O_CREAT
# define MP_O_TRUNC _O_TRUNC

namespace frameMP 
{
    class CPath : public std::list<const TCHAR*>
    {
            const TCHAR *m_orgPath;
            TCHAR *m_pathListContainer;
            TCHAR m_sep;
            bool m_isRelPath;
            inline void Init();
            inline void InitPath(const TCHAR *path, TCHAR sep);
        public:
            CPath(const TCHAR *path = NULL, TCHAR sep = MP_PATH_SEP);
            CPath(const Str &path, TCHAR sep = MP_PATH_SEP);
            CPath(const CPath& rhs);
            ~CPath();
            static Str NormalizePathSeps(const TCHAR *osPath);

            void operator=(const CPath& rhs);
            void operator=(const Str& rhs);
            void operator+=(const TCHAR *addPathPart);
            operator const TCHAR*() {return m_orgPath;}
            operator bool() {return !!m_orgPath;}
            inline TCHAR GetSep() {return m_sep;}
            bool IsFile();
            bool IsDir();
            bool GetLastDirPath(Str &dirPath);
            inline bool IsRelPath() {return m_isRelPath;}
            Str GetFullPath() const;
            static Str GetFullPath(const Str &path);
			static Str GetTmpFilePath();
			static void RemoveAllTmpFiles();
    };

    struct SFile
    {
        static inline int Open(const Str &fileToOpen, int oflag = MP_O_RDONLY | MP_O_BINARY, int pmode = MP_S_READ);
        static inline size_t Read(int fd, void *buffer, size_t count);
        static inline int Write(int fd, void *buffer, size_t count);
        static inline size_t GetFileSize(int fd);
        static inline bool Close(int fd);
		static size_t ReadFileToStr(const Str& filePath, StrA& str);
    };
#
}; // namespace frameMP

#endif // _MPFILE_HXX_
