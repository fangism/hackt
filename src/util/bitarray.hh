/**
	\file "util/bitarray.hh"
	A fixed-size bitset implementation.
	Applications: efficient set operations for graph algorithms.  
	Related:
		util/bitset (extends std::bitset slightly)
		util/bitvector (forthcoming, the flexible size version)
	$Id: bitarray.hh,v 1.1 2010/04/19 02:46:12 fang Exp $
 */

#ifndef	__UTIL_BITARRAY_H__
#define	__UTIL_BITARRAY_H__

#include "util/array.hh"		// underlying container

namespace util {

/**
	The advantage of using a fixed-size array:
	no dynamic memory allocation, all stack-based memory.
	TODO: arithmetic operations
	TODO: auto-vectorize!!!
 */
template <size_t L>
class bitarray : protected array<size_t, L> {
	typedef	bitarray<L>		this_type;
	typedef	array<size_t, L>	impl_type;
	/**
		For now, we fix the underlying integr type to the natural
		machine-integer as determined by the compiler.
	 */
	typedef	size_t			limb_type;
public:
	enum {
		limbs		= L,
		max_bits	= limbs << 3
	};

	// TODO: provide some ctors for set operations
	// as an alternative to return-by-value implementations
	// benefit: eliminate a stack-move

	// default ctor (zero-fill)
	bitarray();

	// default copy-ctor
	// default dtor (POD)

	size_t
	size(void) const { return max_bits; }

	bool
	operator < (const this_type& r) const;

	bool
	operator == (const this_type& r) const;

	this_type&
	operator &= (const this_type& r);

	this_type&
	operator |= (const this_type& r);

	this_type&
	operator ^= (const this_type& r);

	this_type
	operator ~ () const;

};	// end class bitarray

}	// end namespace util

#endif	// __UTIL_BITARRAY_H__

