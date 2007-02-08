/**
	\file "AST/expr.h"
	Expression-related parser classes for HAC.
	$Id: expr.h,v 1.9 2007/02/08 02:10:59 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_expr.h,v 1.15.42.1 2005/12/11 00:45:05 fang Exp
 */

#ifndef __HAC_AST_EXPR_H__
#define __HAC_AST_EXPR_H__

#include "AST/expr_list.h"	// for array_concatenation
#include "AST/identifier.h"

namespace HAC {
namespace parser {
//=============================================================================
// class expr defined in "AST/expr_base.h"

//=============================================================================
/// abstract base class for unary expressions
class unary_expr : public expr {
protected:
	const excl_ptr<const expr>	e;	///< the argument expr
	const excl_ptr<const char_punctuation_type>	op;	///< the operator, may be null
public:
	unary_expr(const expr* n, const char_punctuation_type* o);

virtual	~unary_expr();

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

};	// end class unary_expr

//-----------------------------------------------------------------------------
/**
	Prefix unary expressions.  
	Has no children classes, so members need not be virtual. 
 */
class prefix_expr : public unary_expr {
public:
	prefix_expr(const char_punctuation_type* o, const expr* n);
	~prefix_expr();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;
	CHECK_PRS_EXPR_PROTO;
};	// end class prefix_expr

//=============================================================================
/// base class for general binary expressions
class binary_expr : public expr {
protected:
	const excl_ptr<const expr> 	l;	///< left-hand side
	const excl_ptr<const char_punctuation_type>	op;	///< operator
	const excl_ptr<const expr>	r;	///< right-hand side
public:
	binary_expr(const expr* left, const char_punctuation_type* o, 
		const expr* right);

virtual	~binary_expr();

virtual	ostream&
	what(ostream& o) const = 0;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

};	// end class binary_expr

//-----------------------------------------------------------------------------
/**
	Class of arithmetic expressions.  
	Takes ints as arguments and returns an int.  
 */
class arith_expr : public binary_expr {
public:
	arith_expr(const expr* left, const char_punctuation_type* o, 
		const expr* right);

	~arith_expr();

	ostream&
	what(ostream& o) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;
};	// end class arith_expr

//-----------------------------------------------------------------------------
/**
	Class of relational expressions.  
	Takes ints as arguments and returns a bool.  
 */
class relational_expr : public binary_expr {
public:
	relational_expr(const expr* left, const char_punctuation_type* o, 
		const expr* right);

	~relational_expr();

	ostream&
	what(ostream& o) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;
};	// end class relational_expr

//-----------------------------------------------------------------------------
/**
	Class of logical expressions
	Takes bools as arguments and returns a bool.  
 */
class logical_expr : public binary_expr {
public:
	logical_expr(const expr* left, const char_punctuation_type* o, 
		const expr* right);

	~logical_expr();

	ostream&
	what(ostream& o) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;
	CHECK_PRS_EXPR_PROTO;
};	// end class logical_expr

//-----------------------------------------------------------------------------
/**
	Repetition of production rules in a loop.  
 */
class loop_operation : public expr {
protected:
	const excl_ptr<const char_punctuation_type>	lp;
	const excl_ptr<const char_punctuation_type>	op;
	const excl_ptr<const token_identifier>	index;
	const excl_ptr<const range>		bounds;
	const excl_ptr<const expr>		body;
	const excl_ptr<const char_punctuation_type>	rp;
public:
	loop_operation(const char_punctuation_type* l,
		const char_punctuation_type* o, 
		const token_identifier* id, const range* b,
		const expr* e, const char_punctuation_type* r);

	~loop_operation();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;
	CHECK_PRS_EXPR_PROTO;
};	// end class loop_operation

//=============================================================================
// typedef	expr_list			array_concatenation_base;

/**
	Concatenation of arrays to make bigger arrays.  

	(2005-05-12)
	This is always used to wrap around even simple instance references, 
		according to the semantic actions in the grammar.  
	Q: is this restricted to instance references, 
		or can this include values as well?
	A: should handle both cases, but be resolved at check-time
 */
class array_concatenation : public expr, public expr_list {
protected:
	typedef	expr_list			parent_type;
public:
	explicit
	array_concatenation(const expr* e);

	~array_concatenation();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;

	// have to do something different
	CHECK_GENERIC_PROTO;
};	// end class array_concatenation

//-----------------------------------------------------------------------------
/**
	Iterative concatenation, when the number elements is dynamic.  
 */
class loop_concatenation : public expr {
protected:
	const excl_ptr<const char_punctuation_type>	lp;
	const excl_ptr<const token_identifier>		id;
	const excl_ptr<const range>			bounds;
	const excl_ptr<const expr>			ex;
	const excl_ptr<const char_punctuation_type>	rp;
public:
	loop_concatenation(const char_punctuation_type* l, 
		const token_identifier* i, 
		const range* rng, 
		const expr* e, 
		const char_punctuation_type* r);

	~loop_concatenation();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;

	// have to do something different (see array_concatenation explanation)
	CHECK_GENERIC_PROTO;
};	// end class loop_concatenation

//-----------------------------------------------------------------------------
/**
	Constructing a higher dimension array from an existing array.  
	Basically wrapping in braces adds one dimension.  
 */
class array_construction : public expr {
protected:
	const excl_ptr<const char_punctuation_type>	lb;
	/** either simple expression or concatenation */
//	const excl_ptr<const expr>		ex;
	const excl_ptr<const expr_list>		ex;
	const excl_ptr<const char_punctuation_type>	rb;
public:
	array_construction(const char_punctuation_type* l, 
		const expr_list* e, const char_punctuation_type* r);

	~array_construction();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;

	// have to do something different (see array_concatenation explanation)
	CHECK_GENERIC_PROTO;
};	// end class array_construction

//=============================================================================

}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_EXPR_H__

