/**
	\file "util/libguile.hh"
	$Id: libguile.hh,v 1.9 2011/02/28 09:37:49 fang Exp $
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

// guile-1.6 header defect (-Wunused)
#ifndef	HAVE_SYS_STDTYPES_H
#define	HAVE_SYS_STDTYPES_H		0
#endif

// guile-1.8 libguile/inline.h header defect, under C++, use C90
#ifndef	__STDC_VERSION__
#define	__STDC_VERSION__	199001L
#endif

#include <cstdio>		// because <gmp.h> needs std::FILE
#include <libguile.h>
// really only need <libguile/tags.h> for the SCM type

//=============================================================================
/**
	This typedef appeared in guile 2.0.
	We define this in the global namespace, where it is provided.
 */
#if	!defined(HAVE_SCM_T_SUBR)
typedef	SCM (*scm_t_subr)();
#endif

namespace util {
namespace guile {
/**
	Function pointer type for procedure casting.  Ewww...
 */
typedef	SCM (*scm_gsubr_type)();

#ifndef	HAVE_SCM_T_BITS
#ifdef	HAVE_SCM_BITS_T
typedef	scm_bits_t		scm_t_bits;
#else
#error	"No equivalent type for scm_t_bits found."
#endif
#endif

//-----------------------------------------------------------------------------
/// fused call to define_gsubr followed by export
SCM
scm_c_define_gsubr_exported(const char*, const int, const int, const int, 
	scm_gsubr_type);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// defines a symbol value instead of a procedure
SCM
scm_c_define_exported(const char*, const SCM&);

//-----------------------------------------------------------------------------
/// wrapper for scm_pair_p, provided by guile 1.8
#ifndef	HAVE_SCM_IS_BOOL
inline
bool
scm_is_bool(const SCM& s) {
	return SCM_BOOLP(s);
}
#endif

//-----------------------------------------------------------------------------
/// wrapper for scm_pair_p, provided by guile 1.8
#ifndef	HAVE_SCM_IS_PAIR
inline
bool
scm_is_pair(const SCM& s) {
	return SCM_NFALSEP(scm_pair_p(s));	// assume must have
//	return SCM_EQ_P(scm_pair_p(s), SCM_BOOL_T);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// wrapper for scm_string_p, provided by guile 1.8
#ifndef	HAVE_SCM_IS_STRING
inline
bool
scm_is_string(const SCM& s) {
	return SCM_NFALSEP(scm_string_p(s));	// assume must have
}
#define	USING_SCM_IS_STRING	using util::guile::scm_is_string;
#else
#define	USING_SCM_IS_STRING
#endif

//-----------------------------------------------------------------------------
void
scm_assert_string(const SCM&, const char* fn, const int pos);

void
scm_assert_pair(const SCM&, const char* fn, const int pos);

//-----------------------------------------------------------------------------
#ifndef	HAVE_SCM_FROM_LOCALE_SYMBOL
SCM
scm_from_locale_symbol(const char*);
#define	USING_SCM_FROM_LOCALE_SYMBOL	using util::guile::scm_from_locale_symbol;
#else
#define	USING_SCM_FROM_LOCALE_SYMBOL
#endif

//-----------------------------------------------------------------------------
#ifndef	HAVE_SCM_ASSERT_SMOB_TYPE
void
scm_assert_smob_type(scm_t_bits tag, SCM val);

#define	USING_SCM_ASSERT_SMOB_TYPE	using util::guile::scm_assert_smob_type;
#else
#define	USING_SCM_ASSERT_SMOB_TYPE
#endif

//-----------------------------------------------------------------------------
}	// end namespace guile
}	// end namespace util

#endif	// HAVE_LIBGUILE_H
#endif	// __UTIL_LIBGUILE_H__

