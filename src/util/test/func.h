// "func.h"
// some test functions for pointer classes

#include "../ptrs.h"
// #include "var.h"

using namespace PTRS_NAMESPACE;

template <class T>
excl_ptr<T> sum(const never_const_ptr<T> a, const never_const_ptr<T> b) {
	// check a and b?
	return excl_ptr<T>(new T(*a +*b));
}

// e.g. T = int, var_base

