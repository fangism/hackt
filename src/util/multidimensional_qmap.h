// "multidimensional_qmap.h"
// a fixed depth/dimension tree representing sparsely instantiated indices
// David Fang, Cornell University, 2004

#ifndef	__MULTIDIMENSIONAL_QMAP_H__
#define	__MULTIDIMENSIONAL_QMAP_H__

#include <list>
#include <iostream>
#include <numeric>		// for accumulate

#include "sstream.h"		// used by "dump" method

// forward declarations
#include "multidimensional_qmap_fwd.h"
#include "multidimensional_sparse_set.h"
#include "qmap.h"		// queryable maps

/*
	Decision: Sub-trees of the map will be kept as count_ptr's
	for the sake of efficient copy constructibility.  
	This means that there may be multiple references to 
	subtree branches, which means deep-copying must be explicit.  


	TO DO: make lexicographical iterator through references.  
	Be able to go next, previous...
	Basically implement standard container interfaces.  
 */

/**
	Namespace containing multidimensional-sparse-set classes.  
 */
namespace MULTIDIMENSIONAL_QMAP_NAMESPACE {
//=============================================================================
using namespace std;
using namespace QMAP_NAMESPACE;
using namespace MULTIDIMENSIONAL_SPARSE_SET_NAMESPACE;

//=============================================================================
// utility functions

/**
	Given a list of integers (type T), return a pair of iterators, 
	the first pointing to begin(), the second pointing to the last
	element, one before end().  
 */
template <template <class> class L, class T>
inline
pair<typename L<T>::const_iterator, typename L<T>::const_iterator>
make_iter_range(const L<T>& l) {
	typename L<T>::const_iterator e = l.end();
	e--;
	return make_pair(l.begin(), e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// forward declaration for population count helper
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
size_t
population(size_t val,
	const typename multidimensional_qmap<D,K,T,L>::value_type& i);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// forward declaration of clean helper
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
void
clean(typename multidimensional_qmap<D,K,T,L>::value_type& i);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// forward declaration of empty helper
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
bool
empty(const typename multidimensional_qmap<D,K,T,L>::value_type& i);

//=============================================================================
/**
	Abstract base-class for multidimensional queryable map.  
	Param K is the key type, typically an integer, or integer-like class.  
	Param T is the object's value type.  
	Param L is the container for indexing (list, vector).  
	// maybe introduce P for pointer class overriding?
	Interface: unlike base_multidimensional_sparse_set, operations
		cannot be done using ranges, but only one index at a time.  
	Index lists are generalized as a pair of iterators, may even be T*.  
 */
BASE_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
class base_multidimensional_qmap {
public:
	typedef	base_multidimensional_qmap<K,T,L>	this_type;
	typedef	L<K>					key_list_type;
	typedef	typename key_list_type::const_iterator	const_list_iterator;
	typedef	pair<const_list_iterator, const_list_iterator>
							index_arg_type;
public:
	static const size_t			LIMIT = 4;

public:
virtual	~base_multidimensional_qmap() { }

virtual bool empty(void) const = 0;
virtual void clear(void) = 0;
virtual	void clean(void) = 0;

virtual	size_t dimensions(void) const = 0;
virtual	size_t population(void) const = 0;

virtual	T& operator [] (const index_arg_type& i) = 0;
virtual	T operator [] (const index_arg_type& i) const = 0;

	/**
		For convenience, need not be virtual.  
	 */
	T& operator [] (const key_list_type& l) {
		// pure virtual call
		return (*this)[make_iter_range(l)];
	}

	/**
		For convenience, need not be virtual.  
	 */
	T operator [] (const key_list_type& l) const {
		// pure virtual call
		return static_cast<const this_type&>(*this)[make_iter_range(l)];
	}

// virtual bool probe(...) const;

virtual	bool erase(const index_arg_type& l) = 0;

	bool erase(const key_list_type& l) {
		return erase(make_iter_range(l));
	}

// another that dereferences one-level only, given an index
// cannot be specified here in base class

protected:
virtual	ostream& dump(ostream& o, const string& pre) const = 0;

public:
virtual	ostream& dump(ostream& o) const = 0;

// static functions
	/** virtually, a virtual constructor */
static	this_type* 
		make_multidimensional_qmap(const size_t d);
};	// end class base_multidimensional_qmap

//=============================================================================
#if 0
// not ready yet...
template <size_t D, class K, class T>
struct multdimensional_map_iterator_base {
	typedef	multidimensional_map_iterator_base*		_base_ptr;
	typedef	const multidimensional_map_iterator_base*	_const_base_ptr;

};	// end class multidimensional_map_iterator_base

template <size_t D, class K, class T>
struct multidimensional_map_iterator :
		public multidimensional_map_iterator_base<D,K,T> {
	typedef	T	value_type;
	typedef	T&	reference;
	typedef	T*	pointer;
	// typedef	...	difference_type;

	typedef	multidimensional_map_iterator<D,K,T>	this_type;

	// see bits/stl_tree.h of standard includes...

};	// end class multidimensional_map_iterator
#endif

//=============================================================================
/**
	A true multidimensional map, not just a flat map with
	N-ary keys.  
	A representation of sparse indices in a multidimensional array.
	Implemented as a fixed depth tree.  
	Parameter class should integer-like.  
	int D in template signature?
	Less memory, for multidimensional storage.  
	fixed dimension type-enforced?
	sparse_set<N> contains sparse_set<N-1>'s?
	problem: limits implementation's template depth.  
	Places limit on dimensionality of arrays...
	Is 4 enough?
 */
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
class multidimensional_qmap : public base_multidimensional_qmap<K, T, L> {
friend class multidimensional_qmap<D+1, K, T, L>;

public:
	typedef	base_multidimensional_qmap<K, T, L>	parent;
	typedef	multidimensional_qmap<D, K, T, L>	this_type;
	typedef	typename parent::index_arg_type		index_arg_type;
	typedef multidimensional_qmap<D-1, K, T, L>	child_type;
	typedef	child_type				map_value_type;
	typedef	typename parent::key_list_type		key_list_type;
	typedef	typename key_list_type::const_iterator	const_list_iterator;
	typedef	qmap<K, map_value_type>			map_type;
	typedef	typename map_type::iterator		map_iterator;
	typedef	typename map_type::const_iterator	const_map_iterator;
	typedef	typename map_type::reverse_iterator	reverse_map_iterator;
	typedef	typename map_type::const_reverse_iterator const_reverse_map_iterator;
	typedef	typename map_type::value_type		value_type;

protected:
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_qmap() : parent(), index_map() { }

	/** default copy constructor */
	multidimensional_qmap(const this_type& s) :
		parent(), index_map(s.index_map) { }

	/** default destructor */
	~multidimensional_qmap() { }

	size_t dimensions(void) const { return D; }

	/**
		Expurgates empty sub-maps bottom-up.
		Empty is defined as having no elements, or only elements
		that are equal to the default value.  
		This means Integer value classes that default to 0 
		may lose 0 values, and string classes will have empty 
		strings expurgated.  
		This is particularly useful for pointer-classes that 
		default to NULL.  
		Do not use naked-pointers.  
	 */
	void clean(void) {
#if 0
		for_each(index_map.begin(), index_map.end(), 
			unary_compose(
				mem_fun_ref(&child_type::clean), 
				_Select2nd<typename map_type::value_type>()
			)
		);
		remove_if(index_map.begin(), index_map.end(), 
			unary_compose(
				mem_fun_ref(&child_type::empty), 
				_Select2nd<typename map_type::value_type>()
			)
		);
#elif 1
		map_iterator i = index_map.begin();
		const const_map_iterator e = index_map.end();
		for ( ; i!=e; ) {
			i->second.clean();
			if (i->second.empty()) {
				map_iterator j = i;
				j++;
				index_map.erase(i);
				i = j;
			} else {
				i++;
			}
		}
#else
		for_each(index_map.begin(), index_map.end(), 
			MULTIDIMENSIONAL_QMAP_NAMESPACE::clean<D,K,T,L>);
		// problems with assignment = of pairs with const first
		remove_if(index_map.begin(), index_map.end(), 
			MULTIDIMENSIONAL_QMAP_NAMESPACE::empty<D,K,T,L>);
#endif
	}

	bool empty(void) const { return index_map.empty(); }

	void clear(void) { index_map.clear(); }

	/**
		Return true if entry was erased, i.e. previously existed.  
	 */
	bool erase(const index_arg_type& l) {
		const_list_iterator h = l.first;
		const_list_iterator t = l.second;
		map_iterator probe = index_map.find(*h);
		if (probe == index_map.end()) {
			return false;
		} else if (h == t) {
			// erase entire subrange
			index_map.erase(probe);
			return true;
		} else {
			index_arg_type sub(h, t);
			sub.first++;
			return probe->second.erase(sub);
		}
	}

	bool erase(const key_list_type& l) {
		return erase(make_iter_range(l));
	}

	size_t population(void) const {
		return accumulate(index_map.begin(), index_map.end(), 0,
			MULTIDIMENSIONAL_QMAP_NAMESPACE::population<D,K,T,L>);
	}

	T& operator [] (const index_arg_type& i) {
		assert(i.first != i.second);
		K k = *(i.first);
		index_arg_type sub(i.first, i.second);
		sub.first++;
		return index_map[k][sub];
	}

	/**
		Note this returns a *copy* of the actual instance, 
		not a direct reference.  
	 */
	T operator [] (const index_arg_type& i) const {
		assert(i.first != i.second);
		K k = *(i.first);
		index_arg_type sub(i.first, i.second);
		sub.first++;
		return static_cast<const map_type&>(index_map)[k][sub];
	}

	/**
		For convenience, need not be virtual.  
	 */
	T& operator [] (const key_list_type& l) {
		return (*this)[make_iter_range(l)];
	}

	/**
		For convenience, need not be virtual.  
	 */
	T operator [] (const key_list_type& l) const {
		return static_cast<const this_type&>(*this)[make_iter_range(l)];
	}

protected:
	/**
		Prints out list of all members recursively.  
		\param o the output stream.  
		\param pre is a prefix string passed by the caller.  
		\return the output stream.
	 */
	ostream& dump(ostream& o, const string& pre) const {
		const_map_iterator i = index_map.begin();
		const const_map_iterator e = index_map.end();
		for ( ; i!=e; i++) {
			ostringstream p;
			p << pre << '[' << i->first << ']';
			i->second.dump(o, p.str());
		}
		return o;
	}

public:
	ostream& dump(ostream& o) const { return dump(o, ""); }

};	// end class multidimensional_qmap

//-----------------------------------------------------------------------------
/**
	Specialization of a one-dimensional array.  
 */
BASE_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
class multidimensional_qmap<1,K,T,L> :
		public base_multidimensional_qmap<K,T,L> {
friend class multidimensional_qmap<2,K,T,L>;

public:
	typedef	base_multidimensional_qmap<K,T,L>	parent;
	typedef	multidimensional_qmap<1,K,T,L>		this_type;
	typedef	typename parent::index_arg_type		index_arg_type;
	typedef	typename parent::key_list_type		key_list_type;
	typedef	typename key_list_type::const_iterator	const_list_iterator;
	typedef	qmap<K,T>				map_type;
	typedef	typename map_type::iterator		map_iterator;
	typedef	typename map_type::const_iterator	const_map_iterator;
	typedef	typename map_type::value_type		value_type;

protected:
	/**
		Storing with this an interval map for efficiency.  
	 */
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_qmap() : parent(), index_map() { }

	/** default copy constructor */
	multidimensional_qmap(const multidimensional_qmap<1,K,T,L>& s) :
		parent(), index_map(s.index_map) { }

	/** default destructor */
	~multidimensional_qmap() { }

	bool empty(void) const { return index_map.empty(); }

	void clear(void) { index_map.clear(); }

	void clean(void) { index_map.clean(); }

	size_t dimensions(void) const { return 1; }

	size_t population(void) const {
		return index_map.size();
	}

	bool erase(const K i) {
		map_iterator probe = index_map.find(i);
		if (probe != index_map.end()) {
			index_map.erase(probe);
			return true;
		} else	return false;
	}

	bool erase(const index_arg_type& l) {
		assert(l.first == l.second);
		return erase(*(l.first));
	}

	bool erase(const key_list_type& l) {
		return erase(make_iter_range(l));
	}

	/**
		This will create an entry if it doesn't already exist.  
	 */
	T& operator [] (const index_arg_type& i) {
		assert(i.first == i.second);
		return index_map[*(i.first)];
	}

	/**
		Note this returns a *copy* of the actual instance, 
		not a direct reference.  
	 */
	T operator [] (const index_arg_type& i) const {
		assert(i.first == i.second);
		return static_cast<const map_type&>(index_map)[*(i.first)];
	}

	/**
		For convenience, need not be virtual.  
	 */
	T& operator [] (const key_list_type& l) {
		return (*this)[make_iter_range(l)];
	}

	/**
		For convenience, need not be virtual.  
	 */
	T operator [] (const key_list_type& l) const {
		return static_cast<const this_type&>(*this)[make_iter_range(l)];
	}

protected:
	/**
		Prints out list of all members recursively.  
		\param o the output stream.  
		\param pre is a prefix string passed by the caller.  
		\return the output stream.
	 */
	ostream& dump(ostream& o, const string& pre) const {
		const_map_iterator i = index_map.begin();
		const const_map_iterator e = index_map.end();
		for ( ; i!=e; i++) {
			o << pre << '[' << i->first << "] = "
				<< i->second << endl;
		}
		return o;
	}

public:
	ostream& dump(ostream& o) const { return dump(o, ""); }

};	// end class multidimensional_qmap

//=============================================================================
// method definitions

BASE_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
base_multidimensional_qmap<K,T,L>* 
base_multidimensional_qmap<K,T,L>::
make_multidimensional_qmap(const size_t d) {
	// slow switch-case, but we need constants
	assert(d > 0 && d <= LIMIT);			// HARD LIMIT
	// there may be some clever way to make a call table to 
	// the various constructors, but this is a rare operation: who cares?
	switch(d) {
		case 1:	return new multidimensional_qmap<1,K,T,L>();
		case 2:	return new multidimensional_qmap<2,K,T,L>();
		case 3:	return new multidimensional_qmap<3,K,T,L>();
		case 4:	return new multidimensional_qmap<4,K,T,L>();
		// add more cases if LIMIT is ever extended.
		default: return NULL;
	}
}

//=============================================================================
// utility function definitions

/**
	Helper binary function to accumulate population.
 */
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
inline
size_t
population(size_t val,
		const typename multidimensional_qmap<D,K,T,L>::value_type& i) {
	return val + i.second.population();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
inline
void
clean(typename multidimensional_qmap<D,K,T,L>::value_type& i) {
	i.second.clean();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
inline
bool
empty(const typename multidimensional_qmap<D,K,T,L>::value_type& i) {
	return (i.second.empty());
}

//=============================================================================
}	// end namespace MULTIDIMENSIONAL_QMAP_NAMESPACE

#endif	// __MULTIDIMENSIONAL_QMAP_H__

