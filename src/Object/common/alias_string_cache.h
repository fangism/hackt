/**
	\file "Object/common/alias_string_cache.h"
	Alias string cache structure.  
	$Id: alias_string_cache.h,v 1.3 2005/11/03 07:52:04 fang Exp $
 */

#ifndef	__OBJECT_COMMON_ALIAS_STRING_CACHE_H__
#define	__OBJECT_COMMON_ALIAS_STRING_CACHE_H__

#include <vector>
#include <string>

namespace ART {
namespace entity {
using std::vector;
//=============================================================================
/**
	Cache type for maintaining alias strings.  
	Used by alias_reference_set.  
	Struct is public, and contents and state are maintained
	externally to this class.  
 */ 
struct alias_string_cache {
public:
	typedef vector<std::string>        		string_array_type;
	typedef	string_array_type::const_iterator	const_iterator;
	typedef	string_array_type::const_reference	const_reference;
	typedef	string_array_type::value_type		value_type;
public:
	/**
		Publically accessible scratch-pad list of strings, 
		to be managed and maintained coherent by user.  
	 */
	string_array_type                       strings;

	/**
		Whether or not the cache is valid.  
		Also manually managed.  
	 */
	bool                                    valid;
public:
	alias_string_cache() : strings(), valid(false) { }
	~alias_string_cache() { }

#if 0
	void
	resize(const size_t s) { strings.resize(s); }

	void
	invalidate(void) const { valid = false; }

	void
	update(void) { valid = true; }
#endif

	void
	uncache(void) {
		valid = false;
		strings.clear();
	}

};      // end class alias_string_cache

//=============================================================================
/**
	Used to accumulate all aliases across the hierarchy during cflat.  
 */
struct wire_alias_set : public vector<alias_string_cache> {
};	// end struct wire_alias_set

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_COMMON_ALIAS_STRING_CACHE_H__

