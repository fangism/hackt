/**
	\file "pointer_traits.h"
	Pointer traits and concepts for pointer classes.  

	$Id: pointer_traits.h,v 1.2 2004/11/30 01:26:06 fang Exp $
 */

#ifndef	__POINTER_TRAITS_H__
#define	__POINTER_TRAITS_H__

/***
	For hints on implementation: look at std::iterator_traits.  
	Time to read up on use of compile-time traits in C++...
***/

namespace util {

/***
	The home of pointer_traits will be in the 
	memory sub-namespace because it is used primarily for
	memory-management.
***/
namespace memory {

//=============================================================================
// general non-pointer traits
template <class T>
struct raw_pointer_traits {
	// is not a pointer
	static const bool is_pointer = false;
	static const size_t indirections = 0;
};	// end struct raw_pointer_traits

// general pointer traits, partially specialized
template <class T>
struct raw_pointer_traits<T*> {
	static const bool is_pointer = true;
	static const size_t indirections =
		raw_pointer_traits<T>::indirections +1;
	static const bool is_void_pointer = false;
};	// end struct raw_pointer_traits

// fully specialized void* pointer traits
template <>
struct raw_pointer_traits<void*> {
	static const bool is_pointer = true;
	static const size_t indirections = 1;
	static const bool is_void_pointer = true;
};	// end struct raw_pointer_traits

//=============================================================================
// tags used to distinguish different classes of pointer-classes

struct raw_pointer_tag { };

/// most generat base tag for all pointer classes
struct pointer_class_base_tag { };

/// pointer classes with this tag have one owner per object
struct single_owner_pointer_tag : public pointer_class_base_tag { };

/// pointer classes with this tag have shared ownership, e.g. reference-count
struct shared_owner_pointer_tag : public pointer_class_base_tag { };

// single and shared must be mutuall exclusive...

// may need a lattice of orthogonal clasification tags

//=============================================================================
/**
	Foundation characteristics of pointer classes.  
	Used primarily for compile-time selection of pointer manipulation
	methods.  
 */
template <class T>
struct pointer_traits {
	typedef	typename T::element_type		element_type;
	typedef	typename T::reference		reference;
	typedef	typename T::pointer		pointer;

	static const bool is_raw_pointer = false;

	typedef	typename T::pointer_category	pointer_category;

	/**
		Whether or not the pointer is responsible for an
		entire array.  
	 */
	static const bool is_array = T::is_array;

	/**
		Whether or not the pointer class relies on the 
		referenced object containing information
		about memory management.  
	 */
	static const bool is_intrusive = T::is_intrusive;

	/**
		Whether or not the pointer object is refrence counted.  
		Counted and non-counted pointers may not interact directly, 
		like oil and water.  
	 */
	static const bool is_counted = T::is_counted;

	/**
		Whether or not multiple pointers to the same object
		(regardless of ownership) may coexist.  
		Otherwise is unique.
	 */
	static const bool is_shared = T::is_shared;

	/**
		Ownership characteristics.
		Arguably redundant...
	 */
	static const bool always_owns = T::always_owns;
	static const bool sometimes_owns = T::sometimes_owns;
	static const bool never_owns = T::never_owns;

	/**
		The default class to return from a non-transferring
		dynamic cast.
		Not every pointer class may allow this.  
	 */
	typedef	typename T::nontransfer_cast_type	nontransfer_cast_type;

	/**
		The default class to return from a transferring
		dynamic cast.
	 */
	typedef	typename T::transfer_cast_type		transfer_cast_type;

};	// end struct pointer_traits

/**
	Specialization for naked pointers.  
 */
template <class T>
struct pointer_traits<T*> {
	typedef	T				element_type;
	typedef	T&				reference;
	typedef	T*				pointer;

	typedef	raw_pointer_tag			pointer_category;

	static const bool is_raw_pointer = true;
	static const bool is_intrusive = false;
	static const bool is_counted = false;

};	// end struct pointer_traits


// for convenience
template <class P>
inline
typename pointer_traits<P>::pointer_category
__pointer_category(const P&) {
	return typename pointer_traits<P>::pointer_category();
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __POINTER_TRAITS_H__

