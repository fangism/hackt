/**
	\file "util/packed_array.tcc"
	$Id: packed_array.tcc,v 1.11.12.1 2005/06/21 01:08:25 fang Exp $
 */

#ifndef	__UTIL_PACKED_ARRAY_TCC__
#define	__UTIL_PACKED_ARRAY_TCC__

#include "util/packed_array.h"

#ifndef	EXTERN_TEMPLATE_UTIL_PACKED_ARRAY

#include <iostream>
#include <numeric>
#include <iterator>
#include "util/macros.h"

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_PACKED_ARRAY
#define	EXTERN_TEMPLATE_UTIL_MULTIKEY
#endif

#include "util/multikey.tcc"

namespace util {
#include "util/using_ostream.h"
using std::accumulate;
using std::ostream_iterator;
using util::multikey_generator;

//=============================================================================
PACKED_ARRAY_TEMPLATE_SIGNATURE
const
typename packed_array<D,K,T>::ones_type
packed_array<D,K,T>::ones(1);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Constructs a packed array given an array of dimensions.
	\param s the dimensions of the new array.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,K,T>::packed_array(const key_type& s) :
		sizes(s), values(sizes_product(s)), offset(), coeffs(1) {
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a packed array with index offsets in each dimension.
	\param s dimensions of the new array.
	\oaram o the offset of the new array.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,K,T>::packed_array(const key_type& s, const key_type& o) :
		sizes(s), values(sizes_product(s)), offset(o), coeffs(1) {
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
packed_array<D,K,T>::packed_array(const packed_array& a, 
		const key_type& l, const key_type& u) :
		sizes((INVARIANT(l <= u), u - l + ones)), 
		values(sizes_product(sizes)), offset(), coeffs(1) {
	// offset remains 0
	reset_coeffs();
	key_generator_type key_gen(l, u);
	key_gen.initialize();
	INVARIANT(values.size() == sizes_product(sizes));
	register index_type i = 0;
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
packed_array<D,K,T>::~packed_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the index of the first entry, which is the offset. 
	Useful for constructing the multikey_generator.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,K,T>::key_type
packed_array<D,K,T>::first_key(void) const {
	return offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,K,T>::key_type
packed_array<D,K,T>::last_key(void) const {
	return sizes +offset -ones;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function to compute the size based on dimensions.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,K,T>::index_type
packed_array<D,K,T>::sizes_product(const key_type& k) {
	return accumulate(k.begin(), k.end(), 1, std::multiplies<index_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caches the coefficients used in computing the internal index.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
void
packed_array<D,K,T>::reset_coeffs(void) {
	coeffs = ones;
	index_type i = 1;	// skip first
	for ( ; i < D; i++) {
		index_type j = 0;
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
packed_array<D,K,T>::range_check(const key_type& k) const {
	index_type i = 0;
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
typename packed_array<D,K,T>::index_type
packed_array<D,K,T>::key_to_index(const key_type& k) const {
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
packed_array<D,K,T>::operator [] (const key_type& k) {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
const T&
packed_array<D,K,T>::operator [] (const key_type& k) const {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the size of the packed array, 
	but does not clear existing entries.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
void
packed_array<D,K,T>::resize(const key_type& s) {
	values.resize(sizes_product(s));
	sizes = s;
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
ostream&
packed_array<D,K,T>::dump(ostream& o) const {
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
typename packed_array<D,K,bool>::ones_type
packed_array<D,K,bool>::ones(1);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Constructs a packed array given an array of dimensions.
	\param s the dimensions of the new array.
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,K,bool>::packed_array(const key_type& s) :
		sizes(s), values(sizes_product(s)), offset(), coeffs(1) {
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a packed array with index offsets in each dimension.
	\param s dimensions of the new array.
	\oaram o the offset of the new array.
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
packed_array<D,K,bool>::packed_array(const key_type& s, const key_type& o) :
		sizes(s), values(sizes_product(s)), offset(o), coeffs(1) {
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
packed_array<D,K,bool>::packed_array(const packed_array& a, 
		const key_type& l, const key_type& u) :
		sizes((INVARIANT(l <= u), u - l + ones)), 
		values(sizes_product(sizes)), offset(), coeffs(1) {
	// offset remains 0
	reset_coeffs();
	key_generator_type key_gen(l, u);
	key_gen.initialize();
	INVARIANT(values.size() == sizes_product(sizes));
	register index_type i = 0;
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
packed_array<D,K,bool>::~packed_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the index of the first entry, which is the offset. 
	Useful for constructing the multikey_generator.  
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,K,bool>::key_type
packed_array<D,K,bool>::first_key(void) const {
	return offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,K,bool>::key_type
packed_array<D,K,bool>::last_key(void) const {
	return sizes +offset -ones;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function to compute the size based on dimensions.  
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,K,bool>::index_type
packed_array<D,K,bool>::sizes_product(const key_type& k) {
	return accumulate(k.begin(), k.end(), 1, std::multiplies<index_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caches the coefficients used in computing the internal index.  
 */
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
void
packed_array<D,K,bool>::reset_coeffs(void) {
	coeffs = ones;
	index_type i = 1;	// skip first
	for ( ; i < D; i++) {
		index_type j = 0;
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
packed_array<D,K,bool>::range_check(const key_type& k) const {
	index_type i = 0;
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
typename packed_array<D,K,bool>::index_type
packed_array<D,K,bool>::key_to_index(const key_type& k) const {
	const key_type diff(k -offset);
	return std::inner_product(diff.begin(), &diff.back(), coeffs.begin(), 
		diff.back());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,K,bool>::reference
packed_array<D,K,bool>::operator [] (const key_type& k) {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
typename packed_array<D,K,bool>::const_reference
packed_array<D,K,bool>::operator [] (const key_type& k) const {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
void
packed_array<D,K,bool>::resize(const key_type& s) {
	values.resize(sizes_product(s));
	sizes = s;
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_BOOL_ARRAY_TEMPLATE_SIGNATURE
ostream&
packed_array<D,K,bool>::dump(ostream& o) const {
	o << "packed_array: size = " << sizes <<
		", offset = " << offset << ", coeffs = " << coeffs << endl;
	o << "{ ";
	ostream_iterator<bool> osi(o, ", ");
	copy(begin(), end(), osi);
	return o << " }" << endl;
}

//=============================================================================
// class packed_array_generic method definitions

/**
	\param d the dimensions of the ones-vector.
	\return vector of d 1's.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<K,T>::ones_type
packed_array_generic<K,T>::ones(const size_t d) {
	return ones_type(d, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Establishes the dimensionality of the array.  
	\param d the number of dimensions.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
packed_array_generic<K,T>::packed_array_generic(const size_t d) :
		dim(d), sizes(d, 0), values(), offset(d, 0), 
		coeffs(d ? d-1 : d, 1) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Constructs a packed array given an array of dimensions.
	\param s the dimensions of the new array.
	\pre s.size() > 0.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
packed_array_generic<K,T>::packed_array_generic(const key_type& s) :
		dim(s.size()), sizes(s), 
		values(sizes_product(s)), offset(dim, 0), coeffs(dim-1, 1) {
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs a packed array with index offsets in each dimension.
	\param s dimensions of the new array.
	\oaram o the offset of the new array.
	\pre s and o have the same size and size is > 0.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
packed_array_generic<K,T>::packed_array_generic(
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
packed_array_generic<K,T>::packed_array_generic(
		const packed_array_generic& a, 
		const key_type& l, const key_type& u) :
		dim(l.size()), 
		sizes((INVARIANT(dim == u.size()),
			INVARIANT(l <= u),	// uses lexicographical_compare
			u - l + key_type(dim, 1))), 
		values(sizes_product(sizes)), offset(dim, 0), coeffs(dim-1, 1) {
	// offset remains 0
	reset_coeffs();
	key_generator_type key_gen(l, u);
	key_gen.initialize();
	INVARIANT(index_type(values.size()) == sizes_product(sizes));
	register index_type i = 0;
	do {
		// write valarray directly
		values[i++] = a[key_gen];
		key_gen++;
	} while (key_gen != key_gen.lower_corner);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
packed_array_generic<K,T>::~packed_array_generic() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the index of the first entry, which is the offset. 
	Useful for constructing the multikey_generator.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<K,T>::key_type
packed_array_generic<K,T>::first_key(void) const {
	return offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<K,T>::key_type
packed_array_generic<K,T>::last_key(void) const {
#if 1
	return sizes +offset -key_type(dim, 1);
#else
	return std::plus<key_type>(sizes, offset)();
//	return std::minus(std::plus(sizes, offset), key_type(dim, 1));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function to compute the size based on dimensions.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<K,T>::index_type
packed_array_generic<K,T>::sizes_product(const key_type& k) {
	return accumulate(k.begin(), k.end(), 1, std::multiplies<index_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caches the coefficients used in computing the internal index.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
packed_array_generic<K,T>::reset_coeffs(void) {
	coeffs = key_type(dim, 1);
	index_type i = 1;	// skip first
	for ( ; i < index_type(dim); i++) {
		index_type j = 0;
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
packed_array_generic<K,T>::range_check(const key_type& k) const {
	index_type i = 0;
	for ( ; i < index_type(dim); i++) {
		const index_type k_diff = k[i] -offset[i];
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
typename packed_array_generic<K,T>::index_type
packed_array_generic<K,T>::key_to_index(const key_type& k) const {
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
typename packed_array_generic<K,T>::reference
packed_array_generic<K,T>::operator [] (const key_type& k) {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename packed_array_generic<K,T>::const_reference
packed_array_generic<K,T>::operator [] (const key_type& k) const {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if arguments are equal.
	NOTE: equality only requires that sizes and values are equivalent, 
		the offset does not matter.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
bool
packed_array_generic<K,T>::operator == (const this_type& a) const {
	return (sizes == a.sizes && values == a.values);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resizes to 0-dimensions, i.e. one scalar element.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
packed_array_generic<K,T>::resize(void) {
	dim = 0;
	sizes = key_type(0);
	values.resize(1);
	// offsets? = key_type(0)?
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
packed_array_generic<K,T>::resize(const key_type& s) {
	dim = s.size();
	sizes = s;
	values.resize(sizes_product(s));
	// offsets?
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
ostream&
packed_array_generic<K,T>::dump(ostream& o) const {
	o << "packed_array_generic: size = " << sizes <<
		", offset = " << offset << ", coeffs = " << coeffs << endl;
	o << "{ ";
	ostream_iterator<T> osi(o, ", ");
	copy(begin(), end(), osi);
	return o << " }" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
ostream&
packed_array_generic<K,T>::write(ostream& o) const {
	write_value(o, dim);
	sizes.write(o);
	offset.write(o);
	write_sequence(o, values);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
istream&
packed_array_generic<K,T>::read(istream& i) {
	read_value(i, dim);
	sizes.read(i);
	offset.read(i);
	resize(sizes);		// optional
	read_sequence_resize(i, values);
	return i;
}

//=============================================================================
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_PACKED_ARRAY
#endif	// __UTIL_PACKED_ARRAY_TCC__

