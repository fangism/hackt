// "art_utils.cc"

#include <assert.h>
#include <iostream>
// #include <fstream>
#include "art_utils.h"

//=============================================================================
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

//=============================================================================

/**
	Write a string to an output file stream (binary).  
 */
void
write_string(ostream& f, const string& s) {
	const string::size_type len = s.length();
		// excludes null-termination
	f.write((const char*) &len, sizeof(len));
	f.write(s.c_str(), len);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Reads a string from an input file stream (binary).  
 */
void
read_string(istream& f, string& s) {
	static const size_t def_size = 64;
	char def_buf[def_size];
	string::size_type len;
	f.read((char*) &len, sizeof(len));

	if (len -1 > def_size) {
		// too big for default
		char* big_buf = new char [len+1];	// extra space for \0
		assert(big_buf);
		f.read(big_buf, len);		// load into buffer
		big_buf[len] = '\0';
		s = big_buf;		// copy operation needs NULL-term string
		delete [] big_buf;		// free buffer
	} else {
		f.read(def_buf, len);		// load into buffer
		def_buf[len] = '\0';
		s = def_buf;		// copy operation needs NULL-term string
	}
	assert(s.length() == len);	// sanity check
}

//=============================================================================

