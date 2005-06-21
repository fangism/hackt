/**
	\file "util/memory/destruction_policy.tcc"
	Implementation of policy-specific explicit destructor invokers.  
	$Id: destruction_policy.tcc,v 1.5 2005/06/21 21:26:40 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DESTRUCTION_POLICY_TCC__
#define	__UTIL_MEMORY_DESTRUCTION_POLICY_TCC__

#include "util/memory/destruction_policy.h"

#ifndef	EXTERN_TEMPLATE_UTIL_MEMORY_DESTRUCTION_POLICY

#include "util/STL/construct_fwd.h"

namespace util {
namespace memory {
USING_CONSTRUCT
USING_DESTROY

//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
template <class T>
inline
void
eager_construct(T* const p, const eager_destruction_tag) {
}

template <class T>
inline
void
eager_construct(T* const p, const lazy_destruction_tag) {
}

template <class T>
inline
void
lazy_construct(T* const p, const eager_destruction_tag) {
	_Construct(p);
}

template <class T>
inline
void
lazy_construct(T* const p, const lazy_destruction_tag) {
}

//-----------------------------------------------------------------------------
}	// end namespace memory
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_MEMORY_DESTRUCTION_POLICY
#endif	// __UTIL_MEMORY_DESTRUCTION_POLICY_TCC__

