/**
	\file "util/persistent.h"
	Base class interface for persistent, serializable objects.  
	$Id: persistent.h,v 1.15 2006/05/08 06:12:12 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_H__
#define	__UTIL_PERSISTENT_H__

#include <iosfwd>
#include "util/string_fwd.h"

#include "util/persistent_fwd.h"
#include "util/STL/hash_map_fwd.h"
#include "util/nullary_function_fwd.h"
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
	using std::string;

// forward declarations
	class persistent;
	class persistent_object_manager;

//=============================================================================
// general non-member functions

//=============================================================================
// typedefs

/**
	Allocators for persistent object re-allocation are allowed to
	have one integer argument, in the event that one wishes to 
	use one allocator entry to allocate one of several sub-types.  
	Hint: see notes on multidimensional subtypes.
	In most cases, no argument is necesssary when each entry
	maps to one concrete type.  
	The role of such function is to just allocate, even if it leaves
	object members in an incoherent state.  

	Argument (if applicable) should be aux_alloc_arg_type.
 */
typedef	nullary_function_virtual<persistent*>	reconstruction_function_type;
typedef	const reconstruction_function_type*	reconstruct_function_ptr_type;

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
	typedef	char			aux_alloc_arg_type;
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
	/**
		Implementers of the persistent class, MUST have a 
		unique key with which it can register its type
		and allocator with the persistent object manager.  
		The key should be public, static, and constant.  
		(Static => is not inherited.)
		The point of declaring this type as a class is for 
		consistency: the key needs to be small and of the same 
		size and type because it will be stored in the 
		manager's header for every persistent object instance.  
	 */
	class hash_key {
	public:
		typedef	char*		iterator;
		typedef	const char*	const_iterator;
	public:
		static const size_t	MAX_LEN = 8;
	protected:
		/**
			8 bytes = 2 ints or 1 long-long.  
			Not relying on architecture.  
		 */
		char	key[MAX_LEN];
		/**
			Computed once (upon construction) from the key string.
		 */
		size_t	hash;
	public:
		hash_key();
		hash_key(const char* k);
		hash_key(const string& k);
		// standard destructor

		size_t
		get_hash(void) const { return hash; }

		const_iterator
		begin(void) const { return &key[0]; }

		const_iterator
		end(void) const { return &key[MAX_LEN]; }

#if	defined(HASH_MAP_INTEL_STYLE)
		/**
			Intel-style's hash_compare function needs an
			implicit conversion to size_t.  
			\return precomputed hash index.  
		 */
		operator size_t () const { return hash; }
#endif

		friend bool operator == (const hash_key&, const hash_key&);
		friend bool operator != (const hash_key&, const hash_key&);
		friend bool operator < (const hash_key&, const hash_key&);
		friend ostream& operator << (ostream&, const hash_key&);

	public:
		static const hash_key		null;
	};	// end class hash_key
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
namespace HASH_MAP_NAMESPACE {
using util::persistent;

/**
	Since hash_key::key is not null-terminated, don't use 
	standard char* hash, use precomputed hash member.
 */
template <>
struct hash<persistent::hash_key> {
	size_t
	operator () (const persistent::hash_key& k) const
		{ return k.get_hash(); }
};	// end struct hash

}	// end namespace HASH_MAP_NAMESPACE

//=============================================================================

#endif	// __UTIL_PERSISTENT_H__

