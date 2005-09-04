/**
	\file "util/static_assert.h"
	Compile-time assertion, implemented with template struct.
	Is like a concept/constraint check.  
	Ah, the wonders of template meta-programming.  
	$Id: static_assert.h,v 1.3 2005/09/04 21:15:08 fang Exp $
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
	// enum { value = 1};
	typedef	void		type;
};

}	// end namespace util

/**
	Static assertion will pass only if x is true because
	must_be_true<false> has no type member.  
 */
#if 0
// old crappy definition
#define	UTIL_STATIC_ASSERT(x)						\
	static const int						\
		UNIQUIFY(__check_) __ATTRIBUTE_UNUSED__ =		\
		util::must_be_true<x >::value
#else
#define	UTIL_STATIC_ASSERT(x)						\
	typedef	typename util::must_be_true<x >::type UNIQUIFY(__check_type_)
#endif

#endif	// __UTIL_STATIC_ASSERT_H__

