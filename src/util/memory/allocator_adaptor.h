/**
	\file "util/memory/allocator_adaptor.h"
	Takes an allocator-like class and encapsulates into a
	global member to become suitable substitution for std::allocator.
	$Id: allocator_adaptor.h,v 1.1 2010/04/05 00:18:47 fang Exp $
 */

#ifndef	__UTIL_MEMORY_ALLOCATOR_ADAPTOR_H__
#define	__UTIL_MEMORY_ALLOCATOR_ADAPTOR_H__

#include "util/size_t.h"
#include "util/attributes.h"
#include "util/macros.h"

namespace util {
namespace memory {
//=============================================================================
/**
	Contains a static-global instance of the pool from which to allocate.
 */
template <class A>
class allocator_adaptor {
	typedef	A				impl_type;
	static impl_type			__pool;
	typedef	allocator_adaptor<A>		this_type;
public:
	typedef size_t				size_type;
	typedef ptrdiff_t			difference_type;
	typedef typename impl_type::value_type	value_type;
	typedef value_type*			pointer;
	typedef const value_type*		const_pointer;
	typedef value_type&			reference;
	typedef const value_type&		const_reference;

	/// \param _Tp1 is the value type being allocated by the pool
	template<typename _Tp1>
	struct rebind {
		typedef	typename impl_type::template rebind<_Tp1>::other
						rebound_allocator;
		typedef allocator_adaptor<rebound_allocator>
						other;
	};

	allocator_adaptor() throw() { }

	// no members to copy, default no-throw
	allocator_adaptor(const this_type& __a) throw() { }

	// does not copy underlying pool
	template<typename _Tp1>
	allocator_adaptor(const allocator_adaptor<_Tp1>&) throw() { }

	~allocator_adaptor() throw() { }

	pointer
	address(reference __x) const { return &__x; }

	const_pointer
	address(const_reference __x) const { return &__x; }

	pointer
	allocate(const size_type __n, const void* = 0) __ATTRIBUTE_MALLOC__ {
		INVARIANT(__n == 1);
		return __pool.allocate();
	}

	// __p is not permitted to be a null pointer.
	void
	deallocate(const pointer __p, size_type) {
		__pool.deallocate(__p);
	}

	size_type
	max_size(void) const throw() {
		return size_t(-1) / sizeof(value_type);
	}

	void
	construct(const pointer __p, const_reference __val) const {
		__pool.construct(__p, __val);
	}

	void
	destroy(const pointer __p) const {
		__pool.destroy(__p);
	}

};	// end class allocator_adaptor

template<typename _T1, typename _T2>
inline
bool
operator == (const allocator_adaptor<_T1>&, const allocator_adaptor<_T2>&)
{ return true; }

template<typename _T1, typename _T2>
inline
bool
operator != (const allocator_adaptor<_T1>&, const allocator_adaptor<_T2>&)
{ return false; }

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_ALLOCATOR_ADAPTOR_H__

