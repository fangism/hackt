/**
	\file "util/value_saver.hh"
	$Id: value_saver.hh,v 1.5 2009/10/03 09:34:58 fang Exp $
 */

#ifndef	__UTIL_VALUE_SAVER_H__
#define	__UTIL_VALUE_SAVER_H__

#include "util/value_saver_fwd.hh"
#include "util/attributes.h"
#include "util/macros.h"

namespace util {
//=============================================================================
/**
	Helper struct to save away a value for the duration
	of a scope and restore the former value upon end-of-scope.  
	Useful for auto-balancing in a stack-like manner.  
 */
template <class T>
class value_saver {
public:
	typedef	T		value_type;
private:
	value_type&		reference;
	const value_type	saved_value;
public:
	explicit
	value_saver(value_type& t) : reference(t), saved_value(reference) { }

	value_saver(value_type& t, const value_type& v) :
		reference(t), saved_value(reference) {
		this->reference = v;
	}

	~value_saver() { this->reference = this->saved_value; }

private:
	// undefined copy-constructing
	explicit
	value_saver(const value_saver&);

	// undefined assignment
	value_saver<T>&
	operator = (const value_saver<T>&);

} __ATTRIBUTE_UNUSED__ ;	// end class value_saver

//=============================================================================
/**
	More versatile version that uses an underlying pointer, 
	and is more container-friendly.  
 */
template <class T>
class ptr_value_saver {
public:
	typedef	T		value_type;
private:
	value_type*		pointer;
	value_type		saved_value;
public:
	ptr_value_saver() : pointer(NULL) { }

	explicit
	ptr_value_saver(value_type& t) : pointer(&t), saved_value(t) { }

	ptr_value_saver(value_type& t, const value_type& v) :
		pointer(&t), saved_value(*pointer) {
		*this->pointer = v;
	}

	~ptr_value_saver() {
	if (this->pointer) {
		*this->pointer = this->saved_value;
	}
	}

	/**
		Once bound, cannot be re-bound, as safety measure.
	 */
	void
	bind(value_type& v) {
		MUST_BE_NULL(this->pointer);
		this->pointer = &v;
		saved_value = v;
	}

	bool
	is_bound(void) const { return this->pointer; }

	// restricted copy-constructing, like a move constructor
	ptr_value_saver(const ptr_value_saver& r) : 
		pointer(r.pointer), saved_value(r.saved_value) {
		MUST_BE_NULL(r.pointer);
	}

	// limited assignment
	ptr_value_saver<T>&
	operator = (const ptr_value_saver<T>& r) {
		MUST_BE_NULL(r.pointer);
		this->pointer = r.pointer;
		this->saved_value = r.saved_value;
		return *this;
	}

} __ATTRIBUTE_UNUSED__ ;	// end class ptr_value_saver

//=============================================================================
}	// end namepace util

#endif	// __UTIL_VALUE_SAVER_H__

