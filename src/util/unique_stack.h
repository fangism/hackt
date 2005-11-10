/**
	\file "util/unique_stack.h"
	Stack that tracks uniqueness.
	Order-preserving set.  

	$Id: unique_stack.h,v 1.2 2005/11/10 02:13:11 fang Exp $
 */

#ifndef __UTIL_UNIQUE_STACK__
#define __UTIL_UNIQUE_STACK__

#include "util/macros.h"
#include <stack>
#include <set>
#include <algorithm>		// for find

namespace util {
using std::find;
//=============================================================================
/**
        Uses a set to track uniqueness.  
        Consider using extensible vector for speed, instead of stack.  
	Can also use sstack for stack for forward-only iteration.
	__gnu_cxx::hash_set provides constant time hashing.  
 */
template <class T, class Stack = std::stack<T>, class Set = std::set<T> >
class unique_stack {
public:
	typedef	T				value_type;
//	typedef	T&				reference;
	typedef	const T&			const_reference;
//	typedef	T*				pointer;
	typedef	const T*			const_pointer;
	typedef Set				set_type;
	typedef Stack				stack_type;
private:
	set_type				_set;
	stack_type				_stack;
public:
	/// default constructor
	unique_stack() : _set(), _stack() { }

	// default copy-constructor

	/// default destructor
	~unique_stack() { }

	/**
		\return whether or not the stack is empty.  
	 */
	bool
	empty(void) const {
		INVARIANT(_set.empty() == _stack.empty());
		return _set.empty();
	}

	/**
		\return number of elements in stack/set.
	 */
	size_t
	size(void) const {
		INVARIANT(_set.size() == _stack.size());
		return _set.size();
	}

	/**
		Conditionally appends a value to end of stack (and set) 
		\return false if it's not already in stack.  
	 */
	bool
	push(const_reference n) {
		const typename set_type::const_iterator probe(_set.find(n));
		// if not found, add
		if (probe == _set.end()) {
			_set.insert(n);
			_stack.push(n);
			return false;
		}
		else	return true;
	}

	/**
		Alias for push().
	 */
	bool
	insert(const_reference n) {
		return push(n);
	}

	/**
		Removes a value from the set/stack.
	 */
	void
	erase(const_reference n) {
		const typename set_type::iterator probe(_set.find(n));
		if (probe != _set.end()) {
			const typename stack_type::iterator
				l_probe(find(_stack.begin(),
					_stack.end(), n));
			INVARIANT(l_probe != _stack.end());
			_stack.erase(l_probe);
			_set.erase(probe);
		}
		// else not already in set
	}

};      // end class unique_stack

//=============================================================================
}	// end namespace util

#endif	// __UTIL_UNIQUE_STACK__

