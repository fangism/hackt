/**
	\file "memory/destruction_policy.tcc"
	Implementation of policy-specific explicit destructor invokers.  
	$Id: destruction_policy.tcc,v 1.1.2.1 2005/02/27 21:57:09 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DESTRUCTION_POLICY_TCC__
#define	__UTIL_MEMORY_DESTRUCTION_POLICY_TCC__

#include "memory/destruction_policy.h"

namespace util {
namespace memory {
USING_CONSTRUCT
USING_DESTROY

template <class T>
inline
void
eager_destroy(T* const p, const eager_destruction_tag) {
	_Destroy(p);            // p->~T();
	// construct empty, else will double destruct!
	_Construct(p);
}

template <class T>
inline
void
eager_destroy(T* const p, const lazy_destruction_tag) {
	// do nothing, absolutely nothing!
}

template <class T>
inline
void
lazy_destroy(T* const p, const eager_destruction_tag) {
	// do nothing, absolutely nothing!
}

template <class T>
inline
void
lazy_destroy(T* const p, const lazy_destruction_tag) {
	_Destroy(p);            // p->~T();
	// no need to construct
}

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_DESTRUCTION_POLICY_TCC__

