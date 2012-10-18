/**
	\file "util/STL/back_inserter.hh"
	Contains a specialization to allow push_back operation
	on a valarray, or other pseudo-containers.  
	$Id: back_inserter.hh,v 1.1 2006/08/23 20:57:26 fang Exp $
 */

#ifndef	__UTIL_STL_BACK_INSERTER_H__
#define	__UTIL_STL_BACK_INSERTER_H__

#include <iterator>
#include "util/STL/valarray_fwd.hh"

namespace std {
//=============================================================================
/**
	Partial specialization to allow an inefficient push_back
	operation on valarrays.  
 */
template <class T>
class back_insert_iterator<valarray<T> > :
	public iterator<output_iterator_tag, void, void, void, void> {
public:
	typedef valarray<T>			container_type;
protected:
	container_type*				container;
public:
	explicit 
	back_insert_iterator(container_type& __x) : container(&__x) { }

	/**
		Inefficiently reallocates, but provides the right interface.
	 */
	back_insert_iterator&
	operator=(typename container_type::const_reference __value) { 
		container->resize(container->size()+1);
		container[container->size() -1] = __value;
		return *this;
	}

	back_insert_iterator& 
	operator*() { return *this; }

	back_insert_iterator& 
	operator++() { return *this; }
		      
	back_insert_iterator
	operator++(int) { return *this; }
};	// end class back_insert_iterator

//=============================================================================
}	// end namespace std

#endif	// __UTIL_STL_BACK_INSERTER_H__

