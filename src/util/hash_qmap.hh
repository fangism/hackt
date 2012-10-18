/**
	\file "util/hash_qmap.hh"
	Queryable hash_map that doesn't add empty elements on lookup.  
	$Id: hash_qmap.hh,v 1.10 2006/04/27 00:16:53 fang Exp $
 */

#ifndef	__UTIL_HASH_QMAP_H__
#define	__UTIL_HASH_QMAP_H__

#include "util/macros.h"
#include "util/STL/hash_map.hh"
#include "util/hash_qmap_fwd.hh"		// forward declarations only
#include "util/const_assoc_query.hh"

namespace util {
using std::pair;

//-----------------------------------------------------------------------------
/**
	Extension of Standard Template Library's map container.  
	Adds an lookup operator with constant semantics for querying
	whether or not something is in the map, but without modifying it.  
	Useful for maps of pointers and pointer classes.  
	\param K the key type.
	\param T the mapped data type.
	\param H the hashing function.
	\param E the equal_to comparator function.
	\param A the allocator.  
 */
HASH_QMAP_TEMPLATE_SIGNATURE
class hash_qmap :
	public const_assoc_query<HASH_MAP_NAMESPACE::HASH_MAP_CLASS > {
	// EVERYTHING inherited
};

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	//	__UTIL_HASH_QMAP_H__

