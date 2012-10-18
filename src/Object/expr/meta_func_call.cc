/**
	\file "Object/expr/meta_func_call.cc"
	$Id: meta_func_call.cc,v 1.2 2010/09/21 00:18:16 fang Exp $
 */

#include "Object/expr/meta_func_call.hh"
#include "Object/expr/meta_func_lib.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/expr/dynamic_param_expr_list.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/pstring_const.hh"
#include "Object/expr/expr_visitor.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/unroll/param_expression_assignment.hh"
#include "Object/ref/aggregate_meta_value_reference_base.hh"
#include "Object/traits/value_traits.hh"
#include "Object/persistent_type_hash.hh"
#include "common/TODO.hh"
#include "common/ICE.hh"
#include "util/IO_utils.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"

namespace util {
using namespace HAC::entity;

SPECIALIZE_UTIL_WHAT(pint_meta_func_call, "pint-func-call")
SPECIALIZE_UTIL_WHAT(pbool_meta_func_call, "pbool-func-call")
SPECIALIZE_UTIL_WHAT(preal_meta_func_call, "preal-func-call")
SPECIALIZE_UTIL_WHAT(pstring_meta_func_call, "pstring-func-call")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pint_meta_func_call, PINT_META_FUNCTION_CALL_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pbool_meta_func_call, PBOOL_META_FUNCTION_CALL_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	preal_meta_func_call, PREAL_META_FUNCTION_CALL_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	pstring_meta_func_call, PSTRING_META_FUNCTION_CALL_TYPE_KEY, 0)

namespace memory {
// explicit template instantiations, needed for -O3
template class count_ptr<pint_meta_func_call>;
template class count_ptr<const pint_meta_func_call>;
template class count_ptr<pbool_meta_func_call>;
template class count_ptr<const pbool_meta_func_call>;
template class count_ptr<preal_meta_func_call>;
template class count_ptr<const preal_meta_func_call>;
template class count_ptr<pstring_meta_func_call>;
template class count_ptr<const pstring_meta_func_call>;
}	// end namespace memory
}

//-----------------------------------------------------------------------------
namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::persistent_traits;

// need some sort of function registry

//=============================================================================
// class meta_func_call method definitions

meta_func_call_base::meta_func_call_base() : fname(), fargs(), fsym() { }

meta_func_call_base::meta_func_call_base(const string& s, 
		const fargs_ptr_type& fp) :
		fname(s), fargs(fp), fsym() {
	NEVER_NULL(fargs);
}

meta_func_call_base::~meta_func_call_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
meta_func_call<B>::meta_func_call() : parent_type(), meta_func_call_base() { }

template <class B>
meta_func_call<B>::meta_func_call(const string& s, 
		const fargs_ptr_type& fp) :
		parent_type(), 
		meta_func_call_base(s, fp) {
}

template <class B>
meta_func_call<B>::~meta_func_call() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return statically typed function call based on return type.
 */
// meta_func_call_base*
param_expr*
meta_func_call_base::make_meta_func_call(const string& fn, 
		const fargs_ptr_type& fa) {
	const mapped_func_entry_type& fp(lookup_meta_function(fn));
	if (!fp.first) {
		// already have error message
		cerr << "Error resolving symbol of meta function call." << endl;
		return NULL;
	}
	switch (fp.second) {
#define	CASE(Tag)							\
	case class_traits<Tag>::type_tag_enum_value:			\
		return new meta_func_call<class_traits<Tag>::expr_base_type>(fn, fa);
	CASE(pbool_tag)
	CASE(pint_tag)
	CASE(preal_tag)
	CASE(pstring_tag)
#undef	CASE
	default: break;
	}
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
ostream&
meta_func_call<B>::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
meta_func_call_base::dump(ostream& o, const expr_dump_context& c) const {
	o << fname << '(';
	fargs->dump(o, c) << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
ostream&
meta_func_call<B>::dump(ostream& o, const expr_dump_context& c) const {
	return meta_func_call_base::dump(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now, functions can only return single values.
	TODO: future support for returning const_collections.
 */
template <class B>
size_t
meta_func_call<B>::dimensions(void) const {
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
bool
meta_func_call<B>::must_be_equivalent(const parent_type& e) const {
	const this_type* fe(IS_A(const this_type*, &e));
	if (!fe)
		return false;
	else	if (fname != fe->fname)
		return false;
	else	return fargs->must_be_equivalent(*fe->fargs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
bool
meta_func_call<B>::is_static_constant(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
bool
meta_func_call<B>::is_relaxed_formal_dependent(void) const {
	return fargs->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
typename B::value_type
meta_func_call<B>::static_constant_value(void) const {
	// should really never be called
	ICE_NEVER_CALL(cerr);
	return value_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
good_bool
meta_func_call<B>::unroll_resolve_value(const unroll_context& c, 
		value_type& i) const {
	const count_ptr<const const_expr_type>
		loc(__unroll_resolve_rvalue(c));
	if (loc) {
		i = loc->static_constant_value();
		return good_bool(true);
	} else {
		// diagnostic?
		return good_bool(false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
count_ptr<const B>
meta_func_call<B>::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const parent_type>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
count_ptr<const const_param>
meta_func_call<B>::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const parent_type>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
count_ptr<const typename B::const_expr_type>
meta_func_call<B>::__unroll_resolve_rvalue(const unroll_context& c,
		const count_ptr<const parent_type>& p) const {
	INVARIANT(p == this);
	return __unroll_resolve_rvalue(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluate all arguments, then apply function.  
	This overload doesn't need a self pointer to be passed in.
	Always returns a new constant expression.  
 */
template <class B>
count_ptr<const typename B::const_expr_type>
meta_func_call<B>::__unroll_resolve_rvalue(const unroll_context& c) const {
// #define	ERROR_OUT	THROW_EXIT
#define	ERROR_OUT	return return_type(NULL)
	typedef	count_ptr<const const_expr_type>	return_type;
	STACKTRACE_VERBOSE;
	// resolve symbol if this is the first time
try {
	if (!fsym) {
		const meta_function_ptr_type
			fp(lookup_meta_function(fname).first);
		if (!fp) {
			// already have error message
			cerr << "Error resolving symbol of "
				"meta function call." << endl;
			ERROR_OUT;
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
		this->dump(cerr, expr_dump_context::brief) << endl;
		ERROR_OUT;
	}
	// cast from const_param back to subtype
	return (*fsym)(*rargs).template is_a<const const_expr_type>();
} catch (...) {
	cerr << "Run-time error calling function: ";
	// drat, process_index unavailable from context...
	this->dump(cerr, expr_dump_context::brief) << endl;
	ERROR_OUT;
}
#undef	ERROR_OUT
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
void
meta_func_call<B>::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	See operators.cc.
	See dynamic_param_expr_list::certify_template_arguments()
 */
template <class B>
count_ptr<const B>
meta_func_call<B>::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e, 
		const count_ptr<const parent_type>& p) const {
	NEVER_NULL(fargs);
	FINISH_ME(Fang);
	return count_ptr<const parent_type>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_func_call_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	fargs->collect_transient_info(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_func_call_base::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	util::write_value(o, fname);
	m.write_pointer(o, fargs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
meta_func_call_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	util::read_value(i, fname);
	m.read_pointer(i, fargs);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
void
meta_func_call<B>::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
void
meta_func_call<B>::write_object(const persistent_object_manager& m,
		ostream& o) const {
	write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class B>
void
meta_func_call<B>::load_object(const persistent_object_manager& m, 
		istream& i) {
	load_object_base(m, i);
}

//=============================================================================
// explicit template instantiations

template class meta_func_call<pint_expr>;
template class meta_func_call<pbool_expr>;
template class meta_func_call<preal_expr>;
template class meta_func_call<pstring_expr>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC
