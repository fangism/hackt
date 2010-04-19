/**
	\file "util/bitarray.tcc"
	A fixed-size bitset implementation.
	$Id: bitarray.tcc,v 1.1 2010/04/19 02:46:12 fang Exp $
 */

#ifndef	__UTIL_BITARRAY_TCC__
#define	__UTIL_BITARRAY_TCC__

#include <algorithm>
#include "util/bitarray.h"
#include "util/array.tcc"		// underlying container
#include "util/bitmanip.h"		// for low-level bitwise operations
#include "util/bitwise_functional.h"

namespace util {
//=============================================================================
// class bitarray method definitions
template <size_t L>
bitarray<L>::
	bitarray() : impl_type() {
		std::fill(this->begin(), this->end(), limb_type(0));
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t L>
bool
bitarray<L>::operator < (const this_type& r) const {
// reverse_iterators b/c the MSBs reside at the *higher* indices
	return std::lexicographical_compare(
		this->rbegin(), this->rend(), r.rbegin(), r.rend());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t L>
bool
bitarray<L>::operator == (const this_type& r) const {
	return std::equal(this->begin(), this->end(), r.begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t L>
bitarray<L>&
bitarray<L>::operator &= (const this_type& r) {
	std::transform(this->begin(), this->end(), r.begin(),
		this->begin(), bitwise_and<limb_type>());
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t L>
bitarray<L>&
bitarray<L>::operator |= (const this_type& r) {
	std::transform(this->begin(), this->end(), r.begin(),
		this->begin(), bitwise_or<limb_type>());
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t L>
bitarray<L>&
bitarray<L>::operator ^= (const this_type& r) {
	std::transform(this->begin(), this->end(), r.begin(),
		this->begin(), bitwise_xor<limb_type>());
	return *this;

}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t L>
bitarray<L>
bitarray<L>::operator ~ () const {
	this_type ret;
	std::transform(this->begin(), this->end(), ret.begin(),
		bitwise_not<limb_type>());
	return ret;
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_BITARRAY_TCC__

