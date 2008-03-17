/**
	"util/numeric/functional.h"
	Functors for more numerical computations.
	$Id: functional.h,v 1.2 2008/03/17 23:03:12 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_FUNCTIONAL_H__
#define	__UTIL_NUMERIC_FUNCTIONAL_H__

#include <functional>		// for unary_function, binary_function

namespace util {
namespace numeric {
using std::unary_function;
//=============================================================================
/**
	Reciprocate.  Multiplicatove inverse.  
 */
template <class T>
struct reciprocate : public unary_function<T, T> {
	T
	operator () (const T& x) const {
		return T(1.0)/x;
	}
};	// end struct reciprocate

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_FUNCTIONAL_H__

