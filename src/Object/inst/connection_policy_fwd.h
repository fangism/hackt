/**
	\file "Object/inst/connection_policy_fwd.h"
	Specializations for connections in the HAC language. 
	$Id: connection_policy_fwd.h,v 1.1.2.1 2006/11/30 05:05:00 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_FWD_H__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_FWD_H__

namespace HAC {
namespace entity {
//=============================================================================
/**
	These are to be used as base classes for instance_alias_info.  
	\param B is true if instances have directional properties
		such as channels.  
 */
template <bool B>
struct directional_connect_policy;

template <>
class directional_connect_policy<false>;

template <>
struct directional_connect_policy<true>;

/**
	Flags type for direction checking.  
	Needs to be big enough to hold all flags, 
	defined in directional_connect_policy_true.
 */
// typedef	unsigned char	connection_flags_type;
typedef	unsigned short	connection_flags_type;

//=============================================================================
}	// end namesace entity
}	// end namesace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_FWD_H__


