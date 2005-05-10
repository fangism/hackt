/**
	\file "util/assoc_traits_fwd.h"
	Forward declarations for traits-class for 
	selecting key or value from associative
	container values such as set, map, hash_map.  

	$Id: assoc_traits_fwd.h,v 1.3 2005/05/10 04:51:22 fang Exp $
 */

#ifndef	__UTIL_ASSOC_TRAITS_FWD_H__
#define	__UTIL_ASSOC_TRAITS_FWD_H__

namespace util {
//=============================================================================
/**
	Selects the key/value from the value_type of the container type's 
	implementation.
	Also contains a few other useful traits in typedefs.  
	This is required, because access to the internals of stl_tree's
	_Rb_tree are protected.
	For every associative container you create that needs this trait, 
	you'll need to define a partial specialization.  
	No default definition is provided intentionally.  
	We provide partial specializations here for STL containers.  

	Specializations should define a typedef selector, referring to 
	one of the above standard slectors, or similar type.  
  */
template <class M>
struct assoc_traits;

//=============================================================================
}	// end namespace util

#endif	// __UTIL_ASSOC_TRAITS_FWD_H__

