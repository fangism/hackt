// "hash_specializations.h"

#ifndef	__HASH_SPECIALIZATIONS_H__
#define	__HASH_SPECIALIZATIONS_H__

#include "hash_map.h"		// wrapper for <hash_map>

#include <string>
using std::string;

//=============================================================================
// template specializations

namespace
#ifdef	__GNUC__
#if	(__GNUC__ >= 3)
	__gnu_cxx
#else	// (__GNUC__ <= 2)
	std
#endif
#else	// __GNUC__
	DEATH
#endif	// __GNUC__
{
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
};	// end namespace

//=============================================================================

#endif	// __HASH_SPECIALIZATIONS_H__
