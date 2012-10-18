/**
	\file "util/string.tcc"
	Implementations of some useful string functions.  
	$Id: string.tcc,v 1.6 2011/03/30 20:59:26 fang Exp $
 */

#ifndef	__UTIL_STRING_TCC__
#define	__UTIL_STRING_TCC__

#include <string>
#include <iterator>
#include <algorithm>
#include "util/string.hh"
#include "util/sstream.hh"

#include "util/numeric/sign_traits.hh"

namespace util {
namespace strings {
using std::string;
using std::stringstream;
using std::istringstream;
using std::ostringstream;
//=============================================================================
/**
	\param S true if type is signed
	\return true if rejected.
 */
template <bool S>
inline
bool
__reject_unsigned_negative(const char) {
	// negatives are allowed for signed, of course
	return false;
}

/**
	Specialization for unsigned, reject negatives.
 */
template <>
inline
bool
__reject_unsigned_negative<false>(const char f) {
	return f == '-';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for converting string to int.  
	Just a wrapper around using stringstream to convert to int.  
	\param I some integer type.  
	\return true on error.  

	See: http://gcc.gnu.org/bugzilla/show_bug.cgi?id=39802
	In older versions, parsing -1 into an unsigned type
	would fail-to-fail.  
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
#if !defined(CXX_ISTREAM_NEGATIVE_UNSIGNED_FAILS)
	// workaround compiler/library bug
	if (s.length() && __reject_unsigned_negative<
			util::numeric::is_signed<I>::value>(s[0]))
		return true;
#endif
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

