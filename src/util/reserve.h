/**
	\file "util/reserve.h"
	Utility for optionally reserving space in a sequence container.  
	$Id: reserve.h,v 1.1.2.1 2005/07/05 21:02:21 fang Exp $
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
 */
template <class T>
struct reserver {
	typedef	T			sequence_type;
	/**
		No-op.
	 */
	void
	operator () (T&, const size_t) { }
};	// end struct reserver

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization to enable reserving memory in advance.  
	This is appropriate for vectors.  
 */
template <class T, class A>
struct reserver<std::vector<T, A> > {
	typedef	std::vector<T, A>	sequence_type;
	/**
		Actually reserves memory in advance.  
	 */
	void
	operator () (T& t, const size_t s) {
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
	reserver<T>()(seq, sz);
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_RESERVE_H__

