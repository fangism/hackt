// "art_parser.h"
// parser classes for ART only!

#ifndef __ART_PARSER_H__
#define __ART_PARSER_H__

// probably stick to lists, no hash_map's until the build() phase
// if this file gets too honkin' big, I'll split it up...

#include <iostream>
#include <string>

#include <stdio.h>		// just for sprintf
#include <string.h>		// for a few C-string functions

#ifdef	__APPLE__		// different location with gcc-3.3 on OS X
#include <ext/hash_map>
using namespace __gnu_cxx;
#else
#include <hash_map>
#endif

#include "art_switches.h"
#include "list_of_ptr.h"	// includes <list>

using namespace std;

/// This is the general namespace for all ART-related classes.  
namespace ART {

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
// forward declarations

//=============================================================================
// some constant delimiter strings
extern	const char	none[];
extern	const char	comma[];
extern	const char	semicolon[];
extern	const char	scope[];
extern	const char	thickbar[];
extern	const char	colon[];

//=============================================================================
/// the abstract base class for parser nodes, universal return type
/**
	Rather than have to rely on unions in the lexer and parser, 
	we prefer to use a single type of node for manipulation.  
	This will be useful when we wish to dump the parser's value stack
	in error reporting and debugging.  
 */
class node {
public:
///	standard virtual destructor
virtual	~node() { }

/**
	Shows representation without recursive descent.  
	Derived classes of non-terminals should just print their type name.  
 */
virtual	ostream& what(ostream& o) const { return o << "(node)"; }

// will type-check and return a usable ART::object
// virtual object* check_build(const context* c) const = 0;
};

//=============================================================================
/// abstract base class for terminal tokens, mainly to be used by lexer
class terminal : virtual public node {
protected:
/// the position in the file where token was found, pos.off is unused
	token_position	pos;
public:
///	base constructor always records the current position of the token
//	"current" is defined in art_switches.h
	terminal() { pos.line = current.line; pos.leng = current.leng;
		pos.col = current.col; }

///	standard virtual destructor
virtual	~terminal() { }

virtual	int string_compare(const char* d) const = 0;
virtual	ostream& what(ostream& o) const { return o << "(terminal)"; }
};

//=============================================================================
/// abstract base class for non-terminal symbols, mainly to be used by parser
class nonterminal : virtual public node {
public:
///	standard virtual destructor
virtual ~nonterminal() { }

virtual	ostream& what(ostream& o) const { return o << "(non-terminal)"; }
// virtual	ostream& where(ostream& o) const = 0;
	// includes token position information
};

//=============================================================================
/// abstract base class for general expressions
class expr : public nonterminal {
public:
	expr() : nonterminal() { }
virtual	~expr() { }

virtual	ostream& what(ostream& o) const { return o << "(expr)"; }
};

//=============================================================================
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
};

//-----------------------------------------------------------------------------
/// keyword version of token_string class, not an expr
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
};

//-----------------------------------------------------------------------------
/// class for "else" keyword, which is a legitimate expr
class token_else : public token_keyword, public expr {
public:
	token_else(const char* tf) : token_keyword(tf), expr()
		{ assert(!strcmp(tf,"true") || !strcmp(tf,"false")); }
virtual	~token_else() { }

virtual	ostream& what(ostream& o) const
		{ return o << "keyword: " << *((const string*) this); }
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
};

//=============================================================================
/**
	This is the general class for list structures of nodes in the
	syntax tree.  What is unique about this implementation is that
	its derived classes keep track of the delimiter tokens that 
	separated the lists, if applicable.  
	The specifier T, a derived class from node, is only used for 
	type-checking.  
 */
template <class T>
class node_list_base : public nonterminal, public list_of_ptr<node> {
public:
	node_list_base() : nonterminal(), list_of_ptr<node>() { }
// initializing with first element, T must be subclass of node!
	node_list_base(node* n) : node_list_base() 
		{ assert(dynamic_cast<T*>(n)); push_back(n); }
virtual	~node_list_base() { }

///< set the enclosing strings accordingly
	node_list_base<T>* append(node* n);

virtual	ostream& what(ostream& o) const { return o << "(node_list_base<>)"; }
// later, use static functions (operator <<) to determine type name...
};

//-----------------------------------------------------------------------------
/**
	The delimiter specifier, D, is used for checking that every other
	token is separated by a D character (if D is not '\0').  
 */
template <class T, const char D[] = none>
class node_list : public node_list_base<T> {
protected:
	terminal*	begin;		///< wrapping string, such as "("
	terminal*	end;		///< wrapping string, such as ")"
public:
	node_list() : node_list_base<T>() { begin = end = NULL; }
	node_list(node* n) : node_list_base<T>()
		{ assert(dynamic_cast<T*>(n)); push_back(n); }
virtual	~node_list() { if (begin) delete begin; if (end) delete end; }

/**
	This attaches enclosing text, such as brackets, braces, parentheses, 
	around a node_list.  The arguments are type-checked as token_strings.  
 */
virtual	node_list<T,D>* wrap(node* b, node* e) {
		begin = dynamic_cast<terminal*>(b);
		assert(begin);
		assert(dynamic_cast<token_char*>(begin) || 
			dynamic_cast<token_string*>(begin));
		end = dynamic_cast<terminal*>(e);
		assert(end);
		assert(dynamic_cast<token_char*>(end) || 
			dynamic_cast<token_string*>(end));
		return this;
	}

/**
	Adds an element to a node list, along with the delimiting
	character token.  Also checks that delimiter matchs, and
	that type of argument matches.
 */
virtual	node_list<T,D>* append(node* d, node* n) {
		if (d) {
			// check for delimiter character match
			terminal* t = dynamic_cast<terminal*>(d);
			assert(t);		// throw exception
			// will fail if incorrect type is passed
			assert(!(t->string_compare(D)));
			push_back(d);
		} else {
			assert(!strlen(D));	// no delimiter was expected
		}
		assert(dynamic_cast<T*>(n));	// type-check
		push_back(n);			// n may be null
		return this;
	}

};

//=============================================================================
/*
	id_expr has been typedef'd to node_list<token_string,scope>
	A generalization of identifiers to include those qualified by 
	scoped namespaces.  
class id_expr : public expr, public token_string {
protected:
	id_expr*	next;


};
 */

//=============================================================================
/**
	Can describe either a range of integers (inclusive) or a 
	single integer.  Often found in sparse or multidimensional 
	array declarations and expressions.  
 */
class range : public expr {
protected:
	expr*		lower;		///< inclusive lower bound
	terminal*	op;		///< range operator token ".."
	expr*		upper;		///< inclusive upper bound
public:
/**
	Upper bound is optional.  If omitted, range is interpreted as [l,l].  
	Failure to dynamic_cast will result in assignment to a NULL pointer, 
	which will be detected, and properly memory managed, assuming
	that the arguments exclusively "owned" their memory locations.  
 */
	range(node* l, node* o, node* u = NULL) : expr(), 
		// note: cannot static_cast from a virtual base
		lower(dynamic_cast<expr*>(l)), 
		op(dynamic_cast<terminal*>(o)),
		upper(dynamic_cast<expr*>(u)) {
			if (l && !lower) delete l;
			if (o && !op) delete o;
			if (u && !upper) delete u;
		}
virtual	~range() { if (lower) delete lower; 
		if (op) delete op; if (upper) delete upper; }

virtual	ostream& what(ostream& o) const { return o << "(range)"; }
};

//=============================================================================
class unary_expr : public expr {
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
	unary_expr(node* n, node* o) : expr(), e(dynamic_cast<expr*>(n)), 
		op(dynamic_cast<terminal*>(o)) {
			if (n && !e) delete n;	// or use assert?
			if (o && !op) delete o;
		}
virtual	~unary_expr() { if (e) delete e; if (op) delete op; }

virtual	ostream& what(ostream& o) const = 0;
};

//-----------------------------------------------------------------------------
/// class for prefix unary expressions
class prefix_expr : public unary_expr {
public:
	prefix_expr(node* op, node* n) : unary_expr(n,op) { }
virtual	~prefix_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(prefix-expr)"; }
};

//-----------------------------------------------------------------------------
/// class for postfix unary expressions
class postfix_expr : public unary_expr {
public:
	postfix_expr(node* n, node* op) : unary_expr(n,op) { }
virtual	~postfix_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(postfix-expr)"; }
};

//-----------------------------------------------------------------------------
/// class for member (of user-defined type) expressions
class member_expr : public postfix_expr {
protected:
	expr*		member;		// should be an id_expr or token_string
public:
	member_expr(node* l, node* op, node* m) : postfix_expr(l,op), 
		member(dynamic_cast<expr*>(m)) { assert(member); }
virtual	~member_expr() { if (member) delete member; }

virtual	ostream& what(ostream& o) const { return o << "(member-expr)"; }
};

//-----------------------------------------------------------------------------
class index_expr : public postfix_expr {
protected:
	expr*		ranges;			// should be range_list
public:
	index_expr(node* l, node* i) : postfix_expr(l, NULL), 
		ranges(dynamic_cast<expr*>(i)) { }
virtual	~index_expr() { if (ranges) delete ranges; }

virtual	ostream& what(ostream& o) const { return o << "(index-expr)"; }
};

//=============================================================================
class binary_expr : public expr {
protected:
	expr* 		l;			///< left-hand side
	terminal*	op;			///< operator
	expr*		r;			///< right-hand side
public:
	binary_expr(node* left, node* o, node* right) : expr(), 
		l(dynamic_cast<expr*>(left)), op(dynamic_cast<terminal*>(o)), 
		r(dynamic_cast<expr*>(right))
		{ assert(l); assert(op); assert(r); }
virtual	~binary_expr() { if (l) delete l; if (op) delete op; if (r) delete r; }

virtual	ostream& what(ostream& o) const { return o << "(binary-expr)"; }
};

//-----------------------------------------------------------------------------
class arith_expr : public binary_expr {
public:
	arith_expr(node* left, node* o, node* right) : 
		binary_expr(left, o, right) { }
virtual	~arith_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(arith-expr)"; }
};

//-----------------------------------------------------------------------------
class relational_expr : public binary_expr {
public:
	relational_expr(node* left, node* o, node* right) : 
		binary_expr(left, o, right) { }
virtual	~relational_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(relational-expr)"; }
};

//-----------------------------------------------------------------------------
class logical_expr : public binary_expr {
public:
	logical_expr(node* left, node* o, node* right) : 
		binary_expr(left, o, right) { }
virtual	~logical_expr() { }

virtual	ostream& what(ostream& o) const { return o << "(logical-expr)"; }
};

//=============================================================================
/// base class for statements (assignments, increment, decrement...)
class statement : public nonterminal {
public:
	statement() : nonterminal() { }
virtual	~statement() { }

virtual	ostream& what(ostream& o) const { return o << "(statement)"; }
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
virtual	~incdec_stmt() { if (e) delete e; if (op) delete op; }

/**
	Release operations are needed for destructive transfer of ownership.  
	The consumers of the return pointers are thus responsible for the 
	memory at their location.  
 */
virtual	expr* release_expr(void) { expr* r = e; e = NULL; return r; }
virtual	terminal* release_op(void) { terminal* r = op; op = NULL; return r; }

virtual	ostream& what(ostream& o) const { return o << "(inc/dec-stmt)"; }
};

//-----------------------------------------------------------------------------
/// class for binary expression statements, with left- and right-hand sides
class assign_stmt : virtual public statement {
protected:
	expr*		lhs;
	terminal*	op;
	expr*		rhs;
public:
	assign_stmt(node* left, node* o, node* right) : statement(), 
		lhs(dynamic_cast<expr*>(left)), 
		op(dynamic_cast<terminal*>(o)), 
		rhs(dynamic_cast<expr*>(right))
		{ assert(lhs); assert(op); assert(rhs); }
virtual	~assign_stmt() { if (lhs) delete lhs; if (op) delete op;
		if (rhs) delete rhs; }

virtual	ostream& what(ostream& o) const { return o << "(assign-stmt)"; }
};

//=============================================================================
/// abstract base class for language bodies
class language_body : public nonterminal {
public:
	language_body() : nonterminal() { }
virtual	~language_body() { }

virtual	ostream& what(ostream& o) const { return o << "(language-body)"; }
};

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#endif	// __ART_PARSER_H__

