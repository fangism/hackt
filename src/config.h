/**
	\file "config.h"
	Convenient wrapper for the one automatically generated
	by configure.
	$Id: config.h,v 1.2 2005/05/22 06:18:29 fang Exp $
 */

#ifndef	__CONFIG_H__
#define	__CONFIG_H__

// NOTE: the generated "config.h" doesn't have include-once protection!

// One can predefine -DIGNORE_CONFIG_H to suppress it, 
// but good luck trying to compile everything without it!
// (Decent chance of success, since config.h wasn't 
// depended upon for a long time.)

#if	defined(HAVE_CONFIG_H) && !defined(IGNORE_CONFIG_H)
#include "../config.h"
#endif

#endif	// __CONFIG_H__

