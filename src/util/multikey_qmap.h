/**
	\file "multikey_qmap.h"
	Multidimensional queryable map.  
	NOTE: this should erally be redefined as a template specialization, 
	not as a child class.
	$Id: multikey_qmap.h,v 1.11 2004/12/23 00:07:45 fang Exp $
 */

#ifndef	__MULTIKEY_QMAP_H__
#define	__MULTIKEY_QMAP_H__

#include "multikey_qmap_fwd.h"
#include "qmap.h"
#include "multikey_map.h"

namespace MULTIKEY_MAP_NAMESPACE {
using namespace MULTIKEY_NAMESPACE;
using QMAP_NAMESPACE::qmap;

//=============================================================================
/**
	Specialization of multikey_map with qmap as the underlying map type.  
 */
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
class multikey_map<D,K,T,qmap> :
		protected qmap<multikey<D,K>, T>, 
		public multikey_map_base<K,T> {
private:
	/** this is the representation-type */
	typedef	multikey_map_base<K,T>			interface_type;
	typedef qmap<multikey<D,K>, T>			map_type;
	typedef map_type				mt;
public:
	typedef typename mt::key_type			key_type;
	typedef typename mt::mapped_type		mapped_type;
	typedef typename mt::value_type			value_type;
	typedef typename mt::key_compare		key_compare;
	typedef typename mt::allocator_type		allocator_type;

	typedef typename mt::reference			reference;
	typedef typename mt::const_reference		const_reference;
	typedef typename mt::iterator			iterator;
	typedef typename mt::const_iterator		const_iterator;
	typedef typename mt::reverse_iterator		reverse_iterator;
	typedef typename mt::const_reverse_iterator	const_reverse_iterator;
	typedef typename mt::size_type			size_type;
	typedef typename mt::difference_type		difference_type;
	typedef typename mt::pointer			pointer;
	typedef typename mt::const_pointer		const_pointer;
	typedef typename mt::allocator_type		allocator_type;

        typedef typename interface_type::key_list_type  key_list_type;
        typedef typename interface_type::key_list_pair_type
                                                        key_list_pair_type;
        typedef pair<key_type, key_type>		key_pair_type;

public:
	// for array_traits<>
	static const size_t dim = D;

public:

	// need final overriders for virtual functions

	multikey_map();

	~multikey_map();

	size_t
	dimensions(void) const { return D; }

	bool
	empty(void) const { return map_type::empty(); }

	void
	clear(void);

	size_t
	population(void) const { return map_type::size(); }

	// specialization
	void
	clean(void);

	using map_type::begin;
	using map_type::end;
	using map_type::rbegin;
	using map_type::rend;

	template <size_t D2>
	iterator
	lower_bound(const multikey<D2,K>& k);

	template <size_t D2>
	const_iterator
	lower_bound(const multikey<D2,K>& k) const;

	template <size_t D2>
	iterator
	upper_bound(const multikey<D2,K>& k);

	template <size_t D2>
	const_iterator
	upper_bound(const multikey<D2,K>& k) const;

	/**
		How many members match the given prefix of sub-dimensions?
	 */
	template <size_t D2>
	size_type
	count(const multikey<D2,K>& k) const;

	/** specialization for D2 == 1 */
	size_type
	count(const K i) const; 

	template <size_t D2>
	size_type
	erase(const multikey<D2,K>& k);

	/** specialization of erase() for only 1 dimension specified */
	size_type
	erase(const K i);

	T&
	operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T
	operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
	}

	T&
	operator [] (const multikey_base<K>& k);

	T
	operator [] (const multikey_base<K>& k) const;

	/**
		Check length of list?
	 */
	T&
	operator [] (const list<K>& k);

	/**
		Check length of list?
	 */
	T
	operator [] (const list<K>& k) const;

	key_list_pair_type
	is_compact_slice(const key_list_type& l, const key_list_type& u) const;

	key_list_pair_type
	is_compact_slice(const key_list_type& l) const;

	key_list_pair_type
	is_compact(void) const;

	key_list_pair_type
	index_extremities(void) const;

	ostream&
	dump(ostream& o) const;

	ostream&
	write(ostream& o) const;

	istream&
	read(istream& i);

};	// end class multikey_qmap

//=============================================================================

BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
class multikey_map<1,K,T,qmap> :
		protected qmap<K,T>, public multikey_map_base<K,T> {
protected:
	typedef multikey_map_base<K,T>			interface_type;
	typedef qmap<K, T>				map_type;
	typedef map_type				mt;
public:
	typedef typename mt::key_type			key_type;
	typedef typename mt::mapped_type		mapped_type;
	typedef typename mt::value_type			value_type;
	typedef typename mt::key_compare		key_compare;
	typedef typename mt::allocator_type		allocator_type;

	typedef typename mt::reference			reference;
	typedef typename mt::const_reference		const_reference;
	typedef typename mt::iterator			iterator;
	typedef typename mt::const_iterator		const_iterator;
	typedef typename mt::reverse_iterator		reverse_iterator;
	typedef typename mt::const_reverse_iterator	const_reverse_iterator;
	typedef typename mt::size_type			size_type;
	typedef typename mt::difference_type		difference_type;
	typedef typename mt::pointer			pointer;
	typedef typename mt::const_pointer		const_pointer;
	typedef typename mt::allocator_type		allocator_type;

	typedef typename interface_type::key_list_type	key_list_type;
	typedef typename interface_type::key_list_pair_type
							key_list_pair_type;
	typedef pair<key_type, key_type>		key_pair_type;

	// for array_traits<>
	static const size_t dim = 1;
public:
	multikey_map();
	~multikey_map();

	bool
	empty(void) const {
		return map_type::empty();
	}

	void
	clear(void);

	size_t
	dimensions(void) const { return 1; }

	size_t
	population(void) const { return mt::size(); }

	// specialization
	void
	clean(void);

	using map_type::begin;
	using map_type::end;
	using map_type::rbegin;
	using map_type::rend;

	T&
	operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T
	operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
	}

	T&
	operator [] (const key_list_type& k);

	T
	operator [] (const key_list_type& k) const;

	T&
	operator [] (const multikey_base<K>& k);

	T
	operator [] (const multikey_base<K>& k) const;

	key_list_pair_type
	is_compact_slice(const key_list_type& l, const key_list_type& u) const;

	/**
		With one argument, is always true.
	 */
	key_list_pair_type
	is_compact_slice(const key_list_type& l) const {
		typedef key_list_pair_type	return_type;
		return return_type(l,l);
	}

	key_list_pair_type
	is_compact(void) const;


	ostream&
	dump(ostream& o) const;

	ostream&
	write(ostream& f) const;

	istream&
	read(istream& f);

	// all other methods are the same as general template class

};      // end class multikey_map (specialization)

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#endif	//	__MULTIKEY_QMAP_H__

