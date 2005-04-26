/**
	\file "persistent_gcc41death.h"
	Base class interface for persistent, serializable objects.  
	$Id: persistent_gcc41death.h,v 1.1.2.1 2005/04/26 00:13:09 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_GCC41DEATH_H__
#define	__UTIL_PERSISTENT_GCC41DEATH_H__

// #include <iosfwd>
namespace std {
	class ostream;
	class istream;
}

// #include "STL/hash_map_fwd.h"
// #include "nullary_function_fwd.h"
// #include "new_functor_fwd.h"

//=============================================================================
/**
	Handy utilities go here.
 */
namespace util {
	using std::istream;
	using std::ostream;

// forward declarations
	class persistent;
	class persistent_object_manager;

//=============================================================================
// general non-member functions

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
#if 0
	/**
		Type for auxiliary construction argument.  
		Should be small like a char for space-efficiency.
		NOTE: currently construct_empty does NOT follow this...
		This should be fixed for consistency sake.  
	 */
	typedef	char			aux_alloc_arg_type;
#endif
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

};	// end class persistent

//-----------------------------------------------------------------------------
}	// end namespace util

//=============================================================================

#endif	// __UTIL_PERSISTENT_GCC41DEATH_H__

