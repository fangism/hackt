/**
	\file "what.h"
	Utility for user-defined type-names.
	$Id: what.h,v 1.2 2005/01/12 03:19:41 fang Exp $
 */

#ifndef	__UTIL_WHAT_H__
#define	__UTIL_WHAT_H__

#define	UTIL_WHAT_TEMPLATE_SIGNATURE		template <class T>

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

}	// end namespace util

#endif	// __UTIL_WHAT_H__

