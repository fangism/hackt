/**
	\file "hash_qmap.tcc"
	Template method definitions for queryable hash map class.  

	$Id: hash_qmap.tcc,v 1.1.16.1 2005/02/03 03:34:55 fang Exp $
 */

#ifndef	__UTIL_HASH_QMAP_TCC__
#define	__UTIL_HASH_QMAP_TCC__

#include "hash_qmap.h"

namespace util {
//=============================================================================
// class hash_qmap method definitions

HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T,H,E,A>::hash_qmap() : the_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T,H,E,A>::hash_qmap(size_type __n) : the_map(__n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T,H,E,A>::hash_qmap(size_type __n, const hasher& __hf) :
		the_map(__n, __hf) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T,H,E,A>::hash_qmap(size_type __n, const hasher& __hf, 
		const key_equal& __eql, const allocator_type& __a) :
		the_map(__n, __hf, __eql, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
template <class _InputIterator>
hash_qmap<K,T,H,E,A>::hash_qmap(_InputIterator __f, _InputIterator __l) :
		the_map(__f, __l) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
template <class _InputIterator>
hash_qmap<K,T,H,E,A>::hash_qmap(_InputIterator __f, _InputIterator __l, 
		size_type __n) :
		the_map(__f, __l, __n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
template <class _InputIterator>
hash_qmap<K,T,H,E,A>::hash_qmap(_InputIterator __f, _InputIterator __l, 
		size_type __n, const hasher& __hf) :
		the_map(__f, __l, __n, __hf) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
template <class _InputIterator>
hash_qmap<K,T,H,E,A>::hash_qmap(_InputIterator __f, _InputIterator __l, 
		size_type __n, const hasher& __hf, 
		const key_equal& __eql, const allocator_type& __a) :
		the_map(__f, __l, __n, __hf, __eql, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T,H,E,A>::~hash_qmap() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T,H,E,A>::swap(hash_qmap& h) {
	the_map.swap(h.the_map);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
pair<typename hash_qmap<K,T,H,E,A>::iterator, bool>
hash_qmap<K,T,H,E,A>::insert(const value_type& __obj) {
	return the_map.insert(__obj);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
pair<typename hash_qmap<K,T,H,E,A>::iterator, bool>
hash_qmap<K,T,H,E,A>::insert_noresize(const value_type& __obj) {
	return the_map.insert_noresize(__obj);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T,H,E,A>::iterator
hash_qmap<K,T,H,E,A>::find(const key_type& __key) {
	return the_map.find(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T,H,E,A>::const_iterator
hash_qmap<K,T,H,E,A>::find(const key_type& __key) const {
	return the_map.find(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T,H,E,A>::mapped_type&
hash_qmap<K,T,H,E,A>::operator [] (const key_type& __key) {
	// finds or inserts if not found!
	return the_map[__key];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T,H,E,A>::mapped_type
hash_qmap<K,T,H,E,A>::operator [] (const key_type& __key) const {
	const_iterator i = find(__key);	// uses find() const
	return (i != the_map.end()) ? i->second : mapped_type();
	// if T is a pointer class, should be equivalent to NULL
	// or whatever the default constructor is
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T,H,E,A>::size_type
hash_qmap<K,T,H,E,A>::count(const key_type& __key) const {
	return the_map.count(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
pair<typename hash_qmap<K,T,H,E,A>::iterator,
	typename hash_qmap<K,T,H,E,A>::iterator>
hash_qmap<K,T,H,E,A>::equal_range(const key_type& __key) {
	return the_map.equal_range(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
pair<typename hash_qmap<K,T,H,E,A>::const_iterator,
	typename hash_qmap<K,T,H,E,A>::const_iterator>
hash_qmap<K,T,H,E,A>::equal_range(const key_type& __key) const {
	return the_map.equal_range(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T,H,E,A>::size_type
hash_qmap<K,T,H,E,A>::erase(const key_type& __key) {
	return the_map.erase(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T,H,E,A>::erase(iterator __it) {
	the_map.erase(__it);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T,H,E,A>::erase(iterator __f, iterator __l) {
	the_map.erase(__f, __l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T,H,E,A>::clear(void) {
	the_map.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T,H,E,A>::resize(size_type __hint) {
	the_map.resize(__hint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T,H,E,A>::size_type
hash_qmap<K,T,H,E,A>::bucket_count(void) const {
	return the_map.bucket_count();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T,H,E,A>::size_type
hash_qmap<K,T,H,E,A>::max_bucket_count(void) const {
	return the_map.max_bucket_count();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T,H,E,A>::size_type
hash_qmap<K,T,H,E,A>::elems_in_bucket(size_type __n) const {
	return the_map.elems_in_bucket(__n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
bool
operator == (const hash_qmap<K,T,H,E,A>& __h1, 
		const hash_qmap<K,T,H,E,A>& __h2) {
	return __h1.the_map == __h2.the_map;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
bool
operator != (const hash_qmap<K,T,H,E,A>& __h1, 
		const hash_qmap<K,T,H,E,A>& __h2) {
	return __h1.the_map != __h2.the_map;
}

//=============================================================================
// class hash_qmap method definitions (specialized)

HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T*,H,E,A>::hash_qmap() : the_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T*,H,E,A>::hash_qmap(size_type __n) : the_map(__n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T*,H,E,A>::hash_qmap(size_type __n, const hasher& __hf) :
		the_map(__n, __hf) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T*,H,E,A>::hash_qmap(size_type __n, const hasher& __hf, 
		const key_equal& __eql, const allocator_type& __a) :
		the_map(__n, __hf, __eql, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
template <class _InputIterator>
hash_qmap<K,T*,H,E,A>::hash_qmap(_InputIterator __f, _InputIterator __l) :
		the_map(__f, __l) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
template <class _InputIterator>
hash_qmap<K,T*,H,E,A>::hash_qmap(_InputIterator __f, _InputIterator __l, 
		size_type __n) :
		the_map(__f, __l, __n) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
template <class _InputIterator>
hash_qmap<K,T*,H,E,A>::hash_qmap(_InputIterator __f, _InputIterator __l, 
		size_type __n, const hasher& __hf) :
		the_map(__f, __l, __n, __hf) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
template <class _InputIterator>
hash_qmap<K,T*,H,E,A>::hash_qmap(_InputIterator __f, _InputIterator __l, 
		size_type __n, const hasher& __hf, 
		const key_equal& __eql, const allocator_type& __a) :
		the_map(__f, __l, __n, __hf, __eql, __a) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
hash_qmap<K,T*,H,E,A>::~hash_qmap() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T*,H,E,A>::swap(hash_qmap& h) {
	the_map.swap(h.the_map);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
pair<typename hash_qmap<K,T*,H,E,A>::iterator, bool>
hash_qmap<K,T*,H,E,A>::insert(const value_type& __obj) {
	return the_map.insert(__obj);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
pair<typename hash_qmap<K,T*,H,E,A>::iterator, bool>
hash_qmap<K,T*,H,E,A>::insert_noresize(const value_type& __obj) {
	return the_map.insert_noresize(__obj);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T*,H,E,A>::iterator
hash_qmap<K,T*,H,E,A>::find(const key_type& __key) {
	return the_map.find(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T*,H,E,A>::const_iterator
hash_qmap<K,T*,H,E,A>::find(const key_type& __key) const {
	return the_map.find(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T*,H,E,A>::mapped_type&
hash_qmap<K,T*,H,E,A>::operator [] (const key_type& __key) {
	// finds or inserts if not found!
	return the_map[__key];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T*,H,E,A>::mapped_type
hash_qmap<K,T*,H,E,A>::operator [] (const key_type& __key) const {
	const_iterator i = find(__key);	// uses find() const
	return (i != the_map.end()) ? i->second : NULL;
	// if T is a pointer class, should be equivalent to NULL
	// or whatever the default constructor is
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T*,H,E,A>::size_type
hash_qmap<K,T*,H,E,A>::count(const key_type& __key) const {
	return the_map.count(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
pair<typename hash_qmap<K,T*,H,E,A>::iterator,
	typename hash_qmap<K,T*,H,E,A>::iterator>
hash_qmap<K,T*,H,E,A>::equal_range(const key_type& __key) {
	return the_map.equal_range(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
pair<typename hash_qmap<K,T*,H,E,A>::const_iterator,
	typename hash_qmap<K,T*,H,E,A>::const_iterator>
hash_qmap<K,T*,H,E,A>::equal_range(const key_type& __key) const {
	return the_map.equal_range(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T*,H,E,A>::size_type
hash_qmap<K,T*,H,E,A>::erase(const key_type& __key) {
	return the_map.erase(__key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T*,H,E,A>::erase(iterator __it) {
	the_map.erase(__it);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T*,H,E,A>::erase(iterator __f, iterator __l) {
	the_map.erase(__f, __l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T*,H,E,A>::clear(void) {
	the_map.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T*,H,E,A>::resize(size_type __hint) {
	the_map.resize(__hint);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T*,H,E,A>::size_type
hash_qmap<K,T*,H,E,A>::bucket_count(void) const {
	return the_map.bucket_count();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T*,H,E,A>::size_type
hash_qmap<K,T*,H,E,A>::max_bucket_count(void) const {
	return the_map.max_bucket_count();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
typename hash_qmap<K,T*,H,E,A>::size_type
hash_qmap<K,T*,H,E,A>::elems_in_bucket(size_type __n) const {
	return the_map.elems_in_bucket(__n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
void
hash_qmap<K,T*,H,E,A>::clean(void) {
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
HASH_QMAP_TEMPLATE_SIGNATURE
bool
operator == (const hash_qmap<K,T*,H,E,A>& __h1, 
		const hash_qmap<K,T*,H,E,A>& __h2) {
	return __h1.the_map == __h2.the_map;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
HASH_QMAP_TEMPLATE_SIGNATURE
bool
operator != (const hash_qmap<K,T*,H,E,A>& __h1, 
		const hash_qmap<K,T*,H,E,A>& __h2) {
	return __h1.the_map != __h2.the_map;
}

//=============================================================================
}	// end namespace HASH_QMAP_NAMESPACE

#endif	// __UTIL_HASH_QMAP_TCC__
