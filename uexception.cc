// This file is part of the uSTL library, an STL implementation.
//
// Copyright (c) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.

#include "uexception.h"
#include "ustring.h"
#include "mistream.h"
#include "sostream.h"
#include "strmsize.h"
#include "uspecial.h"
#include <errno.h>

#if HAVE_CXXABI_H && WANT_NAME_DEMANGLING
extern "C" char* __cxa_demangle (const char* mangled_name, char* output_buffer, size_t* length, int* status);
#endif

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

#if WITHOUT_LIBSTDCPP
} // namespace std
namespace ustl {
#endif
//----------------------------------------------------------------------

/// Initializes the empty object. \p operation is the function that returned the error code.
libc_exception::libc_exception (const char* operation) noexcept
: exception()
,_errno (errno)
,_operation (operation)
{
    set_format (xfmt_LibcException);
}

/// Copies object \p v.
libc_exception::libc_exception (const libc_exception& v) noexcept
: exception (v)
,_errno (v._errno)
,_operation (v._operation)
{
}

/// Copies object \p v.
const libc_exception& libc_exception::operator= (const libc_exception& v)
{
    _errno = v._errno;
    _operation = v._operation;
    return *this;
}

/// Returns a descriptive error message. fmt="%s: %s"
void libc_exception::info (string& msgbuf, const char* fmt) const noexcept
{
    if (!fmt) fmt = "%s: %s";
    try { msgbuf.format (fmt, _operation, strerror(_errno)); } catch (...) {}
}

/// Reads the exception from stream \p is.
void libc_exception::read (istream& is)
{
    exception::read (is);
    is >> _errno >> _operation;
}

/// Writes the exception into stream \p os.
void libc_exception::write (ostream& os) const
{
    exception::write (os);
    os << _errno << _operation;
}

/// Returns the size of the written exception.
size_t libc_exception::stream_size (void) const noexcept
{
    return exception::stream_size() +
	    stream_size_of(_errno) +
	    stream_size_of(_operation);
}

//----------------------------------------------------------------------

/// Initializes the empty object. \p operation is the function that returned the error code.
file_exception::file_exception (const char* operation, const char* filename) noexcept
: libc_exception (operation)
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
    try { msgbuf.format (fmt, Operation(), Filename(), strerror(Errno())); } catch (...) {}
}

/// Reads the exception from stream \p is.
void file_exception::read (istream& is)
{
    libc_exception::read (is);
    string filename;
    is >> filename;
    is.align (8);
    strncpy (_filename, filename.c_str(), VectorSize(_filename));
    _filename [VectorSize(_filename)-1] = 0;
}

/// Writes the exception into stream \p os.
void file_exception::write (ostream& os) const
{
    libc_exception::write (os);
    os << string (_filename);
    os.align (8);
}

/// Returns the size of the written exception.
size_t file_exception::stream_size (void) const noexcept
{
    return libc_exception::stream_size() +
	    Align (stream_size_of (string (_filename)), 8);
}

//----------------------------------------------------------------------

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

//----------------------------------------------------------------------

/// Initializes the empty object. \p operation is the function that returned the error code.
stream_bounds_exception::stream_bounds_exception (const char* operation, const char* type, uoff_t offset, size_t expected, size_t remaining) noexcept
: libc_exception (operation)
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
    libc_exception::read (is);
    is >> _typeName >> _offset >> _expected >> _remaining;
}

/// Writes the exception into stream \p os.
void stream_bounds_exception::write (ostream& os) const
{
    libc_exception::write (os);
    os << _typeName << _offset << _expected << _remaining;
}

/// Returns the size of the written exception.
size_t stream_bounds_exception::stream_size (void) const noexcept
{
    return libc_exception::stream_size() +
	    stream_size_of(_typeName) +
	    stream_size_of(_offset) +
	    stream_size_of(_expected) +
	    stream_size_of(_remaining);
}

} // namespace ustl
