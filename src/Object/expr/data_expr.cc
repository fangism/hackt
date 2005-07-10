/**
	\file "Object/expr/data_expr.cc"
	Implementation of data expression classes.  
	NOTE: file was moved from "Object/art_objec_data_expr.cc"
	$Id: data_expr.cc,v 1.1.2.4 2005/07/10 21:11:22 fang Exp $
 */

#include <iostream>
#include "Object/expr/int_arith_expr.h"
#include "Object/expr/int_relational_expr.h"
#include "Object/expr/bool_logical_expr.h"
#include "Object/expr/int_negation_expr.h"
#include "Object/expr/bool_negation_expr.h"
#include "Object/expr/int_range_expr.h"
#include "Object/expr/nonmeta_index_list.h"
#include "Object/expr/int_range_list.h"

#include "Object/art_object_type_hash.h"
#include "Object/art_object_nonmeta_value_reference.tcc"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_bool_traits.h"
// #include "Object/art_object_int_traits.h"

#include "util/reserve.h"
#include "util/persistent_object_manager.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/what.h"
#include "util/IO_utils.h"

namespace util {
using ART::entity::int_arith_expr;
using ART::entity::int_relational_expr;
using ART::entity::bool_logical_expr;
using ART::entity::int_negation_expr;
using ART::entity::bool_negation_expr;
using ART::entity::nonmeta_index_list;
using ART::entity::int_range_expr;

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

namespace ART {
namespace entity {
using std::istream;
using util::persistent_traits;
using util::write_value;
using util::read_value;
//=============================================================================
// class int_arith_expr method definitions

// static member initializations (order matters!)

const plus<int_value_type, int_value_type>	int_arith_expr::adder;
const minus<int_value_type, int_value_type>	int_arith_expr::subtractor;
const multiplies<int_value_type, int_value_type> int_arith_expr::multiplier;
const divides<int_value_type, int_value_type>	int_arith_expr::divider;
const modulus<int_value_type, int_value_type>	int_arith_expr::remainder;

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
int_arith_expr::op_map_register(const char c, const op_type* o) {
	NEVER_NULL(o);
	const_cast<op_map_type&>(op_map)[c] = o;
	const_cast<reverse_op_map_type&>(reverse_op_map)[o] = c;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Static initialization of registered arithmetic operators.  
 */
size_t
int_arith_expr::op_map_init(void) {
	op_map_register('+', &adder);
	op_map_register('-', &subtractor);
	op_map_register('*', &multiplier);
	op_map_register('/', &divider);
	op_map_register('%', &remainder);
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
int_arith_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_arith_expr::dump_brief(ostream& o) const {
	return rx->dump_brief(lx->dump_brief(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
int_arith_expr::get_data_type_ref(void) const {
	typedef	count_ptr<const data_type_reference>	return_type;
	const return_type lt(lx->get_data_type_ref());
	const return_type rt(rx->get_data_type_ref());
	if (!lt || !rt)
		return return_type(NULL);
	// check that they may be equivalent...
	// this call currently uses generic check, which is ok.
	if (lt->may_be_connectibly_type_equivalent(*rt)) {
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
	write_value(f, reverse_op_map[op]);     // writes a character
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
int_relational_expr::dump_brief(ostream& o) const {
	return rx->dump_brief(lx->dump_brief(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_relational_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	BUG: may_be_type_equivalent rejects pint vs. int comparison.  
 */
count_ptr<const data_type_reference>
int_relational_expr::get_data_type_ref(void) const {
	typedef	count_ptr<const data_type_reference>	return_type;
	const return_type lt(lx->get_data_type_ref());
	const return_type rt(rx->get_data_type_ref());
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
	op_map_register("&", &op_and);
	op_map_register("|", &op_or);
	op_map_register("^", &op_xor);
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
bool_logical_expr::dump_brief(ostream& o) const {
	return rx->dump_brief(lx->dump_brief(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_logical_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << reverse_op_map[op]);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
bool_logical_expr::get_data_type_ref(void) const {
	typedef	count_ptr<const data_type_reference>	return_type;
	const return_type lt(lx->get_data_type_ref());
	const return_type rt(rx->get_data_type_ref());
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
int_negation_expr::int_negation_expr(const operand_ptr_type& e) :
		parent_type(), ex(e) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int_negation_expr::~int_negation_expr() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(int_negation_expr)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_negation_expr::dump(ostream& o) const {
	return ex->dump(o << '~');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
int_negation_expr::dump_brief(ostream& o) const {
	return ex->dump_brief(o << '~');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
int_negation_expr::get_data_type_ref(void) const {
	return ex->get_data_type_ref();
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
int_negation_expr::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, ex);
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
bool_negation_expr::dump(ostream& o) const {
	return ex->dump(o << '~');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
bool_negation_expr::dump_brief(ostream& o) const {
	return ex->dump_brief(o << '~');
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
count_ptr<const data_type_reference>
bool_negation_expr::get_data_type_ref(void) const {
	return ex->get_data_type_ref();
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
int_range_expr::dump(ostream& o) const {
	return upper->what(lower->what(o << '[') << "..") << ']';
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
nonmeta_index_list::dump(ostream& o) const {
	const_iterator i(begin());
	const const_iterator e(end());
	for ( ; i!=e; i++) {
		NEVER_NULL(*i);
		const count_ptr<const int_expr>
			b(i->is_a<const int_expr>());
		if (b)
#if 0
			// don't have yet
			b->dump_brief(o << '[') << ']';
#else
			b->dump(o << '[') << ']';
#endif
		else    (*i)->dump(o);
	}
	return o;
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
}	// end namespace ART

