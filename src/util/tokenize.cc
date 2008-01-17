/**
	\file "util/tokenize.cc"
	$Id: tokenize.cc,v 1.3.96.1 2008/01/17 01:32:42 fang Exp $
 */

#include "util/tokenize.h"
#include <list>
#include <string>
#if 0
#include <algorithm>
#include <functional>
#else
#include <valarray>
#endif
#include "util/string.h"		// for eat_whitespace
#include "util/macros.h"
#if 0
#include "util/compose.h"
#endif
// #include <iostream>

namespace util {
using std::string;
#if 0
using std::find;
using std::find_if;
using std::logical_not;
using std::ptr_fun;
using ADS::unary_compose;
#else
using std::valarray;
#endif
//=============================================================================
/**
	\param s the string to tokenize.
	\param l the result list in which to append tokens.  
 */
void
tokenize(const string& s, string_list& l) {
#if 0
	typedef	string::const_iterator		const_iterator;
	const_iterator i(s.begin());
	const const_iterator e(s.end());
	do {
		// find non-whitespace
		i = find_if(i, e,
			unary_compose(logical_not<int>(), ptr_fun(&isspace)));
			// int should correspond to return type of isspace
			// TODO: use TMP to deduce this.
		if (i != e) {
			// find whitespace
			const const_iterator g(find_if(i, e, isspace));
			if (i != g) {
				l.push_back(string(i, g));
			}
			i = g;
		}
	} while (i != e);
#else
	static const char delim[] = " \t\n";
	// TODO: avoid allocating every time
	// copy to scratch space
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
#endif
}

//=============================================================================
}	// end namespace util

