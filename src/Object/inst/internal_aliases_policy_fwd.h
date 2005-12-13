/**
	\file "Object/inst/internal_aliases_policy_fwd.h"
	Definitions appear in "Object/inst/internal_aliases_policy.h"
	$Id: internal_aliases_policy_fwd.h,v 1.3 2005/12/13 04:15:30 fang Exp $
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

