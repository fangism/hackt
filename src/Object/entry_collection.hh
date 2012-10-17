/**
	\file "Object/entry_collection.hh"
	Handy class for accumulating all reachable subinstances.  
	$Id: entry_collection.hh,v 1.5 2011/02/08 02:06:47 fang Exp $
 */

#ifndef	__HAC_OBJECT_ENTRY_COLLECTION_H__
#define	__HAC_OBJECT_ENTRY_COLLECTION_H__

#include <set>
#include "util/size_t.h"
#include "Object/traits/classification_tags.hh"
// #include "Object/ref/refernces_enum.hh"
#include "Object/devel_switches.hh"

namespace HAC {
namespace entity {
typedef	std::set<size_t>		index_set_type;

//=============================================================================
template <class Tag>
struct entry_collection_base {

	index_set_type				index_set;

	entry_collection_base() : index_set() { }

	~entry_collection_base() { }

};	// end struct entry_collection_base

//-----------------------------------------------------------------------------
/**
	Is this class replaceable with global_references_set
	from "Object/ref/reference_set.h"?
	TODO: import from global_references_array.
 */
struct entry_collection :
	public entry_collection_base<bool_tag>, 
	public entry_collection_base<int_tag>, 
	public entry_collection_base<enum_tag>, 
#if ENABLE_DATASTRUCTS
	public entry_collection_base<datastruct_tag>, 
#endif
	public entry_collection_base<channel_tag>, 
	public entry_collection_base<process_tag> {
	typedef	entry_collection			this_type;

	entry_collection() : 
		entry_collection_base<bool_tag>(), 
		entry_collection_base<int_tag>(), 
		entry_collection_base<enum_tag>(), 
#if ENABLE_DATASTRUCTS
		entry_collection_base<datastruct_tag>(), 
#endif
		entry_collection_base<channel_tag>(), 
		entry_collection_base<process_tag>() { }
		
	~entry_collection() { }

	template <class Tag>
	const index_set_type&
	get_index_set(void) const {
		return entry_collection_base<Tag>::index_set;
	}

	template <class Tag>
	index_set_type&
	get_index_set(void) {
		return entry_collection_base<Tag>::index_set;
	}

};	// end struct entry_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_ENTRY_COLLECTION_H__

