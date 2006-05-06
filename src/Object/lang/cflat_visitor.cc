/**
	\file "Object/lang/cflat_visitor.cc"
	$Id: cflat_visitor.cc,v 1.6 2006/05/06 04:18:40 fang Exp $
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

/**
	Possibly move class definition into header, leaving methods
	in this file.  
 */
class cflat_visitor::expr_pool_setter {
private:
	cflat_visitor&                          cfv;
public:
	expr_pool_setter(cflat_visitor& _cfv, const footprint& _fp) :
			cfv(_cfv) {
		cfv.expr_pool = &_fp.get_expr_pool();
	}

	~expr_pool_setter() {
		cfv.expr_pool = NULL;
	}
};      // end struct expr_pool_setter

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
	for ( ; pid < plim; ++pid) {
		entity::production_rule_substructure::accept(
			proc_entry_pool[pid], *this);
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

