/**
	\file "util/memory/destruction_policy.tcc"
	Implementation of policy-specific explicit destructor invokers.  
	$Id: destruction_policy.tcc,v 1.6 2005/09/04 21:15:09 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DESTRUCTION_POLICY_TCC__
#define	__UTIL_MEMORY_DESTRUCTION_POLICY_TCC__

#include "util/memory/destruction_policy.hh"

#ifndef	EXTERN_TEMPLATE_UTIL_MEMORY_DESTRUCTION_POLICY

#include "util/memory/construct.hh"

namespace util {
namespace memory {

//-----------------------------------------------------------------------------
template <class T>
inline
void
eager_destroy(T* const p, const eager_destruction_tag) {
	destroy(p);            // p->~T();
	// construct empty, else will double destruct!
	construct(p);
}

template <class T>
inline
void
eager_destroy(T* const, const lazy_destruction_tag) {
	// do nothing, absolutely nothing!
}

template <class T>
inline
void
lazy_destroy(T* const, const eager_destruction_tag) {
	// do nothing, absolutely nothing!
}

template <class T>
inline
void
lazy_destroy(T* const p, const lazy_destruction_tag) {
	destroy(p);            // p->~T();
	// no need to construct
}

//-----------------------------------------------------------------------------
template <class T>
inline
void
eager_construct(T* const, const eager_destruction_tag) {
}

template <class T>
inline
void
eager_construct(T* const, const lazy_destruction_tag) {
}

template <class T>
inline
void
lazy_construct(T* const p, const eager_destruction_tag) {
	construct(p);
}

template <class T>
inline
void
lazy_construct(T* const, const lazy_destruction_tag) {
}

//-----------------------------------------------------------------------------
}	// end namespace memory
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_MEMORY_DESTRUCTION_POLICY
#endif	// __UTIL_MEMORY_DESTRUCTION_POLICY_TCC__

