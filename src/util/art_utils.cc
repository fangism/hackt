/**
	\file "art_utils.cc"
	Utility function definitions (for non-templates only). 
	$Id: art_utils.cc,v 1.5 2004/11/02 07:52:10 fang Exp $
 */

#include <assert.h>
#include <iostream>
#include "art_utils.h"

using namespace std;

//=============================================================================
// arbitrary sanity check
#define STRING_LIMIT		256

//=============================================================================
#if 0
namespace ART {

// inline
ostream& operator << (ostream& o, const line_range& l) {
	if (l.start.line == l.end.line) {
		o << "[" << l.start.line << ":" << l.start.col;
		if (l.start.col != l.end.col)
			o << ".." << l.end.col;
		o << "]";
	} else {
		o << "[" << l.start.line << ":" << l.start.col << "--"
			<< l.end.line << ":" << l.end.col << "]";
	}
	return o;
}

};	// end namespace ART
#endif

//=============================================================================
namespace util {
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
	String specialization of binary writing.
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
	f.write((const char*) &len, sizeof(len));
//	assert(len >= 0);		// unsigned, always true
	if (len)
		f.write(s.c_str(), len);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Reads a string from an input file stream (binary).  
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
		char* big_buf = new char [len+1];	// extra space for \0
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

