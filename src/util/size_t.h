/**
	\file "util/size_t.h"
	Configuration-wrapped header file for the size_t type.
	$Id: size_t.h,v 1.2.18.2 2005/08/18 05:41:36 fang Exp $
 */
#ifndef	__UTIL_SIZE_T_H__
#define	__UTIL_SIZE_T_H__

#include "config.h"

#if defined(HAVE_CSTDDEF) && HAVE_CSTDDEF
// size_t is usually typedef'd in <stddef.h>
#include <cstddef>
#elif defined(HAVE_STDDEF_H) && HAVE_STDDEF_H
#include <stddef.h>
#endif

#endif	// __UTIL_SIZE_T_H__

