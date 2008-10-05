/**
	\file "Object/inst/connection_policy_fwd.h"
	Specializations for connections in the HAC language. 
	These are referenced from the meta-type traits classes defined in
	"Object/traits/....h" headers.
	$Id: connection_policy_fwd.h,v 1.3 2008/10/05 23:00:11 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_FWD_H__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_FWD_H__

namespace HAC {
namespace entity {
//=============================================================================
/**
	Empty directional connection policy.
 */
struct null_connect_policy;

/**
	Connection policy for bools.  
	TODO: direction checking, attributes, etc...
 */
struct bool_connect_policy;			// coming soon...
/**
	These are to be used as base classes for instance_alias_info.  
	\param B is true if instances have directional properties
		such as channels.  
 */
struct channel_connect_policy;

/**
	Flags type for direction checking.  
	Needs to be big enough to hold all flags, 
	defined in directional_connect_policy<channel_tag>.
 */
// typedef	unsigned char	connection_flags_type;
typedef	unsigned short	connection_flags_type;

//=============================================================================
}	// end namesace entity
}	// end namesace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_FWD_H__


