/**
	\file "multikey_assoc.tcc"
	Template method definitions for multikey_assoc class adapter.  
	$Id: multikey_assoc.tcc,v 1.2 2005/02/27 22:54:23 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_ASSOC_TCC__
#define	__UTIL_MULTIKEY_ASSOC_TCC__

#include "multikey_assoc.h"

#include <limits>
#include <functional>
#include <algorithm>
#include <iterator>


#define	DEBUG_SLICE		0

namespace util {
#include "using_ostream.h"
using std::pair;
using std::_Select1st;
using std::_Select2nd;
using std::numeric_limits;
USING_LIST

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
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::is_compact_slice(
		const key_list_type& l, const key_list_type& u) const {
	typedef key_list_pair_type	return_type;
	typedef typename key_type::generic_generator_type	generator_type;
	const size_t l_size = l.size();
	{       // check for consistency
		typedef typename key_list_type::const_iterator	list_iterator;
		INVARIANT(l_size == u.size());
		pair<list_iterator, list_iterator>
			mm = mismatch(l.begin(), l.end(), u.begin(),
				std::less_equal<index_type>()
			);
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
	key_list_type list_key(key_gen.begin(), key_gen.end());

	const return_type s(is_compact_slice(list_key));
	if (s.first.empty()) {
		INVARIANT(s.second.empty());
#if DEBUG_SLICE
		cerr << "foo1: s is empty." << endl;
#endif
		return s;
	}
	typename key_list_type::const_iterator
		s_first_start = s.first.begin(),
		s_second_start = s.second.begin();
	{
		size_t s_skip = 0;
		for ( ; s_skip < l_size; s_skip++,
			s_first_start++, s_second_start++
		);
	}

	key_gen++;
	for ( ; key_gen != key_gen.get_lower_corner(); key_gen++) {
		const key_list_type
			for_list_key(key_gen.begin(), key_gen.end());
		const return_type t(is_compact_slice(for_list_key));
		if (t.first.empty()) {
			INVARIANT(t.second.empty());
			return return_type();
		} else {
			// compare suffixes
			typename key_list_type::const_iterator
				t_first_start = t.first.begin(),
				t_second_start = t.second.begin();
			{
				size_t t_skip = 0;
				for ( ; t_skip < l_size; t_skip++,
					t_first_start++,
					t_second_start++
				);
			}
			if (!equal(t_first_start, t.first.end(),
					s_first_start) ||
				!equal(t_second_start, t.second.end(),
					s_second_start)) {
				return return_type();
			}
			// else continue checking
		}
	}
	// if this is reached, then all subdimensions matched
	const key_list_type
		ret_l(key_gen.get_lower_corner().begin(),
			key_gen.get_lower_corner().end());
	const key_list_type
		ret_u(key_gen.get_upper_corner().begin(),
			key_gen.get_upper_corner().end());
	return_type ret(ret_l, ret_u);
	copy(s_first_start, s.first.end(), back_inserter(ret.first));
	copy(s_second_start, s.second.end(), back_inserter(ret.second));

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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::is_compact_slice(const key_list_type& l) const {
	typedef key_list_pair_type	return_type;
#if DEBUG_SLICE
	{
	cerr << "In multikey_assoc::is_compact_slice(l): ";
	ostream_iterator<index_type> osi(cerr, ",");
	cerr << "l = {";
	copy(l.begin(), l.end(), osi);
	cerr << "}" << endl;
	}
#endif
	const size_t l_size = l.size();
	INVARIANT(l_size);
	INVARIANT(l_size <= D);
	// special case for ==?
	if (l_size == D) {
		// if value is default, consider it empty
		// should use find() instead of [] operator
#if 0
		return ((*this)[key_type(l)] == mapped_type()) ?
			return_type() :
			return_type(l,l);
#else
		const_iterator i = find(key_type(l));
#if 1
		return (i != this->end() && *i != value_type()) ?
			return_type(l,l) : return_type();
#else
		return (i != this->end() &&
			_Select2nd<value_type>()(*i) != mapped_type()) ?
			return_type(l,l) : return_type();
#endif
#endif
	}

	// else is under-specified
	key_list_type lower(l);
	key_list_type last(l);
	last.back()++;
#if DEBUG_SLICE && 0
	{
	ostream_iterator<index_type> osi(cerr, ",");
	cerr << "lower = ";
	copy(lower.begin(), lower.end(), osi);
	cerr << " last = ";
	copy(last.begin(), last.end(), osi);
	cerr << endl;
	}
#endif
	const_iterator lower_iter = lower_bound(key_type(lower));
	if (lower_iter == this->end()) {
		// then sub-range is empty
		return return_type();
	}
	const_iterator last_iter = --upper_bound(key_type(last));
	if (lower_iter == last_iter) {
		// then sub-range has one element, therefore is dense
		return_type ret;
#if 0
		copy(lower_iter->first.begin(), lower_iter->first.end(),
			back_inserter(ret.first));
		copy(lower_iter->first.begin(), lower_iter->first.end(),
			back_inserter(ret.second));
#else
		copy(_Select1st<value_type>()(*lower_iter).begin(), 
			_Select1st<value_type>()(*lower_iter).end(),
			back_inserter(ret.first));
		copy(_Select1st<value_type>()(*lower_iter).begin(),
			_Select1st<value_type>()(*lower_iter).end(),
			back_inserter(ret.second));
#endif
		return ret;
	}
#if DEBUG_SLICE && 0
	{
	ostream_iterator<index_type> osi(cerr, ",");
	cerr << "lower_iter->first = ";
	copy(lower_iter->first.begin(), lower_iter->first.end(), osi);
	cerr << " last_iter->first = ";
	copy(last_iter->first.begin(), last_iter->first.end(), osi);
	cerr << endl;
	}
#endif

	// get range of next dimension to check
#if 0
	const index_type start_index = lower_iter->first[l_size];
	const index_type end_index = last_iter->first[l_size];
#else
	const index_type
		start_index =_Select1st<value_type>()(*lower_iter)[l_size];
	const index_type
		end_index = _Select1st<value_type>()(*last_iter)[l_size];
#endif
	key_list_type upper(lower);
	lower.push_back(start_index);
	upper.push_back(end_index);
#if DEBUG_SLICE && 0
	{
	ostream_iterator<index_type> osi(cerr, ",");
	cerr << "lower = ";
	copy(lower.begin(), lower.end(), osi);
	cerr << " upper = ";
	copy(upper.begin(), upper.end(), osi);
	cerr << endl;
	}
#endif
	return is_compact_slice(lower, upper);
}

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
	if (this->empty()) 
		return return_type();
	const_iterator first = this->begin();
	const_iterator last = --(this->end());
#if 0
	// this seems WRONG
	key_list_type start, end;
#if 0
	start.push_back(first->first.front());
	end.push_back(last->first.front());
#else
	// _Select1st is specialized for multikey_set_element_derived
	// and maplikeset_element_derived.
	start.push_back(_Select1st<value_type>()(*first).front());
	end.push_back(_Select1st<value_type>()(*last).front());
#endif
	return is_compact_slice(start, end);
#else
	// this seems better...
	const key_list_pair_type ext(this->index_extremities());
	return is_compact_slice(ext.first, ext.second);
#endif
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::index_extremities(void) const {
	typedef key_list_pair_type	return_type;
	if (this->empty())
		return return_type();
	const const_iterator iter = this->begin();
	const const_iterator end = this->end();
#if 0
	key_pair_type ext(iter->first, iter->first);
#else
	const key_type&
		start = _Select1st<value_type>()(*iter);
	key_pair_type ext(start, start);
#endif
	ext = accumulate(iter, end, ext,
		typename key_type::accumulate_extremities());
	return_type ret;
	copy(ext.first.begin(), ext.first.end(),
		back_inserter(ret.first));
	copy(ext.second.begin(), ext.second.end(),
		back_inserter(ret.second));
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
	INVARIANT(l.size() == 1);
	INVARIANT(u.size() == 1);
	index_type k = l.front();
	INVARIANT(k <= u.front());
	for ( ; k <= u.front(); k++) {
		const const_iterator i = find(k);
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
	if (this->empty()) {
		return return_type();
	}
	const const_iterator first = this->begin();
	const const_iterator end = this->end();
	const_iterator last = end;
	last--;
#if 0
	// requires "compose.h"
	const size_t vcount =
	count_if(this->begin(), this->end(), 
		unary_compose(
			bind2nd(not_equal<T>, T()), 
			_Select2nd()
		)
	);
#else
#if 0
	index_type k = first->first;
	const index_type k_end = last->first;
#else
	index_type k = _Select1st<value_type>()(*first);
	const index_type k_end = _Select1st<value_type>()(*last);
#endif
	for ( ; k <= k_end; k++) {
		const const_iterator i = find(k);
		if (i == this->end() ||
			_Select2nd<value_type>()(*i) == mapped_type())
//		if (i == this->end() || i->second == mapped_type())
//		if ((*this)[k] == mapped_type())
		{        // static_cast const?
			return return_type();
		}
	}
	return_type ret;
#if 0
	ret.first.push_back(first->first);
	ret.first.push_back(last->first);
#else
	ret.first.push_back(_Select1st<value_type>()(*first));
//	ret.first.push_back(_Select1st<value_type>()(*last));	// WRONG!?!?
	ret.second.push_back(_Select1st<value_type>()(*last));
#endif
	return ret;
#endif
}


//=============================================================================
}	// end namespace util

#undef	DEBUG_SLICE

#endif	// __UTIL_MULTIKEY_ASSOC_TCC__

