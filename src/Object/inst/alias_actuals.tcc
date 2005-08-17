/**
	\file "Object/inst/alias_actuals.tcc"
	Definition of template methods for instance_alias_info_actuals
		and instance_alias_info_empty.
	This file was "Object/art_object_instance_alias_actuals.tcc"
		in a previous life.  
	$Id: alias_actuals.tcc,v 1.2.8.2 2005/08/17 22:34:57 fang Exp $
 */

#ifndef	__OBJECT_INST_ALIAS_ACTUALS_TCC__
#define	__OBJECT_INST_ALIAS_ACTUALS_TCC__

// #include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_alias.h"

namespace ART {
namespace entity {
//=============================================================================
// class instance_alias_info_empty method definitions

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

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ALIAS_ACTUALS_TCC__

