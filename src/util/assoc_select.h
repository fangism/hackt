/**
	\file "assoc_select.h"
	A traits-class for selecting key or value from associative
	container values such as set, map, hash_map.  

	Incidentally, this is used by "const_assoc_query.h".
	$Id: assoc_select.h,v 1.1.2.1 2005/02/06 02:22:09 fang Exp $
 */

#ifndef	__UTIL_ASSOC_SELECT_H__
#define	__UTIL_ASSOC_SELECT_H__

#include "assoc_select_fwd.h"
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
struct select_value<std::map<K,T,C,A> > {
	typedef	std::map<K,T,C,A>			container_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Select2nd<value_type>		selector;
};	// end struct select_value

//-----------------------------------------------------------------------------
// STL_HASH_MAP_TEMPLATE_SIGNATURE
template <class K, class T, class H, class E, class A>
struct select_value<HASH_MAP_NAMESPACE::hash_map<K,T,H,E,A> > {
	typedef	HASH_MAP_NAMESPACE::hash_map<K,T,H,E,A>	container_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Select2nd<value_type>		selector;
};	// end struct select_value

//-----------------------------------------------------------------------------
// STL_SET_TEMPLATE_SIGNATURE
template <class K, class C, class A>
struct select_value<std::set<K,C,A> > {
	typedef	std::set<K,C,A>				container_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Identity<value_type>		selector;
};	// end struct select_value

//=============================================================================
// STL_MAP_TEMPLATE_SIGNATURE
template <class K, class T, class C, class A>
struct select_key<std::map<K,T,C,A> > {
	typedef	std::map<K,T,C,A>			container_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Select1st<value_type>		selector;
};	// end struct select_key

//-----------------------------------------------------------------------------
// STL_HASH_MAP_TEMPLATE_SIGNATURE
template <class K, class T, class H, class E, class A>
struct select_key<HASH_MAP_NAMESPACE::hash_map<K,T,H,E,A> > {
	typedef	HASH_MAP_NAMESPACE::hash_map<K,T,H,E,A>	container_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Select1st<value_type>		selector;
};	// end struct select_key

//-----------------------------------------------------------------------------
// STL_SET_TEMPLATE_SIGNATURE
template <class K, class C, class A>
struct select_key<std::set<K,C,A> > {
	typedef	std::set<K,C,A>				container_type;
	typedef	typename container_type::value_type	value_type;
	typedef	std::_Identity<value_type>		selector;
};	// end struct select_key

//=============================================================================
}	// end namespace util

#endif	// __UTIL_ASSOC_SELECT_H__

