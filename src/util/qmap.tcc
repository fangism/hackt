/**
	\file "qmap.tcc"
	Template class method definitions for queryable map.  
	$Id: qmap.tcc,v 1.2.16.2 2005/02/17 00:10:22 fang Exp $
 */

#ifndef	__UTIL_QMAP_TCC__
#define	__UTIL_QMAP_TCC__

#include "qmap.h"
#include "STL/map.tcc"
#include "const_assoc_query.tcc"

namespace util {
/**
	Purges map of entries that are just default values, 
	useful for removing null pointers for maps of pointers
	(where semantically appropriate).  
	Requirement: T must be comparable for equality.  
 */
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T,C,A>::clean(void) {
	const mapped_type def = null_construct<T>();  // default value
	iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; ) {
		if (i->second == def) {
			iterator j = i;
			j++;
			this->erase(i);
			i = j;
		} else {
			i++;
		}
	}
}

#if 0
OBSOLETE
//=============================================================================
// class qmap method definitions

QMAP_TEMPLATE_SIGNATURE
qmap<K,T,C,A>::qmap() : the_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
qmap<K,T,C,A>::qmap(const key_compare& __comp, const allocator_type& __a) :
		the_map(__comp, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
qmap<K,T,C,A>::qmap(const qmap& __x) : the_map(__x.the_map) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
template <typename _InputIterator>
qmap<K,T,C,A>::qmap(_InputIterator __first, _InputIterator __last) :
		the_map(__first, __last) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
template <typename _InputIterator>
qmap<K,T,C,A>::qmap(_InputIterator __first, _InputIterator __last, 
		const key_compare& __comp, const allocator_type& __a) :
		the_map(__first, __last, __comp, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
qmap<K,T,C,A>::~qmap() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
qmap<K,T,C,A>&
qmap<K,T,C,A>::operator = (const qmap<K,T,C,A>& __x) {
	the_map = __x.the_map;
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::mapped_type&
qmap<K,T,C,A>::operator [] (const key_type& k) {
	return the_map[k];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::mapped_type
qmap<K,T,C,A>::operator [] (const key_type& k) const {
	const_iterator i = the_map.find(k);     // uses find() const;
	return (i != the_map.end()) ? i->second : mapped_type();
	// if T is a pointer class, should be equivalent to NULL
	// or whatever the default constructor is
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Purges map of entries that are just default values, 
	useful for removing null pointers for maps of pointers
	(where semantically appropriate).  
	Requirement: T must be comparable for equality.  
 */
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T,C,A>::clean(void) {
	const mapped_type def = null_construct<T>();  // default value
#if 0
	// won't work because of pair<> b.s.
	remove_if(index_map.begin(), index_map.end(),
		unary_compose(     
			bind2nd(equal_to<T>(), def),       
			_Select2nd<typename map_type::value_type>()
			// some error involving operator =
			// with const first type.
		)
	);
#else
	iterator i = the_map.begin();
	const const_iterator e = the_map.end();
	for ( ; i!=e; ) {
		if (i->second == def) {
			iterator j = i;
			j++;
			the_map.erase(i);
			i = j;
		} else {
			i++;
		}
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
pair<typename qmap<K,T,C,A>::iterator,bool>
qmap<K,T,C,A>::insert(const value_type& __x) {
	return the_map.insert(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::iterator
qmap<K,T,C,A>::insert(iterator position, const value_type& __x) {
	return the_map.insert(position, __x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
template <typename _InputIterator>
void
qmap<K,T,C,A>::insert(_InputIterator __first, _InputIterator __last) {
	the_map.insert(__first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T,C,A>::erase(iterator __position) {
	the_map.erase(__position);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::size_type
qmap<K,T,C,A>::erase(const key_type& __x) {
	return the_map.erase(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T,C,A>::erase(iterator __first, iterator __last) {
	the_map.erase(__first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T,C,A>::swap(qmap& __x) {
	the_map.swap(__x.the_map);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T,C,A>::clear(void) {
	the_map.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::iterator
qmap<K,T,C,A>::find(const key_type& __x) {
	return the_map.find(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::const_iterator
qmap<K,T,C,A>::find(const key_type& __x) const {
	return the_map.find(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::size_type
qmap<K,T,C,A>::count(const key_type& __x) const {
	return the_map.count(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::iterator
qmap<K,T,C,A>::lower_bound(const key_type& __x) {
	return the_map.lower_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::const_iterator
qmap<K,T,C,A>::lower_bound(const key_type& __x) const {
	return the_map.lower_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::iterator
qmap<K,T,C,A>::upper_bound(const key_type& __x) {
	return the_map.upper_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T,C,A>::const_iterator
qmap<K,T,C,A>::upper_bound(const key_type& __x) const {
	return the_map.upper_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
pair<typename qmap<K,T,C,A>::iterator,typename qmap<K,T,C,A>::iterator>
qmap<K,T,C,A>::equal_range(const key_type& __x) {
	return the_map.equal_range(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
pair<typename qmap<K,T,C,A>::const_iterator,
	typename qmap<K,T,C,A>::const_iterator>
qmap<K,T,C,A>::equal_range(const key_type& __x) const {
	return the_map.equal_range(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
bool
operator == (const qmap<K,T,C,A>& __m1, const qmap<K,T,C,A>& __m2) {
	return __m1.the_map == __m2.the_map;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
bool
operator < (const qmap<K,T,C,A>& __m1, const qmap<K,T,C,A>& __m2) {
	return __m1.the_map < __m2.the_map;
}

//=============================================================================
// class qmap method definitions (specialized)

QMAP_TEMPLATE_SIGNATURE
qmap<K,T*,C,A>::qmap() : the_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
qmap<K,T*,C,A>::qmap(const key_compare& __comp, const allocator_type& __a) :
		the_map(__comp, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
qmap<K,T*,C,A>::qmap(const qmap& __x) : the_map(__x.the_map) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
template <typename _InputIterator>
qmap<K,T*,C,A>::qmap(_InputIterator __first, _InputIterator __last) :
		the_map(__first, __last) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
template <typename _InputIterator>
qmap<K,T*,C,A>::qmap(_InputIterator __first, _InputIterator __last, 
		const key_compare& __comp, const allocator_type& __a) :
		the_map(__first, __last, __comp, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
qmap<K,T*,C,A>::~qmap() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
qmap<K,T*,C,A>&
qmap<K,T*,C,A>::operator = (const qmap<K,T*,C,A>& __x) {
	the_map = __x.the_map;
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::mapped_type&
qmap<K,T*,C,A>::operator [] (const key_type& k) {
	return the_map[k];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::mapped_type
qmap<K,T*,C,A>::operator [] (const key_type& k) const {
	const_iterator i = find(k);     // uses find() const;
	return (i != the_map.end()) ? i->second : mapped_type();
	// if T is a pointer class, should be equivalent to NULL
	// or whatever the default constructor is
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T*,C,A>::clean(void) {
	iterator i = the_map.begin();
	const const_iterator e = the_map.end();
	for ( ; i!=e; ) {
		if (i->second == NULL) {
			iterator j = i;
			j++;
			the_map.erase(i);
			i = j;
		} else {
			i++;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
pair<typename qmap<K,T*,C,A>::iterator,bool>
qmap<K,T*,C,A>::insert(const value_type& __x) {
	return the_map.insert(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::iterator
qmap<K,T*,C,A>::insert(iterator position, const value_type& __x) {
	return the_map.insert(position, __x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
template <typename _InputIterator>
void
qmap<K,T*,C,A>::insert(_InputIterator __first, _InputIterator __last) {
	the_map.insert(__first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T*,C,A>::erase(iterator __position) {
	the_map.erase(__position);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::size_type
qmap<K,T*,C,A>::erase(const key_type& __x) {
	return the_map.erase(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T*,C,A>::erase(iterator __first, iterator __last) {
	the_map.erase(__first, __last);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T*,C,A>::swap(qmap& __x) {
	the_map.swap(__x.the_map);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
void
qmap<K,T*,C,A>::clear(void) {
	the_map.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::iterator
qmap<K,T*,C,A>::find(const key_type& __x) {
	return the_map.find(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::const_iterator
qmap<K,T*,C,A>::find(const key_type& __x) const {
	return the_map.find(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::size_type
qmap<K,T*,C,A>::count(const key_type& __x) const {
	return the_map.count(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::iterator
qmap<K,T*,C,A>::lower_bound(const key_type& __x) {
	return the_map.lower_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::const_iterator
qmap<K,T*,C,A>::lower_bound(const key_type& __x) const {
	return the_map.lower_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::iterator
qmap<K,T*,C,A>::upper_bound(const key_type& __x) {
	return the_map.upper_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
typename qmap<K,T*,C,A>::const_iterator
qmap<K,T*,C,A>::upper_bound(const key_type& __x) const {
	return the_map.upper_bound(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
pair<typename qmap<K,T*,C,A>::iterator,typename qmap<K,T*,C,A>::iterator>
qmap<K,T*,C,A>::equal_range(const key_type& __x) {
	return the_map.equal_range(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
pair<typename qmap<K,T*,C,A>::const_iterator,
	typename qmap<K,T*,C,A>::const_iterator>
qmap<K,T*,C,A>::equal_range(const key_type& __x) const {
	return the_map.equal_range(__x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
bool
operator == (const qmap<K,T*,C,A>& __m1, const qmap<K,T*,C,A>& __m2) {
	return __m1.the_map == __m2.the_map;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
QMAP_TEMPLATE_SIGNATURE
bool
operator < (const qmap<K,T*,C,A>& __m1, const qmap<K,T*,C,A>& __m2) {
	return __m1.the_map < __m2.the_map;
}

//=============================================================================
#endif
}	// end namespace util

#endif	// __UTIL_QMAP_TCC__

