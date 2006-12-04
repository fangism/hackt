/**
	\file "sim/chpsim/StateConstructor.cc"
	$Id: StateConstructor.cc,v 1.1.2.1 2006/12/04 09:55:59 fang Exp $
 */

#include "sim/chpsim/StateConstructor.h"
#include "util/visitor_functor.h"
#include "Object/global_entry.tcc"
#include "Object/lang/CHP_footprint.h"

namespace HAC {
namespace SIM {
namespace CHPSIM {
using entity::process_tag;
using entity::global_entry_pool;

//=============================================================================
// class StateConstructor method definitions

StateConstructor::~StateConstructor() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Copy-modified from entity::cflat_visitor::visit()
 */
void
StateConstructor::visit(const state_manager& _sm) {
	size_t pid = 1;		// 0-indexed, but 0th entry is null
	// const global_entry_pool<process_tag>& proc_entry_pool(sm);
	const global_entry_pool<process_tag>&
		proc_entry_pool(_sm.get_pool<process_tag>());
	// Could re-write in terms of begin() and end() iterators.  
	const size_t plim = proc_entry_pool.size();
	for ( ; pid < plim; ++pid) {
		// visit CHP instead
		entity::CHP_substructure<true>::accept(
			proc_entry_pool[pid], *this);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const entity::PRS::footprint& f) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const footprint_rule&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const footprint_expr_node&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const footprint_macro&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
StateConstructor::visit(const entity::SPEC::footprint_directive&) { }

//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

