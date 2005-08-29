/**
	\file "Object/inst/internal_alaises_policy.tcc"
	$Id: internal_aliases_policy.tcc,v 1.1.2.1 2005/08/29 21:32:05 fang Exp $
 */

#ifndef	__OBJECT_INST_INTERNAL_ALIASES_POLICY_TCC__
#define	__OBJECT_INST_INTERNAL_ALIASES_POLICY_TCC__

#include "Object/inst/internal_aliases_policy.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/port_alias_signature.h"
#include "Object/def/footprint.h"
#include "Object/type/canonical_type.h"
#include "Object/def/port_formals_manager.h"
#include "util/memory/excl_ptr.h"

namespace ART {
namespace entity {
//=============================================================================
// class internal_aliases_policy method definitions

/**
	\param AliasType must have substructure.  
	Pretty much only used for processes.  
 */
template <class AliasType>
good_bool
internal_aliases_policy<true>::connect(AliasType& _alias) {
	typedef	typename AliasType::container_type::parent_type
						container_type;
	typedef	typename container_type::instance_collection_parameter_type
						canonical_type_type;
	const container_type& c(*_alias.container);
#if 0
	canonical_type_type t(c.get_canonical_type());
#endif
	const canonical_type_type
		_type(_alias.complete_type_actuals(c));
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
	const never_ptr<const definition_type> def(_type.get_base_def());
	const footprint&
		fp(def->get_footprint(_type.get_raw_template_params()));
	const port_formals_manager& pfm(def->get_port_formals());
	const port_alias_signature asig(pfm, fp);
	return _alias.connect_port_aliases(asig);
}

//=============================================================================
}	// end namespace enity
}	// end namespace ART

#endif	// __OBJECT_INST_INTERNAL_ALIASES_POLICY_TCC__

