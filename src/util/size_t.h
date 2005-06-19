/**
	\file "util/size_t.h"
	Configuration-wrapped header file for the size_t type.
	$Id: size_t.h,v 1.1.2.1 2005/06/19 01:31:54 fang Exp $
 */
#ifndef	__UTIL_SIZE_T_H__
#define	__UTIL_SIZE_T_H__

#include "config.h"

#if defined(HAVE_STDDEF_H) && HAVE_STDDEF_H
// size_t is usually typedef'd in <stddef.h>
#include <cstddef>
// <cstddef> is the standard C++ wrapper for C's <stddef.h>
#endif

#endif	// __UTIL_SIZE_T_H__

