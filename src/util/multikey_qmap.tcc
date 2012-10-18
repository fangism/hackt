/**
	\file "util/multikey_qmap.tcc"
	Template method definitions for queryable multikey map.
	Almost entirely copied from multikey_map.tcc.
	$Id: multikey_qmap.tcc,v 1.8 2005/06/21 21:26:37 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_QMAP_TCC__
#define	__UTIL_MULTIKEY_QMAP_TCC__

#include "util/multikey_qmap.hh"

#ifndef	EXTERN_TEMPLATE_UTIL_MULTIKEY_QMAP

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_MULTIKEY_QMAP
#define	EXTERN_TEMPLATE_UTIL_MULTIKEY_MAP
#define	EXTERN_TEMPLATE_UTIL_QMAP
#endif

#include "util/multikey_map.tcc"
#include "util/qmap.tcc"

// thanks to good template design, this file is zero size

#endif	// EXTERN_TEMPLATE_UTIL_MULTIKEY_QMAP
#endif	// __UTIL_MULTIKEY_QMAP_TCC__

