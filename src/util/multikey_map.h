/**
	\file "multikey_map.h"
	Multidimensional map implemented as plain map with 
	multidimensional key.  
 */

#ifndef	__MULTIKEY_MAP_H__
#define	__MULTIKEY_MAP_H__

#include <list>
#include <utility>		// for std::pair
#include <limits>		// for numeric_limits specializations
#include <functional>
#include <algorithm>

// for debugging
#include <iterator>		// for ostream_iterator
#include <memory>		// for auto_ptr

#include "multikey.h"
	// includes <iostream>
#include "multikey_map_fwd.h"

namespace MULTIKEY_MAP_NAMESPACE {
using namespace std;
using MULTIKEY_NAMESPACE::multikey_base;
using MULTIKEY_NAMESPACE::multikey;
using MULTIKEY_NAMESPACE::multikey_generator_base;
using MULTIKEY_NAMESPACE::multikey_generator;

//=============================================================================
/**
	Abstract base class for pseudo-multidimensional map.
	Implementation-independent.  
 */
template <class K, class T>
class multikey_map_base {
public:
	typedef	pair<multikey_base<K>, multikey_base<K> >
							multikey_base_pair;
	typedef	multikey_map_base<K,T>			this_type;
	typedef	list<K>					key_list_type;
	typedef	pair<key_list_type, key_list_type >	key_list_pair_type;
public:
	static const size_t				LIMIT = 4;
public:
virtual	~multikey_map_base() { }

virtual	size_t dimensions(void) const = 0;
virtual	size_t population(void) const = 0;
	size_t size(void) const { return this->population(); }
virtual	bool empty(void) const = 0;
virtual	void clear(void) = 0;
virtual	void clean(void) = 0;

virtual	T& operator [] (const list<K>& k) = 0;
virtual	T operator [] (const list<K>& k) const = 0;
virtual	T& operator [] (const multikey_base<K>& k) = 0;
virtual	T operator [] (const multikey_base<K>& k) const = 0;

virtual	key_list_pair_type is_compact_slice(
		const key_list_type& l, const key_list_type& u) const = 0;
virtual	key_list_pair_type is_compact_slice(const key_list_type& l) const = 0;
virtual	key_list_pair_type is_compact(void) const = 0;

virtual	ostream& dump(ostream& o) const = 0;

template <template <class, class> class M>
static	this_type* make_multikey_map(const size_t d);

};	// end class multikey_map_base

//=============================================================================
/**
	Adapter class for pseudo-multidimensional maps.  
	\param D is the dimension.  
	\param K is the key type, such as integer.  
	\param T is the element type.  
	\param M is the map class.  {e.g. map, qmap}

	allocater? comparator?

	\example multikey_qmap_test.cc
 */
template <size_t D, class K, class T, template <class, class> class M>
class multikey_map : public M<multikey<D,K>, T>, multikey_map_base<K,T> {
protected:
	/** this is the representation-type */
	typedef	multikey_map_base<K,T>			interface_type;
	typedef	M<multikey<D,K>, T>			map_type;
	typedef	map_type				mt;
public:
	typedef	typename mt::key_type			key_type;
	typedef	typename mt::mapped_type		mapped_type;
	typedef	typename mt::value_type			value_type;
	typedef	typename mt::key_compare		key_compare;
	typedef	typename mt::allocator_type		allocator_type;

	typedef	typename mt::reference			reference;
	typedef	typename mt::const_reference		const_reference;
	typedef	typename mt::iterator			iterator;
	typedef	typename mt::const_iterator		const_iterator;
	typedef	typename mt::reverse_iterator		reverse_iterator;
	typedef	typename mt::const_reverse_iterator	const_reverse_iterator;
	typedef	typename mt::size_type			size_type;
	typedef	typename mt::difference_type		difference_type;
	typedef	typename mt::pointer			pointer;
	typedef	typename mt::const_pointer		const_pointer;
	typedef	typename mt::allocator_type		allocator_type;

	typedef	typename interface_type::key_list_type	key_list_type;
	typedef	typename interface_type::key_list_pair_type
							key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

public:
	/**
		Default empty constructor.  
	 */
	multikey_map() : map_type(), interface_type() { }

	/**
		Default destructor.
	 */
	~multikey_map() { }

	/**
		Whether or not this map contains any elements.
		Need final overrider here to resolve ambiguity.  
	 */
	bool
	empty(void) const {
		return map_type::empty();
	}

	/**
		Number of dimensions.
	 */
	size_t
	dimensions(void) const { return D; }

	/**
		\return The number of elements (leaves) in map.  
	 */
	size_t
	population(void) const { return mt::size(); }

	/**
		Removes all elements.
	 */
	void
	clear(void) {
		map_type::clear();
	}

	/**
		General method for removing default values.  
	 */
	void
	clean(void) {
		const T def;
		iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; ) {
			if (i->second == def) {
				iterator j = i;
				j++;
				map_type::erase(i);
				i = j;
			} else {
				i++;
			}
		}
	}

	/**
		\param k The key of the (key, value) pair to find.  
		\return First element >= key k, or end().  
	 */
	template <size_t D2, K init2>
	iterator
	lower_bound(const multikey<D2,K,init2>& k) {
		key_type x(k, numeric_limits<K>::min());
		return map_type::lower_bound(x);
	}

	template <size_t D2, K init2>
	const_iterator
	lower_bound(const multikey<D2,K,init2>& k) const {
		key_type x(k, numeric_limits<K>::min());
		return map_type::lower_bound(x);
	}

	/**
		\param k The key of the (key, value) pair to find.  
		\return First element > key k, or end().  
	 */
	template <size_t D2, K init2>
	iterator
	upper_bound(const multikey<D2,K,init2>& k) {
		key_type x(k, numeric_limits<K>::min());
		return map_type::upper_bound(x);
	}

	template <size_t D2, K init2>
	const_iterator
	upper_bound(const multikey<D2,K,init2>& k) const {
		key_type x(k, numeric_limits<K>::min());
		return map_type::upper_bound(x);
	}

	/**
		How many members match the given prefix of sub-dimensions?
	 */
	template <size_t D2, K init2>
	size_type
	count(const multikey<D2,K,init2>& k) const {
		key_type l(k);
		l[D2-1]++;
		return distance(lower_bound(k), lower_bound(l));
	}

	/** specialization for D2 == 1 */
	size_type
	count(const K i) const {
		multikey<1,K> l;
		l[0] = i;
		key_type m(l);
		m[0]++;
		return distance(lower_bound(l), lower_bound(m));
	}

	template <size_t D2, K init2>
	size_type
	erase(const multikey<D2,K,init2>& k) {
		if (D2 < D) {
			multikey<D2,K,init2> m(k);
			m[D2-1]++;
			const iterator l(lower_bound(k));
			const iterator u(lower_bound(m));
			size_type ret = distance(l,u);
			if (ret) map_type::erase(l,u);
			return ret;
		} else {	// D2 >= D
			const key_type l(k);
			const iterator f(map_type::find(l));
			if (f != this->end()) {
				map_type::erase(f);
				return 1;
			} else	return 0;
		}
	}

	/** specialization of erase() for only 1 dimension specified */
	size_type
	erase(const K i) {
		multikey<1,K> m, k;
		k[0] = i;
		m[0] = i+1;
		const iterator l(lower_bound(k));
		const iterator u(lower_bound(m));
		size_type ret = distance(l,u);
		if (ret) map_type::erase(l,u);
		return ret;
	}

	T& operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
	}

	/**
		Check length of list?
	 */
	T& operator [] (const list<K>& k) {
		assert(k.size() == D);
		multikey<D,K> dk(k);
		return map_type::operator[](dk);
	}

	/**
		Check length of list?
	 */
	T operator [] (const list<K>& k) const {
		assert(k.size() == D);
		key_type dk(k);
		return map_type::operator[](dk);
	}

	T& operator [] (const multikey_base<K>& k) {
		// what if initial value is different?
		const key_type* dk =
			dynamic_cast<const key_type*>(&k);
		assert(dk);
		return map_type::operator[](*dk);
	}

	T operator [] (const multikey_base<K>& k) const {
		// what if initial value is different?
		const key_type* dk =
			dynamic_cast<const key_type*>(&k);
		assert(dk);
		return map_type::operator[](*dk);
	}

#define DEBUG_SLICE	0

	/**
		Recursive routine to determine implicit desnsely 
		packed subslice.  
		\param r list of upper bound and lower bounds.  
		\return explicit indices for the subarray, or pair of empty
			lists if sub-array is not densely packed.
	 */
	key_list_pair_type
	is_compact_slice(const key_list_type& l, const key_list_type& u) const {
		typedef	key_list_pair_type	return_type;
		const size_t l_size = l.size();
		{	// check for consistency
			typedef typename list<K>::const_iterator
							list_iterator;
			assert(l_size == u.size());
			pair<list_iterator, list_iterator>
				mm = mismatch(l.begin(), l.end(), u.begin(), 
					less_equal<K>()
				);
			assert(mm.first == l.end() && mm.second == u.end());
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

		auto_ptr<multikey_generator_base<K> >
			key_gen( multikey_generator_base<K>::
				make_multikey_generator(l_size));
		assert(key_gen.get());
		copy(l.begin(), l.end(), key_gen->get_lower_corner().begin());
		copy(u.begin(), u.end(), key_gen->get_upper_corner().begin());
		key_gen->initialize();
		key_list_type list_key(key_gen->begin(), key_gen->end());

		const return_type s = is_compact_slice(list_key);
		if (s.first.empty()) {
			assert(s.second.empty());
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

		(*key_gen)++;
		for ( ; *key_gen != key_gen->get_lower_corner(); (*key_gen)++) {
			key_list_type
				for_list_key(key_gen->begin(), key_gen->end());
			const return_type t = is_compact_slice(for_list_key);
			if (t.first.empty()) {
				assert(t.second.empty());
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
		const key_list_type ret_l(key_gen->get_lower_corner().begin(), 
			key_gen->get_lower_corner().end());
		const key_list_type ret_u(key_gen->get_upper_corner().begin(), 
			key_gen->get_upper_corner().end());
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

	/**
		This version queries one set of indices only.  
		Is self-recursive.  
		\return pair of index lists representing explicit
			indices of a dense slice, or empty if failure.  
	 */
	key_list_pair_type
	is_compact_slice(const key_list_type& l) const {
		typedef	key_list_pair_type	return_type;
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
		assert(l_size);
		assert(l_size <= D);
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
#undef	DEBUG_SLICE

	/**
		"Is the entire set compact?"
		\return explicit indices covering the entire set if it
			is indeed compact, otherwise empty.  
	 */
	key_list_pair_type
	is_compact(void) const {
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

	ostream&
	dump(ostream& o) const {
		const_iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; i++)
			o << i->first << " = " << i->second << endl;
		return o;
	}

	/**
		Returns the extremities of the indicies in each dimension.
		If empty, returns empty lists.  
	 */
	key_list_pair_type
	index_extremities(void) const {
		typedef	key_list_pair_type	return_type;
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
	};

};	// end class multikey_map

//-----------------------------------------------------------------------------
/**
	Specialization for one-dimension: just use base map type.  
 */
template <class K, class T, template <class, class> class M>
class multikey_map<1,K,T,M> : public M<K,T>, public multikey_map_base<K,T> {
protected:
	typedef	multikey_map_base<K,T>			interface_type;
	typedef	M<K, T>					map_type;
	typedef	map_type				mt;
public:
	typedef	typename mt::key_type			key_type;
	typedef	typename mt::mapped_type		mapped_type;
	typedef	typename mt::value_type			value_type;
	typedef	typename mt::key_compare		key_compare;
	typedef	typename mt::allocator_type		allocator_type;

	typedef	typename mt::reference			reference;
	typedef	typename mt::const_reference		const_reference;
	typedef	typename mt::iterator			iterator;
	typedef	typename mt::const_iterator		const_iterator;
	typedef	typename mt::reverse_iterator		reverse_iterator;
	typedef	typename mt::const_reverse_iterator	const_reverse_iterator;
	typedef	typename mt::size_type			size_type;
	typedef	typename mt::difference_type		difference_type;
	typedef	typename mt::pointer			pointer;
	typedef	typename mt::const_pointer		const_pointer;
	typedef	typename mt::allocator_type		allocator_type;

	typedef	typename interface_type::key_list_type	key_list_type;
	typedef	typename interface_type::key_list_pair_type
							key_list_pair_type;
	typedef	pair<key_type, key_type>		key_pair_type;

public:
	multikey_map() : map_type(), interface_type() { }
	~multikey_map() { }

	bool
	empty(void) const {
		return map_type::empty();
	}

	void
	clear(void) {
		map_type::clear();
	}

	size_t
	dimensions(void) const { return 1; }

	size_t
	population(void) const { return mt::size(); }

	void
	clean(void) {
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

	T& operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
	}

	T& operator [] (const key_list_type& k) {
		assert(k.size() == 1);
		return map_type::operator[](k.front());
	}

	T operator [] (const key_list_type& k) const {
		assert(k.size() == 1);
		return map_type::operator[](k.front());
	}

	T& operator [] (const multikey_base<K>& k) {
		// what if initial value is different?
		const multikey<1,K>* dk =
			dynamic_cast<const multikey<1,K>*>(&k);
		assert(dk);
		return map_type::operator[]((*dk)[0]);
	}

	T operator [] (const multikey_base<K>& k) const {
		// what if initial value is different?
		const multikey<1,K>* dk =
			dynamic_cast<const multikey<1,K>*>(&k);
		assert(dk);
		return map_type::operator[]((*dk)[0]);
	}

	key_list_pair_type
	is_compact_slice(const key_list_type& l, const key_list_type& u) const {
		typedef	key_list_pair_type	return_type;
		assert(l.size() == 1);
		assert(u.size() == 1);
		K k = l.front();
		assert(k <= u.front());
		for ( ; k <= u.front(); k++) {
			if ((*this)[k] == T()) {	// static_cast const?
				return return_type();
			}
		}
		// else success
		return return_type(l,u);
	}

	/**
		With one argument, is always true.
	 */
	key_list_pair_type
	is_compact_slice(const key_list_type& l) const {
		typedef	key_list_pair_type	return_type;
		return return_type(l,l);
	}

	key_list_pair_type
	is_compact(void) const {
		typedef	key_list_pair_type	return_type;
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
			if ((*this)[k] == T()) {	// static_cast const?
				return return_type();
			}
		}
		return_type ret;
		ret.first.push_back(first->first);
		ret.first.push_back(last->first);
		return ret;
#endif
	}


	ostream&
	dump(ostream& o) const {
		const_iterator i = this->begin();
		const const_iterator e = this->end();
		for ( ; i!=e; i++)
			o << '[' << i->first << ']' << " = "
				<< i->second << endl;
		return o;
	}

	// all other methods are the same as general template class

};	// end class multikey_map specialization

//=============================================================================
// static function definitions

template <class K, class T>
template <template <class, class> class M>
multikey_map_base<K,T>*
multikey_map_base<K,T>::make_multikey_map(const size_t d) {
	// slow switch-case, but we need constants
	assert(d > 0 && d <= LIMIT);
	// there may be some clever way to make a call table to
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1: return new multikey_map<1,K,T,M>();
		case 2: return new multikey_map<2,K,T,M>();
		case 3: return new multikey_map<3,K,T,M>();
		case 4: return new multikey_map<4,K,T,M>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_MAP_H__

