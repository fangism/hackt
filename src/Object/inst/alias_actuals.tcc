/**
	\file "Object/inst/alias_actuals.tcc"
	Definition of template methods for instance_alias_info_actuals
		and instance_alias_info_empty.
	This file was "Object/art_object_instance_alias_actuals.tcc"
		in a previous life.  
	$Id: alias_actuals.tcc,v 1.15.32.2 2007/07/09 02:40:32 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_ACTUALS_TCC__
#define	__HAC_OBJECT_INST_ALIAS_ACTUALS_TCC__

#include <iostream>
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/type/canonical_type.h"
#include "util/stacktrace.h"
#include "Object/expr/expr_dump_context.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class instance_alias_info_empty method definitions

//=============================================================================
// class instance_alias_info_actuals method definitions

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
	STACKTRACE_VERBOSE;
	typedef	typename InstColl::instance_collection_parameter_type
						canonical_type_type;
	canonical_type_type
		_type(_inst.get_canonical_collection().__get_raw_type());
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
	Completes type and creates the dependent type.  
 */
template <class AliasType>
good_bool
instance_alias_info_actuals::create_dependent_types(const AliasType& _alias, 
		const footprint& top) {
	typedef	typename AliasType::canonical_container_type	container_type;
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
			::create_definition_footprint(_type, top).good) {
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
	typedef	typename AliasType::canonical_container_type	container_type;
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
	typedef	typename AliasType::canonical_container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
				complete_type_type;
	typedef	typename complete_type_type::canonical_definition_type
				canonical_definition_type;
	STACKTRACE_VERBOSE;
	const complete_type_type
		_type(_alias.complete_type_actuals(*_alias.container));
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
	// now possible because relaxed template types may be bound
	// later, not necessarily upon first instantiation.  
	if (!_type) {
		// print error message
		_alias.dump_hierarchical_name(cerr << "\tinstance: ") << endl;
		return good_bool(false);
	}
#else
	INVARIANT(_type);
#endif
	return canonical_type_footprint_frame_policy<canonical_definition_type>
		::initialize_and_assign(_type, ff, sm, pmc, ind);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_ACTUALS_TCC__

