// "art_parser_expr.h"
// parser classes for ART only!
// this segment is limited to those classes related to expressions.  

#ifndef __ART_PARSER_EXPR_H__
#define __ART_PARSER_EXPR_H__

#include <iosfwd>
#include <string>

#include "art_parser_base.h"

#if 0
#include "art_macros.h"
#include "art_utils.h"		// for token_position
#include "sublist.h"		// for efficient list slices
#include "ptrs.h"		// experimental pointer classes
#include "count_ptr.h"		// reference-counted pointers
#endif

/**
	This is the general namespace for all ART-related classes.  
 */
namespace ART {
//=============================================================================

// forward declaration of outside namespace and classes
namespace entity {
	// defined in "art_object.h"
	class object;
	class enum_datatype_def;
	class process_definition;
}

using namespace std;
using namespace entity;
using namespace fang;		// for experimental pointer classes

//=============================================================================
/// This namespace is reserved for ART's parser-related classes.  
/**
	This contains all of the classes for the abstract syntax tree (AST).  
	Each class should implement recursive methods of traversal.  
	The created AST will only reflect a legal instance of the grammar, 
	therefore, one should use the type-check and build phase to 
	return a more useful manipulate ART object.  
 */
namespace parser {
//=============================================================================
// forward declarations in this namespace
	class node;		// from "art_parser.h"
	class token_char;	// defined here
	class token_string;	// defined here
	class qualified_id;	// defined here
	class concrete_type_ref;	// defined here
	class context;		// defined in "art_symbol_table.h"

//=============================================================================
// class expr defined in "art_parser.h"

//=============================================================================
/**
	An expression in parentheses, for all purposes, parens may be ignored.
	Pretty retarded, class is just a wrapper containing paren tokens.  
	Consider discarding later.  
 */
class paren_expr : public expr {
protected:
	const excl_const_ptr<token_char>	lp;	///< left parenthesis
	const excl_const_ptr<expr>		e;	///< enclosed expression
	const excl_const_ptr<token_char>	rp;	///< right parenthesis
public:
	paren_expr(const token_char* l, const expr* n, const token_char* r);
virtual	~paren_expr();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class paren_expr

//=============================================================================
#if 1
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
class id_expr : public expr {
protected:
	/**
		Wraps around a qualified_id.  
		Is owned and non-transferrable.  
	 */
	const excl_ptr<qualified_id>	qid;
public:
explicit id_expr(qualified_id* i);
	id_expr(const id_expr& i);
virtual	~id_expr();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

// should return a type object, with which one may pointer compare
//	with typedefs, follow to canonical
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;

never_const_ptr<qualified_id> get_id(void) const
		{ return qid; }
//		{ return never_const_ptr<qualified_id>(qid); }
		// gcc-2.95.3 dies on this.

/// Tags this id_expr as absolute, to be resolved from the global scope.  
qualified_id*	force_absolute(token_string* s);
bool		is_absolute(void) const;

// want a method for splitting off the last id, isolating namespace portion
qualified_id	copy_namespace_portion(void) const
			{ return qid->copy_namespace_portion(); }
		// remember to delete this after done using!?

friend	ostream& operator << (ostream& o, const id_expr& id);
};	// end class id_expr
#endif

//=============================================================================
/**
	Can describe either a range of integers (inclusive) or a 
	single integer.  Often found in sparse or multidimensional 
	array declarations and expressions.  
	Ranges are considered expressions, and consist of expressions.  
	Class is final, no subclasses.  
 */
class range : public node {
protected:
	const excl_const_ptr<expr>	lower;	///< inclusive lower bound
	const excl_const_ptr<terminal>	op;	///< range operator token ".."
	const excl_const_ptr<expr>	upper;	///< inclusive upper bound
public:
/// simple constructor for when range is just one integer expression
	range(const expr* l);
/**
	Full range constructor with min and max.  
 */
	range(const expr* l, const terminal* o, const expr* u);
	~range();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class range

//-----------------------------------------------------------------------------
/// base class for range_list
typedef node_list<range,none>	range_list_base;

/**
	All sparse range lists are no C-style x[N][M], 
	now limited to 4-dimensions.  
 */
class range_list : public range_list_base {
protected:
	typedef	range_list_base				parent;
	// no additional members
public:
	range_list(const range* r);
	~range_list();

	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class range_list

#define range_list_wrap(b,l,e)						\
	IS_A(range_list*, l->wrap(b,e))
#define range_list_append(l,d,n)					\
	IS_A(range_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/// base class for dense_range_list
typedef node_list<expr,none>	dense_range_list_base;

/**
	All dense range lists are no C-style x[N][M], 
	now limited to 4-dimensions.  
 */
class dense_range_list : public dense_range_list_base {
protected:
	typedef	dense_range_list_base			parent;
	// no additional members
public:
	dense_range_list(const expr* r);
	~dense_range_list();

	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class range_list

#define dense_range_list_wrap(b,l,e)					\
	IS_A(dense_range_list*, l->wrap(b,e))
#define dense_range_list_append(l,d,n)					\
	IS_A(dense_range_list*, l->append(d,n))

//=============================================================================
/// abstract base class for unary expressions
class unary_expr : public expr {
protected:
	const excl_const_ptr<expr>	e;	///< the argument expr
	const excl_const_ptr<terminal>	op;	///< the operator, may be null
public:
	unary_expr(const expr* n, const terminal* o);
virtual	~unary_expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
};	// end class unary_expr

//-----------------------------------------------------------------------------
/// class for prefix unary expressions
class prefix_expr : public unary_expr {
public:
	prefix_expr(const terminal* o, const expr* n);
virtual	~prefix_expr();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class prefix_expr

//-----------------------------------------------------------------------------
/// class for postfix unary expressions
class postfix_expr : public unary_expr {
public:
	postfix_expr(const expr* n, const terminal* o);
virtual	~postfix_expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
};	// end class postfix_expr

//-----------------------------------------------------------------------------
/// class for member (of user-defined type) expressions
// is not really unary, derive directly from expr?
// final class?
class member_expr : public postfix_expr {
protected:
	/// the member name
	const excl_const_ptr<token_identifier>	member;
public:
	member_expr(const expr* l, const terminal* op, 
		const token_identifier* m);
	~member_expr();

	ostream& what(ostream& o) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class member_expr

//-----------------------------------------------------------------------------
/// class for array indexing, with support for multiple dimensions and ranges
// final class?
class index_expr : public postfix_expr {
protected:
	const excl_const_ptr<range_list>	ranges;		///< index
public:
	index_expr(const expr* l, const range_list* i);
	~index_expr();

	ostream& what(ostream& o) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class index_expr

//=============================================================================
/// base class for general binary expressions
class binary_expr : public expr {
protected:
	const excl_const_ptr<expr> 	l;	///< left-hand side
	const excl_const_ptr<terminal>	op;	///< operator
	const excl_const_ptr<expr>	r;	///< right-hand side
public:
	binary_expr(const expr* left, const terminal* o, const expr* right);
virtual	~binary_expr();

virtual	ostream& what(ostream& o) const = 0;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
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

	ostream& what(ostream& o) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
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

	ostream& what(ostream& o) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
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

	ostream& what(ostream& o) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class logical_expr

//=============================================================================
typedef	node_list<expr,pound>		array_concatenation_base;
/**
	Concatenation of arrays to make bigger arrays.  
 */
class array_concatenation : public expr, public array_concatenation_base {
protected:
	typedef	array_concatenation_base		parent;
public:
	array_concatenation(const expr* e);
	~array_concatenation();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class array_concatenation

#define array_concatenation_wrap(b,l,e)					\
	IS_A(array_concatenation*, l->wrap(b,e))
#define array_concatenation_append(l,d,n)				\
	IS_A(array_concatenation*, l->append(d,n))

//-----------------------------------------------------------------------------
/**
	Iterative concatenation, when the number elements is dynamic.  
 */
class loop_concatenation : public expr {
protected:
	const excl_const_ptr<token_char>		lp;
	const excl_const_ptr<token_char>		pd;
	const excl_const_ptr<token_char>		col1;
	const excl_const_ptr<token_identifier>		id;
	const excl_const_ptr<token_char>		col2;
	const excl_const_ptr<range>			bounds;
	const excl_const_ptr<token_char>		col3;
	const excl_const_ptr<expr>			ex;
	const excl_const_ptr<token_char>		rp;
public:
	loop_concatenation(const token_char* l, const token_char* h, 
		const token_char* c1, const token_identifier* i, 
		const token_char* c2, const range* rng, 
		const token_char* c3, const expr* e, 
		const token_char* r);
	~loop_concatenation();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class loop_concatenation

//-----------------------------------------------------------------------------
/**
	Constructing a higher dimension array from an existing array.  
	Basically wrapping in braces adds one dimension.  
 */
class array_construction : public expr {
protected:
	const excl_const_ptr<token_char>	lb;
	/** either simple expression or concatenation */
	const excl_const_ptr<expr>		ex;
	const excl_const_ptr<token_char>	rb;
public:
	array_construction(const token_char* l, 
		const expr* e, const token_char* r);
	~array_construction();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class array_construction

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_EXPR_H__

