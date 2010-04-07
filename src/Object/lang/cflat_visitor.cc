/**
	\file "Object/lang/cflat_visitor.cc"
	$Id: cflat_visitor.cc,v 1.14 2010/04/07 00:12:50 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <algorithm>
#include "Object/def/footprint.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/lang/CHP_visitor.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/global_entry.tcc"
#include "Object/global_channel_entry.h"
#include "Object/traits/instance_traits.h"
#include "common/TODO.h"
#include "util/visitor_functor.h"
#include "util/index_functor.h"
#include "util/compose.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
namespace PRS {
using ADS::unary_compose_void;
using util::visitor_ref;
using util::typed_visitor_ref;
using util::index_functor;
using std::for_each;
//=============================================================================
// struct cflat_visitor class definition

//=============================================================================
void
cflat_visitor::visit(const entity::footprint& f) {
	STACKTRACE_VERBOSE;
	FINISH_ME_EXIT(Fang);
	// visit PRS
	// visit CHP
	// visit SPEC
}

//=============================================================================
/**
	Default traversal for cflat_visitor over the PRS::footprint.  
	PRS::footprint has rules and macros as immediate subobjects.  
 */
void
cflat_visitor::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
	for_each(f.rule_pool.begin(), f.rule_pool.end(), visitor_ref(*this));
	for_each(f.macro_pool.begin(), f.macro_pool.end(), visitor_ref(*this));
#if 0
	for_each(f.invariant_pool.begin(), f.invariant_pool.end(), 
		unary_compose_void(
		typed_visitor_ref<this_type, const footprint_expr_node>(*this),
			index_functor(f.expr_pool)));
#else
	// defer invariant expression visiting to subclasses
#endif
}

//=============================================================================
void
cflat_visitor::visit(const SPEC::footprint& f) {
	STACKTRACE_VERBOSE;
	for_each(f.begin(), f.end(), visitor_ref(*this));
}

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

