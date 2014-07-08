/**
	\file "sim/prsim/process_graph.hh"
	$Id: process_graph.hh,v 1.6 2010/09/29 00:13:43 fang Exp $
	Unique process subgraph structure, shared by all process instances
	of the same type.
 */

#ifndef	__HAC_SIM_PRSIM_PROCESS_GRAPH_HH__
#define	__HAC_SIM_PRSIM_PROCESS_GRAPH_HH__

#include <iosfwd>
#include <map>
#include <set>
#include <valarray>

#include "util/STL/hash_map.hh"
#include "sim/common.hh"
#include "sim/prsim/devel_switches.hh"
#include "sim/prsim/Rule.hh"
#include "sim/prsim/Expr.hh"
#include "Object/common/frame_map.hh"

namespace HAC {
namespace entity {
	class footprint;
}
namespace SIM {
namespace PRSIM {
class ExprAlloc;
struct ExprAllocFlags;
using std::map;
using std::set;
using HASH_MAP_NAMESPACE::hash_map;
using std::valarray;
using entity::footprint;
struct process_sim_state;
using entity::footprint_frame_map_type;


/**
	Define to 1 to maintain a separate expr->process id map.
	TODO: benchmark the different between performance
	on large hierarchical designs.  
	Whe the map is combined search may need to be modified
	to use lower_bound rather than --upper_bound
	Goal: 0
	Status: broken
 */
#define	PRSIM_SEPARATE_PROCESS_EXPR_MAP		1

//=============================================================================
// structures to account for local fanin contribution
typedef	vector<expr_index_type>		fanin_array_type;
typedef	fanin_array_type		fanout_array_type;

/**
	This resembles Node(Struct), but only contains 
	fanin and fanout information.  
 */
struct faninout_struct_type {
#if PRSIM_WEAK_RULES
	fanin_array_type		pull_up[2];
	fanin_array_type		pull_dn[2];
#else
	fanin_array_type		pull_up;
	fanin_array_type		pull_dn;
#endif
	fanout_array_type		fanout;

/**
	if needed, extend this to a bit fields like Node::struct_flags_enum
	not all of those fields are important.
	is_atomic should be consistent with Node::is_atomic.
	This is set during the ExprAlloc pass.
	We maintain this information here to avoid having to lookup
	Node information from the global state array.
 */
	bool				is_atomic;

	struct counter;			// counter

	faninout_struct_type() : is_atomic(false) { }
	// default dtor/copy

	void
	import_attributes(void);

	fanin_array_type&
	get_pull_expr(const bool b
#if PRSIM_WEAK_RULES
		, const rule_strength w
#endif
		) {
		return b ? pull_up STR_INDEX(w)
			: pull_dn STR_INDEX(w);
	}

	const fanin_array_type&
	get_pull_expr(const bool b
#if PRSIM_WEAK_RULES
		, const rule_strength w
#endif
		) const {
		return b ? pull_up STR_INDEX(w)
			: pull_dn STR_INDEX(w);
	}

	bool
	has_fanin(void) const;

	bool
	contains_fanout(const expr_index_type) const;

	size_t
	fans(void) const {
		return fanout.size()
#if PRSIM_WEAK_RULES
			+pull_up[NORMAL_RULE].size()
			+pull_up[WEAK_RULE].size()
			+pull_dn[NORMAL_RULE].size()
			+pull_dn[WEAK_RULE].size();
#else
			+pull_up.size() +pull_dn.size();
#endif
	}

	static
	ostream&
	dump_faninout_list(ostream&, const fanin_array_type&);

	ostream&
	dump_struct(ostream&) const;

	static
	expr_index_type
	add_size(const expr_index_type N, const faninout_struct_type& f) {
		return N +f.fans();
	}

};	// end struct faninout_struct_type

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_SETUP_HOLD || PRSIM_TIMING_BACKANNOTATE
struct timing_constraint_entry {
	/**
		Process-local index of the reference node of this constraint.  
	 */
#if PRSIM_FWD_POST_TIMING_CHECKS
	node_index_type				trig_node;
#else
	node_index_type				ref_node;
#endif
	rule_time_type				time;

	timing_constraint_entry() :
#if PRSIM_FWD_POST_TIMING_CHECKS
		trig_node(INVALID_NODE_INDEX),
#else
		ref_node(INVALID_NODE_INDEX),
#endif
		time(0) { }
	timing_constraint_entry(const node_index_type n,
		const rule_time_type t) :
#if PRSIM_FWD_POST_TIMING_CHECKS
		trig_node(n),
#else
		ref_node(n),
#endif
		time(t) { }
};	// end struct timing_constraing

#if PRSIM_FWD_POST_TIMING_CHECKS
typedef	timing_constraint_entry			hold_constraint_entry;
struct setup_constraint_entry : public timing_constraint_entry {
	bool					dir;
};	// end struct setup_constraint_entry
#else
typedef	timing_constraint_entry			setup_constraint_entry;

struct hold_constraint_entry : public timing_constraint_entry {
	bool					dir;
};	// end struct hold_constraint_entry
#endif

#endif
#if PRSIM_TIMING_BACKANNOTATE
struct min_delay_entry : public timing_constraint_entry	{
	// if !null, predicate refers to bool/ebool that must be true
	// for this min-delay constraint to apply.
	// a null-predicate implies true; always applied
	node_index_type				predicate;
};	// end struct min_delay_entry
#endif

//=============================================================================
/**
	shared structures used per process type
	This is where we save memory!
	This extends information that would normally go into
		a process's footprint. 
	TODO: eventually, PRSIM_UNIFY_GRAPH_STRUCTURES to avoid 
		unnecessary data replication, structure from create/unrolling.
	No stateful information should be kept here.  
	TODO: rings for mk_excl and check_excl!
	TODO: global_entry_context-like hierarchical mapping of exprs!
		using local private mapping.
 */
struct unique_process_subgraph {
	typedef	Expr				expr_struct_type;
	typedef	Rule<rule_time_type>		rule_type;
	typedef	ExprGraphNode			graph_node_type;
	/**
		Collection of all subexpressions.  
		These expressions, unlike those in the footprint,
		point and propagate from leaf to root, bottom up.  
		Indices are *local* to process (type)!
	 */
	typedef	vector<expr_struct_type>	expr_pool_type;
	/**
		Top-down graph structure.  
		Indices are *local* to process (type)!
		Cannot use list_vector, which is not copy-constructible (yet).
	 */
	typedef	vector<graph_node_type>		expr_graph_node_pool_type;
	/**
		Collection of rule static information, attributes.  
		Indices are *local* to process (type).
	 */
	typedef	vector<rule_type>		rule_pool_type;
	/**
		Sparse map from top-level expressions to rules.
		Can probably use ordered map.  
		TODO: even better, since both keys increase monotonically, 
		just binary search through a vector of pairs, 
		which can support reverse-lookups.
		ALERT: the trick is to keep it sorted by both keys
		in the face of pool-compaction optimizations...
	 */
	typedef	hash_map<expr_index_type, rule_index_type>
						rule_map_type;
	/**
		key: graph's local expression index
		value: source-footprint's invariant index (pool)
	 */
	typedef	map<expr_index_type, size_t>	invariant_map_type;
#if PRSIM_HIERARCHICAL_RINGS
	/**
		Instead of using circular linked lists with pointers, 
		we use a map of (cyclic referenced) indices to represent
		the exclusive rings. 
		Requires half as much memory as equivalent ring of pointers.  
		Another possiblity: fold these next-indices into the 
			Node structure.  
		Alternative: use map for sparser exclusive rings.  
	 */
	typedef	set<node_index_type>		ring_set_type;
#endif
	/**
		Structure for passing around set of node indices.
	 */
	typedef	set<node_index_type>		node_set_type;

	/**
		For convenience of lookup, this points to the original
		footprint from which this process graph was constructed.
	 */
	const entity::footprint*		_footprint;

	expr_pool_type				expr_pool;
	expr_graph_node_pool_type		expr_graph_node_pool;
	rule_pool_type				rule_pool;
	rule_map_type				rule_map;
	invariant_map_type			invariant_map;

	/**
		Member functions interpret this as a node for 
		structural purposes.  
	 */
	typedef	faninout_struct_type			node_type;
	/**
		indexed by local node index (1-based).
		Now subtract 1, for 0-based indexing!
	 */
	typedef	std::vector<faninout_struct_type>	faninout_map_type;
	/**
		This array-size should match number of nodes in unique_process.
		This sort of functions as a local node_pool.
	 */
	faninout_map_type			local_faninout_map;

#if PRSIM_SETUP_HOLD
	/**
		key: local target trigger node index
		value: sequence of local reference nodes and times
		(for PRSIM_FWD_POST_TIMING_CHECKS,
			key is the reference node index,
			and value is the sequence of triggers)
		constraints are: (reference, trigger-target)
		Any time a target node is queried, we will want
		all of its constraints at once.
	 */
#if PRSIM_FWD_POST_TIMING_CHECKS
	typedef	node_index_type			setup_constraint_key_type;
	typedef	pair<node_index_type, bool>	hold_constraint_key_type;
#else
	typedef	pair<node_index_type, bool>	setup_constraint_key_type;
	typedef	node_index_type			hold_constraint_key_type;
#endif
	typedef	map<setup_constraint_key_type, vector<setup_constraint_entry> >
						setup_constraint_set_type;
	typedef	map<hold_constraint_key_type, vector<hold_constraint_entry> >
						hold_constraint_set_type;
	setup_constraint_set_type		setup_constraints;
	hold_constraint_set_type		hold_constraints;
#endif
#if PRSIM_TIMING_BACKANNOTATE
	// initially do just a simple any-to-any direction min delay
	// TODO: account for rise/fall/unate direction
	typedef	node_index_type			min_delay_key_type;
	/**
		key: target node
		value: set of local timing (min-delay) arcs,
			relative to reference nodes
		When target node events are scheduled, must consider the
			max of +min_delay times over all reference nodes.
	 */
	typedef	map<min_delay_key_type, vector<min_delay_entry> >
						min_delay_set_type;
	/**
		Q: checkpoint?
	 */
	min_delay_set_type			min_delays;
#endif

	struct memory_accumulator;

	explicit
	unique_process_subgraph(const entity::footprint* = NULL);
	~unique_process_subgraph();

	node_index_type
	local_root_expr(expr_index_type) const;

	void
	void_expr(const expr_index_type);

	void
	check_expr(const expr_index_type) const;

	void
	check_node(const node_index_type) const;

	void
	check_structure(void) const;

	expr_index_type
	fan_count(void) const;

	const rule_type*
	lookup_rule(const expr_index_type) const;

	bool
	is_rule_expr(const expr_index_type) const;

	// are there local PRS that drive nodes?
	void
	has_local_fanin_map(vector<bool>&) const;

	void
	has_not_local_fanin_map(vector<bool>&) const;

	void
	rules_matching_fanin(const node_index_type, const bool,
		vector<rule_index_type>&) const;

	void
	rules_matching_fanout(const node_index_type,
		vector<rule_index_type>&) const;

	void
	rules_matching_faninout(const node_index_type, 
		const node_index_type, const bool, 
		vector<rule_index_type>&) const;

#if PRSIM_TIMING_BACKANNOTATE
	void
	add_min_delay_constraint(const node_index_type ref,
		const node_index_type tgt,
		const rule_time_type del,
		const node_index_type pred = INVALID_NODE_INDEX);

	void
	reset_delay_constraints(void) {
		min_delays.clear();
	}
#endif

	ostream&
	dump_invariant_message(ostream&, const expr_index_type, 
		const char*, const char*) const;

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_struct_dot(ostream&, const expr_index_type) const;

#if PRSIM_SETUP_HOLD
	ostream&
	dump_timing_constraints(ostream&) const;
#endif
#if PRSIM_TIMING_BACKANNOTATE
	ostream&
	dump_backannotated_delays(ostream&) const;

	ostream&
	dump_backannotated_delays_targeting(ostream&,
		const vector<node_index_type>&) const;
#endif

};	// end struct unique_process_subgraph

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_PROCESS_GRAPH_HH__

