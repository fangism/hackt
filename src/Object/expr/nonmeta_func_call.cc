/**
	\file "Object/expr/nonmeta_func_call.cc"
	$Id: nonmeta_func_call.cc,v 1.1.2.1 2007/07/20 21:07:44 fang Exp $
 */

#include "Object/expr/nonmeta_func_call.h"
#include "Object/expr/nonmeta_expr_list.h"
#include "Object/expr/expr_visitor.h"
#include "Object/expr/const_param.h"
#include "Object/type/data_type_reference.h"
#include "Object/type/canonical_generic_datatype.h"
#include "Object/persistent_type_hash.h"
#include "common/ltdl-wrap.h"
#include "common/TODO.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/IO_utils.tcc"
#include "util/what.h"

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

//=============================================================================
// class nonmeta_func_call method definitions

nonmeta_func_call::nonmeta_func_call() : data_expr(), fname(), fargs() { }

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
 */
count_ptr<const data_type_reference>
nonmeta_func_call::get_unresolved_data_type_ref(void) const {
	// really don't know what to do here...
	FINISH_ME(Fang);
	return count_ptr<const data_type_reference>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Cannot perform create-time type checking either, since function
	is still unbound.  
 */
canonical_generic_datatype
nonmeta_func_call::get_resolved_data_type_ref(const unroll_context& c) const {
	FINISH_ME(Fang);
	return canonical_generic_datatype();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_DATA_EXPR_EQUIVALENCE
bool
nonmeta_func_call::may_be_type_equivalent(const data_expr&) const {
	FINISH_ME(Fang);
	return false;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_expr>
nonmeta_func_call::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	FINISH_ME_EXIT(Fang);
	return count_ptr<const data_expr>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
nonmeta_func_call::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const data_expr>& p) const {
	FINISH_ME_EXIT(Fang);
	return count_ptr<const const_param>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_func_call::evaluate_write(const nonmeta_context_base&, 
		channel_data_writer&, 
		const count_ptr<const data_expr>& p) const {
	FINISH_ME_EXIT(Fang);
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

