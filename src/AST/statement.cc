/**
	\file "AST/statement.cc"
	Class method definitions for statement classes.
	$Id: statement.cc,v 1.3 2006/01/22 06:52:54 fang Exp $
 */

#ifndef	__HAC_AST_STATEMENT_CC__
#define	__HAC_AST_STATEMENT_CC__

#define	ENABLE_STACKTRACE		0

// rule-of-thumb for inline directives:
// only inline constructors if you KNOW that they will not be be needed
// outside of this module, because we don't have a means to export
// inline methods other than defining in the header or using
// -fkeep-inline-functions

#include <exception>
#include <iostream>

#include "AST/expr_base.h"
#include "AST/node_position.h"
#include "AST/statement.h"
#include "AST/parse_context.h"

#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

//=============================================================================
// for specializing util::what
namespace util {
SPECIALIZE_UTIL_WHAT(HAC::parser::incdec_stmt, "(inc/dec-stmt)")
SPECIALIZE_UTIL_WHAT(HAC::parser::assign_stmt, "(assign-stmt)")
}

//=============================================================================
namespace HAC {
namespace parser {
#include "util/using_ostream.h"
using util::indent;
using util::auto_indent;

//=============================================================================
// class statement method definitions

#if 0
CONSTRUCTOR_INLINE
statement::statement() { }

DESTRUCTOR_INLINE
statement::~statement() { }
#endif

//=============================================================================
// class incdec_stmt method definitions

CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(const expr* n, const string_punctuation_type* o) :
		statement(), e(n), op(o) {
	NEVER_NULL(e); NEVER_NULL(op);
}

#if 0
CONSTRUCTOR_INLINE
incdec_stmt::incdec_stmt(excl_ptr<const expr> n, excl_const_ptr<string_punctuation_type> o) :
		statement(), e(n), op(o) {
	assert(e); assert(op);
}
#endif

DESTRUCTOR_INLINE
incdec_stmt::~incdec_stmt() {
	SAFEDELETE(e); SAFEDELETE(op);
}

/**
	Release operations are needed for destructive transfer of ownership.
	The consumers of the return pointers are thus responsible for the
	memory at their location. 
 */
#if 0
excl_ptr<const expr>
incdec_stmt::release_expr(void) {
	return e;
}

excl_ptr<const string_punctuation_type>
incdec_stmt::release_op(void) {
	return op;
}
#else
const expr*
incdec_stmt::release_expr(void) {
	const expr* ret = e;
	e = NULL;
	return ret;
}

const string_punctuation_type*
incdec_stmt::release_op(void) {
	const string_punctuation_type* ret = op;
	op = NULL;
	return ret;
}
#endif

PARSER_WHAT_DEFAULT_IMPLEMENTATION(incdec_stmt)

line_position
incdec_stmt::leftmost(void) const {
	// if (e)
	return e->leftmost();
}

line_position
incdec_stmt::rightmost(void) const {
	// if (op)
	return op->rightmost();
}

//=============================================================================
// class assign_stmt method definitions

CONSTRUCTOR_INLINE
assign_stmt::assign_stmt(const expr* left, const char_punctuation_type* o, 
		const expr* right) : statement(),
		lhs(left), op(o), rhs(right) {
	NEVER_NULL(lhs); NEVER_NULL(op); NEVER_NULL(rhs);
}

#if 0
CONSTRUCTOR_INLINE
assign_stmt::assign_stmt(excl_ptr<const expr> left, excl_const_ptr<string_punctuation_type> o, 
		excl_ptr<const expr> right) : statement(),
		lhs(left), op(o), rhs(right) {
	assert(lhs); assert(op); assert(rhs);
}
#endif

DESTRUCTOR_INLINE
assign_stmt::~assign_stmt() {
	SAFEDELETE(lhs); SAFEDELETE(op); SAFEDELETE(rhs);
}

#if 0
excl_ptr<const expr>
assign_stmt::release_lhs(void) {
	return lhs;
}

excl_ptr<const string_punctuation_type>
assign_stmt::release_op(void) {
	return op;
}

excl_ptr<const expr>
assign_stmt::release_rhs(void) {
	return rhs;
}
#else
const expr*
assign_stmt::release_lhs(void) {
	const expr* ret = lhs;
	lhs = NULL;
	return ret;
}

const char_punctuation_type*
assign_stmt::release_op(void) {
	const char_punctuation_type* ret = op;
	op = NULL;
	return ret;
}

const expr*
assign_stmt::release_rhs(void) {
	const expr* ret = rhs;
	rhs = NULL;
	return ret;
}
#endif

PARSER_WHAT_DEFAULT_IMPLEMENTATION(assign_stmt)

line_position
assign_stmt::leftmost(void) const {
	return lhs->leftmost();
}

line_position
assign_stmt::rightmost(void) const {
	return rhs->rightmost();
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

#endif	// __HAC_AST_STATEMENT_CC__

