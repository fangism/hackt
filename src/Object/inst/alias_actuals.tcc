/**
	\file "Object/inst/alias_actuals.tcc"
	Definition of template methods for instance_alias_info_actuals
		and instance_alias_info_empty.
	This file was "Object/art_object_instance_alias_actuals.tcc"
		in a previous life.  
	$Id: alias_actuals.tcc,v 1.4.6.2 2005/10/14 03:30:20 fang Exp $
 */

#ifndef	__OBJECT_INST_ALIAS_ACTUALS_TCC__
#define	__OBJECT_INST_ALIAS_ACTUALS_TCC__

#include <iostream>
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_alias.h"
#include "Object/type/canonical_type.h"
#include "util/stacktrace.h"
#include "Object/expr/expr_dump_context.h"

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
		actuals->dump(cerr << "\tgot: ", 
			expr_dump_context::default_value) << endl;
		a->dump(cerr << "\tand: ", 
			expr_dump_context::default_value) << endl;
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
			l.actuals->dump(cerr << "\tgot: ", 
				expr_dump_context::default_value) << endl;
			r.actuals->dump(cerr << "\tand: ", 
				expr_dump_context::default_value) << endl;
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
	TODO: if container's type is strict and is being merged with
		relaxed actuals, then make sure the relaxed actuals
		match up, otherwise, is an error.  
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
		if (actuals && !_type.match_relaxed_actuals(actuals)) {
			// TODO: this is wrong, see comment TODO
			cerr << "ERROR: collection type is already complete, "
				"cannot merge relaxed actuals." << endl;
			_type.dump(cerr << "\thave: ");
			actuals->dump(cerr << " with <", 
				expr_dump_context::default_value)
				<< '>' << endl;
			return canonical_type_type();
		} else {
			return _type;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Completes tyoe and creates the dependent type.  
 */
template <class AliasType>
good_bool
instance_alias_info_actuals::create_dependent_types(const AliasType& _alias) {
	typedef	typename AliasType::container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
				complete_type_type;
	STACKTRACE_VERBOSE;
	const complete_type_type
		_type(_alias.complete_type_actuals(*_alias.container));
	if (!_type) {
		// already have error message
		_alias.dump_hierarchical_name(cerr << "Failed to instantiate ")
			<< endl;
		return good_bool(false);
	}
	else if (!container_type::collection_type_manager_parent_type
			::create_definition_footprint(_type).good) {
		// have error message already
		_alias.dump_hierarchical_name(cerr << "Instantiated by: ")
			<< endl;
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the complete-type to which the footprint belongs.  
	\param _alias the instance alias whose type should be dumped.  
	\param o the output stream.  
	\param f is only used for assertion checking.  
 */
template <class AliasType>
ostream&
instance_alias_info_actuals::dump_complete_type(const AliasType& _alias, 
		ostream& o, const footprint* const f) {
	typedef	typename AliasType::container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
				complete_type_type;
	typedef	typename complete_type_type::canonical_definition_type
				canonical_definition_type;
	const complete_type_type
		_type(_alias.complete_type_actuals(*_alias.container));
	_type.dump(o);
	check_footprint_policy<canonical_definition_type>()(_type, f);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluates the complete canonical type based on the
	container's type and the relaxed actuals.  
	Uses the corresponding footprint of the canonical type
	to initialize the footprint_frame, basically size-copying.  
	Fills in the footprint frame with global ID as resolved
	in the port_member_context structure.  
	\param _alias the instance alias used to extrace the type.  
	\param ff the footprint frame in which global substructure 
		information will be retained.  
	\param ind the global index of the alias's for sake
		of making parent back-reference.  
	\return good upon success.  
 */
template <class AliasType>
good_bool
instance_alias_info_actuals::__initialize_assign_footprint_frame(
		const AliasType& _alias, footprint_frame& ff, 
		state_manager& sm, const port_member_context& pmc, 
		const size_t ind) {
	typedef	typename AliasType::container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
				complete_type_type;
	typedef	typename complete_type_type::canonical_definition_type
				canonical_definition_type;
	STACKTRACE_VERBOSE;
	const complete_type_type
		_type(_alias.complete_type_actuals(*_alias.container));
	INVARIANT(_type);
	return canonical_type_footprint_frame_policy<canonical_definition_type>
		::initialize_and_assign(_type, ff, sm, pmc, ind);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: is there a way to save/restore the footprint without
		writing the canonical type, but rather inferring
		it from the hierarchical position?  Probably.  
	Enhancement request:
	TODO: (alternate) use another pass during reconstruction
		to deduce the footprint types from the hierarchy
		to restore the footprint pointers.  
		This is preferable because it eliminates having to save
		away redundant canonical type information.  
 */
template <class AliasType>
void
instance_alias_info_actuals::save_canonical_footprint(const AliasType& _alias, 
		const persistent_object_manager& m, ostream& o, 
		const footprint* const _f) {
	typedef	typename AliasType::container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
				complete_type_type;
	typedef	typename complete_type_type::canonical_definition_type
				canonical_definition_type;
	const complete_type_type
		_type(_alias.complete_type_actuals(*_alias.container));
	check_footprint_policy<canonical_definition_type>()(_type, _f);
	_type.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
void
instance_alias_info_actuals::restore_canonical_footprint(
		const AliasType& _alias, const persistent_object_manager& m, 
		istream& i, const footprint*& _f) {
	typedef	typename AliasType::container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
				complete_type_type;
	typedef	typename complete_type_type::canonical_definition_type
				canonical_definition_type;
	complete_type_type _type;
	_type.load_object_base(m, i);
	// temporary ugly hack
	footprint_frame _frame;
	canonical_type_footprint_frame_policy<canonical_definition_type>
		::initialize_frame_pointer_only(_type, _f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_ALIAS_ACTUALS_TCC__

