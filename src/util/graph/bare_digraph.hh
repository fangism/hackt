/**
	\file "util/graph/bare_digraph.hh"
	$Id: $
 */
#ifndef	__UTIL_GRAPH_BARE_DIGRAPH_HH__
#define	__UTIL_GRAPH_BARE_DIGRAPH_HH__

#include "util/graph/bare_digraph_fwd.hh"

namespace util {
namespace graph {
using std::vector;
using std::ostream;

/// remove empty SCCs
extern
void
compact_SCCs(SCC_type&);

extern
void
dump_SCCs(ostream&, const SCC_type&);

extern
void
SCCs_filter_cycles(const SCC_type&, SCC_type&);

extern
bool
SCCs_contains_cycles(const SCC_type&);

/**
	Nodes are merely contiguous numbers, for lookup efficiency.
 */
class bare_digraph {
	typedef	size_t				node_index_type;
	// vector must be ordered
	typedef	vector<node_index_type>	out_edges_type;
	typedef	vector<out_edges_type>	nodes_type;
	nodes_type				nodes;

public:
	void
	resize(const size_t N) { nodes.resize(N); }

	size_t
	size(void) const { return nodes.size(); }

	const out_edges_type&
	get_node(const node_index_type i) const {
		return nodes[i];
	}

	void
	__add_edge(const node_index_type i, const node_index_type j) {
		// unsafe, bounds should be checked by caller a priori
		// j should be unique, duplicate-free
		nodes[i].push_back(j);
	}

	void
	reverse(const bare_digraph&);

	// return-type?
	void
	strongly_connected_components(SCC_type&) const;

	void
	transitive_closure(void);

	// operates on clusters of SCCs first (condensed graphs)
	void
	transitive_closure_with_SCCs(void);

	ostream&
	dump(ostream&) const;

private:
	struct scc_node_info;
	struct scc_state;

	void
	__strong_connect(SCC_type&, scc_state&, const node_index_type) const;

};	// end class bare_digraph

}	// end namespace graph
}	// end namespace util

#endif	// __UTIL_GRAPH_BARE_DIGRAPH_HH__
