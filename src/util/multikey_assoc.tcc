/**
	\file "util/multikey_assoc.tcc"
	Template method definitions for multikey_assoc class adapter.  
	$Id: multikey_assoc.tcc,v 1.8 2006/02/01 06:11:46 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_ASSOC_TCC__
#define	__UTIL_MULTIKEY_ASSOC_TCC__

#include "util/multikey_assoc.h"

// predefine to suppress template definition
#ifndef	EXTERN_TEMPLATE_UTIL_MULTIKEY_ASSOC

#include <iostream>
#include <limits>
#include <functional>
#include <algorithm>
#include <iterator>

#define	DEBUG_SLICE		0

#if DEBUG_SLICE
#include "util/stacktrace.h"
#define	STACKTRACE_MULTIKEY_ASSOC		STACKTRACE_VERBOSE
#else
#define	STACKTRACE_MULTIKEY_ASSOC
#endif

namespace util {
#include "util/using_ostream.h"
using std::pair;
using std::_Select1st;
using std::_Select2nd;
using std::numeric_limits;
using std::list;
#if DEBUG_SLICE
using std::ostream_iterator;
#endif

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
	Helper function for establishing dense subranges.  
	\pre l and u must have matching prefixes.  
	\return empty key list pair if not dense.
 */
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::__is_dense_subslice(const key_list_type& l) const {
	typedef key_list_pair_type		return_type;
	typedef	typename key_type::value_type	key_value_type;
	STACKTRACE_MULTIKEY_ASSOC;
	const size_t l_size = l.size();
	INVARIANT(l_size);
	INVARIANT(l_size <= D);
	key_list_type u(l);
	++u.back();
#if DEBUG_SLICE
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
	const_iterator lb(lower_bound(lk)), ub(lower_bound(uk));
#if DEBUG_SLICE
	cerr << "distance (lb,ub) = " << distance(lb, ub) << endl;
#endif
	const key_value_type next_min = _Select1st<value_type>()(*lb)[l_size];
	key_value_type next_max = next_min;
	// check for contiguity
	for ( ; lb != ub; ++lb) {
		const key_value_type n = _Select1st<value_type>()(*lb)[l_size];
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
#if DEBUG_SLICE
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
	STACKTRACE_MULTIKEY_ASSOC;
	typedef key_list_pair_type	return_type;
	typedef	typename key_type::value_type	key_value_type;
	typedef typename key_type::generic_generator_type	generator_type;
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

#if DEBUG_SLICE
	{
	cerr << "In multikey_assoc::is_compact_slice(l,u): ";
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
		const const_iterator e(this->end());
		// already filled dimensions,
		// just verify existence of each element indexed
		do {
			if (find(key_type(key_gen)) == e)
				{ return return_type(); }
			key_gen++;
		} while (key_gen != key_gen.get_lower_corner());
		return return_type(l, u);
	}
	// else check for compact subdimensions

	const_iterator lb(lower_bound(key_gen));
	key_list_type lower_list_key(key_gen.begin(), key_gen.end());
	key_gen++;
	const return_type ret(__is_dense_subslice(lower_list_key));
if (key_gen != key_gen.get_lower_corner()) {
	if (ret.first.empty()) {
		return ret;
	}
	const key_value_type next_min = ret.first.back();
	const key_value_type next_max = ret.second.back();
	for ( ; key_gen != key_gen.get_lower_corner(); key_gen++) {
		lower_list_key = key_list_type(key_gen.begin(), key_gen.end());
		const return_type __ret(__is_dense_subslice(lower_list_key));
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
	return is_compact_slice(next_l, next_u);
} else {
	// only have one entry
	if (!ret.first.empty())
		return is_compact_slice(ret.first, ret.second);
	else	return ret;
}

#if DEBUG_SLICE
	{
	cerr << "End of multikey_assoc::is_compact_slice(l,u): ";
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
	TODO: Suspiciously WRONG...
	The keys of the first and last element don't necessarily
	represent the extremities covered by the indexed members.  
	For example, [0][0], [0][4], [1][0].
 */
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::is_compact(void) const {
	typedef key_list_pair_type	return_type;
	STACKTRACE_MULTIKEY_ASSOC;
	if (this->empty()) 
		return return_type();
#if 0
	// useless code?
	const_iterator first = this->begin();
	const_iterator last = --(this->end());
#endif
	const key_list_pair_type ext(this->index_extremities());
	return is_compact_slice(ext.first, ext.second);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return a pair of keys whose indices are the minimum
		and maximum values seen in each dimension.  
 */
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::index_extremities(void) const {
	typedef key_list_pair_type	return_type;
	STACKTRACE_MULTIKEY_ASSOC;
	if (this->empty())
		return return_type();
	const const_iterator iter(this->begin());
	const const_iterator t_end(this->end());
	const key_type&
		start = _Select1st<value_type>()(*iter);
	key_pair_type ext(start, start);
	ext = accumulate(iter, t_end, ext,
		typename key_type::accumulate_extremities());
	return_type ret;
	copy(ext.first.begin(), ext.first.end(), back_inserter(ret.first));
	copy(ext.second.begin(), ext.second.end(), back_inserter(ret.second));
	return ret;
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
	typedef key_list_pair_type      return_type;
	STACKTRACE_MULTIKEY_ASSOC;
	INVARIANT(l.size() == 1);
	INVARIANT(u.size() == 1);
	index_type k = l.front();
	INVARIANT(k <= u.front());
	for ( ; k <= u.front(); k++) {
		const const_iterator i(find(k));
		if (i == this->end() ||
			_Select2nd<value_type>()(*i) == mapped_type())
//		if (i == this->end() || i->second == mapped_type())
//		if ((*this)[k] == mapped_type())
		{        // static_cast const?
			return return_type();
		}
	}
	// else success
	return return_type(l,u);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<1,C>::key_list_pair_type
multikey_assoc<1,C>::is_compact(void) const {
	typedef key_list_pair_type      return_type;
	STACKTRACE_MULTIKEY_ASSOC;
	if (this->empty()) {
		return return_type();
	}
	const const_iterator first(this->begin());
	const const_iterator this_end(this->end());
	const_iterator last(this_end);
	last--;
	index_type k = _Select1st<value_type>()(*first);
	const index_type k_end = _Select1st<value_type>()(*last);
	for ( ; k <= k_end; k++) {
		const const_iterator i(find(k));
		if (i == this_end ||
			_Select2nd<value_type>()(*i) == mapped_type())
//		if (i == this_end || i->second == mapped_type())
//		if ((*this)[k] == mapped_type())
		{        // static_cast const?
			return return_type();
		}
	}
	return_type ret;
	ret.first.push_back(_Select1st<value_type>()(*first));
	ret.second.push_back(_Select1st<value_type>()(*last));
	return ret;
}


//=============================================================================
}	// end namespace util

#undef	DEBUG_SLICE

#endif	// EXTERN_TEMPLATE_UTIL_MULTIKEY_ASSOC
#endif	// __UTIL_MULTIKEY_ASSOC_TCC__

