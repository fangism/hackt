/**
	\file "Object/expr/data_expr.cc"
	Implementation of data expression classes.  
	NOTE: file was moved from "Object/art_object_data_expr.cc"
	$Id: data_expr.cc,v 1.9.6.2 2006/09/06 04:02:26 fang Exp $
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/expr/int_arith_expr.h"
#include "Object/expr/int_relational_expr.h"
#include "Object/expr/bool_logical_expr.h"
#include "Object/expr/int_negation_expr.h"
#include "Object/expr/bool_negation_expr.h"
#include "Object/expr/int_range_expr.h"
#include "Object/expr/real_expr.h"
#include "Object/expr/enum_expr.h"
#include "Object/expr/struct_expr.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/int_range_list.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/operator_precedence.h"
#include "Object/expr/const_index_list.h"
#include "Object/expr/pint_const.h"

#include "Object/persistent_type_hash.h"
#include "Object/type/data_type_reference.h"
#if USE_RESOLVED_DATA_TYPES
#include "Object/type/canonical_generic_datatype.h"
#endif
#include "Object/traits/bool_traits.h"

#include "util/reserve.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/what.h"
#include "util/IO_utils.h"
#include "util/stacktrace.h"
#include "util/multikey.h"

namespace util {
using HAC::entity::int_arith_expr;
using HAC::entity::int_relational_expr;
using HAC::entity::bool_logical_expr;
using HAC::entity::int_negation_expr;
using HAC::entity::bool_negation_expr;
using HAC::entity::nonmeta_index_list;
using HAC::entity::int_range_expr;

	SPECIALIZE_UTIL_WHAT(int_arith_expr, "int-arith-expr")
	SPECIALIZE_UTIL_WHAT(int_relational_expr, "int-relatonal-expr")
	SPECIALIZE_UTIL_WHAT(bool_logical_expr, "bool-logical-expr")
	SPECIALIZE_UTIL_WHAT(int_negation_expr, "int-negation-expr")
	SPECIALIZE_UTIL_WHAT(bool_negation_expr, "bool-negation-expr")
	SPECIALIZE_UTIL_WHAT(nonmeta_index_list, "nonmeta-index-list")
	SPECIALIZE_UTIL_WHAT(int_range_expr, "int-range-expr")

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
		int_range_expr, NONMETA_RANGE_TYPE_KEY, 0)
}	// end namespace util

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using std::istream;
using util::persistent_traits;
using util::write_value;
using util::read_value;

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

//=============================================================================
// class bool_expr method definitions

count_ptr<const data_expr>
bool_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const this_type>());
}

//=============================================================================
// class real_expr method definitions

count_ptr<const data_expr>
real_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const this_type>());
}

//=============================================================================
// class enum_expr method definitions

count_ptr<const data_expr>
enum_expr::unroll_resolve_copy(const unroll_context& c, 
		const count_ptr<const data_expr>& p) const {
	INVARIANT(p == this);
	return unroll_resolve_copy(c, p.is_a<const this_type>());
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
int_arith_expr::op_map_register(const char c, const op_type* o, const char p) {
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
		lx(l), rx(r), op(op_map[o]) {
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
#if 1
	const op_info& oi(reverse_op_map[op]);
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
#else
	return rx->dump(lx->dump(o, c) << reverse_op_map[op], c);
#endif
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
#if USE_RESOLVED_DATA_TYPES
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
#endif

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
	write_value(f, reverse_op_map[op].op);     // writes a character
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_arith_expr::load_object(const persistent_object_manager& m, istream& f) {
	{
	char o;
	read_value(f, o);
	op = op_map[o];
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
		lx(l), rx(r), op(op_map[o]) {
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
	return rx->dump(lx->dump(o, c) << reverse_op_map[op], c);
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
	if (lt->may_be_connectibly_type_equivalent(*rt)) {
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
#if USE_RESOLVED_DATA_TYPES
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
#endif

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
	write_value(f, reverse_op_map[op]);
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
	op = op_map[s];
	NEVER_NULL(op);
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
bool_logical_expr::op_map_register(const string& s, const op_type* o) {
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
		const string& o, const operand_ptr_type& r) :
		lx(l), rx(r), op(op_map[o]) {
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
	return rx->dump(lx->dump(o, c) << reverse_op_map[op], c);
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
	if (lt->may_be_connectibly_type_equivalent(*rt))
		return bool_traits::built_in_type_ptr;
	// idea: if one type is complete and resolvable, then prefer it.
	else	return return_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_RESOLVED_DATA_TYPES
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
#endif

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
	write_value(f, reverse_op_map[op]);
	m.write_pointer(f, lx);
	m.write_pointer(f, rx);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_logical_expr::load_object(const persistent_object_manager& m, istream& f) {
	{
	string s;
	read_value(f, s);
	op = op_map[s];
	NEVER_NULL(op);
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
#if USE_RESOLVED_DATA_TYPES
canonical_generic_datatype
int_negation_expr::get_resolved_data_type_ref(const unroll_context& c) const {
	return ex->get_resolved_data_type_ref(c);
}
#endif

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
#if USE_RESOLVED_DATA_TYPES
canonical_generic_datatype
bool_negation_expr::get_resolved_data_type_ref(const unroll_context& c) const {
	return ex->get_resolved_data_type_ref(c);
}
#endif

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
		ret->push_back((*i)->unroll_resolve_copy(c, *i));
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
	Recursively visits pointer list to register expression
	objects with the persistent object manager.
 */
void
nonmeta_index_list::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
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
		m.load_object_once(&const_cast<nonmeta_index_expr_base&>(*ip));
	}
#endif
}

//=============================================================================
// explicit template instantiations

// steal nonmeta_value_references from 
// "Object/expr/nonmeta_param_value_reference.cc"?

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

