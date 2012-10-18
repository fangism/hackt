/**
	\file "util/swap_saver.hh"
	$Id: swap_saver.hh,v 1.2 2010/04/02 22:19:22 fang Exp $
 */

#ifndef	__UTIL_SWAP_SAVER_H__
#define	__UTIL_SWAP_SAVER_H__

#include "util/attributes.h"
#include "util/macros.h"

namespace util {
//=============================================================================
/**
	Helper struct to save away a value for the duration
	of a scope and restore the former value upon end-of-scope.  
	Useful for auto-balancing in a stack-like manner.  
	Instead of copy-assigning, this invokes the swap() member function.
	This can result in a significant performance boost by avoiding copying,
	when it suffices to swap internal pointers in many containers.
	TODO: use SFINAE to determine whether or not a swap member function
	exists, and if not, fallback to using std::swap().
 */
template <class T>
class swap_saver {
public:
	typedef	T		value_type;
private:
	value_type&		reference;
	value_type		saved_value;
public:
	explicit
	swap_saver(value_type& t) : reference(t), saved_value() {
		this->reference.swap(saved_value);
		// since saved_value was default constructed
		// reference will be fresh after swap
	}

	~swap_saver() { this->reference.swap(this->saved_value); }

private:
	// undefined copy-constructing
	explicit
	swap_saver(const swap_saver&);

	// undefined assignment
	swap_saver<T>&
	operator = (const swap_saver<T>&);

} __ATTRIBUTE_UNUSED__ ;	// end class swap_saver

//=============================================================================
}	// end namepace util

#endif	// __UTIL_SWAP_SAVER_H__

