/**
	\file "sim/prsim/State.cc"
	Implementation of prsim simulator state.  
	$Id: State.cc,v 1.1.2.4 2006/01/04 08:42:14 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <algorithm>
#include <functional>
#include "sim/prsim/State.h"
#include "sim/prsim/ExprAlloc.h"
#include "util/list_vector.tcc"
#include "Object/module.h"
#include "Object/state_manager.h"
#include "Object/traits/classification_tags.h"
#include "Object/global_entry.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity { }

namespace SIM {
namespace PRSIM {
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
		event_pool(), event_queue() {
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
		mem_fun_ref(&Node::initialize));
	for_each(expr_pool.begin(), expr_pool.end(), 
		mem_fun_ref(&Expr::initialize));
	while (!event_queue.empty()) {
		const EventPlaceholder next(event_queue.pop());
		event_pool.deallocate(next.event_index);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pre-allocates one element in node and expr pools, which are 1-indexed.
	\pre all pools are empty, as if just clear()ed.
	\post the pools have one null element at position 0.  
 */
void
State::head_sentinel(void) {
	node_pool.resize(1);
	expr_pool.resize(1);
	expr_graph_node_pool.push_back(ExprGraphNode());
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
	node_index_type i = 1;
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
	expr_index_type i = 1;
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

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

