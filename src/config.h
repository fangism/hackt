/**
	\file "config.h"
	Convenient wrapper for the one automatically generated
	by configure.
	$Id: config.h,v 1.4 2005/12/10 03:56:51 fang Exp $
 */

#ifndef	__CONFIG_H__
#define	__CONFIG_H__

/***
	NOTE: the generated "config.h" doesn't have include-once protection!
	One can predefine -DIGNORE_CONFIG_H to suppress it, but good luck 
	trying to compile everything without it!  (Decent chance of success, 
	since config.h wasn't depended upon for a long time.)
***/

#if	defined(HAVE_CONFIG_H) && !defined(IGNORE_CONFIG_H)
#include "../config.h"
#endif

#ifdef	const
#error	Detected preprocessor definition of const.  \
	This means that a configure test found const non-ANSI-conforming.  \
	Sorry, but I refuse to let you compile this project with broken const.
#endif

#endif	/* __CONFIG_H__ */

