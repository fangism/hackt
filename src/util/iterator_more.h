/**
	\file "util/iterator_more.h"
	More iterator functionality.  
	$Id: iterator_more.h,v 1.2 2005/05/19 18:43:35 fang Exp $
 */

#ifndef	__UTIL_ITERATOR_MORE_H__
#define	__UTIL_ITERATOR_MORE_H__

#include <iterator>

namespace util {
//=============================================================================
/**
	Similar in functionality to std::back_insert_iterator, 
	but different in implementation.  
 */
template<typename _Container>
class back_insert_assign_iterator :
	public std::iterator<std::output_iterator_tag, void, void, void, void> {
	typedef	back_insert_assign_iterator<_Container>		this_type;
protected:
	_Container* container;

public:
	/// A nested typedef for the type of whatever container you used.
	typedef _Container          			container_type;
	typedef	typename container_type::value_type	value_type;

	/// The only way to create this %iterator is with a container.
	explicit 
	back_insert_assign_iterator(_Container& __x) : container(&__x) { }

	this_type&
	operator=(typename _Container::const_reference __value) { 
		container->push_back(value_type());
		container->back() = __value;
		return *this;
	}

	/**
		We provide a non-const mechanism for cases
		of non-const rvalues, such as when transfer of ownership
		is involved.  
	 */
	this_type&
	operator=(typename _Container::reference __value) { 
		container->push_back(value_type());
		container->back() = __value;
		return *this;
	}

	/// Simply returns *this.
	this_type&
	operator*() { return *this; }

	/// Simply returns *this.  (This %iterator does not "move".)
	this_type& 
	operator++() { return *this; }

	/// Simply returns *this.  (This %iterator does not "move".)
	this_type
	operator++(int) { return *this; }

};	// end class back_insert_assign_iterator

//-----------------------------------------------------------------------------
/**
	\return Instance of a back_insert_assign_iterator.  
 */
template<typename _Container>
inline
back_insert_assign_iterator<_Container>
back_insert_assigner(_Container& __x) {
	return back_insert_assign_iterator<_Container>(__x);
}

//=============================================================================
/**
	Similar in functionality to std::front_insert_iterator, 
	but different in implementation.  
 */
template<typename _Container>
class front_insert_assign_iterator :
	public std::iterator<std::output_iterator_tag, void, void, void, void> {
	typedef	front_insert_assign_iterator<_Container>	this_type;
protected:
	_Container* container;

public:
	/// A nested typedef for the type of whatever container you used.
	typedef _Container          			container_type;
	typedef	typename container_type::value_type	value_type;

	/// The only way to create this %iterator is with a container.
	explicit 
	front_insert_assign_iterator(_Container& __x) : container(&__x) { }

	this_type&
	operator=(typename _Container::const_reference __value) { 
		container->push_front(value_type());
		container->front() = __value;
		return *this;
	}

	/**
		We provide a non-const mechanism for cases
		of non-const rvalues, such as when transfer of ownership
		is involved.  
	 */
	this_type&
	operator=(typename _Container::reference __value) { 
		container->push_front(value_type());
		container->front() = __value;
		return *this;
	}

	/// Simply returns *this.
	this_type&
	operator*() { return *this; }

	/// Simply returns *this.  (This %iterator does not "move".)
	this_type& 
	operator++() { return *this; }

	/// Simply returns *this.  (This %iterator does not "move".)
	this_type
	operator++(int) { return *this; }

};	// end class front_insert_assign_iterator

//-----------------------------------------------------------------------------
/**
	\return Instance of a front_insert_assign_iterator.  
 */
template<typename _Container>
inline
front_insert_assign_iterator<_Container>
front_insert_assigner(_Container& __x) {
	return front_insert_assign_iterator<_Container>(__x);
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_ITERATOR_MORE_H__

