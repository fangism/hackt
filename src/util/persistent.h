/**
	\file "persistent.h"
	Base class interface for persistent, serializable objects.  
	$Id: persistent.h,v 1.6 2004/12/11 06:22:44 fang Exp $
 */

#ifndef	__PERSISTENT_H__
#define	__PERSISTENT_H__

#include <iosfwd>
#include <string>

#include <functional>

#include "hash_map.h"

//=============================================================================
// macros

/***
	Standard set of prototypes for persistent object IO-related
	methods.  
	I got sick of typing and pasting them over and over...
	Should be in classes' public sections.  
	Note: this macro should be used only in final concrete classes, 
	because they are all non-virtual.  
	Don't stick a semicolon after this.  
***/

#define	PERSISTENT_METHODS_NO_ALLOC_NO_POINTERS				\
	void write_object(const persistent_object_manager& m) const;	\
	void load_object(persistent_object_manager& m);

#define	PERSISTENT_METHODS_NO_POINTERS					\
	PERSISTENT_METHODS_NO_ALLOC_NO_POINTERS				\
static	persistent* construct_empty(const int);

#define	PERSISTENT_METHODS_NO_ALLOC					\
	PERSISTENT_METHODS_NO_ALLOC_NO_POINTERS				\
	void collect_transient_info(persistent_object_manager& m) const;

#define	PERSISTENT_METHODS						\
	PERSISTENT_METHODS_NO_POINTERS					\
	void collect_transient_info(persistent_object_manager& m) const;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Some classes just need to satisfy the persistent requirements without
	actually implementing them, because no objects of their type will
	actually ever be saved at run-time.  
	This macro supplies default no-op definitions for them.  
 */
#define	PERSISTENT_METHODS_DUMMY_IMPLEMENTATION(T)			\
persistent* T::construct_empty(const int i) { return NULL; }		\
void T::collect_transient_info(persistent_object_manager& m) const { }	\
void T::write_object(const persistent_object_manager& m) const { }	\
void T::load_object(persistent_object_manager& m) { }

//-----------------------------------------------------------------------------
// macros for use in write_object and load_object
// just sanity-check extraneous information, later enable or disable
// with another switch.

// sanity check switch is overrideable by the includer
#ifndef	NO_OBJECT_SANITY
#define	NO_OBJECT_SANITY	0	// default 0, keep sanity checks
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Writes the numeric index of the object, which really isn't
	necessary, but adds a consistency check.  
	This macro can be added inside a write_object method implementation.  
	If used, it should be complemented with STRIP_POINTER_INDEX
	in the load_object counterpart.  
 */
#if NO_OBJECT_SANITY
#define	WRITE_POINTER_INDEX(f, m)
#else
#define	WRITE_POINTER_INDEX(f, m)					\
	write_value(f, m.lookup_ptr_index(this))
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads the object enumerated index for a consistency check.  
 */
#if NO_OBJECT_SANITY
#define	STRIP_POINTER_INDEX(f, m)
#else
#define	STRIP_POINTER_INDEX(f, m) 					\
	{								\
	long index;							\
	read_value(f, index);						\
	if (index != m.lookup_ptr_index(this)) {			\
		long hohum = m.lookup_ptr_index(this);			\
		cerr << "<persistent>::load_object(): " << endl		\
			<< "\tthis = " << this << ", index = " << index	\
			<< ", expected: " << hohum << endl;		\
		assert(index == m.lookup_ptr_index(this));		\
	}								\
	}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sanity check value for end-of-object.
	To be complemented by STRIP_OBJECT_FOOTER.  
 */
#if NO_OBJECT_SANITY
#define	WRITE_OBJECT_FOOTER(f)
#else
#define	WRITE_OBJECT_FOOTER(f)						\
	write_value(f, -1L)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sanity check for object alignment.  
 */
#if NO_OBJECT_SANITY
#define	STRIP_OBJECT_FOOTER(f)
#else
#define	STRIP_OBJECT_FOOTER(f)						\
	{								\
	long neg_one;							\
	read_value(f, neg_one);						\
	assert(neg_one == -1L);						\
	}
#endif

//=============================================================================
// temporary switches

//=============================================================================
/**
	Handy utilities go here.
 */
namespace util {
//=============================================================================
	using std::ostream;
	using std::string;

//=============================================================================
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
 */
typedef	persistent* reconstruction_function_type(const int);

typedef	reconstruction_function_type*	reconstruct_function_ptr_type;

#if 0
// need way of comparing with pointers for null check
typedef	pointer_to_unary_function<persistent*, const int>
						reconstruction_functor;
#endif

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
/** standard default destructor, but virtual */
virtual ~persistent() { }

/** The infamous what-function */
virtual	ostream& what(ostream& o) const = 0;

/** walks object hierarchy and registers reachable pointers with manager */
virtual	void collect_transient_info(persistent_object_manager& m) const = 0;

/** Writes the object out to a managed buffer */
virtual	void write_object(const persistent_object_manager& m) const = 0;

/** Loads the object from a managed buffer */
virtual	void load_object(persistent_object_manager& m) = 0;

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

		size_t get_hash(void) const { return hash; }
		const_iterator begin(void) const { return &key[0]; }
		const_iterator end(void) const { return &key[MAX_LEN]; }

		friend bool operator == (const hash_key&, const hash_key&);
		friend bool operator != (const hash_key&, const hash_key&);
		friend bool operator < (const hash_key&, const hash_key&);
		friend ostream& operator << (ostream&, const hash_key&);

	public:
		static const hash_key		null;
	};	// end class hash_key
};	// end class persistent

}	// end namespace util

//=============================================================================
namespace HASH_MAP_NAMESPACE {
using namespace util;

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

#endif	// __PERSISTENT_H__

