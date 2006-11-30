/**
	\file "Object/inst/connection_policy.h"
	Specializations for connections in the HAC language. 
	$Id: connection_policy.h,v 1.2.2.3 2006/11/30 23:13:54 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_H__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_H__

#include "Object/inst/connection_policy_fwd.h"
#include <iosfwd>
#include "util/boolean_types.h"

/**
	Define to 1 to use new set of connection flags.  
 */
#define	NEW_CONNECTION_FLAGS			1

namespace HAC {
namespace entity {
using std::istream;
using std::ostream;
using util::good_bool;
//=============================================================================
/**
	Operations in this specialization should be no-ops.  
 */
template <>
class directional_connect_policy<false> {
	typedef	directional_connect_policy<false>	this_type;
	// default ctor and dtor
protected:
	/**
		No-op.  
	 */
	static
	good_bool
	synchronize_flags(const this_type&, const this_type&) {
		return good_bool(true);
	}

	template <class ContainerType>
	void
	initialize_direction(const ContainerType&) const { }

	void
	initialize_actual_direction(const this_type&) const { }

public:
	good_bool
	set_connection_flags(const connection_flags_type) const {
		return good_bool(true);
	}

protected:
	void
	__update_flags(const this_type&) const { }

	static
	good_bool
	__check_connection(const this_type&) {
		return good_bool(true);
	}

public:
	struct connection_flag_setter {
		const good_bool		status;

		explicit
		connection_flag_setter(const connection_flags_type) :
			status(true) { }

		/**
			No-op, directions do not apply.  
		 */
		void
		operator () (const this_type&) const { }
	};	// end struct collection_connection_flag_setter

protected:
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
#if NEW_CONNECTION_FLAGS
		/**
			Mutually exclusive with 
			CONNECTED_TO_SUBSTRUCT_PRODUCER
			and CONNECTED_CHP_PRODUCER.
			Setting this means either connected to port formal.
		 */
		CONNECTED_TO_LOCAL_PRODUCER = 0x0001,
		CONNECTED_TO_LOCAL_CONSUMER = 0x0100,
		/**
			Access is shared.  Requires ALL participants
			in an alias set to be shared to be legal.  
		 */
		CONNECTED_PRODUCER_IS_SHARED = 0x0002,
		CONNECTED_CONSUMER_IS_SHARED = 0x0200,
		/**
			Connection inferred from substructure, 
			propagated from formal to actual.  
		 */
		CONNECTED_TO_SUBSTRUCT_PRODUCER = 0x0004,
		CONNECTED_TO_SUBSTRUCT_CONSUMER = 0x0400,
		/**
			CHP source of same locale is allowed 
			multiple references.
		 */
		CONNECTED_CHP_PRODUCER = 0x0008,
		CONNECTED_CHP_CONSUMER = 0x0800,
		/**
			Exclusive with IS_NONMETA.
			Implies LOCAL or SUBSTRUCT PRODUCER.  
		 */
		CONNECTED_PRODUCER_IS_META = 0x0010,
		CONNECTED_CONSUMER_IS_META = 0x1000,
		/**
			Exclusive with IS_META.
			Implies LOCAL or SUBSTRUCT PRODUCER.  
		 */
		CONNECTED_PRODUCER_IS_NONMETA = 0x0020,
		CONNECTED_CONSUMER_IS_NONMETA = 0x2000,
		// derived values
		CONNECTED_PORT_FORMAL_PRODUCER =
			CONNECTED_TO_LOCAL_PRODUCER |
			CONNECTED_PRODUCER_IS_META,	// maybe shared later
		CONNECTED_PORT_FORMAL_CONSUMER =
			CONNECTED_TO_LOCAL_CONSUMER |
			CONNECTED_CONSUMER_IS_META,	// maybe shared later
		CONNECTED_TO_ANY_PRODUCER = 
			CONNECTED_TO_LOCAL_PRODUCER |
			CONNECTED_CHP_PRODUCER |
			CONNECTED_TO_SUBSTRUCT_PRODUCER,
		CONNECTED_TO_ANY_CONSUMER = 
			CONNECTED_TO_LOCAL_CONSUMER |
			CONNECTED_CHP_CONSUMER |
			CONNECTED_TO_SUBSTRUCT_CONSUMER,
		CONNECTED_TO_NONCHP_PRODUCER = 
			CONNECTED_TO_LOCAL_PRODUCER |
			CONNECTED_TO_SUBSTRUCT_PRODUCER,
		CONNECTED_TO_NONCHP_CONSUMER = 
			CONNECTED_TO_LOCAL_CONSUMER |
			CONNECTED_TO_SUBSTRUCT_CONSUMER,
		CONNECTED_TO_CHP_META_PRODUCER =
			CONNECTED_CHP_PRODUCER |
			CONNECTED_PRODUCER_IS_META,
		CONNECTED_TO_CHP_META_CONSUMER =
			CONNECTED_CHP_CONSUMER |
			CONNECTED_CONSUMER_IS_META,
		CONNECTED_TO_CHP_NONMETA_PRODUCER =
			CONNECTED_CHP_PRODUCER |
			CONNECTED_PRODUCER_IS_NONMETA,
		CONNECTED_TO_CHP_NONMETA_CONSUMER =
			CONNECTED_CHP_CONSUMER |
			CONNECTED_CONSUMER_IS_NONMETA,
		/// exclusion violation detection
#if 0
		CONNECTED_META_NONMETA_PRODUCER =
			CONNECTED_PRODUCER_IS_META |
			CONNECTED_PRODUCER_IS_NONMETA,
		CONNECTED_META_NONMETA_CONSUMER =
			CONNECTED_CONSUMER_IS_META |
			CONNECTED_CONSUMER_IS_NONMETA,
#endif
		/// default value
		DEFAULT_CONNECT_FLAGS = 0x0000
#else	// NEW_CONNECTION_FLAGS
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
		CONNECTED_ANY_PRODUCER = CONNECTED_TO_PRODUCER |
			CONNECTED_CHP_PRODUCER,
		/**
			Derived value combination.
		 */
		CONNECTED_ANY_CONSUMER = CONNECTED_TO_CONSUMER |
			CONNECTED_CHP_CONSUMER,
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
#endif	// NEW_CONNECTION_FLAGS
	};
protected:
	connection_flags_type		direction_flags;
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

	good_bool
	set_connection_flags(const connection_flags_type);

protected:
	template <class ContainerType>
	void
	initialize_direction(const ContainerType&);

	template <class AliasType>
	void
	initialize_actual_direction(const AliasType&);

	template <class AliasType>
	void
	__update_flags(AliasType&);

	template <class AliasType>
	static
	good_bool
	__check_connection(const AliasType&);

public:
	struct connection_flag_setter {
		good_bool		status;
		const connection_flags_type	update;

		explicit
		connection_flag_setter(const connection_flags_type f) :
			status(true), update(f) { }

		void
		operator () (this_type& a) {
			if (!a.set_connection_flags(update).good) {
				status.good = false;
			}
		}
	};	// end struct connection_flag_setter

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


