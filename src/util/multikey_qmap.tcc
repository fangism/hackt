/**
	\file "multikey_qmap.tcc"
	Template method definitions for queryable multikey map.
	Almost entirely copied from multikey_map.tcc.
	$Id: multikey_qmap.tcc,v 1.5.24.2 2005/02/06 18:25:36 fang Exp $
 */

#ifndef	__MULTIKEY_QMAP_TCC__
#define	__MULTIKEY_QMAP_TCC__

#include "multikey.tcc"
#include "multikey_qmap.h"
#include "qmap.tcc"

#define	DEBUG_SLICE		0

namespace MULTIKEY_MAP_NAMESPACE {
#include "using_ostream.h"
using std::istream;
using std::pair;
using std::numeric_limits;
USING_LIST
using util::write_value;
using util::read_value;
using util::write_map;
using util::read_map;
using MULTIKEY_NAMESPACE::multikey_generator;

//=============================================================================
// class multikey_map method definitions

MULTIKEY_QMAP_TEMPLATE_SIGNATURE
multikey_map<D,K,T,qmap>::multikey_map() : map_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
multikey_map<D,K,T,qmap>::~multikey_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
void
multikey_map<D,K,T,qmap>::clear(void) {
	map_type::clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialized implementation of clean(), using qmap::clean().
 */
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
void
multikey_map<D,K,T,qmap>::clean(void) {
	map_type::clean();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
template <size_t D2>
typename multikey_map<D,K,T,qmap>::iterator
multikey_map<D,K,T,qmap>::lower_bound(const multikey<D2,K>& k) {
	key_type x(k, numeric_limits<K>::min());
	return map_type::lower_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
template <size_t D2>
typename multikey_map<D,K,T,qmap>::const_iterator
multikey_map<D,K,T,qmap>::lower_bound(const multikey<D2,K>& k) const {
	key_type x(k, numeric_limits<K>::min());
	return map_type::lower_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
template <size_t D2>
typename multikey_map<D,K,T,qmap>::iterator
multikey_map<D,K,T,qmap>::upper_bound(const multikey<D2,K>& k) {
	key_type x(k, numeric_limits<K>::min());
	return map_type::upper_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
template <size_t D2>
typename multikey_map<D,K,T,qmap>::const_iterator
multikey_map<D,K,T,qmap>::upper_bound(const multikey<D2,K>& k) const {
	key_type x(k, numeric_limits<K>::min());
	return map_type::upper_bound(x);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
template <size_t D2>
typename multikey_map<D,K,T,qmap>::size_type
multikey_map<D,K,T,qmap>::count(const multikey<D2,K>& k) const {
	key_type l(k);
	l[D2-1]++;
	return distance(lower_bound(k), lower_bound(l));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
typename multikey_map<D,K,T,qmap>::size_type
multikey_map<D,K,T,qmap>::count(const K i) const {
	multikey<1,K> l;
	l[0] = i;
	key_type m(l);
	m[0]++;
	return distance(lower_bound(l), lower_bound(m));
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
template <size_t D2>
typename multikey_map<D,K,T,qmap>::size_type
multikey_map<D,K,T,qmap>::erase(const multikey<D2,K>& k) {
	if (D2 < D) {
		multikey<D2,K> m(k);
		m[D2-1]++;
		const iterator l(lower_bound(k));
		const iterator u(lower_bound(m));
		size_type ret = distance(l,u);
		if (ret) map_type::erase(l,u);
		return ret;
	} else {        // D2 >= D
		const key_type l(k);
		const iterator f(map_type::find(l));
		if (f != this->end()) {
			map_type::erase(f);
			return 1;
		} else  return 0;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
typename multikey_map<D,K,T,qmap>::size_type
multikey_map<D,K,T,qmap>::erase(const K i) {
	multikey<1,K> m, k;
	k[0] = i;
	m[0] = i+1;
	const iterator l(lower_bound(k));
	const iterator u(lower_bound(m));
	size_type ret = distance(l,u);
	if (ret) map_type::erase(l,u);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
T&
multikey_map<D,K,T,qmap>::operator [] (const list<K>& k) {
	INVARIANT(k.size() == D);
	multikey<D,K> dk(k);
	return map_type::operator[](dk);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
T
multikey_map<D,K,T,qmap>::operator [] (const list<K>& k) const {
	INVARIANT(k.size() == D);
	key_type dk(k);
	return map_type::operator[](dk);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
typename multikey_map<D,K,T,qmap>::key_list_pair_type
multikey_map<D,K,T,qmap>::is_compact_slice(
		const key_list_type& l, const key_list_type& u) const {
	typedef key_list_pair_type	return_type;
	const size_t l_size = l.size();
	{       // check for consistency
		typedef typename list<K>::const_iterator list_iterator;
		INVARIANT(l_size == u.size());
		pair<list_iterator, list_iterator>
			mm = mismatch(l.begin(), l.end(), u.begin(),
				std::less_equal<K>()
			);
		INVARIANT(mm.first == l.end() && mm.second == u.end());
	}

#if DEBUG_SLICE
	{
	cerr << "In multikey_map::is_compact_slice(l,u): ";
	ostream_iterator<K> osi(cerr, ",");
	cerr << "l = {";
	copy(l.begin(), l.end(), osi);
	cerr << "}, u = {";
	copy(u.begin(), u.end(), osi);
	cerr << "}" << endl;
	}
#endif

	multikey_generator_generic<K> key_gen(l_size);
	copy(l.begin(), l.end(), key_gen.get_lower_corner().begin());
	copy(u.begin(), u.end(), key_gen.get_upper_corner().begin());
	key_gen.initialize();
	key_list_type list_key(key_gen.begin(), key_gen.end());

	const return_type s = is_compact_slice(list_key);
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
	// why not do-while?
	const multikey_generic<K>& last_key(key_gen.get_lower_corner());
	for ( ; key_gen != last_key; key_gen++) {
		const key_list_type
			for_list_key(key_gen.begin(), key_gen.end());
		const return_type t = is_compact_slice(for_list_key);
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
	cerr << "End of multikey_map::is_compact_slice(l,u): ";
	ostream_iterator<K> osi(cerr, ",");
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
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
typename multikey_map<D,K,T,qmap>::key_list_pair_type
multikey_map<D,K,T,qmap>::is_compact_slice(const key_list_type& l) const {
	typedef key_list_pair_type	return_type;
#if DEBUG_SLICE
	{
	cerr << "In multikey_map::is_compact_slice(l): ";
	ostream_iterator<K> osi(cerr, ",");
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
		return ((*this)[key_type(l)] == T()) ?
			return_type() :
			return_type(l,l);
	}

	// else is under-specified
	key_list_type lower(l);
	key_list_type last(l);
	last.back()++;
#if DEBUG_SLICE && 0
	{
	ostream_iterator<K> osi(cerr, ",");
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
	ostream_iterator<K> osi(cerr, ",");
	cerr << "lower_iter->first = ";
	copy(lower_iter->first.begin(), lower_iter->first.end(), osi);
	cerr << " last_iter->first = ";
	copy(last_iter->first.begin(), last_iter->first.end(), osi);
	cerr << endl;
	}
#endif

	// get range of next dimension to check
	K start_index = lower_iter->first[l_size];
	K end_index = last_iter->first[l_size];
	key_list_type upper(lower);
	lower.push_back(start_index);
	upper.push_back(end_index);
#if DEBUG_SLICE && 0
	{
	ostream_iterator<K> osi(cerr, ",");
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
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
typename multikey_map<D,K,T,qmap>::key_list_pair_type
multikey_map<D,K,T,qmap>::is_compact(void) const {
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
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
ostream&
multikey_map<D,K,T,qmap>::dump(ostream& o) const {
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++)
		o << i->first << " = " << i->second << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
typename multikey_map<D,K,T,qmap>::key_list_pair_type
multikey_map<D,K,T,qmap>::index_extremities(void) const {
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
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
ostream&
multikey_map<D,K,T,qmap>::write(ostream& f) const {
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
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
istream&
multikey_map<D,K,T,qmap>::read(istream& f) {
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


//=============================================================================
// class multikey_map method definitions (specialized)

BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
multikey_map<1,K,T,qmap>::multikey_map() : map_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
multikey_map<1,K,T,qmap>::~multikey_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
void
multikey_map<1,K,T,qmap>::clear(void) {
	map_type::clear();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialization of clean, using qmap::clean().
 */
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
void
multikey_map<1,K,T,qmap>::clean(void) {
	map_type::clean();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
T&
multikey_map<1,K,T,qmap>::operator [] (const key_list_type& k) {
	INVARIANT(k.size() == 1);
	return map_type::operator[](k.front());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
T
multikey_map<1,K,T,qmap>::operator [] (const key_list_type& k) const {
	INVARIANT(k.size() == 1);
	return map_type::operator[](k.front());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
typename multikey_map<1,K,T,qmap>::key_list_pair_type
multikey_map<1,K,T,qmap>::is_compact_slice(
		const key_list_type& l, const key_list_type& u) const {
	typedef key_list_pair_type      return_type;
	INVARIANT(l.size() == 1);
	INVARIANT(u.size() == 1);
	K k = l.front();
	INVARIANT(k <= u.front());
	for ( ; k <= u.front(); k++) {
		if ((*this)[k] == T()) {        // static_cast const?
			return return_type();
		}
	}
	// else success
	return return_type(l,u);
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
typename multikey_map<1,K,T,qmap>::key_list_pair_type
multikey_map<1,K,T,qmap>::is_compact(void) const {
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
	K k = first->first;
	for ( ; k <= last->first; k++) {
		if ((*this)[k] == T()) {        // static_cast const?
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
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
ostream&
multikey_map<1,K,T,qmap>::dump(ostream& o) const {
	const_iterator i = this->begin();
	const const_iterator e = this->end();
	for ( ; i!=e; i++)
		o << '[' << i->first << ']' << " = "
			<< i->second << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
ostream&
multikey_map<1,K,T,qmap>::write(ostream& f) const {
#if 0
	write_map(f, static_cast<const map_type&>(*this));
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
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
istream&
multikey_map<1,K,T,qmap>::read(istream& f) {
// strange Apple gcc-3.3 (build 1640) bug reporting
// undefined reference to blah with char_traints (TYPO in name-mangled libs)!
// when this is fixed, set following to #if 1, same above with write().
#if 0
	read_map(f, static_cast<map_type&>(*this));
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

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#undef	DEBUG_SLICE

#endif	// __MULTIKEY_QMAP_TCC__

