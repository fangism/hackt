/**
	\file "Object/lang/CHP_visitor.cc"
	$Id: CHP_visitor.cc,v 1.4 2008/10/11 06:35:12 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "Object/lang/CHP_visitor.h"
#include "Object/def/footprint.h"
#include "Object/lang/CHP.h"
#include "Object/global_entry.h"
#include "common/ICE.h"

namespace HAC {
namespace entity {
namespace CHP {

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No-op.
 */
void
chp_visitor::visit(const entity::state_manager&) {
	// should be overridden
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chp_visitor::visit(const global_entry<process_tag>& p) {
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//=============================================================================
}	// end namespace CHP
}	// end namespace entity
}	// end namespace HAC

