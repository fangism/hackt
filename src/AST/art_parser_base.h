/**
	\file "art_parser_base.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_base.h,v 1.18.4.1 2005/01/23 01:33:51 fang Exp $
 */

#ifndef __ART_PARSER_BASE_H__
#define __ART_PARSER_BASE_H__

#include <string>
#include <iosfwd>

#include "macros.h"
#include "art_lex.h"		// for token_position
#include "sublist.h"		// for efficient list slices
#include "memory/pointer_classes.h"	// experimental pointer classes
#include "art_parser_fwd.h"
#include "memory/list_vector_pool_fwd.h"

/**
	Macro for convenient definition of the node::what member function.  
	util::what is defined in "what.h".
 */
#define PARSER_WHAT_DEFAULT_IMPLEMENTATION(T)				\
std::ostream&								\
T::what(std::ostream& o) const {					\
        return o << util::what<T >::name();				\
}

//=============================================================================
/**
	This is the general namespace for all ART-related classes.  
 */
namespace ART {
//=============================================================================
// global variable
namespace lexer {
extern	token_position current;
}

// forward declaration of outside namespace and classes
namespace entity {
	// defined in "art_object_base.h"
	class object;
}

using lexer::current;			// current token position
using std::string;
using std::ostream;
USING_LIST
using entity::object;
using util::sublist;
using namespace util::memory;		// for experimental pointer classes

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
// some constant delimiter strings, defined in art_parser.cc
// these are used as delimiter template arguments for node_list
extern	const char	none[];
extern	const char	comma[];
extern	const char	semicolon[];
extern	const char	scope[];
extern	const char	thickbar[];
extern	const char	colon[];
extern	const char	alias[];
extern	const char	pound[];	// calling it "hash" would be confusing

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
private:
/**
	Rule regarding g++ emission of vtables:
	"If the class declares any non-inline, non-pure virtual functions, 
	the first one is chosen as the "key method" for the class, 
	and the vtable is only emitted in the translation unit where 
	the key method is defined."
	This bogus method is defined in "art_parser_base.cc".
 */
virtual	void
	bogus(void) const;
public:

/** Standard virtual destructor, mother-of-all virtual destructors */
virtual	~node() { }

/**
	Shows representation without recursive descent.  
	Derived classes of non-terminals should just print their type name.  
	\param o the output stream.  
	\return the output stream.
 */
virtual	ostream&
	what(ostream& o) const = 0;

/**
	Shows the position where the node's subtree starts.  
	\return the starting position.
 */
virtual	line_position
	leftmost(void) const = 0;

/**
	Shows the position where the node's subtree ends.  
	\return the ending position.
 */
virtual	line_position
	rightmost(void) const = 0;

/**
	Shows the range of file position covered by a particular node's
	subtree.  
 */
virtual	line_range
	where(void) const;

/**
	Type-check and return a usable ART::entity::object, which 
	contains a hierarchical symbol table.  
	\return pointer to resulting object.  
 */
virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class node

//=============================================================================
// From here below, begins the definitions of the fundamental base classes
// for the ART parser.  
// Most children classes that derive from them are defined in
// headers "art_parser_*.h".  
//=============================================================================
/**
	Abstract base class for general expressions.
	Expressions may be terminal or nonterminal.
	Defined in "art_parser_expr.h".
 */
class expr : virtual public node {
public:
	expr() { }
virtual ~expr() { }

virtual ostream&
	what(ostream& o) const = 0;

virtual line_position
	leftmost(void) const = 0;

virtual line_position
	rightmost(void) const = 0;
/**
	In all implementations, must create a param_expr object.
	The created object will be pushed onto the context's stack.
 */
virtual never_ptr<const object>
	check_build(context& c) const = 0;
};	// end class expr

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
	terminal() : node(), pos(current) { }

public:
///	standard virtual destructor
virtual	~terminal() { }

public:
virtual	int
	string_compare(const char* d) const = 0;

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;
};	// end class terminal

//-----------------------------------------------------------------------------
/**
	Single token characters.
	Methods defined in "art_parser_token.cc"
 */
class token_char : public terminal {
private:
	typedef	token_char			this_type;
protected:
/// the character
	int c;
public:
	/// inlined, intended only for allocator use
	token_char();

	explicit
	token_char(const int i) : terminal(), c(i) { }

	~token_char() { }

	int
	get_char(void) const { return c; }

	int
	string_compare(const char* d) const;

	ostream&
	what(ostream& o) const;

	LIST_VECTOR_POOL_STATIC_DECLARATIONS
// private:
};      // end class token_char

//-----------------------------------------------------------------------------
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
	explicit
	token_string(const char* s) : node(), string(s), terminal() { }

	/// default copy-constructor
	token_string(const token_string& s) : node(), string(s), terminal() { }

virtual ~token_string() { }

virtual int
	string_compare(const char* d) const;

virtual ostream&
	what(ostream& o) const;

virtual line_position
	rightmost(void) const;

// never really check the type of a string yet (no built-in type yet)
// virtual	never_ptr<const object> check_build(context& c) const;
};	// end class token_string

//-----------------------------------------------------------------------------
/**
	Class for single plain identifiers, used in declarations.
	Final, no sub-classes.
 */
class token_identifier : public token_string, public expr {
					// consider postfix_expr?
public:
	explicit
	token_identifier(const char* s) : node(), token_string(s), expr() { }

	token_identifier(const token_identifier& i) :
		node(), token_string(i), expr() { }

	~token_identifier() { }

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};      // end class token_identifier

//-----------------------------------------------------------------------------
/// keyword version of token_string class, not necessarily an expr
class token_keyword : public token_string {
public:
	explicit
	token_keyword(const char* s) : token_string(s) { }

virtual ~token_keyword() { }

virtual ostream&
	what(ostream& o) const;
};	// end class token_keyword

//=============================================================================
#define	NODE_LIST_BASE_TEMPLATE_SIGNATURE				\
	template <class T>

/**
	This is the general class for list structures of nodes in the
	syntax tree.  What is unique about this implementation is that
	its derived classes keep track of the delimiter tokens that 
	separated the lists, if applicable.  
	The specifier T, a derived class from node, is only used for 
	type-checking.  
	Consider deriving from list<base_ptr<const T> > to allow 
	copyable lists, using never_ptr<const T>'s.  
	Then dynamically casting elements may be a pain?
 */
template <class T>
class node_list_base : virtual public node, public list<count_ptr<T> > {
private:
	/**
		Base class.  (was derived from excl_const_ptr, 
		then some_count_ptr)
	 */
	typedef		list<count_ptr<T> >	list_parent;
	// read-only, but transferrable ownership
public:
	typedef	typename list_parent::value_type	value_type;
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
	explicit
	node_list_base(const T* n);

virtual	~node_list_base();

using	list_parent::begin;
using	list_parent::end;

// later, use static functions (operator <<) to determine type name...
/// Prints out type of first element in list, if not null.  
virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

virtual	never_ptr<const object>
	check_build(context& c) const;

/// Releases memory to the destination list, transfering ownership
virtual	void
	release_append(node_list_base<T>& dest);
};	// end template class node_list_base

//-----------------------------------------------------------------------------
#define	NODE_LIST_TEMPLATE_SIGNATURE_DEFAULT				\
	template <class T, const char D[] = none>
#define	NODE_LIST_TEMPLATE_SIGNATURE					\
	template <class T, const char D[]>
#define	NODE_LIST_TEMPLATE_SIGNATURE_EXPORT				\
	export NODE_LIST_TEMPLATE_SIGNATURE
	// "export" is ignored by gcc, even 3.3

/**
	The delimiter specifier, D, is used for checking that every other
	token is separated by a D character (if D is not '\0').  
 */
NODE_LIST_TEMPLATE_SIGNATURE_DEFAULT
class node_list : public node_list_base<T> {
private:
	typedef		node_list_base<T>	parent;
public:
	typedef	typename parent::iterator	iterator;
	typedef	typename parent::const_iterator	const_iterator;
	typedef	typename parent::reverse_iterator	reverse_iterator;
	typedef	typename parent::const_reverse_iterator	const_reverse_iterator;
	typedef	list<count_ptr<const terminal> >	delim_list;
protected:
	excl_ptr<const terminal>	open;	///< wrapping string, e.g. "("
	excl_ptr<const terminal>	close;	///< wrapping string, e.g. ")"
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

	explicit
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
virtual	node_list<T,D>*
	wrap(const terminal* b, const terminal* e);

/**
	Adds an element to a node list, along with the delimiting
	character token.  Also checks that delimiter matchs, and
	that type of argument matches.
	\param d the delimiter token, such as a comma, must match D.
	\param n the useful node.  
	\return this.
 */
virtual	node_list<T,D>*
	append(const terminal* d, const T* n);

// the following methods are defined in "art_parser_template_methods.h"

/// Prints out type of first element in list, if not null.  
// virtual	ostream& what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

virtual	void
	release_append(node_list<T,D>& dest);
};	// end of template class node_list<>

//=============================================================================
/**
	List of expressions.  
	Implemented in "art_parser_expr.cc".  
	All expression lists are comma-separated.  
	No need to sub-class expression lists into 
	template args and actuals.  

 */
typedef node_list<const expr,comma>			expr_list_base;

/**
	General parser expression list.  
	Class connection_argument_list is derived from expr_list, 
	so we should keep methods virtual.
 */
class expr_list : public expr_list_base {
protected:
	typedef	expr_list_base			parent;
public:
	expr_list();

	explicit
	expr_list(const expr* e);

virtual	~expr_list();

virtual	ostream&
	what(ostream& o) const;

	using parent::leftmost;
	using parent::rightmost;

virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class expr_list

#define expr_list_wrap(b,l,e)						\
	IS_A(expr_list*, l->wrap(b,e))
#define expr_list_append(l,d,n)						\
	IS_A(expr_list*, l->append(d,n))

//=============================================================================
/**
	Abstract base class for root-level items.  
	Root-level items include statements that can be found
	in namespaces.  
	Assertion: all root items are nonterminals.  
 */
class root_item : virtual public node {
public:
	root_item() : node() { }

virtual	~root_item() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;
};	// end class root_item

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef node_list<const root_item>	root_item_list;

/** list of root_items */
class root_body : public root_item_list {
protected:
	typedef	root_item_list			parent;
public:
	explicit
	root_body(const root_item* r);

	~root_body();
};	// end class root_body

#define root_body_wrap(b,l,e)						\
	IS_A(root_body*, l->wrap(b,e))
#define root_body_append(l,d,n)						\
	IS_A(root_body*, l->append(d,n))

//=============================================================================
typedef	node_list<const token_identifier,scope>	qualified_id_base;

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
protected:
	typedef	qualified_id_base		parent;
public:
	typedef	parent::iterator		iterator;
	typedef	parent::const_iterator		const_iterator;
	typedef	parent::reverse_iterator	reverse_iterator;
	typedef	parent::const_reverse_iterator	const_reverse_iterator;
protected:
	/**
		Indicates whether identifier is absolute, meaning
		that it is to be resolved from the global scope down, 
		as opposed to inner scope outward (relative).
		Particularly useful for disambiguation.
	 */
	excl_ptr<const token_string>			absolute;
public:
	explicit
	qualified_id(const token_identifier* n);

	qualified_id(const qualified_id& i);

virtual	~qualified_id();

virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

// should return a type object, with which one may pointer compare
//	with typedefs, follow to canonical
virtual	never_ptr<const object>
	check_build(context& c) const;

using parent::begin;
using parent::end;
using parent::empty;

// overshadow parent's
virtual	qualified_id*
	append(terminal* d, token_identifier* n);

/// Tags this id_expr as absolute, to be resolved from the global scope.  
	qualified_id*
	force_absolute(const token_string* s);

	bool
	is_absolute(void) const { return absolute; }

// want a method for splitting off the last id, isolating namespace portion
// copy must be using never_ptrs! original must use excl_ptr
	qualified_id
	copy_namespace_portion(void) const;
		// remember to delete this after done using!
	qualified_id
	copy_beheaded(void) const;

	friend
	ostream&
	operator << (ostream& o, const qualified_id& id);
};	// end class qualified_id

// no need for wrap, ever
#define qualified_id_append(l,d,n)					\
	IS_A(qualified_id*, l->append(d,n))

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
class qualified_id_slice {
protected:
	typedef	sublist<count_ptr<const token_identifier> >	parent_type;
	parent_type		parent;
public:
 	typedef	parent_type::list_type::size_type
				size_type;
 	typedef	parent_type::list_type::const_iterator
				const_iterator;
protected:
	const bool		absolute;
public:
	/**
		Constructor (implicit) that take a plain qualified_id.  
		Works because qualified_id is a subclass of
		list<excl_ptr<const token_identifier> >.  
		By default, just wrap with begin and end iterators 
		around the entire list.  
	 */
	qualified_id_slice(const qualified_id& qid);

	qualified_id_slice(const qualified_id_slice& qid);

	~qualified_id_slice();

	bool
	is_absolute(void) const { return absolute; }

	size_type
	size(void) const { return std::distance(parent.begin(), parent.end()); }

/***
using parent::begin;
using parent::end;
using parent::rbegin;
using parent::rend;
using parent::empty;
***/
	const_iterator
	begin(void) const { return parent.begin(); }

	const_iterator
	end(void) const { return parent.end(); }

	bool
	empty(void) const { return parent.empty(); }

	qualified_id_slice&
	behead(void) { parent.behead(); return *this; }

	qualified_id_slice&
	betail(void) { parent.betail(); return *this; }

	friend
	ostream&
	operator << (ostream& o, const qualified_id_slice& q);
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
	explicit
	namespace_id(qualified_id* i);

	~namespace_id();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

//	consider c->lookup_namespace()
//	never_ptr<const object> check_build(context& c) const;

	never_ptr<const qualified_id>
	get_id(void) const { return qid; }
//		{ return never_ptr<const qualified_id>(qid); }
		// gcc-2.95.3 dies on this.

/// Tags this id_expr as absolute, to be resolved from the global scope.  
	qualified_id*
	force_absolute(const token_string* s);

	bool
	is_absolute(void) const;

	friend
	ostream&
	operator << (ostream& o, const namespace_id& id);
};	// end class namespace_id

//=============================================================================
/**
	Abstract base class for types in general (parameters, data, channel, 
	process...)
	Does not include any template parameters.  
 */
class type_base : virtual public node {
public:
	type_base() : node() { }

virtual	~type_base() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;

	/**
		Should return valid pointer to a fundamental type definition, 
		parameter, data, channel, or process.  
	 */
virtual	never_ptr<const object>
	check_build(context& c) const = 0;
};	// end class type_base

//-----------------------------------------------------------------------------
/**
	Type identifier.
	Template parameters are separate.  
	Just wraps around id_expr;
	Can refer to a any user-defined data, channel, or process type.  
 */
class type_id : public type_base {
protected:
	const excl_ptr<const qualified_id>	base;
public:
	explicit
	type_id(const qualified_id* b);

virtual	~type_id();

virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

virtual	never_ptr<const object>
	check_build(context& c) const;

	friend
	ostream&
	operator << (ostream& o, const type_id& id);
};	// end class type_id

//-----------------------------------------------------------------------------
typedef node_list<const concrete_type_ref,comma>	data_type_ref_list_base;
	// consider making concrete_datatype_ref sub-class
	// or overriding class's check_build

/// list of base data types
class data_type_ref_list : public data_type_ref_list_base {
protected:
	typedef	data_type_ref_list_base			parent;
public:
	explicit
	data_type_ref_list(const concrete_type_ref* c);

	~data_type_ref_list();
};

// construction macros
#define data_type_ref_list_wrap(b,l,e)					\
	IS_A(data_type_ref_list*, l->wrap(b,e))
#define data_type_ref_list_append(l,d,n)				\
        IS_A(data_type_ref_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/// full base channel type, including base type list
class chan_type : public type_base {
protected:
	const excl_ptr<const token_keyword>	chan;	///< keyword "channel"
	const excl_ptr<const token_char>	dir;	///< port direction: in or out
	excl_ptr<const data_type_ref_list>	dtypes;	///< data types communicated
public:
	chan_type(const token_keyword* c, const token_char* d = NULL, 
		const data_type_ref_list* t = NULL);

virtual	~chan_type();

	chan_type*
	attach_data_types(const data_type_ref_list* t);

virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

virtual	never_ptr<const object>
	check_build(context& c) const;
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
	statement() : node() { }

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
	const terminal*		op;
#else
	excl_ptr<const expr>		e;
	excl_ptr<const terminal>	op;
#endif
public:
	incdec_stmt(const expr* n, const terminal* o);
//	incdec_stmt(excl_ptr<const expr> n, excl_ptr<const terminal> o);
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

	const terminal*
	release_op(void);
#else
	excl_ptr<const expr>
	release_expr(void);

	excl_ptr<const terminal>
	release_op(void);
#endif
};	// end class incdec_stmt

//-----------------------------------------------------------------------------
/// class for binary expression statements, with left- and right-hand sides
class assign_stmt : virtual public statement {
protected:
#if 0
	excl_ptrconst <expr>		lhs;		///< destination
	excl_ptr<const terminal>	op;		///< operation
	excl_ptr<const expr>		rhs;		///< source expression
#else
	const expr*		lhs;		///< destination
	const terminal*		op;		///< operation
	const expr*		rhs;		///< source expression
#endif
public:
	assign_stmt(const expr* left, const terminal* o, const expr* right);
#if 0
	assign_stmt(excl_ptr<const expr> left, excl_ptr<const terminal> o,
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

	excl_ptr<const terminal>
	release_op(void);

	excl_ptr<const expr>
	release_rhs(void);
#else
	const expr*
	release_lhs(void);

	const terminal*
	release_op(void);

	const expr*
	release_rhs(void);
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
	def_body_item() : node() { }

virtual	~def_body_item() { }

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;
};	// end class def_body_item

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/// definition body is just a list of definition items
typedef	node_list<const def_body_item>	def_body_item_list;

class definition_body : public def_body_item_list {
protected:
	typedef	def_body_item_list		parent;
public:
	definition_body();

	explicit
	definition_body(const def_body_item* d);

	~definition_body();

};	// end class definition_body

#define definition_body_wrap(b,l,e)					\
	IS_A(definition_body*, l->wrap(b,e))
#define definition_body_append(l,d,n)					\
	IS_A(definition_body*, l->append(d,n))

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
	excl_ptr<const token_keyword>	tag;	///< what language
public:
	explicit
	language_body(const token_keyword* t);

virtual	~language_body();

virtual language_body*
	attach_tag(token_keyword* t);

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const = 0;
};	// end class language_body

//=============================================================================
/// namespace enclosed body
class namespace_body : public root_item {
protected:
	const excl_ptr<const token_keyword>	ns;	///< keyword "namespace"
	const excl_ptr<const token_identifier>	name;	///< name of namespace
	const excl_ptr<const terminal>		lb;
	const excl_ptr<const root_body>		body;	///< contents of namespace
	const excl_ptr<const terminal>		rb;
	const excl_ptr<const terminal>		semi;	///< semicolon token
public:
	namespace_body(const token_keyword* s, const token_identifier* n, 
		const terminal* l, const root_body* b,
		const terminal* r, const terminal* c);

	~namespace_body();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class namespace_body

//-----------------------------------------------------------------------------
/// command to search namespace for identifiers
class using_namespace : public root_item {
protected:
	const excl_ptr<const token_keyword>		open;
	const excl_ptr<const namespace_id>		id;
	const excl_ptr<const token_keyword>		as;
	const excl_ptr<const token_identifier>		alias;
	const excl_ptr<const token_char>		semi;
public:
	using_namespace(const token_keyword* o, const namespace_id* i,
		const token_char* s);
		// a "AS" and n (alias) are optional

	using_namespace(const token_keyword* o, const namespace_id* i, 
		const token_keyword* a, const token_identifier* n,
		const token_char* s);

	~using_namespace();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class using_namespace

//=============================================================================
/**
	Reference to a concrete type, i.e. definition with its
	template parameters specified (if applicable).
 */
class concrete_type_ref : public node {
protected:
	/** definition name base */
	const excl_ptr<const type_base>			base;
	/** optional template arguments */
	const excl_ptr<const expr_list>			temp_spec;
public:
	explicit
	concrete_type_ref(const type_base* n, const expr_list* t = NULL);

	~concrete_type_ref();

	never_ptr<const type_base>
	get_base_def(void) const;

	never_ptr<const expr_list>
	get_temp_spec(void) const;

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class concrete_type_ref

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_BASE_H__

