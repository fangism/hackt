/**
	\file "hash_qmap_fwd.h"
	Forward declarations for queryable hash_map, defined in "hash_qmap.h"
	$Id: hash_qmap_fwd.h,v 1.5 2004/11/02 07:52:14 fang Exp $
 */

#ifndef	__HASH_QMAP_FWD_H__
#define	__HASH_QMAP_FWD_H__

#ifndef	HASH_QMAP_NAMESPACE
#define	HASH_QMAP_NAMESPACE		hash_qmap_namespace
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
using namespace HASH_MAP_NAMESPACE;
using std::equal_to;
using std::allocator;

// apologies for the short parameter names
template <class K, class T, 
          class H = HASH_MAP_NAMESPACE::hash<K>,
          class E = equal_to<K>,
          class A = allocator<T> >
class hash_qmap;

}	// end namespace

#endif	//	__HASH_QMAP_FWD_H__

