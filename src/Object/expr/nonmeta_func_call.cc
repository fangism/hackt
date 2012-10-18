/**
	\file "Object/expr/nonmeta_func_call.cc"
	$Id: nonmeta_func_call.cc,v 1.6 2010/08/25 23:26:38 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include "Object/expr/nonmeta_func_call.hh"
#include "Object/expr/nonmeta_expr_list.hh"
#include "Object/expr/expr_visitor.hh"
#include "Object/expr/const_param.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/type/data_type_reference.hh"
#include "Object/type/canonical_generic_datatype.hh"
#include "Object/persistent_type_hash.hh"
// #include "Object/expr/dlfunction.hh"
#include "Object/devel_switches.hh"
#if USE_TOP_DATA_TYPE
#include "Object/def/built_in_datatype_def.hh"
#include "Object/def/user_def_datatype.hh"		// why?
#endif
#include "common/TODO.hh"
#include "util/stacktrace.hh"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/IO_utils.tcc"
#include "util/what.hh"

namespace util {
using HAC::entity::nonmeta_func_call;

	SPECIALIZE_UTIL_WHAT(nonmeta_func_call, "func-call-expr")

	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		nonmeta_func_call, NONMETA_FUNCTION_CALL_EXPR_TYPE_KEY, 0)
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
namespace HAC {
namespace entity {
using util::write_value;
using util::read_value;
#include "util/using_ostream.hh"

//=============================================================================
// class nonmeta_func_call method definitions

nonmeta_func_call::nonmeta_func_call() : 
		data_expr(), fname(), fargs(), fsym(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_func_call::nonmeta_func_call(const string& f, 
		const fargs_ptr_type& a) : 
		data_expr(), fname(f), fargs(a), fsym(NULL) {
	NEVER_NULL(fargs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_func_call::~nonmeta_func_call() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(nonmeta_func_call)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
nonmeta_func_call::dump(ostream& o, const expr_dump_context& c) const {
	o << fname << '(';
	if (fargs)
		return fargs->dump(o, c) << ')';
	else	return o << "null)";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Scalar return values only, please.  (for now)
 */
size_t
nonmeta_func_call::dimensions(void) const {
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Cannot perform compile-time type check, since function is unbound.
	HACK: The caller should catch this case and allow it as an exception.
 */
count_ptr<const data_type_reference>
nonmeta_func_call::get_unresolved_data_type_ref(void) const {
#if USE_TOP_DATA_TYPE
	static const count_ptr<const data_type_reference>
		dtop(new data_type_reference(
			never_ptr<const datatype_definition_base>(
				&top_data_definition)));
	return dtop;
#else
	// really don't know what to do here...
	FINISH_ME(Fang);
	return count_ptr<const data_type_reference>(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Cannot perform create-time type checking either, since function
	is still unbound.  
	HACK: The caller shouls catch this case and allow it as an exception.
 */
canonical_generic_datatype
nonmeta_func_call::get_resolved_data_type_ref(const unroll_context& c) const {
	FINISH_ME(Fang);
	return canonical_generic_datatype();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_DATA_EXPR_EQUIVALENCE
/**
	Type is not known a priori, conservatively return true.
 */
bool
nonmeta_func_call::may_be_type_equivalent(const data_expr&) const {
//	FINISH_ME(Fang);
	return true;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**	
	\return deep or shallow copy of self, depending on result of unroll.
 */
count_ptr<const nonmeta_func_call>
nonmeta_func_call::__unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const this_type>& p) const {
	// resolve arguments, compare...
	typedef	count_ptr<const nonmeta_func_call>		return_type;
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<const nonmeta_expr_list>
		rf(fargs->unroll_resolve_copy(c, fargs));
	if (!rf) {
		cerr << "Error resolving function arguments." << endl;
		return return_type(NULL);
	}
	if (rf == fargs) {
		return p;
	} else {
		const count_ptr<const this_type>
			ret(new this_type(fname, rf));
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_expr>
nonmeta_func_call::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	return __unroll_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Really lazy linking: lookup function the first time it is called.
	NOTE: some functions may return void (statements only)
	thus, we must throw an exception to halt execution (in simulation).
 */
count_ptr<const const_param>
nonmeta_func_call::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const data_expr>& p) const {
	typedef	count_ptr<const const_param>		return_type;
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	// resolve symbol if this is the first time
try {
	if (!fsym) {
		const chp_dlfunction_ptr_type fp(lookup_chpsim_function(fname));
		if (!fp) {
			// already have error message
			cerr << "Error resolving symbol of "
				"nonmeta function call." << endl;
			THROW_EXIT;
			return return_type(NULL);
		}
		fsym = fp;
	}
	NEVER_NULL(fsym);
	const count_ptr<const const_param_expr_list>
		rargs(fargs->nonmeta_resolve_copy(c, fargs));
	if (!rargs) {
		// already have error message?
		cerr << "Error resolving arguments of nonmeta function call."
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
}	// end nonmeta_resolve_copy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Must resolve value first before being able to call
	virtual evaluate_write with actual value.
 */
void
nonmeta_func_call::evaluate_write(const nonmeta_context_base& c, 
		channel_data_writer& w, 
		const count_ptr<const data_expr>& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<const const_param> rv(nonmeta_resolve_copy(c, p));
	if (!rv) {
		cerr << "Error evaluating function `" << fname << "\'." << endl;
		THROW_EXIT;
	}
	// lazy and slow (dynamic cross-cast), but self-maintaining:
	const count_ptr<const data_expr>
		trv(rv.is_a<const data_expr>());
	NEVER_NULL(trv);
#if ENABLE_STACKTRACE
	rv->dump(cout << "const : ", expr_dump_context::default_value) << endl;
	trv->dump(cout << "result: ", expr_dump_context::default_value) << endl;
#endif
	trv->evaluate_write(c, w, trv);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_func_call::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_func_call::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	if (fargs) {
		fargs->collect_transient_info(m);
	}
	// leave the function pointer unbound
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_func_call::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, fname);
	m.write_pointer(f, fargs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_func_call::load_object(const persistent_object_manager& m, 
		istream& f) {
	read_value(f, fname);
	m.read_pointer(f, fargs);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

