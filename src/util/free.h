/**
	\file "util/free.h"
	Proper forward declaration of libstdc's free().
	You'd think that this is trivial...
	$Id: free.h,v 1.2 2006/01/22 06:53:33 fang Exp $
 */

#ifndef	__UTIL_FREE_H__
#define	__UTIL_FREE_H__

#include "util/c_decl.h"

BEGIN_C_DECLS
/**
	TODO: check for presence of throw specification
 */
extern void free(void*)
#ifdef	__THROW
	__THROW
#endif
;
END_C_DECLS


#endif	/* __UTIL_FREE_H__ */

