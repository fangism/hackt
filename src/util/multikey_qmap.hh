/**
	\file "util/multikey_qmap.hh"
	Multidimensional queryable map.  
	NOTE: this should really be redefined as a template specialization, 
	not as a child class.
	$Id: multikey_qmap.hh,v 1.14 2006/04/27 00:17:07 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_QMAP_H__
#define	__UTIL_MULTIKEY_QMAP_H__

#if __cplusplus >= 201103L
#error "Do not use this header in C++11 or newer.  Rewrite the code."
#endif

#include "util/multikey_qmap_fwd.hh"
#include "util/qmap.hh"
#include "util/multikey_map.hh"

namespace util {

//=============================================================================
}	// end namespace util

#endif	// __UTIL_MULTIKEY_QMAP_H__

