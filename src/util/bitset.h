/**
	\file "util/bitset.h"
	Extended bitset.
	$Id: bitset.h,v 1.2 2005/08/08 16:51:13 fang Exp $
 */

#ifndef	__UTIL_BITSET_H__
#define	__UTIL_BITSET_H__

#include <iosfwd>
#include <bitset>
#include "util/inttypes.h"

namespace util {
//=============================================================================
/**
	bitset with extended functionality.
	TODO: add arithmetic, and relational operations.  
 */
template <size_t NB>
class bitset : public std::bitset<NB> {

};	// end class bitset

//=============================================================================
/**
	Compare != zero.
 */
template <class T>
struct any_bits {
	bool
	operator () (const T& t) const { return t; }
};	// end struct any_bits

/**
	Specialized comparison against 0 for std::bitset.
 */
template <size_t NB>
struct any_bits<std::bitset<NB> > {
	typedef	std::bitset<NB>		arg_type;

	bool
	operator () (const arg_type& t) const { return t.any(); }
};	// end struct any_bits

//-----------------------------------------------------------------------------
/**
	Compare != -1.
 */
template <class T>
struct all_bits {
	bool
	operator () (const T& t) const { return !any_bits<T>()(~t); }
};	// end struct all_bits

/**
	Specialized comparison against 0 for std::bitset.
 */
template <size_t NB>
struct all_bits<std::bitset<NB> > {
	typedef	std::bitset<NB>		arg_type;

	bool
	operator () (const arg_type& t) const { return (~t).any(); }
};	// end struct all_bits

//-----------------------------------------------------------------------------
/**
	Sets the first available bit.  
	Generalized for built-in integer types.  
 */
template <class T>
struct set_any_bit {
	/**
		\pre There is at least one unset bit.  
	 */
	T
	operator () (const T& t) const {
		return t | (t +1);
	}
};	// end struct set_any_bit

/**
	Sets the first available bit.  
	Specialized for std::bitset.
 */
template <size_t NB>
struct set_any_bit<std::bitset<NB> > {
	typedef	std::bitset<NB>		arg_type;

	/**
		\pre There is at least one unset bit.  
	 */
	arg_type
	operator () (const arg_type& t) const {
		arg_type temp(~t);
		temp[temp._Find_first()] = 0;
		return ~temp;
	}
};	// end struct all_bits

//-----------------------------------------------------------------------------
template <class T>
struct print_bits_hex {
	std::ostream&
	operator () (std::ostream&, const T&) const;
};	// end class print_bits_hex

template <>
struct print_bits_hex<uint64> {
	std::ostream&
	operator () (std::ostream&, const uint64&) const;
};	// end class print_bits_hex

/**
	Currently prints binary, not hex...
 */
template <size_t NB>
struct print_bits_hex<std::bitset<NB> > {
	typedef	std::bitset<NB>		arg_type;

	std::ostream&
	operator () (std::ostream&, const arg_type&) const;
};	// end class print_bits_hex

//=============================================================================
}	// end namespace util

#endif	// __UTIL_BITSET_H__

