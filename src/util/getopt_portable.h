/**
	\file "getopt_portable.h"
	Configuration-dependent locations for getopt prototypes.  
	(Not actually used yet.)
	$Id: getopt_portable.h,v 1.2 2004/11/02 07:52:14 fang Exp $
 */

#ifndef __GETOPT_PORTABLE_H__
#define __GETOPT_PORTABLE_H__
 
/*
	FreeBSD, Apple (OS X) and Sun systems declare getopt in unistd.h,
	other systems (Linux) use getopt.h
*/

#if defined (__APPLE__) || defined(__FreeBSD__) || \
	(defined (__SVR4) && defined (__sun))
#include <unistd.h>
#else
#include "getopt.h"
#endif
 
#endif	// __GETOPT_PORTABLE_H__

