/**
	\file "macros.h"
	Macros for general use.  

	Some predefinable macros can be used to control compilations:
	DISABLE_INVARIANT turns off invariant checking.  
	DISABLE_NULL_CHECK turns off null pointer checks.  

	$Id: macros.h,v 1.3 2004/11/30 01:25:21 fang Exp $
 */

#ifndef	__MACROS_H__
#define	__MACROS_H__

#ifndef	NULL
#define	NULL			0
#endif

#include <assert.h>

//=============================================================================
/**
	Checks for NULL before deleting. 
	Then NULLs out pointer (optional).  
	Improper use of such a deleter pointer will result in null dereference.
	For less performance overhead, remove the NULL-ing statement.  
	Argument can be any pointer.

	NOTE: please consider using pointer-classes before resorting to 
	quick-and-dirty bare-pointer manipulation, even in classes.  
 */
#define	SAFEDELETE(x)	{ if (x) delete x; x = NULL; }

//=============================================================================
// type-cast and type checks...
// is there a way to use typeid() to infer type (even statically)
// to make dynamic_casts more convenient?

/**
	Abbreviation for dynamic_cast.  
	\param type must be a pointer or reference.  
 */
#define	IS_A(type, id)		dynamic_cast<type>(id)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifdef	DISABLE_INVARIANT
#define	MUST_BE_A(type, id)
#else
/**
	Abbreviation for dynamic_cast assertion.  
	Can be disabled with DISABLE_INVARIANT.
 */
#define	MUST_BE_A(type, id)	assert(IS_A(type,id))
#endif


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Abbreviation for static_cast.  
 */
#define	AS_A(type, id)		static_cast<type>(id)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Abbreviation for reinterpret_cast.
	Please don't use reinterpret_cast, they are distasteful and dangerous. 
 */
#define	TO_A(type, id)		reinterpret_cast<type>(id)

//=============================================================================
// various assertions, debug statements...
#ifdef	DISABLE_INVARIANT
/**
	Invariant assertions.  Really only intended for assertions.  
	Can be disabled by predefining INVARIANT, or defining
	DISABLE_INVARIANT.
 */
#define	INVARIANT(x)
#elif	!defined(INVARIANT)
#define	INVARIANT(x)		assert(x)
#endif

// of course invariant-checks may also come as expensive operations, 
// those we leave to specific instances.  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#ifdef	DISABLE_NULL_CHECK
#define	NEVER_NULL(x)
#define	MUST_BE_NULL(x)
#else
#define	NEVER_NULL(x)		assert(x)
#define	MUST_BE_NULL(x)		assert(!(x))
#endif

//=============================================================================
// exception throwing...

//=============================================================================
// error reporting...

#endif	// __MACROS_H__

