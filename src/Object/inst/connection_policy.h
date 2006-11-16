/**
	\file "Object/inst/connection_policy.h"
	Specializations for connections in the HAC language. 
	$Id: connection_policy.h,v 1.1.2.1 2006/11/16 20:28:44 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_H__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_H__

#include <iosfwd>
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
using std::istream;
using std::ostream;
using util::good_bool;
//=============================================================================
/**
	These are to be used as base classes for instance_alias_info.  
	\param B is true if instances have directional properties
		such as channels.  
 */
template <bool B>
struct directional_connect_policy;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Operations in this specialization should be no-ops.  
 */
template <>
struct directional_connect_policy<false> {
	typedef	directional_connect_policy<false>	this_type;
	// default ctor and dtor

	/**
		No-op.  
	 */
	template <class AliasType>
	static
	good_bool
	synchronize_flags(const AliasType&, const AliasType&) {
		return good_bool(true);
	}

	void
	write_flags(const ostream&) const { }

	void
	read_flags(const istream&) const { }

};	// end struct directional_connect_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialized operations for direction-sensitive meta-types.  
 */
template <>
struct directional_connect_policy<true> {
	typedef	directional_connect_policy<true>	this_type;
	enum direction_flags {
		/**
			Set if connected to some producer of values, 
			such as a receive-port-formal, 
			or send-port-actual member, 
			be it shared or non-shared. 
		 */
		CONNECTED_TO_PRODUCER = 0x01,
		/**
			Set if connected to some consumer of values, 
			such as a send-port-formal, 
			or receive-port-actual member, 
			be it shared or non-shared. 
		 */
		CONNECTED_TO_CONSUMER = 0x02,
		/**
			Proposed flag for indicating whether 
			producer connection is shared or not.  
			Only meaningful when CONNECTED_TO_PRODUCER is set.
		 */
		CONNECTED_SHARED_PRODUCER = 0x04,
		/**
			Proposed flag for indicating whether 
			consumer connection is shared or not.  
			Only meaningful when CONNECTED_TO_CONSUMER is set.
		 */
		CONNECTED_SHARED_CONSUMER = 0x08,
		/**
			Initial value: not connected.  
			Port formal instantiations however should
			set them to accordingly.  
		 */
		DEFAULT_CONNECT_FLAGS = 0
	};
	unsigned char		direction_flags;

	directional_connect_policy() :
		direction_flags(DEFAULT_CONNECT_FLAGS) { }

	/**
		Also print diagnostic.
	 */
	template <class AliasType>
	static
	good_bool
	synchronize_flags(AliasType&, AliasType&);

	void
	write_flags(ostream&) const;

	void
	read_flags(istream&);

};	// end struct directional_connect_policy

//=============================================================================
}	// end namesace entity
}	// end namesace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_H__


