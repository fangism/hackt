/**
	\file "sim/prsim/current-path-graph.cc"
 */

#include "sim/prsim/current-path-graph.h"
#include "util/iterator_more.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using NET::transistor;
using NET::netlist;
using util::set_inserter;

//=============================================================================
/**
	Destination is initialized to NULL, must be set depending on
	forward/backward direction of edge.
 */
transistor_edge::transistor_edge(const transistor& fet) :
		gate(fet.gate), destination(0), type(fet.type), 
		attributes(fet.attributes) {
}

//=============================================================================
current_path_graph::current_path_graph(const netlist& nl) :
		_netlist(nl), nodes() {
	// allocate nodes, construct graph, identify supplies, precharged nodes
	const size_t ns = nl.node_pool.size();
	nodes.resize(ns);
{
	size_t i = netlist::first_node_index;	// skip the void node
	for ( ; i<ns; ++i) {
		const NET::node& nn(nl.node_pool[i]);
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
	set<index_type> precharge_source_nodes, precharge_drain_nodes;
{
	// iterate over transistors instead
	size_t i = 0;
	const size_t nt = nl.total_transistor_count();
	// don't have an iterator that spans local subcircuits
	for ( ; i<nt; ++i) {
		const transistor&
			fet(nl.lookup_transistor(i));
		transistor_edge fwd(fet), bwd(fet);
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
}{
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
}{
	// nodes that are only drains of precharges (AND NOT source)
	// are the precharged internal nodes of interest to us
	std::set_difference(precharge_drain_nodes.begin(),
		precharge_drain_nodes.end(),
		precharge_source_nodes.begin(), 
		precharge_source_nodes.end(),
		set_inserter(precharged_internal_nodes));
}

}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

