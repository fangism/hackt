/**
	\file "multikey.tcc"
	Multidimensional key class method definitions.
	$Id: multikey.tcc,v 1.3 2004/12/25 03:12:22 fang Exp $
 */

#ifndef	__MULTIKEY_TCC__
#define	__MULTIKEY_TCC__

#include "multikey.h"

#include <iostream>
#include <algorithm>		// for transform
#include <functional>

#include "IO_utils.tcc"


namespace MULTIKEY_NAMESPACE {
using util::write_value;
using util::read_value;
#include "using_ostream.h"
using std::copy;
using std::fill;
using std::transform;
using std::lexicographical_compare;

//=============================================================================
// class multikey_base method definitions

BASE_MULTIKEY_TEMPLATE_SIGNATURE
multikey_base<K>*
multikey_base<K>::make_multikey(const size_t d) {
	INVARIANT(d > 0 && d <= LIMIT);
	// there may be some clever way to make a call table to
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1: return new multikey<1,K>();
		case 2: return new multikey<2,K>();
		case 3: return new multikey<3,K>();
		case 4: return new multikey<4,K>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------
// class multikey method definitions

MULTIKEY_TEMPLATE_SIGNATURE
multikey<D,K>::multikey(const K i) :
		base_type() {
	fill(indices, &indices[D], i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
template <size_t D2>
multikey<D,K>::multikey(const multikey<D2,K>& k, const K i) {
	// depends on <algorithm>
	if (D <= D2) {
		copy(k.indices, &k.indices[D], indices);
	} else {
		copy(k.indices, &k.indices[D2], indices);
		fill(&indices[D2], &indices[D], i);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
MULTIKEY_TEMPLATE_SIGNATURE
multikey<D,K>::multikey(const multikey_base<K>& k) {
	// depends on <algorithm>
	const size_t D2 = k.dimensions();
	if (D <= D2) {
		copy(k.indices, &k.indices[D], indices);
	} else {
		copy(k.indices, &k.indices[D2], indices);
		fill(&indices[D2], &indices[D], init);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
template <template <class> class S>
multikey<D,K>::multikey(const S<K>& s, const K i) {
	const size_t sz = s.size();
	if (D < sz) {
		size_t i = 0;
		typename S<K>::const_iterator iter = s.begin();
		for ( ; i<sz; i++)
			indices[i] = *iter;
	} else {
		copy(s.begin(), s.end(), indices);
		fill(&indices[sz], &indices[D], i);
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
multikey_base<K>&
multikey<D,K>::operator = (const multikey_base<K>& s) {
	const size_t sz = s.dimensions();
	if (D < sz) {
		size_t i = 0;
		const_iterator iter = s.begin();
		for ( ; i<sz; i++)
			indices[i] = *iter;
	} else {
		copy(s.begin(), s.end(), indices);
		fill(&indices[sz], &indices[D], default_value());
	}
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
multikey<D,K>&
multikey<D,K>::operator = (const this_type& s) {
	copy(s.begin(), s.end(), this->begin());
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
multikey<D,K>&
multikey<D,K>::operator += (const this_type& k) {
	transform(this->begin(), this->end(), k.begin(), 
		this->begin(), std::plus<K>());
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
multikey<D,K>&
multikey<D,K>::operator -= (const this_type& k) {
	transform(this->begin(), this->end(), k.begin(), 
		this->begin(), std::minus<K>());
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
ostream&
multikey<D,K>::write(ostream& o) const {
	// wish there was ostream_iterator equivalent for write()
	INVARIANT(o.good());
	const_iterator i = begin();
	const const_iterator e = end();
	for ( ; i!=e; i++)
		write_value(o, *i);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
istream&
multikey<D,K>::read(istream& f) {
	// wish there was ostream_iterator equivalent for write()
	INVARIANT(f.good());
	iterator i = begin();
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
multikey<D,K>::accumulate_extremities::mymin(const K& a, const K& b) {
	return (a<b)?a:b;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
// inline	// private
K
multikey<D,K>::accumulate_extremities::mymax(const K& a, const K& b) {
	return (a>b)?a:b;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
void
multikey<D,K>::accumulate_extremities::operator () (
		const multikey<D,K>& k) {
	transform(min.begin(), min.end(), k.begin(), 
		min.begin(), ptr_fun(mymin));
	transform(max.begin(), max.end(), k.begin(), 
		max.begin(), ptr_fun(mymax));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
typename multikey<D,K>::accumulate_extremities::key_pair
multikey<D,K>::accumulate_extremities::operator () (
		const key_pair& a, const multikey<D,K>& b) {
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
multikey<D,K>::accumulate_extremities::operator () (
		const pair<const multikey<D,K>, T>& p) {
	this->operator()(p.first);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_TEMPLATE_SIGNATURE
template <class T>
key_pair
multikey<D,K>::accumulate_extremities::operator () (
		const key_pair& a, const pair<const multikey<D,K>, T>& p) {
	return this->operator()(a, p.first);
}
#endif

//-----------------------------------------------------------------------------
#if 0
MULTIKEY_TEMPLATE_SIGNATURE
const multikey<D,K>
multikey<D,K>::ones = multikey<D,K,1>();
#endif

//=============================================================================
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
template <size_t D>
multikey_generic<K>::multikey_generic(const multikey<D,K>& m) :
		interface_type(), impl_type(D) {
	// valarray doesn't have a sequence-copy constructor like vector
	copy(m.begin(), m.end(), begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
multikey_base<K>&
multikey_generic<K>::operator = (const multikey_base<K>& k) {
	const size_t k_size = k.dimensions();
	if (k_size != size());
		impl_type::resize(k_size);
	copy(k.begin(), k.end(), this->begin());
	return *this;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if dimensions are equal and values are equal.  
 */
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
bool
multikey_generic<K>::operator == (const this_type& m) const {
	return (size() == m.size() && 
		std::equal(this->begin(), this->end(), m.begin()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
multikey_generic<K>
multikey_generic<K>::operator + (const this_type& m) const {
	INVARIANT(size() == m.size());
	this_type ret(size());
	transform(this->begin(), this->end(), m.begin(), ret.begin(),
		std::plus<K>());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
multikey_generic<K>
multikey_generic<K>::operator - (const this_type& m) const {
	INVARIANT(size() == m.size());
	this_type ret(size());
	transform(this->begin(), this->end(), m.begin(), ret.begin(),
		std::minus<K>());
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
ostream&
multikey_generic<K>::write(ostream& o) const {
	write_array(o, AS_A(const impl_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERIC_TEMPLATE_SIGNATURE
istream&
multikey_generic<K>::read(istream& i) {
	read_sequence_resize(i, AS_A(impl_type&, *this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if 0
template <class K>
class multikey_generic : public multikey_base<K>, public valarray<K> {
protected:
	typedef	multikey_base<K>			interface_type;
	typedef	valarray<K>				impl_type;
public:
	typedef	typename interface_type::value_type	value_type;
	typedef	typename interface_type::iterator	iterator;
	typedef	typename interface_type::const_iterator	const_iterator;
	typedef	typename impl_type::reference		reference;
	typedef	typename impl_type::const_reference	const_reference;
public:
	multikey_generic() : interface_type(), impl_type() { }

	multikey_generic(const size_t d) : interface_type(), impl_type(d) { }

	~multikey_generic() { }

	using impl_type::size;

	size_t
	dimensions(void) const { return size(); }

	K
	default_value(void) const { return 0; }

	iterator
	begin(void) { return &impl_type::operator[](0); }

	const_iterator
	begin(void) const { return &impl_type::operator[](0); }

	iterator
	end(void) { return &impl_type::operator[](size()); }

	const_iterator
	end(void) const { return &impl_type::operator[](size()); }

	multikey_base<K>&
	operator = (const multikey_base<K>& k) {
		const size_t k_size = k.dimensions();
		if (k_size != size());
			impl_type::resize(k_size);
		copy(k.begin(), k.end(), this->begin());
		return *this;
	}

	reference
	operator [] (const size_t i) {
		return impl_type::operator[](i);
	}

	const_reference
	operator [] (const size_t i) const {
		return impl_type::operator[](i);
	}

};	// end class multikey_generic
#endif

//=============================================================================

BASE_MULTIKEY_TEMPLATE_SIGNATURE
ostream&
operator << (ostream& o, const multikey_base<K>& k) {
	typename multikey_base<K>::const_iterator i = k.begin();
	const typename multikey_base<K>::const_iterator e = k.end();
	for ( ; i!=e; i++)
		o << '[' << *i << ']';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <size_t D, class K>
ostream&
operator << (ostream& o, const multikey<D,K>& k) {
	register size_t i = 0;
	for ( ; i<D; i++)
		o << '[' << k.indices[i] << ']';
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator < (const multikey<D,K>& l, const multikey<D,K>& r) {
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
operator > (const multikey<D,K>& l, const multikey<D,K>& r) {
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
operator == (const multikey<D,K>& l, const multikey<D,K>& r) {
	return std::equal(l.begin(), l.end(), r.begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Dimensions must match!
 */
template <class K>
bool
operator == (const multikey_base<K>& l, const multikey_base<K>& r) {
	if (l.dimensions() != r.dimensions())
		return false;
	return std::equal(l.begin(), l.end(), r.begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator != (const multikey<D,K>& l, const multikey<D,K>& r) {
	return !(l == r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class K>
bool
operator != (const multikey_base<K>& l, const multikey_base<K>& r) {
	return !(l == r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <size_t D, class K>
bool
operator <= (const multikey<D,K>& l, const multikey<D,K>& r) {
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
operator >= (const multikey<D,K>& l, const multikey<D,K>& r) {
	return !(l < r);
}

template <size_t D1, size_t D2, class K>
bool
operator >= (const multikey<D1,K>& l, const multikey<D2,K>& r) {
	return !(l < r);
}

//=============================================================================
// class multikey_generator_base method definitions

BASE_MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
multikey_generator_base<K>*
multikey_generator_base<K>::make_multikey_generator(const size_t d) {
	INVARIANT(d > 0 && d <= multikey_base<K>::LIMIT);
	// there may be some clever way to make a call table to
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1: return new multikey_generator<1,K>();
		case 2: return new multikey_generator<2,K>();
		case 3: return new multikey_generator<3,K>();
		case 4: return new multikey_generator<4,K>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//-----------------------------------------------------------------------------
// class multikey_generator method definitions

MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
template <template <class> class L, template <class, class> class P>
multikey_generator<D,K>::multikey_generator(const L<P<K,K> >& l) : base_type(), 
		interface_type(), lower_corner(), upper_corner() {
	typedef	L<P<K,K> >	sequence_type;
	INVARIANT(l.size() <= D);	// else error on user!
	iterator li = lower_corner.begin();
	iterator ui = upper_corner.begin();
	typename sequence_type::const_iterator i = l.begin();
	const typename sequence_type::const_iterator e = l.end();
	for ( ; l != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
template <class LP>
multikey_generator<D,K>::multikey_generator(const LP& l) : base_type(), 
		interface_type(), lower_corner(), upper_corner() {
	typedef	LP	sequence_type;
	INVARIANT(l.size() <= D);	// else error on user!
	iterator li = lower_corner.begin();
	iterator ui = upper_corner.begin();
	typename sequence_type::const_iterator i = l.begin();
	const typename sequence_type::const_iterator e = l.end();
	for ( ; l != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
void
multikey_generator<D,K>::validate(void) const {
	const_iterator min = lower_corner.begin();
	const_iterator max = upper_corner.begin();
	const const_iterator min_end = lower_corner.end();
	for ( ; min != min_end; min++, max++) {
		INVARIANT(*min <= *max);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
void
multikey_generator<D,K>::initialize(void) {
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

	multikey_base<K>&
	get_lower_corner(void) { return lower_corner; }

	const multikey_base<K>&
	get_lower_corner(void) const { return lower_corner; }

	multikey_base<K>&
	get_upper_corner(void) { return upper_corner; }

	const multikey_base<K>&
	get_upper_corner(void) const { return upper_corner; }
#endif

MULTIKEY_GENERATOR_TEMPLATE_SIGNATURE
multikey_base<K>&
multikey_generator<D,K>::operator ++ (int) {
#if 0
	iterator inc = --(base_type::end());
	const const_iterator msp = --(base_type::begin());
	const_iterator min = --(lower_corner.end());
	const_iterator max = --(upper_corner.end());
#else
	iterator inc = &this->indices[D-1];
	const const_iterator msp = &this->indices[-1];
	const_iterator min = &lower_corner.indices[D-1];
	const_iterator max = &upper_corner.indices[D-1];
#endif
	for ( ; inc != msp; inc--, min--, max--) {
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
#if 0
template <class K>
ostream&
operator << (ostream& o, const multikey_generator_base<K>& k) {
	const multikey_base<K>* mk =
		dynamic_cast<const multikey_base<K>*>(&k);
	INVARIANT(mk);
	return o << *mk;
}
#endif

//-----------------------------------------------------------------------------
// class multikey_generator_generic method definitions

MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE
template <template <class> class L, template <class, class> class P>
multikey_generator_generic<K>::multikey_generator_generic(
		const L<P<K,K> >& l) : base_type(), 
		interface_type(), lower_corner(), upper_corner() {
	typedef	L<P<K,K> >	sequence_type;
	INVARIANT(l.size() <= dimensions());	// else error on user!
	iterator li = lower_corner.begin();
	iterator ui = upper_corner.begin();
	typename sequence_type::const_iterator i = l.begin();
	const typename sequence_type::const_iterator e = l.end();
	for ( ; l != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE
template <class LP>
multikey_generator_generic<K>::multikey_generator_generic(const LP& l) :
		base_type(), interface_type(), lower_corner(), upper_corner() {
	typedef	LP	sequence_type;
	INVARIANT(l.size() <= dimensions());	// else error on user!
	iterator li = lower_corner.begin();
	iterator ui = upper_corner.begin();
	typename sequence_type::const_iterator i = l.begin();
	const typename sequence_type::const_iterator e = l.end();
	for ( ; l != e; i++, li++, ui++) {
		*li = i->first;
		*ui = i->second;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE
void
multikey_generator_generic<K>::validate(void) const {
	const_iterator min = lower_corner.begin();
	const_iterator max = upper_corner.begin();
	const const_iterator min_end = lower_corner.end();
	for ( ; min != min_end; min++, max++) {
		INVARIANT(*min <= *max);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_GENERATOR_GENERIC_TEMPLATE_SIGNATURE
void
multikey_generator_generic<K>::initialize(void) {
	validate();
	copy(lower_corner.begin(), lower_corner.end(), this->begin());
}

//=============================================================================
}	// end namespace MULTIKEY_NAMESPACE


#endif	//	__MULTIKEY_TCC__

