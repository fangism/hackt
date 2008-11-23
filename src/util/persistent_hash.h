/**
	\file "util/persistent_hash.h"
	Base class interface for persistent, serializable objects.  
	$Id: persistent_hash.h,v 1.3 2008/11/23 17:55:03 fang Exp $
 */

#ifndef	__UTIL_PERSISTENT_HASH_H__
#define	__UTIL_PERSISTENT_HASH_H__

#include "util/persistent.h"
#include "util/string_fwd.h"
#include "util/STL/hash_map_fwd.h"

//=============================================================================
namespace util {
using std::ostream;
using std::string;

//=============================================================================
/**
	Implementers of the persistent class, MUST have a 
	unique key with which it can register its type
	and allocator with the persistent object manager.  
	The key should be public, static, and constant.  
	(Static => is not inherited.)
	The point of declaring this type as a class is for 
	consistency: the key needs to be small and of the same 
	size and type because it will be stored in the 
	manager's header for every persistent object instance.  
 */
class persistent::hash_key {
public:
	typedef	char*		iterator;
	typedef	const char*	const_iterator;
public:
	static const size_t	MAX_LEN = 8;
protected:
	/**
		8 bytes = 2 ints or 1 long-long.  
		Not relying on architecture.  
	 */
	char	key[MAX_LEN];
	/**
		Computed once (upon construction) from the key string.
	 */
	size_t	hash;
public:
	hash_key();
	hash_key(const char* k);
	hash_key(const string& k);
	// standard destructor

	size_t
	get_hash(void) const { return hash; }

	const_iterator
	begin(void) const { return &key[0]; }

	const_iterator
	end(void) const { return &key[MAX_LEN]; }

#if	defined(HASH_MAP_INTEL_STYLE)
	/**
		Intel-style's hash_compare function needs an
		implicit conversion to size_t.  
		\return precomputed hash index.  
	 */
	operator size_t () const { return hash; }
#endif

	friend bool operator == (const hash_key&, const hash_key&);
	friend bool operator != (const hash_key&, const hash_key&);
	friend bool operator < (const hash_key&, const hash_key&);
	friend ostream& operator << (ostream&, const hash_key&);

public:
	static const hash_key		null;
};	// end class hash_key

//-----------------------------------------------------------------------------
}	// end namespace util

//=============================================================================
BEGIN_HASH_MAP_NS
using util::persistent;

/**
	Since hash_key::key is not null-terminated, don't use 
	standard char* hash, use precomputed hash member.
 */
template <>
struct hash<persistent::hash_key> {
	size_t
	operator () (const persistent::hash_key& k) const
		{ return k.get_hash(); }
};	// end struct hash

END_HASH_MAP_NS	// end namespace HASH_MAP_NAMESPACE

//=============================================================================

#endif	// __UTIL_PERSISTENT_HASH_H__

