/**
	\file "util/attributes.h"
	Configure-dependent use of compiler attributes.  
	This file is highly dependent on compiler characteristics.  
	TODO: define these conditionally depending on configure.
	Hint: use AC_COMPILE_IFELSE in configure.ac
	$Id: attributes.h,v 1.6 2006/02/13 05:35:25 fang Exp $
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
	Also use for variable that are only used in assertion checks, 
	which are disabled by -DNDEBUG
	If all else fails, you can always -Wno-unused or -Wno-error
 */
#if HAVE_ATTRIBUTE_UNUSED
#define	__ATTRIBUTE_UNUSED__		__attribute__ ((unused))
#else
#define	__ATTRIBUTE_UNUSED__
#endif

/**
	In constructor syntax, some compilers like the attribute before the
	constructor arguments, some after.  YMMV.
	e.g.	foo bar __attribute__((unused)) (x, y, z);
	e.g.	foo bar (x, y, z) __attribute__((unused));
	To handle these cases automatically, wrap around the
	constructor arguments like so (yes, double parens are needed):
	foo bar __ATTRIBUTE_UNUSED_CTOR__((x, y, z));
 */
#if HAVE_ATTRIBUTE_UNUSED
#if HAVE_ATTRIBUTE_UNUSED_BEFORE_CTOR
	/* known: g++-4.0 */
#define	__ATTRIBUTE_UNUSED_CTOR__(args)		__ATTRIBUTE_UNUSED__ args
#elif HAVE_ATTRIBUTE_UNUSED_AFTER_CTOR
	/* known: g++-3.3 */
#define	__ATTRIBUTE_UNUSED_CTOR__(args)		args __ATTRIBUTE_UNUSED__
#else
/* #error I don't know where __attribute__((unused)) belongs! */
#define	__ATTRIBUTE_UNUSED_CTOR__(args)		args
#endif
#else
#define	__ATTRIBUTE_UNUSED_CTOR__(args)		args
#endif

//=============================================================================
// visibility attributes (support added in gcc-4.0)

/**
	Public symbol visibility.  
 */
#if HAVE_ATTRIBUTE_VISIBILITY_DEFAULT
#define	__VISIBILITY_DEFAULT__	__attribute__ ((visibility("default")))
#else
#define	__VISIBILITY_DEFAULT__
#endif

/**
	Hidden symbol visibility.  
 */
#if HAVE_ATTRIBUTE_VISIBILITY_HIDDEN
#define	__VISIBILITY_HIDDEN__	__attribute__ ((visibility("hidden")))
#else
#define	__VISIBILITY_HIDDEN__
#endif

//=============================================================================
// type and variable attributes

/**
	TODO: configure.ac should check for these
 */

#define	__ATTRIBUTE_ALIGNED__		__attribute__ ((aligned))
#define	__ATTRIBUTE_ALIGNED_SIZE__(x)	__attribute__ ((aligned(x)))
#define	__ATTRIBUTE_PACKED__		__attribute__ ((packed))

//=============================================================================
// function attributes

/**
	Use this in functions that do not return, such as exit and abort
	style functions.  
 */
#if HAVE_ATTRIBUTE_NORETURN
#define	__ATTRIBUTE_NORETURN__		__attribute__ ((noreturn))
#else
#define	__ATTRIBUTE_NORETURN__
#endif

/**
	The const attribute asserts that this function has no
	side-effects: it only takes arguments and produces a 
	return value.  
 */
#if HAVE_ATTRIBUTE_CONST
#define	__ATTRIBUTE_CONST__		__attribute__ ((const))
#else
#define	__ATTRIBUTE_CONST__
#endif


//=============================================================================

#endif	// __UTIL_ATTRIBUTES_H__

