/**
	\file "util/assoc_traits.hh"
	A traits-class for selecting key or value from associative
	container values such as set, map, hash_map.  

	Incidentally, this is used by "const_assoc_query.hh".
	$Id: assoc_traits.hh,v 1.5 2006/05/06 04:18:56 fang Exp $
 */

#ifndef	__UTIL_ASSOC_TRAITS_H__
#define	__UTIL_ASSOC_TRAITS_H__

#include "util/assoc_traits_fwd.hh"
#include "util/STL/map_fwd.hh"
#include "util/STL/hash_map_fwd.hh"
#include "util/STL/set_fwd.hh"
// SGI's hash_set?

//=============================================================================
BEGIN_NAMESPACE_STD
// selectors (functors) used to select members (foward declarations)

template <class T>
struct _Select1st;

template <class T>
struct _Select2nd;

template <class T>
struct _Identity;

END_NAMESPACE_STD
//=============================================================================

namespace util {
// partial specializations here

// STL_MAP_TEMPLATE_SIGNATURE
STD_MAP_TEMPLATE_SIGNATURE
struct assoc_traits<STD_MAP_CLASS > {
	typedef	STD_MAP_CLASS				container_type;
	typedef	typename container_type::key_type	key_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Select1st<value_type>		key_selector;
	typedef	std::_Select2nd<value_type>		value_selector;
};	// end struct assoc_traits

//-----------------------------------------------------------------------------
// STL_HASH_MAP_TEMPLATE_SIGNATURE
HASH_MAP_TEMPLATE_SIGNATURE
struct assoc_traits<HASH_MAP_NAMESPACE::HASH_MAP_CLASS > {
	typedef	HASH_MAP_NAMESPACE::HASH_MAP_CLASS	container_type;
	typedef	typename container_type::key_type	key_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Select1st<value_type>		key_selector;
	typedef	std::_Select2nd<value_type>		value_selector;
};	// end struct assoc_traits

//-----------------------------------------------------------------------------
// STL_SET_TEMPLATE_SIGNATURE
STD_SET_TEMPLATE_SIGNATURE
struct assoc_traits<STD_SET_CLASS > {
	typedef	STD_SET_CLASS				container_type;
	typedef	typename container_type::key_type	key_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Identity<value_type>		key_selector;
	typedef	std::_Identity<value_type>		value_selector;
};	// end struct assoc_traits

//=============================================================================
#if 0
ASHAMED TO EVEN ADMIT THIS EVER EXISTED :)
/**
	Really dirty traits hack to get the virtual key/value components
	of a set's value_type, which is the same as set<>::key_type.
	The constraint on V is that it has key_type and value_type defined.  
	(This is used for multikey_set_element, which meets this requirement.)
	This effectively distinguishes mapped type from key type of 
	set elements.  
	(Set didn't have a mapped type...)
 */
template <class V>
struct set_element_traits {
	typedef	typename V::key_type		key_type;
	typedef	typename V::value_type		mapped_type;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization intended to catch the case for maps' value_types, 
	which are invariably implemented as std::pair<const key, value>.  
 */
template <class K, class T>
struct set_element_traits<std::pair<const K, T> > {
	typedef	std::pair<const K, T>		pair_type;
	typedef	typename const_traits<
		typename pair_type::first_type>::non_const_type
						key_type;
	typedef	typename pair_type::second_type	mapped_type;
};
#endif

//=============================================================================
}	// end namespace util

#endif	// __UTIL_ASSOC_TRAITS_H__

