/**
	\file "util/string.hh"
	Configure-detected string library header.  
	For now, this is really reserved for C++.
	$Id: string.h,v 1.7 2009/11/11 00:34:07 fang Exp $
 */

#ifndef	__UTIL_STRING_H__
#define	__UTIL_STRING_H__

#include "config.h"
#include "util/NULL.h"
#include "util/libc.h"

#ifdef	__cplusplus
	#if	defined(HAVE_CSTRING) && HAVE_CSTRING
	#include <cstring>
	#endif
	#include "util/string_fwd.hh"
#else
	#if	defined(HAVE_STRING_H) && HAVE_STRING_H
	#include <string.h>
	#endif
	#if	defined(HAVE_STRINGS_H) && HAVE_STRINGS_H
	#include <strings.h>
	#endif
#endif

BEGIN_C_DECLS
// C-stuff here

END_C_DECLS

#ifdef	__cplusplus
namespace util {
namespace strings {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param C is the character pointer/iterator type, 
	may be const, may be wchar_t.  
 */
template <class C>
inline
C
eat_whitespace(C& s) {
	if (s) {
		while (*s && isspace(*s))
			++s;
	}
	return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// Return false if conversion-assignment to int is successful.  
template <class I>
bool
string_to_num(const std::string&, I&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// regex-like functions
// waiting for std::regex <c++0x>

/**
	String global substitution.
	This is only based on exact-matching, not regular expressions!
	\param t the target string (modified)
	\param s the search pattern
	\param r the replacement string
	\return number of substitutions performed
 */
size_t
strgsub(std::string& t, const std::string& s, const std::string& r);


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
strip_prefix(std::string& t, const std::string& s);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
string_begins_with(const std::string&, const std::string&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <typename Trans>
std::string
transform_string(const std::string&, Trans);

std::string
string_tolower(const std::string&);

std::string
string_toupper(const std::string&);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace strings
}	// end namespace util
#endif	/* __cplusplus */

#endif	/* __UTIL_STRING_H__ */

