/**
	\file "Object/inst/connection_policy.hh"
	Specializations for connections in the HAC language. 
	$Id: connection_policy.hh,v 1.22 2011/05/02 21:27:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_CONNECTION_POLICY_H__
#define	__HAC_OBJECT_INST_CONNECTION_POLICY_H__

#include "Object/inst/connection_policy_fwd.hh"
#include "Object/type/channel_direction_enum.hh"
#include <iosfwd>
#include "common/status.hh"		// for error_count
#include "util/string_fwd.hh"
#include "util/boolean_types.hh"

/**
	Define to 1 to add a field for checking PRS fanin/fanout, 
	and direction checking.  
	Goal: 1
	Rationale: connectivity summary needed for netlist generation.
	Status: done, fairly reliable, still testing though...
	The BOOL_CONNECTIVITY_CHECKING option is slightly different.
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

	good_bool
	declare_direction(const direction_type) const {
		return good_bool(true);
	}

protected:
	void
	__update_flags(const this_type&) const { }

	static
	error_count
	__check_connection(const this_type&) {
		return error_count();
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

	ostream&
	dump_explicit_attributes(ostream& o) const { return o; }

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
	// the atomic attribute overloads the bitfields
//	static const char*		atomic_attribute_names[];

	const char** get_attribute_names_set(void) const {
//		return is_atomic() ? atomic_attribute_names : attribute_names;
		return attribute_names;
	}
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
		BOOL_IS_COMBINATIONAL	= 0x00000001,
	/**
		Tells netlist generations tools NOT to automatically
		add staticizers to dynamic node.  
	 */
		BOOL_NO_AUTOKEEPER	= 0x00000002,
	/**
		Attributes added by request.
	 */
		BOOL_IS_RVC1		= 0x00000004,
		BOOL_IS_RVC2		= 0x00000008,
		BOOL_IS_RVC3		= 0x00000010,
	/**
		Set if node's fanin rules are permitted to interfere.
	 */
		BOOL_MAY_INTERFERE	= 0x00000020,
		BOOL_MAY_WEAK_INTERFERE	= 0x00000040,
	// supply attributes
		BOOL_SUPPLY_LOW		= 0x00000080,
		BOOL_SUPPLY_HIGH	= 0x00000100,
	// reset attributes
		BOOL_RESET_LOW		= 0x00000200,
		BOOL_RESET_HIGH		= 0x00000400,
	/**
		run-time atomic expression 'ebool'
		If this is true, many other bitfields are overloaded
		to different meanings.
	 */
		BOOL_ATOMIC		= 0x00000800,
		BOOL_ATOMIC_INIT_MASK = BOOL_ATOMIC,

	// all attributes that are explicitly written
		BOOL_EXPLICIT_NONATOMIC_ATTRIBUTES_MASK =
			BOOL_IS_COMBINATIONAL |
			BOOL_NO_AUTOKEEPER |
			BOOL_IS_RVC1 | BOOL_IS_RVC2 | BOOL_IS_RVC3 |
			BOOL_MAY_INTERFERE | BOOL_MAY_WEAK_INTERFERE |
			BOOL_SUPPLY_LOW | BOOL_SUPPLY_HIGH |
			BOOL_RESET_LOW | BOOL_RESET_HIGH,
		BOOL_EXPLICIT_ATTRIBUTES_MASK =
			BOOL_EXPLICIT_NONATOMIC_ATTRIBUTES_MASK | BOOL_ATOMIC,

#if BOOL_CONNECTIVITY_CHECKING
		// these keep their meaning as atomic ebools
		// port is marked with '?'
		BOOL_PORT_FORMAL_INPUT = 0x00100000,
		// port is marked with '!'
		BOOL_PORT_FORMAL_OUTPUT = 0x00200000,
#endif
#if BOOL_PRS_CONNECTIVITY_CHECKING
	/**
		atomic run-time expression def/use attributes (implicit)
	 */
		BOOL_LOCAL_RTE_FANOUT = 0x00010000,
		BOOL_LOCAL_RTE_FANIN = 0x00020000,
		BOOL_SUBSTRUCT_RTE_FANOUT = 0x00040000,
		BOOL_SUBSTRUCT_RTE_FANIN = 0x00080000,
		BOOL_LOCAL_RTE_MASK =
			BOOL_LOCAL_RTE_FANOUT | BOOL_LOCAL_RTE_FANIN,
		BOOL_SUBSTRUCT_RTE_MASK =
			BOOL_SUBSTRUCT_RTE_FANOUT | BOOL_SUBSTRUCT_RTE_FANIN,
		BOOL_ANY_RTE_FANOUT =
			BOOL_LOCAL_RTE_FANOUT | BOOL_SUBSTRUCT_RTE_FANOUT,
		BOOL_ANY_RTE_FANIN =
			BOOL_LOCAL_RTE_FANIN | BOOL_SUBSTRUCT_RTE_FANIN,
	/**
		This is NOT an attribute, is an intrinsic property
		automatically updated during connections.
		This really should be a general instance property, 
		not just specific to bools.  
		This should be set immediately upon instantiating 
		a port substructure.
	 */
		BOOL_IS_ALIASED_TO_PORT = 0x00800000,
	/**
		The first four flags are properties from participation
		in production rules and propagated from subinstances.
		Since these are local properties, they should be 
		masked out and replaced with their substructure
		counterparts.  
		These are only computed, never set by user directly.
	 */
		BOOL_LOCAL_PRS_FANOUT_PULL_DN	= 0x01000000,
		BOOL_LOCAL_PRS_FANOUT_PULL_UP	= 0x02000000,
		BOOL_LOCAL_PRS_FANIN_PULL_DN	= 0x04000000,
		BOOL_LOCAL_PRS_FANIN_PULL_UP	= 0x08000000,
		/// derived flag: all local prs fields
		BOOL_LOCAL_PRS_MASK =
			BOOL_LOCAL_PRS_FANOUT_PULL_DN |
			BOOL_LOCAL_PRS_FANOUT_PULL_UP |
			BOOL_LOCAL_PRS_FANIN_PULL_DN |
			BOOL_LOCAL_PRS_FANIN_PULL_UP,
			
	/**
		Fanin/fanout inherited properties from substructures.  
	 */
		BOOL_SUBSTRUCT_FANOUT_PULL_DN	= 0x10000000,
		BOOL_SUBSTRUCT_FANOUT_PULL_UP	= 0x20000000,
		BOOL_SUBSTRUCT_FANIN_PULL_DN	= 0x40000000,
		BOOL_SUBSTRUCT_FANIN_PULL_UP	= 0x80000000,
		/// derived flag: all substructure field
		BOOL_SUBSTRUCT_PRS_MASK =
			BOOL_SUBSTRUCT_FANOUT_PULL_DN |
			BOOL_SUBSTRUCT_FANOUT_PULL_UP |
			BOOL_SUBSTRUCT_FANIN_PULL_DN |
			BOOL_SUBSTRUCT_FANIN_PULL_UP,
		// more derived flags:
		BOOL_SUBSTRUCT_FANOUT =
			BOOL_SUBSTRUCT_FANOUT_PULL_DN |
			BOOL_SUBSTRUCT_FANOUT_PULL_UP,
		BOOL_SUBSTRUCT_FANIN =
			BOOL_SUBSTRUCT_FANIN_PULL_DN |
			BOOL_SUBSTRUCT_FANIN_PULL_UP,
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
		BOOL_IMPLICIT_NONATOMIC_ATTRIBUTES_MASK = 
			BOOL_SUBSTRUCT_FANOUT |
			BOOL_SUBSTRUCT_FANIN,
		BOOL_IMPLICIT_ATOMIC_ATTRIBUTES_MASK = 
			BOOL_SUBSTRUCT_RTE_MASK,
		BOOL_IMPLICIT_ATTRIBUTES_MASK = 
			BOOL_IMPLICIT_NONATOMIC_ATTRIBUTES_MASK |
			BOOL_IMPLICIT_ATOMIC_ATTRIBUTES_MASK,
	/// mask for attributes to distinguish from connectivity fields
	// both implicit and explicit attributes should be preserved
		BOOL_ATTRIBUTES_MASK	=
			BOOL_EXPLICIT_ATTRIBUTES_MASK |
			BOOL_IMPLICIT_ATTRIBUTES_MASK,
		BOOL_INIT_ATTRIBUTES_MASK	=
			BOOL_ATTRIBUTES_MASK | BOOL_IS_ALIASED_TO_PORT,
		BOOL_DEFAULT_ATTRIBUTES = 0x00000000
	};
	enum {
		// TODO: keep this in sync with above list!
		BOOL_NUM_EXPLICIT_ATTRIBUTES = 12,
		// overall number of attributes, explicit and implicit
		BOOL_NUM_ATTRIBUTES = sizeof(connection_flags_type) << 3
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

	// really should let PRS determine this
	good_bool
	declare_direction(const direction_type);

	bool
	has_nondefault_attributes(const bool i = false) const;

	size_t
	num_display_attributes(const bool i) const {
		return i ? BOOL_NUM_ATTRIBUTES : BOOL_NUM_EXPLICIT_ATTRIBUTES;
	}

	void
	flag_port(void) {
		attributes |= BOOL_IS_ALIASED_TO_PORT;
	}

	bool
	is_aliased_to_port(void) const {
		return attributes & BOOL_IS_ALIASED_TO_PORT;
	}

	void
	nonatomic_only_attribute(void) const;

	void
	atomic_only_attribute(void) const;

	void
	nonatomic_only_prs_literal(void) const;

	void
	atomic_only_rte_literal(void) const;

	void
	set_may_interfere(void) {
		nonatomic_only_attribute();
		attributes |= BOOL_MAY_INTERFERE;
	}

	void
	set_may_weak_interfere(void) {
		nonatomic_only_attribute();
		attributes |= BOOL_MAY_WEAK_INTERFERE;
	}

	bool
	may_interfere(void) const {
		nonatomic_only_attribute();
		return attributes & BOOL_MAY_INTERFERE;
	}

	bool
	may_weak_interfere(void) const {
		nonatomic_only_attribute();
		return attributes & BOOL_MAY_WEAK_INTERFERE;
	}

	void
	set_is_comb(void) {
		nonatomic_only_attribute();
		attributes |= BOOL_IS_COMBINATIONAL;
	}

	void
	set_no_autokeeper(void) {
		nonatomic_only_attribute();
		attributes |= BOOL_NO_AUTOKEEPER;
	}

	void
	set_is_rvc1(void) {
		nonatomic_only_attribute();
		attributes |= BOOL_IS_RVC1;
	}

	void
	set_is_rvc2(void) {
		nonatomic_only_attribute();
		attributes |= BOOL_IS_RVC2;
	}

	void
	set_is_rvc3(void) {
		nonatomic_only_attribute();
		attributes |= BOOL_IS_RVC3;
	}

	void
	set_supply(const bool);

	void
	set_reset(const bool);

	void
	set_atomic(const bool);

	bool
	is_atomic(void) const { return attributes & BOOL_ATOMIC; }

#if BOOL_PRS_CONNECTIVITY_CHECKING
	bool
	has_any_fanin(void) const {
		return attributes & BOOL_ANY_FANIN;
	}

	bool
	has_any_fanout(void) const {
		return attributes & BOOL_ANY_FANOUT;
	}

	// always considered safe to fanout arbitrarily
	void
	prs_fanout(const bool dir) {
		nonatomic_only_prs_literal();
		attributes |= dir ?
			BOOL_LOCAL_PRS_FANOUT_PULL_UP :
			BOOL_LOCAL_PRS_FANOUT_PULL_DN;
	}

#if BOOL_CONNECTIVITY_CHECKING
	good_bool
	prs_fanin(const bool dir);
#else
	void
	prs_fanin(const bool dir) {
		nonatomic_only_prs_literal();
		attributes |= dir ?
			BOOL_LOCAL_PRS_FANIN_PULL_UP :
			BOOL_LOCAL_PRS_FANIN_PULL_DN;
	}
#endif

	void
	rte_fanout(void) {
		// can be either atomic or non-atomic
		attributes |= BOOL_LOCAL_RTE_FANOUT;
	}

	// bool is attached to an expression (defined)
	good_bool
	rte_fanin(void);
#endif

#if BOOL_CONNECTIVITY_CHECKING
	bool
	is_input_port(void) const {
		return attributes & BOOL_PORT_FORMAL_INPUT;
	}

	bool
	is_output_port(void) const {
		return attributes & BOOL_PORT_FORMAL_OUTPUT;
	}
#endif

protected:
	template <class AliasType>
	void
	__update_flags(AliasType&);

	template <class AliasType>
	static
	error_count
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
	dump_raw_attributes(ostream&) const;

	ostream&
	dump_attributes(ostream&, const bool implicit = true) const;

	ostream&
	dump_explicit_attributes(ostream& o) const {
		return dump_attributes(o, false);
	}


	ostream&
	dump_flat_attributes(ostream&, const bool i = false) const;

	ostream&
	dump_split_attributes(ostream&, const std::string&,
		const bool i = false) const;

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
class channel_connect_policy {
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
			Mutually exclusive with 
			CONNECTED_TO_LOCAL_PRODUCER
			and CONNECTED_CHP_PRODUCER.
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
			Mutually exclusive with 
			CONNECTED_TO_LOCAL_PRODUCER
			and CONNECTED_TO_SUBSTRUCT_PRODUCER.
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
			CONNECTED_TO_LOCAL_PRODUCER,
		CONNECTED_PORT_FORMAL_CONSUMER =
			CONNECTED_TO_LOCAL_CONSUMER,
		// exclude port formal decl.
		CONNECTED_TO_NONPORT_PRODUCER =
			CONNECTED_CHP_PRODUCER |
			CONNECTED_TO_SUBSTRUCT_PRODUCER,
		CONNECTED_TO_NONPORT_CONSUMER =
			CONNECTED_CHP_CONSUMER |
			CONNECTED_TO_SUBSTRUCT_CONSUMER,
		CONNECTED_TO_ANY_PRODUCER = 
			CONNECTED_TO_LOCAL_PRODUCER |
			CONNECTED_TO_NONPORT_PRODUCER,
		CONNECTED_TO_ANY_CONSUMER = 
			CONNECTED_TO_LOCAL_CONSUMER |
			CONNECTED_TO_NONPORT_CONSUMER,
		// don't mask away CHP, preserve it
		// this is non-essential, just decided this way to
		// minimize test case differences
		ACTUAL_INITIALIZATION_MASK =
			CONNECTED_TO_LOCAL_PRODUCER |
			CONNECTED_TO_SUBSTRUCT_PRODUCER |
			CONNECTED_TO_LOCAL_CONSUMER |
			CONNECTED_TO_SUBSTRUCT_CONSUMER |
			CONNECTED_PRODUCER_IS_SHARED |
			CONNECTED_CONSUMER_IS_SHARED,
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
	static const char*			attribute_names[];
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

	good_bool
	declare_direction(const direction_type) const;

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
	error_count
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
		operator () (AliasType&);
	};	// end struct connection_flag_setter

	// only for non-directional channels?
	// forward_local_to_external_flags(...);

	bool
	has_nondefault_attributes(void) const {
		return direction_flags != DEFAULT_CONNECT_FLAGS;
	}

	ostream&
	dump_flat_attributes(ostream&) const;

	ostream&
	dump_attributes(ostream&) const;

	ostream&
	dump_explicit_attributes(ostream& o) const { return o; }

	void
	write_flags(ostream&) const;

	void
	read_flags(istream&);

};	// end struct channel_connect_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Mostly just redirects many calls to substructure, for recursion.
 */
class process_connect_policy
#if !PROCESS_CONNECTIVITY_CHECKING
	: public null_connect_policy
#endif
{
// parent type is temporary as we gradually implement this...
private:
	typedef	process_connect_policy		this_type;
public:
#if PROCESS_CONNECTIVITY_CHECKING
	enum direction_flags {
		/**
			Set if connected to a local port producer.  
			Mutually exclusive with 
			CONNECTED_TO_SUBSTRUCT_PRODUCER
			Setting this means either connected to port formal.
		 */
		CONNECTED_TO_LOCAL_PRODUCER = 0x01,
		/**
			Analogous to CONNECTED_TO_LOCAL_CONSUMER.
		 */
		CONNECTED_TO_LOCAL_CONSUMER = 0x10,
		/**
			Connection state inferred from hierarchy
			when substructure is instantiated, 
			propagated from formal to actual.  
		 */
		CONNECTED_TO_SUBSTRUCT_PRODUCER = 0x02,
		/**
			Analogous to CONNECTED_TO_SUBSTRUCT_PRODUCER.
		 */
		CONNECTED_TO_SUBSTRUCT_CONSUMER = 0x20,
#if 0
		/**
			These flags are set when the local
			process instance is connected (by PRS)
			to an end-consumer, or end-producer.
			For now, such an attribute needs to be
			explicitly declared, until it can be inferred.
		 */
		CONNECTED_PRS_PRODUCER = 0x04,
		CONNECTED_PRS_CONSUMER = 0x40,
#endif
		/// don't have any explicit attributes yet
		PROCESS_EXPLICIT_ATTRIBUTES_MASK = 0x0,
		// port is marked with '?'
		PROCESS_PORT_FORMAL_INPUT = 0x08,
		// port is marked with '!'
		PROCESS_PORT_FORMAL_OUTPUT = 0x80,
#if 1
		PROCESS_IS_ALIASED_TO_PORT = 0x0100,
#endif
#if 0
		/**
			Set if this channel is allowed to be connected
			to other shared producers.  
			Requires ALL participants
			in an alias set to be shared to be legal.  
		 */
		CONNECTED_PRODUCER_IS_SHARED = 0x02,
		/**
			Analogous to CONNECTED_CONSUMER_IS_SHARED.
		 */
		CONNECTED_CONSUMER_IS_SHARED = 0x20,
#endif
		// derived values
		CONNECTED_PORT_FORMAL_PRODUCER =
			PROCESS_PORT_FORMAL_INPUT,
		CONNECTED_PORT_FORMAL_CONSUMER =
			PROCESS_PORT_FORMAL_OUTPUT,
		CONNECTED_TO_NONPORT_PRODUCER = 
			CONNECTED_TO_LOCAL_PRODUCER |
			CONNECTED_TO_SUBSTRUCT_PRODUCER,
		CONNECTED_TO_NONPORT_CONSUMER = 
			CONNECTED_TO_LOCAL_CONSUMER |
			CONNECTED_TO_SUBSTRUCT_CONSUMER,
		CONNECTED_TO_ANY_PRODUCER = 
			CONNECTED_PORT_FORMAL_PRODUCER |
			CONNECTED_TO_NONPORT_PRODUCER,
		CONNECTED_TO_ANY_CONSUMER = 
			CONNECTED_PORT_FORMAL_CONSUMER |
			CONNECTED_TO_NONPORT_CONSUMER,

		PROCESS_IMPLICIT_ATTRIBUTES_MASK = 
			CONNECTED_TO_SUBSTRUCT_PRODUCER |
			CONNECTED_TO_SUBSTRUCT_CONSUMER,
	/// mask for attributes to distinguish from connectivity fields
	// both implicit and explicit attributes should be preserved
		PROCESS_ATTRIBUTES_MASK	=
			PROCESS_EXPLICIT_ATTRIBUTES_MASK |
			PROCESS_IMPLICIT_ATTRIBUTES_MASK,
		PROCESS_INIT_ATTRIBUTES_MASK	=
			PROCESS_ATTRIBUTES_MASK
#if 1
			| PROCESS_IS_ALIASED_TO_PORT
#endif
			,
		/// default value
		DEFAULT_CONNECT_FLAGS = 0x00,
		/// TODO: keep sync'd with above enums
		PROCESS_NUM_ATTRIBUTES = 9
	};
protected:
	static const char*			attribute_names[];
	connection_flags_type		direction_flags;
public:
	process_connect_policy() : direction_flags(DEFAULT_CONNECT_FLAGS) { }
#endif	// PROCESS_CONNECTIVITY_CHECKING
private:
#if !PROCESS_CONNECTIVITY_CHECKING
	using null_connect_policy::initialize_direction;
#endif
protected:

	template <class AliasType>
	void
	initialize_direction(AliasType&, const unroll_context&);

#if PROCESS_CONNECTIVITY_CHECKING
protected:
	template <class AliasType>
	static
	good_bool
	synchronize_flags(AliasType&, AliasType&);

	template <class AliasType>
	void
	initialize_actual_direction(const AliasType&);

public:
	good_bool
	set_connection_flags(const connection_flags_type);

	good_bool
	declare_direction(const direction_type);

	bool
	has_nondefault_attributes(void) const {
		return direction_flags != DEFAULT_CONNECT_FLAGS;
	}

	bool
	is_input_port(void) const {
		return direction_flags & PROCESS_PORT_FORMAL_INPUT;
	}

	bool
	is_output_port(void) const {
		return direction_flags & PROCESS_PORT_FORMAL_OUTPUT;
	}

	bool
	is_terminal_producer(void) const {
//		return direction_flags & CONNECTED_PRS_PRODUCER;
		return direction_flags & CONNECTED_TO_LOCAL_PRODUCER;
	}

	bool
	is_terminal_consumer(void) const {
//		return direction_flags & CONNECTED_PRS_CONSUMER;
		return direction_flags & CONNECTED_TO_LOCAL_CONSUMER;
	}

#if 1
	void
	flag_port(void) {
		direction_flags |= PROCESS_IS_ALIASED_TO_PORT;
	}

	bool
	is_aliased_to_port(void) const {
		return direction_flags & PROCESS_IS_ALIASED_TO_PORT;
	}
#endif

protected:
	template <class AliasType>
	void
	__update_flags(AliasType&);

	template <class AliasType>
	static
	error_count
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
		operator () (AliasType&);
	};	// end struct collection_connection_flag_setter

	ostream&
	dump_flat_attributes(ostream&) const;

	ostream&
	dump_attributes(ostream& o) const;

	ostream&
	dump_explicit_attributes(ostream& o) const { return o; }

protected:
	void
	write_flags(ostream&) const;

	void
	read_flags(istream&);

#endif	// PROCESS_CONNECTIVITY_CHECKING

};	// end struct process_connect_policy

//=============================================================================
}	// end namesace entity
}	// end namesace HAC

#endif	// __HAC_OBJECT_INST_CONNECTION_POLICY_H__


