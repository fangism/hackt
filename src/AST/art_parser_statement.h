/**
	\file "AST/art_parser_statement.h"
	Base set of classes for the HAC parser.  
	$Id: art_parser_statement.h,v 1.6.48.1 2005/12/11 00:45:10 fang Exp $
 */

#ifndef __AST_HAC_PARSER_STATEMENT_H__
#define __AST_HAC_PARSER_STATEMENT_H__

#include "AST/art_parser_fwd.h"
#include "AST/art_parser_expr_base.h"

namespace HAC {
namespace parser {
//=============================================================================
/**
	Base class for statements (assignments, increment, decrement...)
	Assertion: all statements are nonterminals.  
 */
class statement {
public:
	statement() { }

virtual	~statement() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;
};	// end class statement

//-----------------------------------------------------------------------------
/**
	Class for unary expression statements, 
	such as increment and decrement.  
 */
class incdec_stmt : virtual public statement {
protected:
	// not const, so they may be released
#if 1
	const expr*		e;
	const string_punctuation_type*		op;
#else
	excl_ptr<const expr>		e;
	excl_ptr<const string_punctuation_type>	op;
#endif
public:
	incdec_stmt(const expr* n, const string_punctuation_type* o);
//	incdec_stmt(excl_ptr<const expr> n, excl_ptr<const string_punctuation_type> o);
virtual	~incdec_stmt();

virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

#if 1
	const expr*
	release_expr(void);

	const string_punctuation_type*
	release_op(void);
#else
	excl_ptr<const expr>
	release_expr(void);

	excl_ptr<const string_punctuation_type>
	release_op(void);
#endif
};	// end class incdec_stmt

//-----------------------------------------------------------------------------
/// class for binary expression statements, with left- and right-hand sides
class assign_stmt : virtual public statement {
protected:
#if 0
	excl_ptrconst <expr>		lhs;		///< destination
	excl_ptr<const string_punctuation_type>	op;		///< operation
	excl_ptr<const expr>		rhs;		///< source expression
#else
	const expr*			lhs;		///< destination
	const string_punctuation_type*	op;		///< operation
	const expr*			rhs;		///< source expression
#endif
public:
	assign_stmt(const expr* left, const string_punctuation_type* o,
		const expr* right);
#if 0
	assign_stmt(excl_ptr<const expr> left, excl_ptr<const string_punctuation_type> o,
		excl_ptr<const expr> right);
#endif
virtual	~assign_stmt();

virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

#if 0
	excl_ptr<const expr>
	release_lhs(void);

	excl_ptr<const string_punctuation_type>
	release_op(void);

	excl_ptr<const expr>
	release_rhs(void);
#else
	const expr*
	release_lhs(void);

	const char_punctuation_type*
	release_op(void);

	const expr*
	release_rhs(void);
#endif
};	// end class assign_stmt

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __AST_HAC_PARSER_STATEMENT_H__

