/**
	\file "hash_qmap_fwd.h"
	Forward declarations for queryable hash_map, defined in "hash_qmap.h"
	$Id: hash_qmap_fwd.h,v 1.6 2004/12/15 23:31:13 fang Exp $
 */

#ifndef	__HASH_QMAP_FWD_H__
#define	__HASH_QMAP_FWD_H__

#ifndef	HASH_QMAP_NAMESPACE
#define	HASH_QMAP_NAMESPACE		util
#endif

#include "hash_map.h"
// needed for reference to default hash function

// these class parameters are explained below
#define HASH_QMAP_TEMPLATE_SIGNATURE					\
template <class K, class T, class H, class E, class A>

/**
	Namespace for queryable hash map, one with const-semantics lookup.  
 */
namespace HASH_QMAP_NAMESPACE {

// apologies for the short parameter names
template <class K, class T, 
          class H = HASH_MAP_NAMESPACE::hash<K>,
          class E = std::equal_to<K>,
          class A = std::allocator<T> >
class hash_qmap;

}	// end namespace

#endif	//	__HASH_QMAP_FWD_H__

