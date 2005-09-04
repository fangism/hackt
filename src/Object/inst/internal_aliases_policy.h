/**
	\file "Object/inst/internal_aliases_policy.h"
	$Id: internal_aliases_policy.h,v 1.1.2.5 2005/09/04 18:10:44 fang Exp $
 */

#ifndef	__OBJECT_INST_INTERNAL_ALIASES_POLICY_H__
#define	__OBJECT_INST_INTERNAL_ALIASES_POLICY_H__

#include "Object/inst/internal_aliases_policy_fwd.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
using util::good_bool;

//=============================================================================
/**
	Uses the can_internally_alias meta-class trait.  
 */
template <bool CanAlias>
struct internal_aliases_policy {

	// this is wrong, obsolete
	template <class AliasType>
	static
	good_bool
	connect(const AliasType&) {
		return good_bool(true);
	}

};	// end struct internal_aliases_policy

//-----------------------------------------------------------------------------
/**
	Specialized for meta-types that may have internal aliases.  
 */
template <>
struct internal_aliases_policy<true> {

	// wrong, obsolete
	template <class AliasType>
	static
	good_bool
	connect(AliasType&);

private:
	/**
		Variant where the canonical type is already evaluated, 
		and need not be re-evaluated.  
	 */
	template <class AliasType, class CanonicalType>
	static
	good_bool
	connect(AliasType&, const CanonicalType&);

};	// end struct_internal_aliases_policy

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INTERNAL_ALIASES_POLICY_H__

