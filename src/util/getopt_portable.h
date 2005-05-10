/**
	\file "util/getopt_portable.h"
	Configuration-dependent locations for getopt prototypes.  
	(Not actually used yet.)
	$Id: getopt_portable.h,v 1.3 2005/05/10 04:51:23 fang Exp $
 */

#ifndef __UTIL_GETOPT_PORTABLE_H__
#define __UTIL_GETOPT_PORTABLE_H__
 
/*
	FreeBSD, Apple (OS X) and Sun systems declare getopt in unistd.h,
	other systems (Linux) use getopt.h
*/

#if defined (__APPLE__) || defined(__FreeBSD__) || \
	(defined (__SVR4) && defined (__sun))
#include <unistd.h>
#else
#include "util/getopt.h"
#endif
 
#endif	// __UTIL_GETOPT_PORTABLE_H__

