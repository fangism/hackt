/**
	\file "util/guile_gh.h"
	Configure-wrapped header to deprecated guile interface 'gh'.
	$Id: guile_gh.h,v 1.3 2008/11/23 17:54:46 fang Exp $
 */

#ifndef	__UTIL_GUILE_GH_H__
#define	__UTIL_GUILE_GH_H__

#include "config.h"
#ifdef	HAVE_GUILE_GH_H
#ifdef	SIZEOF_LONG_LONG
#undef	SIZEOF_LONG_LONG
#endif	/* already defined by libguile/scmconfig.h */
#ifdef	__cplusplus
#include <cstdio>	// for std::FILE, required by <gmp.h>
#else
#include <stdio.h>
#endif
#include <guile/gh.h>

#endif	/* HAVE_GUILE_GH_H */
#endif	/* __UTIL_GUILE_GH_H__ */

