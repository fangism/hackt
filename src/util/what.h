/**
	\file "util/what.h"
	Utility for user-defined type-names.
	This file provides a generic default implementation.  
	$Id: what.h,v 1.8 2005/06/19 01:58:52 fang Exp $
 */

#ifndef	__UTIL_WHAT_H__
#define	__UTIL_WHAT_H__

#include "util/string_fwd.h"
#include "util/what_fwd.h"

#ifndef	UTIL_WHAT_PARTIAL_SPECIALIZATIONS
#define	UTIL_WHAT_PARTIAL_SPECIALIZATIONS	1
#endif

/**
	Where does the `const' go?  
	To be unambiguous const follows the thing that is const, 
	so a const T is printed as T const.  
	Default set to 1: suffix-style.
	But if you want to be bass-ackwards about it (or is it?)
	then you can set this to 0.  
 */
#define	SUFFIX_STYLE_CONST			1

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace util {
using std::string;

/**
	The default implementation of the type name.  
	Specializations are required to have a public (static const) 
	printable (may be char* or string, etc.) member named "name".
	This may be partially specialized, but specialization must be
	in the home (util) namespace.  
	\param T the type to be named.  
 */
UTIL_WHAT_TEMPLATE_SIGNATURE
struct what {
	/// the type of the name (must be printable)
	typedef	const char*		name_type;
	/// the full name of the type
	static name_type		name(void);
};	// end struct what

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if UTIL_WHAT_PARTIAL_SPECIALIZATIONS
// partial specializations of what
// compound types like *& will automatically be printed correctly!

/***
	See note in "what.tcc" for rationale for extra static members.  
***/

UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<const T> {
	typedef	const char*		name_type;
	static name_type		name(void);
};

UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<T&> {
	typedef	const char*		name_type;
	static name_type		name(void);
};

UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<T*> {
	typedef	const char*		name_type;
	static name_type		name(void);
};

#if !SUFFIX_STYLE_CONST
// this is not necessary when using the suffix-style const qualifier
UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<T* const> {
	typedef	const char*		name_type;
	static name_type		name(void);
};
#endif

#endif	// UTIL_WHAT_PARTIAL_SPECIALIZATIONS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A way of getting the canonical type of an identified object
	instance, just a wrapped call to what<T>::name().
	NOTE: this takes an argument by const reference, so no
	copy constructor is required.  
	As a result, constness and reference-ness will be stripped.
	\param T the type of object.
	\return string of type name.  
 */
template <class T>
inline
typename what<T>::name_type
what_is(const T&) {
	return what<T>::name();		// specialized!
}

}	// end namespace util

#endif	// __UTIL_WHAT_H__

