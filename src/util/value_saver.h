/**
	\file "util/value_saver.h"
	$Id: value_saver.h,v 1.3 2007/02/27 05:37:44 fang Exp $
 */

#ifndef	__UTIL_VALUE_SAVER_H__
#define	__UTIL_VALUE_SAVER_H__

#include "util/value_saver_fwd.h"
#include "util/attributes.h"

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
}	// end namepace util

#endif	// __UTIL_VALUE_SAVER_H__

