/**
	\file "multikey_assoc.tcc"
	Template method definitions for multikey_assoc class adapter.  
	$Id: multikey_assoc.tcc,v 1.1.2.2 2005/02/08 06:41:23 fang Exp $
 */

#ifndef	__UTIL_MULTIKEY_ASSOC_TCC__
#define	__UTIL_MULTIKEY_ASSOC_TCC__

#include "multikey_assoc.h"
// #include "multikey.tcc"

#include <limits>
#include <functional>
#include <algorithm>
#include <iterator>

// #include "IO_utils.tcc"

#define	DEBUG_SLICE		0

namespace util {
#include "using_ostream.h"
using std::pair;
using std::numeric_limits;
USING_LIST
#if 0
using util::write_value;
using util::read_value;
using util::write_map;
using util::read_map;
#endif
// using MULTIKEY_NAMESPACE::multikey_generator;

//=============================================================================
// class multikey_assoc method definitions

MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
multikey_assoc<D,C>::multikey_assoc() : assoc_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
multikey_assoc<D,C>::~multikey_assoc() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
void
multikey_assoc<D,C>::clean(void) {
	const T def;
	iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; ) {
		if (i->second == def) {
			iterator j = i;
			j++;
			assoc_type::erase(i);
			i = j;
		} else {
			i++;
		}
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
template <class K>
typename multikey_assoc<D,C>::iterator
multikey_assoc<D,C>::lower_bound(const K& k) {
	const key_type
		x(k.self_key(),
		numeric_limits<typename K::self_key_type::value_type>::min());
	// YUCK
	const typename assoc_type::key_type xx(x);
	return assoc_type::lower_bound(xx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
template <class K>
typename multikey_assoc<D,C>::const_iterator
multikey_assoc<D,C>::lower_bound(const K& k) const {
	const key_type
		x(k.self_key(),
		numeric_limits<typename K::self_key_type::value_type>::min());
	// YUCK
	const typename assoc_type::key_type xx(x);
	return assoc_type::lower_bound(xx);
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
//	typename assoc_type::key_type l(k);
	l[dim2-1]++;
	const typename assoc_type::key_type kk(k);
	const typename assoc_type::key_type ll(l);
	return distance(lower_bound(kk),
		lower_bound(ll));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::size_type
multikey_assoc<D,C>::count(const index_type i) const {
#if 0
	multikey<1,K> l;
	l[0] = i;
	key_type m(l);
	m[0]++;
	return distance(lower_bound(l), lower_bound(m));
#else
	return distance(lower_bound(i), lower_bound(i+1));
#endif
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Erases all entries matching the prefix multikey.  
 */
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
template <class K>
typename multikey_assoc<D,C>::size_type
multikey_assoc<D,C>::erase(const K& k) {
	static const size_t dim2 = K::self_key_type::dim;
	if (dim2 < D) {
		typename K::self_key_type m(k.self_key());
		m[dim2-1]++;
		const iterator l(lower_bound(k));
		const iterator u(lower_bound(m));
		const size_type ret = distance(l,u);
		if (ret) assoc_type::erase(l,u);
		return ret;
	} else {        // D2 >= D
		const typename assoc_type::key_type l(k.self_key());
		const iterator f(assoc_type::find(l));
		if (f != this->end()) {
			assoc_type::erase(f);
			return 1;
		} else  return 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 1
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::size_type
multikey_assoc<D,C>::erase(const index_type i) {
#if 0
	multikey<1,K> m, k;
	k[0] = i;
	m[0] = i+1;
	const iterator l(lower_bound(k));
	const iterator u(lower_bound(m));
#else
	const iterator l(lower_bound(i));
	const iterator u(lower_bound(i+1));
#endif
	size_type ret = distance(l,u);
	if (ret) assoc_type::erase(l,u);
	return ret;
}
#endif

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
		return (i != this->end() && *i != value_type()) ?
			return_type(l,l) : return_type();
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
		copy(lower_iter->first.begin(), lower_iter->first.end(),
			back_inserter(ret.first));
		copy(lower_iter->first.begin(), lower_iter->first.end(),
			back_inserter(ret.second));
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
	const index_type start_index = lower_iter->first[l_size];
	const index_type end_index = last_iter->first[l_size];
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
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::is_compact(void) const {
	typedef key_list_pair_type	return_type;
	if (empty()) 
		return return_type();
	const_iterator first = this->begin();
	const_iterator last = --(this->end());
	key_list_type start, end;
	start.push_back(first->first.front());
	end.push_back(last->first.front());
	return is_compact_slice(start, end);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
ostream&
multikey_assoc<D,C>::dump(ostream& o) const {
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++)
		o << i->first << " = " << i->second << endl;
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
typename multikey_assoc<D,C>::key_list_pair_type
multikey_assoc<D,C>::index_extremities(void) const {
	typedef key_list_pair_type	return_type;
	if (this->empty())
		return return_type();
	const_iterator iter = this->begin();
	const_iterator end = this->end();
#if 0
	typename key_type::accumulate_extremities ext(iter->first);
	for_each(iter, end, ext);	// passes a COPY of ext...
#else
	key_pair_type ext(iter->first, iter->first);
	ext = accumulate(iter, end, ext,
		typename key_type::accumulate_extremities());
#endif
	return_type ret;
	copy(ext.first.begin(), ext.first.end(),
		back_inserter(ret.first));
	copy(ext.second.begin(), ext.second.end(),
		back_inserter(ret.second));
	return ret;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
ostream&
multikey_assoc<D,C>::write(ostream& f) const {
	INVARIANT(f.good());
	write_value(f, population());
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++) {
		i->first.write(f);
		write_value(f, i->second);
	}
	return f;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
istream&
multikey_assoc<D,C>::read(istream& f) {
	INVARIANT(f.good());
	INVARIANT(empty());
	size_t size, i=0;
	read_value(f, size);
	for ( ; i<size; i++) {
		key_type key;
		mapped_type val;
		key.read(f);
		read_value(f, val);
		(*this)[key] = val;
	}
	return f;
}
#endif

//=============================================================================
// class multikey_assoc method definitions (specialized)

SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
multikey_assoc<1,C>::multikey_assoc() : assoc_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
multikey_assoc<1,C>::~multikey_assoc() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
void
multikey_assoc<1,C>::clean(void) {
	const T def;
	iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; ) {
		if (i->second == def) {
			iterator j = i;
			j++;
			this->erase(i);
			i = j;
		} else {
			i++;
		}
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
T&
multikey_assoc<1,C>::operator [] (const key_list_type& k) {
	INVARIANT(k.size() == 1);
	return assoc_type::operator[](k.front());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
T
multikey_assoc<1,C>::operator [] (const key_list_type& k) const {
	INVARIANT(k.size() == 1);
	return assoc_type::operator[](k.front());
}
#endif

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
		if ((*this)[k] == mapped_type()) {        // static_cast const?
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
	index_type k = first->first;
	for ( ; k <= last->first; k++) {
		if ((*this)[k] == mapped_type()) {        // static_cast const?
			return return_type();
		}
	}
	return_type ret;
	ret.first.push_back(first->first);
	ret.first.push_back(last->first);
	return ret;
#endif
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
ostream&
multikey_assoc<1,C>::dump(ostream& o) const {
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++)
		o << '[' << i->first << ']' << " = "
			<< i->second << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
ostream&
multikey_assoc<1,C>::write(ostream& f) const {
#if 0
	write_assoc(f, static_cast<const assoc_type&>(*this));
#else
	INVARIANT(f.good());
	write_value(f, population());
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++) {
#if 0
		i->first.write(f);      // invalid for D=1
		write_value(f, i->second);
#else
		util::write_key_value_pair(f, *i);
#endif
	}
#endif
	return f;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
SPECIALIZED_MULTIKEY_ASSOC_TEMPLATE_SIGNATURE
istream&
multikey_assoc<1,C>::read(istream& f) {
// strange Apple gcc-3.3 (build 1640) bug reporting
// undefined reference to blah with char_traints (TYPO in name-mangled libs)!
// when this is fixed, set following to #if 1, same above with write().
#if 0
	read_assoc(f, static_cast<assoc_type&>(*this));
#else
	INVARIANT(f.good());
	INVARIANT(empty());
	size_t size, i=0;
	read_value(f, size);
	for ( ; i<size; i++) {
#if 0
		key_type key;
		mapped_type val;
		key.read(f);            // invalid for D=1
		read_value(f, val);
		(*this)[key] = val;
#else
		pair<K, T> p;
		util::read_key_value_pair(f, p);
		(*this)[p.first] = p.second;
#endif
	}
#endif
	return f;
}
#endif

//=============================================================================
}	// end namespace util

#undef	DEBUG_SLICE

#endif	// __UTIL_MULTIKEY_ASSOC_TCC__

