/**
	\file "packed_array.h"
	Fake multidimensional array/block/slice, implemented as a
	specially indexed vector.  
	$Id: packed_array.h,v 1.9 2005/02/27 22:54:26 fang Exp $
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
template <size_t D, class K>

namespace util {
using util::multikey;
using std::istream;
using std::ostream;

//=============================================================================
/**
	Abstraction of a dense multidimensionally indexed block, 
	implemented as a valarray with dimension coefficients.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
class packed_array {
private:
	typedef	std::valarray<T>			impl_type;
public:
	typedef	K					index_type;
	typedef	T					value_type;
	typedef	multikey<D,K>				key_type;
	typedef	key_type				zeros_type;
	typedef	key_type				ones_type;
	typedef	multikey_generator<D,K>			key_generator_type;
	typedef	T*					pointer;
	typedef	const T*				const_pointer;
	typedef	T&					reference;
	typedef	const T&				const_reference;
	typedef	pointer					iterator;
	typedef	const_pointer				const_iterator;
	typedef	std::reverse_iterator<pointer>		reverse_iterator;
	typedef	std::reverse_iterator<const_pointer>	const_reverse_iterator;
	typedef	size_t					size_type;

public:
	/// convenient array of all 1's
	static const ones_type				ones;

protected:
	/**
		Coefficient default to 1, because used for multiplication.  
	 */
	typedef	multikey<D-1,K>				coeffs_type;
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
	index_type
	sizes_product(const key_type& k);

	void
	resize(const key_type& s);

	bool
	range_check(const key_type& k) const;

protected:
	void
	reset_coeffs(void);

	index_type
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
class packed_array<D, K, bool> {
private:
	typedef	std::vector<bool>			impl_type;
public:
	typedef	K					index_type;
	typedef	bool					value_type;
	typedef	multikey<D,K>				key_type;
	typedef	key_type				ones_type;
	typedef	key_type				zeros_type;
	typedef	multikey_generator<D,K>			key_generator_type;
	typedef	typename impl_type::pointer		pointer;
	typedef	typename impl_type::const_pointer	const_pointer;
	typedef	typename impl_type::reference		reference;
	typedef	typename impl_type::const_reference	const_reference;
	typedef	typename impl_type::iterator		iterator;
	typedef	typename impl_type::const_iterator	const_iterator;
	typedef	typename impl_type::reverse_iterator	reverse_iterator;
	typedef	typename impl_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	size_t					size_type;
public:
	/// convenient array of all 1's
	static const ones_type				ones;

protected:
	/**
		Coefficient default to 1, because used for multiplication.  
	 */
	typedef	multikey<D-1,K>				coeffs_type;
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
	index_type
	sizes_product(const key_type& k);

	void
	resize(const key_type& s);

	bool
	range_check(const key_type& k) const;

protected:
	void
	reset_coeffs(void);

	index_type
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
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
class packed_array_generic {
private:
	typedef	std::vector<T>				impl_type;
	typedef	packed_array_generic<K,T>		this_type;
public:
	typedef	K					index_type;
	typedef	T					value_type;
	typedef	util::multikey_generic<K>		key_type;
	typedef	key_type				zeros_type;
	typedef	key_type				ones_type;
	typedef	multikey_generator_generic<K>		key_generator_type;
	typedef	typename impl_type::pointer		pointer;
	typedef	typename impl_type::const_pointer	const_pointer;
	typedef	typename impl_type::reference		reference;
	typedef	typename impl_type::const_reference	const_reference;
	typedef	typename impl_type::iterator		iterator;
	typedef	typename impl_type::const_iterator	const_iterator;
	typedef	typename impl_type::reverse_iterator	reverse_iterator;
	typedef	typename impl_type::const_reverse_iterator
							const_reverse_iterator;
	typedef	size_t					size_type;

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
	size_type					dim;
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

	packed_array_generic(const size_type d);

	// also make generic sequence-based constructors

	explicit
	packed_array_generic(const key_type& s);

	packed_array_generic(const key_type& s, const key_type& o);

	// default copy constructor is fine

	/// ranged copy-constructor
	packed_array_generic(const packed_array_generic& a, 
		const key_type& l, const key_type& u);

	~packed_array_generic();

	size_type
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
	index_type
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

	index_type
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

