/**
	\file "Object/lang/CHP_visitor.cc"
	$Id: CHP_visitor.cc,v 1.4.24.5 2010/02/12 18:20:30 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "Object/lang/CHP_visitor.h"
#include "Object/def/footprint.h"
#include "Object/lang/CHP.h"
#include "Object/global_entry.h"
#include "Object/inst/state_instance.h"
#include "common/ICE.h"

namespace HAC {
namespace entity {
namespace CHP {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	No-op.
 */
void
chp_visitor::visit(const entity::state_manager&) {
	// should be overridden
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_visitor::visit(const GLOBAL_ENTRY<process_tag>& p) {
//	cflat_context_visitor::visit(p);	// optional: PRS, SPEC
// TODO: will need this once CHP interacts with SPEC directives
	chp_visitor& v(*this);
	const entity::footprint* const f(p._frame._footprint);
	NEVER_NULL(f);
if (f->has_chp_footprint()) {
	const CHP::concurrent_actions& cfp(f->get_chp_footprint());
	cfp.accept(v);
}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
/**
	No-op.
 */
void
chp_visitor::visit(const entity::PRS::footprint&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
chp_visitor::visit(const footprint_rule&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
chp_visitor::visit(const footprint_expr_node&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
chp_visitor::visit(const footprint_macro&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
chp_visitor::visit(const entity::SPEC::footprint_directive&) { }
#endif

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

