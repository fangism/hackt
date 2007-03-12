/**
	\file "util/memory/deallocation_policy_fwd.h"
	Forward declarations of names of deallocation policies.  
	Their definitions appear in "util/memory/deallocation_policy.h".
	$Id: deallocation_policy_fwd.h,v 1.4 2007/03/12 07:38:15 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DEALLOCATION_POLICY_FWD_H__
#define	__UTIL_MEMORY_DEALLOCATION_POLICY_FWD_H__

#include "util/sfinae_types.h"
// #include <cassert>		// for assert debugging

namespace util {
namespace memory {
//=============================================================================
/**
	Deallocate something allocated by new, single object allocation.
	This is the default assumed by all pointer classes.  
	\sa delete_array_tag.  
 */
struct delete_tag {
	template <class T>
	inline
	void
	operator () (T* t) const {
		delete t;
	}
};      // end struct delete_tag

//-----------------------------------------------------------------------------
/**
	Uses SFINAE to determine whether or not class has operator delete[].
 */
template <class T>
class has_class_operator_delete_array : public sfinae_types {
	template <void (*PF)(void*)>
	struct wrap_type { };	// what about throw specification?

	template <class U>
	static one __test(wrap_type<&U::operator delete[]>*);

	template <class U>
	static two __test(...);

public:
	enum { value = (sizeof(__test<T>(0)) == 1) } ;
};	// end class has_class_operator_delete_array

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class, bool>
class array_deleter;

template <class T>
struct array_deleter<T, true> {
	inline
	void
	operator () (T* t) {
		T::operator delete [] (t);
	}
};	// end struct array_deleter<T, true>

template <class T>
struct array_deleter<T, false> {
	inline
	void
	operator () (T* t) {
		delete [] t;
	}
};	// end struct array_deleter<T, false>

template <class T>
inline
void
operator_delete_array(T* t) {
	array_deleter<T, has_class_operator_delete_array<T>::value>()(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Deallocate something allocated by new [], object array allocations.
	\sa delete_tag.
 */
struct delete_array_tag {
	template <class T>
	inline
	void
	operator () (T* t) const {
#if 0
		delete [] t;
#else
		operator_delete_array(t);
#endif
	}
};      // end struct delete_array_tag

//=============================================================================
struct free_tag;
typedef	free_tag	malloc_tag;

/**
	Standard FILE I/O streams.  
 */
struct fclose_tag;
typedef	fclose_tag	FILE_tag;

/**
	Process I/O file stream.  
 */
struct pclose_tag;
typedef	pclose_tag	process_tag;

struct iostream_tag;

template <class T, void (*f)(T*)>
struct custom_ptr_fun_tag;

template <class T, void (f)(T*)>
struct custom_fun_ref_tag;

template <class F>
struct custom_functor_tag;

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_DEALLOCATION_POLICY_FWD_H__

