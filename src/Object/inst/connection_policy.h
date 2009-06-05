/**
	\file "Object/inst/connection_policy.h"
	Specializations for connections in the HAC language. 
	$Id: connection_policy.h,v 1.12 2009/06/05 16:28:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_H__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_H__

#include "Object/inst/connection_policy_fwd.h"
#include <iosfwd>
#include "util/string_fwd.h"
#include "util/boolean_types.h"

/**
	Define to 1 to add a field for checking PRS fanin/fanout, 
	and direction checking.  
	Goal: 1
	Rationale: connectivity summary needed for netlist generation.
 */
#define	BOOL_PRS_CONNECTIVITY_CHECKING		1

namespace HAC {
namespace entity {
class unroll_context;
using std::istream;
using std::ostream;
using util::good_bool;
//=============================================================================
/**
	Operations in this specialization should be no-ops.  
 */
class null_connect_policy {
	typedef	null_connect_policy		this_type;
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

	template <class AliasType>
	void
	initialize_direction(const AliasType&, const unroll_context&) const { }

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

	ostream&
	dump_attributes(ostream& o) const { return o; }

protected:
	void
	write_flags(const ostream&) const { }

	void
	read_flags(const istream&) const { }

};	// end struct null_connect_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Where bool attributes and direction checking information is maintained
	per-alias.
	As connections are made, values are also updated and propagated. 
 */
class bool_connect_policy {
	typedef	bool_connect_policy		this_type;
	/// strings for names of flags
	static const char*			attribute_names[];
protected:
	/**
		The way boolean node attributes are propagated is
		by bitwise OR, so values should be defined and chosen
		so that set bits propagate when aliases are combined.  
	 */
	enum attribute_flags {
	/**
		Stipulates that the node should be treated as combinational
		Number of bit fields is constrained by 
		sizeof(connection_flags_type)
	 */
		BOOL_IS_COMBINATIONAL	= 0x0001,
	/**
		Tells netlist generations tools NOT to automatically
		add staticizers to dynamic node.  
	 */
		BOOL_NO_AUTOKEEPER	= 0x0002,
	/**
		Attributes added by request.
	 */
		BOOL_IS_RVC1		= 0x0004,
		BOOL_IS_RVC2		= 0x0008,
		BOOL_IS_RVC3		= 0x0010,
	/**
		Set if node's fanin rules are permitted to interfere.
	 */
		BOOL_MAY_INTERFERE	= 0x0020,
		BOOL_MAY_WEAK_INTERFERE	= 0x0040,
#if BOOL_PRS_CONNECTIVITY_CHECKING
	/**
		The first four flags are properties from participation
		in production rules and propagated from subinstances.
		Since these are local properties, they should be 
		masked out and replaced with their substructure
		counterparts.  
		These are only computed, never set by user directly.
	 */
		BOOL_LOCAL_PRS_FANOUT_PULL_DN	= 0x0100,
		BOOL_LOCAL_PRS_FANOUT_PULL_UP	= 0x0200,
		BOOL_LOCAL_PRS_FANIN_PULL_DN	= 0x0400,
		BOOL_LOCAL_PRS_FANIN_PULL_UP	= 0x0800,
		/// derived flag: all local prs fields
		BOOL_LOCAL_PRS_MASK =
			BOOL_LOCAL_PRS_FANOUT_PULL_DN |
			BOOL_LOCAL_PRS_FANOUT_PULL_UP |
			BOOL_LOCAL_PRS_FANIN_PULL_DN |
			BOOL_LOCAL_PRS_FANIN_PULL_UP,
			
	/**
		Fanin/fanout inherited properties from substructures.  
	 */
		BOOL_SUBSTRUCT_FANOUT_PULL_DN	= 0x1000,
		BOOL_SUBSTRUCT_FANOUT_PULL_UP	= 0x2000,
		BOOL_SUBSTRUCT_FANIN_PULL_DN	= 0x4000,
		BOOL_SUBSTRUCT_FANIN_PULL_UP	= 0x8000,
		/// derived flag: all substructure field
		BOOL_SUBSTRUCT_PRS_MASK =
			BOOL_SUBSTRUCT_FANOUT_PULL_DN |
			BOOL_SUBSTRUCT_FANOUT_PULL_UP |
			BOOL_SUBSTRUCT_FANIN_PULL_DN |
			BOOL_SUBSTRUCT_FANIN_PULL_UP,
		// more derived flags:
		BOOL_ANY_FANOUT_PULL_DN =
			BOOL_LOCAL_PRS_FANOUT_PULL_DN |
			BOOL_SUBSTRUCT_FANOUT_PULL_DN,
		BOOL_ANY_FANOUT_PULL_UP =
			BOOL_LOCAL_PRS_FANOUT_PULL_UP |
			BOOL_SUBSTRUCT_FANOUT_PULL_UP,
		BOOL_ANY_FANIN_PULL_DN =
			BOOL_LOCAL_PRS_FANIN_PULL_DN |
			BOOL_SUBSTRUCT_FANIN_PULL_DN,
		BOOL_ANY_FANIN_PULL_UP =
			BOOL_LOCAL_PRS_FANIN_PULL_UP |
			BOOL_SUBSTRUCT_FANIN_PULL_UP,
		BOOL_ANY_FANOUT =
			BOOL_ANY_FANOUT_PULL_DN | BOOL_ANY_FANOUT_PULL_UP,
		BOOL_ANY_FANIN =
			BOOL_ANY_FANIN_PULL_DN | BOOL_ANY_FANIN_PULL_UP,
#if 0
		// The following are properties that are specified by
		// the user in source.
		/***
			Some of these attributes should be masked out
			before passing up the hierarchy.
			These are useful for annotating ports.
			Could these be useful for annotating
			structured instances like user-defined channels
			and datatypes?
			A final check over local nodes should detect
			conflict.  
		 ***/
		/// promise not to drive from this level or subinstances
		BOOL_READ_ONLY		= 0x0100,
		/// promise to drive from this level or subinstances
		BOOL_MUST_WRITE		= 0x0200,
		/**
			indicates that fanins may come from more than one
			level of hierarchy.  
			Should this flag stick and be propagated up hierarchy?
		 */
		BOOL_MAY_SHARE_WRITE	= 0x0400
#endif
		/**
			The set of flags that should be OR-combined
			together when connecting aliases
		 */
		BOOL_CONNECTIVITY_OR_MASK	=
			BOOL_LOCAL_PRS_MASK | BOOL_SUBSTRUCT_PRS_MASK,
#endif	// BOOL_PRS_CONNECTIVITY_CHECKING
	/// mask for attributes to distinguish from connectivity fields
		BOOL_ATTRIBUTES_MASK	= 0x00FF,
		BOOL_DEFAULT_ATTRIBUTES = 0x0000
	};
	/**
		Contains both user-attached attributes and
		locally inferred connectivity information.
	 */
	connection_flags_type			attributes;

public:
	bool_connect_policy() : attributes(BOOL_DEFAULT_ATTRIBUTES) { }
protected:
	static
	good_bool
	synchronize_flags(this_type& l, this_type& r);

	template <class AliasType>
	void
	initialize_direction(const AliasType&, const unroll_context&);

	void
	initialize_actual_direction(const this_type&);

public:
	good_bool
	set_connection_flags(const connection_flags_type);

	bool
	has_nondefault_attributes(void) const {
		return attributes & BOOL_ATTRIBUTES_MASK;
		// if any attribute bits are set
	}

	void
	set_may_interfere(void) {
		attributes |= BOOL_MAY_INTERFERE;
	}

	void
	set_may_weak_interfere(void) {
		attributes |= BOOL_MAY_WEAK_INTERFERE;
	}

	bool
	may_interfere(void) const {
		return attributes & BOOL_MAY_INTERFERE;
	}

	bool
	may_weak_interfere(void) const {
		return attributes & BOOL_MAY_WEAK_INTERFERE;
	}

	void
	set_is_comb(void) {
		attributes |= BOOL_IS_COMBINATIONAL;
	}

	void
	set_no_autokeeper(void) {
		attributes |= BOOL_NO_AUTOKEEPER;
	}

	void
	set_is_rvc1(void) {
		attributes |= BOOL_IS_RVC1;
	}

	void
	set_is_rvc2(void) {
		attributes |= BOOL_IS_RVC2;
	}

	void
	set_is_rvc3(void) {
		attributes |= BOOL_IS_RVC3;
	}

#if BOOL_PRS_CONNECTIVITY_CHECKING
	bool
	has_any_fanin(void) const {
		return attributes & BOOL_ANY_FANIN;
	}

	bool
	has_any_fanout(void) const {
		return attributes & BOOL_ANY_FANOUT;
	}

	void
	prs_fanout(const bool dir) {
		attributes |= dir ?
			BOOL_LOCAL_PRS_FANOUT_PULL_UP :
			BOOL_LOCAL_PRS_FANOUT_PULL_DN;
	}

	void
	prs_fanin(const bool dir) {
		attributes |= dir ?
			BOOL_LOCAL_PRS_FANIN_PULL_UP :
			BOOL_LOCAL_PRS_FANIN_PULL_DN;
	}
#endif

protected:
	template <class AliasType>
	void
	__update_flags(AliasType&);

	template <class AliasType>
	static
	good_bool
	__check_connection(const AliasType&);

public:
	struct connection_flag_setter {
		const connection_flags_type	update;
		good_bool			status;

		explicit
		connection_flag_setter(const connection_flags_type f) :
			update(f), status(true) { }

		template <class AliasType>
		void
		operator () (AliasType&);
	};	// end struct collection_connection_flag_setter

	ostream&
	dump_attributes(ostream&) const;

	ostream&
	dump_flat_attributes(ostream&) const;

	ostream&
	dump_split_attributes(ostream&, const std::string&) const;

protected:
	void
	write_flags(ostream&) const;

	void
	read_flags(istream&);
};	// end struct bool_connect_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Specialized operations for direction-sensitive meta-types.  
 */
struct channel_connect_policy {
private:
	typedef	channel_connect_policy		this_type;
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
	channel_connect_policy() :
		direction_flags(DEFAULT_CONNECT_FLAGS) { }

protected:
	/**
		Also print diagnostic.
	 */
	template <class AliasType>
	static
	good_bool
	synchronize_flags(AliasType&, AliasType&);

public:
	good_bool
	set_connection_flags(const connection_flags_type);

	static
	good_bool
	check_meta_nonmeta_usage(const connection_flags_type, const char*);

protected:
	template <class AliasType>
	void
	initialize_direction(const AliasType&, const unroll_context&);

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

	// nothing yet, TODO: print out summary of connection state?
	ostream&
	dump_attributes(ostream& o) const { return o; }

	void
	write_flags(ostream&) const;

	void
	read_flags(istream&);

};	// end struct channel_connect_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Mostly just redirects many calls to substructure, for recursion.
 */
struct process_connect_policy : public null_connect_policy {
// parent type is temporary as we gradually implement this...
private:
	typedef	process_connect_policy		this_type;
public:
	// default ctor
private:
	using null_connect_policy::initialize_direction;

protected:

	template <class AliasType>
	void
	initialize_direction(AliasType&, const unroll_context&);

};	// end struct process_connect_policy

//=============================================================================
}	// end namesace entity
}	// end namesace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_H__


