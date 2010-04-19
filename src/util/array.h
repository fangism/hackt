/**
	\file "util/array.h"
	$Id: array.h,v 1.1 2010/04/19 02:46:09 fang Exp $
 */

#ifndef	__UTIL_ARRAY_H__
#define	__UTIL_ARRAY_H__

#include <cassert>
#include <cstddef>		// for size_t
#include <iosfwd>

namespace util {

/**
	NOTE: use default construction, lazy destruction.  
	\param T the element type
	\param S size must be > 0
 */
template <class T, size_t S>
class array {
	typedef	array<T,S>		this_type;
public:
	typedef	T			value_type;
	typedef	value_type		element_type;
	typedef	T&			reference;
	typedef	const T&		const_reference;
	typedef	T*			pointer;
	typedef	const T*		const_pointer;
	typedef	pointer			iterator;
	typedef	const_pointer		const_iterator;
	typedef	size_t			size_type;
	enum { Size = S };
	// difference_type
	// no allocator type
protected:
	value_type			_value[S];
public:
	array() { }
	// default copy-ctor
	// default assignment operator
	// default destructor suffices

	// initial fill
	explicit
	array(const T&);

	// copy
	array(const T[S]);

	// is not a valid reference when empty
	const_reference
	back(void) const { return this->_value[S-1]; }

	reference
	back(void) { return this->_value[S-1]; }

	const_reference
	front(void) const { return this->_value[0]; }

	reference
	front(void) { return this->_value[0]; }

	const_reference
	operator [] (const size_t i) const { return this->_value[i]; }

	reference
	operator [] (const size_t i) { return this->_value[i]; }

	const_reference
	at(const size_t i) const {
		assert(i < this->size());
		return this->_value[i];
	}

	reference
	at(const size_t i) {
		assert(i < this->size());
		return this->_value[i];
	}

	const_iterator
	begin(void) const { return this->_value; }

	iterator
	begin(void) { return this->_value; }

	const_iterator
	end(void) const { return &this->_value[S]; }

	iterator
	end(void) { return &this->_value[S]; }

};	// end class array

template <class T, size_t S>
std::ostream&
operator << (std::ostream&, const array<T,S>&);

template <class T, size_t S>
std::istream&
operator >> (std::istream&, array<T,S>&);

}	// end namespace util

#endif	// __UTIL_ARRAY_H__

