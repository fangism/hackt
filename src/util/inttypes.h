/**
	\file "util/inttypes.h"
	Configured wrapper around standard integer type headers.  
	$Id: inttypes.h,v 1.4 2005/12/08 22:01:13 fang Exp $
 */

#ifndef	__UTIL_INTTYPES_H__
#define	__UTIL_INTTYPES_H__

#include "config.h"

#if defined(HAVE_INTTYPES_H) && HAVE_INTTYPES_H
#include <inttypes.h>	// usually includes <stdint.h>
#endif

#if defined(HAVE_STDINT_H) && HAVE_STDINT_H
#include <stdint.h>
#endif

// yes, in the global namespace

typedef	int8_t			int8;		// usually char
typedef	int16_t			int16;		// usually short
typedef	int32_t			int32;		// usually int
#if SIZEOF_INT64_T
typedef	int64_t			int64;		// usually long long
#endif

// unsigned counterparts
typedef	uint8_t			uint8;
typedef	uint16_t		uint16;
typedef	uint32_t		uint32;
#if SIZEOF_UINT64_T
typedef	uint64_t		uint64;
#endif

/***
	TODO: introduce artificial longer types.  
	TODO: handle GMP mpz_t? (arbitrary)
***/

#endif	// __UTIL_INTTYPES_H__

