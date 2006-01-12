/**
	\file "util/readline.h"
	Header wrapper implementation for readline.  
	Includes the appropriate header from configuration. 
	$Id: readline.h,v 1.1.2.2 2006/01/12 06:13:32 fang Exp $
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

#if	defined(READLINE_PROMPT_CONST) && !READLINE_PROMPT_CONST
// your readline header sucks
#define RL_CONST_CAST(x)	const_cast<char_type*>(x)
#else
#define RL_CONST_CAST(x)	x
#endif

#endif	/* __UTIL_READLINE_H__ */
