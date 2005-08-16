/**
	\file "util/static_assert.h"
	Compile-time assertion, implemented with template struct.
	Is like a concept/constraint check.  
	$Id: static_assert.h,v 1.2.4.1 2005/08/16 03:50:23 fang Exp $
 */

#ifndef	__UTIL_STATIC_ASSERT_H__
#define	__UTIL_STATIC_ASSERT_H__

#include "util/cppcat.h"
#include "util/attributes.h"

namespace util {

template <bool>
struct must_be_true;

template <>
struct must_be_true<true> {	enum { value = 1};	};

}	// end namespace util

/**
	Static assertion will pass only if x is true because
	must_be_true<false> has no value member.  
 */
#define	UTIL_STATIC_ASSERT(x)						\
	static const int						\
		UNIQUIFY(__check_) __ATTRIBUTE_UNUSED__ =		\
		util::must_be_true<x >::value;

#endif	// __UTIL_STATIC_ASSERT_H__

