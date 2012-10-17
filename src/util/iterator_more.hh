/**
	\file "util/iterator_more.hh"
	More iterator functionality.  
	TODO: define using a generic_insert_iterator with operation.
	$Id: iterator_more.hh,v 1.4 2007/02/26 22:01:06 fang Exp $
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
/**
	Analogous to std::insert_iterator, 
	but uses push() interface instead of insert.  
	This works for queues and stacks.  
 */
template<typename _Container>
class push_iterator :
	public std::iterator<std::output_iterator_tag, void, void, void, void> {
	typedef	push_iterator<_Container>	this_type;
protected:
	_Container*				container;

public:
	/// A nested typedef for the type of whatever container you used.
	typedef _Container          			container_type;
	typedef	typename container_type::value_type	value_type;

	/// The only way to create this %iterator is with a container.
	explicit 
	push_iterator(_Container& __x) : container(&__x) { }

	this_type&
	operator=(typename _Container::const_reference __value) { 
		container->push(__value);
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

};	// end class push_iterator

//-----------------------------------------------------------------------------
/**
	\return Instance of a front_insert_assign_iterator.  
 */
template<typename _Container>
inline
push_iterator<_Container>
pusher(_Container& __x) {
	return push_iterator<_Container>(__x);
}

//=============================================================================
/**
	Analogous to std::insert_iterator, 
	Unlike std::insert_iterator, however, this does not require an 
	additional iterator argument.  
	but uses insert() interface instead of push().  
	This works for sets.
 */
template<typename _Container>
class set_insert_iterator :
	public std::iterator<std::output_iterator_tag, void, void, void, void> {
	typedef	set_insert_iterator<_Container>	this_type;
protected:
	_Container*				container;

public:
	/// A nested typedef for the type of whatever container you used.
	typedef _Container          			container_type;
	typedef	typename container_type::value_type	value_type;

	/// The only way to create this %iterator is with a container.
	explicit 
	set_insert_iterator(_Container& __x) : container(&__x) { }

	this_type&
	operator=(typename _Container::const_reference __value) { 
		container->insert(__value);
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

};	// end class set_insert_iterator

//-----------------------------------------------------------------------------
/**
	\return Instance of a set_insert_iterator.  
 */
template<typename _Container>
inline
set_insert_iterator<_Container>
set_inserter(_Container& __x) {
	return set_insert_iterator<_Container>(__x);
}

//=============================================================================
/**
	Analogous to std::insert_iterator, 
	Unlike std::insert_iterator, however, this does not require an 
	additional iterator argument.  
	but uses insert() interface instead of push().  
	This works for maps, by inserting a key-value pair
	with a default constructed value.
 */
template<typename _Container>
class map_key_insert_iterator :
	public std::iterator<std::output_iterator_tag, void, void, void, void> {
	typedef	map_key_insert_iterator<_Container>	this_type;
protected:
	_Container*				container;

public:
	/// A nested typedef for the type of whatever container you used.
	typedef _Container          			container_type;
	typedef	typename container_type::key_type	key_type;
//	typedef	typename container_type::value_type	value_type;
//	typedef	typename container_type::mapped_type	mapped_type;

	/// The only way to create this %iterator is with a container.
	explicit 
	map_key_insert_iterator(_Container& __x) : container(&__x) { }

	this_type&
	operator=(const key_type& __key) { 
		(*container)[__key];
//		container->insert(value_type(__key, mapped_type()));
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

};	// end class set_insert_iterator

//-----------------------------------------------------------------------------
/**
	\return Instance of a map_key_insert_iterator.  
 */
template<typename _Container>
inline
map_key_insert_iterator<_Container>
map_key_inserter(_Container& __x) {
	return map_key_insert_iterator<_Container>(__x);
}

//=============================================================================
}	// end namespace util

#endif	// __UTIL_ITERATOR_MORE_H__

