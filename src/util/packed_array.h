/**
	\file "packed_array.h"
	Fake multidimensional array/block/slice, implemented as a
	specially indexed vector.  
	$Id: packed_array.h,v 1.1 2004/12/15 23:31:14 fang Exp $
 */

#ifndef	__PACKED_ARRAY_H__
#define	__PACKED_ARRAY_H__

#include <valarray>
#include "macros.h"
#include "multikey.h"

#define	PACKED_ARRAY_TEMPLATE_SIGNATURE					\
template <size_t D, class T>

namespace util {
using MULTIKEY_NAMESPACE::multikey;

//=============================================================================
template <class T>
class packed_array_base {
public:
	typedef	T			value_type;
public:

virtual	~packed_array_base() { }

};	// end class packed_array_base

//=============================================================================
/**
	Abstraction of a dense multidimensionally indexed block, 
	implemented as a valarray with dimension coefficients.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
class packed_array : public packed_array_base<T> {
private:
	typedef	packed_array_base<T>			parent_type;
	typedef	std::valarray<T>			impl_type;
public:
	typedef	T					value_type;
	typedef	multikey<D, size_t, 0>			key_type;
#if 1
	typedef	T*					pointer;
	typedef	const T*				const_pointer;
	typedef	pointer					iterator;
	typedef	const_pointer				const_iterator;
	typedef	pointer					reverse_iterator;
	typedef	const_pointer				const_reverse_iterator;
#endif
protected:
	/**
		Coefficient default to 1, because used for multiplication.  
	 */
	typedef	multikey<D-1, size_t, 1>		coeffs_type;
protected:
	impl_type					values;
	key_type					sizes;
	key_type					offset;
	/**
		Cached array of transformation coefficients
		for computing flat index.  
	 */
	coeffs_type					coeffs;
public:
	packed_array() : values(), sizes(), offset(), coeffs() { }

	explicit
	packed_array(const key_type& s);

	packed_array(const key_type& s, const key_type& o);

	// default copy constructor is fine

	~packed_array();

	void
	resize(const key_type& s);

	bool
	range_check(const key_type& k);

protected:
	void
	reset_coeffs(void);

	size_t
	key_to_index(const key_type& k) const;

public:

	T&
	operator [] (const key_type& k);

	T
	operator [] (const key_type& k) const;

	T&
	at(const key_type& k) {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	T
	at(const key_type& k) const {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

};	// end class packed_array

//=============================================================================
/**
	Specialized for bool, implemented with bitset.
 */
template <size_t D>
class packed_array<D, bool> : public packed_array_base<bool> {

};	// end class packed_array (specialized)

//=============================================================================
template <class T>
class packed_array_generic : public packed_array_base<T> {


};	// end class packed_array_generic

//=============================================================================
}	// end namespace util

#endif	// __PACKED_ARRAY_H__

