/**
	\file "util/string.h"
	Configure-detected string library header.  
	For now, this is really reserved for C++.
	$Id: string.h,v 1.2.26.1 2006/03/26 02:46:22 fang Exp $
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
	#include "util/string_fwd.h"
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

/**
	\param C is the character type, may be const, may be wchar_t.  
 */
template <class C>
inline
C*
eat_whitespace(C*& s) {
	if (s) {
		while (*s && isspace(*s))
			s++;
	}
	return s;
}

#if 0
/**
	Intentionally empty general definition.  
	Valid definitions only appear in specializations.  
 */
template <typename I>
struct string_to_int_converter { };
#endif

/// Return false if conversion-assignment to int is successful.  
template <class I>
bool
string_to_int(const std::string&, I&);

}	// end namespace strings
}	// end namespace util
#endif	/* __cplusplus */

#endif	/* __UTIL_STRING_H__ */

