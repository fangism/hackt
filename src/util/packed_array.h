/**
	\file "packed_array.h"
	Fake multidimensional array/block/slice, implemented as a
	specially indexed vector.  
	$Id: packed_array.h,v 1.2 2004/12/16 03:50:57 fang Exp $
 */

#ifndef	__PACKED_ARRAY_H__
#define	__PACKED_ARRAY_H__

#include <valarray>
#include <vector>
#include <iosfwd>
#include "macros.h"
#include "multikey.h"

#define	PACKED_ARRAY_TEMPLATE_SIGNATURE					\
template <size_t D, class T>

#define	PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE				\
template <size_t D>

namespace util {
using MULTIKEY_NAMESPACE::multikey;
using std::ostream;

//=============================================================================
/**
	Common abstract base class for packed array class family.
 */
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
	typedef	multikey<D, size_t, 1>			ones_type;
	typedef	T*					pointer;
	typedef	const T*				const_pointer;
	typedef	T&					reference;
	typedef	const T&				const_reference;
	typedef	pointer					iterator;
	typedef	const_pointer				const_iterator;
	typedef	pointer					reverse_iterator;
	typedef	const_pointer				const_reverse_iterator;

public:
	/// convenient array of all 1's
	static const ones_type				ones;

protected:
	/**
		Coefficient default to 1, because used for multiplication.  
	 */
	typedef	multikey<D-1, size_t, 1>		coeffs_type;
protected:
	key_type					sizes;
	impl_type					values;
	key_type					offset;
	/**
		Cached array of transformation coefficients
		for computing flat index.  
	 */
	coeffs_type					coeffs;
public:
	packed_array() : sizes(), values(), offset(), coeffs() { }

	explicit
	packed_array(const key_type& s);

	packed_array(const key_type& s, const key_type& o);

	// default copy constructor is fine

	/// ranged copy-constructor
	packed_array(const packed_array& a, 
		const key_type& l, const key_type& u);

	~packed_array();

	static
	size_t
	sizes_product(const key_type& k);

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

	reference
	operator [] (const key_type& k);

	value_type
	operator [] (const key_type& k) const;

	reference
	at(const key_type& k) {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	value_type
	at(const key_type& k) const {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	ostream&
	dump(ostream& o) const;

};	// end class packed_array

//=============================================================================
/**
	Specialized for bool, implemented with vector<bool>, 
	which, itself, is specialized.
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
class packed_array<D, bool> : public packed_array_base<bool> {
private:
	typedef	packed_array_base<bool>			parent_type;
	typedef	std::vector<bool>			impl_type;
public:
	typedef	bool					value_type;
	typedef	multikey<D, size_t, 0>			key_type;
	typedef	multikey<D, size_t, 1>			ones_type;
	typedef	typename impl_type::pointer		pointer;
	typedef	typename impl_type::const_pointer	const_pointer;
	typedef	typename impl_type::reference		reference;
	typedef	typename impl_type::const_reference	const_reference;
	typedef	typename impl_type::iterator		iterator;
	typedef	typename impl_type::const_iterator	const_iterator;
	typedef	typename impl_type::reverse_iterator	reverse_iterator;
	typedef	typename impl_type::const_reverse_iterator
							const_reverse_iterator;

public:
	/// convenient array of all 1's
	static const ones_type				ones;

protected:
	/**
		Coefficient default to 1, because used for multiplication.  
	 */
	typedef	multikey<D-1, size_t, 1>		coeffs_type;
protected:
	key_type					sizes;
	impl_type					values;
	key_type					offset;
	/**
		Cached array of transformation coefficients
		for computing flat index.  
	 */
	coeffs_type					coeffs;
public:
	packed_array() : sizes(), values(), offset(), coeffs() { }

	explicit
	packed_array(const key_type& s);

	packed_array(const key_type& s, const key_type& o);

	// default copy constructor is fine

	/// ranged copy-constructor
	packed_array(const packed_array& a, 
		const key_type& l, const key_type& u);

	~packed_array();

	static
	size_t
	sizes_product(const key_type& k);

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

	reference
	operator [] (const key_type& k);

	value_type
	operator [] (const key_type& k) const;

	reference
	at(const key_type& k) {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	value_type
	at(const key_type& k) const {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	ostream&
	dump(ostream& o) const;

};	// end class packed_array (specialized)

//=============================================================================
#if 0
/**
	Variable dimensions array.  
 */
template <class T>
class packed_array_generic : public packed_array_base<T> {


};	// end class packed_array_generic
#endif

//=============================================================================
}	// end namespace util

#endif	// __PACKED_ARRAY_H__

