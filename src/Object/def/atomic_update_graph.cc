/**
	\file "Object/def/atomic_update_graph.cc"
 */

#include <algorithm>
#include <iterator>

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_IOS				(0 && ENABLE_STACKTRACE)

#include "Object/def/atomic_update_graph.hh"
#include "Object/def/footprint.hh"
#include "Object/global_entry.hh"
#include "util/graph/bare_digraph.hh"
#include "util/unique_list.tcc"		// for worklist
#include "util/iterator_more.hh"	// for set_inserter
#include "util/IO_utils.tcc"
#include "util/indent.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::write_value;
using util::read_value;
using util::auto_indent;

//=============================================================================
// class atomic_update_graph method definitions

/**
	Keep/import all edges with both ends <= max.
	This projects the local reachability graph onto the public ports.
 */
atomic_update_graph::atomic_update_graph(const atomic_update_graph& G, 
		const node_index_type max) : nodes() {
	nodes_type::const_iterator
		i(G.nodes.begin()), e(G.nodes.lower_bound(max));
	for ( ; i!=e; ++i) {
		out_edges_type::const_iterator
			ei(i->second.begin()), ee(i->second.lower_bound(max));
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
atomic_update_graph::atomic_update_graph(const footprint_frame& ff) : nodes() {
	const atomic_update_graph&
		G(ff._footprint->get_exported_atomic_update_graph());
#if ENABLE_STACKTRACE
	cerr << "exported atomic deps:" << endl;
	G.dump(cerr);
	cerr << endl;
#endif
	const footprint_frame_map_type& ffm(ff.get_frame_map<bool_tag>());
	nodes_type::const_iterator i(G.nodes.begin()), e(G.nodes.end());
#if 0
	if (ffm.size()) {
#endif
	const footprint_frame_transformer fft(ffm);
	for ( ; i!=e; ++i) {
		out_edges_type::const_iterator
			ei(i->second.begin()), ee(i->second.end());
		for ( ; ei!=ee; ++ei) {
			add_edge(fft(i->first), fft(*ei));
		}
	}
#if 0
	} else {
		// top-level uses empty frame, should be identity transformation
		STACKTRACE_INDENT_PRINT("empty frame (top-level?)" << endl);
		*this = G;
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
atomic_update_graph::import(const atomic_update_graph& G) {
	STACKTRACE_VERBOSE;
	nodes_type::const_iterator i(G.nodes.begin()), e(G.nodes.end());
	for ( ; i!=e; ++i) {
		out_edges_type::const_iterator
			ei(i->second.begin()), ee(i->second.end());
		for ( ; ei!=ee; ++ei) {
			add_edge(i->first, *ei);
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
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
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
	STACKTRACE_VERBOSE;
	sccs.clear();
	bare_digraph G;
	vector<node_index_type> rmap;
	export_bare_digraph(G, rmap);
#if ENABLE_STACKTRACE
	cerr << "G: " << endl;
	G.dump(cerr) << endl;
	cerr << "rmap: ";
	copy(rmap.begin(), rmap.end(),
		std::ostream_iterator<node_index_type>(cerr, ","));
#endif
	SCC_type S;
	G.strongly_connected_components(S);
#if ENABLE_STACKTRACE
	cerr << "SCCs: " << endl;
	util::graph::dump_SCCs(cerr, S);
	cerr << endl;
#endif
	// translate S back to node indices
	reverse_translate_SCCs(rmap, S, sccs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as above, but filtered to include cycles of size>1
	and singleton cycles with self-edges.  
 */
void
atomic_update_graph::strongly_connected_components_filtered(
		SCC_type& sccs) const {
	STACKTRACE_VERBOSE;
	sccs.clear();
	bare_digraph G;
	vector<node_index_type> rmap;
	export_bare_digraph(G, rmap);
#if ENABLE_STACKTRACE
	cerr << "G: " << endl;
	G.dump(cerr) << endl;
	cerr << "rmap: ";
	copy(rmap.begin(), rmap.end(),
		std::ostream_iterator<node_index_type>(cerr, ","));
#endif
	SCC_type S, T;
	G.strongly_connected_components(S);
	G.SCCs_filter_cycles(S, T);
#if ENABLE_STACKTRACE
	cerr << "SCCs: " << endl;
	util::graph::dump_SCCs(cerr, T);
	cerr << endl;
#endif
	// translate S back to node indices
	reverse_translate_SCCs(rmap, T, sccs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Heuristically, it is recommended to compute SCCs first, 
	however, in our application, cycles are forbidden and would
	have been rejected before reaching this point.
 */
void
atomic_update_graph::transitive_closure(void) {
	STACKTRACE_VERBOSE;
	bare_digraph G;
	vector<node_index_type> rmap;
	export_bare_digraph(G, rmap);
#if ENABLE_STACKTRACE
	cerr << "this: " << endl;
	this->dump(cerr) << endl;
	cerr << "G: " << endl;
	G.dump(cerr) << endl;
	cerr << "rmap: ";
	copy(rmap.begin(), rmap.end(),
		std::ostream_iterator<node_index_type>(cerr, ","));
	cerr << endl;
#endif
	G.transitive_closure();
#if ENABLE_STACKTRACE
	cerr << "G+: " << endl;
	G.dump(cerr) << endl;
#endif
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

#endif
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
atomic_update_graph::dump(ostream& o) const {
	nodes_type::const_iterator i(nodes.begin()), e(nodes.end());
	for ( ; i!=e; ++i) {
		o << auto_indent << i->first << " -> {";
		copy(i->second.begin(), i->second.end(),
			std::ostream_iterator<node_index_type>(o, ","));
		o << '}' << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
atomic_update_graph::write_object(ostream& o) const {
//	util::write_map(o, nodes);	// doesn't work with set<>
	nodes_type::const_iterator i(nodes.begin()), e(nodes.end());
	write_value(o, nodes.size());
	for ( ; i!=e; ++i) {
		write_value(o, i->first);
		util::write_sequence(o, i->second);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
atomic_update_graph::load_object(istream& i) {
//	util::read_map(i, nodes);	// doesn't work with set<>
	size_t s;
	read_value(i, s);
	size_t j = 0;
	for ( ; j<s; ++j) {
		node_index_type k;
		read_value(i, k);
		util::read_sequence_set_insert(i, nodes[k]);
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC
