/**
	\file "util/graph/bare_digraph.hh"
	$Id: $
 */
#ifndef	__UTIL_GRAPH_BARE_DIGRAPH_HH__
#define	__UTIL_GRAPH_BARE_DIGRAPH_HH__

#include <set>
#include <vector>

namespace util {
namespace graph {
using std::vector;
using std::set;

typedef vector<set<size_t> >			SCC_type;

/// remove empty SCCs
extern
void
compact_SCCs(SCC_type&);

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

	void
	__add_edge(const node_index_type i, const node_index_type j) {
		// unsafe, bounds should be checked by caller a priori
		// j should be unique, duplicate-free
		nodes[i].push_back(j);
	}

	// return-type?
	void
	strongly_connected_components(SCC_type&) const;
private:
	struct scc_node_info;
	struct scc_state;

	void
	__strong_connect(SCC_type&, scc_state&, const size_t) const;

};	// end class bare_digraph

}	// end namespace graph
}	// end namespace util

#endif	// __UTIL_GRAPH_BARE_DIGRAPH_HH__
