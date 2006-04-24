/**
	\file "util/packed_array.tcc"
	$Id: packed_array.tcc,v 1.14.20.1 2006/04/24 05:43:06 fang Exp $
 */

#ifndef	__UTIL_PACKED_ARRAY_TCC__
#define	__UTIL_PACKED_ARRAY_TCC__

#include "util/packed_array.h"

#ifndef	EXTERN_TEMPLATE_UTIL_PACKED_ARRAY

#include <iostream>
#include <numeric>
#include "util/macros.h"

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_PACKED_ARRAY
#define	EXTERN_TEMPLATE_UTIL_MULTIKEY
#endif

#include "util/multikey.tcc"

namespace util {
#include "util/using_ostream.h"
using std::accumulate;
// using util::multikey_generator;

//=============================================================================
PACKED_ARRAY_TEMPLATE_SIGNATURE
const
typename PACKED_ARRAY_CLASS::ones_type
PACKED_ARRAY_CLASS::ones(1);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
	Constructs a packed array given an array of dimensions.
	\param s the dimensions of the new array.
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
PACKED_ARRAY_CLASS::packed_array(const key_type& s) :
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
PACKED_ARRAY_CLASS::packed_array(const key_type& s, const key_type& o) :
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
PACKED_ARRAY_CLASS::packed_array(const packed_array& a, 
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
PACKED_ARRAY_CLASS::~packed_array() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the index of the first entry, which is the offset. 
	Useful for constructing the multikey_generator.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
typename PACKED_ARRAY_CLASS::key_type
PACKED_ARRAY_CLASS::first_key(void) const {
	return offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
typename PACKED_ARRAY_CLASS::key_type
PACKED_ARRAY_CLASS::last_key(void) const {
	return sizes +offset -ones;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function to compute the size based on dimensions.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
typename PACKED_ARRAY_CLASS::index_type
PACKED_ARRAY_CLASS::sizes_product(const key_type& k) {
	return accumulate(k.begin(), k.end(), 1, std::multiplies<index_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caches the coefficients used in computing the internal index.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
void
PACKED_ARRAY_CLASS::reset_coeffs(void) {
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
PACKED_ARRAY_CLASS::range_check(const key_type& k) const {
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
typename PACKED_ARRAY_CLASS::index_type
PACKED_ARRAY_CLASS::key_to_index(const key_type& k) const {
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
// T&
typename PACKED_ARRAY_CLASS::reference
PACKED_ARRAY_CLASS::operator [] (const key_type& k) {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
// const T&
typename PACKED_ARRAY_CLASS::const_reference
PACKED_ARRAY_CLASS::operator [] (const key_type& k) const {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resets the size of the packed array, 
	but does not clear existing entries.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
void
PACKED_ARRAY_CLASS::resize(const key_type& s) {
	values.resize(sizes_product(s));
	sizes = s;
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
ostream&
PACKED_ARRAY_CLASS::dump(ostream& o) const {
	o << "size = " << sizes <<
		", offset = " << offset << ", coeffs = " << coeffs << endl;
	return dump_values(o << "values = ") << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_TEMPLATE_SIGNATURE
ostream&
PACKED_ARRAY_CLASS::dump_values(ostream& o) const {
	dump_slice(o, D, 0);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive multidimensional value pretty-printer.  
	This ignores the offset(s) of the packed array entirely.  
	\param o the output stream.
	\param d the dimension of the slice:
		0 means the smallest (or scalar) slice, 
		d == dim means the highest slice, the entire array.
	\param s the offset of the first entry to print.  
 */
PACKED_ARRAY_TEMPLATE_SIGNATURE
void
PACKED_ARRAY_CLASS::dump_slice(ostream& o, const size_type d, 
		const size_type s) const {
	INVARIANT(d <= D);
#if 0
	cerr << "In dump_slice(o, " << d << ", " << s << "):" << endl;
#endif
if (d > 1) {
	o << '{';
	const size_type sub_count = sizes[D -d];
	if (sub_count) {
		const size_type slice_size = coeffs[D -d];
		const size_type e = s +sub_count *slice_size;
		size_type slice_start = s +slice_size;
		dump_slice(o, d-1, s);
		for ( ; slice_start < e; slice_start += slice_size) {
			dump_slice(o << ',', d-1, slice_start);
		}
	} // else slice is thin!
	o << '}';
} else if (d == 1) {
	// optimizing the 1-dimensional case
	o << '{';
	const size_type sub_count = sizes[D -1];
	if (sub_count) {
		o << values[s];
		const size_type e = s +sub_count;
		size_type slice_start = s+1;
		for ( ; slice_start < e; slice_start++) {
			o << ',' << values[slice_start];
		}
	}
	o << '}';
} else {
	// d == 0, always at least one element, even for 0-dimensional scalar
	o << values[0];
}
}

//=============================================================================
// class packed_array_generic method definitions

/**
	\param d the dimensions of the ones-vector.
	\return vector of d 1's.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename PACKED_ARRAY_GENERIC_CLASS::ones_type
PACKED_ARRAY_GENERIC_CLASS::ones(const size_t d) {
	return ones_type(d, 1);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Establishes the dimensionality of the array.  
	\param d the number of dimensions.
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
PACKED_ARRAY_GENERIC_CLASS::packed_array_generic(const size_t d) :
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
PACKED_ARRAY_GENERIC_CLASS::packed_array_generic(const key_type& s) :
		dim(s.size()), sizes(s), 
		values(sizes_product(s)), offset(dim, 0), 
		coeffs(dim ? dim-1 : dim, 1) {
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
PACKED_ARRAY_GENERIC_CLASS::packed_array_generic(
		const key_type& s, const key_type& o) :
		dim(s.size()), sizes(s), 
		values(sizes_product(s)), offset(o),
		coeffs(dim ? dim-1 : dim, 1) {
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
PACKED_ARRAY_GENERIC_CLASS::packed_array_generic(
		const packed_array_generic& a, 
		const key_type& l, const key_type& u) :
		dim(l.size()), 
		sizes((INVARIANT(dim == u.size()),
			INVARIANT(l <= u),	// uses lexicographical_compare
			u - l + key_type(dim, 1))), 
		values(sizes_product(sizes)), offset(dim, 0), 
		coeffs(dim ? dim-1 : dim, 1) {
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
PACKED_ARRAY_GENERIC_CLASS::~packed_array_generic() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return the index of the first entry, which is the offset. 
	Useful for constructing the multikey_generator.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename PACKED_ARRAY_GENERIC_CLASS::key_type
PACKED_ARRAY_GENERIC_CLASS::first_key(void) const {
	return offset;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename PACKED_ARRAY_GENERIC_CLASS::key_type
PACKED_ARRAY_GENERIC_CLASS::last_key(void) const {
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
typename PACKED_ARRAY_GENERIC_CLASS::index_type
PACKED_ARRAY_GENERIC_CLASS::sizes_product(const key_type& k) {
	const index_type ret =
		accumulate(k.begin(), k.end(), 1,
			std::multiplies<index_type>());
	INVARIANT(ret > 0);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Caches the coefficients used in computing the internal index.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
PACKED_ARRAY_GENERIC_CLASS::reset_coeffs(void) {
	coeffs = key_type(dim ? dim-1 : dim, 1);
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
PACKED_ARRAY_GENERIC_CLASS::range_check(const key_type& k) const {
//	cerr << "packed_array_generic::range_check" << endl;
	index_type i = 0;
	for ( ; i < index_type(dim); i++) {
		const index_type k_diff = k[i] -offset[i];
#if 0
		// debugging
		cerr << "\ti=" << i << ", " << "diff = " << k_diff <<
			", offset[i] = " << offset[i] <<
			", size[i] = " << sizes[i] << endl;
#endif
		if (k_diff >= sizes[i]) {
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
typename PACKED_ARRAY_GENERIC_CLASS::index_type
PACKED_ARRAY_GENERIC_CLASS::key_to_index(const key_type& k) const {
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
typename PACKED_ARRAY_GENERIC_CLASS::reference
PACKED_ARRAY_GENERIC_CLASS::operator [] (const key_type& k) {
	return values[key_to_index(k)];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
typename PACKED_ARRAY_GENERIC_CLASS::const_reference
PACKED_ARRAY_GENERIC_CLASS::operator [] (const key_type& k) const {
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
PACKED_ARRAY_GENERIC_CLASS::operator == (const this_type& a) const {
	return (sizes == a.sizes && values == a.values);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Resizes to 0-dimensions, i.e. one scalar element.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
PACKED_ARRAY_GENERIC_CLASS::resize(void) {
	dim = 0;
	sizes = key_type(0);
	values.resize(1);
	// offsets? = key_type(0)?
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
PACKED_ARRAY_GENERIC_CLASS::resize(const key_type& s) {
	dim = s.size();
	// problem: is sizes and s alias, then assignment may go awry
	// this is fixed by catching the alias case in multikey_generic
	sizes = s;
	values.resize(sizes_product(s));
	// offsets?
	reset_coeffs();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
ostream&
PACKED_ARRAY_GENERIC_CLASS::dump(ostream& o) const {
	o << "size = " << sizes <<
		", offset = " << offset << ", coeffs = " << coeffs << endl;
	return dump_values(o << "values = ") << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initiates recursive calls to dump_slice.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
ostream&
PACKED_ARRAY_GENERIC_CLASS::dump_values(ostream& o) const {
	dump_slice(o, dim, 0);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive multidimensional value pretty-printer.  
	This ignores the offset(s) of the packed array entirely.  
	\param o the output stream.
	\param d the dimension of the slice:
		0 means the smallest (or scalar) slice, 
		d == dim means the highest slice, the entire array.
	\param s the offset of the first entry to print.  
 */
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
void
PACKED_ARRAY_GENERIC_CLASS::dump_slice(ostream& o, const size_type d, 
		const size_type s) const {
	INVARIANT(d <= dim);
#if 0
	cerr << "In dump_slice(o, " << d << ", " << s << "):" << endl;
#endif
if (d > 1) {
	o << '{';
	const size_type sub_count = sizes[dim -d];
	if (sub_count) {
		const size_type slice_size = coeffs[dim -d];
		const size_type e = s +sub_count *slice_size;
		size_type slice_start = s +slice_size;
		dump_slice(o, d-1, s);
		for ( ; slice_start < e; slice_start += slice_size) {
			dump_slice(o << ',', d-1, slice_start);
		}
	} // else slice is thin!
	o << '}';
} else if (d == 1) {
	// optimizing the 1-dimensional case
	o << '{';
	const size_type sub_count = sizes[dim -1];
	if (sub_count) {
		o << values[s];
		const size_type e = s +sub_count;
		size_type slice_start = s+1;
		for ( ; slice_start < e; slice_start++) {
			o << ',' << values[slice_start];
		}
	}
	o << '}';
} else {
	// d == 0, always at least one element, even for 0-dimensional scalar
	o << values[0];
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
ostream&
PACKED_ARRAY_GENERIC_CLASS::write(ostream& o) const {
	write_value(o, dim);
	sizes.write(o);
	offset.write(o);
	write_sequence(o, values);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PACKED_ARRAY_GENERIC_TEMPLATE_SIGNATURE
istream&
PACKED_ARRAY_GENERIC_CLASS::read(istream& i) {
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

