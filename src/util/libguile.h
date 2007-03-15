/**
	\file "util/libguile.h"
	$Id: libguile.h,v 1.3 2007/03/15 06:11:12 fang Exp $
	Include wrapper for guile headers.  
	Also provide some convenient wrappers of our own.  
 */

#ifndef	__UTIL_LIBGUILE_H__
#define	__UTIL_LIBGUILE_H__

#include "config.h"
#ifdef	HAVE_LIBGUILE_H
#ifdef	SIZEOF_LONG_LONG
#undef	SIZEOF_LONG_LONG
#endif	// b/c redefined by <libguile/__scm.h>
#include <libguile.h>
// really only need <libguile/tags.h> for the SCM type

namespace util {
namespace guile {
/**
	Function pointer type for procedure casting.  Ewww...
 */
typedef	SCM (*scm_gsubr_type)();

//-----------------------------------------------------------------------------
/// wrapper for scm_pair_p, provided by guile 1.8
#ifndef	HAVE_SCM_IS_PAIR
inline
bool
scm_is_pair(const SCM& s) {
	return SCM_NFALSEP(scm_pair_p(s));
//	return SCM_EQ_P(scm_pair_p(s), SCM_BOOL_T);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// wrapper for scm_string_p, provided by guile 1.8
#ifndef	HAVE_SCM_IS_STRING
inline
bool
scm_is_string(const SCM& s) {
	return SCM_NFALSEP(scm_string_p(s));
}
#endif

//-----------------------------------------------------------------------------
void
scm_assert_string(const SCM&, const char* fn, const int pos);

void
scm_assert_pair(const SCM&, const char* fn, const int pos);

//-----------------------------------------------------------------------------
}	// end namespace guile
}	// end namespace util

#endif	// HAVE_LIBGUILE_H
#endif	// __UTIL_LIBGUILE_H__

