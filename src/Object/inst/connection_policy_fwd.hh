/**
	\file "Object/inst/connection_policy_fwd.hh"
	Specializations for connections in the HAC language. 
	These are referenced from the meta-type traits classes defined in
	"Object/traits/....h" headers.
	$Id: connection_policy_fwd.hh,v 1.4 2010/07/01 20:20:25 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_FWD_H__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_FWD_H__

namespace HAC {
namespace entity {
//=============================================================================
/**
	Empty directional connection policy.
 */
class null_connect_policy;

/**
	Connection policy for bools.  
	TODO: direction checking, attributes, etc...
 */
class bool_connect_policy;			// coming soon...
/**
	These are to be used as base classes for instance_alias_info.  
	\param B is true if instances have directional properties
		such as channels.  
 */
class channel_connect_policy;

/**
	Flags type for direction checking.  
	Needs to be big enough to hold all flags, 
	defined in directional_connect_policy<channel_tag>.
 */
// typedef	unsigned char	connection_flags_type;
// typedef	unsigned short	connection_flags_type;
typedef	unsigned int	connection_flags_type;
// assumes int is 32! better specify uint32_t instead?

//=============================================================================
}	// end namesace entity
}	// end namesace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_FWD_H__


