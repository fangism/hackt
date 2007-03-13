/**
	\file "util/memory/deallocation_policy_fwd.h"
	Forward declarations of names of deallocation policies.  
	Their definitions appear in "util/memory/deallocation_policy.h".
	$Id: deallocation_policy_fwd.h,v 1.5 2007/03/13 02:37:50 fang Exp $
 */

#ifndef	__UTIL_MEMORY_DEALLOCATION_POLICY_FWD_H__
#define	__UTIL_MEMORY_DEALLOCATION_POLICY_FWD_H__

/**
	Should be 1, but gcc-3.3.x ICEs during attempted substitution.
	So we just disable it, there's no harm.  
 */
#define	UTIL_USE_SFINAE_OPERATOR_DELETE_ARRAY		0

#if UTIL_USE_SFINAE_OPERATOR_DELETE_ARRAY
#include "util/sfinae_types.h"
#endif

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
#if UTIL_USE_SFINAE_OPERATOR_DELETE_ARRAY
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
#endif	// UTIL_USE_SFINAE_OPERATOR_DELETE_ARRAY

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
#if UTIL_USE_SFINAE_OPERATOR_DELETE_ARRAY
		operator_delete_array(t);
#else
		delete [] t;
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

