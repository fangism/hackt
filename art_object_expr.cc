// "art_object_expr.cc"

#include <stdio.h>			// for sprintf
#include <stdlib.h>			// for ltoa
#include <assert.h>
#include <iostream>

#include "sstream.h"			// for ostringstring, used by dump

#include "art_parser.h"
#include "art_object.h"			// before including "art_object_expr.h"
#include "art_object_expr.h"

namespace ART {
namespace entity {
//=============================================================================
using namespace std;

// forward declarations (table of contents)
class param_expr;
class index_expr;
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

//=============================================================================
// class param_expr method_definitions

param_expr::param_expr() : object() { }

param_expr::~param_expr() { }

//=============================================================================
// class index_expr method_definitions

index_expr::index_expr() : object() { }

index_expr::~index_expr() { }

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
// class param_literal method definitions
#if 0
OBSOLETE, inlined in header

param_literal::param_literal(count_ptr<param_instance_reference> v) :
		param_expr(), var(v) {
	assert(var);
}

param_literal::~param_literal() {
}

ostream&
param_literal::what(ostream& o) const {
	return o << "param-literal";
}

string
param_literal::hash_string(void) const {
	return var->hash_string();
}
#endif

/**
	Whether or not this reference is initialized or is 
	dependent on a template formal parameter.  
 */
bool
param_literal::is_initialized(void) const {
	return get_inst_ref()->is_initialized();
}

/**
	Find out if the referenced parameter variable is
	has been defined, and if so whether or not it is constant.  
	TO DO: finish
 */
bool
param_literal::is_static_constant(void) const {
	// get_inst_ref()...
	return false;
}

#if 0
void
param_literal::initialize(count_const_ptr<param_expr> i) {
	var->initialize(i);
}
#endif

bool
param_literal::is_loop_independent(void) const {
	// get_inst_ref()...
	return true;
}

bool
param_literal::is_unconditional(void) const {
	// get_inst_ref()...
	return false;
}

//=============================================================================
// class pbool_literal method definitions

pbool_literal::pbool_literal(count_ptr<pbool_instance_reference> v) :
		pbool_expr(), param_literal(), var(v) {
	assert(var);
}

pbool_literal::~pbool_literal() {
}

count_const_ptr<param_instance_reference>
pbool_literal::get_inst_ref(void) const {
	return var;
}

ostream&
pbool_literal::what(ostream& o) const {
	return o << "pbool-literal";
}

ostream&
pbool_literal::dump(ostream& o) const {
	return var->dump(o);
}

string
pbool_literal::hash_string(void) const {
	return var->hash_string();
}

void
pbool_literal::initialize(count_const_ptr<param_expr> i) {
	var->initialize(i);
}

bool
pbool_literal::is_initialized(void) const {
	return var->is_initialized();
}

bool
pbool_literal::is_static_constant(void) const {
	return var->is_static_constant();
}

bool
pbool_literal::is_loop_independent(void) const {
	return var->is_loop_independent();
}

bool
pbool_literal::is_unconditional(void) const {
	return var->is_unconditional();
}

bool
pbool_literal::static_constant_bool(void) const {
	// temporary... later actually bother to look up var...
	assert(0);
	return false;
//	return var->get_param_inst_base()->
//		default_value().is_a<pbool_expr>()->static_constant_bool();
}

//=============================================================================
// class pint_literal method definitions

pint_literal::pint_literal(count_ptr<pint_instance_reference> v) :
		pint_expr(), param_literal(), var(v) {
	assert(var);
}

pint_literal::~pint_literal() {
}

count_const_ptr<param_instance_reference>
pint_literal::get_inst_ref(void) const {
	return var;
}

ostream&
pint_literal::what(ostream& o) const {
	return o << "pint-literal";
}

ostream&
pint_literal::dump(ostream& o) const {
	return var->dump(o);
}

string
pint_literal::hash_string(void) const {
	return var->hash_string();
}

void
pint_literal::initialize(count_const_ptr<param_expr> i) {
	var->initialize(i);
}

bool
pint_literal::is_initialized(void) const {
	return var->is_initialized();
}

bool
pint_literal::is_static_constant(void) const {
	return var->is_static_constant();
}

bool
pint_literal::is_loop_independent(void) const {
	return var->is_loop_independent();
}

bool
pint_literal::is_unconditional(void) const {
	return var->is_unconditional();
}

int
pint_literal::static_constant_int(void) const {
	assert(0);			// temporary
	return 0;
//	return var->get_param_inst_base()->
//		default_value().is_a<pbool_expr>()->static_constant_bool();
}

//=============================================================================
// class pint_const method definitions

ostream&
pint_const::what(ostream& o) const {
	return o << "param-const-int";
}

ostream&
pint_const::dump(ostream& o) const {
	return o << hash_string();
}

string
pint_const::hash_string(void) const {
	char* ret = new char[64];
	assert(sprintf(ret, "%ld", val) == 1);
	return ret;			// will convert to string
}

//=============================================================================
// class pbool_const method definitions

ostream&
pbool_const::what(ostream& o) const {
	return o << "param-const-bool";
}

ostream&
pbool_const::dump(ostream& o) const {
	return o << hash_string();
}

string
pbool_const::hash_string(void) const {
	return (val) ? "true" : "false";
}

//=============================================================================
#if 0
// class param_unary_expr method definitions

param_unary_expr::param_unary_expr(const char o, const param_expr* e) :
		param_expr(), op(o), ex(e) {
	assert(ex);
}

param_unary_expr::param_unary_expr(const param_expr* e, const char o) :
		param_expr(), op(o), ex(e) {
	assert(ex);
}

ostream&
param_unary_expr::what(ostream& o) const {
	return o << "param-unary-expr";
}

string
param_unary_expr::hash_string(void) const {
	return ex->hash_string() +op;
}

//=============================================================================
// class param_binary_expr method definitions

param_binary_expr::param_binary_expr(const param_expr* l, const char o, 
		const param_expr* r) : param_expr(), op(o), lx(l), rx(r) {
	assert(lx);
	assert(rx);
}

ostream&
param_binary_expr::what(ostream& o) const {
	return o << "param-binary-expr";
}

string
param_binary_expr::hash_string(void) const {
	return lx->hash_string() +op +rx->hash_string();
}
#endif

//=============================================================================
// class pint_unary_expr method definitions

pint_unary_expr::pint_unary_expr(
		const char o, count_const_ptr<pint_expr> e) :
		pint_expr(), op(o), ex(e) {
	assert(ex);
}

pint_unary_expr::pint_unary_expr(
		count_const_ptr<pint_expr> e, const char o) :
		pint_expr(), op(o), ex(e) {
	assert(ex);
}

ostream&
pint_unary_expr::what(ostream& o) const {
	return o << "pint-unary-expr";
}

string
pint_unary_expr::hash_string(void) const {
	return ex->hash_string() +op;
}

bool
pint_unary_expr::is_initialized(void) const {
	return ex->is_initialized();
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
}

pbool_unary_expr::pbool_unary_expr(
		count_const_ptr<pbool_expr> e, const char o) :
		pbool_expr(), op(o), ex(e) {
	assert(ex);
}

ostream&
pbool_unary_expr::what(ostream& o) const {
	return o << "pbool-unary-expr";
}

string
pbool_unary_expr::hash_string(void) const {
	return ex->hash_string() +op;
}

bool
pbool_unary_expr::is_initialized(void) const {
	return ex->is_initialized();
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
arith_expr::is_initialized(void) const {
	return lx->is_initialized() && rx->is_initialized();
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
relational_expr::is_initialized(void) const {
	return lx->is_initialized() && rx->is_initialized();
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
logical_expr::is_initialized(void) const {
	return lx->is_initialized() && rx->is_initialized();
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


//=============================================================================
// class pint_range method definitions

pint_range::pint_range(count_const_ptr<pint_expr> n) :
		range_expr(),
		lower(new pint_const(0)),
		upper(new arith_expr(n, '-', 
			count_const_ptr<pint_expr>(new pint_const(1)))) {
}

pint_range::pint_range(count_const_ptr<pint_expr> l, 
		count_const_ptr<pint_expr> u) :
		range_expr(), lower(l), upper(u) {
}

pint_range::pint_range(const pint_range& pr) : object(), range_expr(), 
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

bool
pint_range::is_initialized(void) const {
	return lower->is_initialized() && upper->is_initialized();
}

//=============================================================================
// class const_range method definitions

const_range::const_range(const int n) :
		range_expr(),
		lower(0), upper(n -1) {
	assert(upper >= lower);		// else what!?!?
}

const_range::const_range(const int l, const int u) :
		range_expr(), lower(l), upper(u) {
	assert(upper >= lower);		// else what!?!?
}

ostream&
const_range::what(ostream& o) const {
	return o << "pint-const";
}

ostream&
const_range::dump(ostream& o) const {
	return o << lower << ".." << upper;
}

string
const_range::hash_string(void) const {
	ostringstream o;
	o << lower << ".." << upper;
	return o.str();
}

/**
	Returns whether or not two intervals overlap.  
 */
bool
const_range::static_overlap(const const_range& r) const {
	return (lower <= r.lower && r.lower <= upper ||
		lower <= r.upper && r.upper <= upper ||
		r.lower <= lower && lower <= r.upper ||
		r.lower <= upper && upper <= r.upper);
}

bool
const_range::is_initialized(void) const {
	return true;
}

//=============================================================================
// class range_expr method definitions

range_expr::range_expr() : object(), index_expr() {
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
 */
bool
const_range_list::static_overlap(const range_expr_list& r) const {
	const const_range_list* s = IS_A(const const_range_list*, &r);
	if (s) {
		// dimensionality should match, or else!
		assert(size() == s->size());    
		const_iterator i = begin();
		const_iterator j = s->begin();
		for ( ; i!=end(); i++, j++) {
			// check for index overlap in ALL dimensions
			const const_range& ir = *i;
			const const_range& jr = *j;
			if (ir.static_overlap(jr))
				continue;
			else return false;     
			// if there is any dimension without overlap, false
		}
		// else there is some overlap in all dimensions
		return true;
	} else {	// argument is dynamic, not static
		// conservatively return false
		// overlap is possible but not definite, can't reject now.  
		return false;
	}
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
	for ( ; i!=end(); i++)
		i->dump(o);
	return o;
}

size_t
dynamic_range_list::size(void) const {
	return list_type::size();
}

/**
	Overlap is indefinite with dynamic ranges, conservatively.  
 */
bool
dynamic_range_list::static_overlap(const range_expr_list& r) const {
	return false;
}

//=============================================================================
// class index_list method definitions

index_list::index_list() : object(), indices() { }

index_list::~index_list() { }

ostream&
index_list::what(ostream& o) const {
	return o << "index-list";
}

ostream&
index_list::dump(ostream& o) const {
	return o << hash_string();
}

string
index_list::hash_string(void) const {
	string ret;
	list_type::const_iterator i = indices.begin();
	for ( ; i!=indices.end(); i++) {
		assert(*i);
		ret += "[";
		ret += (*i)->hash_string();
		ret += "]";
	}
	return ret;
}

size_t
index_list::size(void) const {
	return indices.size();
}

/**
	Need to count which dimensions are collapsed.  
 */
size_t
index_list::dimensions(void) const {
	// THIS IS WRONG, just temporary
	return size();
}

bool
index_list::is_initialized(void) const {
	list_type::const_iterator i = indices.begin();
	for ( ; i!=indices.end(); i++) {
		assert(*i);
		if (!(*i)->is_initialized())
			return false;
	}
	return true;
}

bool
index_list::is_static_constant(void) const {
	list_type::const_iterator i = indices.begin();
	for ( ; i!=indices.end(); i++) {
		assert(*i);
		if (!(*i)->is_static_constant())
			return false;
	}
	return true;
}

bool
index_list::is_loop_independent(void) const {
	list_type::const_iterator i = indices.begin();
	for ( ; i!=indices.end(); i++) {
		assert(*i);
		if (!(*i)->is_loop_independent())
			return false;
	}
	return true;
}

bool
index_list::is_unconditional(void) const {
	list_type::const_iterator i = indices.begin();
	for ( ; i!=indices.end(); i++) {
		assert(*i);
		if (!(*i)->is_unconditional())
			return false;
	}
	return true;
}


//=============================================================================
}	// end namepace entity
}	// end namepace ART

