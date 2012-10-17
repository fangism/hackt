/**
	\file "util/numeric/bigger_ints.hh"
	Synthesized bigger integer types.  
	$Id: bigger_ints.hh,v 1.4 2006/04/13 21:45:08 fang Exp $
 */

#ifndef	__UTIL_NUMERIC_BIGGER_INTS_H__
#define	__UTIL_NUMERIC_BIGGER_INTS_H__

#include <bitset>
#include "util/numeric/inttype_traits.hh"

namespace util {
namespace numeric {
//=============================================================================
/**
	Concatenation of two integers to form a bigger integer.  
	A poor man's big integer.  
	See also std::bitset.
	\param H must be unsigned!
 */
template <typename H>
struct cat_int {
	typedef	H			half_type;
	enum { half_size = sizeof(half_type) };
	typedef	cat_int<H>		this_type;
#if 0
	typedef	cat_int<this_type>	double_type;
#endif

	half_type		half[2];

	cat_int() { }

	cat_int(const half_type& l) {
		half[0] = l;
		half[1] = 0;
	}

	cat_int(const half_type& l, const half_type& u) {
		half[0] = l;
		half[1] = u;
	}
	~cat_int() { }

	// default assignment = operator suffices

#if 0
	// need to account for carry, punt on arithmetic operations
	this_type
	operator + (const this_type& t) const {
		return this_type(half[0] +t.half[0], half[1] +t.half[1]);
	}

	this_type
	operator - (const this_type& t) const {
		return this_type(half[0] -t.half[0], half[1] -t.half[1]);
	}

	this_type&
	operator += (const this_type& t) {
		half[0] += t.half[0];
		half[1] += t.half[1];
		return *this;
	}

	this_type&
	operator -= (const this_type& t) {
		half[0] -= t.half[0];
		half[1] -= t.half[1];
		return *this;
	}
#endif

	this_type
	operator & (const this_type& t) const {
		return this_type(half[0] &t.half[0], half[1] &t.half[1]);
	}

	this_type
	operator | (const this_type& t) const {
		return this_type(half[0] |t.half[0], half[1] |t.half[1]);
	}

	this_type&
	operator &= (const this_type& t) {
		half[0] &= t.half[0];
		half[1] &= t.half[1];
		return *this;
	}

	this_type&
	operator |= (const this_type& t) {
		half[0] |= t.half[0];
		half[1] |= t.half[1];
		return *this;
	}

	this_type
	operator << (const size_t s) const {
		return this_type(half[0] << s,
			(half[1] << s) | (half[0] >> (half_size -s)));
	}

	this_type
	operator >> (const size_t s) const {
		return this_type((half[0] >> s) | (half[1] << half_size -s),
			(half[1] >> s));
	}

	this_type&
	operator <<= (const size_t s) {
		half[1] = (half[1] << s) | (half[0] >> (half_size -s));
		half[0] <<= s;
		return *this;
	}

	this_type&
	operator >>= (const size_t s) {
		half[0] = (half[0] >> s) | (half[1] << half_size -s);
		half[1] >>= s;
		return *this;
	}

};	// end struct cat_int

//=============================================================================
// extended typedefs
// caveat: only can do bit manipulation, no arithmetic yet

#ifndef	HAVE_UINT64_TYPE
template <> struct uint_of_size<64> {	typedef	std::bitset<64>	type; };
#endif
template <> struct uint_of_size<128> {	typedef	std::bitset<128>	type; };
template <> struct uint_of_size<256> {	typedef	std::bitset<256>	type; };

#ifndef	HAVE_UINT64_TYPE
typedef	uint_of_size<64>::type	uint64;
#endif
typedef	uint_of_size<128>::type	uint128;
typedef	uint_of_size<256>::type	uint256;

#ifndef	HAVE_UINT64_TYPE
template <> struct half_type<uint64> {		typedef	uint32	type; };
#endif
template <> struct half_type<uint128> {		typedef	uint64	type; };
template <> struct half_type<uint256> {		typedef	uint128	type; };

#ifndef	HAVE_UINT64_TYPE
template <> struct double_type<uint32> {	typedef	uint64	type; };
#endif
template <> struct double_type<uint64> {	typedef	uint128	type; };
template <> struct double_type<uint128> {	typedef	uint256	type; };

//=============================================================================
}	// end namespace numeric
}	// end namespace util

#endif	// __UTIL_NUMERIC_BIGGER_INTS_H__

