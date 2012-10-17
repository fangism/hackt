/**
	\file "util/libguile.cc"
	$Id: libguile.cc,v 1.4 2011/02/28 09:37:49 fang Exp $
	Include wrapper for guile headers.  
	Also provide some convenient wrappers of our own.  
 */

#include "util/libguile.hh"
#ifdef	HAVE_LIBGUILE_H
#include <iostream>

namespace util {
namespace guile {
#include "util/using_ostream.hh"

//-----------------------------------------------------------------------------
/**
	Define and export in current module, unified.  
 */
SCM
scm_c_define_gsubr_exported(const char* fn, const int req, const int opt, 
		const int rest, scm_gsubr_type f) {
	const SCM ret = scm_c_define_gsubr(fn, req, opt, rest, scm_t_subr(f));
	scm_c_export(fn, NULL);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Define and export a value, associated with a symbol.  
 */
SCM
scm_c_define_exported(const char* sym, const SCM& val) {
	const SCM ret = scm_c_define(sym, val);
	scm_c_export(sym, NULL);
	return ret;
}

//-----------------------------------------------------------------------------
/** 
	Convenient type check function.  
	Does not return in the event of a type error.  
 */
void
scm_assert_string(const SCM& s, const char* fn, const int pos) {
	if (!scm_is_string(s)) {
		// consider using scm_error/puts?
		cerr << "Error: expecting string argument." << endl;
		scm_wrong_type_arg(fn, pos, s);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/** 
	Convenient type check function.  
	Does not return in the event of a type error.  
 */
void
scm_assert_pair(const SCM& p, const char* fn, const int pos) {
	if (!scm_is_pair(p)) {
		// consider using scm_error/puts?
		cerr << "Error: expecting pair argument." << endl;
		scm_wrong_type_arg(fn, pos, p);
	}
}

//-----------------------------------------------------------------------------
#ifndef	HAVE_SCM_FROM_LOCALE_SYMBOL
SCM
scm_from_locale_symbol(const char* s) {
#if HAVE_SCM_STR2SYMBOL
	return scm_str2symbol(s);
#else
#error	"Missing const char* to SCM symbol constructor."
#endif
}
#endif

//-----------------------------------------------------------------------------
#ifndef	HAVE_SCM_ASSERT_SMOB_TYPE
/**
	Example taken from guile-1.6: type-checking.
	TODO: enable optional passing of argument context information.  
 */
void
scm_assert_smob_type(scm_t_bits tag, SCM val) {
	SCM_ASSERT(SCM_SMOB_PREDICATE(tag, val), val, 0, "unknown caller");
}
#endif

//-----------------------------------------------------------------------------
}	// end namespace guile
}	// end namespace util

#endif	// HAVE_LIBGUILE_H

