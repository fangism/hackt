/**
	\file "map.tcc"
	Class method definitions for std::map clone wrapper class.  
	$Id: map.tcc,v 1.1 2004/12/05 05:07:57 fang Exp $
 */

#ifndef	__UTIL_STL_MAP_TCC__
#define	__UTIL_STL_MAP_TCC__

#include "STL/map.h"

#if !USE_STD_MAP

#if MAP_INHERITANCE
	#define	MAP_CONSTRUCT	map_type
	#define	MAP_PARENT	map_type::
#else
	#define	MAP_CONSTRUCT	the_map
	#define	MAP_PARENT	the_map.
#endif

namespace util {
namespace STL {
//=============================================================================
// class map method definitions

STL_MAP_TEMPLATE_SIGNATURE
map<K,T,C,A>::map() : MAP_CONSTRUCT() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
map<K,T,C,A>::map(const key_compare& __comp, const allocator_type& __a) :
		MAP_CONSTRUCT(__comp, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
map<K,T,C,A>::map(const map_type& __x) : MAP_CONSTRUCT(__x) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
template <typename _InputIterator>
map<K,T,C,A>::map(_InputIterator __first, _InputIterator __last) :
		MAP_CONSTRUCT(__first, __last) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
template <typename _InputIterator>
map<K,T,C,A>::map(_InputIterator __first, _InputIterator __last, 
		const key_compare& __comp, const allocator_type& __a) :
		MAP_CONSTRUCT(__first, __last, __comp, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
map<K,T,C,A>::~map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::map_type&
map<K,T,C,A>::operator = (const map<K,T,C,A>::map_type& __x) {
#if MAP_INHERITANCE
	return MAP_PARENT operator = (__x);
#else
	the_map = __x.the_map;
	return *this;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::mapped_type&
map<K,T,C,A>::operator [] (const key_type& k) {
	return MAP_PARENT operator [] (k);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
pair<typename map<K,T,C,A>::iterator,bool>
map<K,T,C,A>::insert(const value_type& __x) {
	return MAP_PARENT insert(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::iterator
map<K,T,C,A>::insert(iterator position, const value_type& __x) {
	return MAP_PARENT insert(position, __x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
template <typename _InputIterator>
void
map<K,T,C,A>::insert(_InputIterator __first, _InputIterator __last) {
	MAP_PARENT insert(__first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
void
map<K,T,C,A>::erase(iterator __position) {
	MAP_PARENT erase(__position);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::size_type
map<K,T,C,A>::erase(const key_type& __x) {
	return MAP_PARENT erase(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
void
map<K,T,C,A>::erase(iterator __first, iterator __last) {
	MAP_PARENT erase(__first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
void
map<K,T,C,A>::swap(map_type& __x) {
	MAP_PARENT swap(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MAP_INHERITANCE
STL_MAP_TEMPLATE_SIGNATURE
void
map<K,T,C,A>::swap(map& __x) {
	MAP_PARENT swap(__x.the_map);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
void
map<K,T,C,A>::clear(void) {
	MAP_PARENT clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::iterator
map<K,T,C,A>::find(const key_type& __x) {
	return MAP_PARENT find(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::const_iterator
map<K,T,C,A>::find(const key_type& __x) const {
	return MAP_PARENT find(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::size_type
map<K,T,C,A>::count(const key_type& __x) const {
	return MAP_PARENT count(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::iterator
map<K,T,C,A>::lower_bound(const key_type& __x) {
	return MAP_PARENT lower_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::const_iterator
map<K,T,C,A>::lower_bound(const key_type& __x) const {
	return MAP_PARENT lower_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::iterator
map<K,T,C,A>::upper_bound(const key_type& __x) {
	return MAP_PARENT upper_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
typename map<K,T,C,A>::const_iterator
map<K,T,C,A>::upper_bound(const key_type& __x) const {
	return MAP_PARENT upper_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
pair<typename map<K,T,C,A>::iterator,typename map<K,T,C,A>::iterator>
map<K,T,C,A>::equal_range(const key_type& __x) {
	return MAP_PARENT equal_range(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
pair<typename map<K,T,C,A>::const_iterator,
	typename map<K,T,C,A>::const_iterator>
map<K,T,C,A>::equal_range(const key_type& __x) const {
	return MAP_PARENT equal_range(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MAP_INHERITANCE
STL_MAP_TEMPLATE_SIGNATURE
bool
operator == (const map<K,T,C,A>& __m1, const map<K,T,C,A>& __m2) {
	return __m1.the_map == __m2.the_map;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
STL_MAP_TEMPLATE_SIGNATURE
bool
operator < (const map<K,T,C,A>& __m1, const map<K,T,C,A>& __m2) {
	return __m1.the_map < __m2.the_map;
}
#endif

//=============================================================================
}	// end namespace STL
}	// end namespace util

#undef	MAP_INHERITANCE
#undef	MAP_CONSTRUCT
#undef	MAP_PARENT

#endif	// !USE_STD_MAP

#endif	// __UTIL_STL_MAP_TCC__

