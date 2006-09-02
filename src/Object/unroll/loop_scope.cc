/**
	\file "Object/unroll/loop_scope.cc"
	Control-flow related class method definitions.  
 	$Id: loop_scope.cc,v 1.9.28.2 2006/09/02 03:58:37 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_LOOP_SCOPE_CC__
#define	__HAC_OBJECT_UNROLL_LOOP_SCOPE_CC__

#define	ENABLE_STACKTRACE		0

#include "Object/unroll/loop_scope.h"
#include "Object/unroll/unroll_context.h"
#include "Object/def/template_formals_manager.h"
#include "Object/def/footprint.h"		// for debug dump footprint
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/const_range.h"
#include "Object/inst/pint_value_collection.h"
#if USE_INSTANCE_PLACEHOLDERS
#include "OBject/inst/value_placeholder.h"
#endif
#include "Object/common/dump_flags.h"
#include "Object/persistent_type_hash.h"
#include "common/TODO.h"
#include "common/ICE.h"
#include "util/persistent_object_manager.tcc"
#include "util/indent.h"
#include "util/stacktrace.h"

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::loop_scope, "loop-scope")
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		HAC::entity::loop_scope, LOOP_SCOPE_TYPE_KEY, 0)
}

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// class loop_scope method definitions

// loop_scope::loop_scope(const never_ptr<const sequential_scope> p)
loop_scope::loop_scope() : interface_type(), parent_type(), meta_loop_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
loop_scope::loop_scope(const ind_var_ptr_type& i, const range_ptr_type& r) :
		interface_type(), parent_type(), meta_loop_base(i, r) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
loop_scope::~loop_scope() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(loop_scope)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: add context to instance_management dumps.
 */
ostream&
loop_scope::dump(ostream& o, const expr_dump_context& dc) const {
	NEVER_NULL(ind_var);
	NEVER_NULL(range);
	o << "(;" << ind_var->get_name() << ':';
	range->dump(o, dc) << ':' << endl;
	{
		INDENT_SECTION(o);
		parent_type::dump(o, dc);
	}
	return o << auto_indent << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: source copied mostly from PRS::rule_loop::unroll().
	We fake an unroll context by emulating a definition scope, 
		treating a loop induction variable as a template formal
		parameter and its value as the actual.  
	TODO: warn about variable shadowing.  
 */
good_bool
loop_scope::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const_range cr;
	if (!range->unroll_resolve_range(c, cr).good) {
		cerr << "Error resolving range expression: ";
		range->dump(cerr, expr_dump_context::default_value) << endl;
		return good_bool(false);
	}
	const pint_value_type min = cr.lower();
	const pint_value_type max = cr.upper();
	if (min > max) {
		// range is either empty or backwards
		return good_bool(true);
	}
#if USE_INSTANCE_PLACEHOLDERS
	// since we need a new lookup scope that uses
	// a placeholder for the pint variable...
	// new unroll_context lookup searches footprints
	footprint f;
	const count_ptr<pint_scalar>
		var(initialize_footprint(f));
	// create a temporary by unrolling the placeholder 
	// induction variable into the footprint as an actual variable
	pint_value_type& p(var->get_instance().value);
		// acquire direct reference
	unroll_context cc(&f);
	cc.chain_context(c);
	for (p = min; p <= max; ++p) {
		if (!parent_type::unroll(cc).good) {
			cerr << "Error resolving loop-body during iteration: ";
			ind_var->dump_hierarchical_name(cerr,
				dump_flags::verbose)
				<< " = " << p << endl;
			return good_bool(false);
		}
	}
#else
	template_formals_manager tfm;
	const never_ptr<const pint_scalar> pvc(&*ind_var);
	tfm.add_strict_template_formal(pvc);

	const count_ptr<pint_const> ind(new pint_const(min));
	const count_ptr<const_param_expr_list> al(new const_param_expr_list);
	NEVER_NULL(al);
	NEVER_NULL(ind);
	al->push_back(ind);
	const template_actuals::const_arg_list_ptr_type sl(NULL);
	const template_actuals ta(al, sl);
	const unroll_context cc(ta, tfm, c);
	// cc.chain_context(c);	// same effect
#if 0 && ENABLE_STACKTRACE
	cerr << "parent context, c: " << endl;
	c.dump(cerr) << endl;
	cerr << "cc @ " << &cc << endl;
	cc.dump(cerr) << endl;
	const footprint* const f = cc.get_target_footprint();
	if (f) {
		cerr << "cc\'s target footprint: " << endl;
		f->dump_with_collections(cerr) << endl;
	}
#endif
	pint_value_type ind_val = min;
	for ( ; ind_val <= max; ind_val++) {
		*ind = ind_val;
		if (!parent_type::unroll(cc).good) {
			cerr << "Error resolving loop-body during iteration: ";
			ind_var->dump_hierarchical_name(cerr,
				dump_flags::verbose)
				<< " = " << ind_val << endl;
			return good_bool(false);
		}
	}
#endif
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
loop_scope::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	meta_loop_base::collect_transient_info_base(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
loop_scope::write_object(const persistent_object_manager& m, ostream& o) const {
	meta_loop_base::write_object_base(m, o);
	parent_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
loop_scope::load_object(const persistent_object_manager& m, istream& i) {
	meta_loop_base::load_object_base(m, i);
	parent_type::load_object_base(m, i);
}

//=============================================================================
} 	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_LOOP_SCOPE_CC__

