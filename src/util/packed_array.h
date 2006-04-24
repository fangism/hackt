/**
	\file "util/packed_array.h"
	Fake multidimensional array/block/slice, implemented as a
	specially indexed vector.  
	$Id: packed_array.h,v 1.11.74.1 2006/04/24 05:43:05 fang Exp $
 */

#ifndef	__UTIL_PACKED_ARRAY_H__
#define	__UTIL_PACKED_ARRAY_H__

#include "util/packed_array_fwd.h"

#include <valarray>
#include <vector>
#include <iosfwd>
#include "util/macros.h"
#include "util/multikey.h"

#define	PACKED_ARRAY_CLASS						\
packed_array<D,K,T>

#define	PACKED_ARRAY_GENERIC_CLASS					\
packed_array_generic<K,T>

#if 0
#define	PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE				\
template <size_t D, class K>

#define	PACKED_BOOL_ARRAY_CLASS						\
packed_array<D,K,bool>
#endif

namespace util {
using std::istream;
using std::ostream;

//=============================================================================
/**
	Default internal implementation for packed arrays.  
 */
template <class T>
struct packed_array_implementation {
	typedef	std::valarray<T>			type;
	typedef	T*					pointer;
	typedef	const T*				const_pointer;
	typedef	T&					reference;
	typedef	const T&				const_reference;
	typedef	pointer					iterator;
	typedef	const_pointer				const_iterator;
	typedef	std::reverse_iterator<pointer>		reverse_iterator;
	typedef	std::reverse_iterator<const_pointer>	const_reverse_iterator;
};	// end class packed_array_implementation

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialized implementaiton for packed array of bools. 
 */
template <>
struct packed_array_implementation<bool> {
	typedef	std::vector<bool>			type;
	typedef	type::pointer				pointer;
	typedef	type::const_pointer			const_pointer;
	typedef	type::reference				reference;
	typedef	type::const_reference			const_reference;
	typedef	type::iterator				iterator;
	typedef	type::const_iterator			const_iterator;
	typedef	type::reverse_iterator			reverse_iterator;
	typedef	type::const_reverse_iterator		const_reverse_iterator;
};	// end class packed_array_implementation (specialized)

//=============================================================================
/**
	Abstraction of a dense multidimensionally indexed block, 
	implemented as a valarray with dimension coefficients.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
class packed_array {
private:
	typedef	packed_array_implementation<T>		impl_policy;
	typedef	typename impl_policy::type		impl_type;
	typedef	PACKED_ARRAY_CLASS			this_type;
public:
	typedef	K					index_type;
	typedef	T					value_type;
	typedef	multikey<D,K>				key_type;
	typedef	key_type				zeros_type;
	typedef	key_type				ones_type;
	typedef	multikey_generator<D,K>			key_generator_type;
	typedef	typename impl_policy::pointer		pointer;
	typedef	typename impl_policy::const_pointer	const_pointer;
	typedef	typename impl_policy::reference		reference;
	typedef	typename impl_policy::const_reference	const_reference;
	typedef	typename impl_policy::iterator		iterator;
	typedef	typename impl_policy::const_iterator	const_iterator;
	typedef	typename impl_policy::reverse_iterator	reverse_iterator;
	typedef	typename impl_policy::const_reverse_iterator
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

	reference
	front(void) { return values[0]; }

	const_reference
	front(void) const { return values[0]; }

	reference
	back(void) { return values[values.size() -1]; }

	const_reference
	back(void) const { return values[values.size() -1]; }

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

	ostream&
	dump_values(ostream& o) const;

private:
	void
	dump_slice(ostream&, const size_type d, const size_type s) const;

};	// end class packed_array

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

	reference
	front(void) { return values.front(); }

	const_reference
	front(void) const { return values.front(); }

	reference
	back(void) { return values.back(); }

	const_reference
	back(void) const { return values.back(); }

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

	// dumps values only, without coefficients and all...
	ostream&
	dump_values(ostream& o) const;

	ostream&
	write(ostream& o) const;

	istream&
	read(istream& i);

private:
	void
	dump_slice(ostream&, const size_type d, const size_type s) const;

};	// end class packed_array_generic

//=============================================================================
}	// end namespace util

#endif	// __UTIL_PACKED_ARRAY_H__

