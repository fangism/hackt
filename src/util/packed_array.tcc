/**
	\file "packed_array.tcc"
	$Id: packed_array.tcc,v 1.5 2004/12/20 23:21:16 fang Exp $
 */

#ifndef	__PACKED_ARRAY_TCC__
#define	__PACKED_ARRAY_TCC__

#include <iostream>
#include <numeric>
#include <iterator>
#include "macros.h"
#include "packed_array.h"
#include "multikey.tcc"

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
		values(sizes_product(sizes)), offset(), coeffs() {
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
/**
	\return the index of the first entry, which is the offset. 
	Useful for constructing the multikey_generator.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,T>::key_type
packed_array<D,T>::first_key(void) const {
	return offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,T>::key_type
packed_array<D,T>::last_key(void) const {
	return sizes +offset -ones;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function to compute the size based on dimensions.  
 */
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
	coeffs = ones;
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
packed_array<D,T>::range_check(const key_type& k) const {
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
	const key_type diff(k -offset);
//	const size_t* diff_last = diff.end();
//	--diff_last;			// this is ok
//	const typename key_type::const_iterator
//		diff_last(--diff.end());	// doesn't like this
	return std::inner_product(diff.begin(), &diff.back(), coeffs.begin(), 
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
const T&
packed_array<D,T>::operator [] (const key_type& k) const {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
void
packed_array<D,T>::resize(const key_type& s) {
	values.resize(sizes_product(s));
	sizes = s;
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
ostream&
packed_array<D,T>::dump(ostream& o) const {
	o << "packed_array: size = " << sizes <<
		", offset = " << offset << ", coeffs = " << coeffs << endl;
	o << "{ ";
	ostream_iterator<T> osi(o, ", ");
	copy(begin(), end(), osi);
	return o << " }" << endl;
}

//=============================================================================
// class packed_array method definitions (specialized for bool)

PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
const
typename packed_array<D,bool>::ones_type
packed_array<D,bool>::ones;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Constructs a packed array given an array of dimensions.
	\param s the dimensions of the new array.
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,bool>::packed_array(const key_type& s) :
		sizes(s), values(sizes_product(s)), offset(), coeffs() {
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a packed array with index offsets in each dimension.
	\param s dimensions of the new array.
	\oaram o the offset of the new array.
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,bool>::packed_array(const key_type& s, const key_type& o) :
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
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,bool>::packed_array(const packed_array& a, 
		const key_type& l, const key_type& u) :
		sizes((INVARIANT(l <= u), u - l + ones)), 
		values(sizes_product(sizes)), offset(), coeffs() {
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
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,bool>::~packed_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the index of the first entry, which is the offset. 
	Useful for constructing the multikey_generator.  
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,bool>::key_type
packed_array<D,bool>::first_key(void) const {
	return offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,bool>::key_type
packed_array<D,bool>::last_key(void) const {
	return sizes +offset -ones;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function to compute the size based on dimensions.  
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
size_t
packed_array<D,bool>::sizes_product(const key_type& k) {
	return accumulate(k.begin(), k.end(), 1, std::multiplies<size_t>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caches the coefficients used in computing the internal index.  
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
void
packed_array<D,bool>::reset_coeffs(void) {
	coeffs = ones;
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
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
bool
packed_array<D,bool>::range_check(const key_type& k) const {
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
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
size_t
packed_array<D,bool>::key_to_index(const key_type& k) const {
	const key_type diff(k -offset);
//	const size_t* diff_last = diff.end();
//	--diff_last;			// this is ok
//	const typename key_type::const_iterator
//		diff_last(--diff.end());	// doesn't like this
	return std::inner_product(diff.begin(), &diff.back(), coeffs.begin(), 
		diff.back());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,bool>::reference
packed_array<D,bool>::operator [] (const key_type& k) {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,bool>::const_reference
packed_array<D,bool>::operator [] (const key_type& k) const {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
void
packed_array<D,bool>::resize(const key_type& s) {
	values.resize(sizes_product(s));
	sizes = s;
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
ostream&
packed_array<D,bool>::dump(ostream& o) const {
	o << "packed_array: size = " << sizes <<
		", offset = " << offset << ", coeffs = " << coeffs << endl;
	o << "{ ";
	ostream_iterator<bool> osi(o, ", ");
	copy(begin(), end(), osi);
	return o << " }" << endl;
}

//=============================================================================
#if 1
// not ready yet
// class packed_array_generic method definitions

/**
	\param d the dimensions of the ones-vector.
	\return vector of d 1's.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<T>::ones_type
packed_array_generic<T>::ones(const size_t d) {
	return ones_type(d, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Establishes the dimensionality of the array.  
	\param d the number of dimensions.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
packed_array_generic<T>::packed_array_generic(const size_t d) :
		dim(d), sizes(d, 0), values(), offset(d, 0), coeffs(d-1, 1) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Constructs a packed array given an array of dimensions.
	\param s the dimensions of the new array.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
packed_array_generic<T>::packed_array_generic(const key_type& s) :
		dim(s.size()), sizes(s), 
		values(sizes_product(s)), offset(dim, 0), coeffs(dim-1, 1) {
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a packed array with index offsets in each dimension.
	\param s dimensions of the new array.
	\oaram o the offset of the new array.
	\pre s and o have the same size.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
packed_array_generic<T>::packed_array_generic(
		const key_type& s, const key_type& o) :
		dim(s.size()), sizes(s), 
		values(sizes_product(s)), offset(o), coeffs(dim-1, 1) {
	INVARIANT(dim == o.size());
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a the array from which to copy values.
	\param l the lower bound of indices used to copy from a.
	\param u the upper bound of indices used to copy from a.
	\pre element-for-element, each index of l <= corresponding index in u.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
packed_array_generic<T>::packed_array_generic(
		const packed_array_generic& a, 
		const key_type& l, const key_type& u) :
		dim(l.size()), 
		sizes((INVARIANT(dim == u.size()),
			INVARIANT(l <= u),	// uses lexicographical_compare
			u - l + key_type(dim, 1))), 
		values(sizes_product(sizes)), offset(dim, 0), coeffs(dim-1, 1) {
	// offset remains 0
	reset_coeffs();
	multikey_generator_generic<size_t> key_gen(l, u);
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
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
packed_array_generic<T>::~packed_array_generic() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the index of the first entry, which is the offset. 
	Useful for constructing the multikey_generator.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<T>::key_type
packed_array_generic<T>::first_key(void) const {
	return offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<T>::key_type
packed_array_generic<T>::last_key(void) const {
	return sizes +offset -key_type(dim, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function to compute the size based on dimensions.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
size_t
packed_array_generic<T>::sizes_product(const key_type& k) {
	return accumulate(k.begin(), k.end(), 1, std::multiplies<size_t>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caches the coefficients used in computing the internal index.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
packed_array_generic<T>::reset_coeffs(void) {
	coeffs = key_type(dim, 1);
	size_t i = 1;	// skip first
	for ( ; i < dim; i++) {
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
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
bool
packed_array_generic<T>::range_check(const key_type& k) const {
	size_t i = 0;
	for ( ; i < dim; i++) {
		const size_t k_diff = k[i] -offset[i];
		if (k_diff >= sizes[i]) {
#if 0
			cerr << "i=" << i << ", " <<
				"diff = " << k_diff <<
				", offset = " << offset[i] << endl;
#endif
			return false;
		}
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does not range-check.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
size_t
packed_array_generic<T>::key_to_index(const key_type& k) const {
	const key_type diff(k -offset);
//	const size_t* diff_last = diff.end();
//	--diff_last;			// this is ok
//	const typename key_type::const_iterator
//		diff_last(--diff.end());	// doesn't like this
	return std::inner_product(diff.begin(), &diff.back(), coeffs.begin(), 
		diff.back());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<T>::reference
packed_array_generic<T>::operator [] (const key_type& k) {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<T>::const_reference
packed_array_generic<T>::operator [] (const key_type& k) const {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
packed_array_generic<T>::resize(const key_type& s) {
	values.resize(sizes_product(s));
	sizes = s;
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
ostream&
packed_array_generic<T>::dump(ostream& o) const {
	o << "packed_array_generic: size = " << sizes <<
		", offset = " << offset << ", coeffs = " << coeffs << endl;
	o << "{ ";
	ostream_iterator<T> osi(o, ", ");
	copy(begin(), end(), osi);
	return o << " }" << endl;
}
#endif

//=============================================================================
}	// end namespace util

#endif	// __PACKED_ARRAY_TCC__

