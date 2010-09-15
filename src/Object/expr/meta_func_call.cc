/**
	\file "Object/expr/meta_func_call.cc"
	$Id: meta_func_call.cc,v 1.1.2.1 2010/09/15 00:57:53 fang Exp $
 */

#include "Object/expr/meta_func_call.h"
#include "Object/expr/meta_func_lib.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/dynamic_param_expr_list.h"
#include "Object/expr/const_param.h"
#include "Object/expr/expr_visitor.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/unroll/param_expression_assignment.h"
#include "Object/ref/aggregate_meta_value_reference_base.h"
#include "Object/persistent_type_hash.h"
#include "common/TODO.h"
#include "util/IO_utils.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"

namespace util {
using HAC::entity::meta_func_call;

SPECIALIZE_UTIL_WHAT(meta_func_call, "pstring-const")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	meta_func_call, META_FUNCTION_CALL_TYPE_KEY, 0)

namespace memory {
// explicit template instantiations, needed for -O3
template class count_ptr<meta_func_call>;
template class count_ptr<const meta_func_call>;
}	// end namespace memory
}

//-----------------------------------------------------------------------------
namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::persistent_traits;

// need some sort of function registry

//=============================================================================
// class meta_func_call method definitions

meta_func_call::meta_func_call() : param_expr(), fname(), fargs(), fsym() { }

meta_func_call::meta_func_call(const string& s, const fargs_ptr_type& fp) :
		param_expr(), fname(s), fargs(fp), fsym() {
	NEVER_NULL(fargs);
}

meta_func_call::~meta_func_call() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(meta_func_call)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
meta_func_call::dump(ostream& o, const expr_dump_context& c) const {
	o << fname << '(';
	fargs->dump(o, c) << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now, functions can only return single values.
	TODO: future support for returning const_collections.
 */
size_t
meta_func_call::dimensions(void) const {
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
meta_func_call::may_be_equivalent_generic(const param_expr&) const {
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
meta_func_call::must_be_equivalent_generic(const param_expr& e) const {
	const this_type* fe(IS_A(const this_type*, &e));
	if (!fe)
		return false;
	FINISH_ME(Fang);
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
meta_func_call::is_static_constant(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool
meta_func_call::is_relaxed_formal_dependent(void) const {
	return fargs->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
meta_func_call::static_constant_param(void) const {
	// should really never be called
	FINISH_ME(Fang);
	return count_ptr<const const_param>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
meta_func_call::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const param_expr>& p) const {
	typedef	count_ptr<const const_param>		return_type;
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	// resolve symbol if this is the first time
try {
	if (!fsym) {
		const meta_function_ptr_type fp(lookup_meta_function(fname));
		if (!fp) {
			// already have error message
			cerr << "Error resolving symbol of "
				"meta function call." << endl;
			THROW_EXIT;
			return return_type(NULL);
		}
		fsym = fp;
	}
	NEVER_NULL(fsym);
	const count_ptr<const const_param_expr_list>
		rargs(fargs->unroll_resolve_rvalues(c, fargs));
	if (!rargs) {
		// already have error message?
		cerr << "Error resolving arguments of meta function call."
			<< endl;
		THROW_EXIT;
		return return_type(NULL);
	}
	return (*fsym)(*rargs);
} catch (...) {
	cerr << "Run-time error calling function: ";
	// drat, process_index unavailable from context...
#if 0
	std::ostringstream canonical_name;
	if (process_index) {
		sm.get_pool<process_tag>()[process_index]
			.dump_canonical_name(canonical_name, topfp, sm);
	}
	const expr_dump_context
		edc(process_index ? canonical_name.str() : string());
#endif
	dump(cerr, expr_dump_context::brief) << endl;
	throw;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_func_call::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	See operators.cc.
	See dynamic_param_expr_list::certify_template_arguments()
 */
count_ptr<const param_expr>
meta_func_call::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const param_expr>& p) const {
	NEVER_NULL(fargs);
	FINISH_ME(Fang);
	return count_ptr<const param_expr>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to look up the return type of the function.
 */
count_ptr<param_expression_assignment>
meta_func_call::make_param_expression_assignment_private(
		const count_ptr<const param_expr>& p) const {
	typedef assignment_ptr_type             return_type;
	INVARIANT(p == this);
#if 1
	FINISH_ME(Fang);
	return count_ptr<param_expression_assignment>(NULL);
#else
	return return_type(
		new pint_expression_assignment(p.is_a<const this_type>()));
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Need to know the return type of the function.  
 */
count_ptr<aggregate_meta_value_reference_base>
meta_func_call::make_aggregate_meta_value_reference_private(
		const count_ptr<const param_expr>& p) const {
	INVARIANT(p == this);
#if 1
	FINISH_ME(Fang);
	return count_ptr<aggregate_meta_value_reference_base>(NULL);
#else
	const count_ptr<const this_type> pb(p.is_a<const this_type>());
	NEVER_NULL(pb);
	const count_ptr<aggregate_meta_value_reference<tag_type> >
		ret(new aggregate_meta_value_reference<tag_type>);
	ret->append_meta_value_reference(pb);
	return ret;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_func_call::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	fargs->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_func_call::write_object(const persistent_object_manager& m,
		ostream& o) const {
	util::write_value(o, fname);
	m.write_pointer(o, fargs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_func_call::load_object(const persistent_object_manager& m, 
		istream& i) {
	util::read_value(i, fname);
	m.read_pointer(i, fargs);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC
