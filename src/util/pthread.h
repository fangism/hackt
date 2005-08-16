/**
	\file "util/pthread.h"
	Auto-configured <pthread.h> wrapper.
	$Id: pthread.h,v 1.1.2.1 2005/08/16 03:50:22 fang Exp $
 */

#ifndef	__UTIL_PTHREAD_H__
#define	__UTIL_PTHREAD_H__

#include "config.h"

#if defined(HAVE_PTHREAD_H) && HAVE_PTHREAD_H
#include <pthread.h>
#endif

#endif	// __UTIL_PTHREAD_H__

