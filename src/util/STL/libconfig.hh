/**
	Preamble to any C++ library forward declarations.
	Detect versioned inline namespace (libc++).
 */
#ifndef	__UTIL_STL_LIBCONFIG_HH__
#define	__UTIL_STL_LIBCONFIG_HH__

#ifdef	HAVE_CONFIG_H
#include "config.h"
#endif

// These are configuration files internal to libraries, and usually
// not directly included.
// Only one of the following should be included:

// libc++
#ifdef	CXXLIB_VERSIONED_NAMESPACE
#include <__config>		// libc++
#else
// #include <bits/c++config>	// GNU libstdc++
#endif

#ifdef	_VSTD
#define	VSTD	_VSTD
#define	BEGIN_NAMESPACE_STD	_LIBCPP_BEGIN_NAMESPACE_STD
#define	END_NAMESPACE_STD	_LIBCPP_END_NAMESPACE_STD
#else
#define	VSTD	std
#define	BEGIN_NAMESPACE_STD	namespace std {
#define	END_NAMESPACE_STD	}
#endif

#endif	// __UTIL_STL_LIBCONFIG_HH__
