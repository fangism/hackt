/**
	\file "packed_array.tcc"
	$Id: packed_array.tcc,v 1.2 2004/12/16 03:50:57 fang Exp $
 */

#ifndef	__PACKED_ARRAY_TCC__
#define	__PACKED_ARRAY_TCC__

#include <iostream>
#include <numeric>
#include <iterator>
#include "macros.h"
#include "packed_array.h"

namespace util {
#include "using_ostream.h"
using std::accumulate;
using std::ostream_iterator;
using MULTIKEY_NAMESPACE::multikey_generator;

//=============================================================================
PACKED_ARRAY_TEMPLATE_SIGNATURE
const
typename packed_array<D,T>::ones_type
packed_array<D,T>::ones;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Constructs a packed array given an array of dimensions.
	\param s the dimensions of the new array.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,T>::packed_array(const key_type& s) :
		sizes(s), values(sizes_product(s)), offset(), coeffs() {
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a packed array with index offsets in each dimension.
	\param s dimensions of the new array.
	\oaram o the offset of the new array.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,T>::packed_array(const key_type& s, const key_type& o) :
		sizes(s), values(sizes_product(s)), offset(o), coeffs() {
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a the array from which to copy values.
	\param l the lower bound of indices used to copy from a.
	\param u the upper bound of indices used to copy from a.
	\pre element-for-element, each index of l <= corresponding index in u.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,T>::packed_array(const packed_array& a, 
		const key_type& l, const key_type& u) :
		sizes((INVARIANT(l <= u), u - l + ones)), 
		values(sizes), offset(), coeffs() {
	// offset remains 0
	reset_coeffs();
	multikey_generator<D,size_t> key_gen(l, u);
	key_gen.initialize();
	INVARIANT(values.size() == sizes_product(sizes));
	register size_t i = 0;
	do {
		// write valarray directly
		values[i++] = a[key_gen++];
	} while (key_gen != key_gen.lower_corner);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,T>::~packed_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
size_t
packed_array<D,T>::sizes_product(const key_type& k) {
	return accumulate(k.begin(), k.end(), 1, std::multiplies<size_t>());
}

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
ostream&
packed_array<D,T>::dump(ostream& o) const {
	o << "packed_array: size = " << sizes <<
		", offset = " << offset << endl;
	o << "{ ";
	ostream_iterator<T> osi(o, ", ");
	copy(&values[0], &values[values.size() -1], osi);
	return o << " }" << endl;
}

//=============================================================================
}	// end namespace util

#endif	// __PACKED_ARRAY_TCC__

