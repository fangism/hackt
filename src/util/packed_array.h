/**
	\file "packed_array.h"
	Fake multidimensional array/block/slice, implemented as a
	specially indexed vector.  
	$Id: packed_array.h,v 1.8.2.1.2.2 2005/02/21 19:48:10 fang Exp $
 */

#ifndef	__UTIL_PACKED_ARRAY_H__
#define	__UTIL_PACKED_ARRAY_H__

#include "packed_array_fwd.h"

#include <valarray>
#include <vector>
#include <iosfwd>
#include "macros.h"
#include "multikey.h"

#define	PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE				\
template <size_t D>

namespace util {
using util::multikey;
using std::istream;
using std::ostream;

//=============================================================================
#if USE_PACKED_ARRAY_BASE
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
#endif

//=============================================================================
/**
	Abstraction of a dense multidimensionally indexed block, 
	implemented as a valarray with dimension coefficients.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
class packed_array
#if USE_PACKED_ARRAY_BASE
	: public packed_array_base<T>
#endif
{
private:
#if USE_PACKED_ARRAY_BASE
	typedef	packed_array_base<T>			parent_type;
#endif
	typedef	std::valarray<T>			impl_type;
public:
	typedef	T					value_type;
	typedef	multikey<D, size_t>			key_type;
	typedef	key_type				zeros_type;
	typedef	key_type				ones_type;
	typedef	multikey_generator<D, size_t>		key_generator_type;
	typedef	T*					pointer;
	typedef	const T*				const_pointer;
	typedef	T&					reference;
	typedef	const T&				const_reference;
	typedef	pointer					iterator;
	typedef	const_pointer				const_iterator;
	typedef	std::reverse_iterator<pointer>		reverse_iterator;
	typedef	std::reverse_iterator<const_pointer>	const_reverse_iterator;

public:
	/// convenient array of all 1's
	static const ones_type				ones;

protected:
	/**
		Coefficient default to 1, because used for multiplication.  
	 */
	typedef	multikey<D-1, size_t>			coeffs_type;
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
	packed_array() : sizes(), values(), offset(), coeffs(1) { }

	explicit
	packed_array(const key_type& s);

	packed_array(const key_type& s, const key_type& o);

	// default copy constructor is fine

	/// ranged copy-constructor
	packed_array(const packed_array& a, 
		const key_type& l, const key_type& u);

	~packed_array();

	iterator
	begin(void) { return &values[0]; }

	iterator
	end(void) { return &values[values.size()]; }

	const_iterator
	begin(void) const { return &values[0]; }

	const_iterator
	end(void) const { return &values[values.size()]; }

	reverse_iterator
	rbegin(void) { return &values[values.size()]; }

	reverse_iterator
	rend(void) { return &values[0]; }

	const_reverse_iterator
	rbegin(void) const { return &values[values.size()]; }

	const_reverse_iterator
	rend(void) const { return &values[0]; }

	key_type
	first_key(void) const;

	key_type
	last_key(void) const;

	static
	size_t
	sizes_product(const key_type& k);

	void
	resize(const key_type& s);

	bool
	range_check(const key_type& k) const;

protected:
	void
	reset_coeffs(void);

	size_t
	key_to_index(const key_type& k) const;

public:

	key_type
	size(void) const { return sizes; }

	bool
	empty(void) const { return !values.size(); }

	reference
	operator [] (const key_type& k);

	const_reference
	operator [] (const key_type& k) const;

	reference
	at(const key_type& k) {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	const_reference
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
class packed_array<D, bool>
#if USE_PACKED_ARRAY_BASE
	: public packed_array_base<bool>
#endif
{
private:
#if USE_PACKED_ARRAY_BASE
	typedef	packed_array_base<bool>			parent_type;
#endif
	typedef	std::vector<bool>			impl_type;
public:
	typedef	bool					value_type;
	typedef	multikey<D, size_t>			key_type;
	typedef	key_type				ones_type;
	typedef	key_type				zeros_type;
	typedef	multikey_generator<D, size_t>		key_generator_type;
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
	typedef	multikey<D-1, size_t>			coeffs_type;
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
	packed_array() : sizes(), values(), offset(), coeffs(1) { }

	explicit
	packed_array(const key_type& s);

	packed_array(const key_type& s, const key_type& o);

	// default copy constructor is fine

	/// ranged copy-constructor
	packed_array(const packed_array& a, 
		const key_type& l, const key_type& u);

	~packed_array();

	iterator
	begin(void) { return values.begin(); }

	iterator
	end(void) { return values.end(); }

	const_iterator
	begin(void) const { return values.begin(); }

	const_iterator
	end(void) const { return values.end(); }

	reverse_iterator
	rbegin(void) { return values.rbegin(); }

	reverse_iterator
	rend(void) { return values.rend(); }

	const_reverse_iterator
	rbegin(void) const { return values.rbegin(); }

	const_reverse_iterator
	rend(void) const { return values.rend(); }

	key_type
	first_key(void) const;

	key_type
	last_key(void) const;

	static
	size_t
	sizes_product(const key_type& k);

	void
	resize(const key_type& s);

	bool
	range_check(const key_type& k) const;

protected:
	void
	reset_coeffs(void);

	size_t
	key_to_index(const key_type& k) const;

public:

	key_type
	size(void) const { return sizes; }

	bool
	empty(void) const { return !values.size(); }

	reference
	operator [] (const key_type& k);

	const_reference
	operator [] (const key_type& k) const;

	reference
	at(const key_type& k) {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	const_reference
	at(const key_type& k) const {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	ostream&
	dump(ostream& o) const;

};	// end class packed_array (specialized)

//=============================================================================
/**
	Variable dimensions array.  
	When you're too lazy to make dimension-specific arrays.  
	This is however, generally "unsafer" and is more prone to misuse, 
	unless the user takes the necessary extra precautions.  
	Key type is polymorphic, any sequence of size_t will do.  
	TODO: replace size_t with generic index type, K, like with multikey.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
class packed_array_generic
#if USE_PACKED_ARRAY_BASE
	: public packed_array_base<T>
#endif
{
private:
#if USE_PACKED_ARRAY_BASE
	typedef	packed_array_base<T>			parent_type;
#endif
	typedef	std::vector<T>				impl_type;
	typedef	packed_array_generic<T>			this_type;
public:
	typedef	T					value_type;
	typedef	util::multikey_generic<size_t>		key_type;
	typedef	key_type				zeros_type;
	typedef	key_type				ones_type;
	typedef	multikey_generator_generic<size_t>	key_generator_type;
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
	static
	ones_type
	ones(const size_t d);

protected:
	/**
		Coefficient default to 1, because used for multiplication.  
	 */
	typedef	key_type				coeffs_type;
protected:
	size_t						dim;
	key_type					sizes;
	impl_type					values;
	key_type					offset;
	/**
		Cached array of transformation coefficients
		for computing flat index.  
	 */
	coeffs_type					coeffs;
public:
	packed_array_generic() :
		dim(0), sizes(0), values(), offset(0), coeffs(0) { }

	packed_array_generic(const size_t d);

	// also make generic sequence-based constructors

	explicit
	packed_array_generic(const key_type& s);

	packed_array_generic(const key_type& s, const key_type& o);

	// default copy constructor is fine

	/// ranged copy-constructor
	packed_array_generic(const packed_array_generic& a, 
		const key_type& l, const key_type& u);

	~packed_array_generic();

	size_t
	dimensions(void) const { return dim; }

	iterator
	begin(void) { return values.begin(); }

	iterator
	end(void) { return values.end(); }

	const_iterator
	begin(void) const { return values.begin(); }

	const_iterator
	end(void) const { return values.end(); }

	reverse_iterator
	rbegin(void) { return values.rbegin(); }

	reverse_iterator
	rend(void) { return values.rend(); }

	const_reverse_iterator
	rbegin(void) const { return values.rbegin(); }

	const_reverse_iterator
	rend(void) const { return values.rend(); }

	key_type
	first_key(void) const;

	key_type
	last_key(void) const;

	static
	size_t
	sizes_product(const key_type& k);

	void
	resize(void);

	void
	resize(const key_type& s);

#if 0
	void
	resize_bounds(const key_type& l, const key_type& u);
#endif

	bool
	range_check(const key_type& k) const;

protected:
	void
	reset_coeffs(void);

	size_t
	key_to_index(const key_type& k) const;

public:

	key_type
	size(void) const { return sizes; }

	bool
	empty(void) const { return !values.size(); }

	reference
	operator [] (const key_type& k);

	const_reference
	operator [] (const key_type& k) const;

	reference
	at(const key_type& k) {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	const_reference
	at(const key_type& k) const {
		INVARIANT(range_check(k));
		return (*this)[k];
	}

	bool
	operator == (const this_type& ) const;

	bool
	operator != (const this_type& a) const {
		return !(*this == a);
	}

	ostream&
	dump(ostream& o) const;

	ostream&
	write(ostream& o) const;

	istream&
	read(istream& i);

};	// end class packed_array_generic

//=============================================================================
}	// end namespace util

#endif	// __UTIL_PACKED_ARRAY_H__

