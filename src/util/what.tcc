/**
	\file "what.tcc"
	Default implementation for user-defined type-names.
	$Id: what.tcc,v 1.3.10.1 2005/01/23 00:48:55 fang Exp $
 */

#ifndef	__UTIL_WHAT_TCC__
#define	__UTIL_WHAT_TCC__

#include "what.h"
#include <string>
#include <typeinfo>

namespace util {

//-----------------------------------------------------------------------------
/**
	Default static initializer of type name.  
	Depending on implementation of type_info::name, may or may not
	be human-readable.  
 */
UTIL_WHAT_TEMPLATE_SIGNATURE
typename what<T>::name_type
what<T>::name(void) {
	static const name_type local_name = typeid(T).name();
	return local_name;
}

// this must appear first, because everything below depends on it!
//-----------------------------------------------------------------------------
#if UTIL_WHAT_PARTIAL_SPECIALIZATIONS

UTIL_WHAT_TEMPLATE_SIGNATURE
typename what<const T>::name_type
what<const T>::name(void) {
	static const string name_string =
	// can select preferred style of const-ness
	#if !SUFFIX_STYLE_CONST
	// prefix const style (slightly ambiguous, but common)
		string("const ") +what<T>::name();
	#else
	// suffix const style (unambiguous, but unusual)
		string(what<T>::name()) + " const";
	#endif
	static const char* const local_name = name_string.c_str();
	return local_name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UTIL_WHAT_TEMPLATE_SIGNATURE
typename what<T&>::name_type
what<T&>::name(void) {
	static const string name_string = string(what<T>::name()) + "&";
	static const char* const local_name = name_string.c_str();
	return local_name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UTIL_WHAT_TEMPLATE_SIGNATURE
typename what<T*>::name_type
what<T*>::name(void) {
	static const string name_string = string(what<T>::name()) + "*";
	static const char* const local_name = name_string.c_str();
	return local_name;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if !SUFFIX_STYLE_CONST
UTIL_WHAT_TEMPLATE_SIGNATURE
typename what<T* const>::name_type
what<T* const>::name(void) {
	static const string name_string = string(what<T>::name()) + "* const";
	static const char* const local_name = name_string.c_str();
	return local_name;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif	// UTIL_WHAT_PARTIAL_SPECIALIZATIONS

}	// end namespace util

#endif	// __UTIL_WHAT_TCC__

