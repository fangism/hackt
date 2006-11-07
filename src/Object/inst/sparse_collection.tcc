/**
	\file "Object/inst/sparse_collection.tcc"
	$Id: sparse_collection.tcc,v 1.3 2006/11/07 06:35:00 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SPARSE_COLLECTION_TCC__
#define	__HAC_OBJECT_INST_SPARSE_COLLECTION_TCC__

#include <iostream>
#include <iterator>
#include "Object/inst/sparse_collection.h"
#include "util/likely.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class sparse_collection method definitions

/**
	We pre-allocate the first chunk of memory.
	The space is reserved but unoccupied.  
	The index-value map is populated with its first entry.
	The address_chunk_map is also mapped with its first entry.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
SPARSE_COLLECTION_CLASS::sparse_collection() :
		key_index_map(), 
		key_index_array(), 
		value_pool(), 
		index_value_map(), 
		address_chunk_map() {
	value_pool.push_back(value_chunk_type());
	value_pool.front().reserve(INITIAL_RESERVE);	// pre-allocate
	const typename index_value_map_type::value_type
		p(0, value_pool.begin());
	const std::pair<index_value_map_iterator, bool>
		r(index_value_map.insert(p));
	INVARIANT(r.second);	// was successfully inserted
	address_chunk_map[&value_pool.front().front()] = r.first;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copy constructor never supposed to be called, but we define it
	for the sake of containee requirements for the STL library.
	Reaching this at run-time is detected as an error.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
SPARSE_COLLECTION_CLASS::sparse_collection(const this_type& t) :
		key_index_map(), 
		key_index_array(), 
		value_pool(), 
		index_value_map(), 
		address_chunk_map() {
	INVARIANT(!t.size());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
SPARSE_COLLECTION_CLASS::~sparse_collection() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
ostream&
SPARSE_COLLECTION_CLASS::dump(ostream& o) const {
	// TODO: finish me
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the internal index (0-indexed).
	\return reference to indexed element.
	The returned pointer need not be memory-managed.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::index_value_map_const_iterator
SPARSE_COLLECTION_CLASS::__find_index_value_map_iterator(
		const size_type i) const {
	STACKTRACE_BRIEF;
	STACKTRACE_INDENT_PRINT("i = " << i << endl);
	if (UNLIKELY(i >= key_index_array.size())) {
		STACKTRACE_INDENT_PRINT("i out-of-bounds" << endl);
		STACKTRACE_INDENT_PRINT("key-index-array size == " <<
			key_index_array.size() << endl);
		return index_value_map.end();
	}
	index_value_map_const_iterator f(index_value_map.upper_bound(i));
	INVARIANT(f != index_value_map.begin());
	--f;
	return f;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the internal index (0-indexed).
	\return reference to indexed element.
	The returned pointer need not be memory-managed.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
const typename SPARSE_COLLECTION_CLASS::value_type*
SPARSE_COLLECTION_CLASS::__find(const size_type i) const {
	STACKTRACE_BRIEF;
	STACKTRACE_INDENT_PRINT("i = " << i << endl);
	const index_value_map_const_iterator
		f(this->__find_index_value_map_iterator(i));
	if (f == index_value_map.end()) {
		return NULL;
	}
	const size_type remainder = i - f->first;
	const value_pool_iterator& p(f->second);
	INVARIANT(remainder < p->size());
	return &(*p)[remainder];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the internal index (0-indexed).
	\return reference to indexed element.
	The returned pointer need not be memory-managed.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::value_type*
SPARSE_COLLECTION_CLASS::__find(const size_type i) {
	STACKTRACE_BRIEF;
	STACKTRACE_INDENT_PRINT("i = " << i << endl);
	const index_value_map_const_iterator
		f(this->__find_index_value_map_iterator(i));
	if (f == index_value_map.end()) {
		return NULL;
	}
	const size_type remainder = i - f->first;
	const value_pool_iterator& p(f->second);
	INVARIANT(remainder < p->size());
	return &(*p)[remainder];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the 1-based indexed.
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
const typename SPARSE_COLLECTION_CLASS::value_type*
SPARSE_COLLECTION_CLASS::find(const size_type i) const {
	INVARIANT(i);
	return this->__find(i -1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the 1-based indexed.
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::value_type*
SPARSE_COLLECTION_CLASS::find(const size_type i) {
	INVARIANT(i);
	return this->__find(i -1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Search for value by key.  
	\return NULL if it doesn't exist.  
	The returned pointer need not be memory-managed.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
const typename SPARSE_COLLECTION_CLASS::value_type*
SPARSE_COLLECTION_CLASS::find(const key_type& k) const {
	STACKTRACE_BRIEF;
	const size_type i = this->lookup_index(k);	// 1-based index
	if (!i) {
		return NULL;
	}
	STACKTRACE_INDENT_PRINT("i(1) = " << i << endl);
	return this->__find(i -1);	// expects 0-based
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Search for value by key.  
	\return NULL if it doesn't exist.  
	The returned pointer need not be memory-managed.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::value_type*
SPARSE_COLLECTION_CLASS::find(const key_type& k) {
	STACKTRACE_BRIEF;
	const size_type i = this->lookup_index(k);	// 1-based index
	if (!i) {
		return NULL;
	}
	STACKTRACE_INDENT_PRINT("i(1) = " << i << endl);
	return this->__find(i -1);	// expects 0-based
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::iterator
SPARSE_COLLECTION_CLASS::find_iterator(const key_type& k) {
	return iterator(*this, key_index_map.find(k));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::const_iterator
SPARSE_COLLECTION_CLASS::find_iterator(const key_type& k) const {
	return const_iterator(*this, key_index_map.find(k));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the internal index (1-indexed)
	\pre the referenced element must exist.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
const typename SPARSE_COLLECTION_CLASS::value_type&
SPARSE_COLLECTION_CLASS::operator [] (const size_type i) const {
	INVARIANT(i);
	return *this->__find(i -1);	// expects 0-based
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the internal index (1-indexed)
	\pre the referenced element must exist.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::value_type&
SPARSE_COLLECTION_CLASS::operator [] (const size_type i) {
	INVARIANT(i);
	return *this->__find(i -1);	// expects 0-based
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates associative key into internal index for lookup.  
	\pre key-value pair sought must already exist.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
const typename SPARSE_COLLECTION_CLASS::value_type&
SPARSE_COLLECTION_CLASS::operator [] (const key_type& k) const {
	STACKTRACE_BRIEF;
	const size_type i = this->__lookup_index(k);	// 0-based index
	return *this->__find(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates associative key into internal index for lookup.  
	\pre key-value pair sought must already exist.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::value_type&
SPARSE_COLLECTION_CLASS::operator [] (const key_type& k) {
	STACKTRACE_BRIEF;
	const size_type i = this->__lookup_index(k);	// 0-based index
	return *this->__find(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i 0-indexed index.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
const typename SPARSE_COLLECTION_CLASS::key_type&
SPARSE_COLLECTION_CLASS::__lookup_key(const size_type i) const {
	STACKTRACE_BRIEF;
	INVARIANT(i < key_index_array.size());
	return key_index_array[i]->first;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i 1-indexed index.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
const typename SPARSE_COLLECTION_CLASS::key_type&
SPARSE_COLLECTION_CLASS::lookup_key(const size_type i) const {
	INVARIANT(i);
	return this->__lookup_key(i -1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param v the address of which is used to lookup the key.  
	This is how the value knows its own key, because the key
	is not stored with the value.  
	\pre v reference is actually managed by this container.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
const typename SPARSE_COLLECTION_CLASS::key_type&
SPARSE_COLLECTION_CLASS::lookup_key(const value_type& v) const {
	STACKTRACE_BRIEF;
	const size_type i = this->lookup_index(v);
	return this->lookup_key(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates key to internal index.
	\pre the sought element must exist.
	\return 0-based index.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::size_type
SPARSE_COLLECTION_CLASS::__lookup_index(const key_type& k) const {
	STACKTRACE_BRIEF;
	const key_index_map_const_iterator f(key_index_map.find(k));
	INVARIANT(f != key_index_map.end());
	return f->second;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 1-based index, returns 0 if not found.
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::size_type
SPARSE_COLLECTION_CLASS::lookup_index(const key_type& k) const {
	STACKTRACE_BRIEF;
	const key_index_map_const_iterator f(key_index_map.find(k));
#if ENABLE_STACKTRACE
	if (f != key_index_map.end()) {
		STACKTRACE_INDENT_PRINT("found index: " << f->second +1 << endl);
		return f->second +1;
	} else {
		STACKTRACE_INDENT_PRINT("index not found: " << f->second << endl);
		return 0;
	}
#else
	return (f != key_index_map.end()) ? f->second +1 : 0;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param v reference of value to lookup, by address.
	\pre v must be a member of this collection.  
	\return 
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::size_type
SPARSE_COLLECTION_CLASS::__lookup_index(const value_type& v) const {
	STACKTRACE_BRIEF;
	address_chunk_map_const_iterator f(address_chunk_map.upper_bound(&v));
	INVARIANT(f != address_chunk_map.begin());
	--f;
	const index_value_map_const_iterator& ivi(f->second);
	const value_pool_const_iterator vpi(ivi->second);
	const size_type offset = std::distance(&vpi->front(), &v);
	INVARIANT(offset <= vpi->size());
	return ivi->first +offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 1-indexed index.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::size_type
SPARSE_COLLECTION_CLASS::lookup_index(const value_type& v) const {
	return this->__lookup_index(v) +1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates and assigns a key-value pair in the collection, 
	updating the internal bookkeepping structures as necessary.  
	Since newly allocated chunks double in size, 
	the access time through the pool is actually log(log(N)).
	Algorithm:
	\param k the key with which to associate the new value.
	\param v the value (initial) to be added.  
	\return true if value was actually added, i.e. key didn't already
		exist.  Returns a pointer to the location of the newly
		added value, which is a copy of the value argument.  
 */
SPARSE_COLLECTION_TEMPLATE_SIGNATURE
typename SPARSE_COLLECTION_CLASS::value_type*
SPARSE_COLLECTION_CLASS::insert(const key_type& k, const value_type& v) {
	STACKTRACE_BRIEF;
	const size_type new_index = key_index_array.size();
	const typename key_index_map_type::value_type kip(k, new_index);
	const std::pair<key_index_map_iterator, bool>
		kpr(key_index_map.insert(kip));
	if (!kpr.second) {
		STACKTRACE_INDENT_PRINT("already in map." << endl);
		return NULL;
	}
	key_index_array.push_back(kpr.first);
	STACKTRACE_INDENT_PRINT("new_index(0) = " << new_index << endl);
	value_chunk_type* pb(&value_pool.back());
	if (!(pb->capacity() -pb->size())) {
		STACKTRACE_INDENT_PRINT("allocating new chunk." << endl);
		// then last chunk is full, allocate another
		value_pool.push_back(value_chunk_type());
//		value_pool.back().reserve(pb->capacity() << 1);	// double
		value_pool.back().reserve(new_index);		// double
		pb = &value_pool.back();
		const value_pool_iterator ve(--value_pool.end());
		const typename index_value_map_type::value_type
			p(new_index, ve);
		// use insert-pair with iterator return to save second lookup
		const std::pair<index_value_map_iterator, bool>
			r(index_value_map.insert(p));
		INVARIANT(r.second);	// was successfully inserted
		address_chunk_map[&pb->front()] = r.first;
	}
	pb->push_back(v);
	// guaranteed NOT to realloc because
	// sufficient space was pre-allocated
	return &pb->back();
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_SPARSE_COLLECTION_TCC__

