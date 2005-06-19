/**
	\file "util/getopt_portable.h"
	Configuration-dependent locations for getopt prototypes.  
	(Not actually used yet.)
	$Id: getopt_portable.h,v 1.5.2.1 2005/06/19 01:31:49 fang Exp $
 */

#ifndef __UTIL_GETOPT_PORTABLE_H__
#define __UTIL_GETOPT_PORTABLE_H__
 
/*
	FreeBSD, Apple (OS X) and Sun systems declare getopt in <unistd.h>,
	other systems (Linux) use getopt.h, which often includes <unistd.h>
	and may provide a getopt_long function (glibc).  
*/

#include "config.h"

#if defined(HAVE_UNISTD_H) && HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(HAVE_GETOPT_H) && HAVE_GETOPT_H
#include <getopt.h>
#endif

// additional checks for <gnugetopt/getopt.h>
 
#endif	// __UTIL_GETOPT_PORTABLE_H__

