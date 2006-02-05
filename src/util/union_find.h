/**
	\file "util/union_find.h"
	Classic union-find data structure.
	$Id: union_find.h,v 1.2 2006/02/05 19:46:15 fang Exp $
 */

#ifndef	__UTIL_UNION_FIND_H__
#define	__UTIL_UNION_FIND_H__

#include "util/macros.h"

namespace util {
//=============================================================================
/**
	Safely down-castable base class for template union_find.  
 */
class union_find_base {
private:
	typedef	union_find_base			this_type;
public:
	typedef	this_type*			find_type;
	typedef	const this_type*		const_find_type;
protected:
	/**
		Continuation pointer.  
		Chase the pointer until it finds itself.  
	 */
	find_type				next;

	union_find_base() : next(this) { }

	// trivial destructor

	/**
		Performs path-compression along the way.
	 */
	find_type
	__find(void) {
		NEVER_NULL(this->next);
		if (this->next != this->next->next) {
			this->next = this->next->__find();
		}
		return this->next;
	}

	void
	__union(union_find_base& r) {
		__find()->next = r.__find();
	}

private:
	// private, undefined copy-constructor
	explicit
	union_find_base(const union_find_base&);

};	// end class union_find_base

//=============================================================================
/**
	NOTE: this does NOT do any memory management.
 */
template <class T>
class union_find {
public:
	typedef	T			value_type;
	typedef	T			element_type;
	typedef	union_find<T>		this_type;
protected:
	this_type*			parent;

};	// end class union_find

//=============================================================================
/**
	Variation where the struct is derived from the element type.  
 */
template <class T>
class union_find_derived : public T {

};	// end class union_find_derived

//=============================================================================
}	// end namespace util

#endif	// __UTIL_UNION_FIND_H__

