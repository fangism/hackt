/**
	\file "util/multikey_assoc.tcc"
	Template method definitions for multikey_assoc class adapter.  
	$Id: multikey_assoc.tcc,v 1.11 2009/02/18 00:22:53 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_ASSOC_TCC__
#define	__UTIL_MULTIKEY_ASSOC_TCC__

#include "util/multikey_assoc.hh"

// predefine to suppress template definition
#ifndef	EXTERN_TEMPLATE_UTIL_MULTIKEY_ASSOC

#include <iostream>
#include <limits>
#include <algorithm>
#include <iterator>
#include <numeric>			// for std::accumlate
#include "util/STL/functional.hh"
// consider using util::member_select instead of _Select...

#ifndef	DEBUG_MULTIKEY_ASSOC_SLICE
#define	DEBUG_MULTIKEY_ASSOC_SLICE		0
#endif

#if DEBUG_MULTIKEY_ASSOC_SLICE
#include "util/stacktrace.hh"
#define	STACKTRACE_MULTIKEY_ASSOC		STACKTRACE_VERBOSE
#else
#define	STACKTRACE_MULTIKEY_ASSOC
#endif

namespace util {
#include "util/using_ostream.hh"
using std::pair;
using std::distance;
using std::_Select1st;
using std::_Select2nd;
using std::numeric_limits;
using std::list;
using std::copy;
using std::back_inserter;
#if DEBUG_MULTIKEY_ASSOC_SLICE
using std::ostream_iterator;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a pair of keys whose indices are the minimum
		and maximum values seen in each dimension.  
 */
template <size_t D, class K>
template <class A>
typename multikey_assoc_compact_helper<D,K>::key_list_pair_type
multikey_assoc_compact_helper<D,K>::index_extremities(const A& a) {
	typedef	typename A::const_iterator	const_iterator;
	typedef	typename A::value_type		value_type;
	typedef key_list_pair_type		return_type;
	typedef	_Select1st<value_type>		key_selector;
	STACKTRACE_MULTIKEY_ASSOC;
	if (a.empty())
		return return_type();
	const const_iterator iter(a.begin()), t_end(a.end());
	// gcc-3.3 needs help parsing forwarding operators with ctors
	const key_type& start(key_selector().operator()(*iter));
	key_pair_type ext(start, start);
	ext = std::accumulate(iter, t_end, ext,
		typename key_type::accumulate_extremities());
	return_type ret;
	copy(ext.first.begin(), ext.first.end(), back_inserter(ret.first));
	copy(ext.second.begin(), ext.second.end(), back_inserter(ret.second));
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Algorithm: (TODO)
		Given range prefix L, find the range that expresses the next 
		compact dimension, if it is indeed compact.  
		For all prefixes in this prefix range, determine:
		* if the subslice is spanned by a single range (integer pair).  
		* whether all ranges over all prefixes are equal
		* terminate as soon as one of these does not hold.
		Recurse until dimensions exhausted.  
	\param l the lower bound of prefix, at least 1 dimension.
	\param u the upper bound of prefix, same dimensions as l.
 */
template <size_t D, class K>
template <class A>
typename multikey_assoc_compact_helper<D,K>::key_list_pair_type
multikey_assoc_compact_helper<D,K>::is_compact_slice(const A& a, 
		const key_list_type& l, const key_list_type& u) {
	STACKTRACE_MULTIKEY_ASSOC;
	typedef key_list_pair_type	return_type;
	typedef	typename key_type::value_type	key_value_type;
	typedef typename key_type::generic_generator_type	generator_type;
	typedef	typename A::const_iterator	const_iterator;
	const size_t l_size = l.size();
	INVARIANT(l_size);
	INVARIANT(l_size <= D);
	{       // check for consistency
		typedef typename key_list_type::const_iterator	list_iterator;
		INVARIANT(l_size == u.size());
		const pair<list_iterator, list_iterator>
			mm(mismatch(l.begin(), l.end(), u.begin(),
				std::less_equal<index_type>()));
		INVARIANT(mm.first == l.end() && mm.second == u.end());
	}

#if DEBUG_MULTIKEY_ASSOC_SLICE
	{
	cerr << "In multikey_assoc_compact_helper::is_compact_slice(l,u): ";
	ostream_iterator<index_type> osi(cerr, ",");
	cerr << "l = {";
	copy(l.begin(), l.end(), osi);
	cerr << "}, u = {";
	copy(u.begin(), u.end(), osi);
	cerr << "}" << endl;
	}
#endif

	generator_type key_gen(l_size);
	copy(l.begin(), l.end(), key_gen.get_lower_corner().begin());
	copy(u.begin(), u.end(), key_gen.get_upper_corner().begin());
	key_gen.initialize();
	if (l_size == D) {
		const const_iterator e(a.end());
		// already filled dimensions,
		// just verify existence of each element indexed
		do {
			if (a.find(key_type(key_gen)) == e)
				{ return return_type(); }
			key_gen++;
		} while (key_gen != key_gen.get_lower_corner());
		return return_type(l, u);
	}
	// else check for compact subdimensions

	// const_iterator lb(lower_bound(key_gen));
	key_list_type lower_list_key(key_gen.begin(), key_gen.end());
	key_gen++;
	const return_type ret(__is_dense_subslice(a, lower_list_key));
if (key_gen != key_gen.get_lower_corner()) {
	if (ret.first.empty()) {
		return ret;
	}
	const key_value_type next_min = ret.first.back();
	const key_value_type next_max = ret.second.back();
	for ( ; key_gen != key_gen.get_lower_corner(); key_gen++) {
		lower_list_key = key_list_type(key_gen.begin(), key_gen.end());
		const return_type __ret(__is_dense_subslice(a, lower_list_key));
		if (__ret.first.empty()) {
			return ret;
		} else if (__ret.first.back() != next_min) {
			return return_type();
		} else if (__ret.second.back() != next_max) {
			return return_type();
		}
		// else continue searching next key
	}
	// if this point is reached then recurse
	key_list_type next_l(l), next_u(u);
	next_l.push_back(next_min);
	next_u.push_back(next_max);
	return is_compact_slice(a, next_l, next_u);
} else {
	// only have one entry
	if (!ret.first.empty())
		return is_compact_slice(a, ret.first, ret.second);
	else	return ret;
}

#if DEBUG_MULTIKEY_ASSOC_SLICE
	{
	cerr << "End of multikey_assoc_compact_helper::is_compact_slice(l,u): ";
	ostream_iterator<index_type> osi(cerr, ",");
	cerr << "l = {";
	copy(l.begin(), l.end(), osi);
	cerr << "}, u = {";
	copy(u.begin(), u.end(), osi);
	cerr << "}" << endl;
	cerr << "ret.first = ";
	copy(ret.first.begin(), ret.first.end(), osi);
	cerr << " ret.second = ";
	copy(ret.second.begin(), ret.second.end(), osi);
	cerr << endl;
	}
#endif
	return ret;
}	// end method is_compact_slice

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Helper function for establishing dense subranges.  
	\pre l and u must have matching prefixes.  
	\return empty key list pair if not dense.
 */
template <size_t D, class K>
template <class A>
typename multikey_assoc_compact_helper<D,K>::key_list_pair_type
multikey_assoc_compact_helper<D,K>::__is_dense_subslice(
		const A& a, const key_list_type& l) {
	typedef key_list_pair_type		return_type;
	typedef	typename key_type::value_type	key_value_type;
	typedef	typename A::value_type		value_type;
	typedef	typename A::const_iterator	const_iterator;
	typedef	_Select1st<value_type>		key_selector;
	STACKTRACE_MULTIKEY_ASSOC;
	const size_t l_size = l.size();
	INVARIANT(l_size);
	INVARIANT(l_size <= D);
	key_list_type u(l);
	++u.back();
#if DEBUG_MULTIKEY_ASSOC_SLICE
	{
	cerr << "In multikey_assoc::__is_dense_subslice(l,u): ";
	ostream_iterator<index_type> osi(cerr, ",");
	cerr << "l = {";
	copy(l.begin(), l.end(), osi);
	cerr << "}, u = {";
	copy(u.begin(), u.end(), osi);
	cerr << "}" << endl;
	}
#endif
	// find the range bound by this key prefix
	// try to find one range that covers the interval densely
	key_type lk(l), uk(u);
	const_iterator lb(a.lower_bound(lk)), ub(a.lower_bound(uk));
#if DEBUG_MULTIKEY_ASSOC_SLICE
	cerr << "distance (lb,ub) = " << distance(lb, ub) << endl;
#endif
	const key_value_type next_min = key_selector()(*lb)[l_size];
	key_value_type next_max = next_min;
	// check for contiguity
	for ( ; lb != ub; ++lb) {
		const key_value_type n = key_selector()(*lb)[l_size];
		INVARIANT(n >= 0);	// monotonicity
		if (n - next_max > 1) {
			// then we have a break in the sequence
			return return_type();
		} else {
			next_max = n;
		}
	}
	// if we've made it here, we have a contiguous range, 
	// bound by [next_min, next_max]
	key_list_type next_l(l);
	key_list_type next_u(l);
	next_l.push_back(next_min);
	next_u.push_back(next_max);
#if DEBUG_MULTIKEY_ASSOC_SLICE
	{
	cerr << "next_min/max = " << next_min << ", " << next_max << endl;
	ostream_iterator<index_type> osi(cerr, ",");
	cerr << "next_l = {";
	copy(next_l.begin(), next_l.end(), osi);
	cerr << "}, next_u = {";
	copy(next_u.begin(), next_u.end(), osi);
	cerr << "}" << endl;
	}
#endif
	return return_type(next_l, next_u);
}	// end method __is_dense_subslice

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks to see if the collection's indices spanned by the
	min/max extremities are covered densely.  
 */
template <size_t D, class K>
template <class A>
typename multikey_assoc_compact_helper<D,K>::key_list_pair_type
multikey_assoc_compact_helper<D,K>::is_compact(const A& a) {
	typedef key_list_pair_type	return_type;
	STACKTRACE_MULTIKEY_ASSOC;
	if (a.empty()) 
		return return_type();
#if 0
	// useless code?
	const_iterator first = a.begin();
	const_iterator last = --(a.end());
#endif
	const key_list_pair_type ext(index_extremities(a));
	return is_compact_slice(a, ext.first, ext.second);
}

//-----------------------------------------------------------------------------
/**
	If distance between iterators matches the range spanned, 
	then range is densely populated.
 */
template <class K>
template <class A>
typename multikey_assoc_compact_helper<1,K>::key_list_pair_type
multikey_assoc_compact_helper<1,K>::is_compact_slice(const A& a, 
		const key_list_type& l, const key_list_type& u) {
	typedef key_list_pair_type      return_type;
	typedef	typename A::const_iterator	const_iterator;
	typedef	typename A::value_type		value_type;
//	typedef	typename A::mapped_type		mapped_type;
//	typedef	_Select1st<value_type>		key_selector;
	STACKTRACE_MULTIKEY_ASSOC;
	INVARIANT(l.size() == 1);
	INVARIANT(u.size() == 1);
	const const_iterator this_end(a.end());
#if 0
	index_type k = l.front();
	INVARIANT(k <= u.front());
	for ( ; k <= u.front(); k++) {
		const const_iterator i(a.find(k));
		if (i == this_end
//			|| key_selector()(*i) == mapped_type()
			)
//		if (i == this->end() || i->second == mapped_type())
//		if ((*this)[k] == mapped_type())
		{        // static_cast const?
#if DEBUG_MULTIKEY_ASSOC_SLICE
			cerr << "did not find key: " << k << endl;
#endif
			return return_type();
		}
	}
	// else success
	return return_type(l,u);
#else
	const index_type j = l.front(), k = u.front();
	INVARIANT(j <= k);
	const const_iterator lf(a.find(j)), uf(a.find(k));
	if (lf != this_end && uf != this_end && 
			distance(lf, uf) == k-j) {
		return return_type(l,u);
	} else {
		return return_type();
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class K>
template <class A>
typename multikey_assoc_compact_helper<1,K>::key_list_pair_type
multikey_assoc_compact_helper<1,K>::is_compact(const A& a) {
	typedef key_list_pair_type      return_type;
	typedef	typename A::const_iterator	const_iterator;
	typedef	typename A::value_type		value_type;
//	typedef	typename A::mapped_type		mapped_type;
	typedef	_Select1st<value_type>		key_selector;
//	typedef	_Select2nd<value_type>		value_selector;
	STACKTRACE_MULTIKEY_ASSOC;
	if (a.empty()) {
		return return_type();
	}
	const const_iterator first(a.begin()), this_end(a.end());
	const_iterator last(this_end);
	--last;
#if 0
	index_type k = key_selector()(*first);
	const index_type k_end = key_selector()(*last);
	for ( ; k <= k_end; k++) {
		const const_iterator i(a.find(k));
		if (i == this_end
//			|| value_selector()(*i) == mapped_type()
			)
//		if (i == this_end || i->second == mapped_type())
//		if ((*this)[k] == mapped_type())
		{	// static_cast const?
#if DEBUG_MULTIKEY_ASSOC_SLICE
			cerr << "did not find key: " << k << endl;
#endif
			return return_type();
		}
	}
	return_type ret;
	ret.first.push_back(key_selector()(*first));
	ret.second.push_back(key_selector()(*last));
	return ret;
#else
	const index_type j = key_selector()(*first),
		k = key_selector()(*last);
	INVARIANT(j <= k);
	const const_iterator lf(a.find(j)), uf(a.find(k));
	if (lf != this_end && uf != this_end && 
			distance(lf, uf) == k-j) {
		return_type ret;
		ret.first.push_back(key_selector()(*first));
		ret.second.push_back(key_selector()(*last));
		return ret;
	} else {
		return return_type();
	}
#endif
}

//=============================================================================
// class multikey_assoc method definitions

MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
multikey_assoc<D,C>::multikey_assoc() : assoc_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
multikey_assoc<D,C>::~multikey_assoc() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
template <class K>
typename multikey_assoc<D,C>::iterator
multikey_assoc<D,C>::lower_bound(const K& k) {
	const key_type x(k, numeric_limits<typename K::value_type>::min());
	return assoc_type::lower_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
template <class K>
typename multikey_assoc<D,C>::const_iterator
multikey_assoc<D,C>::lower_bound(const K& k) const {
	const key_type x(k, numeric_limits<typename K::value_type>::min());
	return assoc_type::lower_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
template <class K>
typename multikey_assoc<D,C>::iterator
multikey_assoc<D,C>::upper_bound(const K& k) {
	const key_type x(k, numeric_limits<typename K::value_type>::min());
	return assoc_type::upper_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
template <class K>
typename multikey_assoc<D,C>::const_iterator
multikey_assoc<D,C>::upper_bound(const K& k) const {
	const key_type x(k, numeric_limits<typename K::value_type>::min());
	return assoc_type::upper_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::iterator
multikey_assoc<D,C>::lower_bound(const index_type& k) {
	key_type x(numeric_limits<index_type>::min());
	x[0] = k;
	return assoc_type::lower_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::const_iterator
multikey_assoc<D,C>::lower_bound(const index_type& k) const {
	key_type x(numeric_limits<index_type>::min());
	x[0] = k;
	return assoc_type::lower_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::iterator
multikey_assoc<D,C>::upper_bound(const index_type& k) {
	key_type x(numeric_limits<index_type>::min());
	x[0] = k;
	return assoc_type::upper_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::const_iterator
multikey_assoc<D,C>::upper_bound(const index_type& k) const {
	key_type x(numeric_limits<index_type>::min());
	x[0] = k;
	return assoc_type::upper_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
template <class K>
typename multikey_assoc<D,C>::size_type
multikey_assoc<D,C>::count(const K& k) const {
	static const size_t dim2 = K::dim;
	key_type l(k);
	l[dim2-1]++;
	return distance(lower_bound(k), lower_bound(l));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::size_type
multikey_assoc<D,C>::count(const index_type i) const {
	return distance(lower_bound(i), lower_bound(i+1));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Erases all entries matching the prefix multikey.  
 */
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
template <class K>
typename multikey_assoc<D,C>::size_type
multikey_assoc<D,C>::erase(const K& k) {
	static const size_t dim2 = K::dim;
	if (dim2 < D) {
		K m(k);
		m[dim2-1]++;
		const iterator l(lower_bound(k));
		const iterator u(lower_bound(m));
		const size_type ret = distance(l,u);
		if (ret) assoc_type::erase(l,u);
		return ret;
	} else {        // D2 >= D
		const key_type l(k);
		const iterator f(assoc_type::find(l));
		if (f != this->end()) {
			assoc_type::erase(f);
			return 1;
		} else  return 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::size_type
multikey_assoc<D,C>::erase(const index_type i) {
	const iterator l(lower_bound(i));
	const iterator u(lower_bound(i+1));
	size_type ret = distance(l,u);
	if (ret) assoc_type::erase(l,u);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Algorithm: (TODO)
		Given range prefix L, find the range that expresses the next 
		compact dimension, if it is indeed compact.  
		For all prefixes in this prefix range, determine:
		* if the subslice is spanned by a single range (integer pair).  
		* whether all ranges over all prefixes are equal
		* terminate as soon as one of these does not hold.
		Recurse until dimensions exhausted.  
	\param l the lower bound of prefix, at least 1 dimension.
	\param u the upper bound of prefix, same dimensions as l.
 */
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::is_compact_slice(
		const key_list_type& l, const key_list_type& u) const {
	return compact_helper_type::is_compact_slice(*this, l, u);
}	// end method is_compact_slice

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Checks to see if the collection's indices spanned by the
	min/max extremities are covered densely.  
 */
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::is_compact(void) const {
	return compact_helper_type::is_compact(*this);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a pair of keys whose indices are the minimum
		and maximum values seen in each dimension.  
 */
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::index_extremities(void) const {
#if 0
	typedef key_list_pair_type	return_type;
	typedef	_Select1st<value_type>		key_selector;
	STACKTRACE_MULTIKEY_ASSOC;
	if (this->empty())
		return return_type();
	const const_iterator iter(this->begin());
	const const_iterator t_end(this->end());
	const key_type&
		start = key_selector()(*iter);
	key_pair_type ext(start, start);
	ext = accumulate(iter, t_end, ext,
		typename key_type::accumulate_extremities());
	return_type ret;
	copy(ext.first.begin(), ext.first.end(), back_inserter(ret.first));
	copy(ext.second.begin(), ext.second.end(), back_inserter(ret.second));
	return ret;
#else
	return compact_helper_type::index_extremities(*this);
#endif
}

//=============================================================================
// class multikey_assoc method definitions (specialized)

SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
multikey_assoc<1,C>::multikey_assoc() : assoc_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
multikey_assoc<1,C>::~multikey_assoc() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<1,C>::key_list_pair_type
multikey_assoc<1,C>::is_compact_slice(
		const key_list_type& l, const key_list_type& u) const {
	return compact_helper_type::is_compact_slice(*this, l, u);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<1,C>::key_list_pair_type
multikey_assoc<1,C>::is_compact(void) const {
	return compact_helper_type::is_compact(*this);
}


//=============================================================================
}	// end namespace util

#undef	DEBUG_MULTIKEY_ASSOC_SLICE

#endif	// EXTERN_TEMPLATE_UTIL_MULTIKEY_ASSOC
#endif	// __UTIL_MULTIKEY_ASSOC_TCC__

