/**
	\file "Object/lang/cflat_visitor.cc"
	$Id: cflat_visitor.cc,v 1.9 2008/10/11 06:35:13 fang Exp $
 */

#include <algorithm>
#include "util/visitor_functor.h"
#include "Object/def/footprint.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/lang/CHP_visitor.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/state_manager.h"
#include "Object/global_entry.tcc"
#include "Object/global_channel_entry.h"
#include "Object/traits/instance_traits.h"

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
template <class Tag>
void
cflat_visitor::__default_visit(const global_entry<Tag>& e) {
//	e.accept(*this);
}

#define	DEFINE_VISIT_GLOBAL_ENTRY(Tag)					\
void									\
cflat_visitor::visit(const global_entry<Tag>& e) {			\
	__default_visit(e);						\
}

DEFINE_VISIT_GLOBAL_ENTRY(channel_tag)
DEFINE_VISIT_GLOBAL_ENTRY(int_tag)
DEFINE_VISIT_GLOBAL_ENTRY(enum_tag)
DEFINE_VISIT_GLOBAL_ENTRY(bool_tag)

#undef	DEFINE_VISIT_GLOBAL_ENTRY

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generic PRS footprint traversal.  
	Order of traversal was chosen somewhat arbitrarily.  
	Dynamic-cast (cross-cast) is needed because PRS::cflat_visitor
	is not derived from cflat_context... should it be?
 */
void
cflat_visitor::visit(const global_entry<process_tag>& e) {
	cflat_visitor& v(*this);
	const entity::footprint* const f(e._frame._footprint);
	NEVER_NULL(f);
	const PRS::footprint&
		pfp(f->get_prs_footprint());
// traverse production rules
	const cflat_context::footprint_frame_setter
		tmp(IS_A(cflat_context&, v), e._frame);
	pfp.accept(v);

// traverse SPEC directives
	const SPEC::footprint&
		sfp(f->get_spec_footprint());
	sfp.accept(v);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default state_manager traversal. 
 */
void
cflat_visitor::visit(const state_manager& sm) {
	sm.accept(*this);
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

