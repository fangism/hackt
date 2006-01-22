/**
	\file "util/size_t.h"
	Configuration-wrapped header file for the size_t type.
	TODO: look for minimal forward declaration.
	$Id: size_t.h,v 1.4 2006/01/22 06:53:36 fang Exp $
 */
#ifndef	__UTIL_SIZE_T_H__
#define	__UTIL_SIZE_T_H__

#include "config.h"

#if defined(__cplusplus) && defined(HAVE_CSTDDEF) && HAVE_CSTDDEF
// size_t is usually typedef'd in <stddef.h>
#include <cstddef>
#elif defined(HAVE_STDDEF_H) && HAVE_STDDEF_H
#include <stddef.h>
#else
#error	"I don't know where size_t is defined."
#endif

#endif	// __UTIL_SIZE_T_H__

