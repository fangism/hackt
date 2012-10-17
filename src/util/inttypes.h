/**
	\file "util/inttypes.h"
	Configured wrapper around standard integer type headers.  
	$Id: inttypes.h,v 1.6 2006/04/27 00:16:57 fang Exp $
 */

#ifndef	__UTIL_INTTYPES_H__
#define	__UTIL_INTTYPES_H__

#include "config.h"

#if defined(HAVE_INTTYPES_H) && HAVE_INTTYPES_H
#include <inttypes.h>	/* usually includes <stdint.h> */
#endif

#if defined(HAVE_STDINT_H) && HAVE_STDINT_H
#include <stdint.h>
#endif

/**
	Define to true if we are able to use or define int64.  
	It must also be of integer type that is arithmetically operable.  
 */
#if	(SIZEOF_INT64_T || SIZEOF_INT64)
	#define	HAVE_INT64_TYPE
#elif	defined(QUAD_T_IS_ARITHMETIC)
	#if	(SIZEOF_QUAD_T == 8 && QUAD_T_IS_ARITHMETIC)
	#define	HAVE_INT64_TYPE
	#endif
/* else leave undefined */
#endif

/**
	Define to true if we are able to use or define uint64.  
	It must also be of integer type that is arithmetically operable.  
 */
#if	(SIZEOF_UINT64_T || SIZEOF_UINT64)
	#define	HAVE_UINT64_TYPE
#elif	defined(U_QUAD_T_IS_ARITHMETIC)
	#if	(SIZEOF_U_QUAD_T == 8 && U_QUAD_T_IS_ARITHMETIC)
	#define	HAVE_UINT64_TYPE
	#endif
/* else leave undefined */
#endif

/* yes, in the global namespace */

typedef	int8_t			int8;		/* usually char */
typedef	int16_t			int16;		/* usually short */
typedef	int32_t			int32;		/* usually int */
#if !SIZEOF_INT64
#if SIZEOF_INT64_T
typedef	int64_t			int64;		/* usually long long */
#elif	(SIZEOF_QUAD_T == 8 && QUAD_T_IS_ARITHMETIC)
typedef	quad_t			int64;
#endif
#endif

/* unsigned counterparts */
typedef	uint8_t			uint8;
typedef	uint16_t		uint16;
typedef	uint32_t		uint32;
#if !SIZEOF_UINT64
#if SIZEOF_UINT64_T
typedef	uint64_t		uint64;
#elif	(SIZEOF_U_QUAD_T == 8 && U_QUAD_T_IS_ARITHMETIC)
typedef	u_quad_t		uint64;
#endif
#endif

/***
	TODO: introduce artificial longer types.  
	TODO: handle GMP mpz_t? (arbitrary)
***/

#endif	/* __UTIL_INTTYPES_H__ */

