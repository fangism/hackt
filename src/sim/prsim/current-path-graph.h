/**
	\file "sim/prsim/current-path-graph.h"
	Simplified graph representation of transistor netlist and current paths.
	Edges represent gates of transistors.
	Edges point from Vdd to GND, direction of intended current flow.  
	$Id: $
 */

#ifndef	__HAC_SIM_PRSIM_CURRENT_PATH_GRAPH_H__
#define	__HAC_SIM_PRSIM_CURRENT_PATH_GRAPH_H__

#include "net/netgraph.h"

/**
	Don't think we actually need this.
 */
#define	CACHE_PRECHARGE_PATHS				0

namespace HAC {
namespace SIM {
namespace PRSIM {
using NET::index_type;
using std::vector;
using std::set;
using std::map;
using NET::transistor_base;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Subset of information from struct transistor.
	Each node will have an indexed reference to transistor_base from 
	the tail end and head end.
 */
struct transistor_edge {
	// index into transistor pool where common info is retained
	index_type				index;
	// the other end of this edge, which is directed
	index_type				destination;

	explicit
	transistor_edge(const index_type i) : index(i), destination(0) { }

};	// end struct transistor_edge_info

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Directed graph (digraph).
	Adjacency-list format of node.
 */
struct netgraph_node {
	/// is non-zero if corresponds to named node or supply
	index_type				named_index;
	vector<transistor_edge>			dn_edges; // outgoing, to GND
	vector<transistor_edge>			up_edges; // incoming, from Vdd
	// such bidirectional gates are needed for van Berkel style C-elements
	vector<transistor_edge>			bi_edges; // bidirectional
	/**
		For internal nodes only.
		if false, this node is primarily in N-stacks for pull-down.
		if true, this node is primarily in P-stacks for pull-up.
		For other nodes, this field has no meaning.
	 */
	bool					dir;

	netgraph_node() : named_index(0), dir(false) { }

	bool
	is_named_output(void) const { return named_index; }

};	// end struct netgraph_node

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CACHE_PRECHARGE_PATHS
/**
	A path_subgraph consists of a sparse set of edges (transistors)
	of the whole subcircuit graph.
	Interpretation depends on whether reference node is N or P.
 */
struct subgraph_paths {
	set<index_type>				paths_to_precharge_supply;
	set<index_type>				paths_to_logic_supply;
	set<index_type>				paths_to_named_output;
};	// end struct path_subgraph
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Simplified digraph representation of netlists with source nodes
	as Vdd power supplies, and sink nodes as GND supplies.
	Nodes are internal nodes, stack nodes, or output nodes.
	Edges are labeled with gates.
	Remember that these indices correspond to the netlist pool,
	not the local bool footprint.
 */
class current_path_graph {
	// CAUTION: this reference is owned by ExprAlloc!
	const NET::netlist&			_netlist;
	vector<netgraph_node>			nodes;
	vector<transistor_base>			edges;

	set<index_type>				precharge_transistors;

	set<index_type>				power_supply_nodes;
	set<index_type>				ground_supply_nodes;
	set<index_type>				signal_nodes;
	set<index_type>				internal_nodes;

public:
	/**
		key = node index (precharged nodes)
		value = set of paths to supplies and output nodes
	 */
#if CACHE_PRECHARGE_PATHS
	typedef	map<index_type, subgraph_paths>	precharge_map_type;
#else
	typedef	set<index_type>			precharge_map_type;
#endif
private:
	precharge_map_type			precharged_internal_nodes;

public:
	explicit
	current_path_graph(const NET::netlist&);

	const netgraph_node&
	get_node(const index_type i) const { return nodes[i]; }

	const transistor_base&
	get_edge(const index_type i) const { return edges[i]; }

	bool
	node_is_power(const index_type i) const {
		return power_supply_nodes.find(i) != power_supply_nodes.end();
	}

	bool
	node_is_ground(const index_type i) const {
		return ground_supply_nodes.find(i) != ground_supply_nodes.end();
	}

	bool
	node_is_logic_signal(const index_type) const;

	bool
	node_is_internal(const index_type) const;

	index_type
	translate_logical_bool_index(const index_type i) const {
		return _netlist.lookup_named_node(i);
	}

	const precharge_map_type&
	get_precharge_set(void) const { return precharged_internal_nodes; }

private:
	typedef	current_path_graph		this_type;

	void
	__ctor_initialize_nodes(void);

	void
	__ctor_initialize_edges(void);

	void
	__ctor_identify_supplies(void);

#if CACHE_PRECHARGE_PATHS
	void
	__ctor_identify_precharge_paths(void);
#endif

	typedef	map<index_type, bool>		node_predicate_map_type;

	// __visit_{precharge,logic}_paths_to_{power,ground}
	template <set<index_type> this_type::*,
		vector<transistor_edge> netgraph_node::*,
		bool (transistor_base::*)(void) const>
	bool
	__visit_paths_DFS_generic(
		set<index_type>&, node_predicate_map_type&, 
		const index_type) const;

#if CACHE_PRECHARGE_PATHS
	void
	__visit_precharge_paths_to_power(
		set<index_type>&, const index_type) const;

	void
	__visit_precharge_paths_to_ground(
		set<index_type>&, const index_type) const;

	void
	__visit_logic_paths_to_power(
		set<index_type>&, const index_type) const;

	void
	__visit_logic_paths_to_ground(
		set<index_type>&, const index_type) const;

	void
	__visit_output_paths_up(
		set<index_type>&, const index_type) const;

	void
	__visit_output_paths_down(
		set<index_type>&, const index_type) const;
#endif

	void
	__mark_logical_pull_down_nodes(void);

	void
	__mark_logical_pull_up_nodes(void);

};	// end class current_path_graph

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	 // __HAC_SIM_PRSIM_CURRENT_PATH_GRAPH_H__

