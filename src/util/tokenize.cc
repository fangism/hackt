/**
	\file "util/tokenize.cc"
	$Id: tokenize.cc,v 1.1.2.1 2006/01/12 06:13:33 fang Exp $
 */

#include "util/tokenize.h"
#include <list>
#include <string>
#include <algorithm>
#include <functional>
#include "util/macros.h"
// #include "util/libc.h"
// #include "util/memory/excl_malloc_ptr.h"
#include "util/compose.h"

namespace util {
using std::string;
using std::find;
using std::find_if;
using std::logical_not;
using std::ptr_fun;
// using util::memory::excl_malloc_ptr;
using ADS::unary_compose;
//=============================================================================
/**
	\param s the string to tokenize.
	\param l the result list in which to append tokens.  
 */
void
tokenize(const string& s, string_list& l) {
#if 0
	const excl_malloc_ptr<char> temp(strdup(s.c_str()));
	NEVER_NULL(temp);
#else
	typedef	string::const_iterator		const_iterator;
	const_iterator i(s.begin());
	const const_iterator e(s.end());
	do {
		// find non-whitespace
		i = find_if(i, e,
			unary_compose(logical_not<int>(), ptr_fun(&isspace)));
		if (i != e) {
			// find whitespace
			const const_iterator g(find_if(i, e, isspace));
			if (i != g) {
				l.push_back(string(i, g));
			}
			i = g;
		}
	} while (i != e);
#endif
}

//=============================================================================
}	// end namespace util

