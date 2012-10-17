/**
	\file "util/utypes.h"
	Conditionally defines the unsigned built-in types with
	convenient aliases in the global namespace. 
	$Id: utypes.h,v 1.2 2006/08/12 00:36:38 fang Exp $
 */

#ifndef	__UTIL_UTYPES_H__
#define	__UTIL_UTYPES_H__

#include "config.h"
/* #include "util/inttypes.h" */

#ifdef	HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#if !SIZEOF_UCHAR
typedef	unsigned char			uchar;
#endif

/* what about wchar and uwchar? */

#if !SIZEOF_USHORT
typedef	unsigned short			ushort;
#endif

#if !SIZEOF_UINT
typedef	unsigned int			uint;
#endif

#if !SIZEOF_ULONG
typedef	unsigned long			ulong;
#endif

#if SIZEOF_LONG_LONG
#if !SIZEOF_ULONGLONG
typedef	unsigned long long		ulonglong;
#endif
#endif

/* never use unsigned long long, use uint64, from "util/inttypes.h" */

#endif	/* __UTIL_UTYPES_H__ */

