//-----------------------------------------------------------------------------
// "art_parser.h"
// parser classes for ART only!

#ifndef __ART_PARSER_H__
#define __ART_PARSER_H__

#include <iosfwd>
#include <string>

#include "art_macros.h"
#include "art_utils.h"		// for token_position
#include "list_of_ptr.h"	// includes <list>

/// This is the general namespace for all ART-related classes.  
namespace ART {
//=============================================================================
using namespace std;

// forward declaration of outside namespace and classes
namespace entity {
	class object;		// defined in "art_object.h"
};

using namespace entity;

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
class context;			// defined in art_symbol_table.h
class token_char;		// defined here
class token_string;		// defined here

//=============================================================================
// some constant delimiter strings, defined in art_parser.cc
// these are used as delimiter template arguments for node_list
extern	const char	none[];
extern	const char	comma[];
extern	const char	semicolon[];
extern	const char	scope[];
extern	const char	thickbar[];
extern	const char	colon[];

//=============================================================================
/// the abstract base class for parser nodes, universal return type
/**
	The mother class.  
	Rather than have to rely on unions in the lexer and parser, 
	we prefer to use a single type of node for manipulation.  
	This will be useful when we wish to dump the parser's value stack
	in error reporting and debugging.  
	All immediate subclasses of node must be virtually inherited.  
 */
class node {
public:
///	standard virtual destructor
virtual	~node();

/**
	Shows representation without recursive descent.  
	Derived classes of non-terminals should just print their type name.  
 */
virtual	ostream& what(ostream& o) const = 0;
/// shows the position where node starts
virtual	line_position leftmost(void) const = 0;
/// shows the position where node ends
virtual	line_position rightmost(void) const = 0;
/// shows range of file position covered by a particular node
virtual	line_range where(void) const;

// will type-check and return a usable ART::entity::object
virtual	object* check_build(context* c) const;
};

//=============================================================================
/// abstract base class for terminal tokens, mainly to be used by lexer
class terminal : virtual public node {
protected:
/// the position in the file where token was found, pos.off is unused
	line_position	pos;
// file name will be kept separate?
protected:
///	base constructor always records the current position of the token
	terminal();

public:
///	standard virtual destructor
virtual	~terminal();

public:
virtual	int string_compare(const char* d) const = 0;
virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

};

//=============================================================================
#define	NODE_LIST_BASE_TEMPLATE_SPEC					\
	template <class T>

typedef	list_of_ptr<node>	node_list_base_parent;
/**
	This is the general class for list structures of nodes in the
	syntax tree.  What is unique about this implementation is that
	its derived classes keep track of the delimiter tokens that 
	separated the lists, if applicable.  
	The specifier T, a derived class from node, is only used for 
	type-checking.  
 */
NODE_LIST_BASE_TEMPLATE_SPEC
class node_list_base : virtual public node, public node_list_base_parent {
private:
	typedef		list<node*>			list_grandparent;
	typedef		list_of_ptr<node>		list_parent;
public:
	typedef		list_parent::iterator		iterator;
	typedef		list_parent::const_iterator	const_iterator;
public:
	node_list_base();
	node_list_base(const list_grandparent& l);
// initializing with first element, T must be subclass of node!
	node_list_base(node* n);

virtual	~node_list_base();

using	list_parent::begin;
using	list_parent::end;

// later, use static functions (operator <<) to determine type name...
virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

//-----------------------------------------------------------------------------
#define	NODE_LIST_TEMPLATE_SPEC_DEFAULT					\
	template <class T, const char D[] = none>
#define	NODE_LIST_TEMPLATE_SPEC						\
	template <class T, const char D[]>
#define	NODE_LIST_TEMPLATE_SPEC_EXPORT					\
	export NODE_LIST_TEMPLATE_SPEC
	// "export" is ignored by gcc, even 3.3

/**
	The delimiter specifier, D, is used for checking that every other
	token is separated by a D character (if D is not '\0').  
 */
NODE_LIST_TEMPLATE_SPEC_DEFAULT
class node_list : public node_list_base<T> {
private:
	typedef		node_list_base<T>	parent;
public:
	typedef	typename parent::iterator	iterator;
	typedef	typename parent::const_iterator	const_iterator;
	typedef	typename parent::reverse_iterator	reverse_iterator;
	typedef	typename parent::const_reverse_iterator	const_reverse_iterator;

protected:
	terminal*	open;		///< wrapping string, such as "("
	terminal*	close;		///< wrapping string, such as ")"
public:
	node_list();
	node_list(const parent& l);
	node_list(node* n);
virtual	~node_list();

using	parent::begin;
using	parent::end;

/**
	This attaches enclosing text, such as brackets, braces, parentheses, 
	around a node_list.  The arguments are type-checked as token_strings.  
 */
virtual	node_list<T,D>* wrap(node* b, node* e);		// not inlined

/**
	Adds an element to a node list, along with the delimiting
	character token.  Also checks that delimiter matchs, and
	that type of argument matches.
 */
virtual	node_list<T,D>* append(node* d, node* n);

// the following methods are defined in "art_parser_template_methods.h"

/// prints out type of first element in list, if not null
// later, use static functions (operator <<) to determine type name...
virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

virtual	object* check_build(context* c) const;
};	// end of class node_list<>

//=============================================================================
/**
	Abstract base class for root-level items.  
	Assertion: all root items are nonterimnals.  
 */
class root_item : virtual public node {
public:
	root_item();
virtual	~root_item();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

typedef node_list<root_item>	root_body;

#define root_body_wrap(b,l,e)					\
	IS_A(root_body*, l)->wrap(b,e)
#define root_body_append(l,d,n)					\
	IS_A(root_body*, l)->append(d,n) 


//=============================================================================
/**
	Abstract base class for general expressions.  
	Expressions may be terminal or nonterminal.  
 */
class expr : virtual public node {
public:
	expr();
virtual	~expr();

virtual	ostream& what(ostream& o) const  = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

///	all expression lists are comma-separated
typedef node_list<expr,comma>	expr_list;

#define expr_list_wrap(b,l,e)						\
	IS_A(expr_list*, l)->wrap(b,e)
#define expr_list_append(l,d,n)						\
	IS_A(expr_list*, l)->append(d,n) 


//=============================================================================
/// single token characters
class token_char : public terminal {
protected:
/// the character
	int c;
public:
	token_char(const int i);
virtual	~token_char();

virtual	int string_compare(const char* d) const;
virtual	ostream& what(ostream& o) const;
};

//=============================================================================
/// an expression in parentheses, for all purposes, parens may be ignored
class paren_expr : public expr {
protected:
	token_char*		lp;		///< left parenthesis
	expr*			e;		///< enclosed expression
	token_char*		rp;		///< right parenthesis
public:
	paren_expr(node* l, node* n, node* r);
virtual	~paren_expr();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// stores an integer (long) in native form and retains position information
class token_int : public terminal, public expr {
protected:
	long val;			///< the value
public:
/// standard constructor
	token_int(const long v);
/// standard virtual destructor
virtual	~token_int();

virtual	int string_compare(const char* d) const;
virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// stores an float (double) in native form and retains position information
class token_float : public terminal, public expr {
protected:
	double val;			///< the value
public:
/// standard constructor
	token_float(const double v);
/// standard virtual destructor
virtual	~token_float();

virtual	int string_compare(const char* d) const;
virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/**
	This class extends functionality of the standard string for 
	the lexer and parser.  
	Subclasses thereof are intended for use by the lexer.  
	This class also keeps track of the line and column position in a 
	file for a given token.  
	For now, this class is used for punctuation tokens.  How silly.  
	For identifier, use token_identifier.  
	For quoted strings, use token_quoted_string.  
 */
class token_string : public string, public terminal {
public:
/// uses base class' constructors to copy text and record position
	token_string(const char* s);
virtual	~token_string();

virtual	int string_compare(const char* d) const;
virtual	ostream& what(ostream& o) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// class reserved for identifier tokens
class token_identifier : public token_string, public expr {
					// consider postfix_expr?
public:
	token_identifier(const char* s);
virtual	~token_identifier();
virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<token_identifier,scope>	id_expr_base;

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
class id_expr : public expr, public id_expr_base {
public:
	typedef	id_expr_base::iterator		iterator;
	typedef	id_expr_base::const_iterator	const_iterator;
protected:
	/**
		Indicates whether identifier is absolute, meaning
		that it is to be resolved from the global scope down, 
		as opposed to inner scope outward (relative).
		Particularly useful for disambiguation.
	 */
	token_string*			absolute;
public:
explicit id_expr(node* n);
	id_expr(const id_expr& i);
virtual	~id_expr();

/// Tags this id_expr as absolute, to be resolved from the global scope.  
id_expr*	force_absolute(node* s);

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

// should return a type object, with which one may pointer compare
//	with typedefs, follow to canonical
// virtual	object* check_build(context* c) const;

friend	ostream& operator << (ostream& o, const id_expr& id);
};

// no need for wrap, ever
#define id_expr_append(l,d,n)						\
	IS_A(id_expr*, l)->append(d,n)

//-----------------------------------------------------------------------------
/// keyword version of token_string class, not necessarily an expr
class token_keyword : public token_string {
public:
	token_keyword(const char* s);
virtual	~token_keyword();

virtual	ostream& what(ostream& o) const;
};

//-----------------------------------------------------------------------------
/// class for expression keywords, which happen to be only bools
class token_bool : public token_keyword, public expr {
public:
	token_bool(const char* tf);
virtual	~token_bool();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// class for "else" keyword, which is a legitimate expr
class token_else : public token_keyword, public expr {
public:
	token_else(const char* tf);
virtual	~token_else();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// quoted-string version of token_string class
class token_quoted_string : public token_string, public expr {
public:
	token_quoted_string(const char* s);
virtual	~token_quoted_string();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/**
	Can describe either a range of integers (inclusive) or a 
	single integer.  Often found in sparse or multidimensional 
	array declarations and expressions.  
	Ranges are considered expressions, and consist of expressions.  
 */
class range : public expr {
protected:
	expr*		lower;		///< inclusive lower bound
	terminal*	op;		///< range operator token ".."
	expr*		upper;		///< inclusive upper bound
public:
/// simple constructor for when range is just one integer expression
	range(node* l);
/**
	Failure to dynamic_cast will result in assignment to a NULL pointer, 
	which will be detected, and properly memory managed, assuming
	that the arguments exclusively "owned" their memory locations.  
 */
	range(node* l, node* o, node* u);
virtual	~range();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

/// all range lists are comma-separated
typedef node_list<range,comma>	range_list;

#define range_list_wrap(b,l,e)						\
	IS_A(range_list*, l)->wrap(b,e)
#define range_list_append(l,d,n)					\
	IS_A(range_list*, l)->append(d,n)

//=============================================================================
/// abstract base class for unary expressions
class unary_expr : public expr {
protected:
	expr*		e;		///< the argument expr
	terminal*	op;		///< the operator, may be null
public:
	unary_expr(node* n, node* o);
virtual	~unary_expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

//-----------------------------------------------------------------------------
/// class for prefix unary expressions
class prefix_expr : public unary_expr {
public:
	prefix_expr(node* op, node* n);
virtual	~prefix_expr();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// class for postfix unary expressions
class postfix_expr : public unary_expr {
public:
	postfix_expr(node* n, node* op);
virtual	~postfix_expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// class for member (of user-defined type) expressions
class member_expr : public postfix_expr {
protected:
	expr*		member;		// should be an id_expr or token_string
public:
	member_expr(node* l, node* op, node* m);
virtual	~member_expr();

virtual	ostream& what(ostream& o) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// class for array indexing, with support for multiple dimensions and ranges
class index_expr : public postfix_expr {
protected:
	range_list*		ranges;		///< index
public:
	index_expr(node* l, node* i);
virtual	~index_expr();

virtual	ostream& what(ostream& o) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// base class for general binary expressions
class binary_expr : public expr {
protected:
	expr* 		l;			///< left-hand side
	terminal*	op;			///< operator
	expr*		r;			///< right-hand side
public:
	binary_expr(node* left, node* o, node* right);
virtual	~binary_expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// class of arithmetic expressions
class arith_expr : public binary_expr {
public:
	arith_expr(node* left, node* o, node* right);
virtual	~arith_expr();

virtual	ostream& what(ostream& o) const;
};

//-----------------------------------------------------------------------------
/// class of relational expressions
class relational_expr : public binary_expr {
public:
	relational_expr(node* left, node* o, node* right);
virtual	~relational_expr();

virtual	ostream& what(ostream& o) const;
};

//-----------------------------------------------------------------------------
/// class of logical expressions
class logical_expr : public binary_expr {
public:
	logical_expr(node* left, node* o, node* right);
virtual	~logical_expr();

virtual	ostream& what(ostream& o) const;
};

//=============================================================================
/// abstract base class for type
class type_base : virtual public node {
public:
	type_base();
virtual	~type_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

//-----------------------------------------------------------------------------
/// keywords that are also types
class token_type : public token_keyword, public type_base {
public:
	token_type(const char* tf);
virtual	~token_type();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// type identifier with optional template parameters
class type_id : public type_base {
protected:
/**
	Base type's name.  Can only be an id_expr.  
 */
	id_expr*		base;
	expr_list*		temp_spec;	///< template arguments
public:
	type_id(node* b, node* t);
virtual	~type_id();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// base type for data, such as int...
class data_type_base : public type_base {
protected:
	token_keyword*		type;		// generalize to structs?
	token_char*		la;		// optional angle bracket
	token_int*		width;		// integer width (optional)
	token_char*		ra;
public:
	data_type_base(node* t, node* l, node* w, node* r);
	data_type_base(node* t);
virtual	~data_type_base();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	object* check_build(context* c) const;
};

/// list of base data types
typedef node_list<data_type_base,comma>	base_data_type_list;

// construction macros
#define base_data_type_list_wrap(b,l,e)					\
	IS_A(base_data_type_list*, l)->wrap(b,e)
#define base_data_type_list_append(l,d,n)				\
        IS_A(base_data_type_list*, l)->append(d,n)

//-----------------------------------------------------------------------------
/// full base channel type, including base type list
class chan_type : public type_base {
protected:
	token_keyword*		chan;		///< keyword "channel"
	token_char*		dir;		///< port direction: in or out
	base_data_type_list*	dtypes;		///< data types communicated
public:
	chan_type(node* c, node* d = NULL, node* t = NULL);
virtual	~chan_type();

chan_type* attach_data_types(node* t);

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

#define	chan_type_attach_data_types(ct,t)				\
	IS_A(chan_type*, ct)->attach_data_types(t)

//=============================================================================
/**
	Base class for statements (assignments, increment, decrement...)
	Assertion: all statements are nonterminals.  
 */
class statement : virtual public node {
public:
	statement();
virtual	~statement();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

//-----------------------------------------------------------------------------
/// class for unary expression statements, such as increment and decrement
class incdec_stmt : virtual public statement {
protected:
	expr*		e;
	terminal*	op;
public:
	incdec_stmt(node* n, node* o);
virtual	~incdec_stmt();

virtual	expr* release_expr(void);
virtual	terminal* release_op(void);

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// class for binary expression statements, with left- and right-hand sides
class assign_stmt : virtual public statement {
protected:
	expr*		lhs;			///< destination
	terminal*	op;			///< operation
	expr*		rhs;			///< source expression
public:
	assign_stmt(node* left, node* o, node* right);
virtual	~assign_stmt();

virtual	expr* release_lhs(void);
virtual	terminal* release_op(void);
virtual	expr* release_rhs(void);

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/**
	Abstract base class for items that may be found in a definition body.  
	All definition body items are root_item.
	Except language_body...
 */
class def_body_item : virtual public node {
public:
	def_body_item();
virtual	~def_body_item();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

/// definition body is just a list of definition items
typedef	node_list<def_body_item>	definition_body;

#define definition_body_wrap(b,l,e)					\
	IS_A(definition_body*, l)->wrap(b,e)
#define definition_body_append(l,d,n)					\
	IS_A(definition_body*, l)->append(d,n) 

//=============================================================================
/**
	Abstract base class for language bodies.  
	language_body is the only subclass of def_body_item that is 
	not also a subclass of root_item.  
	Language bodies cannot syntactically or semantically appear
	outside of a definition.  
 */
class language_body : public def_body_item {
protected:
	token_keyword*	tag;			///< what language
public:
	language_body(node* t);
virtual	~language_body();

virtual language_body* attach_tag(node* t);

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const = 0;
};

//=============================================================================
/// namespace enclosed body
class namespace_body : public root_item {
protected:
	token_keyword*		ns;		///< keyword "namespace"
	token_identifier*	name;		///< name of namespace
	terminal*		lb;
	node*			body;		///< contents of namespace
	terminal*		rb;
	terminal*		semi;		///< semicolon token
public:
	namespace_body(node* s, node* n, node* l, node* b, node* r, node* c);
virtual	~namespace_body();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

virtual	object* check_build(context* c) const;
};

//-----------------------------------------------------------------------------
/// command to search namespace for identifiers
class using_namespace : public root_item {
protected:
	token_keyword*		open;
	id_expr*		id;
	token_keyword*		as;
	token_identifier*	alias;
	token_char*		semi;
public:
	using_namespace(node* o, node* i, node* s);
		// a "AS" and n (alias) are optional
	using_namespace(node* o, node* i, node* a, node* n, node* s);
virtual	~using_namespace();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

virtual	object* check_build(context* c) const;
};

//=============================================================================
/// basic declaration or instance identifier, no trimmings
class declaration_base : virtual public def_body_item, 
		virtual public root_item {
protected:
/**
	In declaration context, id should only be a token_identifier, 
	but in instantiation contect, id may be a qualified identifier
	with postfix member/indexing.  
	PUNT: Let's not worry about it until the grammar is updated to 
	reflect this.  
 */
	token_identifier*		id;
public:
	declaration_base(node* i);
virtual	~declaration_base();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	line_range where(void) const;
virtual	object* check_build(context* c) const;
};

typedef	node_list<declaration_base,comma>	declaration_id_list;

#define declaration_id_list_wrap(b,l,e)					\
	IS_A(declaration_id_list*, l)->wrap(b,e)
#define declaration_id_list_append(l,d,n)				\
	IS_A(declaration_id_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/// declaration identifier with ranges
class declaration_array : public declaration_base {
protected:
	range_list*		ranges;		///< optional ranges
public:
	declaration_array(node* i, node* rl);
virtual	~declaration_array();

virtual	ostream& what(ostream& o) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// abstract base class of instance items
class instance_base : virtual public def_body_item, virtual public root_item {
public:
	instance_base();
virtual	~instance_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
virtual	line_range where(void) const;
};

//-----------------------------------------------------------------------------
/// class for an instance declaration
class instance_declaration : public instance_base {
protected:
	type_base*		type;
	declaration_id_list*	ids;
	terminal*		semi;
public:
	instance_declaration(node* t, node* i, node* s = NULL);
virtual	~instance_declaration();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual object* check_build(context* c) const;
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// clever re-use of declaration classes
typedef	node_list<instance_declaration,semicolon>	data_param_list;

#define data_param_list_wrap(b,l,e)					\
	IS_A(data_param_list*, l)->wrap(b,e)
#define data_param_list_append(l,d,n)					\
	IS_A(data_param_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/// class for an or instance port connection or declaration connection
class actuals_connection : public instance_base, public declaration_base {
protected:
//	expr*			id;		// inherited
	expr_list*		actuals;	///< connection actuals
	terminal*		semi;		///< semicolon (optional)
public:
	actuals_connection(node* i, node* a, node* s = NULL);
virtual	~actuals_connection();

// remember to check for declaration context when checking id

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	line_range where(void) const;
};

//-----------------------------------------------------------------------------
/// aliasing identifiers is establishing a connection, or assigning a value
class alias_assign : public instance_base, public declaration_base {
protected:
//	expr*			id;		// inherited
	terminal*		op;		///< assign operator
	expr*			rhs;		///< right-hand side
	terminal*		semi;		///< semicolon
public:
	alias_assign(node* i, node* o, node* r, node* s = NULL);
virtual	~alias_assign();

// type check here

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	line_range where(void) const;
};

//-----------------------------------------------------------------------------
/// class for loop instantiations, to be unrolled in the build phase
class loop_instantiation : public instance_base {
protected:
	terminal*		lp;
	terminal*		delim;
	token_identifier*	index;
	terminal*		colon;
	range*			rng;
	definition_body*	body;
	terminal*		rp;
public:
	loop_instantiation(node* l, node* d, node* i, node* c, 
			node* g, node* b, node* r);
virtual	~loop_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// single port formal identifier, with optional dimension array specification
class port_formal_id : public node {
protected:
	token_identifier*	name;		///< formal name
	range_list*		dim;		///< optional dimensions
public:
	port_formal_id(node* n, node* d);
virtual	~port_formal_id();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

/// list of port-formal identifiers (optional arrays)
typedef	node_list<port_formal_id,comma>	port_formal_id_list;

#define port_formal_id_list_wrap(b,l,e)					\
	IS_A(port_formal_id_list*, l)->wrap(b,e)
#define port_formal_id_list_append(l,d,n)				\
	IS_A(port_formal_id_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/// port formal declaration contains a type and identifier list
class port_formal_decl : public node {
protected:
	type_id*		type;		///< formal base type
	port_formal_id_list*	ids;		///< identifier list
public:
	port_formal_decl(node* t, node* i);
virtual	~port_formal_decl();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

/// list of port-formal declarations
typedef	node_list<port_formal_decl,semicolon>	port_formal_decl_list;

#define port_formal_decl_list_wrap(b,l,e)				\
	IS_A(port_formal_decl_list*, l)->wrap(b,e)
#define port_formal_decl_list_append(l,d,n)				\
	IS_A(port_formal_decl_list*, l)->append(d,n) 

//=============================================================================
/// single template formal identifier, with optional dimension array spec.  
class template_formal_id : public node {
protected:
	token_identifier*	name;		///< formal name
	range_list*		dim;		///< optional dimensions
public:
	template_formal_id(node* n, node* d);
virtual	~template_formal_id();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

/// list of template-formal identifiers (optional arrays)
typedef	node_list<template_formal_id,comma>	template_formal_id_list;

#define template_formal_id_list_wrap(b,l,e)				\
	IS_A(template_formal_id_list*, l)->wrap(b,e)
#define template_formal_id_list_append(l,d,n)				\
	IS_A(template_formal_id_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/// template formal declaration contains a type and identifier list
class template_formal_decl : public node {
protected:
	type_id*			type;	///< formal base type
	template_formal_id_list*	ids;	///< identifier list
public:
	template_formal_decl(node* t, node* i);
virtual	~template_formal_decl();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

/// list of template-formal declarations
typedef	node_list<template_formal_decl,semicolon> template_formal_decl_list;

#define template_formal_decl_list_wrap(b,l,e)				\
	IS_A(template_formal_decl_list*, l)->wrap(b,e)
#define template_formal_decl_list_append(l,d,n)				\
	IS_A(template_formal_decl_list*, l)->append(d,n) 

//=============================================================================
/// definition type identifier, with optional template specifier list
class def_type_id : public type_base {
protected:
	token_identifier*		name;	///< definition name base
	/// optional template specifier
	template_formal_decl_list*	temp_spec;
public:
	def_type_id(node* n, node* t);
virtual	~def_type_id();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/**
	Abstract base class for definitions of complex types, 
	including processes, user-defined channels and data-types.  
 */
class definition : public root_item {
public:
	definition();
virtual	~definition();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

//-----------------------------------------------------------------------------
/// abstract base class for prototypes
class prototype : public root_item {
public:
	prototype();
virtual	~prototype();

// don't bother re-declaring virtual methods
};

//=============================================================================
/**
	Abstract base class for basic process information
	(prototype, basically).  
	This class need not be a node or root_item because it's never 
	constructed on the symbol stack.  
 */
class process_signature {		// not a node or root_item!
protected:
	token_keyword*			def;	///< definition keyword
	def_type_id*			idt;	///< identifier [template]
	port_formal_decl_list*		ports;	///< optional port formal list
public:
	process_signature(node* d, node* i, node* p);
virtual	~process_signature();

};

//-----------------------------------------------------------------------------
/// process prototype declaration
class process_prototype : public prototype, public process_signature {
protected:
	token_char*		semi;		///< semicolon token
public:
	process_prototype(node* d, node* i, node* p, node* s);
virtual	~process_prototype();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// process definition
class process_def : public definition, public process_signature {
protected:
//	token_keyword*			def;	//  inherited
//	def_type_id*			idt;	//  inherited
//	port_formal_decl_list*		ports;	//  inherited
	definition_body*		body;	///< definition body
public:
	process_def(node* d, node* i, node* p, node* b);
virtual	~process_def();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// conditional instantiations in definition body
class guarded_definition_body : public instance_base {
protected:
	expr*				guard;	///< condition expression
	terminal*			arrow;	///< right arrow
	definition_body*		body;
public:
	guarded_definition_body(node* e, node* a, node* b);
virtual	~guarded_definition_body();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

/// list of template-formal declarations
typedef	node_list<guarded_definition_body,thickbar>
		guarded_definition_body_list;

#define guarded_definition_body_list_wrap(b,l,e)			\
	IS_A(guarded_definition_body_list*, l)->wrap(b,e)
#define guarded_definition_body_list_append(l,d,n)			\
	IS_A(guarded_definition_body_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/// wrapper class for conditional instantiations
class conditional_instantiation : public instance_base {
protected:
	guarded_definition_body_list*	gd;
public:
	conditional_instantiation(node* n);
virtual	~conditional_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// user-defined data type
class user_data_type_signature {
protected:
	token_keyword*		def;		///< "deftype" keyword
	token_identifier*	name;		///< name of new type
	token_string*		dop;		///< <: operator
	data_type_base*		bdt;		///< the represented type
	data_param_list*	params;		///< the implementation type
public:
	user_data_type_signature(node* df, node* n, node* dp, node* b, node* p);
virtual	~user_data_type_signature();
};

//-----------------------------------------------------------------------------
/// user-defined data type prototype declaration
class user_data_type_prototype : public prototype, 
		public user_data_type_signature {
protected:
	token_char*		semi;		///< semicolon
public:
	user_data_type_prototype(node* df, node* n, node* dp, node* b, 
		node* p, node* s);
virtual	~user_data_type_prototype();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// user-defined data type (is not a type_base)
class user_data_type_def : public definition, public user_data_type_signature {
protected:
//	token_keyword*		def;		// inherited
//	token_identifier*	name;		// inherited
//	token_string*		dop;		// inherited
//	data_type_base*		bdt;		// inherited
//	data_param_list*	params;		// inherited
	token_char*		lb;		///< left brace
	language_body*		setb;		///< set body
	language_body*		getb;		///< get body
	token_char*		rb;		///< right brace
public:
	user_data_type_def(node* df, node* n, node* dp, node* b, node* p, 
		node* l, node* s, node* g, node* r);
virtual	~user_data_type_def();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================
/// user-defined channel type signature
class user_chan_type_signature {
protected:
	token_keyword*		def;		///< "defchan" keyword
	token_identifier*	name;		///< name of new channel
	token_string*		dop;		///< <: operator
	chan_type*		bct;		///< the represented type
	data_param_list*	params;		///< the implementation type
public:
	user_chan_type_signature(node* df, node* n, node* dp, 
		node* b, node* p);
virtual	~user_chan_type_signature();
};

//-----------------------------------------------------------------------------
/// user-defined channel type prototype
class user_chan_type_prototype : public prototype, 
		public user_chan_type_signature {
protected:
	token_char*		semi;		///< semicolon
public:
	user_chan_type_prototype(node* df, node* n, node* dp, node* b, 
		node* p, node* s);
virtual	~user_chan_type_prototype();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//-----------------------------------------------------------------------------
/// user-defined channel type definition
class user_chan_type_def : public definition, public user_chan_type_signature {
protected:
//	token_keyword*		def;		// inherited
//	token_identifier*	name;		// inherited
//	token_string*		dop;		// inherited
//	chan_type*		bct;		// inherited
//	data_param_list*	params;		// inherited
	token_char*		lb;		///< left brace
	language_body*		sendb;		///< set body
	language_body*		recvb;		///< get body
	token_char*		rb;		///< right brace
public:
	user_chan_type_def(node* df, node* n, node* dp, node* b, node* p, 
		node* l, node* s, node* g, node* r);
virtual	~user_chan_type_def();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};

//=============================================================================

};	// end namespace parser
};	// end namespace ART

#endif	// __ART_PARSER_H__

