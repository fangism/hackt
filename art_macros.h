// "art_macros.h"
// macros for general use in ART

#ifndef	__ART_MACROS_H__
#define	__ART_MACROS_H__

#include <stdlib.h>
#include <assert.h>

// checks for NULL before deleting, and then NULLs out pointer (optional)
// improper use of such a deleter pointer will result in null dereference.
// for less performance overhead, remove the NULL-ing statement.  
// x must be a pointer, compiler will complain otherwise, of course
#define	SAFEDELETE(x)	{ if (x) delete x; x = NULL; }

// type-cast and type checks...
// is there a way to use typeid() to infer type (even statically)
// to make dynamic_casts more convenient?

#define	IS_A(type, id)	dynamic_cast<type>(id)
#define	MUST_BE_A(type, id)	assert(IS_A(type,id))

// various assertions, debug statements...

// exception throwing...

// error reporting...

#endif	// __ART_MACROS_H__

