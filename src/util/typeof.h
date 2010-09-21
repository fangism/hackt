/**
	\file "util/typeof.h"
	Hopefully-portable wrapper to use typeof() operator.
	$Id: typeof.h,v 1.2 2010/09/21 00:18:42 fang Exp $
 */
#ifndef	__UTIL_TYPEOF_H__
#define	__UTIL_TYPEOF_H__

#include "config.h"

/* defined by AC_C_TYPEOF */
#if	defined(HAVE_TYPEOF)
#define	TYPEOF				__typeof__
/* defined by FANG_CXX_DECLTYPE */
#elif	defined(HAVE_DECLTYPE)
#define	TYPEOF				decltype
#else
#error	"Unable to find support for a typeof-like opeartor."
#endif

#endif	// __UTIL_TYPEOF_H__
