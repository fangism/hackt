/**
	\file "multikey_qmap.h"
	Multidimensional queryable map.  
	NOTE: this should erally be redefined as a template specialization, 
	not as a child class.
	$Id: multikey_qmap.h,v 1.9 2004/12/15 23:31:13 fang Exp $
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
#if !SPECIALIZE_MULTIKEY_QMAP
/**
        Abstract base class for pseudo-multidimensional map.
        Implementation-independent.
	Resist temptation to derive from multikey_map_base.  
 */
BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
class multikey_qmap_base {
public:
	typedef multikey_qmap_base<K,T>			this_type;
	typedef typename multikey_map_base<K,T>::key_list_type
							key_list_type;
	typedef typename multikey_map_base<K,T>::key_list_pair_type
							key_list_pair_type;

public:
	static const size_t				LIMIT = 4;
public:
virtual ~multikey_qmap_base() { }

virtual size_t dimensions(void) const = 0;
virtual size_t population(void) const = 0;
	size_t size(void) const { return this->population(); }
virtual bool empty(void) const = 0;
virtual void clear(void) = 0;

virtual ostream& dump(ostream& o) const = 0;

virtual T& operator [] (const multikey_base<K>& k) = 0;
virtual T operator [] (const multikey_base<K>& k) const = 0;

virtual key_list_pair_type is_compact_slice(
		const key_list_type& l, const key_list_type& u) const = 0;
virtual key_list_pair_type is_compact_slice(const key_list_type& l) const = 0;
virtual key_list_pair_type is_compact(void) const = 0;

static  this_type* make_multikey_qmap(const size_t d);

};      // end class multikey_qmap_base
#endif

//=============================================================================
MULTIKEY_QMAP_TEMPLATE_SIGNATURE
#if SPECIALIZE_MULTIKEY_QMAP
class multikey_map<D,K,T,qmap> :
		protected qmap<multikey<D,K>, T>, 
		public multikey_map_base<K,T>
#else
class multikey_qmap :
		public multikey_map<D, K, T, qmap>, 
		public multikey_qmap_base<K,T> 
#endif
{
private:
	/** this is the representation-type */
#if SPECIALIZE_MULTIKEY_QMAP
	typedef	multikey_map_base<K,T>			interface_type;
	typedef qmap<multikey<D,K>, T>			map_type;
#else
	typedef	multikey_qmap_base<K,T>			interface_type;
	typedef multikey_map<D,K,T,qmap>		map_type;
#endif
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

#if SPECIALIZE_MULTIKEY_QMAP

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

	template <size_t D2, K init2>
	iterator
	lower_bound(const multikey<D2,K,init2>& k);

	template <size_t D2, K init2>
	const_iterator
	lower_bound(const multikey<D2,K,init2>& k) const;

	template <size_t D2, K init2>
	iterator
	upper_bound(const multikey<D2,K,init2>& k);

	template <size_t D2, K init2>
	const_iterator
	upper_bound(const multikey<D2,K,init2>& k) const;

	/**
		How many members match the given prefix of sub-dimensions?
	 */
	template <size_t D2, K init2>
	size_type
	count(const multikey<D2,K,init2>& k) const;

	/** specialization for D2 == 1 */
	size_type
	count(const K i) const; 

	template <size_t D2, K init2>
	size_type
	erase(const multikey<D2,K,init2>& k);

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

#else	// !SPECIALIZE_MULTIKEY_QMAP

	size_t
	dimensions(void) const { return map_type::dimensions(); }

	bool
	empty(void) const { return map_type::empty(); }

	void
	clear(void) { map_type::clear(); }

	size_t
	population(void) const { return map_type::population(); }

	// specialization
	void
	clean(void) {
		// same as qmap<multikey<D,K>, T>::clean()
		map_type::map_type::clean();
	}

	T& operator [] (const typename map_type::key_type& k) {
		return map_type::operator[](k);
	}

	T operator [] (const typename map_type::key_type& k) const {
		return map_type::operator[](k);
	}

	T& operator [] (const multikey_base<K>& k) {
		return map_type::operator[](k);
	}

	T operator [] (const multikey_base<K>& k) const {
		return map_type::operator[](k);
	}

	key_list_pair_type
	is_compact_slice(
		const key_list_type& l, const key_list_type& u) const {
		return map_type::is_compact_slice(l, u);
	}

	key_list_pair_type
	is_compact_slice(const key_list_type& l) const {
		return map_type::is_compact_slice(l);
	}

	key_list_pair_type
	is_compact(void) const {
		return map_type::is_compact();
	}

	ostream&
	dump(ostream& o) const { return map_type::dump(o); }

	ostream&
	write(ostream& o) const {
		return map_type::write(o);
	}

	istream&
	read(istream& i) {
		return map_type::read(i);
	}

	// everything else plain inherited
#endif	// SPECIALIZE_MULTIKEY_QMAP

};	// end class multikey_qmap

//=============================================================================
#if SPECIALIZE_MULTIKEY_QMAP

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
#endif	// SPECIALIZE_MULTIKEY_QMAP

//=============================================================================
#if !SPECIALIZE_MULTIKEY_QMAP
// No, I am NOT crazy, this is indeed a 0-D specialization
// class K is useless, but is kept for interface purposes

BASE_MULTIKEY_QMAP_TEMPLATE_SIGNATURE
class multikey_qmap<0,K,T> : public multikey_qmap_base<K,T> {
private:
	typedef	multikey_qmap_base<K,T>			interface_type;
        typedef typename interface_type::key_list_type  key_list_type;
        typedef typename interface_type::key_list_pair_type
                                                        key_list_pair_type;
public:
	// for array_traits<>
	static const size_t dim = 0;

protected:
	T			val;

public:
	// only default constructor
	~multikey_qmap() { }

	size_t dimensions(void) const { return 0; }
	size_t population(void) const { return 1; }
	bool empty(void) const { return false; }

	/** writes with default value */
	void clear(void) { val = T(); }

	operator const T& () const { return val; }
	operator T& () { return val; }

	/** should we allow this? */
	T& operator [] (const multikey_base<K>& k) {
		assert(0);
		return val;
	}

	/** should we allow this? */
	T operator [] (const multikey_base<K>& k) const {
		assert(0);
		return val;
	}

	/**
		Silly to even call this... perhaps assert-fail?
		\return empty pair of index lists, always.
	 */
	key_list_pair_type is_compact_slice(
		const key_list_type& l, const key_list_type& u) const {
		return key_list_pair_type();
	}

	/**
		Silly to even call this...
		\return empty pair of index lists, always.
	 */
	key_list_pair_type is_compact_slice(const key_list_type& l) const {
		return key_list_pair_type();
	}

	/**
		Silly to even call this...
		\return empty pair of index lists, always.
	 */
	key_list_pair_type is_compact(void) const {
		return key_list_pair_type();
	}

	ostream& dump(ostream& o) const {
		return o << val;
	}

};	// end class multikey_qmap (specialized)
#endif	// SPECIALIZE_MULTIKEY_QMAP

//=============================================================================
// static function definitions

#if !SPECIALIZE_MULTIKEY_QMAP
/**
	\param d dimensions of collection, may be 0.
 */
template <class K, class T>
multikey_qmap_base<K,T>*
multikey_qmap_base<K,T>::make_multikey_qmap(const size_t d) {
	// slow switch-case, but we need constants
	assert(d <= LIMIT);
	// there may be some clever way to make a call table to
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 0: return new multikey_qmap<0,K,T>();      
		case 1: return new multikey_qmap<1,K,T>();      
		case 2: return new multikey_qmap<2,K,T>();
		case 3: return new multikey_qmap<3,K,T>();
		case 4: return new multikey_qmap<4,K,T>();
		// add more cases if LIMIT is ever extended.      
		default: return NULL;
	}
}
#endif

//=============================================================================
}	// end namespace MULTIKEY_MAP_NAMESPACE

#if !SPECIALIZE_MULTIKEY_QMAP
namespace util {

MULTIKEY_QMAP_TEMPLATE_SIGNATURE
struct array_traits<MULTIKEY_MAP_NAMESPACE::multikey_qmap<D,K,T> > {
	typedef	MULTIKEY_MAP_NAMESPACE::multikey_qmap<D,K,T>
					array_type;
	static const size_t		dimensions = array_type::dim;
};	// end struct array_traits

}	// end namespace util
#endif	// SPECIALIZE_MULTIKEY_QMAP

#endif	//	__MULTIKEY_QMAP_H__

