/**
	\file "Object/inst/connection_policy.h"
	Specializations for connections in the HAC language. 
	$Id: connection_policy.h,v 1.3 2006/12/01 23:28:49 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_H__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_H__

#include "Object/inst/connection_policy_fwd.h"
#include <iosfwd>
#include "util/boolean_types.h"

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
		/**
			Set if connected to a local port producer.  
			Mutually exclusive with 
			CONNECTED_TO_SUBSTRUCT_PRODUCER
			and CONNECTED_CHP_PRODUCER.
			Setting this means either connected to port formal.
		 */
		CONNECTED_TO_LOCAL_PRODUCER = 0x0001,
		/**
			Analogous to CONNECTED_TO_LOCAL_CONSUMER.
		 */
		CONNECTED_TO_LOCAL_CONSUMER = 0x0100,
		/**
			Set if this channel is allowed to be connected
			to other shared producers.  
			Requires ALL participants
			in an alias set to be shared to be legal.  
		 */
		CONNECTED_PRODUCER_IS_SHARED = 0x0002,
		/**
			Analogous to CONNECTED_CONSUMER_IS_SHARED.
		 */
		CONNECTED_CONSUMER_IS_SHARED = 0x0200,
		/**
			Connection state inferred from hierarchy
			when substructure is instantiated, 
			propagated from formal to actual.  
		 */
		CONNECTED_TO_SUBSTRUCT_PRODUCER = 0x0004,
		/**
			Analogous to CONNECTED_TO_SUBSTRUCT_PRODUCER.
		 */
		CONNECTED_TO_SUBSTRUCT_CONSUMER = 0x0400,
		/**
			This flag is necessary because
			CHP source of same locale is allowed 
			multiple references.
			We allow X!() to appear multiple times without error.
			However, we do not have any checks in-place yet 
			for sharing channels across CHP loop-bodies.  
			TODO: identify CHP body of origin, perhaps with 
			automatic (anonymous) tree-naming.  
		 */
		CONNECTED_CHP_PRODUCER = 0x0008,
		/**
			Analogous to CONNECTED_CHP_PRODUCER.
		 */
		CONNECTED_CHP_CONSUMER = 0x0800,
		/**
			Set if channel is known to be referenced (in CHP)
			at compile time as a producer, using meta-parameters.  
			Exclusive with IS_NONMETA.
			Implies LOCAL or SUBSTRUCT or CHP PRODUCER.  
		 */
		CONNECTED_PRODUCER_IS_META = 0x0010,
		/**
			Analogous to CONNECTED_PRODUCER_IS_META.
		 */
		CONNECTED_CONSUMER_IS_META = 0x1000,
		/**
			Set if channel may be referenced (in CHP) through
			a nonmeta index as a producer.  
			Exclusive with IS_META.
			Implies LOCAL or SUBSTRUCT PRODUCER.  
		 */
		CONNECTED_PRODUCER_IS_NONMETA = 0x0020,
		/**
			Analogous to CONNECTED_PRODUCER_IS_NONMETA.
		 */
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
		/// default value
		DEFAULT_CONNECT_FLAGS = 0x0000
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

	static
	good_bool
	check_meta_nonmeta_usage(const connection_flags_type, const char*);

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

		template <class AliasType>
		void
		operator () (AliasType& a);
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

