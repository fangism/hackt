/**
	\file "Object/inst/alias_actuals.tcc"
	Definition of template methods for instance_alias_info_actuals
		and instance_alias_info_empty.
	This file was "Object/art_object_instance_alias_actuals.tcc"
		in a previous life.  
	$Id: alias_actuals.tcc,v 1.11.30.1 2006/09/05 23:32:16 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_ACTUALS_TCC__
#define	__HAC_OBJECT_INST_ALIAS_ACTUALS_TCC__

#include <iostream>
#include "Object/inst/alias_actuals.h"
#include "Object/inst/instance_alias.h"
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
#if USE_RESOLVED_DATA_TYPES
	canonical_type_type _type(_inst.__get_raw_type());
#else
	canonical_type_type _type(_inst.get_canonical_type());
#endif
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
	This simply does not work because we're collecting a 
	pointer of a temporarily created complete canonical type.  
 */
template <class AliasType>
void
instance_alias_info_actuals::collect_canonical_footprint(
		const AliasType& _alias, persistent_object_manager& m) {
	typedef	typename AliasType::container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
				complete_type_type;
	STACKTRACE_VERBOSE;
#if 0
	const complete_type_type
		_type(_alias.complete_type_actuals(*_alias.container));
	_type.collect_transient_info_base(m);
#endif
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
	20060126:
		NOTE: This may prove infeasible for relaxed template types?
		No, problems arise only when relaxed types are passed
		(completed) as port connections.  
	20060205: HACK: need to make sure the pointers used by the
		temporarily created canonical type are registered
		with the persistent object manager.  
		We forcibly collect the pointers now... :(
		Should be OK, because objects are written to local buffers.
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
	STACKTRACE_VERBOSE;
	const complete_type_type
		_type(_alias.complete_type_actuals(*_alias.container));
	check_footprint_policy<canonical_definition_type>()(_type, _f);
#if 0
	// pushed to callee
	_type.collect_transient_info_base(
		const_cast<persistent_object_manager&>(m));
#endif
	_type.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: see comments for save_canonical_footprint, above.
	We REALLY need to do away with this hackery...
 */
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
	STACKTRACE_VERBOSE;
	complete_type_type _type;
	_type.load_object_base(m, i);
	// temporary ugly hack
	// footprint_frame _frame;	// unused.
	canonical_type_footprint_frame_policy<canonical_definition_type>
		::initialize_frame_pointer_only(_type, _f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_ACTUALS_TCC__

