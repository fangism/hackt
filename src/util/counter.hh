/**
	\file "util/counter.hh"
	Very basic counter class template.  
	All counter are zero-initialized upon default construction.  
	$Id: counter.hh,v 1.2 2005/09/04 21:15:06 fang Exp $
 */

#ifndef	__UTIL_COUNTER_H__
#define	__UTIL_COUNTER_H__

namespace util {

/**
	counter that is always zero-initialized upon default construction.  
	\param T the underlying counter type, integer of some sort.  
 */
template <class T>
struct counter {
	/// the value of the counter.  
	T value;

	counter() : value(0) { }
	counter(const T& i) : value(i) { }
	// default copy-constructor
	~counter() { }

	/// implicit conversion operator
	operator T& () { return value; }
	/// implicit conversion operator (const)
	operator const T& () const { return value; }

};	// end class counter

}	// end namespace util

#endif	// __UTIL_COUNTER_H__

