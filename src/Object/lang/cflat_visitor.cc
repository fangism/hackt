/**
	\file "Object/lang/cflat_visitor.cc"
	$Id: cflat_visitor.cc,v 1.13 2010/04/02 22:18:38 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <algorithm>
#include "Object/def/footprint.h"
#include "Object/lang/cflat_visitor.h"
#include "Object/lang/CHP_visitor.h"
#include "Object/lang/PRS_footprint.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/state_manager.tcc"	// for __accept<Tag>()
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

#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
cflat_visitor::expr_pool_setter::expr_pool_setter(
		cflat_visitor& _cfv, const PRS_footprint_expr_pool_type& _p) :
		cfv(_cfv) {
	cfv.expr_pool = &_p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
#endif

//=============================================================================
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
template <class Tag>
void
cflat_visitor::__default_visit(const GLOBAL_ENTRY<Tag>& e) {
//	e.accept(*this);
}

#define	DEFINE_VISIT_GLOBAL_ENTRY(Tag)					\
void									\
cflat_visitor::visit(const GLOBAL_ENTRY<Tag>& e) {			\
	STACKTRACE_VERBOSE;						\
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
cflat_visitor::visit(const GLOBAL_ENTRY<process_tag>& e) {
	STACKTRACE_VERBOSE;
	cflat_visitor& v(*this);
	const entity::footprint* const f(e._frame._footprint);
	NEVER_NULL(f);
	const PRS::footprint&
		pfp(f->get_prs_footprint());
// traverse production rules
	const cflat_context::footprint_frame_setter
		tmp(IS_A(cflat_context&, v), e._frame);
	pfp.accept(v);
// TODO: traverse CHP?
// traverse SPEC directives
	const SPEC::footprint&
		sfp(f->get_spec_footprint());
	sfp.accept(v);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if MEMORY_MAPPED_GLOBAL_ALLOCATION
void
cflat_visitor::visit(const entity::footprint& f) {
	STACKTRACE_VERBOSE;
	FINISH_ME_EXIT(Fang);
	// visit PRS
	// visit CHP
	// visit SPEC
}
#else
/**
	Default state_manager traversal. 
 */
void
cflat_visitor::visit(const state_manager& sm) {
	STACKTRACE_VERBOSE;
	cflat_visitor& v(*this);
	sm.__accept<process_tag>(v);
	sm.__accept<channel_tag>(v);
#if ENABLE_DATASTRUCTS
	sm.__accept<datastruct_tag>(v);
#endif
	sm.__accept<enum_tag>(v);
	sm.__accept<int_tag>(v);
	sm.__accept<bool_tag>(v);
}
#endif

//=============================================================================
/**
	Default traversal for cflat_visitor over the PRS::footprint.  
	PRS::footprint has rules and macros as immediate subobjects.  
 */
void
cflat_visitor::visit(const footprint& f) {
	STACKTRACE_VERBOSE;
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	const expr_pool_setter temp(*this, f);	// will expire end of scope
#endif
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

