/**
	\file "util/tokenize.cc"
	$Id: tokenize.cc,v 1.7 2011/05/07 03:43:45 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <list>
#include <string>
#include <valarray>
#if ENABLE_STACKTRACE
#include <iostream>
#endif
#include "util/tokenize.hh"
#include "util/string.hh"		// for eat_whitespace
#include "util/macros.h"
#include "util/stacktrace.hh"

namespace util {
using std::string;
using std::valarray;
#if ENABLE_STACKTRACE
using std::endl;
#endif

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Simpler tokenizer with a single character separator.
	Appends to l the values delimited by c.
 */
void
tokenize_char(const string& s, string_list& l, const char c) {
	static const size_t end = string::npos;
	size_t f = 0;
	size_t a = s.find(c, f);
	while (a != end) {
		l.push_back(s.substr(f, a-f));
		f = a+1;
		a = s.find(c, f);
	}
	if (f != end) {
		l.push_back(s.substr(f));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Tokenizes on only *single* occurrences of char d.  
	Returns result in l.
	NOTE: this is a hack.  If you are calling this function, you probably
	should be using a real parser.
 */
void
tokenize_single_char_only(const std::string& s, string_list& l, const char d) {
	STACKTRACE_VERBOSE;
	const size_t len = s.length();
	size_t i = 0;
	for ( ; i<len; ) {
		size_t m = s.find_first_of(d, i);
		if (m == string::npos) {
			l.push_back(s.substr(i));
			break;
		}
		size_t n = s.find_first_not_of(d, m);
		if (n == string::npos) {
			l.push_back(s.substr(i, m-i));
			break;
		}
		if (n-m > 1) {
			// keep going
			m = s.find_first_of(d, n);
			if (m == string::npos) {
				l.push_back(s.substr(i));
				break;
			} else {
				l.push_back(s.substr(i, m-i));
			}
			i = m+1;
		} else {
			l.push_back(s.substr(i, m-i));
			i = n;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s original string
	\param sep separator string, e.g. "::"
	\param l resulting list
 */
void
tokenize_string(const std::string& s, const std::string& sep, string_list& l) {
	const size_t sl = sep.length();
	const size_t len = s.length();
	size_t i = 0;	// scan index
	for ( ; i<len; ) {
		size_t m = s.find(sep, i);
		if (m == string::npos) {
			l.push_back(s.substr(i));	// grab the remainder
			break;
		} else {
			l.push_back(s.substr(i, m-i));
			i = m+sl;
			if (i == len) {
				l.push_back("");
				break;
			}
		}
	}
}

//=============================================================================
}	// end namespace util

