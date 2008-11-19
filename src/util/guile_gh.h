/**
	\file "util/guile_gh.h"
	Configure-wrapped header to deprecated guile interface 'gh'.
	$Id: guile_gh.h,v 1.2.46.1 2008/11/19 05:44:57 fang Exp $
 */

#ifndef	__UTIL_GUILE_GH_H__
#define	__UTIL_GUILE_GH_H__

#include "config.h"
#ifdef	HAVE_GUILE_GH_H
#ifdef	SIZEOF_LONG_LONG
#undef	SIZEOF_LONG_LONG
#endif	// already defined by libguile/scmconfig.h
#include <cstdio>	// for std::FILE, required by <gmp.h>
#include <guile/gh.h>

#endif	// HAVE_GUILE_GH_H
#endif	// __UTIL_GUILE_GH_H__

