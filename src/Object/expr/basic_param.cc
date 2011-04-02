/**
	\file "Object/expr/basic_param.cc"
	Class definitions for basic parameter expression types.  
	NOTE: This file was shaved down from the original 
		"Object/art_object_expr.cc" for revision history tracking.  
 	$Id: basic_param.cc,v 1.32 2011/04/02 01:45:57 fang Exp $
 */

// flags for controlling conditional compilation, mostly for debugging
#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS				(0 && ENABLE_STACKTRACE)
#define	STACKTRACE_PERSISTENTS				(0 && ENABLE_STACKTRACE)

//=============================================================================
// start of static initializations
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "Object/expr/pint_const.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/pstring_const.h"
#include "Object/expr/const_index.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/const_range.h"
#include "Object/expr/const_range_list.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/expr_visitor.h"
#include "Object/inst/param_value_collection.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/string_traits.h"
#include "Object/traits/pint_traits.h"	// needed for assign
#include "Object/traits/pbool_traits.h"	// needed for assign
#include "Object/traits/preal_traits.h"	// needed for assign
#include "Object/traits/pstring_traits.h"
#include "Object/type/data_type_reference.h"
#include "Object/unroll/expression_assignment.h"
#include "Object/persistent_type_hash.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
	// for aggregate_value_references' base
#include "Object/ref/aggregate_meta_value_reference.h"
#include "Object/ref/meta_value_reference.h"
#include "Object/type/canonical_generic_datatype.h"
#include "Object/nonmeta_context.h"
#include "Object/unroll/unroll_context.h"

#include "common/TODO.h"

#include "util/what.tcc"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"
#include "util/memory/list_vector_pool.tcc"
#include "util/persistent_object_manager.tcc"

//=============================================================================
namespace util {
using HAC::entity::pint_const;
using HAC::entity::pbool_const;
using HAC::entity::preal_const;
using HAC::entity::pstring_const;

SPECIALIZE_UTIL_WHAT(pint_const, "pint-const")
SPECIALIZE_UTIL_WHAT(pbool_const, "pbool-const")
SPECIALIZE_UTIL_WHAT(preal_const, "preal-const")
SPECIALIZE_UTIL_WHAT(pstring_const, "pstring-const")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pint_const, CONST_PINT_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pbool_const, CONST_PBOOL_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	preal_const, CONST_PREAL_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pstring_const, CONST_PSTRING_TYPE_KEY, 0)

namespace memory {
using HAC::entity::const_param;
using HAC::entity::pint_expr;
using HAC::entity::pbool_expr;
using HAC::entity::preal_expr;
using HAC::entity::pstring_expr;
	// pool-allocator managed types that are safe to destroy lazily
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(HAC::entity::pbool_const)
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(HAC::entity::pint_const)
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(HAC::entity::preal_const)
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(HAC::entity::pstring_const)

// explicit template instantiations, needed for -O3
template class count_ptr<const pint_expr>;
template class count_ptr<const pbool_expr>;
template class count_ptr<const preal_expr>;
template class count_ptr<const pint_const>;
template class count_ptr<const pbool_const>;
template class count_ptr<const preal_const>;
template class count_ptr<const const_param>;
template class count_ptr<const pstring_expr>;
template class count_ptr<const pstring_const>;
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
using util::write_value;
using util::read_value;
using util::persistent_traits;
#include "util/using_ostream.h"

#if DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE && ENABLE_STACKTRACE
REQUIRES_STACKTRACE_STATIC_INIT
// the robust list_vector_pool requires this.  
#endif

//=============================================================================
// class param_expr method_definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs an expression assignment object of the appropriate
	value type.  
	Wrapped calls to private constructors.  
	\param p the right-hand-side expression of the assignment object.  
	\return newly constructed and initialized assignment object.  
	NOTE: the check for may_be_initialized is optional, just an
		attempt to catch obvious errors earlier.  
 */
count_ptr<param_expression_assignment>
param_expr::make_param_expression_assignment(
		const count_ptr<const param_expr>& p) {
	NEVER_NULL(p);
	return p->make_param_expression_assignment_private(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constructs an aggregate value reference of the appropriate type.  
	NOTE: it is the caller's job to set the mode:
		construction vs. concatenation.  
	NOTE: we already add the first element to the aggregate reference.
 */
count_ptr<aggregate_meta_value_reference_base>
param_expr::make_aggregate_meta_value_reference(
		const count_ptr<const param_expr>& p) {
	typedef	count_ptr<aggregate_meta_value_reference_base>	return_type;
	NEVER_NULL(p);
	const return_type
		ret(p->make_aggregate_meta_value_reference_private(p));
	return ret;
}

//-----------------------------------------------------------------------------
// class const_param method definitions

ostream&
const_param::dump(ostream& o) const {
	return dump(o, expr_dump_context());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
const_param::dump_nonmeta(ostream& o) const {
	return dump(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return by default 0.0 unless overridden
 */
preal_value_type
const_param::to_real_const(void) const {
	return 0.0;
}

//-----------------------------------------------------------------------------
// class pbool_expr method definitions

#if 0
pbool_expr::~pbool_expr() {
	STACKTRACE_DTOR("~pbool_expr()");
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	When pint is interpreted as an int, in non-meta language...
 */
count_ptr<const data_type_reference>
pbool_expr::get_unresolved_data_type_ref(void) const {
	return bool_traits::nonmeta_data_type_ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_datatype
pbool_expr::get_resolved_data_type_ref(const unroll_context&) const {
	return bool_traits::nonmeta_data_type_ptr->make_canonical_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_expr::may_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pbool_expr::may_be_equivalent_generic()");
	const pbool_expr* b = IS_A(const pbool_expr*, &p);
	if (b) {
		if (is_static_constant() && b->is_static_constant())
			return static_constant_value() ==
				b->static_constant_value();
		else	return true;
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_expr::must_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pbool_expr::must_be_equivalent_generic()");
	const pbool_expr* b = IS_A(const pbool_expr*, &p);
	if (b) {
#if 0
		if (is_static_constant() && b->is_static_constant())
			return static_constant_value() ==
				b->static_constant_value();
		// else check template formals?  more cases needed
		else	return false;
#else
		return must_be_equivalent(*b);
#endif
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
 */
count_ptr<const const_param>
pbool_expr::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new pbool_const(static_constant_value()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pbool_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const param_expr>& b) const {
	return unroll_resolve_rvalues(c, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't expect to be called, meta-expressions should've been
	resolved and substituted by create phase.  
 */
count_ptr<const const_param>
pbool_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& b) const {
	return __nonmeta_resolve_rvalue(c, b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_const>
pbool_expr::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& b) const {
	const unroll_context uc(c);
	return __unroll_resolve_rvalue(uc, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const bool_expr>
pbool_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const bool_expr>& b) const {
	return unroll_resolve_copy(c, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Forwarding function.  
 */
count_ptr<const param_expr>
pbool_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const param_expr>& p) const {
	return substitute_default_positional_parameters(f, e, 
		p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<param_expression_assignment>
pbool_expr::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	typedef	assignment_ptr_type		return_type;
	INVARIANT(p == this);
	return return_type(
		new pbool_expression_assignment(p.is_a<const this_type>()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<aggregate_meta_value_reference_base>
pbool_expr::make_aggregate_meta_value_reference_private(
		const count_ptr<const param_expr>& p) const {
	const count_ptr<const this_type> pb(p.is_a<const this_type>());
	NEVER_NULL(pb);
	const count_ptr<aggregate_meta_value_reference<tag_type> >
		ret(new aggregate_meta_value_reference<tag_type>);
	ret->append_meta_value_reference(pb);
	return ret;
}

//-----------------------------------------------------------------------------
// class pint_expr method definitions

#if 0
pint_expr::~pint_expr() {
	STACKTRACE_DTOR("~pint_expr()");
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	When pint is interpreted as an int, in non-meta language...
	\returns int<0> to signal something special
 */
count_ptr<const data_type_reference>
pint_expr::get_unresolved_data_type_ref(void) const {
	return int_traits::nonmeta_data_type_ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: write a built_in_resolved_type for pint.
 */
canonical_generic_datatype
pint_expr::get_resolved_data_type_ref(const unroll_context&) const {
	return int_traits::nonmeta_data_type_ptr->make_canonical_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_expr::may_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pint_expr::may_be_equivalent_generic()");
	const pint_expr* i = IS_A(const pint_expr*, &p);
	if (i) {
		if (is_static_constant() && i->is_static_constant())
			return static_constant_value() ==
				i->static_constant_value();
		else	return true;
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_expr::must_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pint_expr::must_be_equivalent_generic()");
	const pint_expr* i = IS_A(const pint_expr*, &p);
	if (i) {
#if 0
		if (is_static_constant() && i->is_static_constant())
			return static_constant_value() ==
				i->static_constant_value();
		else	return false;
#else
		return must_be_equivalent(*i);
#endif
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
 */
count_ptr<const const_param>
pint_expr::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new pint_const(static_constant_value()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pint_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const param_expr>& b) const {
	return unroll_resolve_rvalues(c, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const int_expr>
pint_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const int_expr>& b) const {
	return unroll_resolve_copy(c, b.is_a<const this_type>());
}

/**
	\return resolved constant index expression, or NULL if error.  
 */
count_ptr<const const_index>
pint_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const meta_index_expr>& b) const {
	return unroll_resolve_copy(c, b.is_a<const this_type>())
		.is_a<const const_index>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need this as a final unique overrider of virtual grandparent.  
 */
count_ptr<const nonmeta_index_expr_base>
pint_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const nonmeta_index_expr_base>& b) const {
	return unroll_resolve_copy(c, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't expect to be called, meta-expressions should've been
	resolved and substituted by create phase.  
 */
count_ptr<const const_param>
pint_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const int_expr>& b) const {
	return __nonmeta_resolve_rvalue(c, b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pint_const>
pint_expr::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const int_expr>& b) const {
	const unroll_context uc(c);
	return __unroll_resolve_rvalue(uc, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Forwarding function.  
 */
count_ptr<const param_expr>
pint_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const param_expr>& p) const {
	return substitute_default_positional_parameters(f, e, 
		p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Forwarding function.  
 */
count_ptr<const meta_index_expr>
pint_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const meta_index_expr>& p) const {
	return substitute_default_positional_parameters(f, e, 
		p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<param_expression_assignment>
pint_expr::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	typedef	assignment_ptr_type		return_type;
	INVARIANT(p == this);
	return return_type(
		new pint_expression_assignment(p.is_a<const this_type>()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<aggregate_meta_value_reference_base>
pint_expr::make_aggregate_meta_value_reference_private(
		const count_ptr<const param_expr>& p) const {
	const count_ptr<const this_type> pb(p.is_a<const this_type>());
	NEVER_NULL(pb);
	const count_ptr<aggregate_meta_value_reference<tag_type> >
		ret(new aggregate_meta_value_reference<tag_type>);
	ret->append_meta_value_reference(pb);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return deep copy of resolve constant integer value, 
	if it is successfully resolved.  
 */
count_ptr<const_index>
pint_expr::unroll_resolve_index(const unroll_context& c) const {
	STACKTRACE("pint_expr::unroll_resolve_index()");
	typedef count_ptr<const_index> return_type;
	value_type i;
	return (unroll_resolve_value(c, i).good) ? 
		return_type(new pint_const(i)) :
		return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_expr::must_be_equivalent_index(const meta_index_expr& i) const {
	const pint_expr* const p = IS_A(const pint_expr*, &i);
	if (p) {
		return must_be_equivalent(*p);
	} else {
		return false;
	}
}

//-----------------------------------------------------------------------------
// class preal_expr method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	When pint is interpreted as an int, in non-meta language...
 */
count_ptr<const data_type_reference>
preal_expr::get_unresolved_data_type_ref(void) const {
#if 1
	FINISH_ME_EXIT(Fang);
	return count_ptr<const data_type_reference>(NULL);
#else
	return real_traits::nonmeta_data_type_ptr;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Shouldn't be called, don't have nonmeta reals yet...
 */
canonical_generic_datatype
preal_expr::get_resolved_data_type_ref(const unroll_context&) const {
	FINISH_ME_EXIT(Fang);
	return canonical_generic_datatype();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_expr::may_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("preal_expr::may_be_equivalent_generic()");
	const preal_expr* b = IS_A(const preal_expr*, &p);
	if (b) {
		if (is_static_constant() && b->is_static_constant())
			return static_constant_value() ==
				b->static_constant_value();
		else	return true;
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_expr::must_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("preal_expr::must_be_equivalent_generic()");
	const preal_expr* b = IS_A(const preal_expr*, &p);
	if (b) {
		return must_be_equivalent(*b);
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre must satisfy is_static_constant.  
	For use with const_param_expr_list.  
 */
count_ptr<const const_param>
preal_expr::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new preal_const(static_constant_value()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
preal_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const param_expr>& b) const {
	return unroll_resolve_rvalues(c, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const real_expr>
preal_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const real_expr>& b) const {
	return unroll_resolve_copy(c, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't expect to be called, meta-expressions should've been
	resolved and substituted by create phase.  
 */
count_ptr<const const_param>
preal_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const real_expr>& b) const {
	return __nonmeta_resolve_rvalue(c, b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const preal_const>
preal_expr::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const real_expr>& b) const {
	const unroll_context uc(c);
	return __unroll_resolve_rvalue(uc, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Forwarding function.  
 */
count_ptr<const param_expr>
preal_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const param_expr>& p) const {
	return substitute_default_positional_parameters(f, e, 
		p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<param_expression_assignment>
preal_expr::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	typedef	assignment_ptr_type		return_type;
	INVARIANT(p == this);
	return return_type(
		new preal_expression_assignment(p.is_a<const this_type>()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<aggregate_meta_value_reference_base>
preal_expr::make_aggregate_meta_value_reference_private(
		const count_ptr<const param_expr>& p) const {
	const count_ptr<const this_type> pb(p.is_a<const this_type>());
	NEVER_NULL(pb);
	const count_ptr<aggregate_meta_value_reference<tag_type> >
		ret(new aggregate_meta_value_reference<tag_type>);
	ret->append_meta_value_reference(pb);
	return ret;
}

//=============================================================================
// class param_expr_collective method defintions

#if 0
param_expr_collective::param_expr_collective() : param_expr(), elist() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
param_expr_collective::~param_expr_collective() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
param_expr_collective::what(ostream& o) const {
	return o << "param-expr-collective";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
string
param_expr_collective::hash_string(void) const {
	string ret("{");
	list<excl_ptr<param_expr> >::const_iterator i(elist.begin());
	for ( ; i!=elist.end(); i++) {
		const never_ptr<const param_expr> p(*i);
		NEVER_NULL(p);
		ret += p->hash_string();
		ret += ",";
	}
	ret += "}";
	return ret;
}
#endif

//-----------------------------------------------------------------------------
// class pstring_expr method definitions

#if 0
pstring_expr::~pstring_expr() {
	STACKTRACE_DTOR("~pstring_expr()");
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	When pint is interpreted as an int, in non-meta language...
 */
count_ptr<const data_type_reference>
pstring_expr::get_unresolved_data_type_ref(void) const {
	return string_traits::nonmeta_data_type_ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_datatype
pstring_expr::get_resolved_data_type_ref(const unroll_context&) const {
	return string_traits::nonmeta_data_type_ptr->make_canonical_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pstring_expr::may_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pstring_expr::may_be_equivalent_generic()");
	const pstring_expr* b = IS_A(const pstring_expr*, &p);
	if (b) {
		if (is_static_constant() && b->is_static_constant())
			return static_constant_value() ==
				b->static_constant_value();
		else	return true;
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pstring_expr::must_be_equivalent_generic(const param_expr& p) const {
	STACKTRACE("pstring_expr::must_be_equivalent_generic()");
	const pstring_expr* b = IS_A(const pstring_expr*, &p);
	if (b) {
#if 0
		if (is_static_constant() && b->is_static_constant())
			return static_constant_value() ==
				b->static_constant_value();
		// else check template formals?  more cases needed
		else	return false;
#else
		return must_be_equivalent(*b);
#endif
	}
	else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
 */
count_ptr<const const_param>
pstring_expr::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new pstring_const(static_constant_value()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pstring_expr::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const param_expr>& b) const {
	return unroll_resolve_rvalues(c, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't expect to be called, meta-expressions should've been
	resolved and substituted by create phase.  
 */
count_ptr<const const_param>
pstring_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const string_expr>& b) const {
	return __nonmeta_resolve_rvalue(c, b);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pstring_const>
pstring_expr::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const string_expr>& b) const {
	const unroll_context uc(c);
	return __unroll_resolve_rvalue(uc, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const string_expr>
pstring_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const string_expr>& b) const {
	return unroll_resolve_copy(c, b.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Forwarding function.  
 */
count_ptr<const param_expr>
pstring_expr::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const param_expr>& p) const {
	return substitute_default_positional_parameters(f, e, 
		p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<param_expression_assignment>
pstring_expr::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	typedef	assignment_ptr_type		return_type;
	INVARIANT(p == this);
	return return_type(
		new pstring_expression_assignment(p.is_a<const this_type>()));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<aggregate_meta_value_reference_base>
pstring_expr::make_aggregate_meta_value_reference_private(
		const count_ptr<const param_expr>& p) const {
	const count_ptr<const this_type> pb(p.is_a<const this_type>());
	NEVER_NULL(pb);
	const count_ptr<aggregate_meta_value_reference<tag_type> >
		ret(new aggregate_meta_value_reference<tag_type>);
	ret->append_meta_value_reference(pb);
	return ret;
}

//=============================================================================
// class pint_const method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
	ALERT: we allocate one of these during the static initialization
	of built-ins, we may need a safeguard to ensure that
	the allocator is initialized first!
***/
// LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(pint_const, 1024)
CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(pint_const)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pint_const)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private inline empty constructor, uninitialized.
 */
inline
pint_const::pint_const() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const::~pint_const() {
	STACKTRACE_DTOR("~pint_const()");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The expr_dump_context parameter is unused.  
 */
ostream&
pint_const::dump(ostream& o, const expr_dump_context&) const {
	return o << val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A pint const is scalar, thus dimension list is empty.
 */
const_range_list
pint_const::static_constant_dimensions(void) const {
	return const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
	Just copy-constructs.  
 */
count_ptr<const const_param>
pint_const::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new pint_const(val));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_const::is_true(void) const {
	return val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_const::negative(void) const {
	return val < 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_const::operator == (const const_range& c) const {
	return (val == c.first) && (val == c.second);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pint_const::must_be_equivalent(const pint_expr& p) const {
	STACKTRACE("pint_const::must_be_equivalent()");
	return p.is_static_constant() && (val == p.static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const::value_type
pint_const::lower_bound(void) const {
	return val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
pint_const::value_type
pint_const::upper_bound(void) const {
	return val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pint_const::resolve_value(value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return deep copy of this constant integer, always succeeds.  
 */
count_ptr<const_index>
pint_const::resolve_index(void) const {
	return count_ptr<const_index>(new pint_const(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return empty range list signifying that expression is scalar, 0-D.  
 */
const_index_list
pint_const::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Is range equivalent in size?
	\return always false, because an int is really a collapsed range.
 */
bool
pint_const::range_size_equivalent(const const_index& i) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<param_expression_assignment>
pint_const::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	return pint_expr::make_param_expression_assignment_private(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_const::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pint_const::unroll_resolve_value(const unroll_context&, value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pint_const>
pint_const::__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pint_expr>& p) const {
	INVARIANT(p == this);
	return p.is_a<const this_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pint_const::unroll_resolve_rvalues(const unroll_context&, 
		const count_ptr<const pint_expr>& p) const {
	INVARIANT(p == this);
	return p.is_a<const this_type>();	// must be true: this
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return itself, there's nothing to substitute.
 */
count_ptr<const pint_expr>
pint_const::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const pint_expr>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const_index>
pint_const::unroll_resolve_index(const unroll_context&) const {
	return count_ptr<const_index>(new pint_const(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constant need not be unrolled, return reference counted copy of self.  
 */
count_ptr<const pint_expr>
pint_const::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pint_expr>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p const parameter integer.
	\pre p must be pint_const or pint_const_collection.
 */
bool
pint_const::operator < (const const_param& p) const {
	const pint_const* const pp(IS_A(const pint_const*, &p));
	if (pp) {
		return val < pp->val;
	} else {
		const pint_const_collection&
			pc(IS_A(const pint_const_collection&, p));
		INVARIANT(!pc.dimensions());	// must be scalar
		return val < pc.front();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convert to param-real.
 */
preal_value_type
pint_const::to_real_const(void) const {
	return preal_value_type(val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Does nothing, no pointers to visit.  
 */
void
pint_const::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, 
		persistent_traits<this_type>::type_key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_const::write_object(const persistent_object_manager& m, ostream& f) const {
	write_value(f, val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pint_const::load_object(const persistent_object_manager& m, istream& f) {
	read_value(f, val);
}

//=============================================================================
// class pbool_const method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(pbool_const, 1024)
CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(pbool_const)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pbool_const)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private inline empty constructor, uninitialized.
 */
inline
pbool_const::pbool_const() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The expr_dump_context parameter is unused.  
 */
ostream&
pbool_const::dump(ostream& o, const expr_dump_context&) const {
	return o << ((val) ? "true" : "false");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_const::is_true(void) const {
	return val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
	Just copy-constructs.  
 */
count_ptr<const const_param>
pbool_const::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new pbool_const(val));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A pbool const is scalar, thus dimension list is empty.
 */
const_range_list
pbool_const::static_constant_dimensions(void) const {
	return const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<param_expression_assignment>
pbool_const::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	return pbool_expr::make_param_expression_assignment_private(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
pbool_const::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pbool_const::resolve_value(value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pbool_const::unroll_resolve_value(const unroll_context&, value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pbool_const>
pbool_const::__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pbool_expr>& p) const {
	INVARIANT(p == this);
	return p.is_a<const this_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pbool_const::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	INVARIANT(p == this);
	return p.is_a<const this_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return itself, there's nothing to substitute.
 */
count_ptr<const pbool_expr>
pbool_const::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const pbool_expr>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constant need not be unrolled, return reference counted copy of self.  
 */
count_ptr<const pbool_expr>
pbool_const::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const pbool_expr>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pbool_const::must_be_equivalent(const pbool_expr& b) const {
	return b.is_static_constant() && (val == b.static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p const parameter boolean.
	\pre p must be pbool_const or pbool_const_collection.
 */
bool
pbool_const::operator < (const const_param& p) const {
	const pbool_const* pp(IS_A(const pbool_const*, &p));
	if (pp) {
		return val < pp->val;
	} else {
		const pbool_const_collection&
			pc(IS_A(const pbool_const_collection&, p));
		INVARIANT(!pc.dimensions());	// must be scalar
		return val < pc.front();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, 
		persistent_traits<this_type>::type_key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pbool_const::load_object(const persistent_object_manager& m, istream& f) {
	read_value(f, val);
}

//=============================================================================
// class preal_const method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// LIST_VECTOR_POOL_ROBUST_STATIC_DEFINITION(preal_const, 256)
CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(preal_const)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(preal_const)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const preal_const::value_type
preal_const::default_value = 0.0;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private inline empty constructor, uninitialized.
 */
inline
preal_const::preal_const() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The expr_dump_context parameter is unused.  
 */
ostream&
preal_const::dump(ostream& o, const expr_dump_context&) const {
	return o << val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false always because we treat this semantically as a 
	comparison with 0.0 which is technically, only ever an approximation.  
 */
bool
preal_const::is_true(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
	Just copy-constructs.  
 */
count_ptr<const const_param>
preal_const::static_constant_param(void) const {
	return count_ptr<const const_param>(
		new preal_const(val));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	A preal const is scalar, thus dimension list is empty.
 */
const_range_list
preal_const::static_constant_dimensions(void) const {
	return const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<param_expression_assignment>
preal_const::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	return preal_expr::make_param_expression_assignment_private(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_index_list
preal_const::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
preal_const::resolve_value(value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_const::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
preal_const::unroll_resolve_value(const unroll_context&, value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const preal_const>
preal_const::__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const preal_expr>& p) const {
	INVARIANT(p == this);
	return p.is_a<const this_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
preal_const::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const preal_expr>& p) const {
	INVARIANT(p == this);
	return p.is_a<const this_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Constant need not be unrolled, return reference counted copy of self.  
 */
count_ptr<const preal_expr>
preal_const::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const preal_expr>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return itself, there's nothing to substitute.
 */
count_ptr<const preal_expr>
preal_const::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const preal_expr>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
preal_const::must_be_equivalent(const preal_expr& b) const {
	return b.is_static_constant() && (val == b.static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param p const parameter real.
	\pre p must be preal_const or preal_const_collection.
 */
bool
preal_const::operator < (const const_param& p) const {
	const preal_const* pp(IS_A(const preal_const*, &p));
	if (pp) {
		return val < pp->val;
	} else {
		const preal_const_collection&
			pc(IS_A(const preal_const_collection&, p));
		INVARIANT(!pc.dimensions());	// must be scalar
		return val < pc.front();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convert to param-real.
 */
preal_value_type
preal_const::to_real_const(void) const {
	return val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_const::collect_transient_info(persistent_object_manager& m) const {
	m.register_transient_object(this, 
		persistent_traits<this_type>::type_key);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_const::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
preal_const::load_object(const persistent_object_manager& m, istream& f) {
	read_value(f, val);
}

//=============================================================================
// class pstring_const method definitions

pstring_const::pstring_const() : pstring_expr(), const_param(), val() { }

pstring_const::pstring_const(const string& s) : 
	pstring_expr(), const_param(), val(s) { }

pstring_const::~pstring_const() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For static global initialization order safety, return a persistent
	function-local static reference.  
 */
const pstring_const::value_type&
pstring_const::safe_default_value(void) {
	static const pstring_const::value_type ret("");
	return ret;
}

const pstring_const::value_type&
pstring_const::default_value(safe_default_value());

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_ROBUST_STATIC_DEFINITION(pstring_const)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(pstring_const)

ostream&
pstring_const::dump(ostream& o, const expr_dump_context&) const {
	return o << '\"' << val << '\"';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
pstring_const::dump_nonmeta(ostream& o) const {
	return o << val;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No array support for strings yet.  
 */
size_t
pstring_const::dimensions(void) const {
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pstring_const::is_true(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pstring_const::is_relaxed_formal_dependent(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pstring_const::has_static_constant_dimensions(void) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pstring_const::static_constant_param(void) const {
	return count_ptr<const this_type>(new this_type(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const_range_list
pstring_const::static_constant_dimensions(void) const {
	return const_range_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Could only conceivable be called during template parameter comparison.
 */
bool
pstring_const::operator < (const const_param& r) const {
	const this_type* const pp = IS_A(const this_type*, &r);
	if (pp) {
		// string comparison
		return val < pp->val;
	} else {
		// string collections don't exist yet
		THROW_EXIT;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
pstring_const::must_be_equivalent(const pstring_expr& b) const {
	return b.is_static_constant() && (val == b.static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pstring_const>
pstring_const::__unroll_resolve_rvalue(const unroll_context&, 
		const count_ptr<const pstring_expr>& p) const {
	INVARIANT(p == this);
	return p.is_a<const this_type>();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
pstring_const::unroll_resolve_value(const unroll_context&, value_type& i) const {
	i = val;
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pstring_expr>
pstring_const::unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const pstring_expr>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
pstring_const::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const pstring_expr>& p) const {
	INVARIANT(p == this);
	return p.is_a<const this_type>();	// should static_cast
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pstring_expr>
pstring_const::substitute_default_positional_parameters(
		const template_formals_manager&,
		const dynamic_param_expr_list&,
		const count_ptr<const pstring_expr>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unreachable until strings are referenceable as rvalues
	in the meta language.
 */
param_expr::assignment_ptr_type
pstring_const::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	return pstring_expr::make_param_expression_assignment_private(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pstring_const::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pstring_const::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	// no pointers to visit
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pstring_const::write_object(const persistent_object_manager& m,
		ostream& o) const {
	write_value(o, val);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
pstring_const::load_object(const persistent_object_manager& m, istream& i) {
	read_value(i, val);
}

//=============================================================================
}	// end namepace entity
}	// end namepace HAC

DEFAULT_STATIC_TRACE_END

// responsibly undefining macros used
// IDEA: for each header, write an undef header file...

#undef	DEBUG_LIST_VECTOR_POOL
#undef	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE
#undef	ENABLE_STACKTRACE
#undef	STACKTRACE_PERSISTENTS
#undef	STACKTRACE_PERSISTENT
#undef	STACKTRACE_DESTRUCTORS
#undef	STACKTRACE_DTOR

