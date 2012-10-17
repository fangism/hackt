/**
	\file "util/stack_saver.hh"
	$Id: $
 */

#ifndef	__UTIL_STACK_SAVER_H__
#define	__UTIL_STACK_SAVER_H__

#include "util/attributes.h"

namespace util {

//-----------------------------------------------------------------------------
/**
	\param S is a last-in-first-out container type, like a stack.
 */
template <class S>
class stack_saver {
	typedef	S					stack_type;
	typedef	typename stack_type::value_type		value_type;
	stack_type&					_stack;
public:
	explicit
	stack_saver(stack_type& s) : _stack(s) {
		_stack.push(value_type());
	}

	~stack_saver() {
		_stack.pop();
	}
} __ATTRIBUTE_UNUSED__ ;	// end class stack_saver

//-----------------------------------------------------------------------------
/**
	\param S is a last-in-first-out container type, like a stack, 
		can be list or vector or deque, where insertion/removal
		from back-end is available.
 */
template <class S>
class back_stack_saver {
	typedef	S					stack_type;
	typedef	typename stack_type::value_type		value_type;
	stack_type&					_stack;
public:
	explicit
	back_stack_saver(stack_type& s) : _stack(s) {
		_stack.push_back(value_type());
	}

	~back_stack_saver() {
		_stack.pop_back();
	}
} __ATTRIBUTE_UNUSED__ ;	// end class stack_saver

//-----------------------------------------------------------------------------
/**
	\param S is a last-in-first-out container type, like a stack, 
		can be list or dequeue, where insertion/removal from 
		front-end is available.
 */
template <class S>
class front_stack_saver {
	typedef	S					stack_type;
	typedef	typename stack_type::value_type		value_type;
	stack_type&					_stack;
public:
	explicit
	front_stack_saver(stack_type& s) : _stack(s) {
		_stack.push_front(value_type());
	}

	~front_stack_saver() {
		_stack.pop_front();
	}
} __ATTRIBUTE_UNUSED__ ;	// end class stack_saver

//-----------------------------------------------------------------------------
}	// end namespace util
#endif	// __UTIL_STACK_SAVER_H__
