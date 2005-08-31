/**
	\file "Object/inst/internal_aliases_policy.h"
	$Id: internal_aliases_policy.h,v 1.1.2.2 2005/08/31 06:19:27 fang Exp $
 */

#ifndef	__OBJECT_INST_INTERNAL_ALIASES_POLICY_H__
#define	__OBJECT_INST_INTERNAL_ALIASES_POLICY_H__

#include "Object/inst/internal_aliases_policy_fwd.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
class port_alias_signature;
using util::good_bool;

//=============================================================================
/**
	Uses the can_internally_alias meta-class trait.  
 */
template <bool CanAlias>
struct internal_aliases_policy {

	template <class AliasType>
	static
	good_bool
	connect(const AliasType&) {
		return good_bool(true);
	}

	/**
		Variant where the canonical type is already evaluated.
	 */
	template <class AliasType, class CanonicalType>
	static
	good_bool
	connect(const AliasType&, const CanonicalType&) {
		return good_bool(true);
	}

};

//-----------------------------------------------------------------------------
/**
	Specialized for meta-types that may have internal aliases.  
 */
template <>
struct internal_aliases_policy<true> {

	template <class AliasType>
	static
	good_bool
	connect(AliasType&);

	/**
		Variant where the canonical type is already evaluated, 
		and need not be re-evaluated.  
	 */
	template <class AliasType, class CanonicalType>
	static
	good_bool
	connect(AliasType&, const CanonicalType&);

	template <class AliasType>
	static
	good_bool
	connect(AliasType&, const port_alias_signature&);

};	// end struct_internal_aliases_policy

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INTERNAL_ALIASES_POLICY_H__

