/**
	\file "util/getopt_mapped.cc"
	$Id: getopt_mapped.cc,v 1.4 2008/11/23 17:54:45 fang Exp $
 */

#include "util/getopt_mapped.h"
#include <iostream>
#include <cstring>
#include "util/libc.h"		// for strsep

namespace util {
using std::vector;
using std::ostream;
using std::endl;

//=============================================================================
/**     
	Standard bad option error message.  
 */     
void
unknown_option(ostream& os, const int o) {
	if (isprint(o))
		os << "Unknown option `-" << char(o) << "'." << endl;
	else os << "Unknown option character `" <<
		reinterpret_cast<void*>(o) << "'." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Splits argument string (using strsep) into an array of 
	characters pointing to the beginning of each token, 
	accumulated in argv.  
	\param args the original string.
	\param argv return array for tokenized strings, shallow copy pointers.
	LIMITATION: does not recognize quoted strings in args.
	Will need lex to handle that.  
 */
void
splitopt(char* args, vector<char*>& argv) {
	static const char delim[] = " \t\n";
	char** stringp = &args;
	char* last;
	while ((last = strsep(stringp, delim))) {
		// eat consecutive whitespace
		if (*last == ' ') continue;
//		std::cout << "tok = " << last << endl;
		argv.push_back(last);
	}
}

//=============================================================================
}	// end namespace util

