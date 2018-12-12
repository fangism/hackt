/**
	\file "util/memory/construct.hh"
	Provide equivalent of std::_Construct and std::_Destroy.
	Don't rely on these being provided by STL in std::.
 */

#ifndef	__UTIL_MEMORY_CONSTRUCT_HH__
#define	__UTIL_MEMORY_CONSTRUCT_HH__

#include <iterator>  // for iterator_traits

#include "config.h"


namespace util {
namespace memory {

template <class _T1>
inline
void
construct(_T1* t) {
  ::new(static_cast<void*>(t)) _T1();
}

/**
	Function for placement new construction. 
 */
template <class _T1, class _T2>
inline
void
construct(_T1* t1, const _T2& t2) {
  ::new(static_cast<void*>(t1)) _T1(t2);
}

/**
	Invoke destructor of referenced object.  
 */
template <class _Tp>
inline
void
destroy(_Tp* p) {
  p->~_Tp();
}


/**
	Invoke destructor over range of objects.  
 */
template <class FwdIter>
inline
void
destroy(FwdIter b, FwdIter e) {
  typedef typename std::iterator_traits<FwdIter>::element_type element_type;
  for ( ; b!=e; ++b) {
    b->~element_type();
  }
}

}	// end namespace memory
}	// end namespace util


#define	USING_CONSTRUCT		using util::memory::construct;
#define	USING_DESTROY		using util::memory::destroy;

#define	FRIEND_STD_CONSTRUCT						\
	template <class _T1>						\
	friend void util::memory::construct(_T1*);
#define	FRIEND_STD_CONSTRUCT2						\
	template <class _T1, class _T2>					\
	friend void util::memory::construct(_T1*, const _T2&);

#endif	// __UTIL_MEMORY_CONSTRUCT_HH__

