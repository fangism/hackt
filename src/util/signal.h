/**
	\file "util/signal.h"
	Configure-wrapped <signal.h> header.  
	$Id: signal.h,v 1.2 2006/04/03 05:30:39 fang Exp $
 */

#ifndef	__UTIL_SIGNAL_H__
#define	__UTIL_SIGNAL_H__

#include "config.h"

#if	defined(__cplusplus) && HAVE_CSIGNAL
#include <csignal>
#elif	defined(HAVE_SIGNAL_H)
#include <signal.h>
#elif	defined(HAVE_SYS_SIGNAL_H)
#include <sys/signal.h>
#else
#error	"I have no idea where to find <signal.h>, please help."
#endif

#endif	/* __UTIL_SIGNAL_H__ */

