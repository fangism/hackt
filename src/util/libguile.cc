/**
	\file "util/libguile.cc"
	$Id: libguile.cc,v 1.1 2007/03/15 06:11:10 fang Exp $
	Include wrapper for guile headers.  
	Also provide some convenient wrappers of our own.  
 */

#include "util/libguile.h"
#ifdef	HAVE_LIBGUILE_H
#include <iostream>

namespace util {
namespace guile {
#include "util/using_ostream.h"

//-----------------------------------------------------------------------------
/** 
	Convenient type check function.  
	Does not return in the event of a type error.  
 */
void
scm_assert_string(const SCM& s, const char* fn, const int pos) {
	if (!scm_is_string(s)) {
		// consider using scm_error?
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
		// consider using scm_error?
		cerr << "Error: expecting pair argument." << endl;
		scm_wrong_type_arg(fn, pos, p);
	}
}

//-----------------------------------------------------------------------------
}	// end namespace guile
}	// end namespace util

#endif	// HAVE_LIBGUILE_H

