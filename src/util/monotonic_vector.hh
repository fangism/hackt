/**
	\file "util/monotonic_vector.hh"
	$Id: $
 */

#ifndef	__UTIL_MONOTONIC_VECTOR_HH__
#define	__UTIL_MONOTONIC_VECTOR_HH__

#include <vector>
#include <utility>		// for std::pair
#include <algorithm>		// for std::binary_search
#include "util/macros.h"

namespace util {

/**
	A vector whose interface is set-like with the 
	invariant that values are inserted in monotonic order.
	TODO: optional operator or override to std::less.
 */
template <class T>
class monotonic_vector : protected std::vector<T> {
	typedef	std::vector<T>			impl_type;
	impl_type				values;

public:
	typedef	typename impl_type::value_type	value_type;
	// no (mutable) iterator interface
	typedef	typename impl_type::const_iterator
						const_iterator;
	typedef	typename impl_type::const_reference
						const_reference;
public:
	monotonic_vector() : values() { }
	~monotonic_vector() { }

	std::pair<const_iterator, bool>
	insert(const value_type& v) {
		typedef	std::pair<const_iterator, bool>	return_type;
		INVARIANT(this->empty() || (v >= values.back()));
		bool inserted = false;
		if (this->empty() || (v > values.back())) {
			values.push_back(v);
			inserted = true;
		}
		return return_type(--values.end(), inserted);
	}

	bool
	erase(const value_type& v) {
		if (this->size() && (values.back() == v)) {
			values.pop_back();
			return true;
		}
		return false;
	}

	const_iterator
	find(const value_type& k) const {
		return std::binary_search(this->begin(), this->end(), k);
	}

	using impl_type::empty;
	using impl_type::size;
	using impl_type::clear;

	const_iterator
	begin(void) const { return values.begin(); }

	const_iterator
	end(void) const { return values.end(); }

};	// end class monotonic_vector

}	// end namespace util

#endif	// __UTIL_MONOTONIC_VECTOR_HH__
