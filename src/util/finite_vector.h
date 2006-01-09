/**
	\file "util/finite_vector.h"
	A vector-like interface to fixed-sized storage.  
	$Id: finite_vector.h,v 1.1.2.1 2006/01/09 21:37:13 fang Exp $
 */

#ifndef	__UTIL_FINITE_VECTOR_H__
#define	__UTIL_FINITE_VECTOR_H__

#include "util/size_t.h"
// #include "util/macros.h"

namespace util {
//=============================================================================
/**
	This container is designed for fast stack allocation.  
	The internal array is entirely contained, and does not allocate
		from the heap.  
	\param T the value_type stored.
	\param N the maximum capacity.  
	TODO: decide destructor policy depending on data-type?
		For now, postpone destruction of entire elements
		until the entire array is destroyed (end-of-life).
	NOTE: this will not work with T = bool, because of references.  
	NOTE: swap is more expensive for this because all elements are swapped.
		Swap is expected to be an infrequent operation for this type.
 */
template <class T, size_t N>
class finite_vector {
public:
	typedef	T				value_type;
	typedef	T*				pointer;
	typedef T&				reference;
	typedef	const T*			const_pointer;
	typedef const T&			const_reference;
	typedef	size_t				size_type;
	typedef	pointer				iterator;
	typedef	const_pointer			const_iterator;
private:
	value_type				data[N];
	size_type				_size;
public:
	finite_vector() : _size(0) { }
	// default copy-constructor
	~finite_vector() { }

	size_type
	size(void) const { return _size; }

	size_type
	capacity(void) const { return N; }

	size_type
	max_size(void) const { return N; }

	void
	reserve(const size_type) { }

	void
	resize(const size_type) { }

	void
	push_back(const_reference v) {
		// INVARIANT(size < N);
		data[_size] = v;
		++_size;
	}

	void
	pop_back(void) {
		// INVARIANT(size);
		--_size;
	}

	reference
	front(void) { return data[0]; }

	const_reference
	front(void) const { return data[0]; }

	reference
	back(void) { return data[N-1]; }

	const_reference
	back(void) const { return data[N-1]; }

	iterator
	begin(void) { return &data[0]; }

	const_iterator
	begin(void) const { return &data[0]; }

	iterator
	end(void) { return &data[N]; }

	const_iterator
	end(void) const { return &data[N]; }

};	// end class finite_vector

//-----------------------------------------------------------------------------
/**
	The closest we can emulate to a template-typedef.  
 */
template <size_t N>
struct finite_vector_size_binder {
	template <class T>
	class type : public finite_vector<T, N> { };
};	// end struct finite_vector_size_binder

//=============================================================================
}	// end namespace util

#endif	// __UTIL_FINITE_VECTOR_H__

