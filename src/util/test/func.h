/**
	\file "func.h"
	Some test functions for pointer classes, not used for ART.
	$Id: func.h,v 1.2 2004/11/02 07:52:21 fang Exp $
 */

#include "../ptrs.h"

using namespace PTRS_NAMESPACE;

template <class T>
excl_ptr<T> sum(const never_const_ptr<T> a, const never_const_ptr<T> b) {
	// check a and b?
	return excl_ptr<T>(new T(*a +*b));
}

// e.g. T = int, var_base

