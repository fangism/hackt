/**
	\file "util/memory/destruction_policy.hh"
	Allocator destruction policies.  

	$Id: destruction_policy.hh,v 1.4 2005/05/10 04:51:33 fang Exp $
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

//-----------------------------------------------------------------------------
template <class T>
static
void
eager_construct(T* const, const eager_destruction_tag);

template <class T>
static
void
eager_construct(T* const, const lazy_destruction_tag);

template <class T>
static
void
lazy_construct(T* const, const eager_destruction_tag);

template <class T>
static
void
lazy_construct(T* const, const lazy_destruction_tag);

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_DESTRUCTION_POLICY_H__

