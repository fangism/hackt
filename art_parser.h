//-----------------------------------------------------------------------------
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
class token_char;
class token_string;

//=============================================================================
// some constant delimiter strings, defined in art_parser.cc
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
// virtual	ostream& where(ostream& o) const = 0;
// virtual	ostream& left_most(ostream& o) const = 0;
// virtual	ostream& right_most(ostream& o) const = 0;

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
private:
	typedef		list_of_ptr<node>		list_parent;
public:
	typedef		list_parent::iterator		iterator;
	typedef		list_parent::const_iterator	const_iterator;
public:
	node_list_base() : nonterminal(), list_of_ptr<node>() { }
// initializing with first element, T must be subclass of node!
	node_list_base(node* n) : node_list_base() 
		{ assert(dynamic_cast<T*>(n)); push_back(n); }
virtual	~node_list_base() { }

using	list_parent::begin;
using	list_parent::end;

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
private:
	typedef		node_list_base<T>	parent;
public:
	typedef	typename parent::iterator	iterator;
	typedef	typename parent::const_iterator	const_iterator;

protected:
	terminal*	open;		///< wrapping string, such as "("
	terminal*	close;		///< wrapping string, such as ")"
public:
	node_list() : node_list_base<T>() { open = close = NULL; }
	node_list(node* n) : node_list_base<T>() {
			if (n) assert(dynamic_cast<T*>(n));
			push_back(n);
		}
virtual	~node_list() { if (open) delete open; if (close) delete close; }

using	parent::begin;
using	parent::end;

/**
	This attaches enclosing text, such as brackets, braces, parentheses, 
	around a node_list.  The arguments are type-checked as token_strings.  
 */
virtual	node_list<T,D>* wrap(node* b, node* e) {
		open = dynamic_cast<terminal*>(b);
		assert(open);
		assert(dynamic_cast<token_char*>(open) || 
			dynamic_cast<token_string*>(open));
		close = dynamic_cast<terminal*>(e);
		assert(close);
		assert(dynamic_cast<token_char*>(close) || 
			dynamic_cast<token_string*>(close));
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

/// prints out type of first element in list, if not null
// later, use static functions (operator <<) to determine type name...
virtual	ostream& what(ostream& o) const {
		// print first item to get type
		const_iterator i = begin();
		o << "(node_list<>): ";
		if (*i) (*i)->what(o) << " ";
		return o << "...";
	}
};

//=============================================================================
/// abstract base class for general expressions
class expr : public nonterminal {
public:
	expr() : nonterminal() { }
virtual	~expr() { }

virtual	ostream& what(ostream& o) const { return o << "(expr)"; }
};

///	all expression lists are comma-separated
typedef node_list<expr,comma>	expr_list;

#define expr_list_wrap(b,l,e)						\
	dynamic_cast<expr_list*>(l)->wrap(b,e)
#define expr_list_append(l,d,n)						\
	dynamic_cast<expr_list*>(l)->append(d,n) 


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

/// generalized scoped identifier expression
typedef node_list<token_identifier,scope>	id_expr;

#define id_expr_append(l,d,n)						\
	dynamic_cast<id_expr*>(l)->append(d,n)

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

/// all range lists are comma-separated
typedef node_list<range,comma>	range_list;

#define range_list_wrap(b,l,e)						\
	dynamic_cast<range_list*>(l)->wrap(b,e)
#define range_list_append(l,d,n)					\
	dynamic_cast<range_list*>(l)->append(d,n)


//=============================================================================
/// abstract base class for unary expressions
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
/// base class for general binary expressions
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

// virtual	ostream& what(ostream& o) const = 0;
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
class type_base : public nonterminal {
public:
	type_base() : nonterminal() { }
virtual	~type_base() { }

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
virtual	~type_id() { if (base) delete base; if (temp_spec) delete temp_spec; }

virtual	ostream& what(ostream& o) const { 
		base->what(o << "(type-id): ");
		if (temp_spec) temp_spec->what(o);
		return o;
	}
};

//-----------------------------------------------------------------------------
/// base class for channel type
class chan_type_root : public type_base {
protected:
	token_keyword*		chan;		///< keyword "channel"
	token_char*		dir;		///< port direction: in or out
public:
	chan_type_root(node* c, node* d = NULL) : type_base(), 
		chan(dynamic_cast<token_keyword*>(c)), 
		dir(dynamic_cast<token_char*>(d))
		{ assert(chan); }		// dir may be null
virtual	~chan_type_root() { if (chan) delete chan; if (dir) delete dir; }

virtual	ostream& what(ostream& o) const { return o << "(chan/port-base)"; }
};

//-----------------------------------------------------------------------------
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
virtual	~data_type_base() { if (type) delete type; if (la) delete la;
		if (width) delete width; if (ra) delete ra; }

virtual	ostream& what(ostream& o) const { return o << "(data-type-base)"; }
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
	chan_type_root*		base;		///< channel/port base type
	base_data_type_list*	dtypes;		///< data types communicated
public:
	chan_type(node* b, node* dt) : type_base(), 
		base(dynamic_cast<chan_type_root*>(b)), 
		dtypes(dynamic_cast<base_data_type_list*>(dt))
		{ assert(base); assert(dtypes); }
virtual	~chan_type() { if (base) delete base; if (dtypes) delete dtypes; }

virtual	ostream& what(ostream& o) const { return o << "(chan-type)"; }
};

//-----------------------------------------------------------------------------
/// user-defined data type
class data_type : public type_base {

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
	expr*		lhs;			///< destination
	terminal*	op;			///< operation
	expr*		rhs;			///< source expression
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
/// abstract base class for items that may be found in a definition body
class def_body_item : public nonterminal {
public:
	def_body_item() : nonterminal() { }
virtual	~def_body_item() { }

virtual	ostream& what(ostream& o) const = 0;
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
virtual	~language_body() { if (tag) delete tag; }

virtual language_body* attach_tag(node* t) {
		tag = dynamic_cast<token_keyword*>(t);
		assert(tag);
		return this;
	}

virtual	ostream& what(ostream& o) const { return o << "(language-body)"; }
};

//=============================================================================
/// namespace enclosed body
class namespace_body : public statement {
protected:
	token_keyword*		ns;		///< keyword "namespace"
	token_identifier*	name;		///< name of namespace
	terminal*		lb;
	node*			body;		///< contents of namespace
	terminal*		rb;
	terminal*		semi;		///< semicolon token
public:
	namespace_body(node* s, node* n, node* l, node* b, node* r, node* c) : 
		statement(), 
		ns(dynamic_cast<token_keyword*>(s)), 
		name(dynamic_cast<token_identifier*>(n)), 
		lb(dynamic_cast<terminal*>(l)), 
		body(dynamic_cast<token_identifier*>(b)), 
		rb(dynamic_cast<terminal*>(r)), 
		semi(dynamic_cast<terminal*>(c)) {
			assert(ns); assert(name); assert(lb);
			// body may be NULL
			assert(rb); assert(semi);
		}
virtual	~namespace_body() { if (name) delete name; if (lb) delete lb;
		if (body) delete body; if (rb) delete rb; }

virtual	ostream& what(ostream& o) const { return o << "(namespace-body)"; }
};

//-----------------------------------------------------------------------------
/// command to search namespace for identifiers
class using_namespace : public statement {
protected:
	token_keyword*		open;
	id_expr*		id;
	token_keyword*		as;
	token_identifier*	alias;
	token_string*		semi;
public:
	using_namespace(node* o, node* i, node* a, node* n, node* s) : 
		statement(), 
		open(dynamic_cast<token_keyword*>(o)),
		id(dynamic_cast<id_expr*>(i)),
		as(dynamic_cast<token_keyword*>(a)),		// optional
		alias(dynamic_cast<token_identifier*>(n)), 	// optional
		semi(dynamic_cast<token_string*>(s)) {
			assert(open); assert(id);
			if (a && !as) delete a;
			if (n && !alias) delete n;
		}
virtual	~using_namespace() { if (open) delete open; if (id) delete id;
		if (as) delete as; if (alias) delete alias; }

virtual	ostream& what(ostream& o) const { return o << "(using-namespace)"; }
};

//=============================================================================
/// abstract base class of instance items
class instance_item : public def_body_item {

};

//-----------------------------------------------------------------------------
/// class for an instance declaration
class instance_declaration : virtual public instance_item {
	// and is a node_list<declaration_id>
protected:
	type_id*		type;
public:

};

//-----------------------------------------------------------------------------
/// class for an instance connection
class instance_connection : virtual public instance_item {

};

//-----------------------------------------------------------------------------
class instance_actuals : public instance_connection {

};

//-----------------------------------------------------------------------------
class instance_assign : public instance_connection {

};

//-----------------------------------------------------------------------------
class instance_decl_connection : public instance_declaration, 
		public instance_connection {

};

//-----------------------------------------------------------------------------
class loop_instantiation : public instance_item {

};

//-----------------------------------------------------------------------------
class conditional_instantiation : public instance_item {

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
virtual	~port_formal_id() { if (name) delete name; if (dim) delete dim; }

virtual	ostream& what(ostream& o) const { 
		name->what(o << "(port-formal-id): "); 
		if (dim) dim->what(o);
		return o;
		}
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
virtual	~port_formal_decl() { if (type) delete type; if (ids) delete ids; }

virtual	ostream& what(ostream& o) const { return o << "(port-formal-decl)"; }
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
virtual	~template_formal_id() { if (name) delete name; if (dim) delete dim; }

virtual	ostream& what(ostream& o) const { 
		name->what(o << "(template-formal-id): "); 
		if (dim) dim->what(o);
		return o;
		}
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
virtual	~template_formal_decl() { if (type) delete type; if (ids) delete ids; }

virtual	ostream& what(ostream& o) const
		{ return o << "(template-formal-decl)"; }
};

/// list of template-formal declarations
typedef	node_list<template_formal_decl,semicolon> template_formal_decl_list;

#define template_formal_decl_list_wrap(b,l,e)				\
	dynamic_cast<template_formal_decl_list*>(l)->wrap(b,e)
#define template_formal_decl_list_append(l,d,n)				\
	dynamic_cast<template_formal_decl_list*>(l)->append(d,n) 

//=============================================================================
/// definition type identifier, with optional template specifier list
class def_type_id : public nonterminal {
protected:
	token_identifier*		name;	///< definition name base
	/// optional template specifier
	template_formal_decl_list*	temp_spec;
public:
	def_type_id(node* n, node* t) : nonterminal(), 
		name(dynamic_cast<token_identifier*>(n)), 
		temp_spec(dynamic_cast<template_formal_decl_list*>(t))
		{ assert(name); if (t) assert(temp_spec); }
virtual	~def_type_id() { if (name) delete name;
		if (temp_spec) delete temp_spec; }

virtual	ostream& what(ostream& o) const { return o << "(def-type-id)"; }
};

//=============================================================================
/// abstract base class for definitions
class definition : public statement {
protected:
	definition_body*		body;	///< definition body
public:
	definition(node* b) : statement(), 
		body(dynamic_cast<definition_body*>(b))
		{ assert(body); }
virtual	~definition() { if (body) delete body; }

virtual	ostream& what(ostream& o) const { return o << "(definition)"; }
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
};

//=============================================================================


//=============================================================================
};	// end namespace parser
};	// end namespace ART

#endif	// __ART_PARSER_H__

