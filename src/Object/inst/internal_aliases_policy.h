/**
	\file "Object/inst/internal_aliases_policy.h"
 */

#ifndef	__OBJECT_INST_INTERNAL_ALIASES_POLICY_H__
#define	__OBJECT_INST_INTERNAL_ALIASES_POLICY_H__

#include "util/boolean_types.h"

namespace ART {
namespace entity {
using util::good_bool;

//=============================================================================
// forward declaration

template <bool CanAlias>
struct internal_aliases_policy;

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

};	// end struct_internal_aliases_policy

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_INTERNAL_ALIASES_POLICY_H__

