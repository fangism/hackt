/**
	\file "util/multidimensional_qmap.h"
	Fixed depth/dimension tree representing sparsely instantiated indices.
	$Id: multidimensional_qmap.h,v 1.14 2006/04/27 00:17:01 fang Exp $
 */
// David Fang, Cornell University, 2004

#ifndef	__UTIL_MULTIDIMENSIONAL_QMAP_H__
#define	__UTIL_MULTIDIMENSIONAL_QMAP_H__

#include <iosfwd>

#include "util/macros.h"

// forward declarations
#include "util/multidimensional_qmap_fwd.h"
	// includes <list>
#include "util/array_traits.h"
#include "util/qmap.h"
#include "util/static_assert.h"
#include "util/type_traits.h"

#define MULTIDIMENSIONAL_QMAP_CLASS	multidimensional_qmap<D,K,T,L>

/*
	Decision: Sub-trees of the map will be kept as count_ptr's
	for the sake of efficient copy constructibility.  
	This means that there may be multiple references to 
	subtree branches, which means deep-copying must be explicit.  


	TODO: make lexicographical iterator through references.  
	Be able to go next, previous...
	Basically implement standard container interfaces.  
	Use nested_iterator concept!
 */

namespace util {
//=============================================================================
using std::pair;
using std::string;
using std::ostream;

//=============================================================================
// utility functions

/**
	Given a list of integers (type T), return a pair of iterators, 
	the first pointing to begin(), the second pointing to the last
	element, one before end().  
 */
template <class L>
inline
pair<typename L::const_iterator, typename L::const_iterator>
make_iter_range(const L& l) {
	typename L::const_iterator e(l.end());
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
#if 0
// not ready yet...
// CONSIDER NESTED_ITERATOR!
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

	\param K is the key type, typically an integer, or integer-like class.  
	\param T is the object's value type.  
	\param L is the container for indexing (list, vector).  
 */
MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
class multidimensional_qmap {
friend class multidimensional_qmap<D+1, K, T, L>;
protected:
	typedef	multidimensional_qmap<D, K, T, L>	this_type;
	typedef multidimensional_qmap<D-1, K, T, L>	child_type;
	typedef	child_type				map_value_type;
public:
	typedef	L					key_list_type;
private:
	typedef	is_same<K, typename key_list_type::value_type>
							__constraint_type1;
	UTIL_STATIC_ASSERT_DEPENDENT(__constraint_type1::value);
public:
	typedef	typename key_list_type::const_iterator	const_list_iterator;
	typedef	pair<const_list_iterator, const_list_iterator>
							index_arg_type;
	typedef	typename default_qmap<K, map_value_type>::type
							map_type;
	typedef	typename map_type::size_type		size_type;
	typedef	typename map_type::iterator		map_iterator;
	typedef	typename map_type::const_iterator	const_map_iterator;
	typedef	typename map_type::reverse_iterator	reverse_map_iterator;
	typedef	typename map_type::const_reverse_iterator
						const_reverse_map_iterator;
	typedef	typename map_type::value_type		value_type;


public:
	// for array_traits<>
	enum { dim = D };

protected:
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_qmap();

	/** default copy constructor */
	multidimensional_qmap(const this_type& s);

	/** default destructor */
	~multidimensional_qmap();

	size_t
	dimensions(void) const { return D; }

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
	void
	clean(void);

	bool
	empty(void) const { return index_map.empty(); }

	void
	clear(void);

	/**
		Return true if entry was erased, i.e. previously existed.  
	 */
	bool
	erase(const index_arg_type& l);

	bool
	erase(const key_list_type& l) {
		return erase(make_iter_range(l));
	}

	size_type
	population(void) const;

	T&
	operator [] (const index_arg_type& i);

	/**
		Note this returns a *copy* of the actual instance, 
		not a direct reference.  
	 */
	T
	operator [] (const index_arg_type& i) const;

	/**
		For convenience, need not be virtual.  
	 */
	T&
	operator [] (const key_list_type& l) {
		return (*this)[make_iter_range(l)];
	}

	/**
		For convenience, need not be virtual.  
	 */
	T
	operator [] (const key_list_type& l) const {
		return AS_A(const this_type&, *this)[make_iter_range(l)];
	}

protected:
	/**
		Prints out list of all members recursively.  
		\param o the output stream.  
		\param pre is a prefix string passed by the caller.  
		\return the output stream.
	 */
	ostream&
	dump(ostream& o, const string& pre) const;

public:
	ostream&
	dump(ostream& o) const { return dump(o, ""); }

};	// end class multidimensional_qmap

//-----------------------------------------------------------------------------
/**
	Specialization of a one-dimensional array.  
 */
SPECIALIZED_MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
class multidimensional_qmap<1,K,T,L> {
friend class multidimensional_qmap<2,K,T,L>;
protected:
	typedef	multidimensional_qmap<1,K,T,L>		this_type;
public:
	typedef	L					key_list_type;
private:
	typedef	is_same<K, typename key_list_type::value_type>
							__constraint_type1;
	UTIL_STATIC_ASSERT_DEPENDENT(__constraint_type1::value);
public:
	typedef	typename key_list_type::const_iterator	const_list_iterator;
	typedef	pair<const_list_iterator, const_list_iterator>
							index_arg_type;
	typedef	typename default_qmap<K,T>::type	map_type;
	typedef	typename map_type::size_type		size_type;
	typedef	typename map_type::iterator		map_iterator;
	typedef	typename map_type::const_iterator	const_map_iterator;
	typedef	typename map_type::value_type		value_type;

public:
	// for array_traits<>
	enum { dim = 1 };

protected:
	/**
		Storing with this an interval map for efficiency.  
	 */
	map_type					index_map;

public:
	/** default empty constructor */
	multidimensional_qmap();

	/** default copy constructor */
	multidimensional_qmap(const this_type& s);

	/** default destructor */
	~multidimensional_qmap();

	bool
	empty(void) const { return index_map.empty(); }

	void
	clear(void) { index_map.clear(); }

	void
	clean(void) { index_map.clean(); }

	size_t
	dimensions(void) const { return 1; }

	size_type
	population(void) const { return index_map.size(); }

	bool
	erase(const K i);

	bool
	erase(const index_arg_type& l);

	bool
	erase(const key_list_type& l) {
		return erase(make_iter_range(l));
	}

	/**
		This will create an entry if it doesn't already exist.  
	 */
	T&
	operator [] (const index_arg_type& i) {
		INVARIANT(i.first == i.second);
		return index_map[*(i.first)];
	}

	/**
		Note this returns a *copy* of the actual instance, 
		not a direct reference.  
	 */
	T
	operator [] (const index_arg_type& i) const {
		INVARIANT(i.first == i.second);
		return AS_A(const map_type&, index_map)[*(i.first)];
	}

	/**
		For convenience, need not be virtual.  
	 */
	T&
	operator [] (const key_list_type& l) {
		return (*this)[make_iter_range(l)];
	}

	/**
		For convenience, need not be virtual.  
	 */
	T
	operator [] (const key_list_type& l) const {
		return AS_A(const this_type&, *this)[make_iter_range(l)];
	}

protected:
	/**
		Prints out list of all members recursively.  
		\param o the output stream.  
		\param pre is a prefix string passed by the caller.  
		\return the output stream.
	 */
	ostream&
	dump(ostream& o, const string& pre) const;

public:
	ostream& dump(ostream& o) const { return dump(o, ""); }

};	// end class multidimensional_qmap

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
}	// end namespace util

// specialization of array_traits
namespace util {

MULTIDIMENSIONAL_QMAP_TEMPLATE_SIGNATURE
struct array_traits<
		util::multidimensional_qmap<D,K,T,L> > {
	typedef	util::multidimensional_qmap<D,K,T,L>	array_type;
	enum { dimensions = array_type::dim };
};	// end struct array_traits

}	// end namespace util

#endif	// __UTIL_MULTIDIMENSIONAL_QMAP_H__

