// "list_of_ptr.h"
// useful template for list<T*>
// the alternative is to instantiate list<auto_ptr<T>>...

// NOTE: assumes EXCLUSIVE ownwership of the pointers contained!
// any violation thereof will require something more powerful,
// such as reference counting.  

#ifndef __LIST_OF_PTR_H__
#define __LIST_OF_PTR_H__

#include <list>

using std::list;

//=============================================================================
/// A list of exclusively owned pointers, based on std::list.  

/**
	The members of this class are pointers to objects of class T, 
	as the name suggests.  The pointers should be exclusive, i.e., 
	whereas other copies of the same pointer may read or write
	the object, only this list has responsibility for de-allocating
	the objects' memory.  This is generally useful for bottom-up 
	constructed objects such as syntax trees.  
 */
template <class T>
class list_of_ptr : virtual public list<T*> {
private:
	typedef	list<T*>				parent;
public:
	typedef	typename parent::iterator		iterator;
	typedef	typename parent::const_iterator		const_iterator;
	typedef	typename parent::reverse_iterator	reverse_iterator;
	typedef	typename parent::const_reverse_iterator	const_reverse_iterator;
protected:
	///< ownership flag for the entire array, all or none
	bool			own;
public:

/// The default constructor just creates an empty list.  
explicit list_of_ptr() : parent(), own(true) { }
/// non-transfer copy constructor
	list_of_ptr(const list_of_ptr<T>& l);

/// The destructor frees memory to non-NULL pointers in the list.  
virtual	~list_of_ptr();		// don't want to inline this

// overriding methods
virtual	void	pop_back(void);
virtual	void	pop_front(void);
virtual	void	clear(void);

// non-essential add-on methods
/*** already available
using	parent::push_back;
using	parent::push_front;
using	parent::begin;
using	parent::end;
using	parent::rbegin;
using	parent::rend;
***/

/// Releases memory to the destination list, transfering ownership.  
virtual	void release_append(list_of_ptr<T>& dest);
};	// end of class list_of_ptr

//=============================================================================
/// A list of read-only pointers, based on std::list.  

/**
	The members of this class are pointers to objects of class T, 
	as the name suggests.  The pointers should be exclusive, i.e., 
	whereas other copies of the same pointer may read or write
	the object, only this list has responsibility for de-allocating
	the objects' memory.  This is generally useful for bottom-up 
	constructed objects such as syntax trees.  
 */
template <class T>
class list_of_const_ptr : virtual public list<const T*> {
private:
	typedef	list<const T*>				parent;
public:
	typedef	typename parent::iterator		iterator;
	typedef	typename parent::const_iterator		const_iterator;
	typedef	typename parent::reverse_iterator	reverse_iterator;
	typedef	typename parent::const_reverse_iterator	const_reverse_iterator;
public:

/// The default constructor just creates an empty list.  
explicit list_of_const_ptr() : parent() { }
	list_of_const_ptr(const list<T*>& l);
	list_of_const_ptr(const parent& l);

virtual	~list_of_const_ptr();

/*** already available
using	parent::push_back;
using	parent::push_front;
using	parent::begin;
using	parent::end;
using	parent::rbegin;
using	parent::rend;
***/
};	// end of class list_of_const_ptr

//=============================================================================

#endif	// __LIST_OF_PTR_H__

