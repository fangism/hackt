/**
	\file "persistent.cc"
	Method definitions for persistent class interface.  
 */

#include <assert.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#include "persistent.h"

namespace util {
//=============================================================================
using namespace std;

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
	\param s must not have length > MAX_LEN;
 */
persistent::hash_key::hash_key(const char* s) {
	assert(strlen(s) <= MAX_LEN);
	strncpy(key, s, MAX_LEN);	// will pad with '\0'
	assert(sizeof(int) == 4);
	const int& hi = * reinterpret_cast<int*>(&key[0]);
	const int& lo = * reinterpret_cast<int*>(&key[4]);
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
	assert(s.length() <= MAX_LEN);
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
	assert(sizeof(int) == 4);
	const int& hi = * reinterpret_cast<int*>(&key[0]);
	const int& lo = * reinterpret_cast<int*>(&key[4]);
	hash = hi ^ lo;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
operator == (const persistent::hash_key& l, const persistent::hash_key& r) {
	return equal(l.begin(), l.end(), r.begin());
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
	return lexicographical_compare(l.begin(), l.end(), r.begin(), r.end());
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

