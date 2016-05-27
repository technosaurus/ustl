// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2006 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "ustl.h"
#include <stdio.h>
#if HAVE_EXECINFO_H
    #include <execinfo.h>
#else
    static inline int backtrace (void**, int)			{ return 0; }
    static inline char** backtrace_symbols (void* const*, int)	{ return nullptr; }
#endif

namespace ustl {

/// Default constructor. The backtrace is obtained here.
CBacktrace::CBacktrace (void) noexcept
:_symbols (nullptr)
,_nFrames (0)
,_symbolsSize (0)
{
    _nFrames = backtrace (VectorBlock (_addresses));
    GetSymbols();
}

/// Copy constructor.
CBacktrace::CBacktrace (const CBacktrace& v) noexcept
:_symbols (nullptr)
,_nFrames (0)
,_symbolsSize (0)
{
    operator= (v);
}

/// Copy operator.
const CBacktrace& CBacktrace::operator= (const CBacktrace& v) noexcept
{
    memcpy (_addresses, v._addresses, sizeof(_addresses));
    _symbols = strdup (v._symbols);
    _nFrames = v._nFrames;
    _symbolsSize = v._symbolsSize;
    return *this;
}

/// Converts a string returned by backtrace_symbols into readable form.
static size_t ExtractAbiName (const char* isym, char* nmbuf) noexcept
{
    // Prepare the demangled name, if possible
    size_t nmSize = 0;
    if (isym) {
	// Copy out the name; the strings are: "file(function+0x42) [0xAddress]"
	const char* mnStart = strchr (isym, '(');
	if (++mnStart == (const char*)(1))
	    mnStart = isym;
	const char* mnEnd = strchr (isym, '+');
	const char* isymEnd = isym + strlen (isym);
	if (!mnEnd)
	    mnEnd = isymEnd;
	nmSize = min (size_t (distance (mnStart, mnEnd)), 255U);
	memcpy (nmbuf, mnStart, nmSize);
    }
    nmbuf[nmSize] = 0;
    // Demangle
    demangle_type_name (nmbuf, 255, &nmSize);
    return nmSize;
}

/// Tries to get symbol information for the addresses.
void CBacktrace::GetSymbols (void) noexcept
{
    char** symbols = backtrace_symbols (_addresses, _nFrames);
    if (!symbols)
	return;
    char nmbuf [256];
    size_t symSize = 1;
    for (uoff_t i = 0; i < _nFrames; ++ i)
	symSize += ExtractAbiName (symbols[i], nmbuf) + 1;
    if ((_symbols = (char*) calloc (symSize, 1))) {
	for (uoff_t i = 0; _symbolsSize < symSize - 1; ++ i) {
	    size_t sz = ExtractAbiName (symbols[i], nmbuf);
	    memcpy (_symbols + _symbolsSize, nmbuf, sz);
	    _symbolsSize += sz + 1;
	    _symbols [_symbolsSize - 1] = '\n';
	}
    }
    free (symbols);
}

#if SIZE_OF_LONG == 8
    #define ADDRESS_FMT	"%16p  "
#else
    #define ADDRESS_FMT	"%8p  "
#endif

/// Prints the backtrace to \p os.
void CBacktrace::text_write (ostringstream& os) const
{
    const char *ss = _symbols, *se;
    for (uoff_t i = 0; i < _nFrames; ++ i) {
	os.format (ADDRESS_FMT, _addresses[i]);
	se = strchr (ss, '\n') + 1;
	os.write (ss, distance (ss, se));
	ss = se;
    }
}

/// Reads the object from stream \p is.
void CBacktrace::read (istream& is)
{
    assert (is.aligned (stream_align_of (_addresses[0])) && "Backtrace object contains pointers and must be void* aligned");
    is >> _nFrames >> _symbolsSize;
    nfree (_symbols);
    _symbols = (char*) malloc (_symbolsSize + 1);
    is.read (_symbols, _symbolsSize);
    _symbols [_symbolsSize] = 0;
    is.align();
    is.read (_addresses, _nFrames * sizeof(void*));
}

/// Writes the object to stream \p os.
void CBacktrace::write (ostream& os) const
{
    assert (os.aligned (stream_align_of (_addresses[0])) && "Backtrace object contains pointers and must be void* aligned");
    os << _nFrames << _symbolsSize;
    os.write (_symbols, _symbolsSize);
    os.align();
    os.write (_addresses, _nFrames * sizeof(void*));
}

/// Returns the size of the written object.
size_t CBacktrace::stream_size (void) const
{
    return Align (stream_size_of (_nFrames) +
		   stream_size_of (_symbolsSize) +
		   _nFrames * sizeof(void*) +
		   _symbolsSize);
}

} // namespace ustl

namespace ustl {

/// \brief Attaches the object to pointer \p p of size \p n.
///
/// If \p p is nullptr and \p n is non-zero, bad_alloc is thrown and current
/// state remains unchanged.
///
void cmemlink::link (const void* p, size_type n)
{
    if (!p && n)
	throw bad_alloc (n);
    unlink();
    relink (p, n);
}

/// Writes the object to stream \p os
void cmemlink::write (ostream& os) const
{
    const written_size_type sz (size());
    assert (sz == size() && "No support for writing memblocks larger than 4G");
    os << sz;
    os.write (cdata(), sz);
    os.align (stream_align_of (sz));
}

/// Writes the object to stream \p os
void cmemlink::text_write (ostringstream& os) const
{
    os.write (begin(), readable_size());
}

/// Returns the number of bytes required to write this object to a stream.
cmemlink::size_type cmemlink::stream_size (void) const noexcept
{
    const written_size_type sz (size());
    return Align (stream_size_of (sz) + sz, stream_align_of(sz));
}

/// Writes the data to file \p "filename".
void cmemlink::write_file (const char* filename, int mode) const
{
    fstream f;
    f.exceptions (fstream::allbadbits);
    f.open (filename, fstream::out | fstream::trunc, mode);
    f.write (cdata(), readable_size());
    f.close();
}

/// Compares to memory block pointed by l. Size is compared first.
bool cmemlink::operator== (const cmemlink& l) const noexcept
{
    return l._size == _size &&
	    (l._data == _data || 0 == memcmp (l._data, _data, _size));
}

} // namespace ustl

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#if HAVE_SYS_MMAN_H
    #include <sys/mman.h>
#endif

namespace ustl {

/// Default constructor.
fstream::fstream (void) noexcept
: ios_base()
,_fd (-1)
,_filename()
{
    exceptions (goodbit);
}

/// Opens \p filename in \p mode.
fstream::fstream (const char* filename, openmode mode)
: ios_base()
,_fd (-1)
,_filename()
{
    exceptions (goodbit);
    open (filename, mode);
}

/// Attaches to \p nfd of \p filename.
fstream::fstream (int nfd, const char* filename)
: ios_base()
,_fd (-1)
,_filename()
{
    exceptions (goodbit);
    attach (nfd, filename);
}

/// Destructor. Closes if still open, but without throwing.
fstream::~fstream (void) noexcept
{
    clear (goodbit);
    exceptions (goodbit);	// Turn off exceptions
    close();			//  so close will not throw
    assert (!(rdstate() & badbit) && "close failed in the destructor! This may lead to loss of user data. Please call close() manually and either enable exceptions or check the badbit.");
}

/// Sets state \p s and throws depending on the exception setting.
void fstream::set_and_throw (iostate s, const char* op)
{
    if (ios_base::set_and_throw (s))
	throw file_exception (op, name());
}

/// Attaches to the given \p nfd.
void fstream::attach (int nfd, const char* filename)
{
    assert (filename && "Don't do that");
    _filename = filename;
    clear (goodbit);
    if (nfd < 0)
	set_and_throw (badbit, "open");
    close();
    _fd = nfd;
}

/// Detaches from the current fd.
void fstream::detach (void) noexcept
{
    _fd = -1;
    _filename.clear();
}

/// Converts openmode bits into libc open flags.
/*static*/ int fstream::om_to_flags (openmode m) noexcept
{
    static const int s_OMFlags [nombits] = {
	0,		// in
	O_CREAT,	// out
	O_APPEND,	// app
	O_APPEND,	// ate
	0,		// binary
	O_TRUNC,	// trunc
	O_NONBLOCK,	// nonblock
	0,		// nocreate
	O_NOCTTY	// noctty
    };
    int flags;
    if (O_RDONLY == in-1 && O_WRONLY == out-1 && O_RDWR == (in|out)-1)
	flags = (m - 1) & O_ACCMODE;
    else
	flags = ((m&(in|out))==(in|out)) ? O_RDWR : ((m&out) ? O_WRONLY : O_RDONLY);
    for (uoff_t i = 0; i < VectorSize(s_OMFlags); ++ i)
	flags |= s_OMFlags[i] & (!(m&(1<<i))-1);
    if (m & nocreate)
	flags &= ~O_CREAT;
    return flags;
}

/// \brief Opens \p filename in the given mode.
/// \warning The string at \p filename must exist until the object is closed.
void fstream::open (const char* filename, openmode mode, mode_t perms)
{
    int nfd = ::open (filename, om_to_flags(mode), perms);
    attach (nfd, filename);
}

/// Closes the file and throws on error.
void fstream::close (void)
{
    if (_fd < 0)
	return;	// already closed
    while (::close(_fd)) {
	if (errno != EINTR) {
	    set_and_throw (badbit | failbit, "close");
	    break;
	}
    }
    detach();
}

/// Moves the current file position to \p n.
off_t fstream::seek (off_t n, seekdir whence)
{
    off_t p = lseek (_fd, n, whence);
    if (p < 0)
	set_and_throw (failbit, "seek");
    return p;
}

/// Returns the current file position.
off_t fstream::pos (void) const noexcept
{
    return lseek (_fd, 0, SEEK_CUR);
}

/// Reads \p n bytes into \p p.
off_t fstream::read (void* p, off_t n)
{
    off_t br = 0;
    while ((br < n) & good())
	br += readsome (advance (p, br), n - br);
    return br;
}

/// Reads at most \p n bytes into \p p, stopping when it feels like it.
off_t fstream::readsome (void* p, off_t n)
{
    ssize_t brn;
    do { brn = ::read (_fd, p, n); } while ((brn < 0) & (errno == EINTR));
    if (brn > 0)
	return brn;
    else if ((brn < 0) & (errno != EAGAIN))
	set_and_throw (failbit, "read");
    else if (!brn && ios_base::set_and_throw (eofbit | failbit))
	throw stream_bounds_exception ("read", name(), pos(), n, 0);
    return 0;
}

/// Writes \p n bytes from \p p.
off_t fstream::write (const void* p, off_t n)
{
    off_t btw = n;
    while (btw) {
	const off_t bw = n - btw;
	ssize_t bwn = ::write (_fd, advance(p,bw), btw);
	if (bwn > 0)
	    btw -= bwn;
	else if (!bwn) {
	    if (ios_base::set_and_throw (eofbit | failbit))
		throw stream_bounds_exception ("write", name(), pos() - bw, n, bw);
	    break;
	} else if (errno != EINTR) {
	    if (errno != EAGAIN)
		set_and_throw (failbit, "write");
	    break;
	}
    }
    return n - btw;
}

/// Returns the file size.
off_t fstream::size (void) const
{
    struct stat st;
    st.st_size = 0;
    stat (st);
    return st.st_size;
}

/// Synchronizes the file's data and status with the disk.
void fstream::sync (void)
{
    if (fsync (_fd))
	set_and_throw (badbit | failbit, "sync");
}

/// Get the stat structure.
void fstream::stat (struct stat& rs) const
{
    if (fstat (_fd, &rs))
	throw file_exception ("stat", name());
}

/// Calls the given ioctl. Use IOCTLID macro to pass in both \p name and \p request.
int fstream::ioctl (const char* rname, int request, long argument)
{
    int rv = ::ioctl (_fd, request, argument);
    if (rv < 0)
	set_and_throw (failbit, rname);
    return rv;
}

/// Calls the given fcntl. Use FCNTLID macro to pass in both \p name and \p request.
int fstream::fcntl (const char* rname, int request, long argument)
{
    int rv = ::fcntl (_fd, request, argument);
    if (rv < 0)
	set_and_throw (failbit, rname);
    return rv;
}

void fstream::set_nonblock (bool v) noexcept
{
    int curf = max (0, fcntl (FCNTLID (F_GETFL)));
    if (v) curf |=  O_NONBLOCK;
    else   curf &= ~O_NONBLOCK;
    fcntl (FCNTLID (F_SETFL), curf);
}

#if HAVE_SYS_MMAN_H

/// Memory-maps the file and returns a link to it.
memlink fstream::mmap (off_t n, off_t offset)
{
    void* result = ::mmap (nullptr, n, PROT_READ | PROT_WRITE, MAP_SHARED, _fd, offset);
    if (result == MAP_FAILED)
	set_and_throw (failbit, "mmap");
    return memlink (result, n);
}

/// Unmaps a memory-mapped area.
void fstream::munmap (memlink& l)
{
    if (::munmap (l.data(), l.size()))
	set_and_throw (failbit, "munmap");
    l.unlink();
}

/// Synchronizes a memory-mapped area.
void fstream::msync (memlink& l)
{
    if (::msync (l.data(), l.size(), MS_ASYNC | MS_INVALIDATE))
	set_and_throw (failbit, "msync");
}

#endif

} // namespace ustl

namespace ustl {

memblock::memblock (void) noexcept		: memlink(), _capacity (0) { }
memblock::memblock (const void* p, size_type n) : memlink(), _capacity (0) { assign (p, n); }
memblock::memblock (size_type n)		: memlink(), _capacity (0) { resize (n); }
memblock::memblock (const cmemlink& b)		: memlink(), _capacity (0) { assign (b); }
memblock::memblock (const memlink& b)		: memlink(), _capacity (0) { assign (b); }
memblock::memblock (const memblock& b)		: memlink(), _capacity (0) { assign (b); }
memblock::~memblock (void) noexcept		{ deallocate(); }

void memblock::unlink (void) noexcept
{
    _capacity = 0;
    memlink::unlink();
}

/// resizes the block to \p newSize bytes, reallocating if necessary.
void memblock::resize (size_type newSize, bool bExact)
{
    if (_capacity < newSize + minimumFreeCapacity())
	reserve (newSize, bExact);
    memlink::resize (newSize);
}

/// Frees internal data.
void memblock::deallocate (void) noexcept
{
    if (_capacity) {
	assert (cdata() && "Internal error: space allocated, but the pointer is nullptr");
	assert (data() && "Internal error: read-only block is marked as allocated space");
	free (data());
    }
    unlink();
}

/// Assumes control of the memory block \p p of size \p n.
/// The block assigned using this function will be freed in the destructor.
void memblock::manage (void* p, size_type n) noexcept
{
    assert (p || !n);
    assert (!_capacity && "Already managing something. deallocate or unlink first.");
    link (p, n);
    _capacity = n;
}

/// "Instantiate" a linked block by allocating and copying the linked data.
void memblock::copy_link (void)
{
    const pointer p (begin());
    const size_t sz (size());
    if (is_linked())
	unlink();
    assign (p, sz);
}
 
/// Copies data from \p p, \p n.
void memblock::assign (const void* p, size_type n)
{
    assert ((p != (const void*) cdata() || size() == n) && "Self-assignment can not resize");
    resize (n);
    copy_n (const_pointer(p), n, begin());
}

/// \brief Reallocates internal block to hold at least \p newSize bytes.
///
/// Additional memory may be allocated, but for efficiency it is a very
/// good idea to call reserve before doing byte-by-byte edit operations.
/// The block size as returned by size() is not altered. reserve will not
/// reduce allocated memory. If you think you are wasting space, call
/// deallocate and start over. To avoid wasting space, use the block for
/// only one purpose, and try to get that purpose to use similar amounts
/// of memory on each iteration.
///
void memblock::reserve (size_type newSize, bool bExact)
{
    if ((newSize += minimumFreeCapacity()) <= _capacity)
	return;
    pointer oldBlock (is_linked() ? nullptr : data());
    const size_t alignedSize (NextPow2 (newSize));
    if (!bExact)
	newSize = alignedSize;
    pointer newBlock = (pointer) realloc (oldBlock, newSize);
    if (!newBlock)
	throw bad_alloc (newSize);
    if (!oldBlock & (cdata() != nullptr))
	copy_n (cdata(), min (size() + 1, newSize), newBlock);
    link (newBlock, size());
    _capacity = newSize;
}

/// Reduces capacity to match size
void memblock::shrink_to_fit (void)
{
    if (is_linked())
	return;
    pointer newBlock = (pointer) realloc (begin(), size());
    if (!newBlock && size())
	throw bad_alloc (size());
    _capacity = size();
    memlink::relink (newBlock, size());
}

/// Shifts the data in the linked block from \p start to \p start + \p n.
memblock::iterator memblock::insert (const_iterator start, size_type n)
{
    const uoff_t ip = start - begin();
    assert (ip <= size());
    resize (size() + n, false);
    memlink::insert (iat(ip), n);
    return iat (ip);
}

/// Shifts the data in the linked block from \p start + \p n to \p start.
memblock::iterator memblock::erase (const_iterator start, size_type n)
{
    const uoff_t ep = start - begin();
    assert (ep + n <= size());
    reserve (size() - n);
    iterator iep = iat(ep);
    memlink::erase (iep, n);
    memlink::resize (size() - n);
    return iep;
}

/// Reads the object from stream \p s
void memblock::read (istream& is)
{
    written_size_type n = 0;
    is >> n;
    if (!is.verify_remaining ("read", "ustl::memblock", n))
	return;
    resize (n);
    is.read (data(), writable_size());
    is.align (stream_align_of (n));
}

/// Reads the entire file \p "filename".
void memblock::read_file (const char* filename)
{
    fstream f;
    f.exceptions (fstream::allbadbits);
    f.open (filename, fstream::in);
    const off_t fsize (f.size());
    reserve (fsize);
    f.read (data(), fsize);
    f.close();
    resize (fsize);
}

memblock::size_type memblock::minimumFreeCapacity (void) const noexcept { return 0; }

} // namespace ustl


namespace ustl {

/// Reads the object from stream \p s
void memlink::read (istream& is)
{
    written_size_type n = 0;
    is >> n;
    if (!is.verify_remaining ("read", "ustl::memlink", n))
	return;
    if (n > size())
	throw length_error ("memlink can not increase the size of the linked storage for reading");
    resize (n);
    is.read (data(), n);
    is.align (stream_align_of (n));
}

/// Fills the linked block with the given pattern.
/// \arg start   Offset at which to start filling the linked block
/// \arg p       Pointer to the pattern.
/// \arg elSize  Size of the pattern.
/// \arg elCount Number of times to write the pattern.
/// Total number of bytes written is \p elSize * \p elCount.
///
void memlink::fill (const_iterator cstart, const void* p, size_type elSize, size_type elCount) noexcept
{
    assert (data() || !elCount || !elSize);
    assert (cstart >= begin() && cstart + elSize * elCount <= end());
    iterator start = const_cast<iterator>(cstart);
    if (elSize == 1)
	fill_n (start, elCount, *reinterpret_cast<const uint8_t*>(p));
    else while (elCount--)
	start = copy_n (const_iterator(p), elSize, start);
}

} // namespace ustl

namespace ustl {

//--------------------------------------------------------------------

/// Checks that \p n bytes are available in the stream, or else throws.
void ios_base::overrun (const char* op, const char* type, uint32_t n, uint32_t pos, uint32_t rem)
{
    if (set_and_throw (rem ? failbit : (failbit | eofbit)))
	throw stream_bounds_exception (op, type, pos, n, rem);
}

//--------------------------------------------------------------------

/// Attaches to the block pointed to by source of size source.pos()
istream::istream (const ostream& source) noexcept
: cmemlink (source.begin(), source.pos())
,_pos (0)
{
}

void istream::unlink (void) noexcept		{ cmemlink::unlink(); _pos = 0; }
void ostream::unlink (void) noexcept		{ memlink::unlink(); _pos = 0; }

/// Writes all unread bytes into \p os.
void istream::write (ostream& os) const
{
    os.write (ipos(), remaining());
}

/// Writes the object to stream \p os.
void istream::text_write (ostringstream& os) const
{
    os.write (ipos(), remaining());
}

/// Reads a null-terminated string into \p str.
void istream::read_strz (string& str)
{
    const_iterator zp = find (ipos(), end(), '\0');
    if (zp == end())
	zp = ipos();
    const size_type strl = distance (ipos(), zp);
    str.assign (ipos(), strl);
    _pos += strl + 1;
}

/// Reads at most \p n bytes into \p s.
istream::size_type istream::readsome (void* s, size_type n)
{
    if (remaining() < n)
	underflow (n);
    const size_type ntr (min (n, remaining()));
    read (s, ntr);
    return ntr;
}

streamsize istream::underflow (streamsize n)
{
    verify_remaining ("read", "byte", n);
    return remaining();
}

//--------------------------------------------------------------------

/// Aligns the write pointer on \p grain. The skipped bytes are zeroed.
void ostream::align (size_type grain)
{
    assert (!((grain-1)&grain) && "grain must be a power of 2");
    iterator ip = ipos();
    iterator ipa = iterator((uintptr_t(ip) + (grain-1)) & ~(grain-1));
    size_t nb = distance (ip, ipa);
#if WANT_STREAM_BOUNDS_CHECKING
    if (!verify_remaining ("align", "padding", nb))
	return;
#else
    assert (remaining() >= nb && "Buffer overrun. Check your stream size calculations.");
#endif
    memset (ip, '\x0', nb);
    _pos += nb;
}

/// Writes \p str as a null-terminated string.
void ostream::write_strz (const char* str)
{
    write (str, strlen(str)+1);
}

/// Writes all available data from \p is.
void ostream::read (istream& is)
{
    write (is.ipos(), is.remaining());
    is.seek (is.size());
}

/// Writes all written data to \p os.
void ostream::text_write (ostringstream& os) const
{
    os.write (begin(), pos());
}

/// Inserts an empty area of \p size, at \p start.
void ostream::insert (iterator start, size_type s)
{
    _pos += s;
    memlink::insert (start, s);
}

/// Erases an area of \p size, at \p start.
void ostream::erase (iterator start, size_type s)
{
    _pos -= s;
    memlink::erase (start, s);
}

//--------------------------------------------------------------------

} // namespace ustl

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
    reserve (default_stream_buffer_size);
}

/// Constructs a stream for writing to \p ofd
ofstream::ofstream (int ofd)
: ostringstream()
,_file (ofd)
{
    clear (_file.rdstate());
    reserve (default_stream_buffer_size);
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
    while (good() && pos() && overflow (capacity())) {}
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
    if (_file.good() && n > capacity() - pos()) {
	size_type bw = _file.write (cdata(), pos());
	erase (begin(), bw);
    }
    return ostringstream::overflow (n);
}

//----------------------------------------------------------------------

/// Constructs an unattached stream
ifstream::ifstream (void)
: istringstream()
,_buffer()
,_file()
{
    set_buffer_size (default_stream_buffer_size);
}

/// Constructs a stream to read from \p ifd.
ifstream::ifstream (int ifd)
: istringstream()
,_buffer()
,_file (ifd)
{
    set_buffer_size (default_stream_buffer_size);
}

/// Constructs a stream to read from \p filename.
ifstream::ifstream (const char* filename, openmode mode)
: istringstream()
,_buffer()
,_file (filename, mode)
{
    set_buffer_size (default_stream_buffer_size);
    clear (_file.rdstate());
}

/// Set the size of the input buffer
void ifstream::set_buffer_size (size_type sz)
{
    _buffer.resize (sz);
    #ifndef NDEBUG
	fill (_buffer.begin(), _buffer.end(), 0xcd);
    #endif
    link (_buffer.data(), streamsize(0));
}

/// Reads at least \p n more bytes and returns available bytes.
ifstream::size_type ifstream::underflow (size_type n)
{
    if (!_file.eof()) {
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
	if (_file.fd() == STDIN_FILENO)
	    cout.flush();

	size_type brn = 1;
	for (; br < oldPos + n && brn && _file.good(); br += brn)
	    brn = _file.readsome (_buffer.begin() + br, _buffer.size() - br);
	clear (_file.rdstate());

	_buffer[br] = 0;
	link (_buffer.data(), br);
	seek (oldPos);
    }
    if (_file.eof())
	verify_remaining ("read", _file.name(), n);
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

namespace ustl {

#define DEFAULT_DELIMITERS	" \t\n\r;:,.?"
const char ios_base::c_DefaultDelimiters [istringstream::c_MaxDelimiters] = DEFAULT_DELIMITERS;

/// Default constructor.
istringstream::istringstream (void) noexcept
: istream()
,_flags (0)
,_gcount (0)
{
    exceptions (goodbit);
    set_delimiters (DEFAULT_DELIMITERS);
}

istringstream::istringstream (const void* p, size_type n) noexcept
: istream()
,_flags (0)
,_gcount (0)
{
    exceptions (goodbit);
    relink (p, n);
    set_delimiters (DEFAULT_DELIMITERS);
}

istringstream::istringstream (const cmemlink& source) noexcept
: istream()
,_flags (0)
,_gcount (0)
{
    exceptions (goodbit);
    relink (source);
    set_delimiters (DEFAULT_DELIMITERS);
}

bool istringstream::is_delimiter (char c) const noexcept
{
    return memchr (_delimiters, c, VectorSize(_delimiters)-1);
}

char istringstream::skip_delimiters (void)
{
    char c = _delimiters[0];
    while (is_delimiter(c)) {
	if (!remaining() && !underflow()) {
	    verify_remaining ("read", "", 1);
	    return 0;
	}
	istream::iread (c);
    }
    return c;
}

//{{{ str_to_num
namespace {

typedef istringstream::iterator issiter_t;
template <typename T>
inline void str_to_num (issiter_t i, issiter_t* iend, unsigned base, T& v)
    { v = strtol (i, const_cast<char**>(iend), base); }
template <> inline void str_to_num (issiter_t i, issiter_t* iend, unsigned, double& v)
    { v = strtod (i, const_cast<char**>(iend)); }
#if HAVE_LONG_LONG && SIZE_OF_LONG_LONG > SIZE_OF_LONG
template <> inline void str_to_num (issiter_t i, issiter_t* iend, unsigned base, long long& v)
    { v = strtoll (i, const_cast<char**>(iend), base); }
#endif

} //}}} namespace

template <typename T>
inline void istringstream::read_number (T& v)
{
    v = 0;
    if (!skip_delimiters())
	return;
    ungetc();
    iterator ilast;
    do {
	str_to_num<T> (ipos(), &ilast, (_flags & hex) ? 16 : (_flags & oct) ? 8 : 0, v);
    } while (ilast == end() && underflow());
    skip (distance (ipos(), ilast));
}

void istringstream::iread (int& v)		{ read_number (v); }
void istringstream::iread (double& v)		{ read_number (v); } 
void istringstream::iread (long& v)		{ read_number (v); }
#if HAVE_LONG_LONG
void istringstream::iread (long long& v)	{ read_number (v); }
#endif

void istringstream::iread (wchar_t& v)
{
    if (!(v = skip_delimiters()))
	return;
    ungetc();
    size_t cs = Utf8SequenceBytes (v);
    if (remaining() < cs && underflow(cs) < cs)
	verify_remaining ("read", "wchar_t", cs);
    else {
	v = *utf8in (ipos());
	skip (cs);
    }
}

void istringstream::iread (bool& v)
{
    static const char tf[2][8] = { "false", "true" };
    char c = skip_delimiters();
    v = (c == 't' || c == '1');
    if (c != tf[v][0])
	return;
    for (const char* tv = tf[v]; c == *tv && (remaining() || underflow()); ++tv)
	istream::iread (c);
    ungetc();
}

void istringstream::iread (string& v)
{
    v.clear();
    char prevc, quoteChar = 0, c = skip_delimiters();
    if (!c)
	return;
    if (c == '\"' || c == '\'')
	quoteChar = c;
    else
	v += c;
    while (remaining() || underflow()) {
	prevc = c;
	istream::iread (c);
	if (!quoteChar && is_delimiter(c))
	    break;
	if (prevc == '\\') {
	    switch (c) {
		case 't':	c = '\t'; break;
		case 'n':	c = '\n'; break;
		case 'r':	c = '\r'; break;
		case 'b':	c = '\b'; break;
		case 'E':	c = 27;   break; // ESC sequence
		case '\"':	c = '\"'; break;
		case '\'':	c = '\''; break;
		case '\\':	c = '\\'; break;
	    };
	    v.end()[-1] = c;
	} else {
	    if (c == quoteChar)
		break;
	    v += c;
	}
    }
}

istringstream& istringstream::read (void* buffer, size_type sz)
{
    if (remaining() < sz && underflow(sz) < sz)
	verify_remaining ("read", "", sz);
    else
	istream::read (buffer, _gcount = sz);
    return *this;
}

/// Reads characters into \p p,n until \p delim is found (but not stored or extracted)
istringstream& istringstream::get (char* p, size_type n, char delim)
{
    _gcount = 0;
    for (char c = 0, *pend = p+n-1; p < pend && (remaining() || underflow()); ++p, ++_gcount) {
	istream::iread (c);
	if (c == delim) {
	    ungetc();
	    break;
	}
	*p = c;
    }
    *p = 0;
    return *this;
}

/// Reads characters into \p s until \p delim is found (but not stored or extracted)
istringstream& istringstream::get (string& v, char delim)
{
    _gcount = 0;
    v.clear();
    while ((remaining() || underflow()) && ipos()[0] != delim) {
	const_iterator p = ipos();
	size_type n = find (p, end(), delim) - p;
	v.append (p, n);
	skip (n);
	_gcount += n;
    }
    return *this;
}

/// Reads characters into \p s until \p delim is extracted (but not stored)
istringstream& istringstream::getline (string& s, char delim)
{
    get (s, delim);
    if (remaining() && ipos()[0] == delim) {
	skip (1);
	++_gcount;
    }
    return *this;
}

/// Reads characters into \p p,n until \p delim is extracted (but not stored)
istringstream& istringstream::getline (char* p, size_type n, char delim)
{
    get (p, n, delim);
    if (remaining() && ipos()[0] == delim) {
	skip (1);
	++_gcount;
    }
    return *this;
}

/// Extract until \p delim or \p n chars have been read.
istringstream& istringstream::ignore (size_type n, char delim)
{
    _gcount = n;
    while (n-- && (remaining() || underflow()) && get() != delim) {}
    _gcount -= n;
    return *this;
}

} // namespace ustl

namespace ustl {

/// Creates an output string stream linked to the given memory area.
ostringstream::ostringstream (void* p, size_t n) noexcept
: ostream()
,_buffer()
,_flags (0)
,_width (0)
,_precision (2)
,_fill (0)
{
    exceptions (goodbit);
    link (p, n);
}

/// Creates an output string stream, initializing the buffer with v.
ostringstream::ostringstream (const string& v)
: ostream()
,_buffer (v)
,_flags (0)
,_width (0)
,_precision (2)
,_fill (0)
{
    exceptions (goodbit);
    ostream::link (_buffer);
}

/// Copies \p s to the internal buffer.
void ostringstream::str (const string& s)
{
    _buffer = s;
    ostream::link (_buffer);
    SetPos (_buffer.size());
}

/// Writes a single character into the stream.
void ostringstream::iwrite (unsigned char v)
{
    if (remaining() >= 1 || overflow() >= 1)
	ostream::iwrite (v);
}

/// Writes the contents of \p buffer of \p size into the stream.
ostringstream& ostringstream::write (const void* buffer, size_type sz)
{
    const char* buf = (const char*) buffer;
    for (size_type bw = 0; (bw = min(sz, remaining() ? remaining() : overflow(sz))); buf += bw, sz -= bw)
	ostream::write (buf, bw);
    return *this;
}

/// Simple decimal encoding of \p n into \p fmt.
inline char* ostringstream::encode_dec (char* fmt, uint32_t n) const noexcept
{
    do {
	*fmt++ = '0' + n % 10;
    } while (n /= 10);
    return fmt;
}

/// Generates a sprintf format string for the given type.
void ostringstream::fmtstring (char* fmt, const char* typestr, bool bInteger) const
{
    *fmt++ = '%';
    if (_width) {
	if (_fill == '0')
	    *fmt++ = '0';
	fmt = encode_dec (fmt, _width);
    }
    if (_flags & left)
	*fmt++ = '-';
    if (bInteger) {
	if (_flags & showpos)
	    *fmt++ = '+';
	if (_flags & showbase)
	    *fmt++ = '#';
    } else {
	*fmt++ = '.';
	fmt = encode_dec (fmt, _precision);
    }
    while (*typestr)
	*fmt++ = *typestr++;
    if (bInteger) {
	if (_flags & hex)
	    fmt[-1] = (_flags & uppercase) ? 'X' : 'x';
	else if (_flags & oct)
	    fmt[-1] = 'o';
    } else if (_flags & scientific)
	fmt[-1] = 'E';
    *fmt = 0;
}

/// Writes \p v into the stream as utf8
void ostringstream::iwrite (wchar_t v)
{
    char buffer [8];
    *utf8out(buffer) = v;
    write (buffer, Utf8Bytes(v));
}

/// Writes value \p v into the stream as text.
void ostringstream::iwrite (bool v)
{
    static const char tf[2][8] = { "false", "true" };
    write (tf[v], 5 - v);
}

/// Equivalent to a vsprintf on the string.
int ostringstream::vformat (const char* fmt, va_list args)
{
#if HAVE_VA_COPY
    va_list args2;
#else
    #define args2 args
    #undef __va_copy
    #define __va_copy(x,y)
#endif
    int rv, space;
    do {
	space = remaining();
	__va_copy (args2, args);
	if (0 > (rv = vsnprintf (ipos(), space, fmt, args2)))
	    return rv;
    } while (rv >= space && rv < (int)overflow(rv+1));
    SetPos (pos() + min (rv, space));
    return rv;
}

/// Equivalent to a sprintf on the string.
int ostringstream::format (const char* fmt, ...)
{
    va_list args;
    va_start (args, fmt);
    const int rv = vformat (fmt, args);
    va_end (args);
    return rv;
}

/// Links to string \p l as resizable.
void ostringstream::link (void* p, size_type n) noexcept
{
    assert ((p || !n) && "The output string buffer must not be read-only");
    ostream::link (p, n);
    _buffer.link (p, n);
}

/// Attempts to create more output space. Returns remaining().
ostringstream::size_type ostringstream::overflow (size_type n)
{
    if (n > remaining() && (good() || n <= capacity() - pos())) {
	const uoff_t oldPos (pos());
	_buffer.reserve (oldPos + n, false);
	_buffer.resize (oldPos + n);
	ostream::link (_buffer);
	SetPos (oldPos);
    }
    verify_remaining ("write", "text", n);
    return remaining();
}

const Sendl endl;
const Sflush flush;

} // namespace ustl

namespace ustl {

// Generic version for implementing fill_nX_fast on non-i386 architectures.
template <typename T> static inline void stosv (T*& p, size_t n, T v)
    { while (n--) *p++ = v; }

#if __i386__ || __x86_64__

//----------------------------------------------------------------------
// Copy functions
//----------------------------------------------------------------------

static inline void movsb_dir_up (void) { asm volatile ("cld"); }
static inline void movsb_dir_down (void) { asm volatile ("std"); }

static inline void movsb (const void*& src, size_t nBytes, void*& dest)
{
    asm volatile ("rep;\n\tmovsb"
	: "=&S"(src), "=&D"(dest), "=&c"(nBytes)
	: "0"(src), "1"(dest), "2"(nBytes)
	: "memory");
}

static inline void movsd (const void*& src, size_t nWords, void*& dest)
{
    asm volatile ("rep;\n\tmovsl"
	: "=&S"(src), "=&D"(dest), "=&c"(nWords)
	: "0"(src), "1"(dest), "2"(nWords)
	: "memory");
}

#if CPU_HAS_MMX
template <> inline void stosv (uint8_t*& p, size_t n, uint8_t v)
{ asm volatile ("rep;\n\tstosb" : "=&D"(p), "=c"(n) : "0"(p), "1"(n), "a"(v) : "memory"); }
#endif
template <> inline void stosv (uint16_t*& p, size_t n, uint16_t v)
{ asm volatile ("rep;\n\tstosw" : "=&D"(p), "=c"(n) : "0"(p), "1"(n), "a"(v) : "memory"); }
template <> inline void stosv (uint32_t*& p, size_t n, uint32_t v)
{ asm volatile ("rep;\n\tstosl" : "=&D"(p), "=c"(n) : "0"(p), "1"(n), "a"(v) : "memory"); }

#if CPU_HAS_MMX
#define MMX_ALIGN	16U	// Data must be aligned on this grain
#define MMX_BS		32U	// Assembly routines copy data this many bytes at a time.

static inline void simd_block_copy (const void* src, void* dest)
{
    const char* csrc ((const char*) src);
    char* cdest ((char*) dest);
    #if CPU_HAS_SSE
    asm (
	"movaps\t%2, %%xmm0	\n\t"
	"movaps\t%3, %%xmm1	\n\t"
	"movntps\t%%xmm0, %0	\n\t"
	"movntps\t%%xmm1, %1"
	: "=m"(cdest[0]), "=m"(cdest[16])
	: "m"(csrc[0]), "m"(csrc[16])
	: "xmm0", "xmm1", "memory");
    #else
    asm (
	"movq	%4, %%mm0	\n\t"
	"movq	%5, %%mm1	\n\t"
	"movq	%6, %%mm2	\n\t"
	"movq	%7, %%mm3	\n\t"
	"movq	%%mm0, %0	\n\t"
	"movq	%%mm1, %1	\n\t"
	"movq	%%mm2, %2	\n\t"
	"movq	%%mm3, %3"
	: "=m"(cdest[0]), "=m"(cdest[8]), "=m"(cdest[16]), "=m"(cdest[24])
	: "m"(csrc[0]), "m"(csrc[8]), "m"(csrc[16]), "m"(csrc[24])
	: "mm0", "mm1", "mm2", "mm3", "st", "st(1)", "st(2)", "st(3)", "memory");
    #endif
}

static inline void simd_block_store (uint8_t* dest)
{
    #if CPU_HAS_SSE
    asm volatile (
	"movntq %%mm0, %0\n\t"
	"movntq %%mm0, %1\n\t"
	"movntq %%mm0, %2\n\t"
	"movntq %%mm0, %3"
	: "=m"(dest[0]), "=m"(dest[8]), "=m"(dest[16]), "=m"(dest[24])
	:: "memory");
    #else
    asm volatile (
	"movq %%mm0, %0	\n\t"
	"movq %%mm0, %1	\n\t"
	"movq %%mm0, %2	\n\t"
	"movq %%mm0, %3"
	: "=m"(dest[0]), "=m"(dest[8]), "=m"(dest[16]), "=m"(dest[24])
	:: "memory");
    #endif
}

static inline void simd_block_cleanup (void)
{
    #if !CPU_HAS_SSE
	simd::reset_mmx();
    #endif
    asm volatile ("sfence");
}

/// The fastest optimized raw memory copy.
void copy_n_fast (const void* src, size_t nBytes, void* dest) noexcept
{
    movsb_dir_up();
    size_t nHeadBytes = Align(uintptr_t(src), MMX_ALIGN) - uintptr_t(src);
    nHeadBytes = min (nHeadBytes, nBytes);
    movsb (src, nHeadBytes, dest);
    nBytes -= nHeadBytes;
    if (!(uintptr_t(dest) % MMX_ALIGN)) {
	const size_t nMiddleBlocks = nBytes / MMX_BS;
	for (uoff_t i = 0; i < nMiddleBlocks; ++ i) {
	    prefetch (advance (src, 512), 0, 0);
	    simd_block_copy (src, dest);
	    src = advance (src, MMX_BS);
	    dest = advance (dest, MMX_BS);
	}
	simd_block_cleanup();
	nBytes %= MMX_BS;
    }
    movsb (src, nBytes, dest);
}
#endif // CPU_HAS_MMX

/// The fastest optimized backwards raw memory copy.
void copy_backward_fast (const void* first, const void* last, void* result) noexcept
{
    prefetch (first, 0, 0);
    prefetch (result, 1, 0);
    size_t nBytes (distance (first, last));
    movsb_dir_down();
    size_t nHeadBytes = uintptr_t(last) % 4;
    last = advance (last, -1);
    result = advance (result, -1);
    movsb (last, nHeadBytes, result);
    nBytes -= nHeadBytes;
    if (uintptr_t(result) % 4 == 3) {
	const size_t nMiddleBlocks = nBytes / 4;
	last = advance (last, -3);
	result = advance (result, -3);
	movsd (last, nMiddleBlocks, result);
	nBytes %= 4;
    }
    movsb (last, nBytes, result);
    movsb_dir_up();
}
#endif // __i386__

//----------------------------------------------------------------------
// Fill functions
//----------------------------------------------------------------------

#if CPU_HAS_MMX
template <typename T> static inline void build_block (T) {}
template <> inline void build_block (uint8_t v)
{
    asm volatile (
	"movd %0, %%mm0\n\tpunpcklbw %%mm0, %%mm0\n\tpshufw $0, %%mm0, %%mm0"
	: : "g"(uint32_t(v)) : "mm0");
}
template <> inline void build_block (uint16_t v)
{
    asm volatile (
	"movd %0, %%mm0\n\tpshufw $0, %%mm0, %%mm0"
	: : "g"(uint32_t(v)) : "mm0");
}
template <> inline void build_block (uint32_t v)
{
    asm volatile (
	"movd %0, %%mm0\n\tpunpckldq %%mm0, %%mm0"
	: : "g"(uint32_t(v)) : "mm0");
}

static inline void simd_block_fill_loop (uint8_t*& dest, size_t count)
{
    prefetch (advance (dest, 512), 1, 0);
    for (const uint8_t* destEnd = dest + count * MMX_BS; dest < destEnd; dest += MMX_BS)
	simd_block_store (dest);
    simd_block_cleanup();
    simd::reset_mmx();
}

template <typename T>
static inline void fill_n_fast (T* dest, size_t count, T v)
{
    size_t nHead = Align(uintptr_t(dest), MMX_ALIGN) - uintptr_t(dest) / sizeof(T);
    nHead = min (nHead, count);
    stosv (dest, nHead, v);
    count -= nHead;
    build_block (v);
    uint8_t* bdest = (uint8_t*) dest;
    simd_block_fill_loop (bdest, count * sizeof(T) / MMX_BS);
    count %= MMX_BS;
    dest = (T*) bdest;
    stosv (dest, count, v);
}

void fill_n8_fast (uint8_t* dest, size_t count, uint8_t v) noexcept
    { fill_n_fast (dest, count, v); }
void fill_n16_fast (uint16_t* dest, size_t count, uint16_t v) noexcept
    { fill_n_fast (dest, count, v); }
void fill_n32_fast (uint32_t* dest, size_t count, uint32_t v) noexcept
    { fill_n_fast (dest, count, v); }
#else
void fill_n8_fast (uint8_t* dest, size_t count, uint8_t v) noexcept { memset (dest, v, count); }
void fill_n16_fast (uint16_t* dest, size_t count, uint16_t v) noexcept { stosv (dest, count, v); }
void fill_n32_fast (uint32_t* dest, size_t count, uint32_t v) noexcept { stosv (dest, count, v); }
#endif // CPU_HAS_MMX

/// Exchanges ranges [first, middle) and [middle, last)
void rotate_fast (void* first, void* middle, void* last) noexcept
{
#if HAVE_ALLOCA_H
    const size_t half1 (distance (first, middle)), half2 (distance (middle, last));
    const size_t hmin (min (half1, half2));
    if (!hmin)
	return;
    void* buf = alloca (hmin);
    if (buf) {
	if (half2 < half1) {
	    copy_n_fast (middle, half2, buf);
	    copy_backward_fast (first, middle, last);
	    copy_n_fast (buf, half2, first);
	} else {
	    copy_n_fast (first, half1, buf);
	    copy_n_fast (middle, half2, first);
	    copy_n_fast (buf, half1, advance (first, half2));
	}
    } else
#else
    if (first == middle || middle == last)
	return;
#endif
    {
	char* f = (char*) first;
	char* m = (char*) middle;
	char* l = (char*) last;
	reverse (f, m);
	reverse (m, l);
	while (f != m && m != l)
	    iter_swap (f++, --l);
	reverse (f, (f == m ? l : m));
    }
}

#if __GNUC__ < 4
size_t popcount (uint32_t v) noexcept
{
    const uint32_t w = v - ((v >> 1) & 0x55555555); // Algorithm from AMD optimization guide
    const uint32_t x = (w & 0x33333333) + ((w >> 2) & 0x33333333);
    return ((x + (x >> 4) & 0x0F0F0F0F) * 0x01010101) >> 24;
}

#if HAVE_INT64_T
/// \brief Returns the number of 1s in \p v in binary.
size_t popcount (uint64_t v) noexcept
{
    v -= (v >> 1) & UINT64_C(0x5555555555555555);		// Algorithm from Wikipedia
    v = (v & UINT64_C(0x3333333333333333)) + ((v >> 2) & UINT64_C(0x3333333333333333));
    v = (v + (v >> 4)) & UINT64_C(0x0F0F0F0F0F0F0F0F);
    return (v * UINT64_C(0x0101010101010101)) >> 56;
}
#endif	// HAVE_INT64_T
#endif	// !__GNUC__

//----------------------------------------------------------------------
// Miscellaneous instantiated stuff from headers which don't have enough
// to warrant creation of a separate file.cc
//----------------------------------------------------------------------

// Used in uspecial to print printable characters
const char _FmtPrtChr[2][8]={"'%c'","%d"};

} // namespace ustl

namespace ustl {

//----------------------------------------------------------------------

/// \brief Returns a descriptive error message. fmt="%s"
/// Overloads of this functions must set nullptr as the default fmt
/// argument and handle that case to provide a default format string
/// in case the user does not have a localized one. The format
/// string should be shown in the documentation to not require
/// translators to look through code. Also, this function must
/// not throw anything, so you must wrap memory allocation routines
/// (like string::format, for instance) in a try{}catch(...){} block.
///
void exception::info (string& msgbuf, const char*) const noexcept
{
    try { msgbuf.format ("%s", what()); } catch (...) {} // Ignore all exceptions
}

/// Reads the exception from stream \p is.
void exception::read (istream& is)
{
    uint32_t stmSize = 0;
    xfmt_t fmt = xfmt_Exception;
    is >> fmt >> stmSize >> _backtrace;
    assert (fmt == _format && "The saved exception is of a different type.");
    assert ((stmSize + 8) - exception::stream_size() <= is.remaining() && "The saved exception data is corrupt.");
    _format = fmt;
}

/// Writes the exception into stream \p os as an IFF chunk.
void exception::write (ostream& os) const
{
    os << _format << uint32_t(stream_size() - 8) << _backtrace;
}

/// Writes the exception as text into stream \p os.
void exception::text_write (ostringstream& os) const noexcept
{
    try {
	string buf;
	info (buf);
	os << buf;
    } catch (...) {}
}

//----------------------------------------------------------------------

#if HAVE_CXXABI_H && WANT_NAME_DEMANGLING
extern "C" char* __cxa_demangle (const char* mangled_name, char* output_buffer, size_t* length, int* status);
#endif

/// \brief Uses C++ ABI call, if available to demangle the contents of \p buf.
///
/// The result is written to \p buf, with the maximum size of \p bufSize, and
/// is zero-terminated. The return value is \p buf.
///
const char* demangle_type_name (char* buf, size_t bufSize, size_t* pdmSize) noexcept
{
    size_t bl = strlen (buf);
#if HAVE_CXXABI_H && WANT_NAME_DEMANGLING
    char dmname [256];
    size_t sz = VectorSize(dmname);
    int bFailed;
    __cxa_demangle (buf, dmname, &sz, &bFailed);
    if (!bFailed) {
	bl = min (strlen (dmname), bufSize - 1);
	memcpy (buf, dmname, bl);
	buf[bl] = 0;
    }
#else
    bl = min (bl, bufSize);
#endif
    if (pdmSize)
	*pdmSize = bl;
    return buf;
}

#if WITHOUT_LIBSTDCPP
} // namespace ustl
namespace std {
#endif

/// Initializes the empty object. \p nBytes is the size of the attempted allocation.
bad_alloc::bad_alloc (size_t nBytes) noexcept
: ustl::exception()
,_bytesRequested (nBytes)
{
    set_format (ustl::xfmt_BadAlloc);
}

/// Returns a descriptive error message. fmt="failed to allocate %d bytes"
void bad_alloc::info (ustl::string& msgbuf, const char* fmt) const noexcept
{
    if (!fmt) fmt = "failed to allocate %d bytes";
    try { msgbuf.format (fmt, _bytesRequested); } catch (...) {}
}

/// Reads the exception from stream \p is.
void bad_alloc::read (ustl::istream& is)
{
    ustl::exception::read (is);
    is >> _bytesRequested;
}

/// Writes the exception into stream \p os.
void bad_alloc::write (ustl::ostream& os) const
{
    ustl::exception::write (os);
    os << _bytesRequested;
}

/// Returns the size of the written exception.
size_t bad_alloc::stream_size (void) const noexcept
{
    return ustl::exception::stream_size() + ustl::stream_size_of(_bytesRequested);
}

} // namespace std

void* tmalloc (size_t n)
{
    void* p = malloc (n);
    if (!p)
	throw ustl::bad_alloc (n);
    return p;
}

void nfree (void* p) noexcept
{
    if (p)
	free (p);
}

#if WITHOUT_LIBSTDCPP
#if __APPLE__	// MacOS lives in the stone age and does not support aliases

void* operator new (size_t n)	{ return tmalloc(n); }
void* operator new[] (size_t n)	{ return tmalloc(n); }

void  operator delete (void* p) noexcept	{ nfree(p); }
void  operator delete[] (void* p) noexcept	{ nfree(p); }
#if HAVE_CPP14
void  operator delete (void* p, size_t) noexcept	{ nfree(p); }
void  operator delete[] (void* p, size_t) noexcept	{ nfree(p); }
#endif // HAVE_CPP14

#else // __APPLE__

void* operator new (size_t n)	WEAKALIAS("tmalloc");
void* operator new[] (size_t n)	WEAKALIAS("tmalloc");

void  operator delete (void* p) noexcept	WEAKALIAS("nfree");
void  operator delete[] (void* p) noexcept	WEAKALIAS("nfree");
#if HAVE_CPP14
void  operator delete (void* p, size_t n) noexcept	WEAKALIAS("nfree");
void  operator delete[] (void* p, size_t n) noexcept	WEAKALIAS("nfree");
#endif // HAVE_CPP14

#endif // __APPLE__
#endif // WITHOUT_LIBSTDCPP

namespace ustl {

//----------------------------------------------------------------------

/// \p arg contains a description of the error.
error_message::error_message (const char* arg) noexcept
: exception()
,_arg()
{
    try { _arg = arg; } catch (...) {}
    set_format (xfmt_ErrorMessage);
}

/// Virtual destructor
error_message::~error_message (void) noexcept
{
}

/// Returns a descriptive error message. fmt="%s: %s"
void error_message::info (string& msgbuf, const char* fmt) const noexcept
{
    if (!fmt) fmt = "%s: %s";
    try { msgbuf.format (fmt, name(), _arg.c_str()); } catch (...) {}
}

/// Reads the object from stream \p is.
void error_message::read (istream& is)
{
    exception::read (is);
    is >> _arg >> ios::align();
}

/// Writes the object to stream \p os.
void error_message::write (ostream& os) const
{
    exception::write (os);
    os << _arg << ios::align();
}

/// Returns the size of the written object.
size_t error_message::stream_size (void) const noexcept
{
    return exception::stream_size() + Align (stream_size_of (_arg));
}

//----------------------------------------------------------------------

/// Initializes the empty object. \p operation is the function that returned the error code.
system_error::system_error (const char* operation) noexcept
#if HAVE_CPP14
: system_error (errno, system_category(), operation)
#else
: runtime_error (strerror (errno))
,_operation (operation)
,_errno (errno)
#endif
{
    set_format (xfmt_SystemError);
}

/// Reads the exception from stream \p is.
void system_error::read (istream& is)
{
    runtime_error::read (is);
    is >> _operation >> ios::align() >> _errno >> ios::align();
}

/// Writes the exception into stream \p os.
void system_error::write (ostream& os) const
{
    runtime_error::write (os);
    os << _operation << ios::align() << _errno << ios::align();
}

/// Returns the size of the written exception.
size_t system_error::stream_size (void) const noexcept
{
    return runtime_error::stream_size() +
	    Align (stream_size_of(_errno)) +
	    Align (stream_size_of(_operation));
}

//----------------------------------------------------------------------

/// Initializes the empty object. \p operation is the function that returned the error code.
file_exception::file_exception (const char* operation, const char* filename) noexcept
: system_error (operation)
{
    memset (_filename, 0, VectorSize(_filename));
    set_format (xfmt_FileException);
    if (filename) {
	strncpy (_filename, filename, VectorSize(_filename));
	_filename [VectorSize(_filename) - 1] = 0;
    }
}

/// Returns a descriptive error message. fmt="%s %s: %s"
void file_exception::info (string& msgbuf, const char* fmt) const noexcept
{
    if (!fmt) fmt = "%s %s: %s";
    try { msgbuf.format (fmt, Operation(), Filename(), _arg.c_str()); } catch (...) {}
}

/// Reads the exception from stream \p is.
void file_exception::read (istream& is)
{
    system_error::read (is);
    string filename;
    is >> filename;
    is.align();
    strncpy (_filename, filename.c_str(), VectorSize(_filename));
    _filename [VectorSize(_filename)-1] = 0;
}

/// Writes the exception into stream \p os.
void file_exception::write (ostream& os) const
{
    system_error::write (os);
    os << string (_filename);
    os.align();
}

/// Returns the size of the written exception.
size_t file_exception::stream_size (void) const noexcept
{
    return system_error::stream_size() +
	    Align (stream_size_of (string (_filename)));
}

//----------------------------------------------------------------------

/// Initializes the empty object. \p operation is the function that returned the error code.
stream_bounds_exception::stream_bounds_exception (const char* operation, const char* type, uoff_t offset, size_t expected, size_t remaining) noexcept
: system_error (operation)
,_typeName (type)
,_offset (offset)
,_expected (expected)
,_remaining (remaining)
{
    set_format (xfmt_StreamBoundsException);
}

/// Returns a descriptive error message. fmt="%s stream %s: @%u: expected %u, available %u";
void stream_bounds_exception::info (string& msgbuf, const char* fmt) const noexcept
{
    char typeName [256];
    strncpy (typeName, _typeName, VectorSize(typeName));
    typeName[VectorSize(typeName)-1] = 0;
    if (!fmt) fmt = "%s stream %s: @0x%X: need %u bytes, have %u";
    try { msgbuf.format (fmt, demangle_type_name (VectorBlock(typeName)), Operation(), _offset, _expected, _remaining); } catch (...) {}
}

/// Reads the exception from stream \p is.
void stream_bounds_exception::read (istream& is)
{
    system_error::read (is);
    is >> _typeName >> _offset >> _expected >> _remaining;
}

/// Writes the exception into stream \p os.
void stream_bounds_exception::write (ostream& os) const
{
    system_error::write (os);
    os << _typeName << _offset << _expected << _remaining;
}

/// Returns the size of the written exception.
size_t stream_bounds_exception::stream_size (void) const noexcept
{
    return system_error::stream_size() +
	    stream_size_of(_typeName) +
	    stream_size_of(_offset) +
	    stream_size_of(_expected) +
	    stream_size_of(_remaining);
}

} // namespace ustl

