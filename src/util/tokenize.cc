/**
	\file "util/tokenize.cc"
	$Id: tokenize.cc,v 1.4.18.1 2009/08/27 20:38:51 fang Exp $
 */

#include "util/tokenize.h"
#include <list>
#include <string>
#include <valarray>
#include "util/string.h"		// for eat_whitespace
#include "util/macros.h"

namespace util {
using std::string;
using std::valarray;

//=============================================================================
/***
from man 3 strsep:

NOTES
       The strsep() function was introduced as a  replacement  for  strtok(),
       since  the  latter cannot handle empty fields.  However, strtok() con-
       forms to ANSI-C and hence is more portable.

BUGS
       This function suffers from the same problems as strtok().  In particu-
       lar, it modifies the original string. Avoid it.
***/

//=============================================================================
/**
	Default tokenize uses spaces.
 */
void
tokenize(const string& s, string_list& l) {
	static const char delim[] = " \t\n";
	tokenize(s, l, delim);
}

/**
	Hmm... still eats whitespace though.
	\param s the string to tokenize.
	\param l the result list in which to append tokens.  
 */
void
tokenize(const string& s, string_list& l, const char* delim) {
	// TODO: avoid allocating every time
	// copy to scratch space, or use stack-alloc (alloca)
	valarray<char> tmp(s.c_str(), s.length() +1);
	char* pp = &tmp[0];
	char** stringp = &pp;
	const char* last;
	while ((last = strsep(stringp, delim))) {
		last = strings::eat_whitespace(last);
		const string t(last);
		if (t.length()) {
			// std::cout << "\"" << t << "\"" << std::endl;
			l.push_back(t);
		}
	}
}

//=============================================================================
}	// end namespace util

