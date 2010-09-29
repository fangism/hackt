/**
	\file "util/reserve.h"
	Utility for optionally reserving space in a sequence container.  
	$Id: reserve.h,v 1.3 2010/09/29 00:13:44 fang Exp $
 */

#ifndef	__UTIL_RESERVE_H__
#define	__UTIL_RESERVE_H__

#include "util/size_t.h"
#include "util/STL/vector_fwd.h"

namespace util {
//=============================================================================
/**
	By default, we assume nothing about the class in question.  
	This suffices for lists, because lists cannot reserve space 
	in advance.  
	Note: since we cannot partially specialize functions, 
	we use this helper class.  
	\param T a sequence container class.  
	\param A the allocator type of T, T::allocator_type.
 */
template <class T, class A>
struct reserver {
	typedef	T			sequence_type;
	/**
		No-op.
	 */
	static
	void
	reserve(const sequence_type&, const size_t) { }
};	// end struct reserver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization to enable reserving memory in advance.  
	This is appropriate for vectors.  
 */
template <class T, class A>
struct reserver<std::vector<T, A>, A> {
	typedef	std::vector<T, A>	sequence_type;
	/**
		Actually reserves memory in advance.  
	 */
	static
	void
	reserve(sequence_type& t, const size_t s) {
		t.reserve(s);
	}
};	// end struct reserver

//-----------------------------------------------------------------------------

/**
	Helper functor to be called for reserving space in advance, 
	if the container allows.  
 */
template <class T>
inline
void
reserve(T& seq, const size_t sz) {
	typedef	typename T::allocator_type	allocator_type;
	typedef	reserver<T, allocator_type>	reserver_type;
	reserver_type::reserve(seq, sz);
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_RESERVE_H__

