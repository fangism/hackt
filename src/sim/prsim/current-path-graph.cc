/**
	\file "sim/prsim/current-path-graph.cc"
	TODO: consider keeping this in net/ as home, as it may be useful
		for other netlist digraph analyses.
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <iterator>
#include <algorithm>
#include "sim/prsim/current-path-graph.hh"
#include "net/netlist_options.hh"
#include "util/iterator_more.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.hh"
using std::pair;
using NET::transistor;
using NET::netlist;
using NET::netlist_options;
using std::ostream_iterator;

//=============================================================================
// struct transistor_edge method definitions

ostream&
operator << (ostream& o, const transistor_edge& e) {
	return o << '[' << e.index << "]->" << e.destination;
}

//=============================================================================
// class netgraph_node method definitions

ostream&
netgraph_node::dump(ostream& o) const {
	o << "\tup: ";
	copy(up_edges.begin(), up_edges.end(),
		ostream_iterator<transistor_edge>(o, " "));
	o << endl;
	o << "\tdn: ";
	copy(dn_edges.begin(), dn_edges.end(),
		ostream_iterator<transistor_edge>(o, " "));
	o << endl;
	o << "\tbi: ";
	copy(bi_edges.begin(), bi_edges.end(),
		ostream_iterator<transistor_edge>(o, " "));
	o << endl;
	return o;
}

//=============================================================================
// class current_path_graph method definitions

current_path_graph::current_path_graph(const netlist& nl) :
		_netlist(nl), nodes()
		// default ctor everything else
		{
	STACKTRACE_VERBOSE;
	// allocate nodes, construct graph, identify supplies, precharged nodes
	__ctor_initialize_nodes();
	__ctor_initialize_edges();
	__ctor_identify_supplies();
	__mark_logical_pull_down_nodes();
	__mark_logical_pull_up_nodes();
#if CACHE_PRECHARGE_PATHS
	__ctor_identify_precharge_paths();
#endif
#if ENABLE_STACKTRACE
	dump(cout);
#endif
}	// end current_path_graph ctor

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
current_path_graph::__ctor_initialize_nodes(void) {
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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
		// forward edge: head towards GND, tail to Vdd (current flow)
		// backward edge: tail towards GND, head to Vdd
		const bool nf = fet.is_NFET();
		if (nf) {
			fwd.destination = fet.source;
			bwd.destination = fet.drain;
		} else {	// is PFET
			fwd.destination = fet.drain;
			bwd.destination = fet.source;
		}
		// non-restoring FETs are backwards
		const bool nr = fet.is_non_restoring();
		if (nr) {
			nodes[bwd.destination].up_edges.push_back(fwd);
			nodes[fwd.destination].dn_edges.push_back(bwd);
		} else {
			nodes[bwd.destination].dn_edges.push_back(fwd);
			nodes[fwd.destination].up_edges.push_back(bwd);
		}
		const bool p = fet.is_precharge();
		if (p) {
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
#if CACHE_PRECHARGE_PATHS
		util::map_key_inserter(precharged_internal_nodes)
#else
		util::set_inserter(precharged_internal_nodes)
#endif
		);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CACHE_PRECHARGE_PATHS
/**
	\pre already called __mark_logical_pull_down_nodes and
		__mark_logical_pull_up_nodes to classify 
		internal node directions.
	\pre precharged_internal_nodes already identified,
		and supplies have already been identified.
 */
void
current_path_graph::__ctor_identify_precharge_paths(void) {
	STACKTRACE_VERBOSE;
	precharge_map_type::iterator
		pi(precharged_internal_nodes.begin()),
		pe(precharged_internal_nodes.end());
	for ( ; pi!=pe; ++pi) {
		// build invariant expressions, reach supplies first
		const size_t& ni = pi->first;
		// pull-up expression to supply
		// pull-dn expression to supply
		// path expression to output node(s)
		subgraph_paths& g(pi->second);
	if (nodes[ni].dir) {	// is P-stack node
		__visit_precharge_paths_to_ground(
			g.paths_to_precharge_supply, ni);
		__visit_logic_paths_to_power(
			g.paths_to_logic_supply, ni);
		__visit_output_paths_down(
			g.paths_to_named_output, ni);
	} else {		// is N-stack node
		__visit_precharge_paths_to_power(
			g.paths_to_precharge_supply, ni);
		__visit_logic_paths_to_ground(
			g.paths_to_logic_supply, ni);
		__visit_output_paths_up(
			g.paths_to_named_output, ni);
	}
	}
}
#endif

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
	STACKTRACE_BRIEF;
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
}	// end __visit_paths_DFS_generic

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CACHE_PRECHARGE_PATHS
/**
	NOTE: this allows both PFET and NFET precharges up to Vdd
 */
void
current_path_graph::__visit_precharge_paths_to_power(
		set<index_type>& g,
		const index_type ni) const {
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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
#endif	// CACHE_PRECHARGE_PATHS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre nodes and edges have been initialized, supplies identified.
	\post intenral nodes in N-stacks have their dir flags set correctly
 */
void
current_path_graph::__mark_logical_pull_down_nodes(void) {
	STACKTRACE_VERBOSE;
	set<index_type>::const_iterator
		gi(ground_supply_nodes.begin()), ge(ground_supply_nodes.end());
	// for each distinct ground supply
	for ( ; gi!=ge; ++gi) {
		node_predicate_map_type v;
		set<index_type> e;	// not really used
		__visit_paths_DFS_generic<&this_type::signal_nodes,
			&netgraph_node::up_edges,
			&transistor_base::is_NFET>(e, v, *gi);
		// NOTE: traversal is limited to NFETs only
		node_predicate_map_type::const_iterator
			ni(v.begin()), ne(v.end());
		for ( ; ni!=ne; ++ni) {
			// ni->second true means that node is on path to output
		if (ni->second) {
		if (internal_nodes.find(ni->first) != internal_nodes.end()) {
			// only interested in internal nodes
			nodes[ni->first].dir = false;
		}
		}
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre nodes and edges have been initialized, supplies identified.
	\post intenral nodes in P-stacks have their dir flags set correctly
 */
void
current_path_graph::__mark_logical_pull_up_nodes(void) {
	STACKTRACE_VERBOSE;
	set<index_type>::const_iterator
		gi(power_supply_nodes.begin()), ge(power_supply_nodes.end());
	// for each distinct ground supply
	for ( ; gi!=ge; ++gi) {
		node_predicate_map_type v;
		set<index_type> e;	// not really used
		__visit_paths_DFS_generic<&this_type::signal_nodes,
			&netgraph_node::dn_edges,
			&transistor_base::is_PFET>(e, v, *gi);
		// NOTE: traversal is limited to NFETs only
		node_predicate_map_type::const_iterator
			ni(v.begin()), ne(v.end());
		for ( ; ni!=ne; ++ni) {
			// ni->second true means that node is on path to output
		if (ni->second) {
		if (internal_nodes.find(ni->first) != internal_nodes.end()) {
			// only interested in internal nodes
			nodes[ni->first].dir = true;
		}
		}
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
current_path_graph::node_is_logic_signal(const index_type i) const {
	return _netlist.node_pool[i].is_logical_node();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
current_path_graph::node_is_internal(const index_type i) const {
	return _netlist.node_pool[i].is_internal_node();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
current_path_graph::dump(ostream& o) const {
{
	vector<netgraph_node>::const_iterator
		ni(nodes.begin()), ne(nodes.end());
	o << "nodes:" << endl;
	size_t j = 0;
	for ( ; ni!=ne; ++ni, ++j) {
		_netlist.node_pool[j].emit(o << j << ": ",
			netlist_options::default_value) << endl;
		ni->dump(o);
	}
}{
	vector<transistor_base>::const_iterator
		ti(edges.begin()), te(edges.end());
	o << "edges:" << endl;
	size_t j = 0;
	for ( ; ti!=te; ++ti, ++j) {
		o << j << ": ";
//		_netlist.transistor_pool[j].emit(o,
//			netlist_options::default_value);
		ti->dump(o);
		o << endl;
	}
}{
	o << "precharge transistors: ";
	copy(precharge_transistors.begin(), precharge_transistors.end(),
		ostream_iterator<index_type>(o, " "));
	o << endl;
	o << "power supplies: ";
	copy(power_supply_nodes.begin(), power_supply_nodes.end(),
		ostream_iterator<index_type>(o, " "));
	o << endl;
	o << "ground supplies: ";
	copy(ground_supply_nodes.begin(), ground_supply_nodes.end(),
		ostream_iterator<index_type>(o, " "));
	o << endl;
	o << "signal nodes: ";
	copy(signal_nodes.begin(), signal_nodes.end(),
		ostream_iterator<index_type>(o, " "));
	o << endl;
	o << "internal nodes: ";
	copy(internal_nodes.begin(), internal_nodes.end(),
		ostream_iterator<index_type>(o, " "));
	o << endl;
	o << "precharged internal nodes: ";
	copy(precharged_internal_nodes.begin(), precharged_internal_nodes.end(),
		ostream_iterator<index_type>(o, " "));
	o << endl;
}
	return o;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

