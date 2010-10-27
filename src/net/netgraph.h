/**
	\file "net/netgraph.h"
	$Id: netgraph.h,v 1.20 2010/10/27 00:16:53 fang Exp $
 */

#ifndef	__HAC_NET_NETGRAPH_H__
#define	__HAC_NET_NETGRAPH_H__

#include <vector>
#include <string>
#include <map>
#include <set>
#include "net/common.h"
#include "Object/lang/cflat_context_visitor.h"
#include "Object/lang/PRS_footprint_expr.h"	// for precharge_ref_type
#include "Object/devel_switches.h"

/**
	Define to 1 to organize transistors by group, so that they
	may be associated with their output nodes or named internal nodes.
	Goal: 1
	Rationale: more consistent device naming across revisions (ECOs).
	Status: done, tested.
 */
#define	NETLIST_GROUPED_TRANSISTORS		1

/**
	Define to 1 to enable static connectivity checking for
	floating nodes.  Semi-redundant checking with PRS checking, 
	but is more detailed at the transistor level.
	Remember to 'inherit' the driven property hierarchically 
	from subcircuits to supercircuits.
	Don't bother checking multiply driven nodes.  
	Goal: 1
	Status: done, tested.
 */
#define	NETLIST_CHECK_CONNECTIVITY		1

/**
	Define to 1 to check for name collisions, due to mangling
	or case-insensitivity, or reserved names.
	TODO: name collision checking should eventually include
		instance names, etc...
	For now, just checks node names.
	Goal: 1
	Status: done, used for a while.
 */
#define	NETLIST_CHECK_NAME_COLLISIONS		1

namespace HAC {
namespace entity {
namespace PRS {
class footprint;
}
}
namespace NET {
using std::vector;
using std::string;
using std::map;
using std::set;
using std::pair;
using entity::footprint;
using entity::state_instance;
using entity::bool_tag;
using entity::process_tag;
using entity::PRS::footprint_expr_node;

typedef	size_t		index_type;
typedef	double		real_type;
class node;
class netlist;
class local_netlist;
class netlist_generator;
struct netlist_options;
/**
	Collection of physical nodes, can be from logical nodes, 
	internal nodes, or auxiliary nodes, or supply nodes.  
 */
typedef	vector<node>			node_pool_type;

// 0-indexed nodes or 1-indexed?
// extern const index_type	base_index = 1;

//=============================================================================
/**
	Standard 4-terminal device for transistor.
	Consider for attributes: pointing to original unrolled literal
		to reference attributes and parameters instead of copying
		to save memory at the expense of run-time.
 */
struct transistor {
	enum fet_type {
		NFET_TYPE,
		PFET_TYPE
	};
	/**
		Devices are somehow named after the rules from which
		they are derived.  
		TODO: need auto-enumeration
	 */
	string				name;
	index_type			source;
	index_type			gate;
	index_type			drain;
	/**
		Substrate contact.
	 */
	index_type			body;
#if NETLIST_GROUPED_TRANSISTORS
	/**
		The index of the node with which this device is
		associated, for the purposes of grouping.  
	 */
	index_type			assoc_node;
	/**
		Whether we are associated with a pull-up or pull-dn
		group of devices.  
		Precharges are associated with the direction
		of the primary rule.  
		Internal nodes...
	 */
	bool				assoc_dir;
#endif
	/// device width parameter
	real_type			width;
	/// device length parameter
	real_type			length;
	/// device type: nfet, pfet
	char				type;

	// allow a conf/tech file to define/enumerate additional types
	// attributes:
	// is_standard_keeper
	// is_combination_feedback_keeper
	enum flags {
		DEFAULT_ATTRIBUTE = 0x0,
		IS_PRECHARGE = 0x01,
		IS_STANDARD_KEEPER = 0x02,
		IS_COMB_FEEDBACK = 0x04,
		IS_PASS = 0x08,
		IS_LOW_VT = 0x10,
		IS_HIGH_VT = 0x20
	};
	typedef	char			attributes_type;
	attributes_type			attributes;

	void
	set_lvt(void) {
		attributes |= IS_LOW_VT;
		attributes &= ~IS_HIGH_VT;
	}

	void
	set_hvt(void) {
		attributes |= IS_HIGH_VT;
		attributes &= ~IS_LOW_VT;
	}

	void
	set_svt(void) {
		attributes &= ~(IS_LOW_VT | IS_HIGH_VT);
	}

	void
	set_pass(void) {
		attributes |= IS_PASS;
	}

	bool
	is_pass(void) const {
		return attributes & IS_PASS;
	}

	void
	mark_used_nodes(node_pool_type&) const;

	ostream&
	emit(ostream&, const index_type, const node_pool_type&, 
		const footprint&, const netlist_options&) const;

	ostream&
	emit_attribute_suffixes(ostream&, const netlist_options&) const;

	ostream&
	dump_raw(ostream&) const;

	static
	char
	opposite_FET_type(const char f) {
		return (f == NFET_TYPE) ? PFET_TYPE : NFET_TYPE;
	}

};	// end struct transistor

//-----------------------------------------------------------------------------
/**
	Group of transistors.
	TODO: indices of originating rule and/or internal node definition?
 */
class device_group {
public:
	typedef	vector<transistor>	transistor_pool_type;
protected:
#if 0
	/**
		This is the index of the rule or internal node 
		associated with this group.
	 */
	index_type			assoc_node_index;
#else
	// infer index from position in node pool
#endif
#if	NETLIST_CHECK_NAME_COLLISIONS
	typedef	set<string>		name_set_type;
	// to check for device name collisions
	name_set_type			names;
#endif
protected:
	/**
		Set of transistors that participate in driving
		this node, be it logical or named internal.  
		The associated node need not be a direct terminal of
		every transistor.  
	 */
	transistor_pool_type		transistor_pool;

public:
	bool
	is_empty(void) const;

#if 0
	ostream&
	emit_devices(ostream&, const index_type, const node_pool_type&, 
		const footprint&, const netlist_options&) const;
#endif
	void
	add_transistor(const transistor&);

	void
	mark_used_nodes(node_pool_type&) const;

	ostream&
	dump_raw_devices(ostream&) const;

};	// end struct device_group

//-----------------------------------------------------------------------------
/**
	Extension of (local) node information.  
	Corresponds to an electrical node in the netlist. 
	Bother with redundant connectivity list?
	Re: device grouping by output node:
	Decided not to derive/contain device group because 
	rules associated with a single node may come from different 
	subcircuit, thus making it difficult to group by node.  
	Thus, we resort to tagging every device with a node index, 
	and maintaining an auxiliary mutable counter.  
 */
struct node {
	struct __logical_node_tag { };
#if !PRS_SUPPLY_OVERRIDES
	struct __supply_node_tag { };
#endif
	struct __internal_node_tag { };
	struct __auxiliary_node_tag { };
#if !PRS_SUPPLY_OVERRIDES
	static const __supply_node_tag		supply_node_tag;
#endif
	static const __logical_node_tag		logical_node_tag;
	static const __internal_node_tag	internal_node_tag;
	static const __auxiliary_node_tag	auxiliary_node_tag;
	/**
		Index may correspond to index in local bool instance pool.
		For internal nodes, they correspond to the footprint's
		local internal index (pooled).
	 */
	index_type			index;
	/**
		Name of node.
		Named nodes inherit their names from canonical names
		in the original local definition, possibly mangled.  
		Need disambioguous ways of naming internal and 
		auxiliary nodes.
		Internal nodes just (optionally) copy this over
		from their footprint.  
	 */
	string				name;
	/**
		Logical nodes are those named in the original source.
		Internal nodes are special named nodes used to
		construct shared node networks.
		Supply nodes are implicit poewr supply nodes.
		Auxiliary nodes are those generated by subexpressions 
		(unnamed), and *cannot* cross subcircuit boundaries.  
	 */
	enum {
		NODE_TYPE_LOGICAL = 0,
		NODE_TYPE_INTERNAL = 1,
		NODE_TYPE_AUXILIARY = 2
#if PRS_SUPPLY_OVERRIDES
		// NODE_TYPE_LOGICAL also applies to supply nodes
#else
		, NODE_TYPE_SUPPLY = 3
#endif
	};
	// is_named -- if this was a named node in original source, 
	//	otherwise is internal, auxiliary node.
	char				type;
	/**
		set to true if node participates in any device 
		(i.e. has any electrical connectivity whatsoever), 
		so unused nodes (e.g. in ports) can be skipped.  
		Q: should only source and gate and body terminals count?
	 */
	bool				used;
#if NETLIST_CHECK_CONNECTIVITY
	/**
		set to true if node is connected to the drain
		of any transistors, i.e. is driven.
		Ports are allowed to be undriven inside a subcircuit.
	 */
	bool				driven;
#define	INIT_DRIVEN			, driven(false)
#else
#define	INIT_DRIVEN
#endif
	// connectivity information needed? would be redundant with devices
#if NETLIST_GROUPED_TRANSISTORS
	/**
		Auxiliary counter used when printing transistors and
		enumerating devices per associated node.  
		When recursing into subcircuits, save and clear the 
		counter value, and upon returning, restore former value.  
		Like class util::value_saver.
		Pull-up rule and pull-down rules are associated with
		separate counters.  
	 */
	mutable
	index_type			device_count[2];
#endif

#if NETLIST_GROUPED_TRANSISTORS
#define	INIT_DEVICE_COUNT	device_count[0] = 0; device_count[1] = 0;
#else
#define	INIT_DEVICE_COUNT
#endif

#if !PRS_SUPPLY_OVERRIDES
	node(const string& s, const __supply_node_tag&) : 
		index(0), name(s), type(NODE_TYPE_SUPPLY), used(false) 
		INIT_DRIVEN	// shouldn't supplies be considered driven?
		{ INIT_DEVICE_COUNT }
#endif
	node(const index_type i, const __logical_node_tag&) : 
		index(i), name(), type(NODE_TYPE_LOGICAL), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
	node(const index_type i, const __internal_node_tag&) : 
		index(i), name(), type(NODE_TYPE_INTERNAL), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
	node(const __auxiliary_node_tag&) : 
		index(0), name(), type(NODE_TYPE_AUXILIARY), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
	// only for VOID node
	node(const char* s, const __auxiliary_node_tag&) : 
		index(0), name(s), type(NODE_TYPE_AUXILIARY), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
	node(const index_type a, const __auxiliary_node_tag&) : 
		index(a), name(), type(NODE_TYPE_AUXILIARY), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
#undef	INIT_DEVICE_COUNT
#undef	INIT_DRIVEN

	bool
	is_logical_node(void) const { return type == NODE_TYPE_LOGICAL; }

	// named internal node
	bool
	is_internal_node(void) const { return type == NODE_TYPE_INTERNAL; }

	// true if ndoe is automatically generated
	bool
	is_auxiliary_node(void) const { return type == NODE_TYPE_AUXILIARY; }

	// at the end of a transistor stack
	bool
	is_stack_end_node(void) const {
		return is_logical_node() || is_internal_node();
	}
	bool
	is_stack_int_node(void) const {
		return !is_stack_end_node();
	}

	bool
	is_supply_node(void) const {
#if PRS_SUPPLY_OVERRIDES
		return false;
#else
		return type == NODE_TYPE_SUPPLY;
#endif
	}

#if NETLIST_CHECK_CONNECTIVITY
	error_status
	check_connectivity(const netlist_options&) const;
#endif

	ostream&
	emit(ostream&, const netlist_options&) const;

	ostream&
	dump_raw(ostream&) const;

};	// end struct node

//-----------------------------------------------------------------------------
/**
	Generic 2-terminal device.
	Capacitor, resistor, or inductor.
	All of these devices only take a single parameter.
 */
struct passive_device {
	/**
		Points to the two terminals.  
	 */
	index_type			t[2];
	// enum type: (R) resistor, (L) inductor, (C) capacitor
	// (D) diode should be separate as it can be tied to a model
	char				type;
	// name?
	real_type			parameter_value;
};	// end struct device

//-----------------------------------------------------------------------------
/**
	Instantiation of a particular substructure type.
	spice card: x
 */
struct instance {
	/**
		Reference to subcircuit type.
		Translated from looking up netlist_map_type in the generator
		class, below.
		Can point to either a local subcircuit or externally
		defined subcircuit (associated to defproc).
		Beware of storing insertion-invalidated pointers.
	 */
	const netlist*			type;
	/**
		Local process index.
	 */
	index_type			pid;
	/**
		Port connections.
	 */
	typedef	vector<index_type>	actuals_list_type;

	actuals_list_type		actuals;

	explicit
	instance(const netlist& t, const index_type p) :
		type(&t), pid(p), actuals() { }


	bool
	is_empty(void) const;

	ostream&
	emit(ostream&, const node_pool_type&, const footprint&, 
		const netlist_options&) const;

	void
	mark_used_nodes(node_pool_type&) const;

	ostream&
	dump_raw(ostream&) const;

};	// end struct instance

//-----------------------------------------------------------------------------
/**
	Structures common to all netlists.  
 */
struct netlist_common : public device_group {
	/**
		TODO: print comment with origin rule before each
		group of transistors.
	 */
	typedef	vector<passive_device>	passive_device_pool_type;
	passive_device_pool_type	passive_device_pool;
	// TODO: keep running count of C, L, R for auto-naming/enumeration?

	bool
	is_empty(void) const;

	ostream&
	emit_passive_devices(ostream&, const node_pool_type&, const footprint&, 
		const netlist_options&) const;

	ostream&
	emit_devices(ostream&, const node_pool_type&, const footprint&, 
		const netlist_options&) const;

};	// end class netlist_common

//-----------------------------------------------------------------------------
/**
	Minimum requirements for any type of netlist.  
	Intended for use with subcircuits.
 */
struct local_netlist : public netlist_common {
	// does not have own list of internal nodes, defer to parent's
	// keep a local subset of nodes used, indexing into parent's collection
	// will use this
	typedef	set<index_type>		node_index_map_type;

	// name of local subcircuit, or just suffix, like :subs_0
	string				name;
	/**
		Mapping from parent node index to local node index.  
		Expected to be a sparse subset of parent's list.  
		Every node in this list will be a port of the 
		subcircuit definition.  (Maybe eventually optimize?)
		key= parent domain node index
	 */
	node_index_map_type			node_index_map;

	// does NOT have local subinstances, only devices
	void
	mark_used_nodes(node_pool_type&);

	// requires a parent netlist for reference
	ostream&
	emit_definition(ostream&, const netlist& n,
		const netlist_options&) const;
	ostream&
	emit_instance(ostream&, const netlist& n,
		const netlist_options&) const;

	ostream&
	dump_raw(ostream&, const netlist&) const;
};	// end class local_netlist

//-----------------------------------------------------------------------------
/**
	A graph.
	One netlist corresponds to a subcircuit, or in HAC a unique footprint.  
	TODO: support for special global nodes like Vdd, GND?
		can be automatic or overridden.
	Beware of using vectors and accidentally invalidating references
	and iterators after tail-insertion, which may cause reallocation.  
 */
class netlist : public netlist_common {
	typedef	entity::PRS::footprint	prs_footprint;
	/**
		Keep around footprint for node reference and printing.
	 */
	const footprint*		fp;
public:
// universal node indices to every subcircuit
	static const node void_node;	// not a real node
#if !PRS_SUPPLY_OVERRIDES
	static const node GND_node;
	static const node Vdd_node;
#endif

	// these should correspond with the order of insertion in netlist's ctor
	static const	index_type	void_index;
#if !PRS_SUPPLY_OVERRIDES
	static const	index_type	GND_index;
	static const	index_type	Vdd_index;
#endif

friend class local_netlist;
friend class netlist_generator;
	/**
		first: local node index
		second: owner subcircuit index (1-indexed)
	 */
	typedef	pair<index_type, index_type> internal_node_entry_type;
	/**
		Maps the local footprint's internal node to 
		the netlist's generic node index.  
		index: footprint's internal node index
	 */
	typedef	vector<internal_node_entry_type>
					internal_node_map_type;
	/**
		Reverse-map for looking up by expression id.  
		key = prs footprint's local expression index
		value = footprint-index of internal node
	 */
	typedef	map<index_type, index_type>
					internal_expr_map_type;
	/**
		index= local footprint index of node
		value= netlist index of node
		Information is somewhat redundant, but provides
		a reverse map.  
		0-value means node has not been used yet.  
	 */
	typedef	vector<index_type>	named_node_map_type;
#if NETLIST_CHECK_NAME_COLLISIONS
	/**
		Reverse map from mangled name to node index.  
		key: mangled string name that would be emitted
		value: node index
		Eventually need to extend to check names of 
		other entities, instance names, etc...
	 */
	typedef	map<string, index_type>	name_collision_map_type;
#endif
	/**
		For local subcircuits only.  
		Such local circuits are not shared outside of this definition
		and are only instantiated once.  
		Not using map because subcircuits may be anonymous. 
	 */
	typedef	vector<local_netlist>	local_subcircuit_list_type;
	typedef	vector<instance>	instance_pool_type;
	/**
		Indices refer to local nodes which need not be the 
		exact same set of local port aliases.
		May include Vdd, GND.
		Internal nodes cannot be ports.  
	 */
	typedef	vector<index_type>	port_list_type;
private:
	/**
		Name can be inferred from the hierarchical name 
		of the footprint.  Will have to be mangled 
		so we save the result.  
	 */
	string				name;
	named_node_map_type		named_node_map;
	/**
		All local nodes, including ports and internal nodes.
		This is only stored in the primary netlist, 
		whereas the local subcircuit netlists use indirect
		references to parents' nodes.  
	 */
	node_pool_type			node_pool;
	instance_pool_type		instance_pool;
	internal_node_map_type		internal_node_map;
	internal_expr_map_type		internal_expr_map;
#if NETLIST_CHECK_NAME_COLLISIONS
	// map for node names
	name_collision_map_type		name_collision_map;
	// separate map for instance names?
#endif
	local_subcircuit_list_type	local_subcircuits;
	/**
		List of local node indices.  
		NOTE: there cannot be any aliases in the ports.  
		Does not include GND and Vdd, which are handled separately.
	 */
	port_list_type			port_list;
	/**
		If this definition is empty, don't bother emitting it
		or instances of it.  
		This is also empty if all instances are also empty.  
		This is set by summarize_ports(), which should be the
		last method called during netlist processing.  
	 */
	bool				empty;
	/**
		Running count of generated auxiliary nodes.
	 */
	index_type			aux_count;
	/**
		Running count of unnamed subcircuits.
		This is only used as a local variable, can toss it...
	 */
	index_type			subs_count;
public:
	/**
		Warning count.
	 */
	size_t				warning_count;
public:
	netlist();
	~netlist();

	void
	bind_footprint(const footprint&, const netlist_options&);

	void
	bind_footprint(const footprint&, const string&);

	const string&
	get_name(void) const { return name; }

	string
	get_unmangled_name(void) const;

	bool
	is_empty(void) const { return empty; }

	index_type
	create_auxiliary_node(void);

	index_type
	create_internal_node(const index_type ni, const index_type ei, 
		const netlist_options& opt);

	index_type
	lookup_internal_node(const index_type i) const;

	index_type
	register_named_node(const index_type, const netlist_options&);

#if 0
	bool
	named_node_is_used(const index_type) const;
#endif

	void
	append_instance(const state_instance<process_tag>&, const netlist&, 
		const index_type, const netlist_options&);

	void
	summarize_ports(const netlist_options&);

#if NETLIST_CHECK_CONNECTIVITY
	error_status
	check_node_connectivity(const netlist_options&) const;
#endif

	ostream&
	emit(ostream&, const bool s, const netlist_options&) const;

	ostream&
	dump_raw(ostream&) const;

	ostream&
	emit_mangle_map(ostream&, const netlist_options&) const;

private:
	void
	mark_used_nodes(void);

	void
	__bind_footprint(const footprint&, const netlist_options&);

	string
	get_original_node_name(const size_t, const netlist_options&) const;

#if NETLIST_CHECK_NAME_COLLISIONS
	void
	check_name_collisions(const string&, const index_type, 
		const netlist_options&);
#endif

};	// end class netlist

//=============================================================================

}	// end namespace NET
}	// end namespace HAC

#endif	// __HAC_NET_NETGRAPH_H__

