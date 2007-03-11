/**
	\file "util/libguile.h"
	$Id: libguile.h,v 1.1 2007/03/11 21:16:54 fang Exp $
	Include wrapper for guile headers.  
 */

#ifndef	__UTIL_LIBGUILE_H__
#define	__UTIL_LIBGUILE_H__

#include "config.h"
#ifdef	HAVE_LIBGUILE_H
#ifdef	SIZEOF_LONG_LONG
#undef	SIZEOF_LONG_LONG
#endif	// b/c redefined by <libguile/__scm.h>
#include <libguile.h>
// really only need <libguile/tags.h> for the SCM type
#endif

#endif	// __UTIL_LIBGUILE_H__

