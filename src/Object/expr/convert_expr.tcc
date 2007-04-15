/**
	\file "Object/expr/convert_expr.tcc"
	$Id: convert_expr.tcc,v 1.3 2007/04/15 05:52:15 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_CONVERT_EXPR_TCC__
#define	__HAC_OBJECT_EXPR_CONVERT_EXPR_TCC__

#include <iostream>
#include "Object/expr/convert_expr.h"
#include "Object/expr/expr_visitor.h"
#include "Object/traits/class_traits_fwd.h"
// #include "util/memory/count_ptr.tcc"
#include "util/what.h"
#include "util/persistent_object_manager.h"

namespace HAC {
namespace entity {
using util::persistent_object_manager;
using std::istream;

//=============================================================================
// class convert_expr method definitions

/**
	Private empty constructor.
 */
CONVERT_EXPR_TEMPLATE_SIGNATURE
CONVERT_EXPR_CLASS::convert_expr() : rvalue_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
CONVERT_EXPR_CLASS::convert_expr(const rvalue_ptr_type& v) : rvalue_expr(v) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
CONVERT_EXPR_CLASS::~convert_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
ostream&
CONVERT_EXPR_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
ostream&
CONVERT_EXPR_CLASS::dump(ostream& o, const expr_dump_context& c) const {
	return rvalue_expr->dump(
		o << class_traits<tag_type>::tag_name << '(', c)
		<< ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
size_t
CONVERT_EXPR_CLASS::dimensions(void) const {
	return this->rvalue_expr->dimensions();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
bool
CONVERT_EXPR_CLASS::must_be_equivalent(const parent_type& r) const {
	const this_type* const rr = IS_A(const this_type*, &r);
	if (rr) {
		return this->rvalue_expr->must_be_equivalent(*rr->rvalue_expr);
	} else {
		return false;	// conservatively
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
bool
CONVERT_EXPR_CLASS::is_static_constant(void) const {
	return this->rvalue_expr->is_static_constant();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
bool
CONVERT_EXPR_CLASS::is_relaxed_formal_dependent(void) const {
	return this->rvalue_expr->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
count_ptr<const const_param>
CONVERT_EXPR_CLASS::static_constant_param(void) const {
	typedef	count_ptr<const const_expr_type>	return_type;
	const count_ptr<const const_rvalue_expr_type>
		v(this->rvalue_expr->static_constant_param()
			.template is_a<const const_rvalue_expr_type>());
	if (!v) {
		return return_type(NULL);
	}
	return return_type(new const_expr_type(
		value_type(v->static_constant_value())));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\pre this really is constant.  
 */
CONVERT_EXPR_TEMPLATE_SIGNATURE
typename CONVERT_EXPR_CLASS::value_type
CONVERT_EXPR_CLASS::static_constant_value(void) const {
	// constructor cast expression will convert
	return value_type(this->rvalue_expr->static_constant_value());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
good_bool
CONVERT_EXPR_CLASS::unroll_resolve_value(const unroll_context& c, 
		value_type& v) const {
	rvalue_value_type rvf;
	if (this->rvalue_expr->unroll_resolve_value(c, rvf).good) {
		v = rvf;
		return good_bool(true);
	} else {
		return good_bool(false);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
count_ptr<const typename CONVERT_EXPR_CLASS::const_expr_type>
CONVERT_EXPR_CLASS::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const parent_type>& t) const {
	typedef	count_ptr<const const_expr_type>	return_type;
	INVARIANT(t == this);
	rvalue_value_type v;
	if (this->rvalue_expr->unroll_resolve_value(c, v).good) {
		return return_type(new const_expr_type(value_type(v)));
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Do any conversion on const_collections?
 */
CONVERT_EXPR_TEMPLATE_SIGNATURE
count_ptr<const const_param>
CONVERT_EXPR_CLASS::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const parent_type>& t) const {
	return this->__unroll_resolve_rvalue(c, t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
count_ptr<const typename CONVERT_EXPR_CLASS::parent_type>
CONVERT_EXPR_CLASS::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const parent_type>& t) const {
	return this->__unroll_resolve_rvalue(c, t);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
count_ptr<const typename CONVERT_EXPR_CLASS::parent_type>
CONVERT_EXPR_CLASS::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const parent_type>& p) const {
	typedef	count_ptr<const parent_type>	return_type;
	INVARIANT(p == this);
	const count_ptr<const rvalue_expr_type>
		rs(this->rvalue_expr->substitute_default_positional_parameters(
			f, e, this->rvalue_expr));
	if (rs) {
		if (rs == this->rvalue_expr) {
			return p;
		} else {
			return return_type(new this_type(rs));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
void
CONVERT_EXPR_CLASS::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
void
CONVERT_EXPR_CLASS::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	this->rvalue_expr->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
void
CONVERT_EXPR_CLASS::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, this->rvalue_expr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CONVERT_EXPR_TEMPLATE_SIGNATURE
void
CONVERT_EXPR_CLASS::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, this->rvalue_expr);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_CONVERT_EXPR_TCC__

