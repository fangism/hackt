/**
	\file "util/memory/allocator_adaptor.tcc"
	$Id: allocator_adaptor.tcc,v 1.1 2010/04/05 00:18:47 fang Exp $
 */

#ifndef	__UTIL_MEMORY_ALLOCATOR_ADAPTOR_TCC__
#define	__UTIL_MEMORY_ALLOCATOR_ADAPTOR_TCC__

#include "util/memory/allocator_adaptor.hh"

namespace util {
namespace memory {

// explicitly instantiate the underlying global pool
template <class A>
typename allocator_adaptor<A>::impl_type
allocator_adaptor<A>::__pool;

}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_ALLOCATOR_ADAPTOR_TCC__

