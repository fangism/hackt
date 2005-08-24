/**
	\file "Object/inst/alias_actuals.tcc"
	Definition of template methods for instance_alias_info_actuals
		and instance_alias_info_empty.
	This file was "Object/art_object_instance_alias_actuals.tcc"
		in a previous life.  
	$Id: alias_actuals.tcc,v 1.2.8.5 2005/08/24 22:37:00 fang Exp $
 */

#ifndef	__OBJECT_INST_ALIAS_ACTUALS_TCC__
#define	__OBJECT_INST_ALIAS_ACTUALS_TCC__

#include <iostream>
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_alias.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class instance_alias_info_empty method definitions

//=============================================================================
// class instance_alias_info_actuals method definitions

/**
	\pre if this has actuals, then everything else in the ring has 
		equivalent actuals, though not necesarily the same pointer, 
		otherwise everything else in this ring has null actuals
		and should be updated.  
	\param AliasType the true alias type.  
	\param a the actuals to compare against or copy.  
	\param i the start output iterator.
	\param e the end output iterator.  
 */
template <class AliasType>
good_bool
instance_alias_info_actuals::__compare_and_propagate_actuals(
		const alias_actuals_type& a, AliasType& alias) {
if (!a) {
	return good_bool(true);
} else if (actuals) {
	// might as well check pointers first
	if (actuals != a && !actuals->must_be_equivalent(*a)) {
		// error message copied from old
		// instance_alias_info_actuals::compare_and_update_actuals
		cerr << "ERROR: attempted to connect instances with "
			"conflicting relaxed parameters!" << endl;
		actuals->dump(cerr << "\tgot: ") << endl;
		a->dump(cerr << "\tand: ") << endl;
		alias.dump_hierarchical_name(cerr << "\tfrom: ")
			<< endl;
		return good_bool(false);
	}
} else {
	alias.propagate_actuals(a);
}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Symmetrically synchronizes two rings' relaxed actual parameters.  
	If both aliases have valid actuals, the result of the compare is 
	returned.  If only one has actuals, they are copied to the
	other alias's ring.  If both are null, nothing happens.  

	CRITICAL: Is it possible to miss connections in the hierarchy?
		The rings are still disjoint, so when actuals are
		propagated to one of several equivalently connected rings, 
		the disconnected rings will be missed!!!
		The fix: if both are NULL, then merge their rings.  
		That way NULL rings will never be separated, 
		whereas it is OK for rings with (equivalent) actuals 
		to remain separated.  This will form delayed connections.  
		Do we need to apply this to the asymmetric
			__compare_and_propagate_actuals?  probably.  
 */
template <class AliasType>
good_bool
instance_alias_info_actuals::__symmetric_synchronize(
		AliasType& l, AliasType& r) {
if (l.actuals) {
	if (r.actuals) {
		// might as well check pointers first
		if (l.actuals != r.actuals &&
			!l.actuals->must_be_equivalent(*r.actuals)) {
		// error message copied from above
			cerr << "ERROR: attempted to connect instances with "
				"conflicting relaxed parameters!" << endl;
			l.actuals->dump(cerr << "\tgot: ") << endl;
			r.actuals->dump(cerr << "\tand: ") << endl;
			l.dump_hierarchical_name(cerr << "\tfrom: ") << endl;
			r.dump_hierarchical_name(cerr << "\tand : ") << endl;
			return good_bool(false);
		}
	} else {
		r.propagate_actuals(l.actuals);
	}
} else {
	if (r.actuals) {
		l.propagate_actuals(r.actuals);
	} else {
		// only need to merge if their collection's types are relaxed
		// we know they are at least collectibly equivalent, 
		// so we can check either type.  
		if (l.container->is_relaxed_type()) {
			typedef	typename AliasType::instance_alias_base_type
					instance_alias_base_type;
			instance_alias_base_type&
				ll(AS_A(instance_alias_base_type&, l));
			instance_alias_base_type&
				rr(AS_A(instance_alias_base_type&, r));
			ll.merge(rr);
		}
	}
}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Completes the type by combining the collection's canonical type
	with the relaxed parameters from the instance.  
 */
template <class InstColl>
typename InstColl::instance_collection_parameter_type
instance_alias_info_actuals::complete_type_actuals(
		const InstColl& _inst) const {
	typedef	typename InstColl::instance_collection_parameter_type
						canonical_type_type;
	canonical_type_type _type(_inst.get_canonical_type());
	if (_type.is_relaxed()) {
		if (actuals) {
			// then merge actuals and return
			_type.combine_relaxed_actuals(actuals);
			return _type;
		} else {
			// ERROR: missing actuals for complete type
			cerr << "ERROR: complete type required "
				"for instantiation" << endl;
			_type.dump(cerr << "\thave: ") << endl;
			return canonical_type_type();
		}
	} else {
		if (actuals) {
			cerr << "ERROR: collection type is already complete, "
				"cannot merge relaxed actuals." << endl;
			_type.dump(cerr << "\thave: ");
			actuals->dump(cerr << " with <") << '>' << endl;
			return canonical_type_type();
		} else {
			return _type;
		}
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ALIAS_ACTUALS_TCC__

