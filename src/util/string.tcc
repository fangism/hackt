/**
	\file "util/string.tcc"
	Implementations of some useful string functions.  
	$Id: string.tcc,v 1.4 2009/08/28 20:45:29 fang Exp $
 */

#ifndef	__UTIL_STRING_TCC__
#define	__UTIL_STRING_TCC__

#include <string>
#include "util/string.h"
#include "util/sstream.h"

namespace util {
namespace strings {
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
string_to_num(const std::string& s, I& i) {
	// prefer this over error-prone libc functions.  
	istringstream str(s);
	if (s.length() >= 2 && s[0] == '0' && s[1] == 'x') {
		str >> std::hex;
	}
	str >> i;
	return str.fail();
}

//=============================================================================
}	// end namespace strings
}	// end namespace util

#endif	// __UTIL_STRING_TCC__

