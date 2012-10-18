/**
	\file "util/persistent.cc"
	Method definitions for persistent class interface.  
	$Id: persistent.cc,v 1.10 2009/04/29 05:33:48 fang Exp $
 */

#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>

#include "util/macros.h"
#include "util/persistent.hh"
#include "util/persistent_hash.hh"

namespace util {
//=============================================================================
#include "util/using_ostream.hh"
using std::fill;

//=============================================================================
// class persistent::hash_key method definitions

const persistent::hash_key
persistent::hash_key::null;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
persistent::warn_unimplemented = false;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
persistent::hash_key::hash_key() {
	fill(&key[0], &key[MAX_LEN], '\0');
	hash = 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wrapped char to int type-punning cast.
	Effectively a reinterpret cast.
 */
static
int
cast_char4_to_int(const char* k) {
#if 0
	return *reinterpret_cast<int*>(k);
#else
	return (((((int(k[0]) << 8) | int(k[1])) << 8) |
		int(k[2])) << 8) | int(k[3]);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s must not have length > MAX_LEN;
 */
persistent::hash_key::hash_key(const char* s) {
	INVARIANT(strlen(s) <= MAX_LEN);
	strncpy(key, s, MAX_LEN);	// will pad with '\0'
	INVARIANT(sizeof(int) == 4);	// should boost::static_assert
	const int hi = cast_char4_to_int(key);
	const int lo = cast_char4_to_int(&key[4]);
	hash = hi ^ lo;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s must not have length > MAX_LEN;
 */
persistent::hash_key::hash_key(const string& s) {
/***
	NOTES:
	The strncpy() copies not more than len characters into dst, appending
	`\0' characters if src is less than len characters long, and not termi-
	nating dst if src is more than len characters long.

	string::length()
	Returns the number of characters in the string, not including any
	null-termination.
***/
	INVARIANT(s.length() <= MAX_LEN);
	const char* str = s.c_str();
	strncpy(key, str, MAX_LEN);	// will pad with '\0'
/***
	Since key is typed as a char array, we can safely (!?)
	reinterpret &char[0] as an integer (4 bytes).
	Value may differ on different architectures, 
	but we don't care because these hash values will not
	be externally observable.  
	Only the key string is observable. 
***/
	INVARIANT(sizeof(int) == 4);	// boost::static_assert
	const int hi = cast_char4_to_int(key);
	const int lo = cast_char4_to_int(&key[4]);
	hash = hi ^ lo;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
operator == (const persistent::hash_key& l, const persistent::hash_key& r) {
	return std::equal(l.begin(), l.end(), r.begin());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
operator != (const persistent::hash_key& l, const persistent::hash_key& r) {
	return !(l == r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
operator < (const persistent::hash_key& l, const persistent::hash_key& r) {
	// should be specialized to strcmp or memcmp by STL
	return std::lexicographical_compare(
		l.begin(), l.end(), r.begin(), r.end());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
operator << (ostream& o, const persistent::hash_key& k) {
/***
	ALERT: may not be good idea to just o << key, because
	the key string may not be null-terminated.  
***/
	size_t i = 0;
	for ( ; i<persistent::hash_key::MAX_LEN && k.key[i]; i++)
		o << k.key[i];
	// space-filler
	for ( ; i<persistent::hash_key::MAX_LEN; i++)
		o << '_';
	return o;
}

//=============================================================================
}	// end namespace util

