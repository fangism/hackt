/**
	\file "what.h"
	Utility for user-defined type-names.
	$Id: what.h,v 1.3 2005/01/14 06:28:46 fang Exp $
 */

#ifndef	__UTIL_WHAT_H__
#define	__UTIL_WHAT_H__

#include "string_fwd.h"

#define	UTIL_WHAT_TEMPLATE_SIGNATURE		template <class T>

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
/**
	A generic macro for total specialization of "what".
	This follows closely to the definition of "what".
	This macro *MUST* appear in the ::util namespace to be effective.  
	Here we maintain the name_type as const char*.
	For partial specializations see the next macro definition.  
 */
#define	SPECIALIZE_UTIL_WHAT_DECLARATION(T)			\
	template <>						\
	struct what<T> {					\
		typedef	const char	name_type[];		\
		static const name_type	name;			\
	};

#define	SPECIALIZE_UTIL_WHAT_DEFINITION(T, __name__)		\
	template <>						\
	const what<T>::name_type				\
	what<T>::name = __name__;

#define	SPECIALIZE_UTIL_WHAT(T, __name__)			\
	SPECIALIZE_UTIL_WHAT_DECLARATION(T)			\
	SPECIALIZE_UTIL_WHAT_DEFINITION(T, __name__)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Works for a single parameter, don't have a generalized macro yet...
	String concatenation works only if what<S> is a const char*.
	TODO: finish me!
 */
#define	PARTIAL_SPECIALIZE_UTIL_WHAT(T, __name__)		\
	template <class S>					\
	struct what<T<S> > {					\
		typedef	const char	name_type[];		\
		static const name_type	name;			\
	};							\
	template <class S>					\
	const what<T<S> >::name_type				\
	what<T<S> >::name = __name__ ## "<" ## what<S>::name ## ">";

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
	static const name_type		name;
};	// end struct what

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if UTIL_WHAT_PARTIAL_SPECIALIZATIONS
// partial specializations of what
// compound types like *& will automatically be printed correctly!

UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<const T> {
	static const string		name_string;
	typedef	const char*		name_type;
	static const name_type		name;
};

UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<T&> {
	static const string		name_string;
	typedef	const char*		name_type;
	static const name_type		name;
};

UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<T*> {
	static const string		name_string;
	typedef	const char*		name_type;
	static const name_type		name;
};

#if !SUFFIX_STYLE_CONST
// this is not necessary when using the suffix-style const qualifier
UTIL_WHAT_TEMPLATE_SIGNATURE
struct what<T* const> {
	static const string		name_string;
	typedef	const char*		name_type;
	static const name_type		name;
};
#endif

#endif	// UTIL_WHAT_PARTIAL_SPECIALIZATIONS

}	// end namespace util

#endif	// __UTIL_WHAT_H__

