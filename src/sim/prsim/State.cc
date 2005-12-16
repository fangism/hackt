/**
	\file "sim/prsim/State.cc"
	Implementation of prsim simulator state.  
	$Id: State.cc,v 1.1.2.2 2005/12/16 02:43:20 fang Exp $
 */

#include <algorithm>
#include <functional>
#include "sim/prsim/State.h"
#include "util/list_vector.tcc"
#include "Object/module.h"
#include "Object/state_manager.h"
#include "Object/traits/classification_tags.h"
#include "Object/global_entry.h"

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
//=============================================================================
// class State method definitions

/**
	TODO: pick reasonable chunk size for expr_pool.  
 */
State::State() : node_pool(), expr_pool(), expr_graph_node_pool(),
		event_pool(), event_queue() {
	expr_graph_node_pool.set_chunk_size(1024);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Allocates simulation state, given a module.
	TODO: do this work in module?
	TODO: add support for top-level PRS (outside of procs)
	\param m the expanded module object.
	\pre m must already be past the allcoate phase.  
 */
State::State(const entity::module& m) : 
		node_pool(), expr_pool(), expr_graph_node_pool(),
		event_pool(), event_queue() {
	const state_manager& sm(m.get_state_manager());
	const global_entry_pool<bool_tag>&
		bool_pool(sm.get_pool<bool_tag>());
	// recall, the global node pool is 1-indexed because entry 0 is null
	// we mirror this in our own node state pool, by allocating
	// the same number of elements.  
	node_pool.resize(bool_pool.size());
	// then go through all processes to generate expressions
#if 0
	const global_entry_pool<process_tag>&
		proc_pool(sm.get_pool<process_tag>());
#endif
	// create a temporary vector of expressions using list_vector first
	// then transfer them over to the expr_pool

	expr_graph_node_pool.set_chunk_size(1024);
	temp_expr_pool_type build_exprs;
	build_exprs.set_chunk_size(1024);
	// use a cflat-prs-like pass to construct the expression netlist
	// got a walker? and prs_expr_visitor?

	// the construct the top-down structure offline.  
}

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

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

