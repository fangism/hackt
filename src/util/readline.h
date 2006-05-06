/**
	\file "util/readline.h"
	Header wrapper implementation for readline and editline.  
	Includes the appropriate header from configuration. 
	$Id: readline.h,v 1.4 2006/05/06 22:08:39 fang Exp $
 */

#ifndef	__UTIL_READLINE_H__
#define	__UTIL_READLINE_H__

#include "util/FILE_fwd.h"

#if	defined(HAVE_GNUREADLINE)
	#if	defined(HAVE_READLINE_READLINE_H)
	#include <readline/readline.h>
	#endif
	#if	defined(HAVE_READLINE_HISTORY_H)
	#include <readline/history.h>
	#endif
#elif	defined(HAVE_BSDEDITLINE)
	#if	defined(EDITLINE_HAS_READLINE_INTERFACE)
		#if	defined(HAVE_EDITLINE_READLINE_H)
		#include <editline/readline.h>
		#endif
	#endif
	#if	defined(EDITLINE_HASH_HISTEDIT_INTERFACE)
		#if	defined(HAVE_HISTEDIT_H)
		#include <histedit.h>
		#endif
	#endif
#endif	/* HAVE_GNUREADLINE || HAVE_BSDEDITLINE */

#if	defined(READLINE_PROMPT_CONST)
#if	!READLINE_PROMPT_CONST
// your readline header sucks: it is not const-correct
#define RL_CONST_CAST(x)	const_cast<char*>(x)
#else
#define RL_CONST_CAST(x)	x
#endif
#else
#define RL_CONST_CAST(x)	x
#endif

#endif	/* __UTIL_READLINE_H__ */
