/**
	\file "memory/pointer_manipulator.h"
	Library of pointers for explicit memory management.  
	The pointer classes contained herein are all non-counted.  

	$Id: pointer_manipulator.h,v 1.2 2005/01/28 19:58:52 fang Exp $
 */

#ifndef	__UTIL_MEMORY_POINTER_MANIPULATOR_H__
#define	__UTIL_MEMORY_POINTER_MANIPULATOR_H__

#include "memory/pointer_traits.h"


namespace util {
// forward declaration only
class persistent_object_manager;

namespace memory {
//=============================================================================

/**
	Not a class for objects, but rather, a private namespace
	for functions to be trusted with pointer manipulations.  
 */
class pointer_manipulator {
// declared and defined in "persistent_object_manager.h"
friend class persistent_object_manager;
private:
	/**
		\param T is a raw pointer type.
	 */
	template <class T>
	inline
	static
	const T&
	__get_pointer(const T& p, raw_pointer_tag) {
		return p;
	}

	/**
		\param T is any pointer class type.  
	 */
	template <class T>
	inline
	static
	const typename T::pointer&
	__get_pointer(const T& p, pointer_class_base_tag) {
		return p.base_pointer();
	}

	template <class T>
	inline
	static
	const typename T::pointer&
	get_pointer(const T& p) {
		return __get_pointer(p, __pointer_category(p));
	}

public:
	/**
		\return true if pointers are equal.
	 */
	template <class P1, class P2>
	inline
	static
	bool
	compare_pointers_equal(const P1& p1, const P2& p2) {
#if 0
		// should work, but compiler can't find get_pointer?
//		return get_pointer(p1) == get_pointer(p2);
		return get_pointer<P1>(p1) == get_pointer<P2>(p2);
#else
		return (__get_pointer(p1, __pointer_category(p1)) ==
			__get_pointer(p2, __pointer_category(p2)));
#endif
	}

public:
	/**
		\return true if pointers are unequal.
	 */
	template <class P1, class P2>
	inline
	static
	bool
	compare_pointers_unequal(const P1& p1, const P2& p2) {
#if 0
		// should work, but compiler can't find get_pointer?
//		return get_pointer(p1) != get_pointer(p2);
		return get_pointer<P1>(p1) != get_pointer<P2>(p2);
#else
		return (__get_pointer(p1, __pointer_category(p1)) !=
			__get_pointer(p2, __pointer_category(p2)));
#endif
	}

};	// end class pointer_manipulator

//=============================================================================
}	// end namespace memory
}	// end namespace util

//=============================================================================

#endif	//	__UTIL_MEMORY_POINTER_MANIPULATOR_H__

