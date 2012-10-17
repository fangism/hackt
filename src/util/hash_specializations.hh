/**
	\file "util/hash_specializations.hh"
	Contains hash function specializations.  
	Include this file before using any hash_map for specializations
	to take effect.
	$Id: hash_specializations.hh,v 1.11 2008/11/25 21:52:45 fang Exp $
 */

#ifndef	__UTIL_HASH_SPECIALIZATIONS_H__
#define	__UTIL_HASH_SPECIALIZATIONS_H__

#include "config.h"

#if 	defined(HASH_MAP_SGI_STYLE)

#include "util/STL/hash_map_fwd.hh"	// needed for namespace definition
#include <string>

#if defined(HAVE_EXT_STL_HASH_FUN_H) && HAVE_EXT_STL_HASH_FUN_H
#include <ext/stl_hash_fun.h>		// from gcc-3.x
#elif defined(HAVE_EXT_HASH_FUN_H) && HAVE_EXT_HASH_FUN_H
#include <ext/hash_fun.h>		// from gcc-4.x
#elif defined(HAVE_STL_HASH_FUN_H) && HAVE_STL_HASH_FUN_H
#include <stl_hash_fun.h>		// from gcc-2.95
#else	// fallback:
#include "util/STL/hash_map.hh"
#endif

//=============================================================================
// template specializations

BEGIN_HASH_MAP_NS

// forward declare the specialization for const char*
template <>
struct hash<const char*>;

/**
	May want to shuffle around bits because pointers tend to be aligned, 
	rendering the lower bits useless.  
	Caution: don't want to make this machine-dependent.
	TODO: use configure to determin correct size operands for doing this.
		This will be critical once pointers are 64b.  
	NOTE: wouldn't unsigned be better?
 */
template <class T>
struct hash<const T*> {
	size_t operator() (const T* x) const {
		// need something with same size as raw pointer
#if	SIZEOF_VOIDP == SIZEOF_SIZE_T
		typedef	size_t		__type;
#else
#error	"I need an integer type with same size as pointer."
#endif
		register const __type y = reinterpret_cast<const __type>(x);
		return hash<__type>()(y ^ (y >> 7));
		// Apple g++-4.0.1 ICEs without explicit operator()...
	}
};	// end hash<>

#if	!USING_UNORDERED_MAP
/**
	Explicit template specialization of hash of a string class, 
	which just uses the internal char* representation as a wrapper.
 */
template <>
struct hash<std::string> {
	size_t operator() (const std::string& x) const {
		return hash<const char*>()(x.c_str());
	}
};	// end hash<>
// else assume C++0x defines it
#endif

END_HASH_MAP_NS	// end namespace HASH_MAP_NAMESPACE

#endif	// defined(HASH_MAP_SGI_STYLE)

//=============================================================================

#endif	// __UTIL_HASH_SPECIALIZATIONS_H__

