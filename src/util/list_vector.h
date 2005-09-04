/**
	\file "util/list_vector.h"
	Really long extendable vector implemented as a list of vectors.  
	Give the abstraction of a continuous array.  

	$Id: list_vector.h,v 1.10 2005/09/04 21:15:07 fang Exp $
 */

#ifndef	__UTIL_LIST_VECTOR_H__
#define	__UTIL_LIST_VECTOR_H__


#include <iosfwd>
#include <algorithm>

#include "util/macros.h"
#include "util/list_vector_fwd.h"
#include "util/STL/list.h"
#include "util/qmap.h"
#include "util/nested_iterator.h"

#define	LIST_VECTOR_CLASS	list_vector<T,ValAlloc,VecAlloc>

namespace util {
USING_LIST
using std::vector;
using std::ostream;
using util::qmap;

//=============================================================================
/**
	Implements vector interface.  
	The goal of this is to provide near constant-time random access
	without ever having to perform an expensive reallocation.  

	Invariant:
	The list of vectors, must always contain one vector, 
	because the list cannot be empty to use nested iterators.  
	vec_list must never be empty!
	(else how would one return the end() nested iterator?)

	Chunks need not be the same size!

	Consider using a map of sizes for rapid indexing...
	Use lower_bound.

	A more efficient implementation will be provided later
	for fixed chunk sizes, then can compute index arithmetically.  

	Doesn't implement erase(), push_front(), pop_front().
	\param T value type of element.  
	\param ValAlloc allocator for the value type.  
		std::allocator is sufficient because the vector is
		already efficiently managed.  
	\param VecAlloc allocator for the vector chunks.  
		If you're concerned about efficiency of creation and deletion
		of the vector chunks (not the values), consider using
		list_vector_pool<vector<T,ValAlloc> >.
 */
template <class T, class ValAlloc, class VecAlloc>
class list_vector {
	typedef	LIST_VECTOR_CLASS		this_type;
public:
	typedef	T				value_type;
	typedef	T*				pointer;
	typedef	const T*			const_pointer;
	typedef	T&				reference;
	typedef	const T&			const_reference;
private:
	// implementation details
	typedef	vector<value_type, ValAlloc>	vector_type;
	typedef	list<vector_type, VecAlloc>	list_type;
	typedef	typename list_type::iterator		list_iterator;
	typedef	typename list_type::const_iterator	const_list_iterator;
	typedef	typename list_type::reverse_iterator	reverse_list_iterator;
	typedef	typename list_type::const_reverse_iterator
						const_reverse_list_iterator;
	typedef	typename vector_type::iterator		vector_iterator;
	typedef	typename vector_type::const_iterator	const_vector_iterator;
	typedef	typename vector_type::reverse_iterator	reverse_vector_iterator;
	typedef	typename vector_type::const_reverse_iterator
						const_reverse_vector_iterator;
public:
	typedef	VecAlloc			vector_allocator_type;
	typedef	ValAlloc			value_allocator_type;
	typedef	ValAlloc			allocator_type;
	typedef	typename vector_type::size_type	size_type;
	typedef	typename vector_type::difference_type	difference_type;
public:
	// iterator type interface
	// should outer iterator type always be const?  No.
	typedef	nested_iterator<list_iterator, vector_iterator>
					iterator_base;
	typedef	nested_iterator<const_list_iterator, const_vector_iterator>
					const_iterator_base;
	typedef	nested_reverse_iterator<reverse_list_iterator, reverse_vector_iterator>
					reverse_iterator_base;
	typedef	nested_reverse_iterator<const_reverse_list_iterator, const_reverse_vector_iterator>
					const_reverse_iterator_base;

#if 1
	// for now, make them the same...
	typedef	iterator_base			iterator;
	typedef	const_iterator_base		const_iterator;
	typedef	reverse_iterator_base		reverse_iterator;
	typedef	const_reverse_iterator_base	const_reverse_iterator;
#else

	// Can also derive these to be smarter and use the map!
	// TODO: derive specializations to override the standard 
	// implementations in nested_iterator.  
	// Will need back-references...

	struct iterator : public iterator_base {
		// stuff goes here
	};	// end struct iterator

	struct const_iterator : public const_iterator_base {
		// stuff goes here
	};	// end struct const_iterator
#endif
private:
	/**
		Map from index to vector chunk.
		Prefer a queryable map with constant semantics
		lookup because we never add NULL pointers.  
		Note that since the key type is size_type, 
		it cannot be negative, thus 0 is the absolute lowest bound.  
	 */
	typedef	qmap<size_type, vector_type*>	vec_map_type;
	typedef	typename vec_map_type::iterator		vec_map_iterator;
	typedef	typename vec_map_type::const_iterator	const_vec_map_iterator;
private:
	static const size_type		DEFAULT_CHUNK_SIZE = 16;
private:
	/**
		The number of elements to allocate at a time, 
		each time the capacity is exceeded.  
	 */
	size_type			current_chunk_size;

	/**
		The list of vectors representing one big abstract vector.  
	 */
	list_type			vec_list;

	/**
		Maps from index to vector.
		The purpose of this map is to quickly jump to
		the correct vector chunk (log-time) and
		perform a fast-lookup.  

		INVARIANT: Each vector-pointer in the map is mapped by the
			number of elements in the vector-list to the 
			left of the mapped vector-pointer.  
		INVARIANT: vec_map.size() == vec_list.size()
			EXCEPT when this is empty, in which case, 
			vec_list contains one chunk, and vec_map is empty.
	 */
	vec_map_type			vec_map;

private:

	/**
		\return modifiable iterator to the end sentinel chunk, 
		which is always empty.  
	 */
	inline
	list_iterator
	end_sentinel_iter(void) { return --vec_list.end(); }

	/**
		\return modifiable iterator to the reverse-end sentinel chunk, 
		which is always empty.  
		Note: should point to the head sentinel chunk.  
	 */
	inline
	reverse_list_iterator
	rend_sentinel_iter(void) { return --vec_list.rend(); }

	/**
		\return read-only iterator to the end sentinel chunk, 
		which is always empty.  
	 */
	inline
	const_list_iterator
	end_sentinel_iter(void) const { return --vec_list.end(); }

	/**
		\return modifiable iterator to the reverse-end sentinel chunk, 
		which is always empty.  
		Note: should point to the head sentinel chunk.  
	 */
	inline
	const_reverse_list_iterator
	rend_sentinel_iter(void) const { return --vec_list.rend(); }

	inline
	list_iterator
	vec_list_front(void) { return ++vec_list.begin(); }

	inline
	const_list_iterator
	vec_list_front(void) const { return ++vec_list.begin(); }

	inline
	list_iterator
	vec_list_back(void) { return --this->end_sentinel_iter(); }

	inline
	const_list_iterator
	vec_list_back(void) const { return --this->end_sentinel_iter(); }

	/**
		Reverse iterator pointing to the front of the chunk list.  
	 */
	inline
	reverse_list_iterator
	vec_list_rfront(void) { return --this->rend_sentinel_iter(); }

	inline
	const_reverse_list_iterator
	vec_list_rfront(void) const { return --this->rend_sentinel_iter(); }

	/**
		Reverse iterator pointing to the back of the chunk list.  
	 */
	inline
	reverse_list_iterator
	vec_list_rback(void) { return ++vec_list.rbegin(); }

	inline
	const_reverse_list_iterator
	vec_list_rback(void) const { return ++vec_list.rbegin(); }


public:
	/**
		Default empty constructor.  

		Note on constructors:
		Trying to copy <bits/stl_vector.h>, 
		however, not bothering with constructors that
		take allocator for argument... yet.

		Initializes with head and tail sentinels, empty vectors.
	 */
	list_vector() : current_chunk_size(DEFAULT_CHUNK_SIZE), 
			vec_list(2), vec_map() {
	}

	/**
		Creates one vector chunk filled with default value.
		Also sets chunk_size to c.
		\param c number of elements, and new chunk size.
	 */
	list_vector(const size_type c) : current_chunk_size(c), 
			vec_list(2), vec_map() {
		size_type i = 0;
		for ( ; i < c; i++)
			push_back(value_type());
		// first_chunk(c);
	}

	/**
		Creates one vector chunk filled with same value.
		Also sets chunk_size to c.
		\param c number of elements, and new chunk size.
		\param v fill value.  
	 */
	list_vector(const size_type c, const value_type& v) :
			current_chunk_size(c),
			vec_list(2), vec_map() {
		size_type i = 0;
		for ( ; i < c; i++)
			push_back(v);
		// first_chunk(c, v);
	}

	// cannot use default copy-constructor because of pointers
	list_vector(const this_type&);

	/// construct from a sequence of values
	template <class InIter>
	list_vector(InIter first, InIter last,
			const size_type s = DEFAULT_CHUNK_SIZE) :
			current_chunk_size(s), vec_list(), vec_map() {
		copy(first, last, back_inserter(*this));
	}

	/// default destructor
	~list_vector() { }

	/**
		\return the current size of block allocation.  
	 */
	size_type
	get_chunk_size(void) const {
		return current_chunk_size;
	}

	/**
		Only subsequent allocations are affected by the new 
		chunk size.  
		This size is used to automatically reserve
		a vector size each time a new vector is allocated.  
		\param s the new chunk size.
	 */
	void
	set_chunk_size(const size_type s) {
		current_chunk_size = s;
	}

#if 0
	list_vector&
	operator = (const list_vector& l);

	/**
		Clobbers old data, replace-filling with new data.  
	 */
	void
	assign(const size_type n, const value_type& v);

	template <class InIter>
	void
	assign(InIter first, InIter last);
#endif

#if 0
	allocator_type
	get_allocator(void) const;

	// what about the other allocator, VecAlloc vs. ValAlloc?
#endif


public:

	/// Modifiable iterator to first element
	iterator
	begin(void) {
		// const list_iterator b = vec_list.begin();
		// return iterator(b, b->begin());
		const list_iterator lf(vec_list_front());
		return iterator(lf, lf->begin());
	}

	/**
		Read-only iterator to first element.
		Note that for an empty list_vector, 
		vec_list_front points to the end-sentinel.  
		At all other times, it should point to the 
		front (valid) chunk.  
	 */
	const_iterator
	begin(void) const {
		// const const_list_iterator b = vec_list.begin();
		// return const_iterator(b, b->begin());
		const const_list_iterator lf(vec_list_front());
		return const_iterator(lf, lf->begin());
	}

	/**
		Modifiable iterator to one-past-last element
		The outer iterator used is the sentinel, 
		which is a dummy chunk after the last valid chunk.  
		(It is the true end of the chunk-list.)
		\pre vec_list is not empty.
	 */
	iterator
	end(void) {
		const list_iterator b(this->end_sentinel_iter());
		return iterator(b, b->end());
		// should be same as begin() wbecause end sentinel is empty
	}

	/**
		Read-only iterator to one-past-last element
		The outer iterator used is the sentinel, 
		which is a dummy chunk after the last valid chunk.  
		(It is the true end of the chunk-list.)
		\pre vec_list is not empty.
	 */
	const_iterator
	end(void) const {
		const const_list_iterator b(this->end_sentinel_iter());
		return const_iterator(b, b->end());
		// should be same as begin() wbecause end sentinel is empty
	}

	/// Modifiable iterator to last element
	reverse_iterator
	rbegin(void) {
		// ++ because sentinel is last chunk
		const reverse_list_iterator b(vec_list_rback());
		return reverse_iterator(b, b->rbegin());
	}

	/// Read-only iterator to last element
	const_reverse_iterator
	rbegin(void) const {
		// ++ because sentinel is last chunk
		const const_reverse_list_iterator b(vec_list_rback());
		return const_reverse_iterator(b, b->rbegin());
	}

	/**
		Modifiable iterator to one-past-first element
		This may require a head sentinel...
		\pre vec_list is not empty.
	 */
	reverse_iterator
	rend(void) {
		const reverse_list_iterator b(this->rend_sentinel_iter());
		return reverse_iterator(b, b->rend());
		// should be same as b->rbegin()
	}

	/**
		Read-only iterator to one-past-first element
		This may require a head sentinel...
		\pre vec_list is not empty.
	 */
	const_reverse_iterator
	rend(void) const {
		const const_reverse_list_iterator b(this->rend_sentinel_iter());
		return const_reverse_iterator(b, b->rend());
		// should be same as b->rbegin()
	}

	/**
		Helper function for accumulating size of chunks.  
	 */
	static
	size_type
	accumulate_size(const size_type s, const vector_type& v) {
		return s +v.size();
	}

	/**
		\return total number of elements.

		For efficiency, may want to cache this value.  
		This should ideally be constant-time.  
		This assumes we've maintained the size/index invariant for
		the vector-map.  
	 */
	size_type
	size(void) const {
		if (vec_map.empty()) {
			return 0;
		} else {
			// constant time!
			const typename vec_map_type::value_type&
				last(*(--vec_map.end()));
			return last.first +last.second->size();
		}
	}

	/**
		Normally is the maximum number of elements addressable 
		by a single vector, size_type(-1) / sizeof(value_type).
		For now, we conservatively return that, although in reality
		it can hold far more.  
		\return maximum number of elements containable.  

		Q: shouldn't this be static?  has nothing to do with object...
	 */
	size_type
	max_size(void) const {
		static const size_type s = size_type(-1) / sizeof(value_type);
		return s;
	}

#if 0
	void
	resize(const size_type n);
#endif

	/**
		Accumulates size of all but-last chunk, 
		assuming each is full, and then adds the capacity
		of the last vector.  
		\pre vec_list is not empty, and all vectors before the last
			one are full (last may also be full).
		\return Total number of storable elements, 
			before having to allocate more memory.  
		is necessary.  
	 */
	size_type
	capacity(void) const {
		if (vec_map.empty()) {
			return 0;
		} else {
			const typename vec_map_type::value_type&
				last(*(--vec_map.end()));
			return last.first +last.second->capacity();
		}
	}

	/**
		Invariant: whole container is empty if the first chunk
		is empty.  
		\pre vec_list is never empty.
		\return emptiness.  
	 */
	bool
	empty(void) const {
		return (vec_list.size() == 2) || (vec_list_front()->empty());
		// only includes head- and end-sentinels
	}

#if 0
	/**
		Reserves capacity.  
		Using chunk size? or max?
	 */
	void
	reserve(const size_type n);
#endif

	/**
		\return reference to indexed element.
		\pre n < size AND vec_map is not empty!
		This lookup is greatly accelerated by a map of vector*'s.
		Note: this access is unchecked, use at() for range-checked
		indexing.
	 */
	reference
	operator [] (const size_type n) {
		INVARIANT(!vec_map.empty());
		// lower_bound returns iterator first element >= key
		// upper_bound returns iterator first element > key
		const vec_map_iterator
			vec_iter(--vec_map.upper_bound(n));
//			vec_iter = --vec_map.lower_bound(n+1);	// same
		// INVARIANT(vec_iter != vec_map.end());
		INVARIANT(vec_iter->second);
		INVARIANT(!vec_iter->second->empty());
		INVARIANT(n >= vec_iter->first);
		return (*vec_iter->second)[n -vec_iter->first];
	}

	/**
		\return const reference to indexed element.
		\pre n < size.
		This would be greatly accelerated by a map of 
		list-iterators.  
		Note: this access is unchecked, use at() for range-checked
		indexing.
	 */
	const_reference
	operator [] (const size_type n) const {
		INVARIANT(!vec_map.empty());
		// lower_bound returns iterator first element >= key
		// upper_bound returns iterator first element > key
		const const_vec_map_iterator
			vec_iter(--vec_map.upper_bound(n));
//			vec_iter = --vec_map.lower_bound(n+1);	// same
		// INVARIANT(vec_iter != vec_map.end());
		INVARIANT(vec_iter->second);
		INVARIANT(!vec_iter->second->empty());
		INVARIANT(n >= vec_iter->first);
		return (*vec_iter->second)[n -vec_iter->first];
	}

	/**
		Assertion check for index bounds.  
		This is an explicit check requested by the user, 
		so do not make this conditional on DISABLE_INVARIANT, 
		defined in "macros.h".  
	 */
	void
	range_check(const size_type n) const {
		assert(n < this->size());
	}

	/// range-checked index
	reference
	at(const size_type n) {
		range_check(n);
		return (*this)[n];
	}

	/// range-checked index
	const_reference
	at(const size_type n) const {
		range_check(n);
		return (*this)[n];
	}

	/**
		\pre this is not empty, there is at least one element in
		a valid chunk (not the sentinels).  
		\return reference to first element.
	 */
	reference
	front(void) {
		INVARIANT(!this->empty());
		return vec_list_front()->front();
	}

	/**
		\pre this is not empty, there is at least one element in
		a valid chunk (not the sentinels).  
		\return const reference to first element.
	 */
	const_reference
	front(void) const {
		INVARIANT(!this->empty());
		return vec_list_front()->front();
	}

	/**
		If we implement pop_back(), then we need to guarantee
		that the last vector chunk is never empty.  
		\pre this is not empty, there is at least one element in
		a valid chunk (not the sentinels).  
		\return reference to first element.
	 */
	reference
	back(void) {
		INVARIANT(!this->empty());
		return vec_list_back()->back();
	}

	/**
		If we implement pop_back(), then we need to guarantee
		that the last vector chunk is never empty.  
		\pre this is not empty, there is at least one element in
		a valid chunk (not the sentinels).  
		\return const reference to first element.
	 */
	const_reference
	back(void) const {
		INVARIANT(!this->empty());
		return vec_list_back()->back();
	}

	/**
		Appends element to end, allocating chunk if necessary.
		Also maintains vec_map's size/index invariant.  
		\pre vec_list is never empty.
		\param v value to append to sequence.  
	 */
	void
	push_back(const value_type& v) {
		// if last chunk points to head sentinel, 
		// it will be detected as a full chunk.
		vector_type* last_chunk(&*vec_list_back());
		const size_type cap(last_chunk->capacity());
		if (UNLIKELY(last_chunk->size() == cap)) {
			// then we need to reserve next chunk
			vec_list.insert(this->end_sentinel_iter(),
				vector_type());
/****
			In std::list, insert() does not invalidate
			any other iterators or references.  
****/
			// aw hell, we do it anyways, it's infrequent...
			last_chunk = &*vec_list_back();
			vec_map[this->size()] = last_chunk;
			last_chunk->reserve(current_chunk_size);
		} 
		last_chunk->push_back(v);
		INVARIANT(!vec_list_back()->empty());	// last chunk
		INVARIANT(vec_list.back().empty());	// end-sentinel
	}

	/**
		Releases last element in list.  
		Assuming this operation is less frequent, 
		we do more maintenance work here.  
		\pre The container is not empty.  
	 */
	void
	pop_back(void) {
		INVARIANT(!this->empty());
		vector_type* last_chunk(&*vec_list_back());
#if 1
		// eager implementation
		INVARIANT(!last_chunk->empty());
		last_chunk->pop_back();
#endif
		if (UNLIKELY(last_chunk->empty())) {
			vec_list.erase(vec_list_back());
			// in case it was the last valid chunk
			last_chunk = &*vec_list_back();
			vec_map.erase(--vec_map.end());
		}
#if 0
		// lazy implementation
		// NOTE: this breaks because begin() != end() when
		// the container is empty, thus we must use the eager 
		// implementation, unless we change begin().
		INVARIANT(!last_chunk->empty());
		last_chunk->pop_back();
#endif
	}

	/**
		Normally inserting an element into a vector is the worst
		thing one can possibly do.  
		This improves upon the operation by strategically
		slicing the vector into segments, thus reducing
		the amount of memory moved.  
		(One advantage to smaller chunk sizes: less fragmentation.)
		Note: some work to be done maintaining map.  
		\param pos the position before which the new value
			is to be inserted.  
		\return iterator to newly inserted data.
	 */
	iterator
	insert(iterator pos, const value_type& v);

	/**
		Inserts a number of copies into vector.  
	 */
	void
	insert(iterator pos, const size_type n, const value_type& v);

	/**
		Inserts a copy of values from a sequence.  
	 */
	template <class InIter>
	void
	insert(iterator pos, InIter first, InIter last);


	/**
		Another bad idea.  
		Depending on position of iterator in vector chunk,
		this will likely fragment the vector
		in which it is found. 
	 */
	iterator
	erase(iterator pos);

	iterator
	erase(iterator first, iterator last);

	/**
		Can lists be swapped quickly?
		Yes, just a pointer swap in std::list::swap.
		Swap chunk sizes too?  Why not.
		\param l the list_vector to swap with.
	 */
	void
	swap(this_type& l) {
		vec_list.swap(l.vec_list);
		vec_map.swap(l.vec_map);
		// yeah, I know can also swap without intermediate...
		swap(this->chunk_size, l.chunk_size);
	}

	/**
		Clears out everything.
	 */
	void
	clear(void) {
		// really, only need to clean out everything between sentinels
		vec_list.erase(vec_list_front(), this->end_sentinel_iter());
		vec_map.clear();
	}

	/**
		Checks that this structure satisfies all invariants.  
	 */
	void
	check_invariants(void) const;

private:
	/**
		Helper class for quickly checking invariants.  
	 */
	struct list_map_checker;

public:
	ostream&
	dump_details(ostream& o) const;

};	// end class list_vector

/// Debug print.  
LIST_VECTOR_TEMPLATE_SIGNATURE
ostream&
operator << (ostream&, const typename list_vector<T,ValAlloc>::list_map_checker&);

//=============================================================================
}	// end namespace util

#endif	// __UTIL_LIST_VECTOR_H__

