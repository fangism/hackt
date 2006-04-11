/**
	\file "util/static_assert.h"
	Compile-time assertion, implemented with template struct.
	Is like a concept/constraint check.  
	Ah, the wonders of template meta-programming.  
	$Id: static_assert.h,v 1.4 2006/04/11 07:54:48 fang Exp $
 */

#ifndef	__UTIL_STATIC_ASSERT_H__
#define	__UTIL_STATIC_ASSERT_H__

#include "util/cppcat.h"
#include "util/attributes.h"

namespace util {

/**
	Intentionally undefined.  
 */
template <bool>
struct must_be_true;

/**
	Specialized case for true parameter.  
 */
template <>
struct must_be_true<true> {
	typedef	must_be_true		type;
};

}	// end namespace util

/**
	Static assertion will pass only if x is true because
	must_be_true<false> has no type member.  
 */
#define	UTIL_STATIC_ASSERT(x)						\
	typedef	util::must_be_true<x >::type UNIQUIFY(__check_type_)

/**
	Template-dependent version.  
 */
#define	UTIL_STATIC_ASSERT_DEPENDENT(x)					\
	typedef	typename util::must_be_true<x >::type UNIQUIFY(__check_type_)

#endif	// __UTIL_STATIC_ASSERT_H__

