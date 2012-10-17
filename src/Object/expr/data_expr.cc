/**
	\file "Object/expr/data_expr.cc"
	Implementation of data expression classes.  
	NOTE: file was moved from "Object/art_object_data_expr.cc"
	$Id: data_expr.cc,v 1.21 2011/01/14 01:32:56 fang Exp $
 */

#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE			0
#define	PARANOID				0

#include <iostream>
#include "Object/expr/int_arith_expr.hh"
#include "Object/expr/int_relational_expr.hh"
#include "Object/expr/bool_logical_expr.hh"
#include "Object/expr/int_negation_expr.hh"
#include "Object/expr/bool_negation_expr.hh"
#include "Object/expr/int_range_expr.hh"
#include "Object/expr/real_expr.hh"
#include "Object/expr/enum_expr.hh"
#include "Object/expr/string_expr.hh"
#include "Object/expr/struct_expr.hh"
#include "Object/expr/nonmeta_expr_functor.hh"
#include "Object/expr/nonmeta_expr_list.hh"
#include "Object/expr/nonmeta_index_list.hh"
#include "Object/expr/int_range_list.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/expr/operator_precedence.hh"
#include "Object/expr/const_index_list.hh"
#include "Object/expr/dynamic_meta_index_list.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/pstring_const.hh"
#include "Object/expr/loop_nonmeta_expr.tcc"
#include "Object/expr/nonmeta_cast_expr.tcc"
#include "Object/expr/nonmeta_func_call.hh"
#include "Object/expr/expr_visitor.hh"
#include "Object/nonmeta_channel_manipulator.hh"

#include "Object/persistent_type_hash.hh"
#include "Object/type/data_type_reference.hh"
#include "Object/type/canonical_generic_datatype.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/traits/int_traits.hh"
#include "Object/traits/real_traits.hh"
#include "Object/traits/string_traits.hh"

#include "common/TODO.hh"

#include "util/reserve.hh"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/what.hh"
#include "util/IO_utils.hh"
#include "util/stacktrace.hh"
#include "util/multikey.hh"

namespace util {
using HAC::entity::int_arith_expr;
using HAC::entity::int_relational_expr;
using HAC::entity::bool_logical_expr;
using HAC::entity::int_negation_expr;
using HAC::entity::bool_negation_expr;
using HAC::entity::nonmeta_index_list;
using HAC::entity::nonmeta_expr_list;
using HAC::entity::int_range_expr;
using HAC::entity::int_arith_loop_expr;
using HAC::entity::bool_logical_loop_expr;
using HAC::entity::bool_return_cast_expr;
using HAC::entity::int_return_cast_expr;
using HAC::entity::real_return_cast_expr;
using HAC::entity::string_return_cast_expr;

	SPECIALIZE_UTIL_WHAT(int_arith_expr, "int-arith-expr")
	SPECIALIZE_UTIL_WHAT(int_relational_expr, "int-relatonal-expr")
	SPECIALIZE_UTIL_WHAT(bool_logical_expr, "bool-logical-expr")
	SPECIALIZE_UTIL_WHAT(int_negation_expr, "int-negation-expr")
	SPECIALIZE_UTIL_WHAT(bool_negation_expr, "bool-negation-expr")
	SPECIALIZE_UTIL_WHAT(nonmeta_index_list, "nonmeta-index-list")
	SPECIALIZE_UTIL_WHAT(nonmeta_expr_list, "nonmeta-expr-list")
	SPECIALIZE_UTIL_WHAT(int_range_expr, "int-range-expr")
	SPECIALIZE_UTIL_WHAT(int_arith_loop_expr, "int-arith-loop-expr")
	SPECIALIZE_UTIL_WHAT(bool_logical_loop_expr, "bool-logical-loop-expr")
	SPECIALIZE_UTIL_WHAT(bool_return_cast_expr, "bool-cast-expr")
	SPECIALIZE_UTIL_WHAT(int_return_cast_expr, "int-cast-expr")

	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		int_arith_expr, NONMETA_INT_ARITH_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		int_relational_expr, NONMETA_INT_RELATIONAL_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		bool_logical_expr, NONMETA_BOOL_LOGICAL_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		int_negation_expr, NONMETA_INT_NEGATION_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		bool_negation_expr, NONMETA_BOOL_NEGATION_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		nonmeta_index_list, NONMETA_INDEX_LIST_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		nonmeta_expr_list, NONMETA_EXPR_LIST_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		int_range_expr, NONMETA_RANGE_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		int_arith_loop_expr, NONMETA_INT_ARITH_LOOP_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		bool_logical_loop_expr, NONMETA_BOOL_LOGICAL_LOOP_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		bool_return_cast_expr, BOOL_RETURN_CAST_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		int_return_cast_expr, INT_RETURN_CAST_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		real_return_cast_expr, REAL_RETURN_CAST_EXPR_TYPE_KEY, 0)
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		string_return_cast_expr, STRING_RETURN_CAST_EXPR_TYPE_KEY, 0)

namespace memory {
template class count_ptr<const HAC::entity::string_expr>;
}
}	// end namespace util

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using std::istream;
using util::persistent_traits;
using util::write_value;
using util::read_value;

//=============================================================================
// reverse-translation of type to tag_type for class_traits

template <class>
struct expr_tag;

template <>
struct expr_tag<bool_expr> {
	typedef	bool_tag	type;
};

template <>
struct expr_tag<int_expr> {
	typedef	int_tag		type;
};

template <>
struct expr_tag<real_expr> {
	typedef	real_tag	type;
};

template <>
struct expr_tag<string_expr> {
	typedef	string_tag	type;
};

//=============================================================================
// class int_expr method definitions

count_ptr<const nonmeta_index_expr_base>
int_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const nonmeta_index_expr_base>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_expr>
int_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const pint_const>
int_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const nonmeta_index_expr_base>& p) const {
	INVARIANT(p == this);
	return __nonmeta_resolve_rvalue(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
int_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return nonmeta_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluate, write, and advance iterator.  
 */
void
int_expr::evaluate_write(const nonmeta_context_base& c,
		channel_data_writer& w, 
		const count_ptr<const data_expr>& _this) const {
	STACKTRACE_VERBOSE;
	INVARIANT(_this == this);
	const count_ptr<const pint_const>
		d(__nonmeta_resolve_rvalue(c, _this.is_a<const this_type>()));
	if (d) {
		*w.iter_ref<int_tag>()++ = d->static_constant_value();
	} else {
		THROW_EXIT;
	}
}

//=============================================================================
// class bool_expr method definitions

count_ptr<const data_expr>
bool_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
bool_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return nonmeta_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluate, write, and advance iterator.  
 */
void
bool_expr::evaluate_write(const nonmeta_context_base& c,
		channel_data_writer& w, 
		const count_ptr<const data_expr>& _this) const {
	STACKTRACE_VERBOSE;
	INVARIANT(_this == this);
	const count_ptr<const pbool_const>
		d(__nonmeta_resolve_rvalue(c, _this.is_a<const this_type>()));
	if (d) {
		*w.iter_ref<bool_tag>()++ = d->static_constant_value();
	} else {
		THROW_EXIT;
	}
}

//=============================================================================
// class real_expr method definitions

count_ptr<const data_expr>
real_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
real_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return nonmeta_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluate, write, and advance iterator.  
 */
void
real_expr::evaluate_write(const nonmeta_context_base& c,
		channel_data_writer& w, 
		const count_ptr<const data_expr>& _this) const {
	STACKTRACE_VERBOSE;
	INVARIANT(_this == this);
	const count_ptr<const preal_const>
		d(__nonmeta_resolve_rvalue(c, _this.is_a<const this_type>()));
	if (d) {
#if 0
		*w.iter_ref<real_tag>()++ = d->static_constant_value();
#else
		FINISH_ME(Fang);
		cerr << "channel_data_writer lacks \'real\' fields." << endl;
#endif
	} else {
		THROW_EXIT;
	}
}

//=============================================================================
// class enum_expr method definitions

count_ptr<const data_expr>
enum_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
enum_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
#if 0
	return nonmeta_resolve_copy(c, p.is_a<const this_type>());
#else
	FINISH_ME(Fang);
	return count_ptr<const const_param>(NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Evaluate, write, and advance iterator.  
 */
void
enum_expr::evaluate_write(const nonmeta_context_base& c,
		channel_data_writer& w, 
		const count_ptr<const data_expr>& _this) const {
	STACKTRACE_VERBOSE;
	INVARIANT(_this == this);
	const count_ptr<const pint_const>
		d(__nonmeta_resolve_rvalue(c, _this.is_a<const this_type>()));
	if (d) {
		*w.iter_ref<enum_tag>()++ = d->static_constant_value();
	} else {
		THROW_EXIT;
	}
}

//=============================================================================
// class struct_expr method definitions

count_ptr<const data_expr>
struct_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const this_type>());
}

//=============================================================================
// class int_arith_expr method definitions

// static member initializations (order matters!)

const plus<int_value_type, int_value_type>	int_arith_expr::adder;
const minus<int_value_type, int_value_type>	int_arith_expr::subtractor;
const multiplies<int_value_type, int_value_type> int_arith_expr::multiplier;
const divides<int_value_type, int_value_type>	int_arith_expr::divider;
const modulus<int_value_type, int_value_type>	int_arith_expr::remainder;
const shift_left<int_value_type, int_value_type>	int_arith_expr::doubler;
const shift_right<int_value_type, int_value_type>	int_arith_expr::halver;
const bitwise_and<int_value_type, int_value_type>	int_arith_expr::masker;
const bitwise_xor<int_value_type, int_value_type>	int_arith_expr::hasher;
const bitwise_or<int_value_type, int_value_type>	int_arith_expr::unmasker;

const int_arith_expr::op_map_type
int_arith_expr::op_map;

const int_arith_expr::reverse_op_map_type
int_arith_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
int_arith_expr::op_map_size = int_arith_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
int_arith_expr::op_map_register(const op_key_type c, 
		const op_type* o, const char p) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[c] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = op_info(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered arithmetic operators.  
 */
size_t
int_arith_expr::op_map_init(void) {
	op_map_register('+', &adder, OP_PREC_PLUS);
	op_map_register('-', &subtractor, OP_PREC_PLUS);
	op_map_register('*', &multiplier, OP_PREC_TIMES);
	op_map_register('/', &divider, OP_PREC_TIMES);
	op_map_register('%', &remainder, OP_PREC_TIMES);
	op_map_register('<', &doubler, OP_PREC_SHIFT);
	op_map_register('>', &halver, OP_PREC_SHIFT);
	op_map_register('&', &masker, OP_PREC_BITWISE_AND);
	op_map_register('|', &unmasker, OP_PREC_BITWISE_OR);
	op_map_register('^', &hasher, OP_PREC_BITWISE_XOR);
	INVARIANT(op_map.size() == reverse_op_map.size());
	return op_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
	Default to adder (bogus), set op later during load.
 */
int_arith_expr::int_arith_expr() : lx(NULL), rx(NULL), op(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_arith_expr::~int_arith_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_arith_expr::int_arith_expr(const operand_ptr_type& l, const op_type* o,
		const operand_ptr_type& r) :
		lx(l), rx(r), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_arith_expr::int_arith_expr(const operand_ptr_type& l, const char o,
		const operand_ptr_type& r) :
		lx(l), rx(r), op(op_map.find(o)->second) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(int_arith_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_arith_expr::dump(ostream& o, const expr_dump_context& c) const {
	const op_info& oi(reverse_op_map.find(op)->second);
	const bool a = op->is_associative();
	const bool p = c.need_parentheses(oi.prec, a);
	const expr_dump_context::stamp_modifier m(c, oi.prec, a);
	if (p) o << '(';
	lx->dump(o, c);
	if (oi.op == '<' || oi.op == '>') {
		o << oi.op;		// for "<<", ">>"
	}
	o << oi.op;
	rx->dump(o, c);
	if (p) o << ')';
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
int_arith_expr::get_unresolved_data_type_ref(void) const {
	typedef	count_ptr<const data_type_reference>	return_type;
	const return_type lt(lx->get_unresolved_data_type_ref());
	const return_type rt(rx->get_unresolved_data_type_ref());
	if (!lt || !rt) {
		return return_type(NULL);
	}
	// check that they may be equivalent...
	// this call currently uses generic check, which is ok.
	if (lt->may_be_binop_type_equivalent(*rt)) {
		return lt;	// or rt, doesn't matter in this phase
	// idea: if one type is complete and resolvable, then prefer it.
	} else {
		cerr << "Operand types mismatch in int_arith_expr, got:"
			<< endl;
		lt->dump(cerr << "\tleft = ") << endl;
		rt->dump(cerr << "\tright = ") << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Can/should this use a stricter must-equivalence check?
 */
canonical_generic_datatype
int_arith_expr::get_resolved_data_type_ref(const unroll_context& c) const {
	typedef	canonical_generic_datatype	return_type;
	const return_type lt(lx->get_resolved_data_type_ref(c));
	const return_type rt(rx->get_resolved_data_type_ref(c));
	if (!lt || !rt) {
		return return_type();
	}
	// check that they may be equivalent...
	// this call currently uses generic check, which is ok.
	if (lt.must_be_connectibly_type_equivalent(rt)) {
		return lt;	// or rt, doesn't matter in this phase
	// idea: if one type is complete and resolvable, then prefer it.
	} else {
		cerr << "Operand types mismatch in int_arith_expr, got:"
			<< endl;
		lt.dump(cerr << "\tleft = ") << endl;
		rt.dump(cerr << "\tright = ") << endl;
		return return_type();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_arith_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const int_expr>
int_arith_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const int_expr>& p) const {
	typedef	count_ptr<const pint_const>	const_ptr_type;
	INVARIANT(p == this);
	const operand_ptr_type lc(lx->unroll_resolve_copy(c, lx));
	const operand_ptr_type rc(rx->unroll_resolve_copy(c, rx));
	if (!lc || !rc) {
		return count_ptr<const int_expr>(NULL);
	}
	const const_ptr_type lk(lc.is_a<const pint_const>());
	const const_ptr_type rk(rc.is_a<const pint_const>());
	if (lk && rk) {
		const pint_value_type lv = lk->static_constant_value();
		const pint_value_type rv = rk->static_constant_value();
		return const_ptr_type(new pint_const((*op)(lv, rv)));
	} else if (lc == lx && rc == rx) {
		// return self-copy
		return p;
	} else {
		// return new resolved expression
		return count_ptr<const this_type>(new this_type(lc, op, rc));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return resolved constant or NULL if resolution failed.  
 */
count_ptr<const pint_const>
int_arith_expr::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const int_expr>& p) const {
	typedef	count_ptr<const pint_const>	const_ptr_type;
	INVARIANT(p == this);
	const const_ptr_type lc(lx->__nonmeta_resolve_rvalue(c, lx));
	const const_ptr_type rc(rx->__nonmeta_resolve_rvalue(c, rx));
	if (!lc || !rc) {
		return const_ptr_type(NULL);
	} else {
		const pint_value_type lv = lc->static_constant_value();
		const pint_value_type rv = rc->static_constant_value();
		return const_ptr_type(new pint_const((*op)(lv, rv)));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
int_arith_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const int_expr>& p) const {
	return __nonmeta_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_arith_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_arith_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, reverse_op_map.find(op)->second.op);     // writes a character
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_arith_expr::load_object(const persistent_object_manager& m, istream& f) {
	{
	char o;
	read_value(f, o);
	const op_map_type::const_iterator i(op_map.find(o));
	INVARIANT(i != op_map.end());
	op = i->second;
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
}

//=============================================================================
// class int_relational_expr method definitions

const equal_to<bool_value_type, pint_value_type>
int_relational_expr::op_equal_to;

const not_equal_to<bool_value_type, pint_value_type>
int_relational_expr::op_not_equal_to;

const less<bool_value_type, pint_value_type>
int_relational_expr::op_less;

const greater<bool_value_type, pint_value_type>
int_relational_expr::op_greater;

const less_equal<bool_value_type, pint_value_type>
int_relational_expr::op_less_equal;

const greater_equal<bool_value_type, pint_value_type>
int_relational_expr::op_greater_equal;

const int_relational_expr::op_map_type
int_relational_expr::op_map;

const int_relational_expr::reverse_op_map_type
int_relational_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
int_relational_expr::op_map_size = int_relational_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
int_relational_expr::op_map_register(const string& s, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[s] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered relationalmetic operators.  
 */
size_t
int_relational_expr::op_map_init(void) {
	op_map_register("==", &op_equal_to);
	op_map_register("!=", &op_not_equal_to);
	op_map_register("<", &op_less);
	op_map_register(">", &op_greater);
	op_map_register("<=", &op_less_equal);
	op_map_register(">=", &op_greater_equal);
	INVARIANT(op_map.size() == reverse_op_map.size());
	return op_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.  
	Note: pass string, not null char.  
 */
int_relational_expr::int_relational_expr() :
		lx(NULL), rx(NULL), op(NULL) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_relational_expr::~int_relational_expr() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_relational_expr::int_relational_expr(const operand_ptr_type& l,
		const string& o, const operand_ptr_type& r) :
		lx(l), rx(r), op(op_map.find(o)->second) {
	INVARIANT(op_map.find(o) != op_map.end());
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_relational_expr::int_relational_expr(const operand_ptr_type& l,
		const op_type* o, const operand_ptr_type& r) :
		lx(l), rx(r), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(int_relational_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_relational_expr::dump(ostream& o, const expr_dump_context& c) const {
	return rx->dump(lx->dump(o, c) << reverse_op_map.find(op)->second, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	BUG: may_be_type_equivalent rejects pint vs. int comparison.  
		(last confirmed? test case?)
	TODO: replace get_unresolved_data_type_ref with nonmeta_inst_ref
		type_equivalence call directly.  
		Fixed by get_resolved_data_type_ref?
	\return NULL to signal error.  
 */
count_ptr<const data_type_reference>
int_relational_expr::get_unresolved_data_type_ref(void) const {
	typedef	count_ptr<const data_type_reference>	return_type;
	STACKTRACE_VERBOSE;
	const return_type lt(lx->get_unresolved_data_type_ref());
	const return_type rt(rx->get_unresolved_data_type_ref());
	if (!lt || !rt)
		return return_type(NULL);
	// check that they may be equivalent...
	// this call currently uses generic check, which is ok.
	if (lt->may_be_binop_type_equivalent(*rt)) {
		return bool_traits::built_in_type_ptr;
	// idea: if one type is complete and resolvable, then prefer it.
	} else {
		cerr << "Operand types mismatch in int_relational_expr, got:"
			<< endl;
		lt->dump(cerr << "\tleft = ") << endl;
		rt->dump(cerr << "\tright = ") << endl;
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_datatype
int_relational_expr::get_resolved_data_type_ref(const unroll_context& c) const {
	typedef	canonical_generic_datatype	return_type;
	STACKTRACE_VERBOSE;
	const return_type lt(lx->get_resolved_data_type_ref(c));
	const return_type rt(rx->get_resolved_data_type_ref(c));
	if (!lt || !rt)
		return return_type();
	// check that they may be equivalent...
	// this call currently uses generic check, which is ok.
	if (lt.must_be_connectibly_type_equivalent(rt)) {
		// allow comparisons of different width integers?
		// or requires explicit conversion?
		return lt;		// or rt, same.
	} else {
		cerr << "Operand types mismatch in int_relational_expr, got:"
			<< endl;
		lt.dump(cerr << "\tleft = ") << endl;
		rt.dump(cerr << "\tright = ") << endl;
		return return_type();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_relational_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const bool_expr>
int_relational_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const bool_expr>& p) const {
	INVARIANT(p == this);
	const operand_ptr_type lc(lx->unroll_resolve_copy(c, lx));
	const operand_ptr_type rc(rx->unroll_resolve_copy(c, rx));
	if (!lc || !rc) {
		return count_ptr<const bool_expr>(NULL);
	}
	if (lc == lx && rc == rx) {
		// return self-copy
		return p;
	} else {
		// return new resolved expression
		return count_ptr<const this_type>(new this_type(lc, op, rc));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return resolved constant or NULL if resolution failed.  
 */
count_ptr<const pbool_const>
int_relational_expr::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& p) const {
	typedef	count_ptr<const pint_const>	const_ptr_type;
	typedef	count_ptr<const pbool_const>	return_type;
	INVARIANT(p == this);
	const const_ptr_type lc(lx->__nonmeta_resolve_rvalue(c, lx));
	const const_ptr_type rc(rx->__nonmeta_resolve_rvalue(c, rx));
	if (!lc || !rc) {
		return return_type(NULL);
	} else {
		const pint_value_type lv = lc->static_constant_value();
		const pint_value_type rv = rc->static_constant_value();
		return return_type(new pbool_const((*op)(lv, rv)));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
int_relational_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& p) const {
	return __nonmeta_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_relational_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_relational_expr::write_object(const persistent_object_manager& m,
		ostream& f) const {
	write_value(f, reverse_op_map.find(op)->second);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_relational_expr::load_object(const persistent_object_manager& m, 
		istream& f) {
	{
	string s;
	read_value(f, s);
	const op_map_type::const_iterator i(op_map.find(s));
	INVARIANT(i != op_map.end());
	op = i->second;
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
}

//=============================================================================
// class bool_logical_expr method definitions

const util::logical_and<bool_value_type, bool_value_type>
bool_logical_expr::op_and;

const util::logical_or<bool_value_type, bool_value_type>
bool_logical_expr::op_or;

const util::logical_xor<bool_value_type, bool_value_type>
bool_logical_expr::op_xor;

const util::logical_xnor<bool_value_type, bool_value_type>
bool_logical_expr::op_xnor;

const bool_logical_expr::op_map_type
bool_logical_expr::op_map;

const bool_logical_expr::reverse_op_map_type
bool_logical_expr::reverse_op_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: will be initialized to 0 (POD -- plain old data) before 
		static objects will be constructed, then will initialized
		to its proper value.  
		Thus, this statement must follow initializations 
		of op_map and reverse_op_map.  
 */
const size_t
bool_logical_expr::op_map_size = bool_logical_expr::op_map_init();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of operator map.  
 */
void
bool_logical_expr::op_map_register(const op_key_type& s, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[s] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered logicalmetic operators.  
	TODO: maintain operators, in case we decide to change them, 
		hopefully more C-style to allow for logical and bitwise 
		operators. 
 */
size_t
bool_logical_expr::op_map_init(void) {
	op_map_register("&&", &op_and);
	op_map_register("||", &op_or);
	op_map_register("!=", &op_xor);
	op_map_register("==", &op_xnor);
	INVARIANT(op_map.size() == reverse_op_map.size());
	return op_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
bool_logical_expr::bool_logical_expr() : lx(NULL), rx(NULL), op(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_logical_expr::~bool_logical_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_logical_expr::bool_logical_expr(const operand_ptr_type& l,
		const op_key_type& o, const operand_ptr_type& r) :
		lx(l), rx(r), op(op_map.find(o)->second) {
	INVARIANT(op_map.find(o) != op_map.end());
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_logical_expr::bool_logical_expr(const operand_ptr_type& l,
		const op_type* o, const operand_ptr_type& r) :
		lx(l), rx(r), op(o) {
	NEVER_NULL(op);
	NEVER_NULL(lx);
	NEVER_NULL(rx);
	INVARIANT(lx->dimensions() == 0);
	INVARIANT(rx->dimensions() == 0);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(bool_logical_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_logical_expr::dump(ostream& o, const expr_dump_context& c) const {
	return rx->dump(lx->dump(o, c) << reverse_op_map.find(op)->second, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
bool_logical_expr::get_unresolved_data_type_ref(void) const {
	typedef	count_ptr<const data_type_reference>	return_type;
	const return_type lt(lx->get_unresolved_data_type_ref());
	const return_type rt(rx->get_unresolved_data_type_ref());
	if (!lt || !rt)
		return return_type(NULL);
	// check that they may be equivalent...
	// type's MAY actually be template dependent
	// so it's possible to defer, don't just assume they are boolean.
	// this call currently uses generic check, which is ok.
	if (lt->may_be_binop_type_equivalent(*rt))
		return bool_traits::built_in_type_ptr;
	// idea: if one type is complete and resolvable, then prefer it.
	else	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Passing an unroll context for this seems rather silly, no?
	Will a future situation ever need it?
 */
canonical_generic_datatype
bool_logical_expr::get_resolved_data_type_ref(const unroll_context& c) const {
	typedef canonical_generic_datatype	return_type;
	const return_type lt(lx->get_resolved_data_type_ref(c));
	const return_type rt(rx->get_resolved_data_type_ref(c));
	if (!lt || !rt)
		return return_type();
	// check that they may be equivalent...
	// type's MAY actually be template dependent
	// so it's possible to defer, don't just assume they are boolean.
	// this call currently uses generic check, which is ok.
	if (lt.must_be_connectibly_type_equivalent(rt))
		return lt;	// or rt, same.
	// idea: if one type is complete and resolvable, then prefer it.
	else	return return_type();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_logical_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const bool_expr>
bool_logical_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const bool_expr>& p) const {
	INVARIANT(p == this);
	const operand_ptr_type lc(lx->unroll_resolve_copy(c, lx));
	const operand_ptr_type rc(rx->unroll_resolve_copy(c, rx));
	if (!lc || !rc) {
		return count_ptr<const bool_expr>(NULL);
	} else if (lc == lx && rc == rx) {
		// return self-copy
		return p;
	} else {
		// return new resolved expression
		return count_ptr<const this_type>(new this_type(lc, op, rc));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return resolved constant or NULL if resolution failed.  
 */
count_ptr<const pbool_const>
bool_logical_expr::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& p) const {
	typedef	count_ptr<const pbool_const>	const_ptr_type;
	INVARIANT(p == this);
	const const_ptr_type lc(lx->__nonmeta_resolve_rvalue(c, lx));
	const const_ptr_type rc(rx->__nonmeta_resolve_rvalue(c, rx));
	if (!lc || !rc) {
		return const_ptr_type(NULL);
	} else {
		const pbool_value_type lv = lc->static_constant_value();
		const pbool_value_type rv = rc->static_constant_value();
		return const_ptr_type(new pbool_const((*op)(lv, rv)));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
bool_logical_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& p) const {
	return __nonmeta_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_logical_expr::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	lx->collect_transient_info(m);
	rx->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_logical_expr::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	write_value(f, reverse_op_map.find(op)->second);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_logical_expr::load_object(const persistent_object_manager& m, istream& f) {
	{
	string s;
	read_value(f, s);
	const op_map_type::const_iterator i(op_map.find(s));
	INVARIANT(i != op_map.end());
	op = i->second;
	}
	m.read_pointer(f, lx);
	m.read_pointer(f, rx);
}

//=============================================================================
// class int_negation_expr method definitions

int_negation_expr::int_negation_expr() : parent_type(), ex() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_negation_expr::int_negation_expr(const operand_ptr_type& e, 
		const char o) :
		parent_type(), ex(e), op(o) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_negation_expr::~int_negation_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(int_negation_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_negation_expr::dump(ostream& o, const expr_dump_context& c) const {
	return ex->dump(o << op, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
int_negation_expr::get_unresolved_data_type_ref(void) const {
	return ex->get_unresolved_data_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_datatype
int_negation_expr::get_resolved_data_type_ref(const unroll_context& c) const {
	return ex->get_resolved_data_type_ref(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_negation_expr::accept(nonmeta_expr_visitor& v) const {
	// cast diambiguates between data_expr and nonmeta_index_expr_base
	// v.visit(static_cast<const data_expr&>(*this));
	// v.visit(static_cast<const nonmeta_index_expr_base&>(*this));
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const int_expr>
int_negation_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const int_expr>& p) const {
	INVARIANT(p == this);
	const operand_ptr_type oc(ex->unroll_resolve_copy(c, ex));
	if (!oc) {
		return count_ptr<const int_expr>(NULL);
	} else if (oc == ex) {
		return p;
	} else {
		return count_ptr<const this_type>(new this_type(oc, op));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return resolved constant or NULL if resolution failed.  
 */
count_ptr<const pint_const>
int_negation_expr::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const int_expr>& p) const {
	typedef	count_ptr<const pint_const>	const_ptr_type;
	typedef	const_ptr_type			return_type;
	INVARIANT(p == this);
	const const_ptr_type lc(ex->__nonmeta_resolve_rvalue(c, ex));
	if (!lc) {
		return return_type(NULL);
	} else {
		const pint_value_type lv = lc->static_constant_value();
		return return_type(new pint_const((op == '-') ? -lv : ~lv));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
int_negation_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const int_expr>& p) const {
	return __nonmeta_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_negation_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	ex->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_negation_expr::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, ex);
	write_value(o, op);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_negation_expr::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, ex);
	read_value(i, op);
}

//=============================================================================
// class bool_negation_expr method definitions

bool_negation_expr::bool_negation_expr() : parent_type(), ex() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_negation_expr::bool_negation_expr(const operand_ptr_type& e) :
		parent_type(), ex(e) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_negation_expr::~bool_negation_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(bool_negation_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_negation_expr::dump(ostream& o, const expr_dump_context& c) const {
	return ex->dump(o << '~', c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
bool_negation_expr::get_unresolved_data_type_ref(void) const {
	return ex->get_unresolved_data_type_ref();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
canonical_generic_datatype
bool_negation_expr::get_resolved_data_type_ref(const unroll_context& c) const {
	return ex->get_resolved_data_type_ref(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_negation_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const bool_expr>
bool_negation_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const bool_expr>& p) const {
	INVARIANT(p == this);
	const operand_ptr_type oc(ex->unroll_resolve_copy(c, ex));
	if (!oc) {
		return count_ptr<const bool_expr>(NULL);
	} else if (oc == ex) {
		return p;
	} else {
		return count_ptr<const this_type>(new this_type(oc));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return resolved constant or NULL if resolution failed.  
 */
count_ptr<const pbool_const>
bool_negation_expr::__nonmeta_resolve_rvalue(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& p) const {
	typedef	count_ptr<const pbool_const>	const_ptr_type;
	typedef	const_ptr_type			return_type;
	INVARIANT(p == this);
	const const_ptr_type lc(ex->__nonmeta_resolve_rvalue(c, ex));
	if (!lc) {
		return return_type(NULL);
	} else {
		const pbool_value_type lv = lc->static_constant_value();
		return return_type(new pbool_const(!lv));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
bool_negation_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const bool_expr>& p) const {
	return __nonmeta_resolve_rvalue(c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_negation_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	ex->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_negation_expr::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, ex);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_negation_expr::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, ex);
}

//=============================================================================
// class int_range_expr method definitions

// empty constructor should really be private
int_range_expr::int_range_expr() : parent_type(), lower(), upper() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_range_expr::int_range_expr(const bound_ptr_type& l, 
		const bound_ptr_type& u) : 
		parent_type(), lower(l), upper(u) {
	NEVER_NULL(lower);
	NEVER_NULL(upper);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_range_expr::~int_range_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(int_range_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_range_expr::dump(ostream& o, const expr_dump_context&) const {
	return upper->what(lower->what(o << '[') << "..") << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_range_expr::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const nonmeta_index_expr_base>
int_range_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const nonmeta_index_expr_base>& p) const {
	INVARIANT(p == this);
	const bound_ptr_type lc(lower->unroll_resolve_copy(c, lower));
	const bound_ptr_type uc(upper->unroll_resolve_copy(c, upper));
	if (!uc || !lc) {
		return count_ptr<const nonmeta_index_expr_base>(NULL);
	}
	if (uc == lower && lc == upper) {
		// return self-copy
		return p;
	} else {
		// return new resolved expression
		return count_ptr<const this_type>(new this_type(lc, uc));
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return NULL No nonmeta ranges exist.
 */
count_ptr<const pint_const>
int_range_expr::nonmeta_resolve_copy(const nonmeta_context_base&, 
		const count_ptr<const nonmeta_index_expr_base>& p) const {
	return count_ptr<const pint_const>(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_range_expr::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	upper->collect_transient_info(m);
	lower->collect_transient_info(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_range_expr::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, lower);
	m.write_pointer(o, upper);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_range_expr::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, lower);
	m.read_pointer(i, upper);
	// don't bother loading here, let object manager do it
}

//=============================================================================
// class nonmeta_index_list method definitions

nonmeta_index_list::nonmeta_index_list() : persistent(), list_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s is just a size to reserve, not the number of initial elements!
 */
nonmeta_index_list::nonmeta_index_list(const size_t s) :
		persistent(), list_type() {
	util::reserve(AS_A(list_type&, *this), s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
nonmeta_index_list::~nonmeta_index_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(nonmeta_index_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
nonmeta_index_list::dimensions_collapsed(void) const {
	size_t ret = 0;
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		if (i->is_a<const int_expr>())
			ret++;
		else INVARIANT(i->is_a<const int_range_expr>());
			// sanity check
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
nonmeta_index_list::dump(ostream& o, const expr_dump_context& c) const {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		const count_ptr<const int_expr>
			b(i->is_a<const int_expr>());
		if (b)
			b->dump(o << '[', c) << ']';
		else    (*i)->dump(o, c);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For now nonmeta-index lists may not contain ranges, 
		only integer indices.  
	\return list of integer constants representing indices.  
 */
count_ptr<const_index_list>
nonmeta_index_list::nonmeta_resolve_copy(const nonmeta_context_base& c) const {
	const count_ptr<const_index_list> ret(new const_index_list);
	NEVER_NULL(ret);
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		const count_ptr<const pint_const> 
			ind((*i)->nonmeta_resolve_copy(c, *i));
		if (ind)
			ret->push_back(ind);
		else	return count_ptr<const_index_list>(NULL);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_index_list::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Produces a list with unroll-resolved values of indices. 
	The caller can compare this-object to the result by doing
	a pointer-sequence compare to determine whether or not the
	result is actually different, as a result of resolving meta-values.  
	For efficiency, when the result is identical to the argument, 
	the (non-mutable) reference argument can be shared to save memory.  
	\return nonmeta index list with resolved indices.  
 */
count_ptr<nonmeta_index_list>
nonmeta_index_list::unroll_resolve_copy(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const count_ptr<this_type> ret(new this_type(this->size()));
	NEVER_NULL(ret);
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
		const value_type v((*i)->unroll_resolve_copy(c, *i));
		NEVER_NULL(v);
		ret->push_back(v);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attempts to construct a const_index_list from a resolved
	nonmeta_index_list.  
	Also check that none of the members are ranges, 
		i.e. all dimensions are collapsed.  
	\param l the multi-index, already sized properly.  
	\return If any members of list are not integer constants, then
		return NULL, signaling (non-fatal) failure.  
 */
good_bool
nonmeta_index_list::make_const_index_list(multikey_index_type& l) const {
	STACKTRACE_VERBOSE;
	INVARIANT(l.dimensions() == size());
	const_iterator i(begin());
	const const_iterator e(end());
	size_t j = 0;
	for ( ; i!=e; ++i, ++j) {
		const count_ptr<const pint_const>
			ic(i->is_a<const pint_const>());
		if (ic) {
			l[j] = ic->static_constant_value();
		} else {
			return good_bool(false);
		}
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Attempts to resolve a meta index list from a nonmeta index list.  
 */
count_ptr<dynamic_meta_index_list>
nonmeta_index_list::make_meta_index_list(void) const {
	typedef	count_ptr<dynamic_meta_index_list>	return_type;
	const return_type ret(new dynamic_meta_index_list);
	NEVER_NULL(ret);
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		const count_ptr<const meta_index_expr>
			mie(i->is_a<const meta_index_expr>());
		if (mie)
			ret->push_back(mie);
		else return return_type(NULL);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
nonmeta_index_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
#if PARANOID
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
	}
#endif
	m.collect_pointer_list(*this);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
nonmeta_index_list::write_object(const persistent_object_manager& m,
		ostream& f) const {
#if PARANOID
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
	}
#endif
	m.write_pointer_list(f, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
nonmeta_index_list::load_object(const persistent_object_manager& m,
		istream& f) {
	m.read_pointer_list(f, *this);
#if 1
	// is this really necessary?
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		const count_ptr<const nonmeta_index_expr_base> ip(*i);
		NEVER_NULL(ip);
//		if (ip) {
		m.load_object_once(&const_cast<nonmeta_index_expr_base&>(*ip));
//		}
	}
#endif
}

//=============================================================================
// class nonmeta_expr_list method definitions

nonmeta_expr_list::nonmeta_expr_list() : persistent(), list_type() { }

nonmeta_expr_list::~nonmeta_expr_list() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(nonmeta_expr_list)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
nonmeta_expr_list::dump(ostream& o, const expr_dump_context& c) const {
	const_iterator i(begin());
	const const_iterator e(end());
if (i!=e) {
	NEVER_NULL(*i);
	(*i)->dump(o, c);
	for (++i; i!=e; ++i) {
		NEVER_NULL(*i);
		(*i)->dump(o << ',', c);
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
nonmeta_expr_list::accept(nonmeta_expr_visitor& v) const {
	v.visit(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const nonmeta_expr_list>
nonmeta_expr_list::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const this_type>& p) const {
	STACKTRACE_VERBOSE;
	INVARIANT(p == this);
	const count_ptr<this_type> ret(new this_type);	// reserve size?
	NEVER_NULL(ret);
	const_iterator i(begin());
	const const_iterator e(end());
	transform(i, e, back_inserter(*ret), data_expr::unroller(c));
	if (equal(i, e, ret->begin())) {
		return p;
	} else {
		return ret;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param_expr_list>
nonmeta_expr_list::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const this_type>&) const {
	typedef	count_ptr<const_param_expr_list>	return_type;
	const return_type ret(new const_param_expr_list);
	NEVER_NULL(ret);
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		const count_ptr<const const_param> 
			v((*i)->nonmeta_resolve_copy(c, *i));
		if (v)
			ret->push_back(v);
		else	return return_type(NULL);
	}
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
nonmeta_expr_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
#if PARANOID
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
	}
#endif
	m.collect_pointer_list(*this);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Serialize this object into an output stream, translating
	pointers to indices as they are encountered.  
 */
void
nonmeta_expr_list::write_object(const persistent_object_manager& m,
		ostream& f) const {
#if PARANOID
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; ++i) {
		NEVER_NULL(*i);
	}
#endif
	m.write_pointer_list(f, *this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Load the object from a serial input stream, translating
	indices to pointers in the reconstruction.  
 */
void
nonmeta_expr_list::load_object(const persistent_object_manager& m,
		istream& f) {
	m.read_pointer_list(f, *this);
#if 1
	// is this really necessary?
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		const count_ptr<const data_expr>& ip(*i);
		NEVER_NULL(ip);
		m.load_object_once(&const_cast<data_expr&>(*ip));
	}
#endif
}

//=============================================================================
// class string_expr method definitions

count_ptr<const data_expr>
string_expr::unroll_resolve_copy(const unroll_context&, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return p;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const const_param>
string_expr::nonmeta_resolve_copy(const nonmeta_context_base& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	// should really static_cast...
	return nonmeta_resolve_copy(c, p.is_a<const this_type>());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
string_expr::evaluate_write(const nonmeta_context_base&, 
		channel_data_writer&, const count_ptr<const data_expr>&) const {
	STACKTRACE_VERBOSE;
	FINISH_ME_EXIT(Fang);
}

//=============================================================================
// explicit template instantiations

// steal nonmeta_value_references from 
// "Object/expr/nonmeta_param_value_reference.cc"?

template class loop_nonmeta_expr<int_arith_expr>;
template class loop_nonmeta_expr<bool_logical_expr>;

template class nonmeta_cast_expr<int_expr, nonmeta_func_call>;
template class nonmeta_cast_expr<bool_expr, nonmeta_func_call>;
template class nonmeta_cast_expr<real_expr, nonmeta_func_call>;
template class nonmeta_cast_expr<string_expr, nonmeta_func_call>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#undef	PARANOID

DEFAULT_STATIC_TRACE_END

