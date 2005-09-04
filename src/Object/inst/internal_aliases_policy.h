/**
	\file "Object/inst/internal_aliases_policy.h"
	$Id: internal_aliases_policy.h,v 1.1.2.4 2005/09/04 06:23:00 fang Exp $
 */

#ifndef	__OBJECT_INST_INTERNAL_ALIASES_POLICY_H__
#define	__OBJECT_INST_INTERNAL_ALIASES_POLICY_H__

#include "Object/inst/internal_aliases_policy_fwd.h"
#include "util/boolean_types.h"
#include "Object/devel_switches.h"

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

	// this is wrong, obsolete
	template <class AliasType>
	static
	good_bool
	connect(const AliasType&) {
		return good_bool(true);
	}

#if 0
	// replacement for the above
	template <class AliasType>
	static
	good_bool
	connect(const AliasType&, const AliasType&) {
		return good_bool(true);
	}
#endif

#if 0
private:
	/**
		Variant where the canonical type is already evaluated.
	 */
	template <class AliasType, class CanonicalType>
	static
	good_bool
	connect(const AliasType&, const CanonicalType&) {
		return good_bool(true);
	}
#endif
};

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

#if 0
	// replacement for the above
	template <class AliasType>
	static
	good_bool
	connect(AliasType&, const AliasType&);
#endif

private:
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

