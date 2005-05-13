/**
	\file "AST/art_parser_expr.h"
	Expression-related parser classes for ART.
	$Id: art_parser_expr.h,v 1.12.2.6 2005/05/13 21:16:37 fang Exp $
 */

#ifndef __AST_ART_PARSER_EXPR_H__
#define __AST_ART_PARSER_EXPR_H__

#include "AST/art_parser_expr_base.h"
#include "AST/art_parser_identifier.h"

namespace ART {
namespace parser {
//=============================================================================
// class expr defined in "art_parser_expr_base.h"

//=============================================================================
/**
	Generalized scoped identifier expression.  
	Has two modes: absolute or relative, depending on whether or 
	not the un/qualified identifier was prefixed by an additional
	scope (::) operator.  
	Even short unqualified identifiers are wrapped into this class
	for uniform use.  
	When type checking with this class, remember to check the 
	path mode before searching, and use all but the last identifier
	chain as the namespace path prefix.  
	e.g. for A::B::C, search for namespace match of A::B with member C.  
 */
class id_expr : public inst_ref_expr {
protected:
	typedef	inst_ref_expr		parent_type;
	/**
		Wraps around a qualified_id.  
		Is owned and non-transferrable.  
	 */
	const excl_ptr<qualified_id>	qid;
public:
	explicit
	id_expr(qualified_id* i);

	id_expr(const id_expr& i);

	~id_expr();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_REFERENCE_PROTO;

	never_ptr<const qualified_id>
	get_id(void) const { return qid; }
//		{ return never_ptr<const qualified_id>(qid); }
		// gcc-2.95.3 dies on this.

/// Tags this id_expr as absolute, to be resolved from the global scope.  
	qualified_id*
	force_absolute(const string_punctuation_type* s);

	bool
	is_absolute(void) const;

// want a method for splitting off the last id, isolating namespace portion
	qualified_id
	copy_namespace_portion(void) const
		{ return qid->copy_namespace_portion(); }
		// remember to delete this after done using!?

	friend
	ostream&
	operator << (ostream& o, const id_expr& id);
};	// end class id_expr

//=============================================================================
/// abstract base class for unary expressions
class unary_expr : public expr {
protected:
	const excl_ptr<const expr>	e;	///< the argument expr
	const excl_ptr<const terminal>	op;	///< the operator, may be null
public:
	unary_expr(const expr* n, const terminal* o);

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
	prefix_expr(const terminal* o, const expr* n);
	~prefix_expr();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_EXPR_PROTO;

};	// end class prefix_expr

//-----------------------------------------------------------------------------
/// class for member (of user-defined type) expressions
// is not really unary, derive directly from expr?
// final class?
class member_expr : public inst_ref_expr {
protected:
	typedef	inst_ref_expr		parent_type;
protected:
	const excl_ptr<const inst_ref_expr>	owner;	///< the argument expr
	/// the member name
	const excl_ptr<const token_identifier>	member;
public:
	member_expr(const inst_ref_expr* l, const token_identifier* m);

	// non-default copy-constructor?

	~member_expr();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_REFERENCE_PROTO;
};	// end class member_expr

//-----------------------------------------------------------------------------
/// class for array indexing, with support for multiple dimensions and ranges
class index_expr : public inst_ref_expr {
protected:
	typedef	inst_ref_expr			parent_type;
protected:
	const excl_ptr<const inst_ref_expr>	base;	///< the argument expr
	const excl_ptr<const range_list>	ranges;	///< index
public:
	index_expr(const inst_ref_expr* l, const range_list* i);

	~index_expr();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_REFERENCE_PROTO;

private:
	range_list_return_type
	intercept_indices_error(context& c) const;

	inst_ref_expr::return_type
	intercept_base_ref_error(context& c) const;
};	// end class index_expr

//=============================================================================
/// base class for general binary expressions
class binary_expr : public expr {
protected:
	const excl_ptr<const expr> 	l;	///< left-hand side
	const excl_ptr<const terminal>	op;	///< operator
	const excl_ptr<const expr>	r;	///< right-hand side
public:
	binary_expr(const expr* left, const terminal* o, const expr* right);

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
	arith_expr(const expr* left, const terminal* o, const expr* right);

	~arith_expr();

	ostream&
	what(ostream& o) const;

	CHECK_EXPR_PROTO;
};	// end class arith_expr

//-----------------------------------------------------------------------------
/**
	Class of relational expressions.  
	Takes ints as arguments and returns a bool.  
 */
class relational_expr : public binary_expr {
public:
	relational_expr(const expr* left, const terminal* o, const expr* right);

	~relational_expr();

	ostream&
	what(ostream& o) const;

	CHECK_EXPR_PROTO;
};	// end class relational_expr

//-----------------------------------------------------------------------------
/**
	Class of logical expressions
	Takes bools as arguments and returns a bool.  
 */
class logical_expr : public binary_expr {
public:
	logical_expr(const expr* left, const terminal* o, const expr* right);

	~logical_expr();

	ostream&
	what(ostream& o) const;

	CHECK_EXPR_PROTO;
};	// end class logical_expr

//=============================================================================
typedef	node_list<const expr>			array_concatenation_base;

/**
	Concatenation of arrays to make bigger arrays.  

	(2005-05-12)
	This is always used to wrap around even simple instance references, 
		according to the semantic actions in the grammar.  
	Q: is this restricted to instance references, 
		or can this include values as well?
	A: should handle both cases, but be resolved at check-time
 */
class array_concatenation :
		public expr, public array_concatenation_base {
protected:
	typedef	array_concatenation_base		parent;
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

	CHECK_EXPR_PROTO;

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
//	const excl_ptr<const char_punctuation_type>	pd;
//	const excl_ptr<const char_punctuation_type>	col1;
	const excl_ptr<const token_identifier>		id;
//	const excl_ptr<const char_punctuation_type>	col2;
	const excl_ptr<const range>			bounds;
//	const excl_ptr<const char_punctuation_type>	col3;
	const excl_ptr<const expr>			ex;
	const excl_ptr<const char_punctuation_type>	rp;
public:
	loop_concatenation(const char_punctuation_type* l, 
//		const char_punctuation_type* h, 
//		const char_punctuation_type* c1, 
		const token_identifier* i, 
//		const char_punctuation_type* c2, 
		const range* rng, 
//		const char_punctuation_type* c3, 
		const expr* e, 
		const char_punctuation_type* r);

	~loop_concatenation();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_EXPR_PROTO;

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
	const excl_ptr<const expr>		ex;
	const excl_ptr<const char_punctuation_type>	rb;
public:
	array_construction(const char_punctuation_type* l, 
		const expr* e, const char_punctuation_type* r);

	~array_construction();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_EXPR_PROTO;

	// have to do something different (see array_concatenation explanation)
	CHECK_GENERIC_PROTO;
};	// end class array_construction

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_EXPR_H__

