/**
	\file "IO_utils.cc"
	Utility function definitions (for non-templates only). 
	$Id: IO_utils.cc,v 1.2.30.1 2005/02/28 18:47:44 fang Exp $
 */

#include <cassert>
#include <iostream>
#include <string>
#include "IO_utils.tcc"		// need to explicitly instantiate for char


//=============================================================================
// arbitrary sanity check
#define STRING_LIMIT		256

//=============================================================================
namespace util {
#include "using_ostream.h"

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
	const string::size_type len = s.length();
		// excludes null-termination
	assert(len < STRING_LIMIT);	// sanity check, not a real limit
	f.write(reinterpret_cast<const char*>(&len), sizeof(len));
//	assert(len >= 0);		// unsigned, always true
	if (len)
		f.write(s.c_str(), len);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Reads a string from an input file stream (binary).  
	Consider using a valarray<char>, or alloca.  
	Is alloca dangerous?
 */
void
read_string(istream& f, string& s) {
	static const size_t def_size = 64;
	string::size_type len;
	f.read((char*) &len, sizeof(len));
//	assert(len >= 0);		// unsigned, always true
	if (len >= STRING_LIMIT) {
		cerr << "read_string(): got len = " << len << ", WTF?" << endl;
		assert(len < STRING_LIMIT);
		// sanity check, not a real limit
	}
if (len) {
	if (len -1 > def_size) {
		// too big for default
		char* const big_buf = new char [len+1];	// extra space for \0
		assert(big_buf);
		f.read(big_buf, len);		// load into buffer
		big_buf[len] = '\0';
		s = big_buf;		// copy operation needs NULL-term string
		delete [] big_buf;		// free buffer
	} else {
		char def_buf[def_size];
		f.read(def_buf, len);		// load into buffer
		def_buf[len] = '\0';
		s = def_buf;		// copy operation needs NULL-term string
	}
	assert(s.length() == len);	// sanity check
} else {
	s == "";
}
}

}	// end namespace util

//=============================================================================

