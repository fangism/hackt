/**
	\file "pointer_traits.h"
	Pointer traits and concepts for pointer classes.  

	$Id: pointer_traits.h,v 1.4.10.2 2005/02/06 08:33:41 fang Exp $
 */

#ifndef	__UTIL_MEMORY_POINTER_TRAITS_H__
#define	__UTIL_MEMORY_POINTER_TRAITS_H__

#include "config.h"
#if HAVE_STDDEF_H
#include <cstddef>			// for size_t
#endif

#define	USING_UTIL_MEMORY_POINTER_TRAITS				\
using util::memory::pointee;						\
using util::memory::internal_reference;					\
using util::memory::internal_pointer;					\
using util::memory::indirections;					\
using util::memory::pointer_category;					\
using util::memory::pointer_traits;					\
using util::memory::not_a_pointer_tag;					\
using util::memory::some_kind_of_pointer_tag;				\
using util::memory::raw_pointer_tag;					\
using util::memory::pointer_class_base_tag;				\
using util::memory::single_owner_pointer_tag;				\
using util::memory::shared_owner_pointer_tag;


/***
	For hints on implementation: look at std::iterator_traits.  
	Time to read up on use of compile-time traits in C++...
***/

namespace util {

/***
	The home of pointer_traits will be in the 
	memory sub-namespace because it is used primarily for
	memory-management.
        The classes contained herein offer efficient light-weight
        pointer management with static type-checking.
***/
namespace memory {

//=============================================================================
// tags used to distinguish different classes of pointer-classes
// may need a lattice of orthogonal clasification tags


/// the default for things that are not pointers
struct not_a_pointer_tag { };

/// opposite of not_a_pointer_tag, can't think of a better name
struct some_kind_of_pointer_tag { };

/// tag for raw, naked, bare pointers
struct raw_pointer_tag : public some_kind_of_pointer_tag { };

/// most generic base tag for all pointer classes
struct pointer_class_base_tag : public some_kind_of_pointer_tag { };

/// pointer classes with this tag have one owner per object
struct single_owner_pointer_tag : public pointer_class_base_tag { };

/// pointer classes with this tag have shared ownership, e.g. reference-count
struct shared_owner_pointer_tag : public pointer_class_base_tag { };

// single and shared must be mutually exclusive...
// is there a way to make sure one didn't screw up?

/// for pointers that manage arrays?  would rather not use them...
struct pointer_class_array_tag { };


//=============================================================================
#if 0
// general non-pointer traits
template <class T>
struct raw_pointer_traits {
	/**
		Sounds silly, but this is useful for writing code that
		automatically distinguishes pointers and pointer classes
		from other non-pointer types (built-in or user-defined).  
	 */
	typedef	not_a_pointer_tag	pointer_category;
	static const bool 		is_pointer = false;
	static const size_t		indirections = 0;
};	// end struct raw_pointer_traits

// general pointer traits, partially specialized
template <class T>
struct raw_pointer_traits<T*> {
	typedef	raw_pointer_tag		pointer_category;
	typedef	T			element_type;
	typedef	T&			reference;
	typedef	T*			pointer;

	static const bool		is_pointer = true;
	static const size_t		indirections =
					raw_pointer_traits<T>::indirections +1;
	static const bool		is_void_pointer = false;
};	// end struct raw_pointer_traits

// fully specialized void* pointer traits
template <>
struct raw_pointer_traits<void*> {
	typedef	raw_pointer_tag		pointer_category;
	typedef	void			element_type;
//	can't declare reference to void, duh!
//	typedef	void&			reference;
	typedef	void*			pointer;

	static const bool		is_pointer = true;
	static const size_t		indirections = 1;
	static const bool		is_void_pointer = true;
};	// end struct raw_pointer_traits
#endif

//=============================================================================
/**
	Foundation characteristics of pointer classes.  
	Used primarily for compile-time selection of pointer manipulation
	methods.  
 */
template <class T>
struct pointer_traits;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The number of *'s or raw pointer indirections.
 */
template <class T>
struct indirections {
	enum { value = 0 };
};

template <class T>
struct indirections<T*> {
	enum { value = indirections<T>::value +1 };
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The type pointed to.
 */
template <class T>
struct pointee;

template <class T>
struct pointee<T*> {
	typedef	T					type;
};

#define	SPECIALIZE_POINTEE_TYPE_AS_ELEMENT_TYPE(ptr_type)		\
template <class T>							\
struct pointee<ptr_type<T> > {						\
	typedef	typename ptr_type<T>::element_type	type;		\
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reference to the class pointed to.  
 */
template <class T>
struct internal_reference;

template <class T>
struct internal_reference<T*> {
	typedef	T&					type;
};

template <>
struct internal_reference<void*> {
	// no such thing as void-reference
};

#define	SPECIALIZE_INTERNAL_REFERENCE_TYPE(ptr_type)			\
template <class T>							\
struct internal_reference<ptr_type<T> > {				\
	typedef	typename ptr_type<T>::reference	type;			\
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The internal pointer implementation type.
 */
template <class T>
struct internal_pointer;

// for pointers, it's the self type
template <class T>
struct internal_pointer<T*> {
	typedef	T*					type;
};

#define	SPECIALIZE_INTERNAL_POINTER_TYPE(ptr_type)			\
template <class T>							\
struct internal_pointer<ptr_type<T> > {					\
	typedef	typename ptr_type<T>::pointer		type;		\
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pointer-related type classification.  
 */
template <class T>
struct pointer_category {
	typedef	not_a_pointer_tag			type;
};

template <class T>
struct pointer_category<T*> {
	typedef	raw_pointer_tag				type;
};

#define	SPECIALIZE_POINTER_CATEGORY_TYPE(ptr_type)			\
template <class T>							\
struct pointer_category<ptr_type<T> > {					\
	typedef	typename ptr_type<T>::pointer_category	type;		\
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The type of pointer returned for a non-transferring cast.
 */
template <class T>
struct nontransfer_cast;

#define	SPECIALIZE_NONTRANSFER_CAST_TYPE(ptr_type)			\
template <class T>							\
struct nontransfer_cast<ptr_type<T> > {					\
	typedef	typename ptr_type<T>::nontransfer_cast_type	type;	\
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The type of pointer returned for a transferring cast.
 */
template <class T>
struct transfer_cast;

#define	SPECIALIZE_TRANSFER_CAST_TYPE(ptr_type)				\
template <class T>							\
struct transfer_cast<ptr_type<T> > {					\
	typedef	typename ptr_type<T>::transfer_cast_type	type;	\
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Useful macro for all pointer classes that follow the default
	pointer_traits typedef pattern.  
 */
#define	SPECIALIZE_ALL_POINTER_TRAITS(ptr_type)				\
	SPECIALIZE_POINTEE_TYPE_AS_ELEMENT_TYPE(ptr_type)		\
	SPECIALIZE_INTERNAL_REFERENCE_TYPE(ptr_type)			\
	SPECIALIZE_INTERNAL_POINTER_TYPE(ptr_type)			\
	SPECIALIZE_POINTER_CATEGORY_TYPE(ptr_type)			\
	SPECIALIZE_NONTRANSFER_CAST_TYPE(ptr_type)			\
	SPECIALIZE_TRANSFER_CAST_TYPE(ptr_type)

//-----------------------------------------------------------------------------

/**
	Complete set of pointer traits, using all the above.  
	Only use this on true pointer types, otherwise compiler
	will complain undeclared members of the above traits.  
 */
template <class T>
struct pointer_traits {
	typedef	typename pointee<T>::type		element_type;
	typedef	typename internal_reference<T>::type	reference;
	typedef	typename internal_pointer<T>::type	pointer;
	typedef	typename pointer_category<T>::type	pointer_category;
#if 0
	static const bool			is_raw_pointer = false;


	/**
		Whether or not the pointer is responsible for an
		entire array.  
	 */
	static const bool	is_array = T::is_array;

	/**
		Whether or not the pointer class relies on the 
		referenced object containing information
		about memory management.  
	 */
	static const bool	is_intrusive = T::is_intrusive;

	/**
		Whether or not the pointer object is refrence counted.  
		Counted and non-counted pointers may not interact directly, 
		like oil and water.  
	 */
	static const bool	is_counted = T::is_counted;

	/**
		Whether or not multiple pointers to the same object
		(regardless of ownership) may coexist.  
		Otherwise is unique.
	 */
	static const bool	is_shared = T::is_shared;

	/**
		Ownership characteristics.
		Arguably redundant...
	 */
	static const bool	always_owns = T::always_owns;
	static const bool	sometimes_owns = T::sometimes_owns;
	static const bool	never_owns = T::never_owns;
#endif

	/**
		The default class to return from a non-transferring
		dynamic cast.
		Not every pointer class may allow this.  
	 */
	typedef	typename nontransfer_cast<T>::type
						nontransfer_cast_type;

	/**
		The default class to return from a transferring
		dynamic cast.
	 */
	typedef	typename transfer_cast<T>::type	transfer_cast_type;

};	// end struct pointer_traits

/**
	Specialization for naked pointers.  
 */
template <class T>
struct pointer_traits<T*> {
	typedef	typename pointee<T*>::type		element_type;
	typedef	typename internal_reference<T*>::type	reference;
	typedef	typename internal_pointer<T*>::type	pointer;
	typedef	typename pointer_category<T*>::type	pointer_category;
	// no transfer types...
};	// end struct pointer_traits


// for convenience
template <class P>
inline
typename pointer_category<P>::type
__pointer_category(const P&) {
	return typename pointer_category<P>::type();
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_POINTER_TRAITS_H__

