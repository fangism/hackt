// "art_macros.h"
// macros for general use in ART

#ifndef	__ART_MACROS_H__
#define	__ART_MACROS_H__

#include <stdlib.h>
#include <assert.h>

// checks for NULL before deleting, and then NULLs out pointer (optional)
// improper use of such a deleter pointer will result in null dereference.
// for less performance overhead, remove the NULL-ing statement.  
#define	SAFEDELETE(x)	{ if (x) delete x; x = NULL; }

// type-cast and type checks...
// is there a way to use typeid() to infer type (even statically)
// to make dynamic_casts more convenient?

// various assertions, debug statements...

// exception throwing...

#endif	// __ART_MACROS_H__

