/**
	\file "sim/prsim/current-path-graph.cc"
 */

#include "sim/prsim/current-path-graph.h"
#include "util/iterator_more.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using std::pair;
using NET::transistor;
using NET::netlist;
using util::set_inserter;

//=============================================================================
// class current_path_graph method definitions

current_path_graph::current_path_graph(const netlist& nl) :
		_netlist(nl), nodes()
		// default ctor everything else
		{
	// allocate nodes, construct graph, identify supplies, precharged nodes
	__ctor_initialize_nodes();
	__ctor_initialize_edges();
	__ctor_identify_supplies();
}	// end current_path_graph ctor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
current_path_graph::__ctor_initialize_nodes(void) {
	const size_t ns = _netlist.node_pool.size();
	nodes.resize(ns);
	size_t i = netlist::first_node_index;	// skip the void node
	for ( ; i<ns; ++i) {
		const NET::node& nn(_netlist.node_pool[i]);
		netgraph_node& n(nodes[i]);
		// identify named output nodes and gates
		if (nn.is_logical_node()) {
			n.named_index = nn.index;
			signal_nodes.insert(i);
			// may include power supplies
		} else {
			// anonymous auxiliary nodes and named internal nodes
			internal_nodes.insert(i);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre transistor edges have already been processed by
		__ctor_initialize_edges.
 */
void
current_path_graph::__ctor_identify_supplies(void) {
	// identify supplies by source and sink nodes in graph
	set<index_type>::const_iterator
		si(signal_nodes.begin()), se(signal_nodes.end());
	for ( ; si!=se; ++si) {
		const size_t& i(*si);
		const netgraph_node& n(nodes[i]);
		if (n.dn_edges.empty() && !n.up_edges.empty()) {
			ground_supply_nodes.insert(i);
		} else if (n.up_edges.empty() && !n.dn_edges.empty()) {
			power_supply_nodes.insert(i);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This also identifies precharged_internal_nodes.
	\pre nodes have been initialized.
 */
void
current_path_graph::__ctor_initialize_edges(void) {
	set<index_type> precharge_source_nodes, precharge_drain_nodes;
	// iterate over transistors instead
	size_t i = 0;
	const size_t nt = _netlist.total_transistor_count();
	edges.reserve(nt);
	// don't have an iterator that spans local subcircuits
	for ( ; i<nt; ++i) {
		const transistor&
			fet(_netlist.lookup_transistor(i));
		const transistor_base tb(fet);
		edges.push_back(tb);
		transistor_edge fwd(i), bwd(i);
		// forward edge: head towards GND, tail to Vdd
		// backward edge: tail towards GND, head to Vdd
		if (fet.type == transistor::NFET_TYPE) {
			fwd.destination = fet.source;
			bwd.destination = fet.drain;
			nodes[fet.drain].dn_edges.push_back(fwd);
			nodes[fet.source].up_edges.push_back(bwd);
		} else {	// is PFET
			fwd.destination = fet.drain;
			bwd.destination = fet.source;
			nodes[fet.source].dn_edges.push_back(fwd);
			nodes[fet.drain].up_edges.push_back(bwd);
		}
		if (fet.is_precharge()) {
			precharge_transistors.insert(i);
			precharge_source_nodes.insert(fet.source);
			precharge_drain_nodes.insert(fet.drain);
		}
	}
	// nodes that are only drains of precharges (AND NOT source)
	// are the precharged internal nodes of interest to us
	std::set_difference(precharge_drain_nodes.begin(),
		precharge_drain_nodes.end(),
		precharge_source_nodes.begin(), 
		precharge_source_nodes.end(),
		set_inserter(precharged_internal_nodes));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre precharged_internal_nodes already identified,
		and supplies have already been identified.
 */
void
current_path_graph::__ctor_identify_precharge_paths(void) {
	set<index_type>::const_iterator
		pi(precharged_internal_nodes.begin()),
		pe(precharged_internal_nodes.end());
	for ( ; pi!=pe; ++pi) {
		// build invariant expressions, reach supplies first
		// pull-up expression to supply
		// pull-dn expression to supply
		// path expression to output node(s)
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive DFS to find all specified directed paths.
	Objective is to just collect all edges on such paths.  
	TODO: include bidirectional edges
	TODO: this assumes directed acyclic graph, might detect cycles?
	\param p is the current DFS path, stack of edges.
	\param g is the set of edges in the subgraph between root node
		and power supply nodes (returned).
	\param v is the set of visited nodes, value is true if 
		power-supply is reachable from key node.
	\param ni node index.
	\return true if node has path to supply.
 */
template <set<index_type> current_path_graph::*terminals,
	vector<transistor_edge> netgraph_node::*edge_set,
	bool (transistor_base::*pred)(void) const>
bool
current_path_graph::__visit_paths_DFS_generic(
		set<index_type>& g,
		node_predicate_map_type& v, 
		const index_type ni) const {
	typedef	node_predicate_map_type		map_type;
	typedef	map_type::value_type	map_value_type;
	typedef	map_type::iterator	visiterator;
	const pair<visiterator, bool> f(v.insert(map_value_type(ni, false)));
if (f.second) {
	// never visited before
	bool ret = false;
	const netgraph_node& n(nodes[ni]);
	if ((this->*terminals).find(ni) != (this->*terminals).end()) {
		ret = true;
		// found a supply node
		if (terminals != &this_type::signal_nodes) {
			INVARIANT((n.*edge_set).empty());	// sanity check
		}
	} else {
		// visit further
	vector<transistor_edge>::const_iterator
		ei((n.*edge_set).begin()), ee((n.*edge_set).end());
	for ( ; ei!=ee; ++ei) {
		if ((edges[ei->index].*pred)()) {
		if (__visit_paths_DFS_generic<terminals, edge_set, pred>(
				g, v, ei->destination)) {
			g.insert(ei->index);
			ret = true;
		}
		}
	}
	}
	f.first->second = ret;
	return ret;
} else {
	// visited before (cached)
	return f.first->second;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this allows both PFET and NFET precharges up to Vdd
 */
void
current_path_graph::__visit_precharge_paths_to_power(
		set<index_type>& g,
		const index_type ni) const {
	node_predicate_map_type v;
	__visit_paths_DFS_generic<&this_type::power_supply_nodes,
		&netgraph_node::up_edges,
		&transistor_base::is_precharge>(g, v, ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this allows both PFET and NFET precharges down to GND
 */
void
current_path_graph::__visit_precharge_paths_to_ground(
		set<index_type>& g,
		const index_type ni) const {
	node_predicate_map_type v;
	__visit_paths_DFS_generic<&this_type::ground_supply_nodes,
		&netgraph_node::dn_edges,
		&transistor_base::is_precharge>(g, v, ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Identify paths from internal node up to power-supply 
	through logical gates.  (not precharge)
 */
void
current_path_graph::__visit_logic_paths_to_power(
		set<index_type>& g,
		const index_type ni) const {
	node_predicate_map_type v;
	__visit_paths_DFS_generic<&this_type::power_supply_nodes,
		&netgraph_node::up_edges,
		&transistor_base::is_not_precharge>(g, v, ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Identify paths from internal node up to ground-supply 
	through logical gates.  (not precharge)
 */
void
current_path_graph::__visit_logic_paths_to_ground(
		set<index_type>& g,
		const index_type ni) const {
	node_predicate_map_type v;
	__visit_paths_DFS_generic<&this_type::ground_supply_nodes,
		&netgraph_node::dn_edges,
		&transistor_base::is_not_precharge>(g, v, ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Identify paths from internal node up to logical output node.
 */
void
current_path_graph::__visit_output_paths_up(
		set<index_type>& g,
		const index_type ni) const {
	node_predicate_map_type v;
	__visit_paths_DFS_generic<&this_type::signal_nodes,
		&netgraph_node::up_edges,
		&transistor_base::is_not_precharge>(g, v, ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Identify paths from internal node down to logical output node.
 */
void
current_path_graph::__visit_output_paths_down(
		set<index_type>& g,
		const index_type ni) const {
	node_predicate_map_type v;
	__visit_paths_DFS_generic<&this_type::signal_nodes,
		&netgraph_node::dn_edges,
		&transistor_base::is_not_precharge>(g, v, ni);
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

