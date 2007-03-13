/**
	\file "util/guile_gh.h"
	Configure-wrapped header to deprecated guile interface 'gh'.
	$Id: guile_gh.h,v 1.1 2007/03/13 04:04:41 fang Exp $
 */

#ifndef	__UTIL_GUILE_GH_H__
#define	__UTIL_GUILE_GH_H__

#include "config.h"
#ifdef	HAVE_GUILE_GH_H
#ifdef	SIZEOF_LONG_LONG
#undef	SIZEOF_LONG_LONG
#endif	// already defined by libguile/scmconfig.h
#include <guile/gh.h>
#endif

#endif	// __UTIL_GUILE_GH_H__

