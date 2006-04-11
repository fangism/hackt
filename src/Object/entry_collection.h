/**
	\file "Object/entry_collection.h"
	Handy class for accumulating all reachable subinstances.  
	$Id: entry_collection.h,v 1.1.2.1 2006/04/11 06:24:14 fang Exp $
 */

#ifndef	__HAC_OBJECT_ENTRY_COLLECTION_H__
#define	__HAC_OBJECT_ENTRY_COLLECTION_H__

#include <set>
#include "Object/traits/classification_tags.h"

namespace HAC {
namespace entity {
typedef	std::set<size_t>		index_set_type;

//=============================================================================
template <class Tag>
struct entry_collection_base {

	index_set_type				index_set;

	entry_collection_base() : index_set() { }

	~entry_collection_base() { }

};	// end class entry_collection_base

//-----------------------------------------------------------------------------
struct entry_collection :
	public entry_collection_base<bool_tag>, 
	public entry_collection_base<int_tag>, 
	public entry_collection_base<enum_tag>, 
	public entry_collection_base<datastruct_tag>, 
	public entry_collection_base<channel_tag>, 
	public entry_collection_base<process_tag> {
	typedef	entry_collection			this_type;

	entry_collection() : 
		entry_collection_base<bool_tag>(), 
		entry_collection_base<int_tag>(), 
		entry_collection_base<enum_tag>(), 
		entry_collection_base<datastruct_tag>(), 
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

};	// end class entry_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_ENTRY_COLLECTION_H__

