/**
	\file "Object/def/atomic_update_graph.cc"
 */

#include <algorithm>
#include <iterator>
// #include <stack>

#include "Object/def/atomic_update_graph.hh"
#include "Object/global_entry.hh"
#include "util/graph/bare_digraph.hh"
#include "util/unique_list.tcc"		// for worklist
#include "util/iterator_more.hh"	// for set_inserter

namespace HAC {
namespace entity {

//=============================================================================
// class atomic_update_graph method definitions

/**
	Keep/import all edges with both ends <= max.
	This projects the local reachability graph onto the public ports.
 */
atomic_update_graph::atomic_update_graph(const atomic_update_graph& G, 
		const node_index_type max) : nodes() {
	nodes_type::const_iterator
		i(G.nodes.begin()), e(G.nodes.upper_bound(max));
	for ( ; i!=e; ++i) {
		out_edges_type::const_iterator
			ei(i->second.begin()), ee(i->second.upper_bound(max));
		for ( ; ei!=ee; ++ei) {
			add_edge(i->first, *ei);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translate graph of formal nodes (definition) to graph of 
	actual nodes (instance).
 */
atomic_update_graph::atomic_update_graph(const atomic_update_graph& G, 
		const footprint_frame& ff) : nodes() {
	const footprint_frame_transformer fft(ff.get_frame_map<bool_tag>());
	nodes_type::const_iterator
		i(G.nodes.begin()), e(G.nodes.end());
	for ( ; i!=e; ++i) {
		out_edges_type::const_iterator
			ei(i->second.begin()), ee(i->second.end());
		for ( ; ei!=ee; ++ei) {
			add_edge(fft(i->first), fft(*ei));
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Compacts the node values of the graph to contiguous key-values.  
	\return rmap the reverse-map to translate back to node indices
 */
void
atomic_update_graph::export_bare_digraph(bare_digraph& F,
		vector<node_index_type>& rmap) const {
	// map is ordered, thus so will rmap
	rmap.clear();
	rmap.reserve(nodes.size());
	// reverse-translation lookup is binary_search-based
	const nodes_type::const_iterator nb(nodes.begin()), ne(nodes.end());
	nodes_type::const_iterator ni(nb);
	for ( ; ni!=ne; ++ni) {
		rmap.push_back(ni->first);
	}
	F.resize(rmap.size());
	for (ni=nb; ni!=ne; ++ni) {
		const vector<node_index_type>::iterator	// const_iterator
			f(std::lower_bound(rmap.begin(), rmap.end(),
				ni->first));
		assert(f != rmap.end());
		const size_t p = std::distance(rmap.begin(), f);
		out_edges_type::const_iterator
			ei(ni->second.begin()), ee(ni->second.end());
		for ( ; ei!=ee; ++ei) {
			const vector<node_index_type>::iterator	// const_iterator
				g(std::lower_bound(rmap.begin(), rmap.end(),
					*ei));
			const size_t q = std::distance(rmap.begin(), g);
			F.__add_edge(p, q);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
atomic_update_graph::reverse_translate_SCCs(const vector<node_index_type>& r,
	const SCC_type& S, SCC_type& T) {
	T.resize(S.size());
	SCC_type::const_iterator i(S.begin()), e(S.end());
	SCC_type::iterator j(T.begin());
	for ( ; i!=e; ++i, ++j) {
		std::set<size_t>::const_iterator ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
			j->insert(r[*ii]);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
atomic_update_graph::reverse_translate_digraph(const vector<node_index_type>& r,
		const bare_digraph& G) {
	size_t i = 0;
	for ( ; i<G.size(); ++i) {
		const vector<node_index_type>& n(G.get_node(i));
		const node_index_type ti = r[i];
		vector<node_index_type>::const_iterator
			j(n.begin()), e(n.end());
		for ( ; j!=e; ++j) {
			nodes[ti].insert(r[*j]);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Find cycles.
 */
void
atomic_update_graph::strongly_connected_components(
		SCC_type& sccs) const {
	sccs.clear();
	bare_digraph G;
	vector<node_index_type> rmap;
	export_bare_digraph(G, rmap);
	SCC_type S;
	G.strongly_connected_components(S);
	// translate S back to node indices
	reverse_translate_SCCs(rmap, S, sccs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Heuristically, it is recommended to compute SCCs first, 
	however, in our application, cycles are forbidden and would
	have been rejected before reaching this point.
 */
void
atomic_update_graph::transitive_closure(void) {
	bare_digraph G;
	vector<node_index_type> rmap;
	export_bare_digraph(G, rmap);
	G.transitive_closure();
	reverse_translate_digraph(rmap, G);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Construct a reverse graph of G.
	This is useful for propagating updates.  
 */
void
atomic_update_graph::reverse(const atomic_update_graph& G) {
	nodes.clear();
	nodes_type::const_iterator
		i(G.nodes.begin()), e(G.nodes.end());
	for ( ; i!=e; ++i) {
		out_edges_type::const_iterator
			ei(i->second.begin()), ee(i->second.end());
		for ( ; ei!=ee; ++ei) {
			add_edge(*ei, i->first);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
atomic_update_graph::propagate_reachability(void) {
	typedef	util::unique_list<node_index_type>	worklist_type;
	worklist_type w;
	// reveres graph?
	// initialize worklist with all nodes
	node_type::const_iterator ni(nodes.begin()), ne(nodes.end());
	for ( ; ni!=ne; ++ni) {
		w.push(ni->first);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace HAC
