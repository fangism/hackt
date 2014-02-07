/**
	\file "util/graph/bare_digraph.cc"
 */
#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <iterator>
#include <algorithm>

#include "util/graph/bare_digraph.hh"
#include "util/unique_list.tcc"		// for worklist
#include "util/iterator_more.hh"	// for set_inserter

#include "util/stacktrace.hh"

namespace util {
namespace graph {
using std::set;
#include "util/using_ostream.hh"

//=============================================================================
// class bare_digraph method definitions

struct bare_digraph::scc_node_info {
	int	index;	// -1 is undefined
	int	lowlink;
	scc_node_info() : index(-1), lowlink(-1) { }

	bool
	undefined(void) const {
		return index < 0;
	}

	bool
	is_root(void) const {
		return !undefined() && (index == lowlink);
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct bare_digraph::scc_state {
	vector<scc_node_info>	node_info;
	// needs fast lookup (membership) access
//	std::stack<size_t>	_stack;
	util::unique_list<size_t>	_stack;

	explicit
	scc_state(const size_t N) : node_info(N), _stack() { }

	ostream&
	dump_stack(ostream& o) const {
#if ENABLE_STACKTRACE
		o << "stack: {";
		std::copy(_stack.begin(), _stack.end(), 
			std::ostream_iterator<size_t>(o, ","));
		o << '}' << endl;
#endif
		return o;
	}
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
bare_digraph::contains_edge(const node_index_type i,
		const node_index_type j) const {
	if (i >= nodes.size() || j >= nodes.size())
		return false;
	return std::binary_search(nodes[i].begin(), nodes[i].end(), j);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Tarjan's SCC algorithm.
 */
void
bare_digraph::strongly_connected_components(SCC_type& sccs) const {
	STACKTRACE_VERBOSE;
	sccs.clear();
	const size_t N = nodes.size();
	sccs.reserve(N);	// guarantee never realloc
	scc_state st(N);
	size_t i = 0;
	for ( ; i<N; ++i) {
		if (st.node_info[i].undefined()) {
			__strong_connect(sccs, st, i);
		}
	}
	compact_SCCs(sccs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
compact_SCCs(SCC_type& s) {
	// compaction: remove empty SCCs, using swap!
	SCC_type t;
	t.reserve(s.size());
	SCC_type::iterator i(s.begin()), e(s.end());
	for ( ; i!=e; ++i) {
		if (!i->empty()) {
			t.resize(t.size() +1);
			t.back().swap(*i);
		}
	}
	// now t is compact
	s.swap(t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bare_digraph::SCCs_filter_cycles(const SCC_type& S, SCC_type& T) const {
	SCC_type::const_iterator i(S.begin()), e(S.end());
	for ( ; i!=e; ++i) {
		const size_t s = i->size();
		if (s > 1)
			T.push_back(*i);
		else if (s == 1) {
		// check for self-edge in singleton case!
			const size_t lone = *i->begin();
			if (contains_self_edge(lone)) {
				T.push_back(*i);
			}
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
SCCs_contains_cycles(const SCC_type& S) {
	SCC_type::const_iterator i(S.begin()), e(S.end());
	for ( ; i!=e; ++i) {
		if (i->size() > 1)
			return true;
	}
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// recursive
void
bare_digraph::__strong_connect(SCC_type& sccs,
		scc_state& st, const node_index_type v) const {
	STACKTRACE_BRIEF;
	STACKTRACE_INDENT_PRINT("visiting " << v << endl);
	scc_node_info& n(st.node_info[v]);
	const size_t ss = sccs.size();
	n.index = ss;
	n.lowlink = ss;
	sccs.resize(ss +1);
	set<size_t>& current_scc(sccs.back());
	st._stack.push(v);
//	STACKTRACE_INDENT_PRINT(""); st.dump_stack(cerr);
/**
    // Consider successors of v
    for each (v, w) in E do
       if (w.index is undefined) then
         // Successor w has not yet been visited; recurse on it
        strongconnect(w)
        v.lowlink := min(v.lowlink, w.lowlink)
      else if (w is in S) then
         // Successor w is in stack S and hence in the current SCC
         v.lowlink := min(v.lowlink, w.index)
      end if
    end for
**/
	out_edges_type::const_iterator
		oi(nodes[v].begin()), oe(nodes[v].end());
	for ( ; oi!=oe; ++oi) {
		const node_index_type& w(*oi);
		scc_node_info& m(st.node_info[w]);
		if (m.undefined()) {
			__strong_connect(sccs, st, w);
			n.lowlink = std::min(n.lowlink, m.lowlink);
		} else if (st._stack.contains(w)) {
			n.lowlink = std::min(n.lowlink, m.index);
		}
	}
/**
    // If v is a root node, pop the stack and generate an SCC
    if (v.lowlink = v.index) then
      start a new strongly connected component
      repeat
        w := S.pop()
        add w to current strongly connected component
      until (w = v)
      output the current strongly connected component
    end if
**/
	if (n.is_root()) {
		STACKTRACE_INDENT_PRINT("node " << v << " is root" << endl);
//	if (!st._stack.empty()) {
		node_index_type w;
		do {
			w = st._stack.pop();
			current_scc.insert(w);
		} while (v != w); // ( && !st._stack.empty());
//	}
	}
}

//-----------------------------------------------------------------------------
/**
	Reverse all edges.
	Set this graph to be the reverse digraph of G.
 */
void
bare_digraph::reverse(const bare_digraph& G) {
	nodes.clear();
	nodes.resize(G.nodes.size());
{
	nodes_type::const_iterator i(G.nodes.begin()), e(G.nodes.end());
	node_index_type j = 0;
	for ( ; i!=e; ++i, ++j) {
		out_edges_type::const_iterator ii(i->begin()), ie(i->end());
		for ( ; ii!=ie; ++ii) {
			__add_edge(*ii, j);
		}
	}
}{
	nodes_type::iterator i(nodes.begin()), e(nodes.end());
	for ( ; i!=e; ++i) {
		std::sort(i->begin(), i->end());
	}
}
}

//-----------------------------------------------------------------------------
/**
	Compute G+ without bothering with SCCs first.
	Algorithm: worklisted back-propagation.
 */
void
bare_digraph::transitive_closure(void) {
	STACKTRACE_BRIEF;
//	bare_digraph R;
//	R.reverse(*this);
	// workspace: use sets, then convert back
	// TODO: consider using vector<bool> or std::bitset
	typedef	set<node_index_type> edge_set_type;
	vector<edge_set_type> W(nodes.size());
	vector<edge_set_type> R(nodes.size());
	// initialize worklist
	unique_list<node_index_type> worklist;
	node_index_type i = 0;
	for ( ; i<nodes.size(); ++i) {
		out_edges_type::const_iterator
			j(nodes[i].begin()), e(nodes[i].end());
		W[i].insert(j, e);	// copy forward edges
		for ( ; j!=e; ++j) {
			R[*j].insert(i);	// create reverse edges
		}
		worklist.push(i);
	}
	while (!worklist.empty()) {
		// propagate the successors of this node to its predecessors
		// also update reverse graph R
		// for each predecessor, if its #succ increased,
		// re-queue it in worklist.
		const node_index_type x = worklist.pop_front();
		STACKTRACE_INDENT_PRINT("considering node " << x << endl);
		edge_set_type::const_iterator pi(R[x].begin()), pe(R[x].end());
		for ( ; pi!=pe; ++pi) {
			STACKTRACE_INDENT_PRINT("  pred " << *pi << endl);
			edge_set_type& f(W[*pi]);
			const size_t ps = f.size();
			edge_set_type::const_iterator
				j(W[x].begin()), e(W[x].end());
			f.insert(j, e);	// set-union
			const size_t ns = f.size();
#if 0
			// It's s trap!
			// updating reverse edges at the same time may
			// seem clever, but this might invalidate R iterators!
			for ( ; j!=e; ++i) {
				R[*j].insert(*pi);
			}
#endif
			if (ns > ps) {
				STACKTRACE_INDENT_PRINT("    was updated" << endl);
				worklist.move_back(*pi);
			}
		}
	}
	// translate sets (W) back to compact form
	for (i=0; i<nodes.size(); ++i) {
		nodes[i].clear();
		copy(W[i].begin(), W[i].end(), back_inserter(nodes[i]));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bare_digraph::dump(ostream& o) const {
	size_t i = 0;
	for ( ; i<nodes.size(); ++i) {
		o << i << ": {";
		copy(nodes[i].begin(), nodes[i].end(),
			std::ostream_iterator<node_index_type>(o, ","));
		o << '}' << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
dump_SCCs(ostream& o, const SCC_type& sccs) {
	SCC_type::const_iterator i(sccs.begin()), e(sccs.end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		o << "SCC[" << j << "]: {";
		std::copy(i->begin(), i->end(),
			std::ostream_iterator<size_t>(o, ","));
		o << '}' << endl;
	}
}

//=============================================================================
}	// end namespace graph
}	// end namespace util
