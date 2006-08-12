/**
	\file "util/rebinder.h"
	Template metaclasses to rebind template types.  
	NOTE: this technique works with gcc-4.2, but not earlier...
	The STL containers have default allocator template parameters, 
	which allows binding to templates with more parameters (but default)
	in earlier versions of gcc.  
	$Id: rebinder.h,v 1.2 2006/08/12 00:36:37 fang Exp $
 */

#ifndef	__UTIL_REBINDER_H__
#define	__UTIL_REBINDER_H__

#include "config.h"

// the relevant configure-flag
#ifdef	HAVE_DEFAULT_TEMPLATE_TEMPLATE_PARAMETER_BINDING
// nothing yet
#endif

namespace util {
//-----------------------------------------------------------------------------
/**
	A non-template type cannot have template parameters rebound, duh!
	The useful work is defined in partial specializations.  
 */
template <class T>
struct rebind {
	/// really there shouldn't be a type...
	// typedef	T				type;
};

//-----------------------------------------------------------------------------
/**
	\param T takes 1 template parameters.  
 */
template <template <typename> class T, typename P1>
struct rebind<T<P1> > {
	/**
		Single template parameter substitution.  
	 */
	template <typename A1>
	struct other {
		typedef	T<A1>			type;
	};
};	// end struct rebind

//-----------------------------------------------------------------------------
/**
	\param T takes 2 template parameters.  
 */
template <template <typename, typename> class T,
	typename P1, typename P2>
struct rebind<T<P1, P2> > {
	/**
		Two parameter substitution.  
		Must pass at least the first parameter to substitute.  
	 */
	template <typename A1, typename A2 = P2>
	struct other {
		typedef	T<A1, A2>		type;
	};

#if 0
	/**
		Usable if T has default trailing parameters.
	 */
	template <typename A1>
	struct other_default1 {
		typedef	T<A1>		type;
	};
#endif

};	// end struct rebind

//-----------------------------------------------------------------------------
/**
	\param T takes 3 template parameters.  
 */
template <template <typename, typename, typename> class T, 
	typename P1, typename P2, typename P3>
struct rebind<T<P1, P2, P3> > {
	/**
		Two parameter substitution.  
		Must pass at least the first parameter to substitute.  
	 */
	template <typename A1, typename A2 = P2, typename A3 = P3>
	struct other {
		typedef	T<A1, A2, A3>		type;
	};

#if 0
	/**
		Usable if T has default trailing parameters.
	 */
	template <typename A1>
	struct other_default1 {
		typedef	T<A1>		type;
	};

	/**
		Usable if T has default trailing parameters.
	 */
	template <typename A1, typename A2>
	struct other_default2 {
		typedef	T<A1, A2>		type;
	};
#endif

};	// end struct rebind

//-----------------------------------------------------------------------------
}	// end namespace util

#endif	// __UTIL_REBINDER_H__

