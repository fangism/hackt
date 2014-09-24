/**
	\file "Object/inst/internal_aliases_policy.tcc"
	$Id: internal_aliases_policy.tcc,v 1.7 2007/07/18 23:28:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INTERNAL_ALIASES_POLICY_TCC__
#define	__HAC_OBJECT_INST_INTERNAL_ALIASES_POLICY_TCC__

#include "Object/inst/internal_aliases_policy.hh"
#include "Object/inst/substructure_alias_base.hh"
#include "Object/inst/port_alias_tracker.hh"
#include "Object/def/footprint.hh"
#include "Object/type/canonical_type.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/collection_interface.hh"
#include "Object/common/dump_flags.hh"
#include "util/memory/excl_ptr.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
//=============================================================================
// class internal_aliases_policy method definitions

/**
	THIS IS WRONG AND OBSOLETE.
	(seems OK to me... 20060308, and this is actually called by
		instance_array::create_dependent_types.)
	Replays a structure's internal aliases explicitly.  
	\param AliasType must have substructure.  
	\param _alias must be a top-most in scope alias, 
		either top-level overall or top-level inside a definition.  
	Pretty much only used for processes.  
	TODO: apply alias's parent_types' signatures.
 */
template <class AliasType>
good_bool
internal_aliases_policy<true>::connect(AliasType& _alias) {
	typedef	typename AliasType::canonical_container_type
						container_type;
	typedef	typename container_type::instance_collection_parameter_type
						canonical_type_type;
	typedef	typename canonical_type_type::canonical_definition_type
						definition_type;
	STACKTRACE_VERBOSE;
	const container_type& c(_alias.container->get_canonical_collection());
	const canonical_type_type _type(_alias.complete_type_actuals(c));
#if ENABLE_STACKTRACE
	_type.dump(STACKTRACE_INDENT << "canonical-type: ") << endl;
#endif
	return connect(_alias, _type);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overloaded with canonical type already specified.  
 */
template <class AliasType, class CanonicalType>
good_bool
internal_aliases_policy<true>::connect(AliasType& _alias, 
		const CanonicalType& _type) {
	typedef	typename CanonicalType::canonical_definition_type
			definition_type;
//	STACKTRACE_VERBOSE;
	const never_ptr<const definition_type> def(_type.get_base_def());
	typedef	typename CanonicalType::const_param_list_ptr_type
			param_list_ptr_type;
	const param_list_ptr_type& p(_type.get_raw_template_params());
	const footprint& fp(def->get_footprint(p));
	const port_alias_tracker& pt(fp.get_port_alias_tracker());
	const good_bool ret(pt.replay_internal_aliases(_alias));
#if ENABLE_STACKTRACE
	_alias.dump_ports(STACKTRACE_INDENT_PRINT("ports after replay:\n"),
		dump_flags::default_value) << endl;
#endif
	return ret;
}

//=============================================================================
}	// end namespace enity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INTERNAL_ALIASES_POLICY_TCC__

