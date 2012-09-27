/**
	\file "util/IO_utils.cc"
	Utility function definitions (for non-templates only). 
	$Id: IO_utils.cc,v 1.7 2007/07/18 23:29:17 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <cassert>
#include <iostream>
#include <string>
#include <valarray>
#include "util/IO_utils.tcc"	// need to explicitly instantiate for char
#include "util/stacktrace.h"

//=============================================================================
/**
	arbitrary sanity check.
	Feel free to extend this limit.  
 */
#define STRING_LIMIT		256

//=============================================================================
namespace util {
#include "util/using_ostream.h"

//-----------------------------------------------------------------------------
/**
	Providing a bool specialization to force, 1-byte size
	writing of boolean to binary.  
	NOTE: darwin-gcc-3.3 defaults to int, 
		i686-linux gcc-3.4 defaults to char.
	\param f output stream.
	\param v boolean value.  
 */ 
template <>
void
write_value(ostream& f, const bool& b) {
	write_value<char>(f, b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Providing a bool specialization to force, 1-byte size
	reading of boolean from binary.  
	\param f output stream.
	\param v boolean value.  
 */
template <>
void
read_value(istream& f, bool& b) {
	char c;
	read_value(f, c);
	b = c;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// explicit instantiation needed for some compilers
template void write_value(ostream&, const char&);
template void read_value(istream&, char&);

//-----------------------------------------------------------------------------
/**
	String specialization of binary writing.
 */
template <>
void
write_value(ostream& f, const string& s) {
	write_string(f, s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	String specialization of binary reading.
 */
template <>
void
read_value(istream& f, string& s) {
	read_string(f, s);
}

//=============================================================================
/**
	Write a string to an output file stream (binary).  
 */
void
write_string(ostream& f, const string& s) {
	STACKTRACE_VERBOSE;
	const string::size_type len = s.length();
		// excludes null-termination
	if (len >= STRING_LIMIT) {
		// sanity check, not a real limit
		cerr << "FATAL: writing string length (" << len <<
			") exceeded maximum (" << STRING_LIMIT << ")." << endl;
		throw std::exception();
	}
	// careful: raw memory write
	write_value(f, len);
//	assert(len >= 0);		// unsigned, always true
	if (len) {
		f.write(s.c_str(), len);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Reads a string from an input file stream (binary).  
	Consider using a valarray<char>, or alloca.  
	Is alloca dangerous?
	Could use a static local vector that resizes...
 */
void
read_string(istream& f, string& s) {
	static const string::size_type def_size = 64;
	STACKTRACE_VERBOSE;
	string::size_type len;
	// careful: raw memory read
	read_value(f, len);
//	assert(len >= 0);		// unsigned, always true
#if ENABLE_STACKTRACE
	cerr << "len == " << len << endl;
#endif
	if (len >= STRING_LIMIT) {
		// sanity check, not a real limit
		cerr << "FATAL: reading string length (" << len <<
			") exceeded maximum (" << STRING_LIMIT << ")." << endl;
		throw std::exception();
	}
if (len) {
	if (len > def_size -1) {
		// too big for default
		// use excl_array_ptr<> or valarray<>
		std::valarray<char> big_buf(len+1);
		// extra space for \0
		// NEVER_NULL(big_buf);
		f.read(&big_buf[0], len);		// load into buffer
		big_buf[len] = '\0';
		// copy operation needs NULL-term string
		s = &big_buf[0];
		// delete [] big_buf;		// free buffer
	} else {
		char def_buf[def_size];
		f.read(def_buf, len);	// load into buffer
		def_buf[len] = '\0';
		s = def_buf;		// copy operation needs NULL-term string
	}
#if ENABLE_STACKTRACE
	if (s.length() != len) {
		cerr << "s.length() == " << s.length() << endl;
		cerr << "len == " << len << endl;
		cerr << "s == \"" << s << '\"' << endl;
	}
#endif
	INVARIANT(s.length() == len);	// sanity check
} else {
	s = "";
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace util

//=============================================================================

