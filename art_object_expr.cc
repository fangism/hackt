// "art_object_expr.cc"

#include <stdio.h>			// for sprintf
#include <stdlib.h>			// for ltoa
#include <assert.h>
#include <iostream>

#include "art_parser.h"
#include "art_object.h"
#include "art_object_expr.h"

namespace ART {
namespace entity {
//=============================================================================
// class param_expr method_definitions

ostream&
param_expr::dump(ostream& o) const {
	return what(o);
}

//=============================================================================
// class param_expr_collective method defintions

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
	list<excl_ptr<param_expr> >::const_iterator i = 
		elist.begin();
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

/**
	Whether or not this reference is initialized or is 
	dependent on a template formal parameter.  
 */
bool
param_literal::is_initialized(void) const {
	return var->is_initialized();
}

/**
	Find out if the referenced parameter variable is
	has been defined, and if so whether or not it is constant.  
	TO DO: finish
 */
bool
param_literal::is_static_constant(void) const {
	return false;
}

void
param_literal::initialize(count_const_ptr<param_expr> i) {
	var->initialize(i);
}

//=============================================================================
// class param_const_int method definitions

ostream&
param_const_int::what(ostream& o) const {
	return o << "param-const-int";
}

string
param_const_int::hash_string(void) const {
	char* ret = new char[64];
	assert(sprintf(ret, "%ld", val) == 1);
	return ret;			// will convert to string
}

//=============================================================================
// class param_const_bool method definitions

ostream&
param_const_bool::what(ostream& o) const {
	return o << "param-const-bool";
}

string
param_const_bool::hash_string(void) const {
	return (val) ? "true" : "false";
}

//=============================================================================
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

//=============================================================================
}	// end namepace entity
}	// end namepace ART

