// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "ofstream.h"
#include "ustring.h"
#include "uexception.h"
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <stdarg.h>

namespace ustl {

//----------------------------------------------------------------------

ifstream cin  (STDIN_FILENO);
ofstream cout (STDOUT_FILENO);
ofstream cerr (STDERR_FILENO);

//----------------------------------------------------------------------

/// Default constructor.
ofstream::ofstream (void)
: ostringstream()
,_file()
{
    reserve (255);
}

/// Constructs a stream for writing to \p ofd
ofstream::ofstream (int ofd)
: ostringstream()
,_file (ofd)
{
    clear (_file.rdstate());
    reserve (255);
}

/// Constructs a stream for writing to \p filename.
ofstream::ofstream (const char* filename, openmode mode)
: ostringstream()
,_file (filename, mode)
{
    clear (_file.rdstate());
}

/// Default destructor.
ofstream::~ofstream (void) noexcept
{
    try { flush(); } catch (...) {}
    if (_file.fd() <= STDERR_FILENO)	// Do not close cin,cout,cerr
	_file.detach();
}

/// Flushes the buffer and closes the file.
void ofstream::close (void)
{
    clear (_file.rdstate());
    flush();
    _file.close();
}

/// Flushes the buffer to the file.
ostream& ofstream::flush (void)
{
    clear();
    while (good() && pos() && overflow (remaining())) {}
    clear (_file.rdstate());
    return *this;
}

/// Seeks to \p p based on \p d.
ofstream& ofstream::seekp (off_t p, seekdir d)
{
    flush();
    _file.seekp (p, d);
    clear (_file.rdstate());
    return *this;
}

/// Called when more buffer space (\p n bytes) is needed.
ofstream::size_type ofstream::overflow (size_type n)
{
    if (eof() || (n > remaining() && n < capacity() - pos()))
	return ostringstream::overflow (n);
    size_type bw = _file.write (cdata(), pos());
    clear (_file.rdstate());
    erase (begin(), bw);
    if (remaining() < n)
	ostringstream::overflow (n);
    return remaining();
}

//----------------------------------------------------------------------

/// Constructs a stream to read from \p ifd.
ifstream::ifstream (int ifd)
: istringstream ()
,_buffer (255,'\0')
,_file (ifd)
{
    link (_buffer.data(), streamsize(0));
}

/// Constructs a stream to read from \p filename.
ifstream::ifstream (const char* filename, openmode mode)
: istringstream ()
,_buffer (255,'\0')
,_file (filename, mode)
{
    clear (_file.rdstate());
    link (_buffer.data(), streamsize(0));
}

/// Reads at least \p n more bytes and returns available bytes.
ifstream::size_type ifstream::underflow (size_type n)
{
    if (eof())
	return istringstream::underflow (n);

    const ssize_t freeSpace = _buffer.size() - pos();
    const ssize_t neededFreeSpace = max (n, _buffer.size() / 2);
    const size_t oughtToErase = Align (max (0, neededFreeSpace - freeSpace));
    const size_type nToErase = min (pos(), oughtToErase);
    _buffer.memlink::erase (_buffer.begin(), nToErase);
    const uoff_t oldPos (pos() - nToErase);

    size_type br = oldPos;
    if (_buffer.size() - br < n) {
	_buffer.resize (br + neededFreeSpace);
	link (_buffer.data(), streamsize(0));
    }
    cout.flush();

    size_type brn = 1;
    for (; br < oldPos + n && brn && _file.good(); br += brn)
	brn = _file.readsome (_buffer.begin() + br, _buffer.size() - br);
    clear (_file.rdstate());

    _buffer[br] = 0;
    link (_buffer.data(), br);
    seek (oldPos);
    return remaining();
}

/// Flushes the input.
int ifstream::sync (void)
{
    istringstream::sync();
    underflow (0U);
    clear (_file.rdstate());
    return -good();
}

/// Seeks to \p p based on \p d.
ifstream& ifstream::seekg (off_t p, seekdir d)
{
    _buffer.clear();
    link (_buffer);
    _file.seekg (p, d);
    clear (_file.rdstate());
    return *this;
}

//----------------------------------------------------------------------

} // namespace ustl
