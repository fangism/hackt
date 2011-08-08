/**
	\file "net/netgraph.h"
	Basic strutures for netlist.
	$Id: netgraph.h,v 1.29 2011/04/29 18:37:09 fang Exp $
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
	Define to 0 to reorder rule processing so that
	subcircuits are completed one at a time, 
	and not interleaved w.r.t. macros, etc.
	The old way (1) has transistor creation interleaved.
	Goal: 0
	Rationale: needed for transistor_index_offset coherence,
		whereby one subcircuit is finished at a time, so that
		devices in the same subcircuit are enumerated
		contiguously.
 */
#define	NETLIST_INTERLEAVE_SUBCKT_RULES		0

/**
	Define to 1 to precompute transistors' node assoc id.
	Goal: 1
	Rationale: so that assoc_uid is not computed while 
		dumping output, eww...
 */
#define	NETLIST_CACHE_ASSOC_UID			0

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

/**
	Define to 1 to support experimental verilog export mode.
	This will have structured ports in the netlist.
	Unsupported: template parameters.
	Goal: 1
	Status: complete, minimally tested.
 */
#define	NETLIST_VERILOG				1

/**
	Define to 1 to store computed parasitic values for each 
	devices, instead of just computing on-demand when printing.
	Costs a little more memory to keep around, but saves from recomputing.
	Goal: 1
 */
#define NETLIST_CACHE_PARASITICS		1

/**
	Define to 1 if local subcircuits get their own copy of nodes
		in a private node pool.  Used for calculating parasitics
		in local subcircuits.  
	Rationale: make local_netlist behave more similarly to netlist.
	Goal: 1
	Status: done, tested, should be perm'd
 */
#define	NETLIST_COMMON_NODE_POOL		1

/**
	Define to 1 to accumulate capacitances on nodes.
	Capacitance components: 
		diffusion area, diffusion perimeter, gate area.  
	Rationale: load estimation.
	Goal: 1
	Status: done, decently tested.
 */
#define	NETLIST_NODE_CAPS		(1 && NETLIST_CACHE_PARASITICS && NETLIST_COMMON_NODE_POOL)

/**
	Define to 1 to enable construction of graph using 
	node adjacency lists, where each node tracks the set of
	terminals to other components.  
	This information is redundant with transister/device netlists,
	and is really only needed for algorithms that do graph traversals.
	The cost of this is memory.
	Rationale: needed for prsim precharge invariant checking.
	Goal: 1
 */
#define	NETLIST_NODE_GRAPH			1

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
#if NETLIST_VERILOG
class proc;
#endif
class netlist;
class local_netlist;
class netlist_generator;
struct netlist_options;
/**
	Collection of physical nodes, can be from logical nodes, 
	internal nodes, or auxiliary nodes, or supply nodes.  
 */
typedef	vector<node>			node_pool_type;
#if NETLIST_VERILOG
typedef	vector<proc>			proc_pool_type;
#endif

// 0-indexed nodes or 1-indexed?
// extern const index_type	base_index = 1;

/**
	Primary structure for maintaining prerequisite ordering
	of emitting dependent subcircuits before they are used.
	Don't actually know if spice requires subcircuits
	to be defined before used, but this is safe.
	This way, only circuits that are used are emitted.
 */
typedef	std::map<const footprint*, netlist>		netlist_map_type;

//=============================================================================
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		Cache of parasitic area/perimeter values.
	 */
	struct parasitics {
		real_type			source_area;
		real_type			source_perimeter;
		real_type			drain_area;
		real_type			drain_perimeter;

		parasitics() :
			source_area(0.0), source_perimeter(0.0),
			drain_area(0.0), drain_perimeter(0.0) { }

		parasitics(const real_type w, const real_type l,
			const bool s_ext, const bool d_ext,
			const netlist_options&);

		void
		__update(const real_type w, const real_type l,
			const bool s_ext, const bool d_ext,
			const netlist_options&);

		void
		update(const transistor&,
			const bool s_ext, const bool d_ext,
			const netlist_options&);

	};	// end struct parasitics

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
#if NETLIST_CACHE_ASSOC_UID
	/**
		unique id assigned within group belonging to same assoc node
		This assigned upon transistor instantiation.
	 */
	index_type			assoc_uid;
#endif
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

#if NETLIST_CACHE_PARASITICS
	parasitics			parasitic_values;
#endif

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

	real_type
	gate_area(void) const { return width *length; }

	void
	mark_used_nodes(node_pool_type&) const;

#if NETLIST_NODE_GRAPH
	void
	mark_node_terminals(node_pool_type&, const size_t) const;
#endif

	ostream&
	emit_identifier(ostream&, const index_type, const node_pool_type&, 
		const netlist_options&) const;

	ostream&
	emit(ostream&, const index_type, const node_pool_type&, 
		const netlist_options&) const;

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

	size_t
	transistor_count(void) const { return transistor_pool.size(); }

	const transistor&
	get_transistor(const size_t i) const { return transistor_pool[i]; }

#if 0
	ostream&
	emit_devices(ostream&, const index_type, const node_pool_type&, 
		const footprint&, const netlist_options&) const;
#endif
	void
	add_transistor(const transistor&);

	void
	mark_used_nodes(node_pool_type&) const;

#if NETLIST_CACHE_PARASITICS
	void
	summarize_parasitics(node_pool_type&, const netlist_options&);
#endif

	ostream&
	dump_raw_devices(ostream&) const;

};	// end struct device_group

//-----------------------------------------------------------------------------
/**
	Information common to unique instance objects.  
 */
struct unique_common {
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

	unique_common() : index(0), name() { }

	explicit
	unique_common(const index_type i) : index(i), name() { }

	unique_common(const index_type i, const string& n) :
		index(i), name(n) { }

	explicit
	unique_common(const string& n) :
		index(0), name(n) { }

};	// end struct unique_common

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_VERILOG
/**
	An instance of channel or data-struct that is passed around 
	through ports.  
 */
struct proc : public unique_common {
	const netlist*			type;
	bool				used;

	explicit
	proc(const index_type i, const netlist* t) :
		unique_common(i), type(t), used(false) { }

	ostream&
	emit(ostream&) const;

	ostream&
	dump_raw(ostream&) const;

};	// end struct proc
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_NODE_CAPS
/**
	Per node accumulated information, grabbed from device terminals.
 */
struct node_caps {
	/**
		total perimeter value of connected sources and drains (NFET)
	 */
	real_type			ndiff_perimeter;
	/**
		total area value of connected sources and drains (NFET)
	 */
	real_type			ndiff_area;
	/**
		total perimeter value of connected sources and drains (PFET)
	 */
	real_type			pdiff_perimeter;
	/**
		total area value of connected sources and drains (PFET)
	 */
	real_type			pdiff_area;
	/**
		total area value of connected gates
	 */
	real_type			gate_area;
	/**
		total wire area, typically only comes from post-extraction
		and back-annotation, or may be estimated with some model.
	 */
	real_type			wire_area;
	/**
		Number of terminals at NFET drains and sources.
	 */
	size_t				ndrain_terms;
	/**
		Number of terminals at PFET drains and sources.
	 */
	size_t				pdrain_terms;
	/**
		Number of gate-connected terminals.  
	 */
	size_t				gate_terms;

	node_caps() :
		ndiff_perimeter(0.0),
		ndiff_area(0.0),
		pdiff_perimeter(0.0),
		pdiff_area(0.0),
		gate_area(0.0),
		wire_area(0.0),
		ndrain_terms(0),
		pdrain_terms(0),
		gate_terms(0) { }

	node_caps&
	operator += (const node_caps& r) {
		ndiff_perimeter += r.ndiff_perimeter;
		ndiff_area += r.ndiff_area;
		pdiff_perimeter += r.pdiff_perimeter;
		pdiff_area += r.pdiff_area;
		gate_area += r.gate_area;
		wire_area += r.wire_area;
		ndrain_terms += r.ndrain_terms;
		pdrain_terms += r.pdrain_terms;
		gate_terms += r.gate_terms;
		return *this;
	}

	node_caps
	operator + (const node_caps& r) const {
		node_caps ret(*this);
		ret += r;
		return ret;
	}

	ostream&
	emit(ostream&, const netlist_options&) const;

};	// end struct node_caps
#endif	// NETLIST_NODE_CAPS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if NETLIST_NODE_GRAPH
/**
	Adjacency list element.
	Refers to device as an edge, not your conventional 
	reference to node on the other end of an edge,
	because devices may have more than two terminals.
 */
struct node_terminal {
	// index into device type pool
	size_t					index;
	/**
		X: index position
		M: 's' 'd' 'g' 'b'
		R,L,D: 0,1
	 */
	size_t					port;
	/// R,L,C,D,M,X
	char					device_type;

	node_terminal(const char t, const size_t i, const size_t p) :
		index(i), port(p), device_type(t) { }

};	// end struct node_terminal
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
struct node : public unique_common {
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
		Logical nodes are those named in the original source.
		Internal nodes are special named nodes used to
		construct shared node networks.
		Supply nodes are implicit power supply nodes.
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
#if NETLIST_NODE_CAPS
	node_caps			cap;
#endif
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
#if NETLIST_NODE_GRAPH
	// connectivity information needed? would be redundant with devices
	/**
		List of all devices this node touches and to
		which terminal it is connected.
	 */
	vector<node_terminal>		terminals;
#endif	// NETLIST_NODE_GRAPH
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
		unique_common(s), 
		type(NODE_TYPE_SUPPLY), used(false) 
		INIT_DRIVEN	// shouldn't supplies be considered driven?
		{ INIT_DEVICE_COUNT }
#endif
	node(const index_type i, const __logical_node_tag&) : 
		unique_common(i),
		type(NODE_TYPE_LOGICAL), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
	node(const index_type i, const __internal_node_tag&) : 
		unique_common(i),
		type(NODE_TYPE_INTERNAL), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
	node(const __auxiliary_node_tag&) : 
		unique_common(), 
		type(NODE_TYPE_AUXILIARY), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
	// only for VOID node
	node(const char* s, const __auxiliary_node_tag&) : 
		unique_common(s),
		type(NODE_TYPE_AUXILIARY), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
	node(const index_type a, const __auxiliary_node_tag&) : 
		unique_common(a),
		type(NODE_TYPE_AUXILIARY), used(false)
		INIT_DRIVEN
		{ INIT_DEVICE_COUNT }
#undef	INIT_DEVICE_COUNT
#undef	INIT_DRIVEN

	// default copy-ctor

public:
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

#if NETLIST_NODE_CAPS
	static
	ostream&
	emit_node_caps(ostream&, const node_pool_type&,
		const netlist_options&);
#endif
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
		Port node connections.
	 */
	typedef	vector<index_type>	node_actuals_list_type;
#if NETLIST_VERILOG
	/**
		Port structure connections.
	 */
	typedef	vector<index_type>	proc_actuals_list_type;
#endif

	node_actuals_list_type		node_actuals;
#if NETLIST_VERILOG
	proc_actuals_list_type		proc_actuals;
#endif

	explicit
	instance(const netlist& t, const index_type p) :
		type(&t), pid(p), node_actuals()
#if NETLIST_VERILOG
		, proc_actuals()
#endif
		{ }


	bool
	is_empty(void) const;

	string
	raw_identifier(const footprint&,
		const netlist_options&) const;

	ostream&
	emit(ostream&, const node_pool_type&,
#if NETLIST_VERILOG
		const proc_pool_type&,
#endif
		const footprint&, const netlist_options&) const;

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
#if NETLIST_COMMON_NODE_POOL
	/**
		All local nodes, including ports and internal nodes.
	 */
	node_pool_type			node_pool;
#endif
	/**
		TODO: print comment with origin rule before each
		group of transistors.
	 */
	typedef	vector<passive_device>	passive_device_pool_type;
	passive_device_pool_type	passive_device_pool;
	// TODO: keep running count of C, L, R for auto-naming/enumeration?

	netlist_common();
	~netlist_common();

	bool
	is_empty(void) const;

	ostream&
	emit_passive_devices(ostream&,
#if !NETLIST_COMMON_NODE_POOL
		const node_pool_type&,
#endif
		const netlist_options&) const;

	ostream&
	emit_devices(ostream&,
#if !NETLIST_COMMON_NODE_POOL
		const node_pool_type&,
#endif
		const netlist_options&) const;

};	// end class netlist_common

//-----------------------------------------------------------------------------
/**
	Minimum requirements for any type of netlist.  
	Intended for use with subcircuits.
 */
struct local_netlist : public netlist_common {
#if NETLIST_COMMON_NODE_POOL
	// maps actual indices to formal indices (ordered!)
	typedef	map<index_type, index_type>		node_index_map_type;
#else
	// does not have own list of internal nodes, defer to parent's
	// keep a local subset of nodes used, indexing into parent's collection
	// will use this
	typedef	set<index_type>		node_index_map_type;
#endif

	// name of local subcircuit, or just suffix, like :subs_0
	string				name;

	/**
		Mapping from parent node index to local node index.  
		Expected to be a sparse subset of parent's list.  
		Every node in this list will be a port of the 
		subcircuit definition.  (Maybe eventually optimize?)
		key= parent domain node index (actual)
		value= local compacted pool index
	 */
	node_index_map_type			node_index_map;

	/**
		Running total of transistor count, so that every transistor
		in a netlist, including local subcircuits, can be enumerated
		and mapped to a unique index.
		Lookups can be done efficiently using binary_search, 
		lower_bound, etc.
	 */
	size_t				transistor_index_offset;

public:
	local_netlist();
	~local_netlist();

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
	dump_raw(ostream&
#if !NETLIST_COMMON_NODE_POOL
		, const netlist&
#endif
		) const;

#if NETLIST_CACHE_PARASITICS
	void
	summarize_parasitics(const netlist_options&);
#endif
};	// end class local_netlist

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is the result of a transistor index lookup.
	first: local subcircuit index (1-based), 0 for main subcircuit
	second: offset within subcircuit
 */
typedef pair<size_t, size_t>		transistor_reference;

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
#if NETLIST_VERILOG
	static const proc void_proc;
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
#if NETLIST_VERILOG
	typedef	vector<index_type>	named_proc_map_type;
#endif
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
	typedef	vector<index_type>	node_port_list_type;
#if NETLIST_VERILOG
	/**
		Local indices of processes (channels) for structured ports.
	 */
	typedef	vector<index_type>	proc_port_list_type;
#endif
private:
	/**
		Name can be inferred from the hierarchical name 
		of the footprint.  Will have to be mangled 
		so we save the result.  
	 */
	string				name;
	named_node_map_type		named_node_map;
#if NETLIST_VERILOG
	named_proc_map_type		named_proc_map;
#endif
#if NETLIST_COMMON_NODE_POOL
	// inherit node_pool from netlist_common
#else
	/**
		All local nodes, including ports and internal nodes.
		This is only stored in the primary netlist, 
		whereas the local subcircuit netlists use indirect
		references to parents' nodes.  
	 */
	node_pool_type			node_pool;
#endif
#if NETLIST_VERILOG
	/**
		Collection of channels and datastructs.  
	 */
	proc_pool_type			proc_pool;
#endif
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
	node_port_list_type		node_port_list;
#if NETLIST_VERILOG
	/**
		List of local process indices.  
	 */
	proc_port_list_type		proc_port_list;
#endif
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

	bool
	is_channel_or_struct(void) const;

	index_type
	create_auxiliary_node(void);

	index_type
	create_internal_node(const index_type ni, const index_type ei, 
		const netlist_options& opt);

	index_type
	lookup_internal_node(const index_type i) const;

	index_type
	register_named_node(const index_type, const netlist_options&);

#if NETLIST_VERILOG
	index_type
	register_named_proc(const index_type, const netlist*, 
		const netlist_options&);
#endif

#if 0
	bool
	named_node_is_used(const index_type) const;
#endif

	transistor_reference
	lookup_transistor_index(const size_t) const;

	const transistor&
	lookup_transistor(const transistor_reference&) const;

	const transistor&
	lookup_transistor(const size_t) const;

	void
	append_instance(const state_instance<process_tag>&, const netlist&, 
		const index_type, 
#if NETLIST_VERILOG
		const netlist_map_type&,
#endif
		const netlist_options&);

	void
	summarize_ports(
#if NETLIST_VERILOG
		const netlist_map_type&,
#endif
		const netlist_options&);

#if NETLIST_CACHE_PARASITICS
	void
	summarize_parasitics(const netlist_options&);
#endif

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

	template <class Tag>
	string
	get_original_node_name(const size_t, const netlist_options&) const;

#if NETLIST_CHECK_NAME_COLLISIONS
	void
	check_name_collisions(const string&, const index_type, 
		const netlist_options&);
#endif

	void
	collect_node_ports(vector<string>&, const netlist_options&) const;

	ostream&
	emit_subinstances(ostream&, const netlist_options&) const;

	ostream&
	emit_local_subcircuits(ostream&, const netlist_options&) const;

#if NETLIST_VERILOG
	void
	collect_struct_ports(vector<string>&, const netlist_options&) const;

	ostream&
	emit_verilog_locals(ostream&, const netlist_options&) const;
#endif

	ostream&
	emit_header(ostream&, const netlist_options&) const;

#if NETLIST_NODE_CAPS
	ostream&
	emit_node_caps(ostream&, const netlist_options&) const;
#endif

#if NETLIST_NODE_GRAPH
	ostream&
	emit_node_terminal_graph(ostream&, const netlist_options&) const;
#endif

};	// end class netlist

//=============================================================================

}	// end namespace NET
}	// end namespace HAC

#endif	// __HAC_NET_NETGRAPH_H__

