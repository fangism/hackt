/**
	\file "macros.h"
	Macros for general use.  
	$Id: macros.h,v 1.1 2004/11/05 02:38:49 fang Exp $
 */

#ifndef	__MACROS_H__
#define	__MACROS_H__

#include <stdlib.h>
	// defines NULL
#include <assert.h>

/**
	Checks for NULL before deleting. 
	Then NULLs out pointer (optional).  
	Improper use of such a deleter pointer will result in null dereference.
	For less performance overhead, remove the NULL-ing statement.  
	Argument must be a non-const pointer, 
	compiler will complain otherwise.  
 */
#define	SAFEDELETE(x)	{ if (x) delete x; x = NULL; }

// type-cast and type checks...
// is there a way to use typeid() to infer type (even statically)
// to make dynamic_casts more convenient?

/**
	Abbreviation for dynamic_cast.  
	\param type must be a pointer or reference.  
 */
#define	IS_A(type, id)		dynamic_cast<type>(id)

/**
	Abbreviation for dynamic_cast assertion.  
 */
#define	MUST_BE_A(type, id)	assert(IS_A(type,id))

/**
	Abbreviation for static_cast.  
 */
#define	AS_A(type, id)		static_cast<type>(id)

// various assertions, debug statements...

// exception throwing...

// error reporting...

#endif	// __MACROS_H__

