// "art_object_expr.cc"

#include <stdlib.h>			// for ltoa
#include <assert.h>
#include <iostream>

#include "sstream.h"			// for ostringstring, used by dump
#include "discrete_interval_set.h"

#include "art_parser_base.h"
#include "art_object.h"			// before including "art_object_expr.h"
#include "art_object_expr.h"

namespace ART {
namespace entity {
//=============================================================================
using namespace std;

// forward declarations (table of contents)
class param_expr;
class index_expr;
class const_index;
class pbool_expr;
class pint_expr;
class param_expr_collective;
class param_literal;
class pbool_literal;
class pint_literal;
class pint_const;
class pbool_const;
class param_unary_expr;
class pint_unary_expr;
class pbool_unary_expr;
class param_binary_expr;
class arith_expr;
class relational_expr;
class logical_expr;
class range_expr;
class pint_range;
class const_range;
class range_expr_list;
class const_range_list;
class dynamic_range_list;
class unconditional_range_list;
class conditional_range_list;
class loop_range_list;
class index_list;
class const_index_list;
class dynamic_index_list;

//=============================================================================
// class param_expr method_definitions

// inline
param_expr::param_expr() : object() { }

// inline
param_expr::~param_expr() { }

//-----------------------------------------------------------------------------
// class const_param method definitions

// inline
const_param::const_param() : param_expr() { }

// inline
const_param::~const_param() { }

//-----------------------------------------------------------------------------
// class pbool_expr method definitions

bool
pbool_expr::may_be_equivalent(const param_expr& p) const {
	const pbool_expr* b = IS_A(const pbool_expr*, &p);
	if (b) {
		if (is_static_constant() && b->is_static_constant())
			return static_constant_bool() ==
				b->static_constant_bool();
		else	return true;
	}
	else	return false;
}

bool
pbool_expr::must_be_equivalent(const param_expr& p) const {
	const pbool_expr* b = IS_A(const pbool_expr*, &p);
	if (b) {
		if (is_static_constant() && b->is_static_constant())
			return static_constant_bool() ==
				b->static_constant_bool();
		else	return false;
	}
	else	return false;
}

/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
 */
count_const_ptr<const_param>
pbool_expr::static_constant_param(void) const {
	return count_const_ptr<const_param>(
		new pbool_const(static_constant_bool()));
}

//-----------------------------------------------------------------------------
// class pint_expr method definitions

bool
pint_expr::may_be_equivalent(const param_expr& p) const {
	const pint_expr* i = IS_A(const pint_expr*, &p);
	if (i) {
		if (is_static_constant() && i->is_static_constant())
			return static_constant_int() ==
				i->static_constant_int();
		else	return true;
	}
	else	return false;
}

bool
pint_expr::must_be_equivalent(const param_expr& p) const {
	const pint_expr* i = IS_A(const pint_expr*, &p);
	if (i) {
		if (is_static_constant() && i->is_static_constant())
			return static_constant_int() ==
				i->static_constant_int();
		else	return false;
	}
	else	return false;
}

/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
 */
count_const_ptr<const_param>
pint_expr::static_constant_param(void) const {
	return count_const_ptr<const_param>(
		new pint_const(static_constant_int()));
}

//-----------------------------------------------------------------------------
// class param_expr_list method definitions

param_expr_list::param_expr_list() : object() { }

param_expr_list::~param_expr_list() { }

#if 0
bool
param_expr_list::may_be_equivalent(const param_expr_list& p) const {
	const list<const param_expr&> this_list(get_const_ref_list());
	const list<const param_expr&> other_list(p.get_const_ref_list());
	if (this_list.size() != other_list.size())
		return false;
	const_iterator i = this_list.begin();
	const_iterator j = other_list.begin();
	for ( ; i!=this_list.end(); i++, j++) {
		if (!i->may_be_equivalent(*j))
			return false;
		// else continue checking...
	}
	assert(j == other_list.end());		// sanity
	return true;
}

bool
param_expr_list::must_be_equivalent(const param_expr_list& p) const {
	const list<const param_expr&> this_list(get_const_ref_list());
	const list<const param_expr&> other_list(p.get_const_ref_list());
	if (this_list.size() != other_list.size())
		return false;
	const_iterator i = this_list.begin();
	const_iterator j = other_list.begin();
	for ( ; i!=this_list.end(); i++, j++) {
		if (!i->must_be_equivalent(*j))
			return false;
		// else continue checking...
	}
	assert(j == p.end());		// sanity
	return true;
}
#endif

//-----------------------------------------------------------------------------
// class const_param_expr_list method definitions

const_param_expr_list::const_param_expr_list() :
		param_expr_list(), parent() { }

const_param_expr_list::~const_param_expr_list() { }

ostream&
const_param_expr_list::what(ostream& o) const {
	return o << "const-param-expr-list";
}

ostream&
const_param_expr_list::dump(ostream& o) const {
	if (empty()) return o;
	// else at least 1 item in list
	const_iterator i = begin();
	assert(*i);
	(*i)->dump(o);
	for (i++; i!=end(); i++) {
		o << ", ";
		assert(*i);
		(*i)->dump(o);
	}
	return o;
}

size_t
const_param_expr_list::size(void) const {
	return parent::size();
}

excl_ptr<param_expr_list>
const_param_expr_list::make_copy(void) const {
	return excl_ptr<param_expr_list>(
		new const_param_expr_list(*this));
	// use default copy constructor
}

bool
const_param_expr_list::may_be_initialized(void) const {
	return true;
}

bool
const_param_expr_list::must_be_initialized(void) const {
	return true;
}

#if 0
/**
	Precondition: all expressions must be non-NULL.  
 */
list<const param_expr&>
const_param_expr_list::get_const_ref_list(void) const {
	list<const param_expr&> ret;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		ret.push_back(**i);
	}
	return ret;
}
#endif

bool
const_param_expr_list::may_be_equivalent(const param_expr_list& p) const {
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &p);
if (cpl) {
	if (size() != cpl->size())
		return false;
	const_iterator i = begin();
	const_iterator j = cpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_const_ptr<const_param> ip(*i);
		const count_const_ptr<const_param> jp(*j);
		assert(ip && jp);
		if (!ip->may_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	assert(j == cpl->end());		// sanity
	return true;
} else {
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &p);
	assert(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i = begin();
	dynamic_param_expr_list::const_iterator j = dpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_const_ptr<const_param> ip(*i);
		const count_const_ptr<param_expr> jp(*j);
		assert(ip && jp);
		if (!ip->may_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	assert(j == dpl->end());		// sanity
	return true;
}
}

bool
const_param_expr_list::must_be_equivalent(const param_expr_list& p) const {
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &p);
if (cpl) {
	if (size() != cpl->size())
		return false;
	const_iterator i = begin();
	const_iterator j = cpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_const_ptr<const_param> ip(*i);
		const count_const_ptr<const_param> jp(*j);
		assert(ip && jp);
		if (!ip->must_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	assert(j == cpl->end());		// sanity
	return true;
} else {
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &p);
	assert(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i = begin();
	dynamic_param_expr_list::const_iterator j = dpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_const_ptr<const_param> ip(*i);
		const count_const_ptr<param_expr> jp(*j);
		assert(ip && jp);
		if (!ip->must_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	assert(j == dpl->end());		// sanity
	return true;
}
}

//-----------------------------------------------------------------------------
// class dynamic_param_expr_list method definitions

dynamic_param_expr_list::dynamic_param_expr_list() :
		param_expr_list(), parent() { }

dynamic_param_expr_list::~dynamic_param_expr_list() { }

ostream&
dynamic_param_expr_list::what(ostream& o) const {
	return o << "param-expr-list";
}

ostream&
dynamic_param_expr_list::dump(ostream& o) const {
	if (empty()) return o;
	// else at least 1 item in list
	const_iterator i = begin();
	if (*i)	(*i)->dump(o);
	else	o << "(null)";
	for (i++; i!=end(); i++) {
		o << ", ";
		if (*i)	(*i)->dump(o);
		else	o << "(null)";
	}
	return o;
}

size_t
dynamic_param_expr_list::size(void) const {
	return parent::size();
}

excl_ptr<param_expr_list>
dynamic_param_expr_list::make_copy(void) const {
	return excl_ptr<param_expr_list>(
		new dynamic_param_expr_list(*this));
	// use default copy constructor
}

bool
dynamic_param_expr_list::is_static_constant(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		count_const_ptr<param_expr> ip(*i);
		assert(ip);	// nothing may be NULL at this point!
		if (!ip->is_static_constant())
			return false;
		// else continue checking...
	}
	return true;
}

bool
dynamic_param_expr_list::is_loop_independent(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		count_const_ptr<param_expr> ip(*i);
		assert(ip);	// nothing may be NULL at this point!
		if (!ip->is_loop_independent())
			return false;
		// else continue checking...
	}
	return true;
}

bool
dynamic_param_expr_list::may_be_initialized(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		count_const_ptr<param_expr> ip(*i);
		assert(ip);	// nothing may be NULL at this point!
		if (!ip->may_be_initialized())
			return false;
		// else continue checking...
	}
	return true;
}

bool
dynamic_param_expr_list::must_be_initialized(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		count_const_ptr<param_expr> ip(*i);
		assert(ip);	// nothing may be NULL at this point!
		if (!ip->must_be_initialized())
			return false;
		// else continue checking...
	}
	return true;
}

#if 0
/**
	Precondition: all expressions must be non-NULL.  
 */
list<const param_expr&>
dynamic_param_expr_list::get_const_ref_list(void) const {
	list<const param_expr&> ret;
	const_iterator i = begin();
	for ( ; i!=end(); i++, j++) {
		count_const_ptr<param_expr> ip(*i);
		assert(ip);
		ret.push_back(*ip);
	}
	return ret;
}
#endif

bool
dynamic_param_expr_list::may_be_equivalent(const param_expr_list& p) const {
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &p);
if (cpl) {
	if (size() != cpl->size())
		return false;
	const_iterator i = begin();
	const_param_expr_list::const_iterator j = cpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_const_ptr<param_expr> ip(*i);
		const count_const_ptr<const_param> jp(*j);
		assert(ip && jp);
		if (!ip->may_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	assert(j == cpl->end());		// sanity
	return true;
} else {
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &p);
	assert(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i = begin();
	const_iterator j = dpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_const_ptr<param_expr> ip(*i);
		const count_const_ptr<param_expr> jp(*j);
		assert(ip && jp);
		if (!ip->may_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	assert(j == dpl->end());		// sanity
	return true;
}
}

bool
dynamic_param_expr_list::must_be_equivalent(const param_expr_list& p) const {
	const const_param_expr_list* cpl =
		IS_A(const const_param_expr_list*, &p);
if (cpl) {
	if (size() != cpl->size())
		return false;
	const_iterator i = begin();
	const_param_expr_list::const_iterator j = cpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_const_ptr<param_expr> ip(*i);
		const count_const_ptr<const_param> jp(*j);
		assert(ip && jp);
		if (!ip->must_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	assert(j == cpl->end());		// sanity
	return true;
} else {
	const dynamic_param_expr_list* dpl =
		IS_A(const dynamic_param_expr_list*, &p);
	assert(dpl);
	if (size() != dpl->size())
		return false;
	const_iterator i = begin();
	const_iterator j = dpl->begin();
	for ( ; i!=end(); i++, j++) {
		const count_const_ptr<param_expr> ip(*i);
		const count_const_ptr<param_expr> jp(*j);
		assert(ip && jp);
		if (!ip->must_be_equivalent(*jp))
			return false;
		// else continue checking...
	}
	assert(j == dpl->end());		// sanity
	return true;
}
}

//=============================================================================
// class index_expr method definitions

index_expr::index_expr() : object() { }

index_expr::~index_expr() { }

//-----------------------------------------------------------------------------
// class const_index method definitions

const_index::const_index() : index_expr() { }

const_index::~const_index() { }

//=============================================================================
// class index_expr_collective method defintions

#if 0
param_expr_collective::param_expr_collective() : param_expr(), elist() {
}

param_expr_collective::~param_expr_collective() {
}

ostream&
param_expr_collective::what(ostream& o) const {
	return o << "param-expr-collective";
}

string
param_expr_collective::hash_string(void) const {
	string ret("{");
	list<excl_ptr<param_expr> >::const_iterator i = elist.begin();
	for ( ; i!=elist.end(); i++) {
		const never_const_ptr<param_expr> p(*i);
		assert(p);
		ret += p->hash_string();
		ret += ",";
	}
	ret += "}";
	return ret;
}
#endif

//=============================================================================
// class pbool_instance_reference method definitions

pbool_instance_reference::pbool_instance_reference(
		never_ptr<pbool_instantiation> pi,
		excl_ptr<index_list> i) :
		param_instance_reference(i, pi->current_collection_state()),
		pbool_expr(), 
		pbool_inst_ref(pi) {
}

never_const_ptr<instantiation_base>
pbool_instance_reference::get_inst_base(void) const {
	return pbool_inst_ref;
}

never_const_ptr<param_instantiation>
pbool_instance_reference::get_param_inst_base(void) const {
	return pbool_inst_ref;
}

ostream&
pbool_instance_reference::what(ostream& o) const {
	return o << "pbool-inst-ref";
}

ostream&
pbool_instance_reference::dump(ostream& o) const {
	return simple_instance_reference::dump(o);
}

string
pbool_instance_reference::hash_string(void) const {
	return simple_instance_reference::hash_string();
}

size_t
pbool_instance_reference::dimensions(void) const {
	return simple_instance_reference::dimensions();
}

bool
pbool_instance_reference::has_static_constant_dimensions(void) const {
	return simple_instance_reference::has_static_constant_dimensions();
}

const_range_list
pbool_instance_reference::static_constant_dimensions(void) const {
	return simple_instance_reference::static_constant_dimensions();
}

/**
	\return true if sucessfully initialized with valid expression.  
 */
bool
pbool_instance_reference::initialize(count_const_ptr<param_expr> i) {
	count_const_ptr<pbool_expr> b(i.is_a<pbool_expr>());
	if (!b) {
		i->what(cerr << "Cannot initialize a bool parameter with a ")
			<< " expression, ERROR!  " << endl;
		return false;
	} else {
		return pbool_inst_ref->initialize(b);
	}
}

bool
pbool_instance_reference::may_be_initialized(void) const {
	return param_instance_reference::may_be_initialized();
}

bool
pbool_instance_reference::must_be_initialized(void) const {
	return param_instance_reference::must_be_initialized();
}

bool
pbool_instance_reference::is_static_constant(void) const {
	return param_instance_reference::is_static_constant();
}

bool
pbool_instance_reference::is_loop_independent(void) const {
	return param_instance_reference::is_loop_independent();
}

bool
pbool_instance_reference::is_unconditional(void) const {
	return param_instance_reference::is_unconditional();
}

/**
	Better make sure that this is_static_constant before calling, 
	else will assert-fail.
 */
bool
pbool_instance_reference::static_constant_bool(void) const {
	assert(is_static_constant());
	return pbool_inst_ref->initial_value()->static_constant_bool();
}

//=============================================================================
// class pint_instance_reference method definitions

pint_instance_reference::pint_instance_reference(
		never_ptr<pint_instantiation> pi,
		excl_ptr<index_list> i) :
		param_instance_reference(i, pi->current_collection_state()),
		pint_expr(), 
		pint_inst_ref(pi) {
}

never_const_ptr<instantiation_base>
pint_instance_reference::get_inst_base(void) const {
	return pint_inst_ref;
}

never_const_ptr<param_instantiation>
pint_instance_reference::get_param_inst_base(void) const {
	return pint_inst_ref;
}

ostream&
pint_instance_reference::what(ostream& o) const {
	return o << "pint-inst-ref";
}

ostream&
pint_instance_reference::dump(ostream& o) const {
	return simple_instance_reference::dump(o);
}

string
pint_instance_reference::hash_string(void) const {
	return simple_instance_reference::hash_string();
}

size_t
pint_instance_reference::dimensions(void) const {
	return simple_instance_reference::dimensions();
}

bool
pint_instance_reference::has_static_constant_dimensions(void) const {
	return simple_instance_reference::has_static_constant_dimensions();
}

const_range_list
pint_instance_reference::static_constant_dimensions(void) const {
	return simple_instance_reference::static_constant_dimensions();
}

/**
	\return true if successfully initialized with valid expression.  
 */
bool
pint_instance_reference::initialize(count_const_ptr<param_expr> i) {
	count_const_ptr<pint_expr> b(i.is_a<pint_expr>());
	if (!b) {
		i->what(cerr << "Cannot initialize an int parameter with a ")
			<< " expression, ERROR!  " << endl;
		return false;
	} else {
		return pint_inst_ref->initialize(b);
	}
}

bool
pint_instance_reference::may_be_initialized(void) const {
	return param_instance_reference::may_be_initialized();
}

bool
pint_instance_reference::must_be_initialized(void) const {
	return param_instance_reference::must_be_initialized();
}

bool
pint_instance_reference::is_static_constant(void) const {
	return param_instance_reference::is_static_constant();
}

bool
pint_instance_reference::is_loop_independent(void) const {
	return param_instance_reference::is_loop_independent();
}

bool
pint_instance_reference::is_unconditional(void) const {
	return param_instance_reference::is_unconditional();
}

/**
	Better make sure that this is_static_constant before calling, 
	else will assert-fail.
 */
int
pint_instance_reference::static_constant_int(void) const {
	assert(is_static_constant());
	return pint_inst_ref->initial_value()->static_constant_int();
}

//=============================================================================
// class pint_const method definitions

ostream&
pint_const::what(ostream& o) const {
	return o << "pint-const";
}

ostream&
pint_const::dump(ostream& o) const {
	return o << hash_string();
}

string
pint_const::hash_string(void) const {
	ostringstream o;
	o << val;
	return o.str();
}

/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
	Just copy-constructs.  
 */
count_const_ptr<const_param>
pint_const::static_constant_param(void) const {
	return count_const_ptr<const_param>(
		new pint_const(val));
}

bool
pint_const::operator == (const const_range& c) const {
	return (val == c.first) && (val == c.second);
}

//=============================================================================
// class pbool_const method definitions

ostream&
pbool_const::what(ostream& o) const {
	return o << "pbool-const";
}

ostream&
pbool_const::dump(ostream& o) const {
	return o << hash_string();
}

string
pbool_const::hash_string(void) const {
	return (val) ? "true" : "false";
}

/**
	Precondition: must satisfy is_static_constant.  
	For use with const_param_expr_list.  
	Just copy-constructs.  
 */
count_const_ptr<const_param>
pbool_const::static_constant_param(void) const {
	return count_const_ptr<const_param>(
		new pbool_const(val));
}

//=============================================================================
// class pint_unary_expr method definitions

pint_unary_expr::pint_unary_expr(
		const char o, count_const_ptr<pint_expr> e) :
		pint_expr(), op(o), ex(e) {
	assert(ex);
	assert(ex->dimensions() == 0);
}

pint_unary_expr::pint_unary_expr(
		count_const_ptr<pint_expr> e, const char o) :
		pint_expr(), op(o), ex(e) {
	assert(ex);
	assert(ex->dimensions() == 0);
}

ostream&
pint_unary_expr::what(ostream& o) const {
	return o << "pint-unary-expr";
}

ostream&
pint_unary_expr::dump(ostream& o) const {
	// parentheses? check operator precedence
	return ex->dump(o << op);
}

string
pint_unary_expr::hash_string(void) const {
//	return op +ex->hash_string();
	return ex->hash_string() +op;
}

bool
pint_unary_expr::is_static_constant(void) const {
	return ex->is_static_constant();
}

bool
pint_unary_expr::is_loop_independent(void) const {
	return ex->is_loop_independent();
}

bool
pint_unary_expr::is_unconditional(void) const {
	return ex->is_unconditional();
}

int
pint_unary_expr::static_constant_int(void) const {
	// depends on op
	return -ex->static_constant_int();
}

//=============================================================================
// class pbool_unary_expr method definitions

pbool_unary_expr::pbool_unary_expr(
		const char o, count_const_ptr<pbool_expr> e) :
		pbool_expr(), op(o), ex(e) {
	assert(ex);
	assert(ex->dimensions() == 0);
}

pbool_unary_expr::pbool_unary_expr(
		count_const_ptr<pbool_expr> e, const char o) :
		pbool_expr(), op(o), ex(e) {
	assert(ex);
	assert(ex->dimensions() == 0);
}

ostream&
pbool_unary_expr::what(ostream& o) const {
	return o << "pbool-unary-expr";
}

ostream&
pbool_unary_expr::dump(ostream& o) const {
	// parentheses?
	return ex->dump(o << op);
}

string
pbool_unary_expr::hash_string(void) const {
	return ex->hash_string() +op;
}

bool
pbool_unary_expr::is_static_constant(void) const {
	return ex->is_static_constant();
}

bool
pbool_unary_expr::is_loop_independent(void) const {
	return ex->is_loop_independent();
}

bool
pbool_unary_expr::is_unconditional(void) const {
	return ex->is_unconditional();
}

bool
pbool_unary_expr::static_constant_bool(void) const {
	return !ex->static_constant_bool();
}

//=============================================================================
// class arith_expr method definitions

arith_expr::arith_expr(count_const_ptr<pint_expr> l, const char o,
		count_const_ptr<pint_expr> r) :
		lx(l), rx(r), op(o) {
	assert(lx);
	assert(rx);
	assert(lx->dimensions() == 0);
	assert(rx->dimensions() == 0);
}

ostream&
arith_expr::what(ostream& o) const {
	return o << "arith-expr";
}

ostream&
arith_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << op);
}

string
arith_expr::hash_string(void) const {
	return lx->hash_string() +op +rx->hash_string();
}

bool
arith_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

bool
arith_expr::is_loop_independent(void) const {
	return lx->is_loop_independent() && rx->is_loop_independent();
}

bool
arith_expr::is_unconditional(void) const {
	return lx->is_unconditional() && rx->is_unconditional();
}

int
arith_expr::static_constant_int(void) const {
	int a = lx->static_constant_int();
	int b = rx->static_constant_int();
	switch(op) {
		case '+':	return a + b;
		case '-':	return a - b;
		case '*':	return a / b;
		case '/':	return a * b;
		case '%':	return a % b;
		default:	assert(0); return 0;
	}
}

//=============================================================================
// class relational_expr method definitions

relational_expr::relational_expr(count_const_ptr<pint_expr> l,
		const string& o, count_const_ptr<pint_expr> r) :
		lx(l), rx(r), op(o) {
	assert(lx);
	assert(rx);
	assert(lx->dimensions() == 0);
	assert(rx->dimensions() == 0);
}

ostream&
relational_expr::what(ostream& o) const {
	return o << "relational-expr";
}

ostream&
relational_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << op);
}

string
relational_expr::hash_string(void) const {
	return lx->hash_string() +op +rx->hash_string();
}

bool
relational_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

bool
relational_expr::is_loop_independent(void) const {
	return lx->is_loop_independent() && rx->is_loop_independent();
}

bool
relational_expr::is_unconditional(void) const {
	return lx->is_unconditional() && rx->is_unconditional();
}

/**
	TO DO: what do you think?
 */
bool
relational_expr::static_constant_bool(void) const {
	int a = lx->static_constant_int();
	int b = rx->static_constant_int();
	// switch
	return false;
}

//=============================================================================
// class logical_expr method definitions

logical_expr::logical_expr(count_const_ptr<pbool_expr> l,
		const string& o, count_const_ptr<pbool_expr> r) :
		lx(l), rx(r), op(o) {
	assert(lx);
	assert(rx);
	assert(lx->dimensions() == 0);
	assert(rx->dimensions() == 0);
}

ostream&
logical_expr::what(ostream& o) const {
	return o << "logical-expr";
}

ostream&
logical_expr::dump(ostream& o) const {
	return rx->dump(lx->dump(o) << op);
}

string
logical_expr::hash_string(void) const {
	return lx->hash_string() +op +rx->hash_string();
}

bool
logical_expr::is_static_constant(void) const {
	return lx->is_static_constant() && rx->is_static_constant();
}

bool
logical_expr::is_loop_independent(void) const {
	return lx->is_loop_independent() && rx->is_loop_independent();
}

bool
logical_expr::is_unconditional(void) const {
	return lx->is_unconditional() && rx->is_unconditional();
}

/**
	TO DO: what do you think?
 */
bool
logical_expr::static_constant_bool(void) const {
	bool a = lx->static_constant_bool();
	bool b = rx->static_constant_bool();
	// switch
	return false;
}

//=============================================================================
// class pint_range method definitions

pint_range::pint_range(count_const_ptr<pint_expr> n) :
		range_expr(),
		lower(new pint_const(0)),
		upper(new arith_expr(n, '-', 
			count_const_ptr<pint_expr>(new pint_const(1)))) {
	assert(n);
	assert(lower);
	assert(upper);
}

pint_range::pint_range(count_const_ptr<pint_expr> l, 
		count_const_ptr<pint_expr> u) :
		range_expr(), lower(l), upper(u) {
	assert(lower);
	assert(upper);
}

pint_range::pint_range(const pint_range& pr) :
		object(), index_expr(), range_expr(), 
		// virtual base object() needs to be explicitly invoked
		// in this copy constructor
		lower(pr.lower), upper(pr.upper) {
}

ostream&
pint_range::what(ostream& o) const {
	return o << "pint-range";
}

ostream&
pint_range::dump(ostream& o) const {
	return o << hash_string();
}

string
pint_range::hash_string(void) const {
	return lower->hash_string() + ".." +upper->hash_string();
}

/**
	Range is sane if lower <= upper.
	If expressions are not constant, then conservatively return true.  
 */
bool
pint_range::is_sane(void) const {
	if (is_static_constant()) {
		return lower->static_constant_int() <=
			upper->static_constant_int();
	}
	else return true;
}

bool
pint_range::is_static_constant(void) const {
	return lower->is_static_constant() && upper->is_static_constant();
}

const_range
pint_range::static_constant_range(void) const {
	return const_range(lower->static_constant_int(), 
		upper->static_constant_int());
}

//=============================================================================
// class const_range method definitions

/**
	Default empty constructor. 
	Makes an invalid range.  
 */
const_range::const_range() : range_expr(), const_index(), parent(0,-1) {
}

/** Protected internal constructor. */
const_range::const_range(const interval_type& i) :
		range_expr(), const_index(), 
		parent(i.empty() ? 0 : i.begin()->first,
			i.empty() ? -1 : i.begin()->second) {
	if (!i.empty())
		assert(upper() >= lower());		// else what!?!?
}

/**
	Explicit constructor of a dense range from 0 to N-1.  
	\param n must be > 0, else assertion will fail.
 */
const_range::const_range(const int n) :
		range_expr(), const_index(), 
		parent(0, n-1) {
	assert(upper() >= lower());		// else what!?!?
}

/**
	Explicit constructor of a dense range from 0 to N-1.  
	\param n must be > 0, else assertion will fail.
 */
const_range::const_range(const pint_const& n) :
		range_expr(), const_index(), 
		parent(0, n.static_constant_int() -1) {
	assert(upper() >= lower());		// else what!?!?
}

/**
	Should both l and u be non-negative too?
	\param l is lower bound, inclusive.  
	\param u is upper bound, inclusive, and must be >= l.  
 */
const_range::const_range(const int l, const int u) :
		range_expr(), const_index(), 
		parent(l, u) {
	assert(upper() >= lower());		// else what!?!?
}

/** standard copy constructor */
const_range::const_range(const const_range& r) :
		object(), 
		index_expr(),
		range_expr(), 
		const_index(), 
		parent(r) {
	// assert check range?
}

const_range::const_range(const parent r) :
		object(), 
		index_expr(),
		range_expr(), 
		const_index(), 
		parent(r) {
	// assert check range?
}

ostream&
const_range::what(ostream& o) const {
	return o << "const-range";
}

ostream&
const_range::dump(ostream& o) const {
	if (empty())
		return o << "[]";
	else
		return o << "[" << lower() << ".." << upper() << "]";
}

string
const_range::hash_string(void) const {
	ostringstream o;
	dump(o);
	return o.str();
}

/**
	Returns whether or not two intervals overlap.  
	\return 
 */
const_range
const_range::static_overlap(const const_range& r) const {
#if 0
	// DEBUG
	cerr << "In const_range::static_overlap with this = "
		<< this << " .interval = " << interval << endl;
	r.dump(cerr << "const const_range& r = ") << endl;
#endif

	interval_type temp(first, second);
	interval_type temp2(r.first, r.second);
	temp.intersect(temp2);
	return const_range(temp);		// private constructor
}

bool
const_range::operator == (const const_range& c) const {
	return (first == c.first) && (second == c.second);
}

/**
	Presumably if this was successfully constructed, then it
	passed the assertion.  No need to recheck.  
 */
bool
const_range::is_sane(void) const {
	return !empty();
}

//=============================================================================
// class range_expr method definitions

range_expr::range_expr() : index_expr() {
}

range_expr::~range_expr() {
}

ostream&
range_expr::dump(ostream& o) const {
	return o << hash_string();
}

//=============================================================================
// class range_expr_list method definitions

range_expr_list::range_expr_list() : object() {
}

//=============================================================================
// class const_range_list method definitions

const_range_list::const_range_list() : range_expr_list(), list_type() {
}

/**
	Implicit conversion.  
 */
const_range_list::const_range_list(const list_type& l) :
		range_expr_list(), list_type(l) {
}

/**
	Explicit conversion from an index list to a range list.  
	Only available for constants, of course.
	Converts x[n] to x[n..n].
	Useful for static range-checking of indices.  
 */
const_range_list::const_range_list(const const_index_list& i) :
		range_expr_list(), list_type() {
	const_index_list::const_iterator j = i.begin();
	for ( ; j!=i.end(); j++) {
		count_ptr<const_index> k(*j);
		assert(k);
		count_ptr<pint_const> p(k.is_a<pint_const>());
		count_ptr<const_range> r(k.is_a<const_range>());
		if (p) {
			const int min_max = p->static_constant_int();
			push_back(const_range(min_max, min_max));	// copy
		} else {
			assert(r);
			push_back(*r);		// deep copy
		}
	}
}

const_range_list::~const_range_list() {
}

ostream&
const_range_list::what(ostream& o) const {
	return o << "const_range_list";
}

ostream&
const_range_list::dump(ostream& o) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++)
		i->dump(o);
	return o;
}

size_t
const_range_list::size(void) const {
	return list_type::size();
}

/**
	Checks for overlap between two static multidimensional index ranges.  
	If argument is actually dynamic, can only conservatively return false.  
	\return a const_range_list which is either false if there's no overlap, 
		or the valid overlap range if there is overlap.  
 */
const_range_list
const_range_list::static_overlap(const range_expr_list& r) const {
#if 0
	// DEBUG
	cerr << "In const_range_list::static_overlap with this = "
		<< this << endl;
	r.dump(cerr << "const range_expr_list& r = ") << endl;
#endif
	const const_range_list* s = IS_A(const const_range_list*, &r);
	const_range_list ret;	// initially empty
	if (s) {
		// dimensionality should match, or else!
		assert(size() == s->size());    
		const_iterator i = begin();
		const_iterator j = s->begin();
		for ( ; i!=end(); i++, j++) {
			// check for index overlap in ALL dimensions
			const const_range& ir = *i;
			const const_range& jr = *j;
			const_range o(ir.static_overlap(jr));
			ret.push_back(o);
			if (!o.empty())
				continue;
			else return const_range_list();
			// if there is any dimension without overlap, false
		}
		// else there is some overlap in all dimensions
		return ret;
	} else {	// argument is dynamic, not static
		// conservatively return false
		// overlap is possible but not definite, can't reject now.  
		return const_range_list();		// empty list
	}
}

#if 0
/**
	Takes a list of ranges and converts back to indices, using a 
	reference index list, resembling the original index list.  
	Useful for converting results from multidimensional_sparse_set
	queries (range lists) back into index form where needed.  
	\param il the reference index list.
 */
const_index_list
const_range_list::revert_to_indices(const const_index_list& il) const {
	// let's actually bother to check consistency of the reference list
	const_index_list ret;
	// have a better idea... punt this for now
}
#endif

/**
	Collapses the multidimensional range list using an
	reference index list as an argument.  
	For each index element of the argument that is an int, 
	not a range, shorten the range list in that dimension.  
	Make sure that the range is unity, as a sanity check.  
 */
void
const_range_list::collapse_dimensions_wrt_indices(const const_index_list& il) {
	assert(size() >= il.size());
	iterator i = begin();
	const_index_list::const_iterator j = il.begin();
	for ( ; j!=il.end(); i++, j++) {
		const count_const_ptr<pint_const>	// or pint_const
			pi(j->is_a<pint_const>());
		if (pi) {
			assert(i->first == i->second);
			erase(i);
		} else {
			const count_const_ptr<const_range>
				pr(j->is_a<const_range>());
			assert(pr);
			assert(pr->first == i->first);
			assert(pr->second == i->second);
		}
	}
	// any remaining indices are kept
}

/**
	Size equality of two multidimensional ranges.  
	Also reports error in size mismatch to stderr.  
 */
bool
const_range_list::is_size_equivalent(const const_range_list& c) const {
	if (size() != c.size())
		return false;
	// else check
	int dim = 1;
	const_iterator i = begin();
	const_iterator j = c.begin();
	for ( ; i!=end(); i++, j++, dim++) {
		const int ldiff = i->second -i->first;
		const int rdiff = j->second -j->first;
		if (ldiff != rdiff) {
			cerr << "Size of dimension " << dim <<
				" does not match!  got: " << ldiff+1 <<
				" and " << rdiff+1 << "." << endl;
			return false;
		}
	}
	assert(j == c.end());
	return true;
}

/**
	Whether two multidimensional range lists are identical.  
	Not just whether or not the size of the spanned ranges are equal.  
 */
bool
const_range_list::operator == (const const_range_list& c) const {
	if (size() != c.size())
		return false;
	// else check
	const_iterator i = begin();
	const_iterator j = c.begin();
	for ( ; i!=end(); i++, j++) {
		if (*i != *j)
			return false;
	}
	assert(j == c.end());
	return true;
}

//=============================================================================
// class dynamic_range_list method definitions

dynamic_range_list::dynamic_range_list() : range_expr_list(), list_type() {
}

dynamic_range_list::~dynamic_range_list() {
}

ostream&
dynamic_range_list::what(ostream& o) const {
	return o << "dynamic_range_list";
}

ostream&
dynamic_range_list::dump(ostream& o) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		(*i)->dump(o);
	}
	return o;
}

size_t
dynamic_range_list::size(void) const {
	return list_type::size();
}

bool
dynamic_range_list::is_static_constant(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		const count_const_ptr<pint_range> pr(*i);
		assert(pr);
		if (!pr->is_static_constant())
			return false;
		// else continue checking
	}
	return true;
}

/**
	Overlap is indefinite with dynamic ranges, conservatively.  
 */
const_range_list
dynamic_range_list::static_overlap(const range_expr_list& r) const {
	return const_range_list();
}

//=============================================================================
// class index_list method definitions

index_list::index_list() : object() { }

index_list::~index_list() { }

//=============================================================================
// class const_index_list method definitions

const_index_list::const_index_list() : index_list(), parent() { }

const_index_list::~const_index_list() { }

ostream&
const_index_list::what(ostream& o) const {
	return o << "const-index-list";
}

ostream&
const_index_list::dump(ostream& o) const {
	return o << hash_string();
}

string
const_index_list::hash_string(void) const {
	string ret;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		ret += "[";
		ret += (*i)->hash_string();
		ret += "]";
	}
	return ret;
}

size_t
const_index_list::size(void) const {
	return parent::size();
}

/**
	A dimension is collapsed if the index is a single integer, 
	and otherwise not collapsed if it's a range.  
	A non-collapsed dimension of size 1 is indicated
	as a range [i..i].  
	\return the number of dimensions are collapsed.  
 */
size_t
const_index_list::dimensions_collapsed(void) const {
	size_t ret = 0;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		if (i->is_a<pint_const>())
			ret++;
		else assert(i->is_a<const_range>());
			// sanity check
	}
	return ret;
}

const_range_list
const_index_list::collapsed_dimension_ranges(void) const {
	const_range_list ret;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		const count_const_ptr<const_range>
			cr(i->is_a<const_range>());
		if (cr)
			ret.push_back(*cr);	// will copy
		else assert(i->is_a<pint_const>());
		// continue
	}
	return ret;
}

/**
	Wrapper to list paren'ts push_back that checks that
	expression is a 0-dimensional pint_inst reference.  
 */
void
const_index_list::push_back(const count_ptr<const_index>& i) {
	// check dimensionality
	assert(i);
	assert(i->dimensions() == 0);
	parent::push_back(i);
}

bool
const_index_list::may_be_initialized(void) const {
	return true;
}

bool
const_index_list::must_be_initialized(void) const {
	return true;
}

bool
const_index_list::is_static_constant(void) const {
	return true;
}

bool
const_index_list::is_loop_independent(void) const {
	return true;
}

bool
const_index_list::is_unconditional(void) const {
	return true;
}

//=============================================================================
// class dynamic_index_list method definitions

dynamic_index_list::dynamic_index_list() : index_list(), parent() { }

dynamic_index_list::~dynamic_index_list() { }

ostream&
dynamic_index_list::what(ostream& o) const {
	return o << "dynamic-index-list";
}

ostream&
dynamic_index_list::dump(ostream& o) const {
	return o << hash_string();
}

string
dynamic_index_list::hash_string(void) const {
	string ret;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		ret += "[";
		ret += (*i)->hash_string();
		ret += "]";
	}
	return ret;
}

void
dynamic_index_list::push_back(const count_ptr<index_expr>& i) {
	assert(i);
	assert(i->dimensions() == 0);
	parent::push_back(i);
}

size_t
dynamic_index_list::size(void) const {
	return parent::size();
}

/**
	Counts which dimensions are collapsed.  
	See description in const_index_list::dimensions_collapsed().  
 */
size_t
dynamic_index_list::dimensions_collapsed(void) const {
	size_t ret = 0;
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		if (i->is_a<pint_expr>())
			ret++;
		else assert(i->is_a<range_expr>());
			// sanity check
	}
	return ret;
}

bool
dynamic_index_list::may_be_initialized(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		if (!(*i)->may_be_initialized())
			return false;
	}
	return true;
}

bool
dynamic_index_list::must_be_initialized(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		if (!(*i)->must_be_initialized())
			return false;
	}
	return true;
}

bool
dynamic_index_list::is_static_constant(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		if (!(*i)->is_static_constant())
			return false;
	}
	return true;
}

bool
dynamic_index_list::is_loop_independent(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		if (!(*i)->is_loop_independent())
			return false;
	}
	return true;
}

bool
dynamic_index_list::is_unconditional(void) const {
	const_iterator i = begin();
	for ( ; i!=end(); i++) {
		assert(*i);
		if (!(*i)->is_unconditional())
			return false;
	}
	return true;
}

//=============================================================================
}	// end namepace entity
}	// end namepace ART

