// "art_parser.h"
// parser classes for ART only!

#ifndef __ART_PARSER_H__
#define __ART_PARSER_H__

#include <iosfwd>
#include <string>

#include "art_macros.h"
#include "art_utils.h"		// for token_position
#include "sublist.h"		// for efficient list slices
#include "ptrs.h"		// experimental pointer classes
#include "count_ptr.h"		// reference-counted pointers

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
	class token_char;	// defined here
	class token_string;	// defined here
	class qualified_id;	// defined here
	class concrete_type_ref;	// defined here
	class context;		// defined in "art_symbol_table.h"

//=============================================================================
// some constant delimiter strings, defined in art_parser.cc
// these are used as delimiter template arguments for node_list
extern	const char	none[];
extern	const char	comma[];
extern	const char	semicolon[];
extern	const char	scope[];
extern	const char	thickbar[];
extern	const char	colon[];
extern	const char	alias[];

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
	\param o the output stream.  
	\return the output stream.
 */
virtual	ostream& what(ostream& o) const = 0;

/**
	Shows the position where the node's subtree starts.  
	\return the starting position.
 */
virtual	line_position leftmost(void) const = 0;

/**
	Shows the position where the node's subtree ends.  
	\return the ending position.
 */
virtual	line_position rightmost(void) const = 0;

/**
	Shows the range of file position covered by a particular node's
	subtree.  
 */
virtual	line_range where(void) const;

/**
	Type-check and return a usable ART::entity::object, which 
	contains a hierarchical symbol table.  
	\return pointer to resulting object.  
 */
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class node

//=============================================================================
/**
	Abstract base class for terminal tokens, mainly to be used by lexer.  
 */
class terminal : virtual public node {
protected:
/// The position in the file where token was found.  (pos.off is unused)
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
};	// end class terminal

//=============================================================================
#define	NODE_LIST_BASE_TEMPLATE_SPEC					\
	template <class T>

/**
	This is the general class for list structures of nodes in the
	syntax tree.  What is unique about this implementation is that
	its derived classes keep track of the delimiter tokens that 
	separated the lists, if applicable.  
	The specifier T, a derived class from node, is only used for 
	type-checking.  
	Consider deriving from list<base_const_ptr<T> > to allow 
	copyable lists, using never_const_ptr<T>'s.  
	Then dynamically casting elements may be a pain?
 */
template <class T>
class node_list_base : virtual public node, public list<count_const_ptr<T> > {
private:
	/**
		Base class.  (was derived from excl_const_ptr, 
		then some_count_ptr)
	 */
	typedef		list<count_const_ptr<T> >	list_parent;
	// read-only, but transferrable ownership
public:
	typedef	typename list_parent::iterator		iterator;
	typedef	typename list_parent::const_iterator	const_iterator;
public:
	/**
		Default empty constructor.  
	 */
	node_list_base();

	/**
		Non-owner-transfer copy constructor.  
	 */

	node_list_base(const node_list_base<T>& l);
	/**
		Constructor with initializing first element, 
		T should be subclass of node!
	 */
	node_list_base(const T* n);

virtual	~node_list_base();

using	list_parent::begin;
using	list_parent::end;

// later, use static functions (operator <<) to determine type name...
/// Prints out type of first element in list, if not null.  
virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;

/// Releases memory to the destination list, transfering ownership
virtual	void release_append(node_list_base<T>& dest);
};	// end template class node_list_base

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
template <class T, const char D[] = none>
class node_list : public node_list_base<T> {
private:
	typedef		node_list_base<T>	parent;
public:
	typedef	typename parent::iterator	iterator;
	typedef	typename parent::const_iterator	const_iterator;
	typedef	typename parent::reverse_iterator	reverse_iterator;
	typedef	typename parent::const_reverse_iterator	const_reverse_iterator;
	typedef	list<count_const_ptr<terminal> >	delim_list;
protected:
	excl_const_ptr<terminal>	open;	///< wrapping string, e.g. "("
	excl_const_ptr<terminal>	close;	///< wrapping string, e.g. ")"
	/**
		We now keep the delimiter tokens in a separate list
		so they no longer collide with the useful elements.
		Alternative is to just keep a list of file positions, 
		since the D template parameter tells us what the delimiter 
		token is already.  
		If we really care about saving memory, can just delete
		the delimiter tokens as they are passed in.  
		If we really care about performance, just don't produce
		allocated tokens and ignore the arguments.  
	 */
	delim_list	delim;
public:
	node_list();
	node_list(const node_list<T,D>& l);
	node_list(const T* n);
virtual	~node_list();

using	parent::begin;
using	parent::end;

/**
	This attaches enclosing text, such as brackets, braces, parentheses, 
	around a node_list.  The arguments are type-checked as token_strings
	or token_chars.  
	\param b the beginning token such as open-parenthesis.  
	\param e the end token such as open-parenthesis.  
	\return this.
 */
virtual	node_list<T,D>* wrap(const terminal* b, const terminal* e);

/**
	Adds an element to a node list, along with the delimiting
	character token.  Also checks that delimiter matchs, and
	that type of argument matches.
	\param d the delimiter token, such as a comma, must match D.
	\param n the useful node.  
	\return this.
 */
virtual	node_list<T,D>* append(const terminal* d, const T* n);

// the following methods are defined in "art_parser_template_methods.h"

/// Prints out type of first element in list, if not null.  
// virtual	ostream& what(ostream& o) const;

virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

virtual	void release_append(node_list<T,D>& dest);
};	// end of template class node_list<>

//=============================================================================
/**
	Abstract base class for root-level items.  
	Root-level items include statements that can be found
	in namespaces.  
	Assertion: all root items are nonterminals.  
 */
class root_item : virtual public node {
public:
	root_item();
virtual	~root_item();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};	// end class root_item

typedef node_list<root_item>	root_body;

#define root_body_wrap(b,l,e)						\
	IS_A(root_body*, l->wrap(b,e))
#define root_body_append(l,d,n)						\
	IS_A(root_body*, l->append(d,n))

//=============================================================================
/**
	Abstract base class for general expressions.  
	Expressions may be terminal or nonterminal.  
 */
class expr : virtual public node {
public:
	expr();
virtual	~expr();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
/**
	In all implementations, must create a param_expr object.
	The created object will be pushed onto the context's stack.  
 */
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
};	// end class expr

///	all expression lists are comma-separated
// need to sub-class expression lists into template args and actuals
typedef node_list_base<expr>	expr_list_base;
typedef node_list<expr,comma>	expr_list;

#define expr_list_wrap(b,l,e)						\
	IS_A(expr_list*, l->wrap(b,e))
#define expr_list_append(l,d,n)						\
	IS_A(expr_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/**
	An expression list specialized for template arguments.  
	Derive from expr_list and re-cast list?  or just contain the list?
	Make sure that whatever contains this, to check for the 
	case where the template arguments are NULL 
	when they are supposed to be.  
 */
class template_argument_list : public expr_list {	// or expr_list_base?
public:
	template_argument_list(expr_list* e);
	~template_argument_list();

	ostream& what(ostream& o) const;
	using expr_list::leftmost;
	using expr_list::rightmost;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class template_argument_list

#define template_argument_list_wrap(b,l,e)				\
	IS_A(template_argument_list*, l->wrap(b,e))
#define template_argument_list_append(l,d,n)				\
	IS_A(template_argument_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/**
	An expression list specialized for port connection arguments.  
 */
class connection_argument_list : public expr_list {
public:
	connection_argument_list(expr_list* e);
virtual	~connection_argument_list();

virtual	ostream& what(ostream& o) const;
using expr_list::leftmost;
using expr_list::rightmost;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class connection_argument_list

#define connection_argument_list_wrap(b,l,e)				\
	IS_A(connection_argument_list*, l->wrap(b,e))
#define connection_argument_list_append(l,d,n)				\
	IS_A(connection_argument_list*, l->append(d,n))

//=============================================================================
/// single token characters
class token_char : public terminal {
protected:
/// the character
	int c;
public:
explicit token_char(const int i);
virtual	~token_char();

virtual	int string_compare(const char* d) const;
virtual	ostream& what(ostream& o) const;
};	// end class token_char

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
/// stores an integer (long) in native form and retains position information
class token_int : public terminal, public expr {
protected:
	long val;			///< the value
public:
/// standard constructor
explicit token_int(const long v);
/// standard virtual destructor
	~token_int();

	int string_compare(const char* d) const;
	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_int

//=============================================================================
/// stores an float (double) in native form and retains position information
class token_float : public terminal, public expr {
protected:
	double val;			///< the value
public:
/// standard constructor
explicit token_float(const double v);
/// standard virtual destructor
	~token_float();

	int string_compare(const char* d) const;
	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_float

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
explicit token_string(const char* s);
	token_string(const token_string& s);
virtual	~token_string();

virtual	int string_compare(const char* d) const;
virtual	ostream& what(ostream& o) const;
virtual	line_position rightmost(void) const;

// never really check the type of a string yet (no built-in type yet)
// virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_string

//-----------------------------------------------------------------------------
/**
	Class for single plain identifiers, used in declarations.  
	Final, no sub-classes.  
 */
class token_identifier : public token_string, public expr {
					// consider postfix_expr?
public:
explicit token_identifier(const char* s);
	token_identifier(const token_identifier& i);
	~token_identifier();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_identifier

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<token_identifier,scope>	qualified_id_base;

/**
	Generalized scoped identifier, referring to a type or instance.  
	Has two modes: absolute or relative, depending on whether or 
	not the un/qualified identifier was prefixed by an additional
	scope (::) operator.  
	Even short unqualified identifiers are wrapped into this class
	for uniform use.  
	When type checking with this class, remember to check the 
	path mode before searching, and use all but the last identifier
	chain as the namespace path prefix.  
	e.g. for A::B::C, search for namespace match of A::B with member C.  
	Defined in "art_parser_expr.cc".
 */
class qualified_id : public qualified_id_base {
public:
	typedef	qualified_id::iterator		iterator;
	typedef	qualified_id::const_iterator	const_iterator;
	typedef	qualified_id::reverse_iterator	reverse_iterator;
	typedef	qualified_id::const_reverse_iterator	const_reverse_iterator;
protected:
	/**
		Indicates whether identifier is absolute, meaning
		that it is to be resolved from the global scope down, 
		as opposed to inner scope outward (relative).
		Particularly useful for disambiguation.
	 */
	token_string*			absolute;
public:
explicit qualified_id(token_identifier* n);
	qualified_id(const qualified_id& i);
virtual	~qualified_id();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

// should return a type object, with which one may pointer compare
//	with typedefs, follow to canonical
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;

using qualified_id_base::begin;
using qualified_id_base::end;
using qualified_id_base::empty;

// overshadow parent's
virtual	qualified_id* append(terminal* d, token_identifier* n);

/// Tags this id_expr as absolute, to be resolved from the global scope.  
qualified_id*	force_absolute(token_string* s);
bool		is_absolute(void) const { return absolute != NULL; }

// want a method for splitting off the last id, isolating namespace portion
// copy must be using never_ptrs! original must use excl_ptr
qualified_id	copy_namespace_portion(void) const;
		// remember to delete this after done using!
qualified_id	copy_beheaded(void) const;

friend	ostream& operator << (ostream& o, const qualified_id& id);
};	// end class qualified_id

// no need for wrap, ever
#define qualified_id_append(l,d,n)					\
	IS_A(qualified_id*, l->append(d,n))

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class qualified_id_slice : public sublist<count_const_ptr<token_identifier> > {
protected:
	typedef	sublist<count_const_ptr<token_identifier> >	parent;
protected:
	const bool	 absolute;
public:
	/**
		Constructor (implicit) that take a plain qualified_id.  
		Works because qualified_id is a subclass of
		list<excl_const_ptr<token_identifier> >.  
		By default, just wrap with begin and end iterators 
		around the entire list.  
	 */
	qualified_id_slice(const qualified_id& qid) :
		parent(qid), absolute(qid.is_absolute()) { }
	qualified_id_slice(const qualified_id_slice& qid) :
		parent(qid.the_list), absolute(qid.absolute) { }
	~qualified_id_slice() { }

bool	is_absolute(void) const { return absolute; }
/***
using parent::begin;
using parent::end;
using parent::rbegin;
using parent::rend;
using parent::empty;
***/

qualified_id_slice& behead(void) { parent::behead(); return *this; }
qualified_id_slice& betail(void) { parent::betail(); return *this; }

friend ostream& operator << (ostream& o, const qualified_id_slice& q);
};	// end class qualified_id_slice

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Generalized scoped identifier namespace.  
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
class namespace_id : public node {
protected:
	/**
		Wraps around a qualified_id.  
	 */
	const excl_ptr<qualified_id>	qid;
public:
explicit namespace_id(qualified_id* i);
	~namespace_id();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;

//	consider c->lookup_namespace()
//	never_const_ptr<object> check_build(never_ptr<context> c) const;

	never_const_ptr<qualified_id> get_id(void) const
		{ return qid; }
//		{ return never_const_ptr<qualified_id>(qid); }
		// gcc-2.95.3 dies on this.

/// Tags this id_expr as absolute, to be resolved from the global scope.  
	qualified_id*	force_absolute(token_string* s)
				{ return qid->force_absolute(s); }
	bool	is_absolute(void) const { return qid->is_absolute(); }

	friend	ostream& operator << (ostream& o, const namespace_id& id);
};	// end class namespace_id

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
qualified_id*	force_absolute(token_string* s)
			{ return qid->force_absolute(s); }
bool		is_absolute(void) const { return qid->is_absolute(); }

// want a method for splitting off the last id, isolating namespace portion
qualified_id	copy_namespace_portion(void) const
			{ return qid->copy_namespace_portion(); }
		// remember to delete this after done using!?

friend	ostream& operator << (ostream& o, const id_expr& id);
};	// end class id_expr

//-----------------------------------------------------------------------------
/// keyword version of token_string class, not necessarily an expr
class token_keyword : public token_string {
public:
	token_keyword(const char* s);
virtual	~token_keyword();

virtual	ostream& what(ostream& o) const;
};	// end class token_keyword

//-----------------------------------------------------------------------------
/// class for expression keywords, which happen to be only bools
class token_bool : public token_keyword, public expr {
public:
	token_bool(const char* tf);
	~token_bool();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_bool

//-----------------------------------------------------------------------------
/// class for "else" keyword, which is a legitimate expr
class token_else : public token_keyword, public expr {
public:
	token_else(const char* tf);
	~token_else();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_else

//-----------------------------------------------------------------------------
/// quoted-string version of token_string class
class token_quoted_string : public token_string, public expr {
public:
	token_quoted_string(const char* s);
	~token_quoted_string();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;

// not until we have built-in type for string
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_quoted_string

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

/// all range lists are comma-separated, until we discard for C-style arrays
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

/// all range lists are comma-separated, until we discard for C-style arrays
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
virtual	~member_expr();

virtual	ostream& what(ostream& o) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class member_expr

//-----------------------------------------------------------------------------
/// class for array indexing, with support for multiple dimensions and ranges
// final class?
class index_expr : public postfix_expr {
protected:
	const excl_const_ptr<range_list>	ranges;		///< index
public:
	index_expr(const expr* l, const range_list* i);
virtual	~index_expr();

virtual	ostream& what(ostream& o) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
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
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;	// = 0;
};	// end class binary_expr

//-----------------------------------------------------------------------------
/// class of arithmetic expressions
class arith_expr : public binary_expr {
public:
	arith_expr(const expr* left, const terminal* o, const expr* right);
virtual	~arith_expr();

virtual	ostream& what(ostream& o) const;
// virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class arith_expr

//-----------------------------------------------------------------------------
/// class of relational expressions
class relational_expr : public binary_expr {
public:
	relational_expr(const expr* left, const terminal* o, const expr* right);
virtual	~relational_expr();

virtual	ostream& what(ostream& o) const;
// virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class relational_expr

//-----------------------------------------------------------------------------
/// class of logical expressions
class logical_expr : public binary_expr {
public:
	logical_expr(const expr* left, const terminal* o, const expr* right);
virtual	~logical_expr();

virtual	ostream& what(ostream& o) const;
// virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class logical_expr

//=============================================================================
/**
	Abstract base class for types in general (parameters, data, channel, 
	process...)
	Does not include any template parameters.  
 */
class type_base : virtual public node {
public:
	type_base();
virtual	~type_base();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;

/**
	Should return valid pointer to a fundamental type definition, 
	parameter, data, channel, or process.  
 */
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
};	// end class type_base

//-----------------------------------------------------------------------------
/**
	Abstract base class for keywords that correspond to built-in types, 
	sub-classed into data-types and parameter types).  
 */
class token_type : public token_keyword, public type_base {
public:
	token_type(const char* tf);
virtual	~token_type();

// just use parent's
virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
};	// end class token_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "int" and "bool" data types.
 */
class token_datatype : public token_type {
public:
	token_datatype(const char* dt);
virtual	~token_datatype();

virtual	ostream& what(ostream& o) const;
using token_type::leftmost;
using token_type::rightmost;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
};	// end class token_datatype

//-----------------------------------------------------------------------------
/**
	Class for built-in "int" data type.
 */
class token_int_type : public token_datatype {
public:
	token_int_type(const char* dt);
	~token_int_type();

//	ostream& what(ostream& o) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_int_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "bool" data type.
 */
class token_bool_type : public token_datatype {
public:
	token_bool_type(const char* dt);
	~token_bool_type();

//	ostream& what(ostream& o) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_bool_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "pint" and "pbool" parameter types.
 */
class token_paramtype : public token_type {
public:
	token_paramtype(const char* dt);
virtual	~token_paramtype();

virtual	ostream& what(ostream& o) const;
using token_type::leftmost;
using token_type::rightmost;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
};	// end class token_paramtype

//-----------------------------------------------------------------------------
/**
	Class for built-in "pint" parameter type.
 */
class token_pbool_type : public token_paramtype {
public:
	token_pbool_type(const char* dt);
	~token_pbool_type();

//	ostream& what(ostream& o) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_pbool_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "pint" parameter type.
 */
class token_pint_type : public token_paramtype {
public:
	token_pint_type(const char* dt);
	~token_pint_type();

//	ostream& what(ostream& o) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class token_pint_type

//-----------------------------------------------------------------------------
/**
	Type identifier.
	Template parameters are separate.  
	Just wraps around id_expr;
	Can refer to a any user-defined data, channel, or process type.  
 */
class type_id : public type_base {
protected:
	const excl_const_ptr<qualified_id>	base;
public:
	type_id(const qualified_id* b);
virtual	~type_id();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;

friend	ostream& operator << (ostream& o, const type_id& id);
};	// end class type_id

//-----------------------------------------------------------------------------

/// list of base data types
typedef node_list<concrete_type_ref,comma>	data_type_ref_list;
	// consider making concrete_datatype_ref sub-class
	// or overriding class's check_build

// construction macros
#define data_type_ref_list_wrap(b,l,e)					\
	IS_A(data_type_ref_list*, l->wrap(b,e))
#define data_type_ref_list_append(l,d,n)				\
        IS_A(data_type_ref_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/// full base channel type, including base type list
class chan_type : public type_base {
protected:
	const excl_const_ptr<token_keyword>	chan;	///< keyword "channel"
	const excl_const_ptr<token_char>	dir;	///< port direction: in or out
	excl_const_ptr<data_type_ref_list>	dtypes;	///< data types communicated
public:
	chan_type(const token_keyword* c, const token_char* d = NULL, 
		const data_type_ref_list* t = NULL);
virtual	~chan_type();

chan_type* attach_data_types(const data_type_ref_list* t);

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class chan_type

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
	const terminal*		op;
#else
	excl_const_ptr<expr>		e;
	excl_const_ptr<terminal>	op;
#endif
public:
	incdec_stmt(const expr* n, const terminal* o);
//	incdec_stmt(excl_const_ptr<expr> n, excl_const_ptr<terminal> o);
virtual	~incdec_stmt();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

	const expr* release_expr(void);
	const terminal* release_op(void);
#if 0
	excl_const_ptr<expr> release_expr(void);
	excl_const_ptr<terminal> release_op(void);
#endif
};	// end class incdec_stmt

//-----------------------------------------------------------------------------
/// class for binary expression statements, with left- and right-hand sides
class assign_stmt : virtual public statement {
protected:
#if 0
	excl_const_ptr<expr>		lhs;		///< destination
	excl_const_ptr<terminal>	op;		///< operation
	excl_const_ptr<expr>		rhs;		///< source expression
#else
	const expr*		lhs;		///< destination
	const terminal*		op;		///< operation
	const expr*		rhs;		///< source expression
#endif
public:
	assign_stmt(const expr* left, const terminal* o, const expr* right);
#if 0
	assign_stmt(excl_const_ptr<expr> left, excl_const_ptr<terminal> o,
		excl_const_ptr<expr> right);
#endif
virtual	~assign_stmt();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

#if 0
	excl_const_ptr<expr> release_lhs(void);
	excl_const_ptr<terminal> release_op(void);
	excl_const_ptr<expr> release_rhs(void);
#else
	const expr* release_lhs(void);
	const terminal* release_op(void);
	const expr* release_rhs(void);
#endif
};	// end class assign_stmt

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
};	// end class def_body_item

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
	excl_const_ptr<token_keyword>	tag;	///< what language
public:
	language_body(const token_keyword* t);
virtual	~language_body();

virtual language_body* attach_tag(token_keyword* t);

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const = 0;
};	// end class language_body

//=============================================================================
/// namespace enclosed body
class namespace_body : public root_item {
protected:
	const excl_const_ptr<token_keyword>	ns;	///< keyword "namespace"
	const excl_const_ptr<token_identifier>	name;	///< name of namespace
	const excl_const_ptr<terminal>		lb;
	const excl_const_ptr<root_body>		body;	///< contents of namespace
	const excl_const_ptr<terminal>		rb;
	const excl_const_ptr<terminal>		semi;	///< semicolon token
public:
	namespace_body(const token_keyword* s, const token_identifier* n, 
		const terminal* l, const root_body* b,
		const terminal* r, const terminal* c);
virtual	~namespace_body();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class namespace_body

//-----------------------------------------------------------------------------
/// command to search namespace for identifiers
class using_namespace : public root_item {
protected:
	const excl_const_ptr<token_keyword>		open;
	const excl_const_ptr<namespace_id>		id;
	const excl_const_ptr<token_keyword>		as;
	const excl_const_ptr<token_identifier>		alias;
	const excl_const_ptr<token_char>		semi;
public:
	using_namespace(const token_keyword* o, const namespace_id* i,
		const token_char* s);
		// a "AS" and n (alias) are optional
	using_namespace(const token_keyword* o, const namespace_id* i, 
		const token_keyword* a, const token_identifier* n,
		const token_char* s);
virtual	~using_namespace();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;

virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class using_namespace

//=============================================================================
/**
	Base class for instance-related items, including declarations, 
	arrays, connections and aliases, conditionals, loops.  
 */
class instance_management : virtual public def_body_item, 
		virtual public root_item {
public:
	instance_management();
virtual	~instance_management();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};	// end class instance_management

//-----------------------------------------------------------------------------
/**
	A list of lvalue expressions aliased/connected together.  
 */
class alias_list : public instance_management, public node_list<expr,alias> {
private:
	typedef node_list<expr,alias>		alias_list_base;
public:
	alias_list(expr* e);
virtual	~alias_list();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class alias_list

#define alias_list_wrap(b,l,e)						\
	IS_A(alias_list*, l->wrap(b,e))
#define alias_list_append(l,d,n)					\
        IS_A(alias_list*, l->append(d,n))

//=============================================================================
/**
	Abstract base class for connection statements of instantiations.  
	Contains actuals list of arguments.  
 */
class actuals_base : virtual public instance_management {
protected:
	const excl_const_ptr<expr_list>		actuals;
public:
	actuals_base(const expr_list* l);
virtual	~actuals_base();

// same virtual methods
// virtual	ostream& what(ostream& o) const;
// virtual	line_position leftmost(void) const;
// virtual	line_position rightmost(void) const;
// virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class actuals_base

//=============================================================================
/**
	Basic instance identifier, no trimmings, just contains an identifier.  
 */
class instance_base : virtual public instance_management {
protected:
/**
	In instantiation context, id should only be a token_identifier, 
 */
	const excl_const_ptr<token_identifier>		id;
public:
	instance_base(const token_identifier* i);
virtual	~instance_base();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance_base

typedef	node_list<instance_base,comma>	instance_id_list;

#define instance_id_list_wrap(b,l,e)					\
	IS_A(instance_id_list*, l)->wrap(b,e)
#define instance_id_list_append(l,d,n)					\
	IS_A(instance_id_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/// instance identifier with ranges
class instance_array : public instance_base {
protected:
	const excl_const_ptr<range_list>	ranges;	///< optional ranges
public:
	instance_array(const token_identifier* i, const range_list* rl);
virtual	~instance_array();

virtual	ostream& what(ostream& o) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance_array

//=============================================================================
/**
	A collection of instances of the same type. 
	Contains a list of instance identifiers.  
 */
class instance_declaration : public instance_management {
protected:
	/**
		The base type of the instantiations in this collection.  
	 */
	const excl_const_ptr<concrete_type_ref>		type;
	/**
		List of instance_base.  
	 */
	const excl_const_ptr<instance_id_list>		ids;
	const excl_const_ptr<terminal>			semi;
public:
	instance_declaration(const concrete_type_ref* t, 
		const instance_id_list* i, const terminal* s = NULL);
virtual	~instance_declaration();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance declaration

//=============================================================================
/**
	Class for port data (rather, members) of a user-defined channels.  
 */
class data_param_id : public node {
	// should be called data_port_id
protected:
	const excl_const_ptr<token_identifier>	id;
	const excl_const_ptr<dense_range_list>	dim;
public:
	data_param_id(const token_identifier* i, const dense_range_list* d);
	~data_param_id();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class data_param_id

typedef	node_list<data_param_id,comma>	data_param_id_list;

#define data_param_id_list_wrap(b,l,e)					\
	IS_A(data_param_id_list*, l)->wrap(b,e)
#define data_param_id_list_append(l,d,n)				\
	IS_A(data_param_id_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/**
	Data parameter port declarations, grouped together by type.  
 */
class data_param_decl : public node {
protected:
	/**
		The base type of the data ports in this collection.  
	 */
	const excl_const_ptr<concrete_type_ref>		type;
	const excl_const_ptr<data_param_id_list>	ids;
public:
	data_param_decl(const concrete_type_ref* t, 
		const data_param_id_list* il);
	~data_param_decl();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class data_param_decl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// clever re-use of declaration classes, or not...
typedef	node_list<data_param_decl,semicolon>		data_param_decl_list;

#define data_param_decl_list_wrap(b,l,e)				\
	IS_A(data_param_decl_list*, l->wrap(b,e))
#define data_param_decl_list_append(l,d,n)				\
	IS_A(data_param_decl_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/// class for an or instance port connection or declaration connection
class instance_connection : public instance_base, public actuals_base {
protected:
//	const excl_const_ptr<token_identifier>	id;		// inherited
//	const excl_const_ptr<expr_list>		actuals;	// inherited
	const excl_const_ptr<terminal>		semi;	///< semicolon (optional)
public:
	instance_connection(const token_identifier* i, const expr_list* a, 
		const terminal* s = NULL);
virtual	~instance_connection();

// remember to check for declaration context when checking id

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance_connection

//-----------------------------------------------------------------------------
/**
	Statement that is just a connection of ports.  
	Resembles a plain function call.  
	Unlike instance_connection, this doesn't create any new 
	instantiations.  
 */
class connection_statement : public actuals_base {
protected:
//	const excl_const_ptr<expr_list>		actuals;	// inherited
	const excl_const_ptr<expr>		lvalue;
	const excl_const_ptr<terminal>		semi;
public:
	connection_statement(const expr* l, const expr_list* a, 
		const terminal* s = NULL);
virtual	~connection_statement();
virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
// virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class connection_statement

//-----------------------------------------------------------------------------
/**
	An alias statement without type identifier, 
	instance is declared at the same time.  
	Can also be a parameter instantiation initialized
	to other parameter expressions.  
	Contains a list of alias identifier expressions.  
 */
class instance_alias : public instance_base {
protected:
//	const excl_const_ptr<token_identifier>	id;	// inherited
	const excl_const_ptr<alias_list>	aliases;
	const excl_const_ptr<terminal>		semi;	///< semicolon
public:
	instance_alias(const token_identifier* i, alias_list* al, 
		const terminal* s = NULL);
virtual	~instance_alias();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance_alias

//=============================================================================
/// class for loop instantiations, to be unrolled in the build phase
class loop_instantiation : public instance_management {
protected:
	const excl_const_ptr<terminal>		lp;
	const excl_const_ptr<terminal>		delim;
	const excl_const_ptr<token_identifier>	index;
	const excl_const_ptr<terminal>		colon1;
	const excl_const_ptr<range>		rng;
	const excl_const_ptr<terminal>		colon2;
	const excl_const_ptr<definition_body>	body;
	const excl_const_ptr<terminal>		rp;
public:
	loop_instantiation(const terminal* l, const terminal* d, 
		const token_identifier* i, const terminal* c1, 
		const range* g, const terminal* c2, 
		const definition_body* b, const terminal* r);
virtual	~loop_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};	// end class loop_instantiation

//=============================================================================
/// single port formal identifier, with optional dimension array specification
class port_formal_id : public node {
protected:
	const excl_const_ptr<token_identifier>	name;	///< formal name
	const excl_const_ptr<dense_range_list>	dim;	///< optional dimensions
public:
	port_formal_id(const token_identifier* n, const dense_range_list* d);
	~port_formal_id();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class port_formal_id

/// list of port-formal identifiers (optional arrays)
typedef	node_list<port_formal_id,comma>	port_formal_id_list;

#define port_formal_id_list_wrap(b,l,e)					\
	IS_A(port_formal_id_list*, l)->wrap(b,e)
#define port_formal_id_list_append(l,d,n)				\
	IS_A(port_formal_id_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/**
	Port formal declaration contains a type and identifier list.
 */
class port_formal_decl : public node {
protected:
	const excl_const_ptr<concrete_type_ref>		type;	///< formal base type
	const excl_const_ptr<port_formal_id_list>	ids;	///< identifier list
public:
	port_formal_decl(const concrete_type_ref* t, 
		const port_formal_id_list* i);
virtual	~port_formal_decl();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};	// end class port_formal_decl

/**
	List of port-formal declarations.  
	Later: distinguish between empt ports and NULL (unspecified ports).  
 */
typedef	node_list<port_formal_decl,semicolon>	port_formal_decl_list;

#define port_formal_decl_list_wrap(b,l,e)				\
	IS_A(port_formal_decl_list*, l)->wrap(b,e)
#define port_formal_decl_list_append(l,d,n)				\
	IS_A(port_formal_decl_list*, l)->append(d,n) 

//=============================================================================
/**
	Single template formal identifier,
	with optional dimension array spec.  
	Now with optional default argument expression.  
 */
class template_formal_id : public node {
protected:
	const excl_const_ptr<token_identifier>	name;	///< formal name
	const excl_const_ptr<dense_range_list>	dim;	///< optional dimensions
	const excl_const_ptr<token_char>	eq;	///< '=' token
	const excl_const_ptr<expr>		dflt;	///< default value
public:
	template_formal_id(const token_identifier* n,
		const dense_range_list* d, 
		const token_char* e = NULL, const expr* v = NULL);
virtual	~template_formal_id();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class template_formal_id

/// list of template-formal identifiers (optional arrays)
typedef	node_list<template_formal_id,comma>	template_formal_id_list;

#define template_formal_id_list_wrap(b,l,e)				\
	IS_A(template_formal_id_list*, l)->wrap(b,e)
#define template_formal_id_list_append(l,d,n)				\
	IS_A(template_formal_id_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/**
	template formal declaration contains a type and identifier list
	Later: ADVANCED -- type can be a concrete_type_ref!
		Mr. Fancy-pants!
		Don't forget that its check build already
		sets current_fundamental_type
 */
class template_formal_decl : public node {
protected:
	const excl_const_ptr<token_paramtype>	type;	///< formal base type
		// why not concrete_type_ref?
	const excl_const_ptr<template_formal_id_list>	ids;	///< identifiers
public:
	template_formal_decl(const token_paramtype* t, 
		const template_formal_id_list* i);
virtual	~template_formal_decl();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class template_formal_decl

//-----------------------------------------------------------------------------
typedef	node_list<template_formal_decl,semicolon> template_formal_decl_list;

#define template_formal_decl_list_wrap(b,l,e)				\
	IS_A(template_formal_decl_list*, l->wrap(b,e))
#define template_formal_decl_list_append(l,d,n)				\
	IS_A(template_formal_decl_list*, l->append(d,n))

//=============================================================================
/**
	Reference to a concrete type, i.e. definition with its
	template parameters specified (if applicable).
 */
class concrete_type_ref : public node {
protected:
	/** definition name base */
	const excl_const_ptr<type_base>			base;
	/** optional template arguments */
	const excl_const_ptr<template_argument_list>	temp_spec;
public:
	concrete_type_ref(const type_base* n, 
		const template_argument_list* t = NULL);
virtual	~concrete_type_ref();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class concrete_type_ref

//=============================================================================
/**
	Abstract base class for definitions of complex types, 
	including processes, user-defined channels and data-types.  
	All definitions are templatable!  
 */
class definition : public root_item {
protected:
public:
	definition();
virtual	~definition();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};	// end class definition

//-----------------------------------------------------------------------------
/// abstract base class for prototypes
class prototype : public root_item {
public:
	prototype();
virtual	~prototype();

// don't bother re-declaring virtual methods
};	// end class prototype

//=============================================================================
/**
	Abstract base class for any definition or declaration.
	All definitions and prototypes may potentially contain a
	template signature.  
 */
class signature_base : virtual public node {
protected:
	const excl_const_ptr<template_formal_decl_list>	temp_spec;
	const excl_const_ptr<token_identifier>		id;
public:
	signature_base(const template_formal_decl_list* tf, 
		const token_identifier* i) :
		node(), temp_spec(tf), id(i) { }
virtual	~signature_base();

virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
};	// end class signature_base

//=============================================================================
/**
	Abstract base class for basic process information
	(prototype, basically).  
	This class need not be a node or root_item because it's never 
	constructed on the symbol stack.  
 */
class process_signature : public signature_base {
protected:
	const excl_const_ptr<token_keyword>	def;	///< definition keyword
		// should never be NULL, could be const reference?
	/**
		Optional port formal list.  
		Never NULL, but may be empty.  
	 */
	const excl_const_ptr<port_formal_decl_list>	ports;
public:
	process_signature(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p);
virtual	~process_signature();

// note: non-virtual
	const token_identifier& get_name(void) const;
//	excl_ptr<process_definition>
//		make_process_signature(never_ptr<context> c) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class process_signature

//-----------------------------------------------------------------------------
/// process prototype declaration
class process_prototype : public prototype, public process_signature {
protected:
	const excl_const_ptr<token_char>	semi;	///< semicolon token
public:
	process_prototype(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p, const token_char* s);
	~process_prototype();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class process_prototype

//-----------------------------------------------------------------------------
/**
	Process definition, syntax structure.  
 */
class process_def : public definition, public process_signature {
protected:
//	const excl_const_ptr<token_keyword>		def;	//  inherited
//	const excl_const_ptr<concrete_type_ref>		idt;	//  inherited
//	const excl_const_ptr<port_formal_decl_list>	ports;	//  inherited
	const excl_const_ptr<definition_body>		body;	///< definition body
public:
	process_def(const template_formal_decl_list*, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p, const definition_body* b);
	~process_def();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class process_def

//=============================================================================
/// conditional instantiations in definition body
class guarded_definition_body : public instance_management {
protected:
	const excl_const_ptr<expr>	guard;	///< condition expression
	const excl_const_ptr<terminal>		arrow;	///< right arrow
	const excl_const_ptr<definition_body>	body;
public:
	guarded_definition_body(const expr* e, const terminal* a, 
		const definition_body* b);
virtual	~guarded_definition_body();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};	// end class guarded_definition_body

/// list of template-formal declarations
typedef	node_list<guarded_definition_body,thickbar>
		guarded_definition_body_list;

#define guarded_definition_body_list_wrap(b,l,e)			\
	IS_A(guarded_definition_body_list*, l)->wrap(b,e)
#define guarded_definition_body_list_append(l,d,n)			\
	IS_A(guarded_definition_body_list*, l)->append(d,n) 

//-----------------------------------------------------------------------------
/// wrapper class for conditional instantiations
class conditional_instantiation : public instance_management {
protected:
	const excl_const_ptr<guarded_definition_body_list>	gd;
public:
	conditional_instantiation(const guarded_definition_body_list* n);
virtual	~conditional_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};	// end class conditional_instantiation

//=============================================================================
/// user-defined data type
class user_data_type_signature : public signature_base {
protected:
	const excl_const_ptr<token_keyword>	def;	///< "deftype" keyword
	const excl_const_ptr<token_string>	dop;	///< <: operator
	const excl_const_ptr<concrete_type_ref>	bdt;	///< the represented type
	const excl_const_ptr<data_param_decl_list>
						params;	///< implementation type
public:
	user_data_type_signature(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, 
		const concrete_type_ref* b, 	// or concrete_datatype_ref
		const data_param_decl_list* p);
virtual	~user_data_type_signature();
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class user_data_type_signature

//-----------------------------------------------------------------------------
/// user-defined data type prototype declaration
class user_data_type_prototype : public prototype, 
		public user_data_type_signature {
protected:
	const excl_const_ptr<token_char>	semi;	///< semicolon
public:
	user_data_type_prototype(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p, const token_char* s);
	~user_data_type_prototype();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class user_data_type_prototype

//-----------------------------------------------------------------------------
/// user-defined data type (is not a type_base)
class user_data_type_def : public definition, public user_data_type_signature {
protected:
//	const excl_const_ptr<token_keyword>	def;	// inherited
//	const excl_const_ptr<token_identifier>	name;	// inherited
//	const excl_const_ptr<token_string>	dop;	// inherited
//	const excl_const_ptr<concrete_type_ref>	bdt;	// inherited
//	const excl_const_ptr<data_param_decl_list>	params;	// inherited
	const excl_const_ptr<token_char>	lb;	///< left brace
	const excl_const_ptr<language_body>	setb;	///< set body
	const excl_const_ptr<language_body>	getb;	///< get body
	const excl_const_ptr<token_char>	rb;	///< right brace
public:
	user_data_type_def(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p, 
		const token_char* l, const language_body* s,
		const language_body* g, const token_char* r);
	~user_data_type_def();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class user_data_type_def

//-----------------------------------------------------------------------------
/**
	Common field data to enumeration declaration and definitions.  
 */
class enum_signature : public signature_base {
protected:
//	const excl_const_ptr<token_identifier>	id;	// inherited
	/** "enum" keyword */
	const excl_const_ptr<token_keyword>	en;
public:
	enum_signature(const token_keyword* e, const token_identifier* i);
virtual	~enum_signature();

//	excl_ptr<enum_datatype_def>
//		make_enum_signature(never_const_ptr<context> c) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class enum_signature

//-----------------------------------------------------------------------------
/**
	Forward declaration of an enum type.  
	Not really a prototype, but classified as such for convenience.  
 */
class enum_prototype : public prototype, public enum_signature {
protected:
	/** semicolon token (optional) */
	const excl_const_ptr<token_char>	semi;
public:
	enum_prototype(const token_keyword* e, const token_identifier* i, 
		const token_char* s);
	~enum_prototype();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class enum_prototype

//-----------------------------------------------------------------------------
typedef	node_list<token_identifier,comma>	enum_member_list_base;

/**
	Specialized list of identifiers for enumeration members.  
 */
class enum_member_list : public enum_member_list_base {
protected:	// no new members
	typedef	enum_member_list_base		parent;
public:
	enum_member_list(const token_identifier* i);
	~enum_member_list();

	/** overrides default */
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end enum_member_list

#define enum_member_list_wrap(b,l,e)					\
	IS_A(enum_member_list*, l->wrap(b,e))
#define enum_member_list_append(l,d,n)					\
	IS_A(enum_member_list*, l->append(d,n))

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Definition of an enumeration data type.
 */
class enum_def : public definition, public enum_signature {
protected:
	/** names of the enumerated members */
	const excl_const_ptr<enum_member_list>	members;
public:
	enum_def(const token_keyword* e, const token_identifier* i,      
		const enum_member_list*	m);
	~enum_def();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class enum_def

//=============================================================================
/// user-defined channel type signature
class user_chan_type_signature : public signature_base {
protected:
	const excl_const_ptr<token_keyword>	def;	///< "defchan" keyword
	const excl_const_ptr<token_string>	dop;	///< <: operator
	const excl_const_ptr<chan_type>		bct;	///< the represented type
	const excl_const_ptr<data_param_decl_list>
						params;	///< the implementation type
public:
	user_chan_type_signature(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p);
virtual	~user_chan_type_signature();
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class user_data_type_signature

//-----------------------------------------------------------------------------
/// user-defined channel type prototype
class user_chan_type_prototype : public prototype, 
		public user_chan_type_signature {
protected:
	const excl_const_ptr<token_char>	semi;	///< semicolon
public:
	user_chan_type_prototype(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p, const token_char* s);
virtual	~user_chan_type_prototype();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};	// end class user_chan_type_prototype

//-----------------------------------------------------------------------------
/// user-defined channel type definition
class user_chan_type_def : public definition, public user_chan_type_signature {
protected:
//	const excl_const_ptr<token_keyword>	def;	// inherited
//	const excl_const_ptr<token_identifier>	name;	// inherited
//	const excl_const_ptr<token_string>	dop;	// inherited
//	const excl_const_ptr<chan_type>		bct;	// inherited
//	const excl_const_ptr<data_param_decl_list>	params;	// inherited
	const excl_const_ptr<token_char>	lb;	///< left brace
	const excl_const_ptr<language_body>	sendb;	///< set body
	const excl_const_ptr<language_body>	recvb;	///< get body
	const excl_const_ptr<token_char>	rb;	///< right brace
public:
	user_chan_type_def(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p, 
		const token_char* l, const language_body* s, 
		const language_body* g, const token_char* r);
	~user_chan_type_def();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class user_chan_type_def

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_H__

