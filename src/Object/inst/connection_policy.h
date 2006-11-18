/**
	\file "Object/inst/connection_policy.h"
	Specializations for connections in the HAC language. 
	$Id: connection_policy.h,v 1.1.2.3 2006/11/18 06:07:24 fang Exp $
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
	static
	good_bool
	synchronize_flags(const this_type&, const this_type&) {
		return good_bool(true);
	}

	template <class ContainerPtrType>
	void
	initialize_direction(const ContainerPtrType&) const { }

	good_bool
	set_connection_flags(const unsigned char) const {
		return good_bool(true);
	}

	struct connection_flag_setter {
		const good_bool		status;

		explicit
		connection_flag_setter(const unsigned char) :
			status(true) { }

		/**
			No-op, directions do not apply.  
		 */
		void
		operator () (const this_type&) const { }
	};	// end struct collection_connection_flag_setter

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
private:
	typedef	directional_connect_policy<true>	this_type;
public:
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
			Not yet used.  
		 */
		CONNECTED_SHARED_PRODUCER = 0x04,
		/**
			Proposed flag for indicating whether 
			consumer connection is shared or not.  
			Only meaningful when CONNECTED_TO_CONSUMER is set.
			Not yet used.  
		 */
		CONNECTED_SHARED_CONSUMER = 0x08,

		/**
			Set if the alias is used in CHP for sending.  
			Should this be exclusive with CONNECTED_TO_PRODUCER?
		 */
		CONNECTED_CHP_PRODUCER = 0x10, 
		/**
			Set if the alias is used in CHP for receiving.  
			Should this be exclusive with CONNECTED_TO_CONSUMER?
		 */
		CONNECTED_CHP_CONSUMER = 0x20, 
		/**
			Set if may be accessed by nonmeta-indexed
			channel reference in CHP.  
			We distinguish from normal connections
			for the purpose of may-analysis.  
			Only meaningful if CONNECTED_CHP_PRODUCER is set.
		 */
		CONNECTED_NONMETA_INDEXED_PRODUCER = 0x40,
		/**
			Set if may be accessed by nonmeta-indexed
			channel reference in CHP.  
			We distinguish from normal connections
			for the purpose of may-analysis.  
			Only meaningful if CONNECTED_CHP_CONSUMER is set.
		 */
		CONNECTED_NONMETA_INDEXED_CONSUMER = 0x80,

		/**
			Derived value combination.
		 */
		CONNECTED_CHP_NONMETA_PRODUCER = CONNECTED_CHP_PRODUCER | 
			CONNECTED_NONMETA_INDEXED_PRODUCER, 
		/**
			Derived value combination.
		 */
		CONNECTED_CHP_NONMETA_CONSUMER = CONNECTED_CHP_CONSUMER | 
			CONNECTED_NONMETA_INDEXED_CONSUMER, 
		/**
			Initial value: not connected.  
			Port formal instantiations however should
			set them to accordingly.  
		 */
		DEFAULT_CONNECT_FLAGS = 0x00
	};
protected:
	unsigned char		direction_flags;
public:
	directional_connect_policy() :
		direction_flags(DEFAULT_CONNECT_FLAGS) { }

	/**
		Also print diagnostic.
	 */
	template <class AliasType>
	static
	good_bool
	synchronize_flags(AliasType&, AliasType&);

	template <class ContainerPtrType>
	void
	initialize_direction(const ContainerPtrType p);

	good_bool
	set_connection_flags(const unsigned char);

	struct connection_flag_setter {
		good_bool		status;
		const unsigned char	update;

		explicit
		connection_flag_setter(const unsigned char f) :
			status(true), update(f) { }

		void
		operator () (this_type& a) {
			if (!a.set_connection_flags(update).good) {
				status.good = false;
			}
		}
	};	// end struct connection_flag_setter

	void
	write_flags(const ostream&) const { }

	// only for non-directional channels?
	// forward_local_to_external_flags(...);

	void
	write_flags(ostream&) const;

	void
	read_flags(istream&);

};	// end struct directional_connect_policy

//=============================================================================
}	// end namesace entity
}	// end namesace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_H__


