/**
	\file "util/memory/free_list.h"
	Container-based free-list implementations.  
	Technically, this need not be in the memory namespace, 
	but this is used so frequently in memory management.  
	$Id: free_list.h,v 1.2.38.1 2006/05/04 23:16:50 fang Exp $
 */

#ifndef	__UTIL_MEMORY_FREE_LIST_H__
#define	__UTIL_MEMORY_FREE_LIST_H__

#include "util/macros.h"
// #include <slist>			// singly-linked list
#include <list>
#include <queue>
#include <stack>
// #include <deque>
#include <vector>
#include <set>

namespace util {
namespace memory {
using std::list;
using std::queue;
using std::priority_queue;
using std::stack;
using std::vector;
using std::set;

//=============================================================================
/**
	Definition of generic free-list interface.  
	Should we supply some default actions?
	\param Container the underlying container type for the free-list.
		It is assumed to have an empty() operation.  
 */
template <class Container>
struct free_list {
	typedef	Container				container_type;
	typedef	typename container_type::value_type	value_type;

	/**
		\param c the free-list container.  
		\return a unique value, such as an index or pointer
		in the free-list.
		\pre the container is NOT empty.  
	 */
	static
	value_type
	acquire(container_type& c);

	/**
		\param v the value to return to the container.  
	 */
	static
	void
	release(container_type& c, const value_type& v);

};	// end class free_list

//-----------------------------------------------------------------------------
/**
	Specialization for std::list.  
	Operates on the back of the list.
	Doesn't check for uniqueness.  
	Disadvantage: uses default list allocator to create
		and destroy nodes, which may be inefficient.  
	\param A is the allocator_type of the list.  
 */
template <class T, class A>
struct free_list<list<T, A> > {
	typedef	list<T, A>				container_type;
	typedef	typename container_type::value_type	value_type;

	static
	value_type
	acquire(container_type& c) {
		const value_type ret = c.back();
		c.pop_back();
		return ret;
	}

	static
	void
	release(container_type& c, const value_type& v) {
		c.push_back(v);
	}
};	// end class free_list (specialization)

//-----------------------------------------------------------------------------
/**
	Specialization for std::vector.  
	Operates on the back of the vector.
	Doesn't check for uniqueness.  
	Should be very efficient after the first few allocations.  
	Those can even be avoided by calling reserve() in advance.  
	\param A is the allocator_type of the vector.  
 */
template <class T, class A>
struct free_list<vector<T, A> > {
	typedef	vector<T, A>				container_type;
	typedef	typename container_type::value_type	value_type;

	static
	value_type
	acquire(container_type& c) {
		const value_type ret = c.back();
		c.pop_back();
		return ret;
	}

	static
	void
	release(container_type& c, const value_type& v) {
		c.push_back(v);
	}
};	// end class free_list (specialization)

//-----------------------------------------------------------------------------
/**
	Specialization for std::queue.  
	Operates on the front and back of the structure, FIFO.
	Doesn't check for uniqueness.  
	\param S is the underlying sequence type of the quueue, 
		which has a direct impact on the efficiency of operations. 
 */
template <class T, class S>
struct free_list<queue<T, S> > {
	typedef	queue<T, S>				container_type;
	typedef	typename container_type::value_type	value_type;

	static
	value_type
	acquire(container_type& c) {
		const value_type ret = c.front();
		c.pop();
		return ret;
	}

	static
	void
	release(container_type& c, const value_type& v) {
		c.push(v);
	}
};	// end class free_list (specialization)

//-----------------------------------------------------------------------------
/**
	Specialization for std::stack.  
	Operates on the front and back of the structure, FIFO.
	Doesn't check for uniqueness.  
	\param S is the underlying sequence type of the quueue, 
		which has a direct impact on the efficiency of operations. 
 */
template <class T, class S>
struct free_list<stack<T, S> > {
	typedef	stack<T, S>				container_type;
	typedef	typename container_type::value_type	value_type;

	static
	value_type
	acquire(container_type& c) {
		const value_type ret = c.top();
		c.pop();
		return ret;
	}

	static
	void
	release(container_type& c, const value_type& v) {
		c.push(v);
	}
};	// end class free_list (specialization)

//-----------------------------------------------------------------------------
/**
	Specialization for std::set.  
	Operates on the underlying tree structure.  
	DOES check for uniqueness!
	Has log(N) insertion time because entries remain sorted.  
	Removes from the set minimum.  
	\param S is the underlying sequence type of the queue, 
		which has a direct impact on the efficiency of operations. 
 */
template <class T, class C, class A>
struct free_list<set<T, C, A> > {
	typedef	set<T, C, A>				container_type;
	typedef	typename container_type::const_iterator	const_iterator;
	typedef	typename container_type::iterator	iterator;
	typedef	typename container_type::value_type	value_type;

	static
	value_type
	acquire(container_type& c) {
		const iterator b(c.begin());
		const value_type ret = *b;
		c.erase(b);
		return ret;
	}

	/**
		Consider using <algorithm> binary_search?
		Only appropriate for a sorted list.  
		This *can* check for uniqueness.  
	 */
	static
	void
	release(container_type& c, const value_type& v) {
#ifndef	DISABLE_INVARIANT
		const const_iterator e(c.find(v));
		INVARIANT(e == c.end());
#endif
		c.insert(v);
	}
};	// end class free_list (specialization)

//=============================================================================
// free_list helper functions

/**
	\param c the free-list. 
	\return an value from the free-list.
	\pre container c must NOT be empty().  
 */
template <class Container>
inline
typename Container::value_type
free_list_acquire(Container& c) {
	return free_list<Container>::acquire(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param c the free-list.
	\param v the value to reclaim.
	\post container c will not be empty, duh!
 */
template <class Container>
inline
void
free_list_release(Container& c, 
		const typename Container::value_type& v) {
	free_list<Container>::release(c, v);
}

//=============================================================================
}	// end namespace memory
}	// end namespace util

#endif	// __UTIL_MEMORY_FREE_LIST_H__

