/**
	\file "Object/inst/collection_pool.tcc"
	$Id: collection_pool.tcc,v 1.4 2008/10/24 01:08:57 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_COLLECTION_POOL_TCC__
#define	__HAC_OBJECT_INST_COLLECTION_POOL_TCC__

#include <iostream>
#include <iterator>
// #include <algorithm>

#include "Object/inst/collection_pool.hh"
#include "Object/inst/collection_index_entry.hh"
#include "Object/inst/collection_traits.hh"
#include "util/likely.h"
#include "util/stacktrace.hh"

#define	STACKTRACE_FIND_ITERATOR	(0 && ENABLE_STACKTACE)

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"

//=============================================================================
// class collection_pool method definitions

/**
	Alias to default constructed value.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
const typename COLLECTION_POOL_CLASS::value_type
COLLECTION_POOL_CLASS::default_value;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	We pre-allocate the first chunk of memory.
	The space is reserved but unoccupied.  
	The index-value map is populated with its first entry.
	The address_chunk_map is also mapped with its first entry.  
	NOTE: the unused tail sentinel is not mapped.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
COLLECTION_POOL_CLASS::collection_pool() :
		value_pool(2), 	// one real chunk, one unused tail-sentinel
		index_value_map(), 
		address_chunk_map(), 
		__back(value_pool.begin()) {
	STACKTRACE_CTOR_VERBOSE;
//	value_pool.push_back(value_chunk_type());
	value_pool.front().reserve(INITIAL_RESERVE);	// pre-allocate
	const typename index_value_map_type::value_type
		p(0, value_pool.begin());
	const std::pair<index_value_map_iterator, bool>
		r(index_value_map.insert(p));
	INVARIANT(r.second);	// was successfully inserted
	address_chunk_map[&value_pool.front().front()] = r.first;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
COLLECTION_POOL_TEMPLATE_SIGNATURE
COLLECTION_POOL_CLASS::~collection_pool() {
	STACKTRACE_DTOR_VERBOSE;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
COLLECTION_POOL_TEMPLATE_SIGNATURE
ostream&
COLLECTION_POOL_CLASS::dump(ostream& o) const {
{
	o << "collection_pool: {" << endl << "index_map: " << endl;
	index_value_map_const_iterator
		i(index_value_map.begin()), e(index_value_map.end());
	for ( ; i!=e; ++i) {
		o << "\t[" << i->first << ",+" << i->second->size() << "/" <<
			i->second->capacity() << "]:@" <<
			&i->second->front() << endl;
	}
}{
	o << "address_map: " << endl;
	address_chunk_map_const_iterator
		i(address_chunk_map.begin()), e(address_chunk_map.end());
	for ( ; i!=e; ++i) {
		o << "\t@" << i->first << ":[" << i->second->first <<
			",+" << i->second->second->size() << "/" <<
			i->second->second->capacity() << "]" << endl;
	}
}
	return o << "}";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the internal index (0-indexed).
	\return reference to indexed element.
	The returned pointer need not be memory-managed.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::index_value_map_const_iterator
COLLECTION_POOL_CLASS::__find_index_value_map_iterator(
		const size_type i) const {
#if STACKTRACE_FIND_ITERATOR
	STACKTRACE_BRIEF;
	STACKTRACE_INDENT_PRINT("i = " << i << endl);
#endif
	if (UNLIKELY(i >= this->size())) {
#if STACKTRACE_FIND_ITERATOR
		STACKTRACE_INDENT_PRINT("i out-of-bounds" << endl);
		STACKTRACE_INDENT_PRINT("this->size == " <<
			this->size() << endl);
#endif
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
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::value_type*
COLLECTION_POOL_CLASS::__find(const size_type i) const {
#if STACKTRACE_FIND_ITERATOR
	STACKTRACE_BRIEF;
	STACKTRACE_INDENT_PRINT("i = " << i << endl);
#endif
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
#if 0
/**
	\param i the internal index (0-indexed).
	\return reference to indexed element.
	The returned pointer need not be memory-managed.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::value_type*
COLLECTION_POOL_CLASS::__find(const size_type i) {
#if STACKTRACE_FIND_ITERATOR
	STACKTRACE_BRIEF;
	STACKTRACE_INDENT_PRINT("i = " << i << endl);
#endif
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the 1-based indexed.
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::value_type*
COLLECTION_POOL_CLASS::find(const size_type i) const {
	INVARIANT(i);
	return this->__find(i -1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\param i the 1-based indexed.
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::value_type*
COLLECTION_POOL_CLASS::find(const size_type i) {
	INVARIANT(i);
	return this->__find(i -1);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the internal index (1-indexed)
	\pre the referenced element must exist.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::value_type&
COLLECTION_POOL_CLASS::operator [] (const size_type i) const {
	INVARIANT(i);
	return *this->__find(i -1);	// expects 0-based
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	\param i the internal index (1-indexed)
	\pre the referenced element must exist.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::value_type&
COLLECTION_POOL_CLASS::operator [] (const size_type i) {
	INVARIANT(i);
	return *this->__find(i -1);	// expects 0-based
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param v reference of value to lookup, by address.
	\pre v must be a member of this collection.  
	\return 0-based index that references the argument.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::size_type
COLLECTION_POOL_CLASS::__lookup_index(const value_type& v) const {
//	STACKTRACE_BRIEF;
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
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::size_type
COLLECTION_POOL_CLASS::lookup_index(const value_type& v) const {
	return this->__lookup_index(v) +1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre last chunk is completely filled before calling this.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::value_chunk_type*
COLLECTION_POOL_CLASS::__new_chunk(const size_type n) {
	STACKTRACE_INDENT_PRINT("allocating new chunk." << endl);
	// then last real chunk is full, allocate another
	// re-use the last chunk, and create a new tail-sentinel
	++this->__back;
	value_pool.push_back(value_chunk_type());
	value_chunk_type* pb(&*this->__back);
	INVARIANT(!pb->capacity());	// was empty
	pb->reserve(n);

	const typename index_value_map_type::value_type
		p(this->size(), this->__back);
	// use insert-pair with iterator return to save second lookup
	const std::pair<index_value_map_iterator, bool>
		r(index_value_map.insert(p));
	INVARIANT(r.second);	// was successfully inserted
	address_chunk_map[&pb->front()] = r.first;
	INVARIANT(this->__back == ----value_pool.end());
	return pb;
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
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::value_type*
COLLECTION_POOL_CLASS::push_back(const value_type& v) {
	STACKTRACE_BRIEF;
	const size_type new_index = this->size();
	STACKTRACE_INDENT_PRINT("new_index(0) = " << new_index << endl);
	value_chunk_type* pb(&*this->__back);
	if (!(pb->capacity() -pb->size())) {
		pb = this->__new_chunk(new_index);
	}
	pb->push_back(v);		// or use placement construction?
	// guaranteed NOT to realloc because
	// sufficient space was pre-allocated
	return &pb->back();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Bulk allocate a bunch of elements, which will happen to be contiguous
	ONLY IF the last chunk has not yet been allocated.  
	Otherwise, elements will have one discontinuity.  

	Rationale: To guarantee continuity of range, would have to require
	that vector::reserve can reduce the vector's capacity, which
	the standard library does not guarantee.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
void
COLLECTION_POOL_CLASS::allocate(const size_type n) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("n = " << n << endl);
#if ENABLE_STACKTRACE
	this->dump(cerr << "before ") << endl;
#endif
	value_chunk_type* pb(&*this->__back);
	const size_type sz = this->size();	// used for debugging only
	const size_type pbs = pb->size();
	const size_type pbc = pb->capacity();
if (!pbs) {
	STACKTRACE_INDENT_PRINT("current chunk size 0" << endl);
	if (pbc) {
		STACKTRACE_INDENT_PRINT("reallocating..." << endl);
		// was already allocated, need to remove map entry
		const size_type e = address_chunk_map.erase(&pb->front());
		INVARIANT(e == 1);
	}
	// can just resize this current chunk, and we're done
	pb->resize(n, default_value);
	address_chunk_map[&pb->front()] = --index_value_map.end();
	// index_value_map unchanged
} else {
	STACKTRACE_INDENT_PRINT("current chunk size " << pbs << endl);
	const size_type d = pbc -pbs;	// remaining
	size_type i;
	size_type r;
	if (d >= n) {
		STACKTRACE_INDENT_PRINT("d = " << d << endl);
		i = n;		// current chunk has enough capacity
		r = 0;
	} else {
		i = d;		// will need second chunk
		r = (n-d);
	}
	for ( ; i ; --i) {	// fill current chunk
		pb->push_back(default_value);
	}
	if (r) {
		INVARIANT(pb->size() == pb->capacity());	// was filled
		// allocate in second chunk
		const size_type m = (pbc > r) ? pbc : r;	// take max
		pb = this->__new_chunk(m);
		pb->resize(r, default_value);	// fills.  more efficient?
		// for ( ; r ; --r)
		//	pb->push_back(default_value);
	}
}
#if ENABLE_STACKTRACE
	this->dump(cerr << "after ") << endl;
#endif
	INVARIANT(this->size() == sz +n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapper that just allocates.
	Caveat: returned memory is already default constructed, 
	which means a destruction is needed before placement construction.  
	TODO: find clean, equivalent solution.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
typename COLLECTION_POOL_CLASS::value_type*
COLLECTION_POOL_CLASS::allocate(void) {
	return this->push_back(default_value);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p the pool that contains element v.  
	\param v a reference to an element in the pool.  
	\return the index stub that references the argument in this pool.  
 */
COLLECTION_POOL_TEMPLATE_SIGNATURE
collection_index_entry
lookup_collection_pool_index_entry(
		const collection_pool<Value>& p, const Value& v) {
	typedef	Value			value_type;
	collection_index_entry ret;
	ret.meta_type = value_type::traits_type::type_tag_enum_value;
	ret.pool_type = collection_traits<value_type>::ENUM_VALUE;
	ret.index = p.lookup_index(v);
	INVARIANT(ret.index);
	return ret;
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_COLLECTION_POOL_TCC__

