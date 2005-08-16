/**
	\file "func.h"
	Some test functions for pointer classes, not used for ART.
	This file is now obsolete.

	$Id: func.h,v 1.4.94.1 2005/08/16 03:50:33 fang Exp $
 */

#include "util/obsolete/ptrs.h"

using namespace PTRS_NAMESPACE;

template <class T>
excl_ptr<T> sum(const never_const_ptr<T> a, const never_const_ptr<T> b) {
	// check a and b?
	return excl_ptr<T>(new T(*a +*b));
}

// e.g. T = int, var_base

