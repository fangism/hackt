/**
	\file "util/attributes.h"
	Configure-dependent use of compiler attributes.  
	This file is highly dependent on compiler characteristics.  
	TODO: define these conditionally depending on configure.
	Hint: use AC_COMPILE_IFELSE in configure.ac
	$Id: attributes.h,v 1.7 2006/02/25 04:55:03 fang Exp $
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
#if defined(HAVE_ATTRIBUTE_UNUSED)
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
#ifdef HAVE_ATTRIBUTE_UNUSED
#ifdef HAVE_ATTRIBUTE_UNUSED_BEFORE_CTOR
	/* known: g++-4.0 */
#define	__ATTRIBUTE_UNUSED_CTOR__(args)		__ATTRIBUTE_UNUSED__ args
#elif defined(HAVE_ATTRIBUTE_UNUSED_AFTER_CTOR)
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
#ifdef HAVE_ATTRIBUTE_VISIBILITY_DEFAULT
#define	__VISIBILITY_DEFAULT__	__attribute__ ((visibility("default")))
#else
#define	__VISIBILITY_DEFAULT__
#endif

/**
	Hidden symbol visibility.  
 */
#ifdef HAVE_ATTRIBUTE_VISIBILITY_HIDDEN
#define	__VISIBILITY_HIDDEN__	__attribute__ ((visibility("hidden")))
#else
#define	__VISIBILITY_HIDDEN__
#endif

//=============================================================================
// type and variable attributes

/**
	Align to natural boundary.  
 */
#ifdef	HAVE_ATTRIBUTE_ALIGNED
#define	__ATTRIBUTE_ALIGNED__		__attribute__ ((aligned))
#else
#define	__ATTRIBUTE_ALIGNED__
#endif

/**
	Align to manually sized boundary.  
 */
#ifdef	HAVE_ATTRIBUTE_ALIGNED_SIZE
#define	__ATTRIBUTE_ALIGNED_SIZE__(x)	__attribute__ ((aligned(x)))
#else
#define	__ATTRIBUTE_ALIGNED_SIZE__(x)
#endif

/**
	Pack structure into minimal amount of space regardless of 
	alignment requirements.  
 */
#ifdef	HAVE_ATTRIBUTE_PACKED
#define	__ATTRIBUTE_PACKED__		__attribute__ ((packed))
#else
#define	__ATTRIBUTE_PACKED__
#endif

//=============================================================================
// function attributes

/**
	Use this in functions that do not return, such as exit and abort
	style functions.  
 */
#ifdef HAVE_ATTRIBUTE_NORETURN
#define	__ATTRIBUTE_NORETURN__		__attribute__ ((noreturn))
#else
#define	__ATTRIBUTE_NORETURN__
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The const attribute asserts that this function has no
	side-effects: it only takes arguments and produces a 
	return value.  
 */
#ifdef HAVE_ATTRIBUTE_CONST
#define	__ATTRIBUTE_CONST__		__attribute__ ((const))
#else
#define	__ATTRIBUTE_CONST__
#endif

#ifdef HAVE_ATTRIBUTE_PURE
#define	__ATTRIBUTE_PURE__		__attribute__ ((pure))
#else
#define	__ATTRIBUTE_PURE__
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The malloc attribute asserts that this non-NULL pointers returned
	by this function do not alias, just like malloc.
	This helps alias-analysis based optimizations.  
 */
#ifdef HAVE_ATTRIBUTE_MALLOC
#define	__ATTRIBUTE_MALLOC__		__attribute__ ((malloc))
#else
#define	__ATTRIBUTE_MALLOC__
#endif


//=============================================================================

#endif	// __UTIL_ATTRIBUTES_H__

