/**
	\file "util/graph/bare_digraph.cc"
 */
#define	ENABLE_STACKTRACE		0

#if ENABLE_STACKTRACE
#include <iostream>
#include <iterator>
#include <algorithm>
#endif

#include "util/graph/bare_digraph.hh"
#include "util/unique_list.tcc"		// for worklist
#include "util/iterator_more.hh"	// for set_inserter

#include "util/stacktrace.hh"

namespace util {
namespace graph {
using std::ostream;
#if ENABLE_STACKTRACE
#include "util/using_ostream.hh"
#endif

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

// recursive
void
bare_digraph::__strong_connect(SCC_type& sccs,
		scc_state& st, const size_t v) const {
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
		const size_t& w(*oi);
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
		size_t w;
		do {
			w = st._stack.pop();
			current_scc.insert(w);
		} while (v != w); // ( && !st._stack.empty());
//	}
	}
}

//=============================================================================
}	// end namespace graph
}	// end namespace util
