//-----------------------------------------------------------------------------
// "art_parser.h"
// parser classes for ART only!

#ifndef __ART_PARSER_H__
#define __ART_PARSER_H__

#include <iostream>
#include <string>

#include <stdio.h>		// just for sprintf
#include <string.h>		// for a few C-string functions

#include "art_macros.h"
#include "art_switches.h"
#include "art_utils.h"
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
	node() { }
///	standard virtual destructor
virtual	~node() { }

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
virtual	line_range where(void) const
		{ return line_range(leftmost(), rightmost()); }

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
public:
///	base constructor always records the current position of the token
//	"current" is defined in art_switches.h
	terminal() : node(), pos(current) { }

///	standard virtual destructor
virtual	~terminal() { }

virtual	int string_compare(const char* d) const = 0;
virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const { return pos; }
virtual	line_position rightmost(void) const { return pos; }

};

//=============================================================================
/// abstract base class for non-terminal symbols, mainly to be used by parser
class nonterminal : virtual public node {
public:
	nonterminal() : node() { }
///	standard virtual destructor
virtual ~nonterminal() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	line_range where(void) const { return node::where(); }
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
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
class node_list_base : public nonterminal, public node_list_base_parent {
private:
	typedef		list<node*>			list_grandparent;
	typedef		list_of_ptr<node>		list_parent;
public:
	typedef		list_parent::iterator		iterator;
	typedef		list_parent::const_iterator	const_iterator;
public:
	node_list_base() : nonterminal(), list_of_ptr<node>() { }
	node_list_base(const list_grandparent& l);
// initializing with first element, T must be subclass of node!
	node_list_base(node* n);

virtual	~node_list_base() { }

using	list_parent::begin;
using	list_parent::end;

// later, use static functions (operator <<) to determine type name...
virtual	ostream& what(ostream& o) const;
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
	node_list() : parent(), open(NULL), close(NULL) { }
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
class root_item : public nonterminal {
public:
	root_item() : nonterminal() { }
virtual	~root_item() { }
virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

typedef node_list<root_item>	root_body;

#define root_body_wrap(b,l,e)					\
	dynamic_cast<root_body*>(l)->wrap(b,e)
#define root_body_append(l,d,n)					\
	dynamic_cast<root_body*>(l)->append(d,n) 


//=============================================================================
/**
	Abstract base class for general expressions.  
	Expressions may be terminal or nonterminal.  
 */
class expr : virtual public node {
public:
	expr() : node() { }
virtual	~expr() { }

virtual	ostream& what(ostream& o) const  = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

///	all expression lists are comma-separated
typedef node_list<expr,comma>	expr_list;

#define expr_list_wrap(b,l,e)						\
	dynamic_cast<expr_list*>(l)->wrap(b,e)
#define expr_list_append(l,d,n)						\
	dynamic_cast<expr_list*>(l)->append(d,n) 


//=============================================================================
/// single token characters
class token_char : public terminal {
protected:
/// the character
	int c;
public:
	token_char(const int i) : terminal(), c(i) { }
virtual	~token_char() { }

virtual	int string_compare(const char* d) const 
		{ char cs[2] = { c, 0 }; return strcmp(cs,d); }
virtual	ostream& what(ostream& o) const { return o << (char) c; }
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

virtual	ostream& what(ostream& o) const { return o << "(paren-expr)"; }
virtual	line_position leftmost(void) const { return lp->leftmost(); }
virtual	line_position rightmost(void) const { return rp->rightmost(); }
};

//=============================================================================
/// stores an integer (long) in native form and retains position information
class token_int : public terminal, public expr {
protected:
/// the value
	long val;
public:
/// standard constructor
	token_int(const long v) : terminal(), expr(), val(v) { }
/// standard virtual destructor
virtual	~token_int() { }

virtual	int string_compare(const char* d) const 
		{ char n[64]; sprintf(n, "%ld", val); return strcmp(n,d); }
virtual	ostream& what(ostream& o) const { return o << "int: " << val; }
virtual	line_position leftmost(void) const { return terminal::leftmost(); }
virtual	line_position rightmost(void) const { return terminal::rightmost(); }
};

//=============================================================================
/// stores an float (double) in native form and retains position information
class token_float : public terminal, public expr {
protected:
/// the value
	double val;
public:
/// standard constructor
	token_float(const double v) : terminal(), expr(), val(v) { }
/// standard virtual destructor
virtual	~token_float() { }

virtual	int string_compare(const char* d) const 
		{ char n[64]; sprintf(n, "%f", val); return strcmp(n,d); }
virtual	ostream& what(ostream& o) const { return o << "float: " << val; }
virtual	line_position leftmost(void) const { return terminal::leftmost(); }
virtual	line_position rightmost(void) const { return terminal::rightmost(); }
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
	token_string(const char* s) : string(s), terminal() { }
virtual	~token_string() { }

virtual	int string_compare(const char* d) const { return compare(d); }
virtual	ostream& what(ostream& o) const
		{ return o << "token: " << (const string&) (*this); }
virtual	line_position rightmost(void) const
		{ return line_position(pos.line, pos.col +length() -1); }
};

//-----------------------------------------------------------------------------
/// class reserved for identifier tokens
class token_identifier : public token_string, public expr {
					// consider postfix_expr?
public:
	token_identifier(const char* s) : token_string(s), expr() { }
virtual	~token_identifier() { }
virtual	ostream& what(ostream& o) const 
		{ return o << "identifier: " << (const string&) (*this); }
virtual	line_position leftmost(void) const { return terminal::leftmost(); }
virtual	line_position rightmost(void) const
		{ return token_string::rightmost(); }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<token_identifier,scope>	id_expr_base;

/// generalized scoped identifier expression
class id_expr : public expr, public id_expr_base {
public:
	typedef	id_expr_base::iterator		iterator;
	typedef	id_expr_base::const_iterator	const_iterator;

explicit id_expr(node* n) : expr(), id_expr_base(n) { }
	id_expr(const id_expr& i);
	
virtual	~id_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(id-expr)"; }
virtual	line_position leftmost(void) const { return id_expr_base::leftmost(); }
virtual	line_position rightmost(void) const
		{ return id_expr_base::rightmost(); }

// should return a type object, with which one may pointer compare
//	with typedefs, follow to canonical
// virtual	object* check_build(context* c) const;

friend	ostream& operator << (ostream& o, const id_expr& id);
};

// no need for wrap, ever
#define id_expr_append(l,d,n)						\
	dynamic_cast<id_expr*>(l)->append(d,n)

//-----------------------------------------------------------------------------
/// keyword version of token_string class, not necessarily an expr
class token_keyword : public token_string {
public:
	token_keyword(const char* s) : token_string(s) { }
virtual	~token_keyword() { }

virtual	ostream& what(ostream& o) const
		{ return o << "keyword: " << *((const string*) this); }
};

//-----------------------------------------------------------------------------
/// class for expression keywords, which happen to be only bools
class token_bool : public token_keyword, public expr {
public:
	token_bool(const char* tf) : token_keyword(tf), expr()
		{ assert(!strcmp(tf,"true") || !strcmp(tf,"false")); }
virtual	~token_bool() { }
virtual	ostream& what(ostream& o) const
		{ return o << "bool: " << *((const string*) this); }
virtual	line_position leftmost(void) const { return terminal::leftmost(); }
virtual	line_position rightmost(void) const
		{ return token_string::rightmost(); }
};

//-----------------------------------------------------------------------------
/// class for "else" keyword, which is a legitimate expr
class token_else : public token_keyword, public expr {
public:
	token_else(const char* tf) : token_keyword(tf), expr()
		{ assert(!strcmp(tf,"else")); }
virtual	~token_else() { }

virtual	ostream& what(ostream& o) const
		{ return o << "keyword: " << *((const string*) this); }
virtual	line_position leftmost(void) const { return terminal::leftmost(); }
virtual	line_position rightmost(void) const { return terminal::rightmost(); }
};

//-----------------------------------------------------------------------------
/// quoted-string version of token_string class
class token_quoted_string : public token_string, public expr {
public:
	token_quoted_string(const char* s) : token_string(s), expr() { }
virtual	~token_quoted_string() { }

virtual	ostream& what(ostream& o) const { 
		// punt: handle special characters later...
		return ((const token_string*) this)->what(o << "string: \"") 
			<< "\"";
	}
virtual	line_position leftmost(void) const { return terminal::leftmost(); }
virtual	line_position rightmost(void) const { return terminal::rightmost(); }
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

virtual	ostream& what(ostream& o) const { return o << "(range)"; }
virtual	line_position leftmost(void) const { return lower->leftmost(); }
virtual	line_position rightmost(void) const {
	if (upper)	return upper->rightmost();
	else if (op)	return op->rightmost();
	else		return lower->rightmost();
}
};

/// all range lists are comma-separated
typedef node_list<range,comma>	range_list;

#define range_list_wrap(b,l,e)						\
	dynamic_cast<range_list*>(l)->wrap(b,e)
#define range_list_append(l,d,n)					\
	dynamic_cast<range_list*>(l)->append(d,n)


//=============================================================================
/// abstract base class for unary expressions
class unary_expr : public expr, public nonterminal {
protected:
	expr*		e;		///< the argument expr
	terminal*	op;		///< the operator, may be null
	// what if is [index]? op2? figure out later...
public:
/**
	Failure to dynamic_cast will result in assignment to a NULL pointer, 
	which will be detected, and properly memory managed, assuming
	that the arguments exclusively "owned" their memory locations.  
 */
	unary_expr(node* n, node* o) : expr(), nonterminal(), 
		e(dynamic_cast<expr*>(n)), 
		op(dynamic_cast<terminal*>(o)) {
			if (n && !e) delete n;	// or use assert?
			if (o && !op) delete o;
		}
virtual	~unary_expr() { SAFEDELETE(e); SAFEDELETE(op); }

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

//-----------------------------------------------------------------------------
/// class for prefix unary expressions
class prefix_expr : public unary_expr {
public:
	prefix_expr(node* op, node* n) : unary_expr(n,op) { }
virtual	~prefix_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(prefix-expr)"; }
virtual	line_position leftmost(void) const { return op->leftmost(); }
virtual	line_position rightmost(void) const { return e->rightmost(); }
};

//-----------------------------------------------------------------------------
/// class for postfix unary expressions
class postfix_expr : public unary_expr {
public:
	postfix_expr(node* n, node* op) : unary_expr(n,op) { }
virtual	~postfix_expr() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const { return e->leftmost(); }
virtual	line_position rightmost(void) const { return op->rightmost(); }
};

//-----------------------------------------------------------------------------
/// class for member (of user-defined type) expressions
class member_expr : public postfix_expr {
protected:
	expr*		member;		// should be an id_expr or token_string
public:
	member_expr(node* l, node* op, node* m) : postfix_expr(l,op), 
		member(dynamic_cast<expr*>(m)) { assert(member); }
virtual	~member_expr() { SAFEDELETE(member); }

virtual	ostream& what(ostream& o) const { return o << "(member-expr)"; }
virtual	line_position rightmost(void) const { return member->rightmost(); }
};

//-----------------------------------------------------------------------------
/// class for array indexing, with support for multiple dimensions and ranges
class index_expr : public postfix_expr {
protected:
	range_list*		ranges;		///< index
public:
	index_expr(node* l, node* i) : postfix_expr(l, NULL), 
		ranges(dynamic_cast<range_list*>(i)) { }
virtual	~index_expr() { SAFEDELETE(ranges); }

virtual	ostream& what(ostream& o) const { return o << "(index-expr)"; }
virtual	line_position rightmost(void) const { return ranges->rightmost(); }
};

//=============================================================================
/// base class for general binary expressions
class binary_expr : public expr, public nonterminal {
protected:
	expr* 		l;			///< left-hand side
	terminal*	op;			///< operator
	expr*		r;			///< right-hand side
public:
	binary_expr(node* left, node* o, node* right) : expr(), 
		l(dynamic_cast<expr*>(left)), op(dynamic_cast<terminal*>(o)), 
		r(dynamic_cast<expr*>(right))
		{ assert(l); assert(op); assert(r); }
virtual	~binary_expr() { SAFEDELETE(l); SAFEDELETE(op); SAFEDELETE(r); }

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const { return l->leftmost(); }
virtual	line_position rightmost(void) const { return r->rightmost(); }
};

//-----------------------------------------------------------------------------
/// class of arithmetic expressions
class arith_expr : public binary_expr {
public:
	arith_expr(node* left, node* o, node* right) : 
		binary_expr(left, o, right) { }
virtual	~arith_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(arith-expr)"; }
};

//-----------------------------------------------------------------------------
/// class of relational expressions
class relational_expr : public binary_expr {
public:
	relational_expr(node* left, node* o, node* right) : 
		binary_expr(left, o, right) { }
virtual	~relational_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(relational-expr)"; }
};

//-----------------------------------------------------------------------------
/// class of logical expressions
class logical_expr : public binary_expr {
public:
	logical_expr(node* left, node* o, node* right) : 
		binary_expr(left, o, right) { }
virtual	~logical_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(logical-expr)"; }
};

//=============================================================================
/// abstract base class for type
class type_base : virtual public node {
public:
	type_base() : node() { }
virtual	~type_base() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

//-----------------------------------------------------------------------------
/// keywords that are also types
class token_type : public token_keyword, public type_base {
public:
	token_type(const char* tf) : token_keyword(tf), type_base() { }
//		{ assert(!strcmp(tf,"true") || !strcmp(tf,"false")); }
virtual	~token_type() { }

virtual	ostream& what(ostream& o) const
		{ return o << "type: " << *((const string*) this); }
virtual	line_position leftmost(void) const
		{ return token_keyword::leftmost(); }
virtual	line_position rightmost(void) const
		{ return token_keyword::rightmost(); }
};

//-----------------------------------------------------------------------------
/// type identifier with optional template parameters
class type_id : public type_base {
protected:
	node*			base;		///< base type's name
	expr_list*		temp_spec;	///< template arguments
public:
	type_id(node* b, node* t) : type_base(), 
		base(b), 	// can't restrict type yet...
			// may be id_expr, or chan_type, or data_type
		temp_spec(dynamic_cast<expr_list*>(t))	// may be NULL
		{ }
virtual	~type_id() { SAFEDELETE(base); SAFEDELETE(temp_spec); }

virtual	ostream& what(ostream& o) const { 
		base->what(o << "(type-id): ");
		if (temp_spec) temp_spec->what(o);
		return o;
	}
virtual	line_position leftmost(void) const { return base->leftmost(); }
virtual	line_position rightmost(void) const { return temp_spec->rightmost(); }
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
	data_type_base(node* t, node* l, node* w, node* r) : type_base(), 
		type(dynamic_cast<token_keyword*>(t)), 
		la(dynamic_cast<token_char*>(l)), 
		width(dynamic_cast<token_int*>(w)), 
		ra(dynamic_cast<token_char*>(r))
		{ assert(type); assert(la); assert(width); assert(ra); }
	data_type_base(node* t) : type_base(), 
		type(dynamic_cast<token_keyword*>(t)), 
		la(NULL), width(NULL), ra(NULL) { assert(type); }
virtual	~data_type_base() { SAFEDELETE(type); SAFEDELETE(la);
		SAFEDELETE(width); SAFEDELETE(ra); }

virtual	ostream& what(ostream& o) const { return o << "(data-type-base)"; }
virtual	line_position leftmost(void) const { return type->leftmost(); }
virtual	line_position rightmost(void) const {
	if (ra)		return ra->rightmost();
	else if (width)	return width->rightmost();
	else if (la)	return la->rightmost();
	else		return type->rightmost();
}
};

/// list of base data types
typedef node_list<data_type_base,comma>	base_data_type_list;

// construction macros
#define base_data_type_list_wrap(b,l,e)					\
	dynamic_cast<base_data_type_list*>(l)->wrap(b,e)
#define base_data_type_list_append(l,d,n)				\
        dynamic_cast<base_data_type_list*>(l)->append(d,n)

//-----------------------------------------------------------------------------
/// full channel type, including base type list
class chan_type : public type_base {
protected:
//	chan_type_root*		base;		///< channel/port base type
	token_keyword*		chan;		///< keyword "channel"
	token_char*		dir;		///< port direction: in or out
	base_data_type_list*	dtypes;		///< data types communicated
public:
	chan_type(node* c, node* d = NULL, node* t = NULL) : type_base(), 
//		base(dynamic_cast<chan_type_root*>(b)), 
		chan(dynamic_cast<token_keyword*>(c)), 
		dir(dynamic_cast<token_char*>(d)), 
		dtypes(dynamic_cast<base_data_type_list*>(t))
		{ assert(c); if(d) assert(dir); if (t) assert(dtypes); }
virtual	~chan_type() { SAFEDELETE(chan); SAFEDELETE(dir); SAFEDELETE(dtypes); }

chan_type* attach_data_types(node* t);

virtual	ostream& what(ostream& o) const { return o << "(chan-type)"; }
virtual	line_position leftmost(void) const { return chan->leftmost(); }
virtual	line_position rightmost(void) const { return dtypes->rightmost(); }
};

#define	chan_type_attach_data_types(ct,t)				\
	dynamic_cast<chan_type*>(ct)->attach_data_types(t)

//-----------------------------------------------------------------------------
/// user-defined data type
class data_type : public type_base {
protected:
	token_keyword*		def;		///< "deftype" keyword
	token_identifier*	name;		///< name of new type
//	...
public:

};

//=============================================================================
/**
	Base class for statements (assignments, increment, decrement...)
	Assertion: all statements are nonterminals
 */
class statement : public nonterminal {
public:
	statement() : nonterminal() { }
virtual	~statement() { }

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
	incdec_stmt(node* n, node* o) : statement(), 
		e(dynamic_cast<expr*>(n)), 
		op(dynamic_cast<terminal*>(o))
		{ assert(e); assert(op); }
virtual	~incdec_stmt() { SAFEDELETE(e); SAFEDELETE(op); }

/**
	Release operations are needed for destructive transfer of ownership.  
	The consumers of the return pointers are thus responsible for the 
	memory at their location.  
 */
virtual	expr* release_expr(void) { expr* r = e; e = NULL; return r; }
virtual	terminal* release_op(void) { terminal* r = op; op = NULL; return r; }

virtual	ostream& what(ostream& o) const { return o << "(inc/dec-stmt)"; }
virtual	line_position leftmost(void) const { return e->leftmost(); }
virtual	line_position rightmost(void) const { return op->rightmost(); }
};

//-----------------------------------------------------------------------------
/// class for binary expression statements, with left- and right-hand sides
class assign_stmt : public statement {
protected:
	expr*		lhs;			///< destination
	terminal*	op;			///< operation
	expr*		rhs;			///< source expression
public:
	assign_stmt(node* left, node* o, node* right) : statement(), 
		lhs(dynamic_cast<expr*>(left)), 
		op(dynamic_cast<terminal*>(o)), 
		rhs(dynamic_cast<expr*>(right))
		{ assert(lhs); assert(op); assert(rhs); }
virtual	~assign_stmt() { SAFEDELETE(lhs); SAFEDELETE(op); SAFEDELETE(rhs); }

virtual	ostream& what(ostream& o) const { return o << "(assign-stmt)"; }
virtual	line_position leftmost(void) const { return lhs->leftmost(); }
virtual	line_position rightmost(void) const { return rhs->rightmost(); }
};

//=============================================================================
/// abstract base class for items that may be found in a definition body
class def_body_item : public nonterminal {
public:
	def_body_item() : nonterminal() { }
virtual	~def_body_item() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};

/// definition body is just a list of definition items
typedef	node_list<def_body_item>	definition_body;

#define definition_body_wrap(b,l,e)					\
	dynamic_cast<definition_body*>(l)->wrap(b,e)
#define definition_body_append(l,d,n)					\
	dynamic_cast<definition_body*>(l)->append(d,n) 

//=============================================================================
/// abstract base class for language bodies
class language_body : public def_body_item {
protected:
	token_keyword*	tag;			///< what language
public:
	language_body(node* t) : def_body_item(), 
		tag(dynamic_cast<token_keyword*>(t)) { if (t) assert(tag); }
virtual	~language_body() { SAFEDELETE(tag); }

virtual language_body* attach_tag(node* t) {
		tag = dynamic_cast<token_keyword*>(t);
		assert(tag);
		return this;
	}

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const { return tag->leftmost(); }
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
virtual	line_position leftmost(void) const { return ns->leftmost(); }
virtual	line_position rightmost(void) const { return semi->rightmost(); }

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

virtual	ostream& what(ostream& o) const { return o << "(using-namespace)"; }
virtual	line_position leftmost(void) const { return open->leftmost(); }
virtual	line_position rightmost(void) const { return semi->rightmost(); }

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
 */
	expr*		id;
public:
	declaration_base(node* i) : def_body_item(), root_item(), 
		id(dynamic_cast<expr*>(i)) { assert(id); }
virtual	~declaration_base() { SAFEDELETE(id); }

virtual	ostream& what(ostream& o) const 
		{ return id->what(o << "(declaration-id): "); }
virtual	line_position leftmost(void) const { return id->leftmost(); }
virtual	line_position rightmost(void) const { return id->rightmost(); }
virtual	line_range where(void) const { return node::where(); }
};

typedef	node_list<declaration_base,comma>	declaration_id_list;

#define declaration_id_list_wrap(b,l,e)					\
	dynamic_cast<declaration_id_list*>(l)->wrap(b,e)
#define declaration_id_list_append(l,d,n)				\
	dynamic_cast<declaration_id_list*>(l)->append(d,n) 

//-----------------------------------------------------------------------------
/// declaration identifier with ranges
class declaration_array : public declaration_base {
protected:
	range_list*		ranges;		///< optional ranges
public:
	declaration_array(node* i, node* rl) : declaration_base(i), 
		ranges(dynamic_cast<range_list*>(rl)) { }
		// ranges may be NULL, equivalent to declaration base
virtual	~declaration_array() { SAFEDELETE(ranges); }

virtual	ostream& what(ostream& o) const 
		{ return ranges->what(id->what(o << "(declaration-array): ")); }
virtual	line_position rightmost(void) const { return ranges->rightmost(); }
};

//=============================================================================
/// abstract base class of instance items
class instance_base : virtual public def_body_item, virtual public root_item {
public:
	instance_base() : def_body_item(), root_item() { }
virtual	~instance_base() { }

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
virtual	line_range where(void) const { return node::where(); }
};

//-----------------------------------------------------------------------------
/// class for an instance declaration
class instance_declaration : public instance_base {
protected:
	type_base*		type;
	declaration_id_list*	ids;
	terminal*		semi;
public:
	instance_declaration(node* t, node* i, node* s = NULL) : 
		instance_base(), 
		type(dynamic_cast<type_base*>(t)), 
		ids(dynamic_cast<declaration_id_list*>(i)), 
		semi(dynamic_cast<terminal*>(s)) 
		{ assert(type); assert(ids); if(s) assert(semi); }
virtual	~instance_declaration() { SAFEDELETE(type); SAFEDELETE (ids);
		SAFEDELETE(semi); }

virtual	ostream& what(ostream& o) const { return o << "(instance-decl)"; }
virtual	line_position leftmost(void) const { return type->leftmost(); }
virtual	line_position rightmost(void) const { return semi->rightmost(); }
};

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// clever re-use of declaration classes
typedef	node_list<instance_declaration,semicolon>	data_param_list;

#define data_param_list_wrap(b,l,e)					\
	dynamic_cast<data_param_list*>(l)->wrap(b,e)
#define data_param_list_append(l,d,n)					\
	dynamic_cast<data_param_list*>(l)->append(d,n) 

//-----------------------------------------------------------------------------
/// class for an or instance port connection or declaration connection
class actuals_connection : public instance_base, public declaration_base {
protected:
//	expr*			id;		// inherited
	expr_list*		actuals;	///< connection actuals
	terminal*		semi;		///< semicolon (optional)
public:
	actuals_connection(node* i, node* a, node* s = NULL) : 
		instance_base(), declaration_base(i), 
		actuals(dynamic_cast<expr_list*>(a)), 
		semi(dynamic_cast<terminal*>(s)) 
		{ assert(actuals); if (s) assert(semi); }
virtual	~actuals_connection()
		{ SAFEDELETE(actuals); SAFEDELETE(semi); }

// remember to check for declaration context when checking id

virtual	ostream& what(ostream& o) const { return o << "(actuals-connection)"; }
virtual	line_position leftmost(void) const
		{ return declaration_base::leftmost(); }
virtual	line_position rightmost(void) const { return semi->rightmost(); }
virtual	line_range where(void) const { return node::where(); }
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
	alias_assign(node* i, node* o, node* r, node* s = NULL) :
		instance_base(), declaration_base(i), 
		op(dynamic_cast<terminal*>(o)), 
		rhs(dynamic_cast<expr*>(r)), 
		semi(dynamic_cast<terminal*>(s)) 
		{ assert(op); assert(rhs); if (s) assert(semi); }
virtual	~alias_assign() { SAFEDELETE(op); SAFEDELETE(rhs); SAFEDELETE(semi); }

// type check here

virtual	ostream& what(ostream& o) const { return o << "(alias-assign)"; }
virtual	line_position leftmost(void) const
		{ return declaration_base::leftmost(); }
virtual	line_position rightmost(void) const { return semi->rightmost(); }
virtual	line_range where(void) const { return node::where(); }
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
	loop_instantiation(node* l, node* d, node* i, node* c, node* g, 
		node* b, node* r) : instance_base(), 
		lp(dynamic_cast<terminal*>(l)), 
		delim(dynamic_cast<terminal*>(d)), 
		index(dynamic_cast<token_identifier*>(i)), 
		colon(dynamic_cast<terminal*>(c)), 
		rng(dynamic_cast<range*>(g)), 
		body(dynamic_cast<definition_body*>(b)), 
		rp(dynamic_cast<terminal*>(r)) {
			assert(lp); assert(delim); assert(index);
			assert(colon); assert(rng); assert(body); assert(lp);
	}
virtual	~loop_instantiation() {
		SAFEDELETE(lp); SAFEDELETE(delim); SAFEDELETE(index);
		SAFEDELETE(colon); SAFEDELETE(rng); SAFEDELETE(body);
		SAFEDELETE(rp);
	}

virtual	ostream& what(ostream& o) const { return o << "(loop-instance)"; }
virtual	line_position leftmost(void) const { return lp->leftmost(); }
virtual	line_position rightmost(void) const { return rp->rightmost(); }
};

//=============================================================================
/// single port formal identifier, with optional dimension array specification
class port_formal_id : public nonterminal {
protected:
	token_identifier*	name;		///< formal name
	range_list*		dim;		///< optional dimensions
public:
	port_formal_id(node* n, node* d) : nonterminal(), 
		name(dynamic_cast<token_identifier*>(n)), 
		dim(dynamic_cast<range_list*>(d))
		{ assert(name); }		// dim may be NULL
virtual	~port_formal_id() { SAFEDELETE(name); SAFEDELETE(dim); }

virtual	ostream& what(ostream& o) const { 
		name->what(o << "(port-formal-id): "); 
		if (dim) dim->what(o);
		return o;
		}
virtual	line_position leftmost(void) const { return name->leftmost(); }
virtual	line_position rightmost(void) const { return dim->rightmost(); }
};

/// list of port-formal identifiers (optional arrays)
typedef	node_list<port_formal_id,comma>	port_formal_id_list;

#define port_formal_id_list_wrap(b,l,e)					\
	dynamic_cast<port_formal_id_list*>(l)->wrap(b,e)
#define port_formal_id_list_append(l,d,n)				\
	dynamic_cast<port_formal_id_list*>(l)->append(d,n) 

//-----------------------------------------------------------------------------
/// port formal declaration contains a type and identifier list
class port_formal_decl : public nonterminal {
protected:
	type_id*		type;		///< formal base type
	port_formal_id_list*	ids;		///< identifier list
public:
	port_formal_decl(node* t, node* i) : nonterminal(), 
		type(dynamic_cast<type_id*>(t)), 
		ids(dynamic_cast<port_formal_id_list*>(i)) {
			assert(type); assert(ids);
		}
virtual	~port_formal_decl() { SAFEDELETE(type); SAFEDELETE(ids); }

virtual	ostream& what(ostream& o) const { return o << "(port-formal-decl)"; }
virtual	line_position leftmost(void) const { return type->leftmost(); }
virtual	line_position rightmost(void) const { return ids->rightmost(); }
};

/// list of port-formal declarations
typedef	node_list<port_formal_decl,semicolon>	port_formal_decl_list;

#define port_formal_decl_list_wrap(b,l,e)				\
	dynamic_cast<port_formal_decl_list*>(l)->wrap(b,e)
#define port_formal_decl_list_append(l,d,n)				\
	dynamic_cast<port_formal_decl_list*>(l)->append(d,n) 

//=============================================================================
/// single template formal identifier, with optional dimension array spec.  
class template_formal_id : public nonterminal {
protected:
	token_identifier*	name;		///< formal name
	range_list*		dim;		///< optional dimensions
public:
	template_formal_id(node* n, node* d) : nonterminal(), 
		name(dynamic_cast<token_identifier*>(n)), 
		dim(dynamic_cast<range_list*>(d))
		{ assert(name); }		// dim may be NULL
virtual	~template_formal_id() { SAFEDELETE(name); SAFEDELETE(dim); }

virtual	ostream& what(ostream& o) const { 
		name->what(o << "(template-formal-id): "); 
		if (dim) dim->what(o);
		return o;
		}
virtual	line_position leftmost(void) const { return name->leftmost(); }
virtual	line_position rightmost(void) const { return dim->rightmost(); }
};

/// list of template-formal identifiers (optional arrays)
typedef	node_list<template_formal_id,comma>	template_formal_id_list;

#define template_formal_id_list_wrap(b,l,e)				\
	dynamic_cast<template_formal_id_list*>(l)->wrap(b,e)
#define template_formal_id_list_append(l,d,n)				\
	dynamic_cast<template_formal_id_list*>(l)->append(d,n) 

//-----------------------------------------------------------------------------
/// template formal declaration contains a type and identifier list
class template_formal_decl : public nonterminal {
protected:
	type_id*			type;	///< formal base type
	template_formal_id_list*	ids;	///< identifier list
public:
	template_formal_decl(node* t, node* i) : nonterminal(), 
		type(dynamic_cast<type_id*>(t)), 
		ids(dynamic_cast<template_formal_id_list*>(i)) {
			assert(type); assert(ids);
		}
virtual	~template_formal_decl() { SAFEDELETE(type); SAFEDELETE(ids); }

virtual	ostream& what(ostream& o) const
		{ return o << "(template-formal-decl)"; }
virtual	line_position leftmost(void) const { return type->leftmost(); }
virtual	line_position rightmost(void) const { return ids->rightmost(); }
};

/// list of template-formal declarations
typedef	node_list<template_formal_decl,semicolon> template_formal_decl_list;

#define template_formal_decl_list_wrap(b,l,e)				\
	dynamic_cast<template_formal_decl_list*>(l)->wrap(b,e)
#define template_formal_decl_list_append(l,d,n)				\
	dynamic_cast<template_formal_decl_list*>(l)->append(d,n) 

//=============================================================================
/// definition type identifier, with optional template specifier list
class def_type_id : public type_base {
protected:
	token_identifier*		name;	///< definition name base
	/// optional template specifier
	template_formal_decl_list*	temp_spec;
public:
	def_type_id(node* n, node* t) : type_base(), 
		name(dynamic_cast<token_identifier*>(n)), 
		temp_spec(dynamic_cast<template_formal_decl_list*>(t))
		{ assert(name); if (t) assert(temp_spec); }
virtual	~def_type_id() { SAFEDELETE(name); SAFEDELETE(temp_spec); }

virtual	ostream& what(ostream& o) const { return o << "(def-type-id)"; }
virtual	line_position leftmost(void) const { return name->leftmost(); }
virtual	line_position rightmost(void) const {
		if (temp_spec) return temp_spec->rightmost();
		else return name->rightmost();
	}
};

//=============================================================================
/// abstract base class for definitions
class definition : public root_item {
protected:
	definition_body*		body;	///< definition body
public:
	definition(node* b) : root_item(), 
		body(dynamic_cast<definition_body*>(b))
		{ assert(body); }
virtual	~definition() { SAFEDELETE(body); }

virtual	ostream& what(ostream& o) const { return o << "(definition)"; }
virtual	line_position leftmost(void) const { return body->leftmost(); }
virtual	line_position rightmost(void) const { return body->rightmost(); }
};

//-----------------------------------------------------------------------------
/// process definition
class process_def : public definition {
protected:
	token_keyword*			def;	///< definition keyword
	def_type_id*			idt;	///< identifier [template]
	port_formal_decl_list*		ports;	///< optional port formal list
public:
	process_def(node* d, node* i, node* p, node* b) : definition(b), 
		def(dynamic_cast<token_keyword*>(d)), 
		idt(dynamic_cast<def_type_id*>(i)), 
		ports(dynamic_cast<port_formal_decl_list*>(p)) {
			assert(def); assert(idt); assert(ports);
		}

virtual	ostream& what(ostream& o) const { return o << "(process-definition)"; }
virtual	line_position leftmost(void) const { return def->leftmost(); }
};

//=============================================================================
/// conditional instantiations in definition body
class guarded_definition_body : public instance_base {
protected:
	expr*				guard;	///< condition expression
	terminal*			arrow;	///< right arrow
	definition_body*		body;
public:
	guarded_definition_body(node* e, node* a, node* b) : instance_base(), 
		guard(dynamic_cast<expr*>(e)), 
		arrow(dynamic_cast<terminal*>(a)), 
		body(dynamic_cast<definition_body*>(b))
		{ assert(guard); assert(arrow); assert(body); }
virtual	~guarded_definition_body() { SAFEDELETE(guard);
		SAFEDELETE(arrow); SAFEDELETE(body); }

virtual	ostream& what(ostream& o) const { return o << "(guarded-def-body)"; }
virtual	line_position leftmost(void) const { return guard->leftmost(); }
virtual	line_position rightmost(void) const { return body->rightmost(); }
};

/// list of template-formal declarations
typedef	node_list<guarded_definition_body,thickbar>
		guarded_definition_body_list;

#define guarded_definition_body_list_wrap(b,l,e)			\
	dynamic_cast<guarded_definition_body_list*>(l)->wrap(b,e)
#define guarded_definition_body_list_append(l,d,n)			\
	dynamic_cast<guarded_definition_body_list*>(l)->append(d,n) 

//-----------------------------------------------------------------------------
/// wrapper class for conditional instantiations
class conditional_instantiation : public instance_base {
protected:
	guarded_definition_body_list*	gd;
public:
	conditional_instantiation(node* n) : instance_base(), 
		gd(dynamic_cast<guarded_definition_body_list*>(n))
		{ assert(gd); }

virtual	ostream& what(ostream& o) const
	{ return o << "(conditional-instance)"; }
virtual	line_position leftmost(void) const { return gd->leftmost(); }
virtual	line_position rightmost(void) const { return gd->rightmost(); }
};

//=============================================================================




};	// end namespace parser
};	// end namespace ART

#endif	// __ART_PARSER_H__

