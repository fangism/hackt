/**
	\file "what_fwd.h"
	Forward declaration for user-defined type-names utility.
	This files provides mostly macros for specialization.  
	$Id: what_fwd.h,v 1.1.2.1 2005/01/26 20:55:28 fang Exp $
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

#define	SPECIALIZE_UTIL_WHAT(T, __name__)			\
	SPECIALIZE_UTIL_WHAT_DECLARATION(T)			\
	SPECIALIZE_UTIL_WHAT_DEFINITION(T, __name__)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
OBSOLETE: these definitions are not needed because the above
macros were made robust.  

/**
	When using what<> during debugging (or whatnot) of static
	initializations, ordering is not guaranteed automatically!
	Thus we need ways of guaranteeing proper initialization.  
	This should be included in a header file, where it is 
	globally visible to all users of class T.  
	This macro must appear in the util namespace.  
 */
#define	SPECIALIZE_UTIL_WHAT_ROBUST_DECLARATION(T)		\
	template <>						\
	struct what<T> {					\
		typedef	const char*	name_type;		\
		static name_type	name;			\
		static int init_once(void);			\
	};

/**
	This provides definitions for the static-init-robust variation.
	This macro must appear in the util namespace.  
 */
#define	SPECIALIZE_UTIL_WHAT_ROBUST_DEFINITION(T, __name__)	\
	template <>						\
	what<T>::name_type					\
	what<T>::name = NULL;					\
								\
	int							\
	what<T>::init_once(void) {				\
		static const name_type local_name = __name__;	\
		if (!name)					\
			name = local_name;			\
		return 1;					\
	}

/**
	This macro will guarantee proper initialization ordering
	across modules.  
	This may appear in any namespace.  
 */
#define	REQUIRES_UTIL_WHAT_STATIC_INIT(T)			\
	static const int					\
	__util_what_init_token_ ## T ## __ = ::util::what<T>::init_once();

// no macro provided to combine this because they are intended 
// to be used separately.
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

