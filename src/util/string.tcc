/**
	\file "util/string.tcc"
	Implementations of some useful string functions.  
	$Id: string.tcc,v 1.5 2009/10/29 00:20:20 fang Exp $
 */

#ifndef	__UTIL_STRING_TCC__
#define	__UTIL_STRING_TCC__

#include <string>
#include <iterator>
#include <algorithm>
#include "util/string.h"
#include "util/sstream.h"

namespace util {
namespace strings {
using std::string;
using std::stringstream;
using std::istringstream;
using std::ostringstream;
//=============================================================================
/**
	Helper function for converting string to int.  
	Just a wrapper around using stringstream to convert to int.  
	\param I some integer type.  
	\return true on error.  
 */
template <class I>
bool
string_to_num(const string& s, I& i) {
	// prefer this over error-prone libc functions.  
	istringstream str(s);
	if (s.length() >= 2 && s[0] == '0' && s[1] == 'x') {
		str >> std::hex;
	}
	str >> i;
	return str.fail();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common routing for string transformations.
	\return new transformed copy of string.
 */
template <typename T>
string
transform_string(const string& s, T t) {
	string ret;
	std::transform(s.begin(), s.end(), std::back_inserter(ret), t);
	return ret;
}

//=============================================================================
}	// end namespace strings
}	// end namespace util

#endif	// __UTIL_STRING_TCC__

