/**
	\file "util/string.tcc"
	Implementations of some useful string functions.  
	$Id: string.tcc,v 1.3 2008/07/17 03:29:37 fang Exp $
 */

#ifndef	__UTIL_STRING_TCC__
#define	__UTIL_STRING_TCC__

#include <string>
#include "util/string.h"
// #include "util/inttypes.h"
#include "util/sstream.h"

namespace util {
namespace strings {
using std::stringstream;
using std::istringstream;
using std::ostringstream;
//=============================================================================
#if 0
template <>
struct string_to_int_converter<int32_t> {
	typedef	int32_t			integer_type;

	/**
		NOTE: atoi may be deprecated on some systems, 
		should probably prefer strtol.  
	 */
	integer_type
	operator () (const char* s) const {
		return atoi(s);
	}

	integer_type
	operator () (const std::string& s) const {
		return (*this)(s.c_str());
	}

};	// end struct string_to_int_converter
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for converting string to int.  
	Just a wrapper around using stringstream to convert to int.  
	\param I some integer type.  
	\return true on error.  
 */
template <class I>
bool
string_to_num(const std::string& s, I& i) {
#if 0
	i = string_to_int_converter<I>()(s);
	return errno;
#else
	// prefer this over error-prone libc functions.  
	istringstream str(s);
	if (s.length() >= 2 && s[0] == '0' && s[1] == 'x') {
		str >> std::hex;
	}
	str >> i;
	return str.fail();
#endif
}

//=============================================================================
}	// end namespace strings
}	// end namespace strings

#endif	// __UTIL_STRING_TCC__

