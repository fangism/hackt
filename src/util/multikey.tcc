/**
	\file "util/multikey.tcc"
	Multidimensional key class method definitions.
	$Id: multikey.tcc,v 1.8.2.2 2005/07/04 01:54:08 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_TCC__
#define	__UTIL_MULTIKEY_TCC__

#include "util/multikey.h"

#ifndef	EXTERN_TEMPLATE_UTIL_MULTIKEY

#include <iostream>
#include <algorithm>		// for transform
#include <functional>

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_MULTIKEY
#define	EXTERN_TEMPLATE_UTIL_IO_UTILS
#endif

#include "util/IO_utils.tcc"

/**
	Macro for explicit instantiation of class and its relevant methods.  
	This macro MUST APPEAR in the util namespace.  
 */ 
#define	INSTANTIATE_MULTIKEY(D,K)					\
template class multikey<D,K >;						\
template std::ostream& operator << (std::ostream&, const multikey<D,K >&); \
template bool operator == (const multikey<D,K >&, const multikey<D,K >&); \
template bool operator != (const multikey<D,K >&, const multikey<D,K >&); \
template bool operator < (const multikey<D,K >&, const multikey<D,K >&);

#define	INSTANTIATE_MULTIKEY_LIST_CTOR(D,K,L)				\
template multikey<D,K >::multikey(const L<K >&, const K);

#define	INSTANTIATE_MULTIKEY_GENERATOR(D,K)				\
template class multikey_generator<D, K >;

namespace util {
using util::write_value;
using util::read_value;
#include "util/using_ostream.h"
using std::copy;
using std::fill;
using std::transform;
using std::lexicographical_compare;
using std::ptr_fun;

//=============================================================================
// class multikey method definitions

MULTIKEY_TEMPLATE_SIGNATURE
MULTIKEY_CLASS::multikey(const K i) {
	fill(this->begin(), this->end(), i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param k generic multikey, whose dimensions MUST match this!
 */
MULTIKEY_TEMPLATE_SIGNATURE
MULTIKEY_CLASS::multikey(const MULTIKEY_GENERIC_CLASS& k) {
	INVARIANT(k.dimensions() == D);
	copy(k.begin(), k.end(), this->begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
template <size_t D2>
MULTIKEY_CLASS::multikey(const multikey<D2,K>& k, const K i) {
	// depends on <algorithm>
	if (D <= D2) {
		// if D == D2? won't k[D] be out of bounds?
		copy(k.begin(), &k[D-1] +1, this->begin());
	} else {
		copy(k.begin(), k.end(), this->begin());
		fill(&(*this)[D2], this->end(), i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
template <template <class> class S>
MULTIKEY_CLASS::multikey(const S<K>& s, const K i) {
	const size_t sz = s.size();
	if (D < sz) {
		size_t j = 0;
		typename S<K>::const_iterator iter(s.begin());
		for ( ; j<sz; j++)
			(*this)[j] = *iter;
	} else {
		copy(s.begin(), s.end(), this->begin());
		fill(&(*this)[sz-1] +1, this->end(), i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
	size_t
	dimensions(void) const { return D; }

	K
	default_value(void) const { return init; }

	iterator
	begin(void) { return &indices[0]; }

	const_iterator
	begin(void) const { return &indices[0]; }

	iterator
	end(void) { return &indices[D]; }

	const_iterator
	end(void) const { return &indices[D]; }

	/**
		Safe indexing with array-bound check.  
		indices is public, so one can always access it directly...
	 */
	reference
	operator [] (const size_t i) {
		INVARIANT(i < D);
		return indices[i];
	}

	/**
		Const version of array indexing.  
	 */
	const_reference
	operator [] (const size_t i) const {
		INVARIANT(i < D);
		return indices[i];
	}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
MULTIKEY_CLASS&
MULTIKEY_CLASS::operator = (const this_type& s) {
	copy(s.begin(), s.end(), this->begin());
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
MULTIKEY_CLASS&
MULTIKEY_CLASS::operator += (const this_type& k) {
	transform(this->begin(), this->end(), k.begin(), 
		this->begin(), std::plus<K>());
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
MULTIKEY_CLASS&
MULTIKEY_CLASS::operator -= (const this_type& k) {
	transform(this->begin(), this->end(), k.begin(), 
		this->begin(), std::minus<K>());
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
ostream&
MULTIKEY_CLASS::write(ostream& o) const {
	// wish there was ostream_iterator equivalent for write()
	INVARIANT(o.good());
	const_iterator i(this->begin());
	const const_iterator e(this->end());
	for ( ; i!=e; i++)
		write_value(o, *i);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
istream&
MULTIKEY_CLASS::read(istream& f) {
	// wish there was ostream_iterator equivalent for write()
	INVARIANT(f.good());
	iterator i(this->begin());
	size_t j = 0;
	for ( ; j < D; j++, i++)
		read_value(f, *i);
	return f;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// class multikey::accumulate_extremities method definitions

MULTIKEY_TEMPLATE_SIGNATURE
// inline	// private
K
MULTIKEY_CLASS::accumulate_extremities::mymin(const K& a, const K& b) {
	return (a<b)?a:b;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
// inline	// private
K
MULTIKEY_CLASS::accumulate_extremities::mymax(const K& a, const K& b) {
	return (a>b)?a:b;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
void
MULTIKEY_CLASS::accumulate_extremities::operator () (
		const MULTIKEY_CLASS& k) {
	transform(min.begin(), min.end(), k.begin(), 
		min.begin(), ptr_fun(mymin));
	transform(max.begin(), max.end(), k.begin(), 
		max.begin(), ptr_fun(mymax));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
typename MULTIKEY_CLASS::accumulate_extremities::key_pair
MULTIKEY_CLASS::accumulate_extremities::operator () (
		const key_pair& a, const MULTIKEY_CLASS& b) {
	key_pair ret;
	transform(a.first.begin(), a.first.end(), b.begin(), 
		ret.first.begin(), ptr_fun(mymin));
	transform(a.second.begin(), a.second.end(), b.begin(), 
		ret.second.begin(), ptr_fun(mymax));
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// inlined in declaration

MULTIKEY_TEMPLATE_SIGNATURE
template <class T>
void
MULTIKEY_CLASS::accumulate_extremities::operator () (
		const pair<const MULTIKEY_CLASS, T>& p) {
	this->operator()(p.first);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
template <class T>
key_pair
MULTIKEY_CLASS::accumulate_extremities::operator () (
		const key_pair& a, const pair<const MULTIKEY_CLASS, T>& p) {
	return this->operator()(a, p.first);
}
#endif

//-----------------------------------------------------------------------------
#if 0
MULTIKEY_TEMPLATE_SIGNATURE
const MULTIKEY_CLASS
MULTIKEY_CLASS::ones = multikey<D,K,1>();
#endif

//=============================================================================
// class multikey_generic method definitions

MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
template <size_t D>
MULTIKEY_GENERIC_CLASS::multikey_generic(const MULTIKEY_CLASS& m) : impl_type(D) {
	// valarray doesn't have a sequence-copy constructor like vector
	copy(m.begin(), m.end(), begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Custom assignment operator required because the valarray
	class implementation is not Assignable; 
	it does not automatically realloc!
 */
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
MULTIKEY_GENERIC_CLASS&
MULTIKEY_GENERIC_CLASS::operator = (const this_type& m) {
	this->~multikey_generic();	// explicit call to own dtor
	new (this) this_type(m);	// placement copy-constructor
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if dimensions are equal and values are equal.  
 */
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
bool
MULTIKEY_GENERIC_CLASS::operator == (const this_type& m) const {
	return (size() == m.size() && 
		std::equal(this->begin(), this->end(), m.begin()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if dimensions are equal and values are equal.  
 */
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
bool
MULTIKEY_GENERIC_CLASS::operator < (const this_type& m) const {
	INVARIANT(size() == m.size());
	return std::lexicographical_compare(
		this->begin(), this->end(), m.begin(), m.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
MULTIKEY_GENERIC_CLASS
MULTIKEY_GENERIC_CLASS::operator + (const this_type& m) const {
	INVARIANT(size() == m.size());
	this_type ret(size());
	transform(this->begin(), this->end(), m.begin(), ret.begin(),
		std::plus<K>());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
MULTIKEY_GENERIC_CLASS
MULTIKEY_GENERIC_CLASS::operator - (const this_type& m) const {
	INVARIANT(size() == m.size());
	this_type ret(size());
	transform(this->begin(), this->end(), m.begin(), ret.begin(),
		std::minus<K>());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
ostream&
MULTIKEY_GENERIC_CLASS::write(ostream& o) const {
	write_array(o, AS_A(const impl_type&, *this));
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
istream&
MULTIKEY_GENERIC_CLASS::read(istream& i) {
	read_sequence_resize(i, AS_A(impl_type&, *this));
	return i;
}

//=============================================================================

MULTIKEY_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const MULTIKEY_CLASS& k) {
	typename MULTIKEY_CLASS::const_iterator i(k.begin());
	const typename MULTIKEY_CLASS::const_iterator e(k.end());
	for ( ; i!=e; i++)
		o << '[' << *i << ']';
	return o;
}

MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const MULTIKEY_GENERIC_CLASS& k) {
	typename MULTIKEY_GENERIC_CLASS::const_iterator i(k.begin());
	const typename MULTIKEY_GENERIC_CLASS::const_iterator e(k.end());
	for ( ; i!=e; i++)
		o << '[' << *i << ']';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator < (const MULTIKEY_CLASS& l, const MULTIKEY_CLASS& r) {
	return lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
}

template <size_t D1, size_t D2, class K>
bool
operator < (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator > (const MULTIKEY_CLASS& l, const MULTIKEY_CLASS& r) {
	return lexicographical_compare(r.begin(), r.end(), l.begin(), l.end());
}

template <size_t D1, size_t D2, class K>
bool
operator > (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return lexicographical_compare(r.begin(), r.end(), l.begin(), l.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator == (const MULTIKEY_CLASS& l, const MULTIKEY_CLASS& r) {
	return std::equal(l.begin(), l.end(), r.begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator != (const MULTIKEY_CLASS& l, const MULTIKEY_CLASS& r) {
	return !(l == r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator <= (const MULTIKEY_CLASS& l, const MULTIKEY_CLASS& r) {
	return !(l > r);
}

template <size_t D1, size_t D2, class K>
bool
operator <= (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return !(l > r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator >= (const MULTIKEY_CLASS& l, const MULTIKEY_CLASS& r) {
	return !(l < r);
}

template <size_t D1, size_t D2, class K>
bool
operator >= (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return !(l < r);
}

//=============================================================================
// class multikey_generator method definitions

MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
template <template <class> class L, template <class, class> class P>
MULTIKEY_GENERATOR_CLASS::multikey_generator(const L<P<K,K> >& l) :
		base_type(), lower_corner(), upper_corner() {
	typedef	L<P<K,K> >	sequence_type;
	INVARIANT(l.size() <= D);	// else error on user!
	iterator li(lower_corner.begin());
	iterator ui(upper_corner.begin());
	typename sequence_type::const_iterator i(l.begin());
	const typename sequence_type::const_iterator e(l.end());
	for ( ; l != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
template <class LP>
MULTIKEY_GENERATOR_CLASS::multikey_generator(const LP& l) :
		base_type(), lower_corner(), upper_corner() {
	typedef	LP	sequence_type;
	INVARIANT(l.size() <= D);	// else error on user!
	iterator li(lower_corner.begin());
	iterator ui(upper_corner.begin());
	typename sequence_type::const_iterator i(l.begin());
	const typename sequence_type::const_iterator e(l.end());
	for ( ; l != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
void
MULTIKEY_GENERATOR_CLASS::validate(void) const {
	const_iterator min(lower_corner.begin());
	const_iterator max(upper_corner.begin());
	const const_iterator min_end(lower_corner.end());
	for ( ; min != min_end; min++, max++) {
		INVARIANT(*min <= *max);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
void
MULTIKEY_GENERATOR_CLASS::initialize(void) {
	validate();
	copy(lower_corner.begin(), lower_corner.end(), this->begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
	size_t
	size(void) const { return base_type::dimensions(); }

	iterator
	begin(void) { return base_type::begin(); }

	const_iterator
	begin(void) const { return base_type::begin(); }

	iterator
	end(void) { return base_type::end(); }

	const_iterator
	end(void) const { return base_type::end(); }

	corner_type&
	get_lower_corner(void) { return lower_corner; }

	const corner_type&
	get_lower_corner(void) const { return lower_corner; }

	corner_type&
	get_upper_corner(void) { return upper_corner; }

	const corner_type&
	get_upper_corner(void) const { return upper_corner; }
#endif

/**
	Postfix increment, advances the multikey to the next key in the
	slice's lexicographical ordering.  
	\return reference to self.  
 */
MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
typename MULTIKEY_GENERATOR_CLASS::corner_type&
MULTIKEY_GENERATOR_CLASS::operator ++ (int) {
	reverse_iterator inc(this->rbegin());
	const const_reverse_iterator msp(this->rend());
	const_reverse_iterator min(lower_corner.rbegin());
	const_reverse_iterator max(upper_corner.rbegin());
	for ( ; inc != msp; inc++, min++, max++) {
		if (*inc >= *max)
			*inc = *min;
		else {
			(*inc)++;
			break;
		}
	}
	return *this;
}

	// all other methods inherited

//-----------------------------------------------------------------------------
// class multikey_generator_generic method definitions

MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE
template <template <class> class L, template <class, class> class P>
MULTIKEY_GENERATOR_GENERIC_CLASS::multikey_generator_generic(
		const L<P<K,K> >& l) : base_type(), 
		lower_corner(), upper_corner() {
	typedef	L<P<K,K> >	sequence_type;
	INVARIANT(l.size() <= this->dimensions());	// else error on user!
	iterator li(lower_corner.begin());
	iterator ui(upper_corner.begin());
	typename sequence_type::const_iterator i(l.begin());
	const typename sequence_type::const_iterator e(l.end());
	for ( ; l != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE
template <class LP>
MULTIKEY_GENERATOR_GENERIC_CLASS::multikey_generator_generic(const LP& l) :
		base_type(), lower_corner(), upper_corner() {
	typedef	LP	sequence_type;
	INVARIANT(l.size() <= this->dimensions());	// else error on user!
	iterator li(lower_corner.begin());
	iterator ui(upper_corner.begin());
	typename sequence_type::const_iterator i(l.begin());
	const typename sequence_type::const_iterator e(l.end());
	for ( ; l != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE
void
MULTIKEY_GENERATOR_GENERIC_CLASS::validate(void) const {
	const_iterator min_iter(lower_corner.begin());
	const_iterator max_iter(upper_corner.begin());
	const const_iterator min_end(lower_corner.end());
	for ( ; min_iter != min_end; min_iter++, max_iter++) {
		INVARIANT(*min_iter <= *max_iter);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE
void
MULTIKEY_GENERATOR_GENERIC_CLASS::initialize(void) {
	validate();
	copy(lower_corner.begin(), lower_corner.end(), this->begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Postfix increment, advances the multikey to the next key in the
	slice's lexicographical ordering.  
	\return reference to self.  
 */
MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE
typename MULTIKEY_GENERATOR_GENERIC_CLASS::corner_type&
MULTIKEY_GENERATOR_GENERIC_CLASS::operator ++ (int) {
	reverse_iterator inc(this->rbegin());
	const const_reverse_iterator msp(this->rend());
	const_reverse_iterator min_iter(lower_corner.rbegin());
	const_reverse_iterator max_iter(upper_corner.rbegin());
	for ( ; inc != msp; inc++, min_iter++, max_iter++) {
		if (*inc >= *max_iter)
			*inc = *min_iter;
		else {
			(*inc)++;
			break;
		}
	}
	return *this;
}

//=============================================================================
// specializations

template <class K>
void
value_writer<MULTIKEY_GENERIC_CLASS >::operator () (const value_type& v) {
	// readlly should reference typename value_type::impl_type
	write_array(os, AS_A(const valarray<K>&, v));
}

//-----------------------------------------------------------------------------

template <class K>
void
value_reader<MULTIKEY_GENERIC_CLASS >::operator () (value_type& v) {
	// readlly should reference typename value_type::impl_type
	read_sequence_resize(is, AS_A(valarray<K>&, v));
}

//=============================================================================
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_MULTIKEY

#endif	// __UTIL_MULTIKEY_TCC__

