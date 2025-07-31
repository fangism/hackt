/**
	\file "util/STL/hash_map_utils.hh"
	This file contains some external utility functions related
	to hash_maps.  
	$Id: hash_map_utils.hh,v 1.4 2008/11/25 21:52:47 fang Exp $
 */

#ifndef	__UTIL_STL_HASH_MAP_UTILS_H__
#define	__UTIL_STL_HASH_MAP_UTILS_H__

#include <unordered_map>

namespace util {
//=============================================================================
// just declaration
template <typename M>
void
hash_map_copy_reverse_buckets(const M&, M&);

/**
	Slow sequential copying of from source hash_map to destination, 
	which effectively reverses the order within each bucket.
 */
template <typename M>
void
hash_map_copy_reverse_buckets(const M& s, M& d) {
	typedef	typename M::const_iterator	const_iterator;
	d.clear();
	d.rehash(s.bucket_count());
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
template <class K, class T, class H, class E, class A>
struct copy_map_reverse_bucket_t<std::unordered_map<K,T,H,E,A> > {
	typedef	std::unordered_map<K,T,H,E,A>		argument_type;
	void
	operator () (const argument_type& s, argument_type& d) const {
		hash_map_copy_reverse_buckets(s, d);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Specialization for hash_qmaps.  
 */
template <class K, class T, class H, class E, class A>
struct copy_map_reverse_bucket_t<util::hash_qmap<K,T,H,E,A> > {
	typedef	util::hash_qmap<K,T,H,E,A>		argument_type;
	void
	operator () (const argument_type& s, argument_type& d) const {
		hash_map_copy_reverse_buckets(s, d);
	}
};
#endif

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
}  // end namespace util

#endif	// __UTIL_STL_HASH_MAP_UTILS_H__

