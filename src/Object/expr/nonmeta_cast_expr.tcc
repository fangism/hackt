/**
	\file "Object/expr/nonmeta_cast_expr.tcc"
	$Id: nonmeta_cast_expr.tcc,v 1.5 2010/01/03 01:34:39 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_NONMETA_CAST_EXPR_TCC__
#define	__HAC_OBJECT_EXPR_NONMETA_CAST_EXPR_TCC__

#ifndef	ENABLE_STACKTRACE
#define	ENABLE_STACKTRACE			0
#endif

#include <iostream>
#include "Object/expr/nonmeta_cast_expr.hh"
#include "Object/expr/const_param.hh"
#include "Object/expr/expr_visitor.hh"
#include "Object/type/canonical_generic_datatype.hh"
#include "Object/traits/class_traits_fwd.hh"
#include "util/what.hh"
#include "util/stacktrace.hh"
#include "util/memory/chunk_map_pool.tcc"
#include "util/persistent_object_manager.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"

template <class>
struct expr_tag;

//=============================================================================
// class nonmeta_cast_expr method definitions

// pool-allocator initialization
// doesn't work because comma in arguments (templates) splits into 
// multiple arguments in macro-expansion... :(
// damn commas...
#if 0
TEMPLATE_CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(
	NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE,
	NONMETA_CAST_EXPR_CLASS)
#elif 0
// same issue, forwarding macro arguments
__CHUNK_MAP_POOL_TEMPLATE_DEFAULT_STATIC_DEFINITION(
	NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE,
	NONMETA_CAST_EXPR_CLASS)
#else
__CHUNK_MAP_POOL_TEMPLATE_DEFAULT_STATIC_INIT(NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE,
	NONMETA_CAST_EXPR_CLASS)
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_DEFAULT_OPERATOR_NEW(NONMETA_CAST_EXPR_CLASS)
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_DEFAULT_OPERATOR_PLACEMENT_NEW(NONMETA_CAST_EXPR_CLASS)
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
__CHUNK_MAP_POOL_DEFAULT_OPERATOR_DELETE(NONMETA_CAST_EXPR_CLASS)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
NONMETA_CAST_EXPR_CLASS::nonmeta_cast_expr() : result_type(), rvalue() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
NONMETA_CAST_EXPR_CLASS::nonmeta_cast_expr(
		const count_ptr<const rvalue_type>& r) : 
		result_type(), rvalue(r) {
	NEVER_NULL(r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
NONMETA_CAST_EXPR_CLASS::~nonmeta_cast_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
size_t
NONMETA_CAST_EXPR_CLASS::dimensions(void) const {
	return rvalue->dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
ostream&
NONMETA_CAST_EXPR_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
ostream&
NONMETA_CAST_EXPR_CLASS::dump(ostream& o, const expr_dump_context& c) const {
	return rvalue->dump(o <<
		class_traits<
			typename expr_tag<result_type>::type
			>::tag_name << '(', c)
		<< ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
count_ptr<const data_type_reference>
NONMETA_CAST_EXPR_CLASS::get_unresolved_data_type_ref(void) const {
	return class_traits<
		typename expr_tag<result_type>::type
		>::nonmeta_data_type_ptr;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
canonical_generic_datatype
NONMETA_CAST_EXPR_CLASS::get_resolved_data_type_ref(
		const unroll_context& c) const {
	return class_traits<
		typename expr_tag<result_type>::type
		>::nonmeta_data_type_ptr->make_canonical_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
count_ptr<const typename NONMETA_CAST_EXPR_CLASS::result_type>
NONMETA_CAST_EXPR_CLASS::unroll_resolve_copy(const unroll_context& c,
		const count_ptr<const result_type>& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<const rvalue_type>
		rd(rvalue->__unroll_resolve_copy(c, rvalue));
	if (rd == rvalue) {
		return p;
	} else {
		return count_ptr<const result_type>(new this_type(rd));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	No casting done here, because this return type is still generic.
 */
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
count_ptr<const const_param>
NONMETA_CAST_EXPR_CLASS::nonmeta_resolve_copy(const nonmeta_context_base& c,
		const count_ptr<const result_type>& p) const {
	INVARIANT(p == this);
	return nonmeta_resolve_copy(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Here is where the type-check occurs. 
	How shall we handle errors?  
 */
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
count_ptr<const typename NONMETA_CAST_EXPR_CLASS::const_expr_type>
NONMETA_CAST_EXPR_CLASS::__nonmeta_resolve_rvalue(const nonmeta_context_base& c,
		const count_ptr<const result_type>& p) const {
	typedef	count_ptr<const const_expr_type>	return_type;
	STACKTRACE_VERBOSE;
	const count_ptr<const const_param>
		r(rvalue->nonmeta_resolve_copy(c, rvalue));
	if (r) {
		const return_type ret(r.template is_a<const const_expr_type>());
		if (!ret) {
			cerr << "Run-time error: expecting a " <<
				class_traits<
					typename expr_tag<result_type>::type
					>::tag_name <<
				", but got a ";
			r->what(cerr) << '.' << endl;
//			THROW_EXIT;
		}
		return ret;
	} else {
		// already have error message?
		cerr << "Run-time error: expecting a " <<
			class_traits<
				typename expr_tag<result_type>::type
				>::tag_name <<
			", but got (void)." << endl;
//		THROW_EXIT;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
void
NONMETA_CAST_EXPR_CLASS::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
void
NONMETA_CAST_EXPR_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		util::persistent_traits<this_type>::type_key)) {
	NEVER_NULL(rvalue);
	rvalue->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
void
NONMETA_CAST_EXPR_CLASS::write_object(
		const persistent_object_manager& m, ostream& o) const {
	m.write_pointer(o, rvalue);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
NONMETA_CAST_EXPR_TEMPLATE_SIGNATURE
void
NONMETA_CAST_EXPR_CLASS::load_object(
		const persistent_object_manager& m, istream& i) {
	m.read_pointer(i, rvalue);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_NONMETA_CAST_EXPR_TCC__

