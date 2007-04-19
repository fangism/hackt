/**
	\file "Object/lang/cflat_visitor.cc"
	$Id: cflat_visitor.cc,v 1.7 2007/04/19 03:13:39 fang Exp $
 */

#include <algorithm>
#include "util/visitor_functor.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/state_manager.h"
#include "Object/global_entry.tcc"
#include "Object/traits/proc_traits.h"

namespace HAC {
namespace entity {
namespace PRS {
using util::visitor_ref;
using std::for_each;
//=============================================================================
// struct cflat_visitor class definition

cflat_visitor::expr_pool_setter::expr_pool_setter(
		cflat_visitor& _cfv, const footprint& _fp) :
		cfv(_cfv) {
	cfv.expr_pool = &_fp.get_expr_pool();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cflat_visitor::expr_pool_setter::expr_pool_setter(
		cflat_visitor& _cfv, const cflat_visitor& _s) :
		cfv(_cfv) {
	cfv.expr_pool = _s.expr_pool;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
cflat_visitor::expr_pool_setter::~expr_pool_setter() {
	cfv.expr_pool = NULL;
}

//=============================================================================
/**
	Default state_manager traversal. 
 */
void
cflat_visitor::visit(const state_manager& sm) {
	size_t pid = 1;		// 0-indexed, but 0th entry is null
	// const global_entry_pool<process_tag>& proc_entry_pool(sm);
	const global_entry_pool<process_tag>&
		proc_entry_pool(sm.get_pool<process_tag>());
	// Could re-write in terms of begin() and end() iterators.  
	const size_t plim = proc_entry_pool.size();
try {
	for ( ; pid < plim; ++pid) {
		entity::production_rule_substructure::accept(
			proc_entry_pool[pid], *this);
	}
} catch (...) {
	cerr << "FATAL: error during processing of process id " << pid
		<< "." << endl;
#if 0
	cerr << "\tinstance: ";
	proc_entry_pool[pid].dump_canonical_name(cerr, topfp, sm) << endl;
#endif
	// topfp footprint is not available here, pass pid in exception
	throw process_exception(pid);
}
}

//=============================================================================
/**
	Default traversal for cflat_visitor over the PRS::footprint.  
	PRS::footprint has rules and macros as immediate subobjects.  
 */
void
cflat_visitor::visit(const footprint& f) {
	const expr_pool_setter temp(*this, f);	// will expire end of scope
	for_each(f.rule_pool.begin(), f.rule_pool.end(), visitor_ref(*this));
	for_each(f.macro_pool.begin(), f.macro_pool.end(), visitor_ref(*this));
}

//=============================================================================
void
cflat_visitor::visit(const SPEC::footprint& f) {
	for_each(f.begin(), f.end(), visitor_ref(*this));
}

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

