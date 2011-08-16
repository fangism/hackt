/**
	\file "sim/prsim/current-path-graph.h"
	Simplified graph representation of transistor netlist and current paths.
	Edges represent gates of transistors.
	Edges point from Vdd to GND, direction of intended current flow.  
	$Id: $
 */

#ifndef	__HAC_SIM_PRSIM_CURRENT_PATH_GRAPH_H__
#define	__HAC_SIM_PRSIM_CURRENT_PATH_GRAPH_H__

#include <set>
#include "net/netgraph.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
using NET::index_type;
using std::vector;
using std::set;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Subset of information from struct transistor.
	Each node will have a copy of the transistor from 
	the tail end and head end.
 */
struct transistor_edge {
	// the named signal connected to FET gate
	index_type				gate;
	// the other end of this edge
	index_type				destination;
	char					type;	// N or P
	NET::transistor::attributes_type	attributes;

	explicit
	transistor_edge(const NET::transistor&);

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

	netgraph_node() : named_index(0) { }

	bool
	is_named_output(void) const { return named_index; }

};	// end struct netgraph_node

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
	const NET::netlist&			_netlist;
	vector<netgraph_node>			nodes;

	set<index_type>				precharge_transistors;

	set<index_type>				power_supply_nodes;
	set<index_type>				ground_supply_nodes;
	set<index_type>				signal_nodes;
	set<index_type>				internal_nodes;
	set<index_type>				precharged_internal_nodes;

public:
	explicit
	current_path_graph(const NET::netlist&);

};	// end class current_path_graph

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

#endif	 // __HAC_SIM_PRSIM_CURRENT_PATH_GRAPH_H__

