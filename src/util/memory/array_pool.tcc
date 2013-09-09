/**
	\file "util/memory/array_pool.tcc"
 */
#ifndef	__UTIL_MEMORY_ARRAY_POOL_TCC__
#define	__UTIL_MEMORY_ARRAY_POOL_TCC__

#include <iostream>
#include <iterator>
#include <numeric>
#include "util/memory/array_pool.hh"

// debug switch defined in this corresponding header file
#if DEBUG_ARRAY_POOL_ALLOC
#define	ENABLE_STACKTRACE			1
#endif

#include "util/memory/index_pool.tcc"
#include "util/stacktrace.hh"

namespace util {
namespace memory {
#include "util/using_ostream.hh"

//=============================================================================
// class array_pool method definitions
/**
	This always reserves the 0th entry as an invalid entry.  
	Thus, 0 should never be in the freelist.  
 */
template <class T, class F>
array_pool<T,F>::array_pool() : _index_pool(), free_indices() {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("this @0x" << this << endl);
	const index_type zero __ATTRIBUTE_UNUSED__ =
		_index_pool.allocate();
	INVARIANT(!zero);
	// never add 0 to the free-list
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T, class F>
array_pool<T,F>::~array_pool() {
	STACKTRACE_VERBOSE;
	INVARIANT(this->check_valid_empty());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Strict paranoia checking.  
	All resources returned properly to the pool, free-list
		must be as big as the pool (minus sentinel).  
	Note: for 2^k-bit architectures, this can easily overflow
		if the free-list size, m, exceeds a number.
		Overflow condition: m*(m+1)/2 > 2^k
		Could be off by a sign bit, due to unsigned right shift!
		Test case: quarantine/set-assert-02.prsimckpttest
 */
template <class T, class F>
bool
array_pool<T,F>::check_valid_empty(void) const {
	bool die = false;
	const size_t p = _index_pool.size() -1;
	const size_t m = free_indices.size();
	if (p != m) {
		cerr << "FATAL: event pool size is " << p <<
			" while free-list size is " << m << endl;
		die = true;
	} else {
		const size_t s = std::accumulate(
			free_indices.begin(), free_indices.end(), size_t(0));
			// explicit type-spec to prevent overflow
#if 0
		const size_t expect_sum = (m*(m+1))>>1; // triangular sum
#else
		// overflow-sign safe version, halve the even multiplicand
		const size_t expect_sum = (m&1) ? m*((m+1)>>1) : (m>>1)*(m+1);
		// result may overflow, but we just want modulo-2^k to match
#endif
		if (s != expect_sum) {
			cerr << "FATAL: event pool free list sum "
				"is not what\'s expected!" << endl;
			cerr << "expected: " << expect_sum << ", but got: " <<
				s << ", difference: " << expect_sum -s << endl;
			die = true;
		}
	}
	if (die) {
		std::ostream_iterator<size_t> osi(cerr, ",");
		cerr << "free-list: ";
		copy(free_indices.begin(), free_indices.end(), osi);
		cerr << endl;
	}
	return !die;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T, class F>
void
array_pool<T,F>::clear(void) {
	free_indices.clear();
	_index_pool.clear();
	const index_type zero __ATTRIBUTE_UNUSED__ =
		_index_pool.allocate();
	INVARIANT(!zero);
	// never add 0 to the free-list
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if DEBUG_ARRAY_POOL_ALLOC
/**
	Keep this consistent with the inline definition.  
	\return index to new event entry, never 0.
 */
template <class T, class F>
typename array_pool<T,F>::index_type
array_pool<T,F>::allocate(const value_type& e) {
	STACKTRACE_VERBOSE;
	if (UNLIKELY(free_indices.empty())) {   // UNLIKELY
		const index_type ret = _index_pool.size();
		_index_pool.allocate(e); // will realloc
		INVARIANT(ret);
		STACKTRACE_INDENT_PRINT("allocating entry: " << ret << endl);
		return ret;
	} else {                        // LIKELY
		const index_type ret = free_list_acquire(free_indices);
		_index_pool[ret] = e;
	STACKTRACE_INDENT_PRINT("allocating entry: " << ret << endl);
		INVARIANT(ret);
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i event index to reclaim, never 0.
 */
template <class T, class F>
void
array_pool<T,F>::deallocate(const index_type i) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("deallocating entry: " << i << endl);
	INVARIANT(i);
	free_list_release(free_indices, i);
}
#endif  

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_ARRAY_POOL_TCC__
