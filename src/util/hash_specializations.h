/**
	\file "hash_specializations.h"
	Contains hash function specializations.  
	Include this file before using any hash_map for specializations
	to take effect.
	$Id: hash_specializations.h,v 1.4 2004/11/02 07:52:14 fang Exp $
 */

#ifndef	__HASH_SPECIALIZATIONS_H__
#define	__HASH_SPECIALIZATIONS_H__

#include "hash_map.h"		// wrapper for <hash_map>

#include <string>

//=============================================================================
// template specializations

namespace HASH_MAP_NAMESPACE {
using std::string;

/**
	Explicit template specialization of hash of a string class, 
	which just uses the internal char* representation as a wrapper.
 */
template <>
struct hash<string> {
	size_t operator() (const string& x) const {
		return hash<const char*>()(x.c_str());
	}
};	// end hash<>

/**
	May want to shuffle around bits because pointers tend to be aligned, 
	rendering the lower bits useless.  
	Caution: don't want to make this machine-dependent.
 */
template <>
struct hash<const void*> {
	size_t operator() (const void* x) const {
		register const long y = (long) x;	// C-style cast!
		return hash<long>()(y ^ (y >> 7));
	}
};	// end hash<>

};	// end namespace HASH_MAP_NAMESPACE

//=============================================================================

#endif	// __HASH_SPECIALIZATIONS_H__

