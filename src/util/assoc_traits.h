/**
	\file "assoc_traits.h"
	A traits-class for selecting key or value from associative
	container values such as set, map, hash_map.  

	Incidentally, this is used by "const_assoc_query.h".
	$Id: assoc_traits.h,v 1.1.2.1 2005/02/07 22:53:13 fang Exp $
 */

#ifndef	__UTIL_ASSOC_TRAITS_H__
#define	__UTIL_ASSOC_TRAITS_H__

#include "assoc_traits_fwd.h"
#include "STL/map_fwd.h"
#include "STL/hash_map_fwd.h"
#include "STL/set_fwd.h"
// SGI's hash_set?

//=============================================================================
namespace std {
// selectors (functors) used to select members (foward declarations)

template <class T>
struct _Select1st;

template <class T>
struct _Select2nd;

template <class T>
struct _Identity;

}	// end namespace std
//=============================================================================

namespace util {
// partial specializations here

// STL_MAP_TEMPLATE_SIGNATURE
template <class K, class T, class C, class A>
struct assoc_traits<std::map<K,T,C,A> > {
	typedef	std::map<K,T,C,A>			container_type;
	typedef	typename container_type::key_type	key_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Select1st<value_type>		key_selector;
	typedef	std::_Select2nd<value_type>		value_selector;
};	// end struct assoc_traits

//-----------------------------------------------------------------------------
// STL_HASH_MAP_TEMPLATE_SIGNATURE
template <class K, class T, class H, class E, class A>
struct assoc_traits<HASH_MAP_NAMESPACE::hash_map<K,T,H,E,A> > {
	typedef	HASH_MAP_NAMESPACE::hash_map<K,T,H,E,A>	container_type;
	typedef	typename container_type::key_type	key_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Select1st<value_type>		key_selector;
	typedef	std::_Select2nd<value_type>		value_selector;
};	// end struct assoc_traits

//-----------------------------------------------------------------------------
// STL_SET_TEMPLATE_SIGNATURE
template <class K, class C, class A>
struct assoc_traits<std::set<K,C,A> > {
	typedef	std::set<K,C,A>				container_type;
	typedef	typename container_type::key_type	key_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Identity<value_type>		key_selector;
	typedef	std::_Identity<value_type>		value_selector;
};	// end struct assoc_traits

//=============================================================================
}	// end namespace util

#endif	// __UTIL_ASSOC_TRAITS_H__

