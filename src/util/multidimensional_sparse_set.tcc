/**
	\file "util/multidimensional_sparse_set.tcc"
	Template method definitions for multidimensional_sparse_set.
	$Id: multidimensional_sparse_set.tcc,v 1.7 2005/07/20 21:01:00 fang Exp $
 */

#ifndef	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_TCC__
#define	__UTIL_MULTIDIMENSIONAL_SPARSE_SET_TCC__

#include "util/multidimensional_sparse_set.hh"

// predefine to suppress definition
#ifndef	EXTERN_TEMPLATE_UTIL_MULTIDIMENSIONAL_SPARSE_SET

#include <iostream>
#include "util/sstream.hh"		// used by the dumo method
#include "util/memory/count_ptr.tcc"

#ifdef	EXCLUDE_DEPENDENT_TEMPLATES_UTIL_MULTIDIMENSIONAL_SPARSE_SET
#define	EXTERN_TEMPLATE_UTIL_QMAP
#define	EXTERN_TEMPLATE_UTIL_DISCRETE_INTERVAL_SET
#endif

#include "util/qmap.tcc"
#include "util/discrete_interval_set.tcc"

namespace util {
using std::ostringstream;
#include "util/using_ostream.hh"

//=============================================================================
// class base_multidimensional_sparse_set method definitions

SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
multidimensional_sparse_set_traits<T,R,L>::match_range_list(
		const range_list_type& s, const range_list_type& t) {
	INVARIANT(s.size() == t.size());
	typename range_list_type::const_iterator si(s.begin());
	typename range_list_type::const_iterator ti(t.begin());
	for ( ; si!=s.end(); si++, ti++) {
		if (si->first != ti->first ||
				si->second != ti->second)
			return false;
		// else range matches, continue
	}
	return true;
}

//=============================================================================
// class multidimensional_sparse_set method definitions

MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
MULTIDIMENSIONAL_SPARSE_SET_CLASS::multidimensional_sparse_set() :
		index_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
MULTIDIMENSIONAL_SPARSE_SET_CLASS::multidimensional_sparse_set(
		const multidimensional_sparse_set& s) :
		index_map(s.index_map) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
MULTIDIMENSIONAL_SPARSE_SET_CLASS::~multidimensional_sparse_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
void
MULTIDIMENSIONAL_SPARSE_SET_CLASS::clear(void) {
	index_map.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
MULTIDIMENSIONAL_SPARSE_SET_CLASS::contains(const range_list_type& r) const {
	INVARIANT(r.size() <= D);
	if (r.empty()) return true;
	range_list_type sub_range(r);
	sub_range.pop_front();
	const T min(r.begin()->first);
	const T max(r.begin()->second);
	T j(min);
	for ( ; j<=max; j++) {
		const map_value_type probe(index_map[j]);
		if (probe) {
			if (!probe->contains(sub_range))
				return false;
		} else {
			return false;
		}
	}
	// if this point reached, all recursive calls returned true
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
MULTIDIMENSIONAL_SPARSE_SET_CLASS::add_ranges(const range_list_type& r) {
	INVARIANT(r.size() == D);
	bool overlap = false;
	const typename range_list_type::const_iterator i(r.begin());
	range_list_type sub_range(r);
	sub_range.pop_front();
	T j(i->first);
	for ( ; j <= i->second; j++) {
		// non-modifying probe first
		const map_value_type probe(AS_A(const map_type&, index_map)[j]);
		if (!probe) {
			index_map[j] = map_value_type(new child_type);
		}
		map_value_type sub(index_map[j]);
		INVARIANT(sub);
		if (sub->add_ranges(sub_range))
			overlap = true;
	}
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
template <class S>
bool
MULTIDIMENSIONAL_SPARSE_SET_CLASS::add_ranges(const S& r) {
	INVARIANT(r.size() == D);
	bool overlap = false;
	const typename S::const_iterator i(r.begin());
	range_list_type sub_range(r.begin(), r.end());	// copy ranges
	sub_range.pop_front();
	T j(i->first);
	for ( ; j <= i->second; j++) {
		// non-modifying probe first
		const map_value_type probe(AS_A(const map_type&, index_map)[j]);
		if (!probe) {
			index_map[j] = map_value_type(new child_type);
		}
		map_value_type sub(index_map[j]);
		INVARIANT(sub);
		if (sub->add_ranges(sub_range))
			overlap = true;
	}
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
MULTIDIMENSIONAL_SPARSE_SET_CLASS::delete_ranges(const range_list_type& r) {
	INVARIANT(r.size() <= D);
	bool overlap = false;
	const typename range_list_type::const_iterator i(r.begin());
	range_list_type sub_range(r);
	sub_range.pop_front();
	T j = i->first;
	for ( ; j <= i->second; j++) {
		// non-modifying probe first
		const map_value_type probe(AS_A(const map_type&, index_map)[j]);
		if (probe) {
			map_value_type sub(index_map[j]);
			INVARIANT(sub);
			if (sub->delete_ranges(sub_range))
				overlap = true;
			if (sub->empty())
				index_map.erase(j);
		}
	}
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
MULTIDIMENSIONAL_SPARSE_SET_CLASS::meet(const this_type& s) {
	// at this dimension, find indices common to both maps
	bool overlap = false;
	typename map_type::iterator i(index_map.begin());
	for ( ; i!=index_map.end(); i++) {
		// recall map_value_type is pointer-class
			// needs to be non-modifying query
		const map_value_type probe(s.index_map[i->first]);
		if (probe) {
			// recursive meet
			// now can use modifing lookup
			if (i->second->meet(*probe)) {
				overlap = true;
			}
		}
	}
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
MULTIDIMENSIONAL_SPARSE_SET_CLASS::join(const this_type& s) {
	bool overlap = false;
	typename map_type::const_iterator s_i(s.index_map.begin());
	for ( ; s_i!=s.index_map.end(); s_i++) {
		// recall map_value_type is pointer-class
		// modifying lookup is ok
		map_value_type probe(index_map[s_i->first]);
		if (probe) {
			// recursive join subtree
			// now can use modifing lookup
			if (probe->join(*s_i->second)) {
				overlap = true;
			}
		} else {
			// add entire subtree
			index_map[s_i->first] = s_i->second;
			overlap = true;
		}
	}
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
MULTIDIMENSIONAL_SPARSE_SET_CLASS::subtract(const this_type& s) {
	bool overlap = false;
	typename map_type::const_iterator i(s.index_map.begin());
	for ( ; i!=s.index_map.end(); i++) {
		// recall map_value_type is pointer-class
			// needs to be non-modifying query
		const map_value_type
			probe(AS_A(const map_type&, index_map)[i->first]);
		if (probe) {
			// recursive meet
			// now can use modifing lookup
			if (probe->subtract(*(i->second))) {
				overlap = true;
				// clean-up
				if (probe->empty())
					index_map.erase(i->first);
			}
		}
	}
	return overlap;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
ostream&
MULTIDIMENSIONAL_SPARSE_SET_CLASS::dump(ostream& o,
		const string& pre) const {
	typename map_type::const_iterator i(index_map.begin());
	for ( ; i!=index_map.end(); i++) {
		ostringstream p;
		p << pre << "[" << i->first << "]";
		i->second->dump(o, p.str());
	}
	return o;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
typename MULTIDIMENSIONAL_SPARSE_SET_CLASS::range_list_type
MULTIDIMENSIONAL_SPARSE_SET_CLASS::compact_dimensions(void) const {
	if (index_map.empty())
		return range_list_type();
	// check if this dimension is dense
	typename map_type::const_iterator i(index_map.begin());
	const typename map_type::const_reverse_iterator e(index_map.rbegin());
	const T min(i->first);
	const T max(e->first);
	const T diff(index_map.size() -1);
	if (max -min != diff)
		return range_list_type();       // not dense
	// else if sub-dimensions match
	range_list_type s(i->second->compact_dimensions());
	// should empty case be acceptable? no, depends on return value
	if (s.empty())
		return range_list_type();
	// match against s
	for (i++; i!=index_map.end(); i++) {
		const range_list_type t(i->second->compact_dimensions());
		if (t.empty())
			return range_list_type();
		else if (!traits_type::match_range_list(s,t))
			return range_list_type();
		// else continue checking...
	}
	// if this point is reached, all subdimensions match
	// prepend with this dimension's range, and return it.  
	range_type r(min, max);
	s.push_front(r);
	return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
typename MULTIDIMENSIONAL_SPARSE_SET_CLASS::range_list_type
MULTIDIMENSIONAL_SPARSE_SET_CLASS::query_compact_dimensions(
		const range_list_type& r) const {
	INVARIANT(r.size() <= D);
	if (r.empty())
		return compact_dimensions();
	// else proceed
	range_list_type sub(r);
	const typename range_list_type::const_iterator tb(sub.begin());
	const T min(tb->first);
	const T max(tb->second);
	INVARIANT(min <= max);
	sub.pop_front();
	T i = min;
	const map_value_type probe(AS_A(const map_type&, index_map)[i]);
	if (!probe)     // referencing un-instantiated index
		return range_list_type();
	range_list_type s(probe->query_compact_dimensions(sub));
	if (s.empty())
		return range_list_type();
	for (i++; i<=max; i++) {
		const map_value_type
			lprobe(AS_A(const map_type&, index_map)[i]);
		if (!lprobe)     // referencing un-instantiated index
			return range_list_type();
		const range_list_type
			t(lprobe->query_compact_dimensions(sub));
		if (t.empty())
			return range_list_type();
		else if (!traits_type::match_range_list(s,t))
			return range_list_type();
		// else continue checking...
	}
	// if this point is reached, all subdimensions match
	// prepend with this dimension's range, and return it.  
	const range_type nr(min, max);
	s.push_front(nr);
	return s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
template <class S>
typename MULTIDIMENSIONAL_SPARSE_SET_CLASS::range_list_type
MULTIDIMENSIONAL_SPARSE_SET_CLASS::query_compact_dimensions(
		const S& r) const {
	INVARIANT(r.size() <= D);
	if (r.empty())
		return compact_dimensions();
	// else proceed
	range_list_type sub(r.begin(), r.end());	// copy range
	const typename range_list_type::const_iterator tb(sub.begin());
	const T min(tb->first);
	const T max(tb->second);
	INVARIANT(min <= max);
	sub.pop_front();
	T i(min);
	const map_value_type probe(AS_A(const map_type&, index_map)[i]);
	if (!probe)     // referencing un-instantiated index
		return range_list_type();
	range_list_type s(probe->query_compact_dimensions(sub));
	if (s.empty())
		return range_list_type();
	for (i++; i<=max; i++) {
		const map_value_type
			lprobe(AS_A(const map_type&, index_map)[i]);
		if (!lprobe)     // referencing un-instantiated index
			return range_list_type();
		const range_list_type
			t(lprobe->query_compact_dimensions(sub));
		if (t.empty())
			return range_list_type();
		else if (!traits_type::match_range_list(s,t))
			return range_list_type();
		// else continue checking...
	}
	// if this point is reached, all subdimensions match
	// prepend with this dimension's range, and return it.  
	const range_type nr(min, max);
	s.push_front(nr);
	return s;
}

//=============================================================================
// class multidimensional_sparse_set method definitions

SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::multidimensional_sparse_set() :
		index_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::multidimensional_sparse_set(
		const SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS& s) :
		index_map(s.index_map) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::~multidimensional_sparse_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
void
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::clear(void) {
	index_map.clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::add_ranges(
		const range_list_type& r) {
	INVARIANT(r.size() == 1);
	const typename range_list_type::const_iterator i(r.begin());
	return index_map.add_range(i->first, i->second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
template <class S>
bool
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::add_ranges(const S& r) {
	INVARIANT(r.size() == 1);
	const typename S::const_iterator i(r.begin());
	return index_map.add_range(i->first, i->second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::contains(
		const range_list_type& r) const {
	INVARIANT(r.size() <= 1);
	if (r.empty()) return true;
	const T min(r.begin()->first);
	const T max(r.begin()->second);
	const typename map_type::const_iterator
		ret(index_map.contains_entirely(min, max));
	return (ret != index_map.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
bool
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::delete_ranges(const range_list_type& r) {
	INVARIANT(r.size() <= 1);
	if (r.empty()) return false;
	const typename range_list_type::const_iterator i(r.begin());
	return index_map.delete_range(i->first, i->second);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
ostream&
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::dump(ostream& o, 
		const string& pre) const {
	typename map_type::const_iterator i(index_map.begin());
	for ( ; i!=index_map.end(); i++) {
		T j = i->first;
		for ( ; j <= i->second; j++) {
			o << pre << "[" << j << "]" << endl;
		}
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
typename SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::range_list_type
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::compact_dimensions(void) const {
	range_list_type ret;
	if (index_map.size() == 1) {
		const typename map_type::const_iterator b(index_map.begin());
		const range_type r(b->first, b->second);
		ret.push_back(r);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
typename SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::range_list_type
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::query_compact_dimensions(
		const range_list_type& r) const {
	INVARIANT(r.size() <= 1);
	range_list_type ret;
	if (!r.empty()) {
		// only one element
		typename range_list_type::const_iterator f(r.begin());
		const T min(f->first);
		const T max(f->second);
		if (index_map.contains_entirely(min, max) != index_map.end()) {
			const range_type rng(min, max);
			ret.push_back(rng);
		}
	} else {
		// not specified, entire set must be compact, 
		// i.e. have only one entry (discrete interval)
		if (index_map.size() == 1) {
			const typename map_type::const_iterator
				m(index_map.begin());
			const range_type rng(m->first, m->second);
			ret.push_back(rng);
		}
		// else is not packed, return empty
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_TEMPLATE_SIGNATURE
template <class S>
typename SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::range_list_type
SPECIALIZED_MULTIDIMENSIONAL_SPARSE_SET_CLASS::query_compact_dimensions(
		const S& r) const {
	INVARIANT(r.size() <= 1);
	range_list_type ret;
	if (!r.empty()) {
		// only one element
		typename S::const_iterator f(r.begin());
		const T min(f->first);
		const T max(f->second);
		if (index_map.contains_entirely(min, max) != index_map.end()) {
			const range_type rng(min, max);
			ret.push_back(rng);
		}
	} else {
		// not specified, entire set must be compact, 
		// i.e. have only one entry (discrete interval)
		if (index_map.size() == 1) {
			const typename map_type::const_iterator
				m(index_map.begin());
			const range_type rng(m->first, m->second);
			ret.push_back(rng);
		}
		// else is not packed, return empty
	}
	return ret;
}

//=============================================================================
}	// end namespace util

#endif	// EXTERN_TEMPLATE_UTIL_MULTIDIMENSIONAL_SPARSE_SET

#endif	// __UTIL_MULTIDIMENSIONAL_SPARSE_SET_TCC__

