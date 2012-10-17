/**
	\file "Object/inst/alias_actuals.tcc"
	Definition of template methods for instance_alias_info_actuals
		and instance_alias_info_empty.
	This file was "Object/art_object_instance_alias_actuals.tcc"
		in a previous life.  
	$Id: alias_actuals.tcc,v 1.21 2010/04/07 00:12:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_ACTUALS_TCC__
#define	__HAC_OBJECT_INST_ALIAS_ACTUALS_TCC__

#include <iostream>
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/type/canonical_type.hh"
#include "util/stacktrace.hh"
#include "Object/expr/expr_dump_context.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
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
	if (!container_type::collection_type_manager_parent_type
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
	When connecting two relaxed aliases, copy one over to the
	other.  If they are both non-null, then they should be equivalent, 
	else reject as a connection error.  
	\param l canonical alias
	\param r canonical alias
	\pre to properly synchronize, parent collections of aliases must
		either both be strict (null params), or both be relaxed.
		We don't recheck this precondition here.
 */
template <class AliasType>
good_bool
instance_alias_info_actuals::synchronize_actuals(AliasType& l, AliasType& r, 
		const unroll_context& c) {
	STACKTRACE_VERBOSE;
	if (l.actuals) {
		if (r.actuals) {
			return compare_actuals(l.actuals, r.actuals);
		} else {
			r.actuals = l.actuals;
			// r's type is complete, instantiate r recursively
			r.instantiate_actuals_only(c);
		}
	} else {
		if (r.actuals) {
			l.actuals = r.actuals;
			// l's type is complete, instantiate l recursively
			l.instantiate_actuals_only(c);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
void
instance_alias_info_actuals::finalize_actuals_and_substructure_aliases(
		AliasType& _this, const unroll_context& c) {
	STACKTRACE_VERBOSE;
	if (_this.copy_actuals(*_this.next)) {
		STACKTRACE_INDENT_PRINT("instantiating after copying actuals");
		// may not want to initialize_direction in this call!
		_this.instantiate_actuals_only(c);
		// will throw exception upon error
	}
	// HACK:
	// If this alias has acquired relaxed actuals, replay connections.
	// This is heavy-handed and inefficient, but we resort to this
	// to guarantee correctness with types being bound after aliases
	// are formed.  see src/NOTES.
	// COST: this may be called excessively from many places...
	// FIXME: un-hacking this  will require a major overhaul of connections.
	if ((&_this != _this.next) && _this.get_relaxed_actuals() && 
		!_this.connect_port_aliases_recursive(*_this.next, c).good) {
		cerr << "Error connecting ports." << endl;
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Throws exception upon error.
	TODO: no longer need unroll_context parameter? for debugging?
 */
template <class AliasType>
void
instance_alias_info_actuals::__finalize_find(
		AliasType& _this, const unroll_context&) {
	__assert_complete_type(_this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
bool
instance_alias_info_actuals::__has_complete_type(const AliasType& _this) {
	typedef	typename AliasType::canonical_container_type	container_type;
	STACKTRACE_VERBOSE;
	const container_type& cont(_this.container->get_canonical_collection());
	return (!cont.has_relaxed_type() || _this.get_relaxed_actuals());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class AliasType>
void
instance_alias_info_actuals::__assert_complete_type(
		const AliasType& _this) {
	typedef	typename AliasType::canonical_container_type	container_type;
	STACKTRACE_VERBOSE;
	const container_type& cont(_this.container->get_canonical_collection());
	if (cont.has_relaxed_type() && !_this.get_relaxed_actuals()) {
		cerr << "Error: instance alias `";
		_this.dump_hierarchical_name(cerr) <<
			"\' has incomplete type: ";
		cont.get_resolved_canonical_type().dump(cerr) << endl;
		THROW_EXIT;
	}
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
		const AliasType& _alias, footprint_frame& ff) {
	typedef	typename AliasType::canonical_container_type	container_type;
	typedef	typename container_type::instance_collection_parameter_type
				complete_type_type;
	typedef	typename complete_type_type::canonical_definition_type
				canonical_definition_type;
	STACKTRACE_VERBOSE;
	const complete_type_type
		_type(_alias.complete_type_actuals(*_alias.container));
	// now possible because relaxed template types may be bound
	// later, not necessarily upon first instantiation.  
	if (!_type) {
		// print error message
		_alias.dump_hierarchical_name(cerr << "\tinstance: ") << endl;
		return good_bool(false);
	}
#if ENABLE_STACKTRACE
	_alias.dump_hierarchical_name(STACKTRACE_INDENT_PRINT("instance: ")) << endl;
	_type.dump(STACKTRACE_INDENT_PRINT("_type: ")) << endl;
#endif
	return canonical_type_footprint_frame_policy<canonical_definition_type>
		::initialize_and_assign(_type, ff);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_ACTUALS_TCC__

