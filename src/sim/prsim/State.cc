/**
	\file "sim/prsim/State.cc"
	Implementation of prsim simulator state.  
	$Id: State.cc,v 1.4.8.2 2006/03/24 00:01:50 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_FANOUT			(0 && ENABLE_STACKTRACE)

#include <iostream>
#include <algorithm>
#include <functional>
#include <string>
#include "sim/prsim/State.h"
#include "sim/prsim/ExprAlloc.h"
#include "sim/prsim/Event.tcc"
#include "util/list_vector.tcc"
#include "Object/module.h"
#include "Object/state_manager.h"
#include "Object/traits/classification_tags.h"
#include "Object/global_entry.h"
#include "util/attributes.h"
#include "util/sstream.h"
#include "util/stacktrace.h"
#include "util/memory/index_pool.tcc"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity { }

namespace SIM {
namespace PRSIM {
using std::string;
using std::ostringstream;
using std::for_each;
using std::mem_fun_ref;
using entity::state_manager;
using entity::global_entry_pool;
using entity::bool_tag;
using entity::process_tag;
#include "util/using_ostream.h"
//=============================================================================
// class State method definitions

#if 0
/**
	TODO: pick reasonable chunk size for expr_pool.  
 */
State::State() : node_pool(), expr_pool(), expr_graph_node_pool(),
		event_pool(), event_queue() {
	expr_graph_node_pool.set_chunk_size(1024);
	head_sentinel();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates simulation state, given a module.
	TODO: do this work in module?
	TODO: add support for top-level PRS (outside of procs)
	TODO: expression minimization pass
	\param m the expanded module object.
	\pre m must already be past the allcoate phase.  
 */
#if 0
State::State(const count_ptr<const entity::module>& m) : 
#else
State::State(const entity::module& m) : 
#endif
		mod(m), 
		node_pool(), expr_pool(), expr_graph_node_pool(),
		event_pool(), event_queue(), 
		current_time(0) {
#if 0
	NEVER_NULL(m);
	const state_manager& sm(m->get_state_manager());
#else
	const state_manager& sm(mod.get_state_manager());
#endif
	const global_entry_pool<bool_tag>&
		bool_pool(sm.get_pool<bool_tag>());
	expr_graph_node_pool.set_chunk_size(1024);
	head_sentinel();
	// recall, the global node pool is 1-indexed because entry 0 is null
	// we mirror this in our own node state pool, by allocating
	// the same number of elements.  
	node_pool.resize(bool_pool.size());
	// then go through all processes to generate expressions
#if 0
	const global_entry_pool<process_tag>&
		proc_pool(sm.get_pool<process_tag>());
#endif
#if 0
	// create a temporary vector of expressions using list_vector first
	// then transfer them over to the expr_pool
	temp_expr_pool_type build_exprs;
	build_exprs.set_chunk_size(1024);
#endif
	// use a cflat-prs-like pass to construct the expression netlist
	// got a walker? and prs_expr_visitor?
	// see "ExprAlloc.h"

	// NOTE: we're referencing 'this' during construction, however, we 
	// are done constructing this State's members at this point.  
	ExprAlloc v(*this);

	sm.accept(v);
	// TODO: expression tree minimization pass
	// during minimization, need to create a map from old
	// to new indices, and keep track of unused indices
	// for a mark-sweep-compaction into target State
}	// end State::State(const module&)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: possibly run some checks?
 */
State::~State() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Destroys the simulator state, releasing all of its memory too.  
 */
void
State::reset(void) {
	node_pool.clear();
#if 0
	expr_pool.clear();
	// BAH! clear() instantiates operator =, which requires assignability!
#else
	expr_pool.~expr_pool_type();
	new (&expr_pool) expr_pool_type();
#endif
	expr_graph_node_pool.clear();
	event_pool.clear();
	event_queue.clear();
	head_sentinel();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre expressions are already properly sized.  
 */
void
State::initialize(void) {
	for_each(node_pool.begin(), node_pool.end(), 
		mem_fun_ref(&node_type::initialize));
	for_each(expr_pool.begin(), expr_pool.end(), 
		mem_fun_ref(&expr_type::initialize));
	// the expr_graph_node_pool contains no stateful information.  
	while (!event_queue.empty()) {
		const event_placeholder_type next(event_queue.pop());
		event_pool.deallocate(next.event_index);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pre-allocates one element in node and expr pools, which are 1-indexed.
	NOTE: the event pool takes care of itself already.  
	\pre all pools are empty, as if just clear()ed.
	\post the pools have one null element at position 0.  
 */
void
State::head_sentinel(void) {
	node_pool.resize(FIRST_VALID_NODE);
	expr_pool.resize(FIRST_VALID_EXPR);
	expr_graph_node_pool.push_back(graph_node_type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Node accessor.
 */
const State::node_type&
State::get_node(const node_index_type i) const {
	INVARIANT(i);
	INVARIANT(i < node_pool.size());
	return node_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
State::node_type&
State::get_node(const node_index_type i) {
	INVARIANT(i);
	INVARIANT(i < node_pool.size());
	return node_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::check_node(const node_index_type i) const {
	const node_type& n(node_pool[i]);
	// check pull-up/dn if applicable
	const expr_index_type upi = n.pull_up_index;
	if (is_valid_expr_index(upi)) {
		const expr_type& e __ATTRIBUTE_UNUSED_CTOR__((expr_pool[upi]));
		assert(e.is_root());
		assert(e.direction());
		assert(e.parent == i);
	}
	const expr_index_type dni = n.pull_dn_index;
	if (is_valid_expr_index(dni)) {
		const expr_type& e __ATTRIBUTE_UNUSED_CTOR__((expr_pool[dni]));
		assert(e.is_root());
		assert(!e.direction());
		assert(e.parent == i);
	}
	// check fanout
	const size_t fs = n.fanout.size();
	size_t j = 0;
	for ( ; j < fs; ++j) {
		assert(expr_graph_node_pool[n.fanout[j]]
			.contains_node_fanin(i));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Double-checks parent-child relationships.  
	\param i index of the expression, must be < expr_pool.size().  
 */
void
State::check_expr(const expr_index_type i) const {
	const expr_type& e(expr_pool[i]);
	const graph_node_type& g(expr_graph_node_pool[i]);
	// check parent
	if (e.is_root()) {
		const node_type& n
			__ATTRIBUTE_UNUSED_CTOR__((node_pool[e.parent]));
		assert(n.get_pull_expr(e.direction()) == i);
	} else {
		// const Expr& pe(expr_pool[e.parent]);
		const graph_node_type& pg(expr_graph_node_pool[e.parent]);
		const graph_node_type::child_entry_type&
			pc __ATTRIBUTE_UNUSED_CTOR__((pg.children[g.offset]));
		assert(!pc.first);	// this is an expression, not node
		assert(pc.second == i);
	}
	// check children
	assert(e.size == g.children.size());
	size_t j = 0;
	for ( ; j<e.size; ++j) {
		const graph_node_type::child_entry_type& c(g.children[j]);
		if (c.first) {		// points to leaf node
			assert(node_pool[c.second].contains_fanout(i));
		} else {		// points to expression
			assert(expr_pool[c.second].parent == i);
			assert(expr_graph_node_pool[c.second].offset == j);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
event_index_type
State::allocate_event(void) {
	return event_pool.allocate();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre i is not already deallocated.  (not checked!)
 */
void
State::deallocate_event(const event_index_type i) {
	event_pool.deallocate(i);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
State::event_type&
State::get_event(const event_index_type i) {
	return event_pool[i];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
State::enqueue_event(const event_placeholder_type& e) {
	event_queue.push(e);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ni the canonically allocated global index of the bool node.
	\param val the new value to set the node.
	\param t the time at which the event should occur.  
	\return status: 0 is accepted, 1 is warning.  
 */
int
State::set_node_time(const node_index_type ni, const char val,
		const time_type t) {

	// we have ni = the canonically allocated index of the bool node
	// just look it up in the node_pool
	node_type& n(get_node(ni));
	const bool pending = n.pending_event();
	const char last_val = n.current_value();
/***
	If node is in the event queue already and is set to the 
	same value, then issue warning.  
***/
	if (pending && val == last_val) {
		const string objname(get_node_canonical_name(ni));
		cout << "WARNING: ignoring set_node on `" << objname <<
			"\' [interferes with pending event]" << endl;
		return ENQUEUE_WARNING;
	}
// If the value is the same as former value, then ignore it.
// What if delay differs?
	if (val == last_val) { return ENQUEUE_ACCEPT; }
// If node has pending even in queue already, warn and ignore.
	if (pending) {
		const string objname(get_node_canonical_name(ni));
		cout << "WARNING: pending value for node `" << objname <<
			"\'; ignoring request" << endl;
		return ENQUEUE_WARNING;
	}
// otherwise, enqueue the event.  
	const event_index_type ei = allocate_event();
	event_type& e(get_event(ei));
	e.node = ni;
	e.val = val;
	n.set_event(ei);
	enqueue_event(event_placeholder_type(t, ei));
	return ENQUEUE_ACCEPT;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Structural assertions.  
	TODO: run-time flag to enable/disable calls to this.  
 */
void
State::check_structure(void) const {
	STACKTRACE_VERBOSE;
{
	const expr_index_type exprs = expr_pool.size();
	INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	for ( ; i<exprs; ++i) {
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "checking Expr " << i << ":" << endl;
#endif
		check_expr(i);
	}
}
{
	const node_index_type nodes = node_pool.size();
	node_index_type j = FIRST_VALID_NODE;
	for ( ; j<nodes; ++j) {
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT << "checking Node " << j << ":" << endl;
#endif
		check_node(j);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i global node index.  
	\return string of the canonical node name.  
 */
string
State::get_node_canonical_name(const node_index_type i) const {
	INVARIANT(i);
	INVARIANT(i < node_pool.size());
	const state_manager& sm(mod.get_state_manager());
	const entity::footprint& topfp(mod.get_footprint());
	const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
	ostringstream oss;
	bp[i].dump_canonical_name(oss, topfp, sm);
	return oss.str();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
State::dump_struct(ostream& o) const {
{
	o << "Nodes: " << endl;
	const state_manager& sm(mod.get_state_manager());
	const entity::footprint& topfp(mod.get_footprint());
	const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
	const node_index_type nodes = node_pool.size();
	node_index_type i = FIRST_VALID_NODE;
	for ( ; i<nodes; ++i) {
		o << "node[" << i << "]: \"";
		bp[i].dump_canonical_name(o, topfp, sm);
		node_pool[i].dump_struct(o << "\" ") << endl;
	}
}
{
	o << "Expressions: " << endl;
	const expr_index_type exprs = expr_pool.size();
	INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	for ( ; i<exprs; ++i) {
		expr_pool[i].dump_struct(o << "expr[" << i << "]: ") << endl;
		expr_graph_node_pool[i].dump_struct(o << '\t') << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
ostream&
State::dump_state(ostream& o) const {
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This prints out the netlist of nodes and expressions
	in dot-form for visualization.  
 */
ostream&
State::dump_struct_dot(ostream& o) const {
	o << "digraph G {" << endl;
{
	const state_manager& sm(mod.get_state_manager());
	const entity::footprint& topfp(mod.get_footprint());
	const global_entry_pool<bool_tag>& bp(sm.get_pool<bool_tag>());
	o << "# nodes: " << endl;
	// box or plaintext
	o << "node [shape=box, fillcolor=white];" << endl;
	const node_index_type nodes = node_pool.size();
	node_index_type i = FIRST_VALID_NODE;
	for ( ; i<nodes; ++i) {
		ostringstream oss;
		oss << "NODE_" << i;
		const string& s(oss.str());
		o << s;
		bp[i].dump_canonical_name(o << "\t[label=\"", topfp, sm)
			<< "\"];" << endl;
		node_pool[i].dump_fanout_dot(o, s) << endl;
	}
}
{
	o << "# Expressions: " << endl;
	const expr_index_type exprs = expr_pool.size();
	INVARIANT(exprs == expr_graph_node_pool.size());
	expr_index_type i = FIRST_VALID_EXPR;
	for ( ; i<exprs; ++i) {
		o << "EXPR_" << i << "\t[label=\"" << i << "\", shape=";
		const expr_type& e(expr_pool[i]);
		e.dump_type_dot_shape(o) << "];" << endl;
		e.dump_parent_dot_edge(o << "EXPR_" << i << " -> ")
			<< ';'<< endl;
	}
}
	o << "}" << endl;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the event queue.  
 */
ostream&
State::dump_event_queue(ostream& o) const {
	typedef	vector<event_queue_type::value_type>	temp_type;
	typedef	temp_type::const_iterator		const_iterator;
	temp_type temp;
	event_queue.copy_to(temp);
	const_iterator i(temp.begin()), e(temp.end());
	o << "event queue:" << endl;
	for ( ; i!=e; ++i) {
		o << '\t' << i->time << '\t';
		const event_type& ev(event_pool[i->event_index]);
		o << get_node_canonical_name(ev.node) << " : " <<
			node_type::value_to_char[ev.val] << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints out nodes affected by the node argument.  
	Note: this only requires structural information, no stateful info.  
	TODO: check for replications.  
	TODO: including all OR combinations, or just the paths that
		include this node conjunctively?
 */
ostream&
State::dump_node_fanout(ostream& o, const node_index_type ni) const {
	typedef	node_type::fanout_array_type	fanout_array_type;
	typedef	fanout_array_type::const_iterator	const_iterator;
	STACKTRACE_VERBOSE;
#if DEBUG_FANOUT
	STACKTRACE_INDENT << "ni = " << ni << endl;
#endif
	const node_type& n(get_node(ni));
	const fanout_array_type& foa(n.fanout);
	const_iterator fi(foa.begin()), fe(foa.end());
	for ( ; fi!=fe; ++fi) {
		// for each leaf expression in the fanout list, 
		// trace up the propagation path to find the affected node.
		expr_index_type ei = *fi;
		const expr_type* e = &expr_pool[ei];
		while (!e->is_root()) {
#if DEBUG_FANOUT
			STACKTRACE_INDENT << "ei = " << ei << endl;
#endif
			ei = e->parent;
			e = &expr_pool[ei];
		}
#if DEBUG_FANOUT
		STACKTRACE_INDENT << "ei = " << ei << endl;
#endif
		// ei is an index to the expression whose parent is *node*.
		const node_index_type nr = e->parent;
		// nr is an index to the root *node*.
#if DEBUG_FANOUT
		STACKTRACE_INDENT << "nr = " << nr << endl;
#endif
		const node_type& no(get_node(nr));
		// track the direction of propagation (pull-up/dn)
		const bool dir = e->direction();
		// then print the entire fanin rule for that node, 
		const expr_index_type pi =
			(dir ? no.pull_up_index : no.pull_dn_index);
#if DEBUG_FANOUT
		STACKTRACE_INDENT << "pi = " << pi << endl;
#endif
		dump_subexpr(o, pi) << " -> ";
		o << get_node_canonical_name(nr) << (dir ? '+' : '-') << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursive expression printer.  
	Should be modeled after cflat's expression printer.  
 */
ostream&
State::dump_subexpr(ostream& o, const expr_index_type ei, 
		const char ptype) const {
	STACKTRACE_VERBOSE;
	INVARIANT(ei);
	INVARIANT(ei < expr_pool.size());
	const expr_type& e(expr_pool[ei]);
	const graph_node_type& g(expr_graph_node_pool[ei]);
	// can elaborate more on when parens are needed
	const bool need_parens = e.parenthesize(ptype);
	const char _type = e.to_prs_enum();
	if (e.is_not()) {
		o << '~';
	}
	const char* op = e.is_or() ? " | " : " & ";
	typedef	graph_node_type::const_iterator		const_iterator;
	const_iterator ci(g.begin()), ce(g.end());
	if (need_parens) {
		o << '(';
	}
	// peel out first iteration for infix printing
	if (ci->first) {
		o << get_node_canonical_name(ci->second);
	} else {
		dump_subexpr(o, ci->second, _type);
	}
	if (g.children.size() >= 1) {
	for (++ci; ci!=ce; ++ci) {
		o << op;
		if (ci->first) {
			o << get_node_canonical_name(ci->second);
		} else {
			dump_subexpr(o, ci->second, _type);
		}
	}
	}
	if (need_parens) {
		o << ')';
	}
	return o;
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

