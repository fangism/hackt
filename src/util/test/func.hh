/**
	\file "func.hh"
	Some test functions for pointer classes, not used for ART.
	This file is now obsolete.

	$Id: func.hh,v 1.5 2005/09/04 21:15:11 fang Exp $
 */

#include "util/obsolete/ptrs.h"

using namespace PTRS_NAMESPACE;

template <class T>
excl_ptr<T> sum(const never_const_ptr<T> a, const never_const_ptr<T> b) {
	// check a and b?
	return excl_ptr<T>(new T(*a +*b));
}

// e.g. T = int, var_base

