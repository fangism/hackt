/**
	\file "util/what_fwd.hh"
	Forward declaration for user-defined type-names utility.
	This files provides mostly macros for specialization.  
	$Id: what_fwd.hh,v 1.5 2005/05/10 04:51:31 fang Exp $
 */

#ifndef	__UTIL_WHAT_FWD_H__
#define	__UTIL_WHAT_FWD_H__

#define	UTIL_WHAT_TEMPLATE_SIGNATURE		template <class T>

namespace util {
// forward declaration, no generic definition
UTIL_WHAT_TEMPLATE_SIGNATURE
struct what;
}	// end namespace util

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
		typedef	const char*	name_type;		\
		static name_type	name(void);		\
	};

/**
	Using function-local static will guarantee safe usage
	and proper initialization before its first use, even
	in across separately initialized modules.  
 */
#define	SPECIALIZE_UTIL_WHAT_DEFINITION(T, __name__)		\
	what<T>::name_type					\
	what<T>::name(void) {					\
		static const name_type local_name = __name__;	\
		return local_name;				\
	}

#if 1
/**
	Combined declaration of a specialization.
	Note: in gcc4, when the same specialization appears in different
	translation units, the linker complains (warns) about
	multiple definitions.  
	Recommend against using this in the future, just split up
	declaration (header) and definition (translation unit).  
 */
#define	SPECIALIZE_UTIL_WHAT(T, __name__)			\
	SPECIALIZE_UTIL_WHAT_DECLARATION(T)			\
	SPECIALIZE_UTIL_WHAT_DEFINITION(T, __name__)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Works for a single parameter, don't have a generalized macro yet...
	String concatenation works only if what<S> is a const char*.
	TODO: finish me!
 */
#define	PARTIAL_SPECIALIZE_UTIL_WHAT(T, __name__)		\
	template <class S>					\
	struct what<T<S> > {					\
		typedef	const char*	name_type;		\
		static name_type	name(void);		\
	};							\
	template <class S>					\
	what<T<S> >::name_type					\
	what<T<S> >::name(void) {				\
		static const name_type local_name =		\
			__name__ ## "<" ## what<S>::name ## ">";\
		return local_name;				\
	}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#endif	// __UTIL_WHAT_FWD_H__

