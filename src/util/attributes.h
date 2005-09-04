/**
	"util/attributes.h"
	Configure-dependent use of compiler attributes.  
	This file is highly dependent on compiler characteristics.  
	$Id: attributes.h,v 1.2 2005/09/04 21:15:06 fang Exp $
 */

#ifndef	__UTIL_ATTRIBUTES_H__
#define	__UTIL_ATTRIBUTES_H__

#include "config.h"
// use configure to determine whether or not compiler supports attributes

//=============================================================================
// general attributes
/**
	Apply to types that are never used, but do some real work
	in their non-trivial constructors and destructors.  
	e.g. stacktrace, index, thread_lock, etc...

	Goal: re-enable -Wunused in warning flags.  
 */
#define	__ATTRIBUTE_UNUSED__		__attribute__ ((unused))

//=============================================================================
// type and variable attributes

#define	__ATTRIBUTE_ALIGNED__		__attribute__ ((aligned))
#define	__ATTRIBUTE_ALIGNED_SIZE__(x)	__attribute__ ((aligned(x)))
#define	__ATTRIBUTE_PACKED__		__attribute__ ((packed))

//=============================================================================
// function attributes

/**
	Use this in functions that do not return, such as exit and abort
	style functions.  
 */
#define	__ATTRIBUTE_NORETURN__		__attribute__ ((noreturn))

/**
	The const attribute asserts that this function has no
	side-effects: it only takes arguments and produces a 
	return value.  
 */
#define	__ATTRIBUTE_CONST__		__attribute__ ((const))


//=============================================================================

#endif	// __UTIL_ATTRIBUTES_H__

