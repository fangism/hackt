/**
	\file "util/convert.h"
	Utility for explicitly performing type conversions through functor.
	$Id: convert.h,v 1.2 2007/10/08 01:21:59 fang Exp $
 */

#ifndef	__UTIL_CONVERT_H__
#define	__UTIL_CONVERT_H__


#include <functional>

namespace util {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class To, class From>
struct converter : public std::unary_function<From, To> {
	To
	operator () (const From& f) const {
		return To(f);
	}
};	// end struct converter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class To>
struct auto_converter {
	template <class From>
	To
	operator () (const From& f) const {
		return To(f);
	}
};	// end struct auto_converter

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace util

#endif	// __UTIL_CONVERT_H__

