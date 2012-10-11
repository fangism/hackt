/**
	\file "util/persistent.h"
	Base class interface for persistent, serializable objects.  
	$Id: persistent.h,v 1.16 2007/07/31 23:23:43 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_H__
#define	__UTIL_PERSISTENT_H__

#include <iosfwd>
#include "util/persistent_fwd.h"
#include "util/new_functor_fwd.h"

//=============================================================================
// macros

/**
	Some classes just need to satisfy the persistent requirements without
	actually implementing them, because no objects of their type will
	actually ever be saved at run-time.  
	This macro supplies default no-op definitions for them.  
 */
#define	PERSISTENT_METHODS_DUMMY_IMPLEMENTATION(T)			\
void									\
T::collect_transient_info(persistent_object_manager&) const { }		\
void									\
T::write_object(const persistent_object_manager&, ostream&) const { }	\
void									\
T::load_object(const persistent_object_manager&, istream&) { }

/**
	Default implementation of ostream& what(ostream&) const 
	member function, which need not be derived from persistent.  
	Requires inclusion of "util/what.h".
 */
#define	PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(T)			\
std::ostream&								\
T::what(std::ostream& o) const {					\
	return o << util::what<T >::name();				\
}

//-----------------------------------------------------------------------------
// macros for use in write_object and load_object, 
// have been relocated to persistent_object_manager.cc (2005-02-01)

//=============================================================================
/**
	Handy utilities go here.
 */
namespace util {
using std::istream;
using std::ostream;

//=============================================================================
/**
	Interface prerequisites for persistent, serializable objects.  
	This class works closely with the persistent_object_manager class.  

	Concept requirement: allocation
	In addition to implementing the pure virtual functions, 
	there also needs to be a function (may be static) that returns
	an allocated persistent object; cannot be a method because
	object doesn't exist yet -- don't know what type.  
	The allocator should return a pointer to this persistent base type.  

	
 */
class persistent {
public:
	/**
		Type for auxiliary construction argument.  
		Should be small like a char for space-efficiency.
		NOTE: currently construct_empty does NOT follow this...
		This should be fixed for consistency sake.  
	 */
	typedef	unsigned char			aux_alloc_arg_type;
public:
/** standard default destructor, but virtual */
virtual ~persistent() { }

/** The infamous what-function */
virtual	ostream&
	what(ostream& o) const = 0;

/** walks object hierarchy and registers reachable pointers with manager */
virtual	void
	collect_transient_info(persistent_object_manager& m) const = 0;

/** Writes the object out to a managed buffer */
virtual	void
	write_object(const persistent_object_manager& m, ostream& o) const = 0;

/** Loads the object from a managed buffer */
virtual	void
	load_object(const persistent_object_manager& m, istream& i) = 0;

public:
	/**
		General purpose flag for printing or suppressing
		warning messages about unimplemented or partially
		implemented interface functions.  
	 */
	static bool			warn_unimplemented;

public:
	class hash_key;
};	// end class persistent

//-----------------------------------------------------------------------------
/**
	This macro is only effective in the util namespace!
 */
#define	SPECIALIZE_PERSISTENT_TRAITS_DECLARATION(T)			\
template <>								\
struct persistent_traits<T> {						\
	typedef	persistent_traits<T>		this_type;		\
	typedef	T				type;			\
	static const persistent::hash_key	type_key;		\
	static const persistent::aux_alloc_arg_type			\
						sub_index;		\
	static const int			type_id;		\
	static								\
	persistent*							\
	construct_empty(void);						\
									\
	static const new_functor<T,persistent>	empty_constructor;	\
};	// end struct persistent_traits (specialized)

/**
	This macro is only effective in the util namespace!
 */
#define	SPECIALIZE_PERSISTENT_TRAITS_INITIALIZATION(T, key, index)	\
const persistent::hash_key						\
persistent_traits<T>::type_key(key);					\
const persistent::aux_alloc_arg_type					\
persistent_traits<T>::sub_index = index;				\
persistent*								\
persistent_traits<T>::construct_empty(void) {				\
	return new T();							\
}									\
const new_functor<T,persistent>						\
persistent_traits<T>::empty_constructor;				\
const int								\
persistent_traits<T>::type_id =						\
	persistent_object_manager::register_persistent_type<T>();


#define	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(T, key, index)	\
	SPECIALIZE_PERSISTENT_TRAITS_DECLARATION(T)			\
	SPECIALIZE_PERSISTENT_TRAITS_INITIALIZATION(T, key, index)

//-----------------------------------------------------------------------------
}	// end namespace util

//=============================================================================

#endif	// __UTIL_PERSISTENT_H__

