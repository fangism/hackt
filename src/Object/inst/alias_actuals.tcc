/**
	\file "Object/inst/alias_actuals.tcc"
	Definition of template methods for instance_alias_info_actuals
		and instance_alias_info_empty.
	This file was "Object/art_object_instance_alias_actuals.tcc"
		in a previous life.  
	$Id: alias_actuals.tcc,v 1.2 2005/07/23 06:52:33 fang Exp $
 */

#ifndef	__OBJECT_INST_ALIAS_ACTUALS_TCC__
#define	__OBJECT_INST_ALIAS_ACTUALS_TCC__

#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_alias.h"

namespace ART {
namespace entity {
//=============================================================================
// class instance_alias_info_empty method definitions

/**
	Empty type never has relaxed actuals, so there's no need to
	walk the connection list to look for actuals!
	\return NULL relaxed actuals.  
 */
template <class Tag>
const instance_alias_info_empty::alias_actuals_type&
instance_alias_info_empty::find_relaxed_actuals(
		const instance_alias_info<Tag>& al) {
	return al.get_relaxed_actuals();
}

//=============================================================================
// class instance_alias_info_actuals method definitions

/**
	Walk the alias connection list until valid actuals found.  
	If not found, then return NULL (last one, e.g).  
 */
template <class Tag>
const instance_alias_info_actuals::alias_actuals_type&
instance_alias_info_actuals::find_relaxed_actuals(
		const instance_alias_info<Tag>& al) {
	typedef	instance_alias_info<Tag>	alias_type;
	typedef	typename alias_type::const_iterator	const_iterator;
	const const_iterator b(al.begin());	// fixed iterator
	const_iterator i(b);			// walking iterator copy
	const const_iterator e(al.end());
	for ( ; i!=e; i++) {
		const alias_actuals_type& aa(i->get_relaxed_actuals());
		if (aa)
			return aa;
	}
	return b->get_relaxed_actuals();	// should be NULL
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ALIAS_ACTUALS_TCC__

