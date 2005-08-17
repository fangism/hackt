/**
	\file "Object/inst/alias_actuals.tcc"
	Definition of template methods for instance_alias_info_actuals
		and instance_alias_info_empty.
	This file was "Object/art_object_instance_alias_actuals.tcc"
		in a previous life.  
	$Id: alias_actuals.tcc,v 1.2.8.1 2005/08/17 21:49:24 fang Exp $
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

#if 0
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
#endif

//=============================================================================
// class instance_alias_info_actuals method definitions

/**
	\pre if this has actuals, then everything else in the ring has 
		equivalent actuals, though not necesarily the same pointer, 
		otherwise everything else in this ring has null actuals
		and should be updated.  
	\param OutIter iterator over ring node aliases.  
	\param a the actuals to compare against or copy.  
	\param i the start output iterator.
	\param e the end output iterator.  
 */
template <class OutIter>
good_bool
instance_alias_info_actuals::__compare_and_propagate_actuals(
		const alias_actuals_type& a, OutIter i, OutIter e) {
	if (!a) {
		return good_bool(true);
	} else if (this->actuals) {
		// might as well check pointers first
		if (this->actuals != a &&
				!this->actuals->must_be_equivalent(*a)) {
			// error message copied from old
			// instance_alias_info_actuals::compare_and_update_actuals
			cerr << "ERROR: attempted to connect instances with "
				"conflicting relaxed parameters!" << endl;
			this->actuals->dump(cerr << "\tgot: ") << endl;
			a->dump(cerr << "\tand: ") << endl;
			return good_bool(false);
		}
	} else {
		for ( ; i!=e; i++) {
			const bool b = i->attach_actuals(a);
			INVARIANT(b);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ALIAS_ACTUALS_TCC__

