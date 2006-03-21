/**
	\file "util/STL/hash_map_utils.h"
	This file contains some external utility functions related
	to hash_maps.  
	$Id: hash_map_utils.h,v 1.1 2006/03/21 21:53:13 fang Exp $
 */

#ifndef	__UTIL_STL_HASH_MAP_UTILS_H__
#define	__UTIL_STL_HASH_MAP_UTILS_H__

#include "util/STL/hash_map.h"
#include "util/hash_qmap.h"

namespace HASH_MAP_NAMESPACE {
//=============================================================================
/**
	Slow sequential copying of from source hash_map to destination, 
	which effectively reverses the order within each bucket.
 */
HASH_MAP_TEMPLATE_SIGNATURE
void
hash_map_copy_reverse_buckets(const HASH_MAP_CLASS& s, HASH_MAP_CLASS& d) {
	typedef	typename HASH_MAP_CLASS::const_iterator	const_iterator;
	d.clear();
	d.resize(s.bucket_count());
	const const_iterator e(s.end());
	const_iterator i(s.begin());
	for ( ; i!=e; ++i) {
		d[i->first] = i->second;
	}
}

//=============================================================================
/**
	Generic assignment copying of non-hash_maps.  
 */
template <class H>
struct copy_map_reverse_bucket_t {
	void
	operator () (const H& s, H& d) const { d = s; }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization for hash_maps.  
 */
HASH_MAP_TEMPLATE_SIGNATURE
struct copy_map_reverse_bucket_t<HASH_MAP_CLASS > {
	typedef	HASH_MAP_CLASS			argument_type;
	void
	operator () (const argument_type& s, argument_type& d) const {
		hash_map_copy_reverse_buckets(s, d);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization for hash_qmaps.  
 */
HASH_QMAP_TEMPLATE_SIGNATURE
struct copy_map_reverse_bucket_t<util::HASH_QMAP_CLASS > {
	typedef	util::HASH_QMAP_CLASS		argument_type;
	void
	operator () (const argument_type& s, argument_type& d) const {
		hash_map_copy_reverse_buckets(s, d);
	}
};

//-----------------------------------------------------------------------------
/**
	Helper function will select the correct implementation.  
	This is useful where T can be switched back and forth between
	a map and a hash_map (or other unordered map).  
 */
template <class T>
inline
void
copy_map_reverse_bucket(const T& s, T& d) {
	copy_map_reverse_bucket_t<T>()(s, d);
}

//=============================================================================
}	// end namespace HASH_MAP_NAMESPACE

#endif	// __UTIL_STL_HASH_MAP_UTILS_H__

