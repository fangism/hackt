/**
	\file "Object/ref/reference_set.h"
	Container for unique-sorted set of indexed references.  
	$Id: reference_set.h,v 1.3 2007/03/15 06:11:04 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_REFERENCE_SET_H__
#define	__HAC_OBJECT_REF_REFERENCE_SET_H__

#include "Object/ref/reference_enum.h"
#include <set>
// #include <cassert>

namespace HAC {
namespace entity {
struct entry_collection;

//=============================================================================
/**
	Set of set of sorted reference indices, binned by meta type.  
	Advantage: sortedness automatically filters uniques, 
	and facilitates linear time set operations.  
	NOTE: this is very closely related to struct entry_collection, 
		can these be combined?
 */
struct global_references_set {
	/**
		should be the total number of physical meta-types.
		See "Object/traits/type_tag_enum.h"
	 */
	enum { MAX = META_TYPES_PHYSICAL };
	typedef	std::set<size_t>	ref_bin_type;
	/**
		Could be a bit wasteful with most meta type slots 
		unused.  
	 */
	ref_bin_type			ref_bin[MAX];

	void
	clear(void);

	bool
	empty(void) const;

	void
	push_back(const global_indexed_reference& r) {
		// assert(r.first < MAX);
		ref_bin[r.first].insert(r.second);
	}

	void
	import_entry_collection(const entry_collection&);

	void
	set_difference(const global_references_set&, 
		global_references_set&) const;

	void
	set_intersection(const global_references_set&, 
		global_references_set&) const;

};	// end struct global_references_set

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_REFERENCE_SET_H__

