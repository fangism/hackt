/**
	\file "Object/inst/internal_aliases_policy_fwd.h"
	Definitions appear in "Object/inst/internal_aliases_policy.h"
	$Id: internal_aliases_policy_fwd.h,v 1.2.20.1 2005/12/11 00:45:37 fang Exp $
 */

#ifndef	__OBJECT_INST_INTERNAL_ALIASES_POLICY_FWD_H__
#define	__OBJECT_INST_INTERNAL_ALIASES_POLICY_FWD_H__

namespace HAC {
namespace entity {

//=============================================================================
// forward declarations

template <bool CanAlias>
struct internal_aliases_policy;

template <>
struct internal_aliases_policy<true>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __OBJECT_INST_INTERNAL_ALIASES_POLICY_FWD_H__

