/**
	\file "Object/inst/internal_alaises_policy.tcc"
	$Id: internal_aliases_policy.tcc,v 1.4 2006/01/22 18:20:07 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_INTERNAL_ALIASES_POLICY_TCC__
#define	__HAC_OBJECT_INST_INTERNAL_ALIASES_POLICY_TCC__

#include "Object/inst/internal_aliases_policy.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/port_alias_tracker.h"
#include "Object/def/footprint.h"
#include "Object/type/canonical_type.h"
#include "util/memory/excl_ptr.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
//=============================================================================
// class internal_aliases_policy method definitions

/**
	THIS IS WRONG AND OBSOLETE.
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
	typedef	typename AliasType::instance_collection_generic_type
						container_type;
	typedef	typename container_type::instance_collection_parameter_type
						canonical_type_type;
	typedef	typename canonical_type_type::canonical_definition_type
						definition_type;
	STACKTRACE_VERBOSE;
	const container_type& c(*_alias.container);
	const canonical_type_type
		_type(_alias.complete_type_actuals(c));
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
	const footprint&
		fp(def->get_footprint(_type.get_raw_template_params()));
	const port_alias_tracker& pt(fp.get_port_alias_tracker());
	return pt.replay_internal_aliases(_alias);
}

//=============================================================================
}	// end namespace enity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_INTERNAL_ALIASES_POLICY_TCC__

