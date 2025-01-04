/**
	\file "util/static_assert.hh"
	Compile-time assertion, implemented with template struct.
	Is like a concept/constraint check.  
	Ah, the wonders of template meta-programming.  
	TODO: migrate these to C++-native static_assert.
	$Id: static_assert.hh,v 1.5 2007/08/29 18:56:44 fang Exp $
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
	If argument 'x' contains a comma, 
	wrap x in an additional set of parenthesis to avoid preprocessor
	argument splitting.
	Mark type as unused because it is not used to declare data,
	and to appease compiler unused-diagnostics.
 */
#define	UTIL_STATIC_ASSERT(x)						\
	typedef	util::must_be_true<x >::type UNIQUIFY(__check_type_) __ATTRIBUTE_UNUSED__

/**
	Template-dependent version.  
 */
#define	UTIL_STATIC_ASSERT_DEPENDENT(x)					\
	typedef	typename util::must_be_true<x >::type UNIQUIFY(__check_type_) __ATTRIBUTE_UNUSED__

#endif	// __UTIL_STATIC_ASSERT_H__

