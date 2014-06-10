/**
	\file "sim/prsim/process_graph.hh"
	$Id: process_graph.hh,v 1.6 2010/09/29 00:13:43 fang Exp $
	Unique process subgraph structure, shared by all process instances
	of the same type.
 */

#ifndef	__HAC_SIM_PRSIM_PROCESS_GRAPH_H__
#define	__HAC_SIM_PRSIM_PROCESS_GRAPH_H__

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

	struct counter;			// counter
	// default ctor/dtor/copy
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

	struct memory_accumulator;

	unique_process_subgraph();
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

	ostream&
	dump_invariant_message(ostream&, const expr_index_type, 
		const char*, const char*) const;

	ostream&
	dump_struct(ostream&) const;

	ostream&
	dump_struct_dot(ostream&, const expr_index_type, 
		const footprint_frame_map_type&) const;

};	// end struct unique_process_subgraph

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_PRSIM_PROCESS_GRAPH_H__

