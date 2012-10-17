/**
	\file "util/memory/pointer_traits.hh"
	Pointer traits and concepts for pointer classes.  

	$Id: pointer_traits.hh,v 1.10 2007/07/31 23:23:53 fang Exp $
 */

#ifndef	__UTIL_MEMORY_POINTER_TRAITS_H__
#define	__UTIL_MEMORY_POINTER_TRAITS_H__

// #include "util/size_t.h"

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
using util::memory::shared_owner_pointer_tag;				\
using util::memory::exclusive_owner_pointer_tag;			\
using util::memory::never_owner_pointer_tag;


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

/// category for exclusively owner pointers
struct exclusive_owner_pointer_tag : public single_owner_pointer_tag { };

/// category for sometimes-owned pointers
struct sometimes_owner_pointer_tag : public single_owner_pointer_tag { };

/// category for never owned pointers
struct never_owner_pointer_tag : public single_owner_pointer_tag { };

/// pointer classes with this tag have shared ownership, e.g. reference-count
struct shared_owner_pointer_tag : public pointer_class_base_tag { };

// single and shared must be mutually exclusive...
// is there a way to make sure one didn't screw up?

/// for pointers that manage arrays?  would rather not use them...
struct pointer_class_array_tag { };


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
	Handy macro for declaring trait partial specializations, 
	expects ptr_type with 1 template argument.
 */
#define	SPECIALIZE_POINTER_TRAIT(trait, member, ptr_type)		\
template <class T>							\
struct trait<ptr_type<T> > {						\
	typedef	typename ptr_type<T>::member	type;			\
};

/**
	Handy macro for declaring trait partial specializations, 
	expects ptr_type with 2 template arguments.
 */
#define	SPECIALIZE_POINTER_TRAIT_2(trait, member, ptr_type)		\
template <class T, class D>						\
struct trait<ptr_type<T, D> > {						\
	typedef	typename ptr_type<T, D>::member	type;			\
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
	SPECIALIZE_POINTER_TRAIT(pointee, element_type, ptr_type)

#define	SPECIALIZE_POINTEE_TYPE_AS_ELEMENT_TYPE_2(ptr_type)		\
	SPECIALIZE_POINTER_TRAIT_2(pointee, element_type, ptr_type)

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
	SPECIALIZE_POINTER_TRAIT(internal_reference, reference, ptr_type)

#define	SPECIALIZE_INTERNAL_REFERENCE_TYPE_2(ptr_type)			\
	SPECIALIZE_POINTER_TRAIT_2(internal_reference, reference, ptr_type)

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
	SPECIALIZE_POINTER_TRAIT(internal_pointer, pointer, ptr_type)

#define	SPECIALIZE_INTERNAL_POINTER_TYPE_2(ptr_type)			\
	SPECIALIZE_POINTER_TRAIT_2(internal_pointer, pointer, ptr_type)

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
	SPECIALIZE_POINTER_TRAIT(pointer_category, pointer_category, ptr_type)

#define	SPECIALIZE_POINTER_CATEGORY_TYPE_2(ptr_type)			\
	SPECIALIZE_POINTER_TRAIT_2(pointer_category, pointer_category, ptr_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The type of pointer returned for a non-transferring cast.
 */
template <class T>
struct nontransfer_cast;

#define	SPECIALIZE_NONTRANSFER_CAST_TYPE(ptr_type)			\
	SPECIALIZE_POINTER_TRAIT(nontransfer_cast, nontransfer_cast_type, ptr_type)

#define	SPECIALIZE_NONTRANSFER_CAST_TYPE_2(ptr_type)			\
	SPECIALIZE_POINTER_TRAIT_2(nontransfer_cast, nontransfer_cast_type, ptr_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The type of pointer returned for a transferring cast.
 */
template <class T>
struct transfer_cast;

#define	SPECIALIZE_TRANSFER_CAST_TYPE(ptr_type)				\
	SPECIALIZE_POINTER_TRAIT(transfer_cast, transfer_cast_type, ptr_type)

#define	SPECIALIZE_TRANSFER_CAST_TYPE_2(ptr_type)				\
	SPECIALIZE_POINTER_TRAIT_2(transfer_cast, transfer_cast_type, ptr_type)

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

#define	SPECIALIZE_ALL_POINTER_TRAITS_2(ptr_type)			\
	SPECIALIZE_POINTEE_TYPE_AS_ELEMENT_TYPE_2(ptr_type)		\
	SPECIALIZE_INTERNAL_REFERENCE_TYPE_2(ptr_type)			\
	SPECIALIZE_INTERNAL_POINTER_TYPE_2(ptr_type)			\
	SPECIALIZE_POINTER_CATEGORY_TYPE_2(ptr_type)			\
	SPECIALIZE_NONTRANSFER_CAST_TYPE_2(ptr_type)			\
	SPECIALIZE_TRANSFER_CAST_TYPE_2(ptr_type)

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

