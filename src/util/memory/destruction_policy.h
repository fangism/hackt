/**
	\file "memory/destruction_policy.h"
	Allocator destruction policies.  

	$Id: destruction_policy.h,v 1.2 2005/02/27 22:12:01 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DESTRUCTION_POLICY_H__
#define	__UTIL_MEMORY_DESTRUCTION_POLICY_H__

namespace util {
namespace memory {

//=============================================================================
/**
	Tag to dictate when destruction occurs in pool-managed resources.  
 */
struct lazy_destruction_tag { };

/**
	Tag to dictate when destruction occurs in pool-managed resources.  
 */
struct eager_destruction_tag { };

//-----------------------------------------------------------------------------
/**
	Default to eager / early destruction.  
	Is safe to use lazy destruction for non-recursive 
	destructors, i.e. object that do not contain pointers
	to other likewise pooled objects.  
	To override the default eager destruction, 
	define a specialization of this (in the util::memory namespace)
	in the module where the pool is instantiated.  
	A macro is provided below.  
	\param T the class type to be pooled.  
 */
template <class T>
struct list_vector_pool_policy {
	typedef	eager_destruction_tag	destruction_policy;
};	// end struct list_vector_pool_policy

//=============================================================================
template <class T>
static
void
eager_destroy(T* const, const eager_destruction_tag);

template <class T>
static
void
eager_destroy(T* const, const lazy_destruction_tag);

template <class T>
static
void
lazy_destroy(T* const, const eager_destruction_tag);

template <class T>
static
void
lazy_destroy(T* const, const lazy_destruction_tag);

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_DESTRUCTION_POLICY_H__

