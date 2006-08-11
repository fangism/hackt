/**
	\file "util/utypes.h"
	Conditionally defines the unsigned built-in types with
	convenient aliases in the global namespace. 
	$Id: utypes.h,v 1.1.2.1 2006/08/11 03:17:27 fang Exp $
 */

#ifndef	__UTIL_UTYPES_H__
#define	__UTIL_UTYPES_H__

#include "config.h"

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

#if !SIZEOF_UINT
typedef	unsigned long			ulong;
#endif

/* never use unsigned long long, use uint64, from "util/inttypes.h" */

#endif	/* __UTIL_UTYPES_H__ */

