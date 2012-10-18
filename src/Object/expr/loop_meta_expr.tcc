/**
	\file "Object/expr/loop_meta_expr.tcc"
	$Id: loop_meta_expr.tcc,v 1.3 2009/03/09 07:30:47 fang Exp $
 */

#ifndef	__HAC_OBJECT_EXPR_LOOP_META_EXPR_TCC__
#define	__HAC_OBJECT_EXPR_LOOP_META_EXPR_TCC__

#include <iostream>
#include "Object/expr/loop_meta_expr.hh"
#include "Object/expr/const_range.hh"
#include "Object/expr/const_index_list.hh"
#include "Object/expr/expr_visitor.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/expr/pint_const.hh"
#include "Object/def/footprint.hh"
#include "Object/inst/param_value_collection.hh"
#include "Object/inst/value_placeholder.hh"
#include "Object/inst/value_scalar.hh"
#include "Object/inst/pint_instance.hh"
#include "Object/traits/classification_tags_fwd.hh"
#include "Object/traits/pint_traits.hh"
#include "Object/common/dump_flags.hh"
#include "Object/unroll/unroll_context.hh"
#include "common/ICE.hh"
#include "util/persistent_object_manager.hh"
#include "util/what.hh"
#include "util/IO_utils.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::persistent_traits;
using util::write_value;
using util::read_value;

//=============================================================================
// class loop_meta_expr method definitions

/**
	Private default constructor, only used for deserialization.  
 */
template <class E>
loop_meta_expr<E>::loop_meta_expr() : meta_loop_base(), parent_type(), 
		ex(), op() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
loop_meta_expr<E>::loop_meta_expr(const ind_var_ptr_type& i, 
		const range_ptr_type& r, const operand_ptr_type& e, 
		const op_type* o) : 
		meta_loop_base(i, r), parent_type(), 
		ex(e), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(ex);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
loop_meta_expr<E>::~loop_meta_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
std::ostream&
loop_meta_expr<E>::what(std::ostream& o) const {
        return o << util::what<this_type>::name();
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: have op include precedence information.  
 */
template <class E>
ostream&
loop_meta_expr<E>::dump(ostream& o, const expr_dump_context& c) const {
	NEVER_NULL(this->ind_var);
	NEVER_NULL(this->range);
	o << '(' << op_key_type(binary_expr_type::reverse_op_map.find(this->op)->second)
		<< ':' << this->ind_var->get_name() << ':';
	this->range->dump(o, c) << ": ";
	return this->ex->dump(o, c) << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return false, conservatively.  
 */
template <class E>
bool
loop_meta_expr<E>::is_static_constant(void) const {
	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
bool
loop_meta_expr<E>::is_relaxed_formal_dependent(void) const {
	return range->is_relaxed_formal_dependent() ||
		ex->is_relaxed_formal_dependent();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
typename loop_meta_expr<E>::value_type
loop_meta_expr<E>::static_constant_value(void) const {
#if 0
	const arg_type a = lx->static_constant_value();
	const arg_type b = rx->static_constant_value();
	// Oooooh, virtual operator dispatch!
	return (*op)(a,b);
#else
	ICE_NEVER_CALL(cerr);
	return value_type();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
template <class E>
loop_meta_expr<E>::value_type
loop_meta_expr<E>::evaluate(const op_key_type o,
		const value_type l, const value_type r) {
	const op_type* op(op_map.find(o)->second);
	INVARIANT(op);
	return (*op)(l,r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
loop_meta_expr<E>::value_type
loop_meta_expr<E>::evaluate(const op_type* op,
		const value_type l, const value_type r) {
	INVARIANT(op);
	return (*op)(l,r);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: not sure if must_be_equivalent_index is correct, 
		might need to write must_be_equivalent_range to be safe.
 */
template <class E>
bool
loop_meta_expr<E>::must_be_equivalent(const expr_base_type& p) const {
	const this_type* const ae = IS_A(const this_type*, &p);
	if (ae) {
		// for now structural equivalence only,
		return (this->op == ae->op) && 
			(this->range->must_be_equivalent_index(*ae->range)) &&
			this->ex->must_be_equivalent(*ae->ex);
		// later, symbolic equivalence, Ooooh!
	} else {
		// conservatively
		return false;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return empty list, signifying a scalar value, because all 
		expressions operations only work on scalars.  
 */
template <class E>
const_index_list
loop_meta_expr<E>::resolve_dimensions(void) const {
	return const_index_list();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
void
loop_meta_expr<E>::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if resolved.
 */
template <class E>
good_bool
loop_meta_expr<E>::unroll_resolve_value(const unroll_context& c,
		value_type& i) const {
	static const expr_dump_context& dc(expr_dump_context::default_value);
	const_range cr;
	if (!this->range->unroll_resolve_range(c, cr).good) {
		cerr << "Error resolving range expression: ";
		range->dump(cerr, dc) << endl;
		return good_bool(false);
	}
	const pint_value_type min = cr.lower();
	const pint_value_type max = cr.upper();
	if (min > max) {
		cerr << "Error: loop expression cannot operate on empty range."
			<< endl;
		return good_bool(false);
	}

	DECLARE_TEMPORARY_FOOTPRINT(f);
	const never_ptr<pint_scalar> var(this->initialize_footprint(f));
	pint_value_type& p(var->get_instance().value);
	const unroll_context cc(&f, c);

	p = min;
	// first iteration: set `i'
	if (!this->ex->unroll_resolve_value(cc, i).good) {
		cerr << "Error resolving operand in loop expression for ";
		this->ind_var->dump_qualified_name(cerr, dump_flags::verbose)
			<< " = " << p << endl;
		return good_bool(false);
	}
	// subsequent iterations: accumulate result in `i'
	for (++p; p <= max; ++p) {
		value_type val;
		if (!this->ex->unroll_resolve_value(cc, val).good) {
			cerr << "Error resolving operand in loop expression for ";
			this->ind_var->dump_qualified_name(cerr, dump_flags::verbose)
				<< " = " << p << endl;
			return good_bool(false);
		}
		i = (*op)(i, val);	// accumulate
	}
	return good_bool(true);
}	// end method unroll_resolve_value

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return pint_const of the resolved value.
 */
template <class E>
count_ptr<const typename loop_meta_expr<E>::const_expr_type>
loop_meta_expr<E>::__unroll_resolve_rvalue(const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	typedef	count_ptr<const const_expr_type>		return_type;
	INVARIANT(p == this);
	value_type val;
	if (this->unroll_resolve_value(c, val).good) {
		return return_type(new const_expr_type(val));
	} else {
		// already have error message
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
count_ptr<const const_param>
loop_meta_expr<E>::unroll_resolve_rvalues(const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
count_ptr<const typename loop_meta_expr<E>::expr_base_type>
loop_meta_expr<E>::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const expr_base_type>& p) const {
	return __unroll_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Substitution on loop expression in template substitution?
	This is wild... complete untested.
	\return expression with any positional parameters substituted.  
 */
template <class E>
count_ptr<const typename loop_meta_expr<E>::expr_base_type>
loop_meta_expr<E>::substitute_default_positional_parameters(
		const template_formals_manager& f, 
		const dynamic_param_expr_list& e,
		const count_ptr<const expr_base_type>& p) const {
	typedef	count_ptr<const expr_base_type>		return_type;
	INVARIANT(p == this);
	const return_type
		rex(this->ex->substitute_default_positional_parameters(
			f, e, ex));
	if (rex) {
		if (rex == this->ex) {
			return p;
		} else {
			return return_type(new this_type(
				this->ind_var, this->range, 
				this->ex, this->op));
		}
	} else {
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
void
loop_meta_expr<E>::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	meta_loop_base::collect_transient_info_base(m);
	this->ex->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
void
loop_meta_expr<E>::write_object(const persistent_object_manager& m,
		ostream& f) const {
	meta_loop_base::write_object_base(m, f);
	write_value(f,
		op_key_type(binary_expr_type::reverse_op_map.find(this->op)->second));
	m.write_pointer(f, ex);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class E>
void
loop_meta_expr<E>::load_object(const persistent_object_manager& m, istream& f) {
	meta_loop_base::load_object_base(m, f);
	{
	op_key_type o;
	read_value(f, o);
	const typename binary_expr_type::op_map_type::const_iterator
		i(binary_expr_type::op_map.find(o));
	INVARIANT(i != binary_expr_type::op_map.end());
	this->op = i->second;
	}
	m.read_pointer(f, ex);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_EXPR_LOOP_META_EXPR_TCC__

