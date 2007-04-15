/**
	\file "Object/lang/CHP_visitor.cc"
	$Id: CHP_visitor.cc,v 1.3 2007/04/15 05:52:21 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "Object/lang/CHP_visitor.h"
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

