/**
	\file "what.tcc"
	Default implementation for user-defined type-names.
	$Id: what.tcc,v 1.3 2005/01/14 06:28:46 fang Exp $
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
const typename what<T>::name_type
what<T>::name = typeid(T).name();

// this must appear first, because everything below depends on it!
//-----------------------------------------------------------------------------
#if UTIL_WHAT_PARTIAL_SPECIALIZATIONS

// can select preferred style of const-ness
UTIL_WHAT_TEMPLATE_SIGNATURE
const string
#if !SUFFIX_STYLE_CONST
// prefix const style (slightly ambiguous, but common)
what<const T>::name_string = string("const ") +what<T>::name;
#else
// suffix const style (unambiguous, but unusual)
what<const T>::name_string = string(what<T>::name) + " const";
#endif

UTIL_WHAT_TEMPLATE_SIGNATURE
const typename what<const T>::name_type
what<const T>::name = name_string.c_str();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UTIL_WHAT_TEMPLATE_SIGNATURE
const string
what<T&>::name_string = string(what<T>::name) + "&";

UTIL_WHAT_TEMPLATE_SIGNATURE
const typename what<T&>::name_type
what<T&>::name = name_string.c_str();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

UTIL_WHAT_TEMPLATE_SIGNATURE
const string
what<T*>::name_string = string(what<T>::name) + "*";

UTIL_WHAT_TEMPLATE_SIGNATURE
const typename what<T*>::name_type
what<T*>::name = name_string.c_str();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#if !SUFFIX_STYLE_CONST
UTIL_WHAT_TEMPLATE_SIGNATURE
const string
what<T* const>::name_string = string(what<T>::name) + "* const";

UTIL_WHAT_TEMPLATE_SIGNATURE
const typename what<T* const>::name_type
what<T* const>::name = name_string.c_str();
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif	// UTIL_WHAT_PARTIAL_SPECIALIZATIONS

}	// end namespace util

#endif	// __UTIL_WHAT_TCC__

