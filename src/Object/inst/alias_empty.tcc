/**
	\file "Object/inst/alias_empty.tcc"
	$Id: alias_empty.tcc,v 1.10.88.1 2010/01/13 17:43:34 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_ALIAS_EMPTY_TCC__
#define	__HAC_OBJECT_INST_ALIAS_EMPTY_TCC__

#include <iostream>
#include "Object/inst/alias_empty.h"
#include "Object/common/dump_flags.h"
#include "Object/type/canonical_type.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
/**
	Implementation policy class.
	\param B whether or not meta_class can have substructure.  
 */
template <bool B>
struct instance_alias_info_empty_helper;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct instance_alias_info_empty_helper<true> {

template <class AliasType>
static
good_bool
__initialize_assign_footprint_frame(
		const AliasType& _alias, footprint_frame& ff,
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		state_manager& sm,
#endif
		const port_member_context& pmc,
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
		::initialize_and_assign(_type, ff, 
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
			sm, 
#endif
			pmc, ind);
}

};	// end struct instance_alias_info_empty_helper<true>

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct instance_alias_info_empty_helper<false> {

template <class AliasType>
static
good_bool
__initialize_assign_footprint_frame(const AliasType&, const footprint_frame&,
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	const state_manager&, 
#endif
	const port_member_context&, const size_t) {
	return good_bool(true);
}

};	// end struct instance_alias_info_empty_helper<false>

//=============================================================================
// class instance_alias_info_empty method definitions

#if 0
// need not be defined, never used or called.  
/**
	\param _alias the instance alias to dump type info.  
	\param o the output stream.
	\param f the footprint is just used for consistency checking.  
		However, no meta types without relaxed actuals have
		footprints, so it is just ignored (should be null).
 */
template <class AliasType>
ostream&
instance_alias_info_empty::dump_complete_type(const AliasType& _alias, 
		ostream& o, const footprint* const f) {
	const complete_type_type
		_type(_alias.container->get_canonical_type());
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param _alias the instance alias whose type is to be unrolled
		and created.  
 */
template <class AliasType>
good_bool
instance_alias_info_empty::create_dependent_types(const AliasType& _alias, 
		const footprint& top) {
	typedef	typename AliasType::canonical_container_type	container_type;
	typedef typename container_type::instance_collection_parameter_type
				complete_type_type;
	STACKTRACE_VERBOSE;
	const complete_type_type
		_type(_alias.container->get_canonical_collection()
			.__get_raw_type());
	if (!_type) {
		// already have error message
		_alias.dump_hierarchical_name(cerr << "Failed to instantiate ", 
			dump_flags::default_value) << endl;
		return good_bool(false);
	}
	else if (!container_type::collection_type_manager_parent_type
			::create_definition_footprint(_type, top).good) {
		// have error message already
		_alias.dump_hierarchical_name(cerr << "Instantiated by: ", 
			dump_flags::default_value) << endl;
		return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// All the code below was copy-adapted from instance_alias_info_actuals
// when we decided that datatypes and channels should NOT have relaxed typing.  
// This is temporary until we find a better way to save/restore
// footprint pointers without going through such hackery.  

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**             
	Prints the complete-type to which the footprint belongs.  
	\param _alias the instance alias whose type should be dumped.  
	\param o the output stream.  
	\param f is only used for assertion checking.  
 */
template <class AliasType>
ostream&        
instance_alias_info_empty::dump_complete_type(const AliasType& _alias,
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
instance_alias_info_empty::__initialize_assign_footprint_frame(
		const AliasType& _alias, footprint_frame& ff,
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		state_manager& sm,
#endif
		const port_member_context& pmc,
		const size_t ind) {
	typedef	typename AliasType::traits_type		traits_type;
	return instance_alias_info_empty_helper<traits_type::has_substructure>
		::template __initialize_assign_footprint_frame<AliasType>(
			_alias, ff, 
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
				sm, 
#endif
				pmc, ind);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_ALIAS_EMPTY_TCC__

