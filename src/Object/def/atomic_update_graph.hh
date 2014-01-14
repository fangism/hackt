/**
	\file "Object/def/atomic_update_graph.hh"
	$Id: $
 */
#ifndef	__HAC_OBJECT_DEF_ATOMIC_UPDATE_GRAPH_HH__
#define	__HAC_OBJECT_DEF_ATOMIC_UPDATE_GRAPH_HH__

#include <map>
#include <set>
#include <vector>
#include "util/graph/bare_digraph_fwd.hh"

namespace HAC {
namespace entity {
class footprint_frame;
using std::vector;
using util::graph::bare_digraph;
using util::graph::SCC_type;

// directed graph
class atomic_update_graph {
	typedef	size_t				node_index_type;
	typedef	std::set<node_index_type>	out_edges_type;
	typedef	std::map<node_index_type, out_edges_type>
						nodes_type;
	nodes_type				nodes;

public:
	atomic_update_graph() : nodes() { }

	// default copy-ctor
	atomic_update_graph(const atomic_update_graph& g) : nodes(g.nodes) { }
	// produce port reachability summary
	atomic_update_graph(const atomic_update_graph&, 
		const node_index_type max);
	// translate formal summary to local instance actuals
	atomic_update_graph(const atomic_update_graph&, 
		const footprint_frame&);

	void
	swap(atomic_update_graph& G) {
		nodes.swap(G.nodes);
	}

	void
	add_edge(const node_index_type i, const node_index_type j) {
		nodes[j];
		nodes[i].insert(j);
	}

	// also saves reverse map
	void
	export_bare_digraph(bare_digraph&, vector<node_index_type>&) const;

	static
	void
	reverse_translate_SCCs(const vector<node_index_type>&, 
		const SCC_type&, SCC_type&);

	void
	reverse_translate_digraph(const vector<node_index_type>&, 
		const bare_digraph&);

#if 0
	// reverse directed graph
	void
	reverse(const atomic_update_graph&);
#endif

	void
	transitive_closure(void);

	// return-type?
	void
	strongly_connected_components(SCC_type&) const;

#if 0
private:
	// essentially matrix multiply
	void
	propagate_reachability(void);
#endif

};	// end class atomic_update_graph

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_ATOMIC_UPDATE_GRAPH_HH__
