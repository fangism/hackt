/**
	\file "func.h"
	Some test functions for pointer classes, not used for ART.
	This file is now obsolete.

	$Id: func.h,v 1.3 2004/11/28 23:46:48 fang Exp $
 */

#include "ptrs.h"

using namespace PTRS_NAMESPACE;

template <class T>
excl_ptr<T> sum(const never_const_ptr<T> a, const never_const_ptr<T> b) {
	// check a and b?
	return excl_ptr<T>(new T(*a +*b));
}

// e.g. T = int, var_base

