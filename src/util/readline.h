/**
	\file "util/readline.h"
	Header wrapper implementation for readline.  
	Includes the appropriate header from configuration. 
	$Id: readline.h,v 1.1.2.1 2005/12/14 05:16:53 fang Exp $
 */

#ifndef	__UTIL_READLINE_H__
#define	__UTIL_READLINE_H__

#include "config.h"
#include <cstdio>       /* for FILE */

#if	defined(HAVE_GNUREADLINE)
#include <readline/readline.h>
#include <readline/history.h>
#elif	defined(HAVE_BSDEDITLINE)
#include <editline/readline.h>
#endif	/* HAVE_GNUREADLINE || HAVE_BSDEDITLINE */

#endif	/* __UTIL_READLINE_H__ */
