/**
	\file "packed_array.tcc"
	$Id: packed_array.tcc,v 1.1 2004/12/15 23:31:14 fang Exp $
 */

#ifndef	__PACKED_ARRAY_TCC__
#define	__PACKED_ARRAY_TCC__

#include <numeric>
#include "macros.h"
#include "packed_array.h"

namespace util {
using std::accumulate;

//=============================================================================
/**
	Constructs a packed array given an array of dimensions.
	\param s the dimensions of the new array.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,T>::packed_array(const key_type& s) :
		values(accumulate(s.begin(), s.end(), 1,
			std::multiplies<size_t>())),
		sizes(s), offset(), coeffs() {
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a packed array with index offsets in each dimension.
	\param s dimensions of the new array.
	\oaram o the offset of the nes array.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,T>::packed_array(const key_type& s, const key_type& o) :
		values(accumulate(s.begin(), s.end(), 1,
			std::multiplies<size_t>())),
		sizes(s), offset(o), coeffs() {
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,T>::~packed_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caches the coefficients used in computing the internal index.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
void
packed_array<D,T>::reset_coeffs(void) {
	size_t i = 1;	// skip first
	for ( ; i < D; i++) {
		size_t j = 0;
		for ( ; j < i; j++)
			coeffs[j] *= sizes[i];
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Since key_type uses size_t (unsigned) we only need to check
	upper bound.  
	\return true if index key is valid.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
bool
packed_array<D,T>::range_check(const key_type& k) {
	size_t i = 0;
	for ( ; i < D; i++)
		if (k[i] -offset[i] >= sizes[i])
			return false;
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does not range-check.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
size_t
packed_array<D,T>::key_to_index(const key_type& k) const {
	key_type diff;
	std::transform(k.begin(), k.end(), offset.begin(), 
		diff.begin(), std::minus<size_t>());
	return std::inner_product(diff.begin(), --diff.end(), coeffs.begin(), 
		diff.back());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
T&
packed_array<D,T>::operator [] (const key_type& k) {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
T
packed_array<D,T>::operator [] (const key_type& k) const {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
void
packed_array<D,T>::resize(const key_type& s) {
	values.resize(accumulate(s.begin(), s.end(), 1,
		std::multiplies<size_t>()));
	sizes = s;
	reset_coeffs();
}


//=============================================================================
}	// end namespace util

#endif	// __PACKED_ARRAY_TCC__

