/**
	\file "util/what.tcc"
	Default implementation for user-defined type-names.
	$Id: what.tcc,v 1.6 2005/05/10 04:51:31 fang Exp $
 */

#ifndef	__UTIL_WHAT_TCC__
#define	__UTIL_WHAT_TCC__

#include "util/what.h"
#include <string>
#include <typeinfo>

/***
	NOTE: Starting with gcc4, the linker complains about multiple 
	definitions of implicitly instantiated function-local symbols, 
	such as those declared in the name() static member functions.  
	These warnings are not fatal, they still allow the program to 
	compile and run fine, however the warnings are irritating
	enough to warrant a workaround.  

	Precisely the problem is...
***/

namespace util {
using std::string;
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

