/**
	\file "assoc_select_fwd.h"
	Forward declarations for traits-class for 
	selecting key or value from associative
	container values such as set, map, hash_map.  

	$Id: assoc_select_fwd.h,v 1.1.2.1 2005/02/06 02:22:09 fang Exp $
 */

#ifndef	__UTIL_ASSOC_SELECT_FWD_H__
#define	__UTIL_ASSOC_SELECT_FWD_H__

namespace util {
//=============================================================================
/**
	Selects the value from the value_type of the container type's 
	implementation.
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
struct select_value;

//-----------------------------------------------------------------------------
/**
	Selects the key from the value_type of the container type's 
	implementation.
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
struct select_key;

//=============================================================================
}	// end namespace util

#endif	// __UTIL_ASSOC_SELECT_FWD_H__

