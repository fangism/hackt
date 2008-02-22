/**
	\file "util/numeric/div.h"
	Template for division to compute quotient and remainder in one call.  
	$Id: div.h,v 1.1.4.1 2008/02/22 06:07:34 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_DIV_H__
#define	__UTIL_NUMERIC_DIV_H__

#include "config.h"
#include <cstdlib>

namespace util {
namespace numeric {

//=============================================================================
/**
	Generic pair(quotient, remainder).
 */
template <typename I>
struct div_t_generic {
	I		quot;
	I		rem;

	div_t_generic(const I& q, const I& r) : quot(q), rem(r) { }
};

/**
	Generic (unoptimized) functor, depends on '/' and '%' operators
	being defined on the argument type.  
	Generally slower tha stdlib functions because of double operation.  
 */
template <typename I>
struct div_type {
	typedef	I			argument_type;
	typedef	div_t_generic<I>	return_type;

	return_type
	operator () (const argument_type n, const argument_type d) const {
		return return_type(n/d, n%d);
	}
};

// the remaining definitions are specializations
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct div_type<int> {
	typedef	int		argument_type;
	typedef	div_t		return_type;

	return_type
	operator () (const argument_type n, const argument_type d) const {
		return div(n, d);
	}
};	// end struct div_type<int>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct div_type<long> {
	typedef	long		argument_type;
	typedef	ldiv_t		return_type;

	return_type
	operator () (const argument_type n, const argument_type d) const {
		return ldiv(n, d);
	}
};	// end struct div_type<long>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if (SIZEOF_LONG_LONG > 0)
// and have lldiv_t type
template <>
struct div_type<long long> {
	typedef	long long	argument_type;
	typedef	lldiv_t		return_type;

	return_type
	operator () (const argument_type n, const argument_type d) const {
		return lldiv(n, d);
	}
};	// end struct div_type<long long>
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
 	Helper function to choose appropriate function using
	template argument deduction.  
	TODO: use call_traits for optimization
 */
template <typename I>
inline
typename div_type<I>::return_type
div(const I& n, const I& d) {
	return div_type<I>()(n, d);
}

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_DIV_H__

