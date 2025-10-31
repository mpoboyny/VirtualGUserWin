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

// file.inl

int SFile::Open(const Str &fileToOpen, int oflag, int pmode)
{
#ifdef _WIN32
    return _wopen(StrToStrW(fileToOpen).c_str(), oflag, pmode);
#elif defined(LINUX_OS)
    return open(StrToStrA(fileToOpen).c_str(), oflag, pmode);
#endif
}

size_t SFile::Read(int fd, void *buffer, size_t count)
{
#ifdef _WIN32
    return _read(fd, buffer, count);
#elif defined(LINUX_OS)
    return read(fd, buffer, count);
#endif
}

int SFile::Write(int fd, void *buffer, size_t count)
{
#ifdef _WIN32
    return _write(fd, buffer, count);
#elif defined(LINUX_OS)
    return write(fd, buffer, count);
#endif
}

bool SFile::Close(int fd)
{
#ifdef _WIN32
    return !!_close(fd);
#elif defined(LINUX_OS)
    return close(fd);
#endif
}

size_t SFile::GetFileSize(int fd)
{
#ifdef _WIN32
    size_t iSize = _lseek(fd, 0, SEEK_END);
    _lseek(fd, 0, SEEK_SET);
    return iSize;
#elif defined(LINUX_OS)
    size_t iSize = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    return iSize;
#endif
}