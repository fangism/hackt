/**
	\file "what.h"
	Utility for user-defined type-names.
	This file provides a generic default implementation.  
	$Id: what.h,v 1.3.4.1.2.1 2005/01/26 20:55:26 fang Exp $
 */

#ifndef	__UTIL_WHAT_H__
#define	__UTIL_WHAT_H__

// not using strings any more
// #include "string_fwd.h"
#include "what_fwd.h"

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
// using std::string;

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

UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<const T> {
	// static const string		name_string;
	typedef	const char*		name_type;
	static name_type		name(void);
};

UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<T&> {
	// static const string		name_string;
	typedef	const char*		name_type;
	static name_type		name(void);
};

UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<T*> {
	// static const string		name_string;
	typedef	const char*		name_type;
	static name_type		name(void);
};

#if !SUFFIX_STYLE_CONST
// this is not necessary when using the suffix-style const qualifier
UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<T* const> {
	// static const string		name_string;
	typedef	const char*		name_type;
	static name_type		name(void);
};
#endif

#endif	// UTIL_WHAT_PARTIAL_SPECIALIZATIONS

}	// end namespace util

#endif	// __UTIL_WHAT_H__

