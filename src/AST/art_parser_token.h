/**
	\file "art_parser_token.h"
	Token-specific parser classes for ART.  
	$Id: art_parser_token.h,v 1.5 2004/12/05 05:06:50 fang Exp $
 */

#ifndef __ART_PARSER_TOKEN_H__
#define __ART_PARSER_TOKEN_H__

#include <iosfwd>
#include <string>

#include "art_parser_base.h"
#include "art_parser_expr.h"

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

using std::ostream;
using namespace entity;
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
// forward declarations in this namespace
	class expr;		// family defined in "art_parser_expr.h"
	class id_expr;
	class token_char;	// defined here
	class token_string;	// defined here
	class qualified_id;	// defined here
	class concrete_type_ref;	// defined here
	class context;		// defined in "art_symbol_table.h"

//=============================================================================
// class terminal is defined in "art_parser.h"
// class token_string is defined in "art_parser.h"
// class token_identifier is defined in "art_parser.h"
// class token_char is defined in "art_parser.h"

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
	never_ptr<const object> check_build(never_ptr<context> c) const;
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
	never_ptr<const object> check_build(never_ptr<context> c) const;
};	// end class token_float

//=============================================================================
#if 0
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
// virtual	never_ptr<const object> check_build(never_ptr<context> c) const;
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
	never_ptr<const object> check_build(never_ptr<context> c) const;
};	// end class token_identifier
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
virtual	never_ptr<const object> check_build(never_ptr<context> c) const;

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
class qualified_id_slice : public sublist<count_ptr<const token_identifier> > {
protected:
	typedef	sublist<count_ptr<const token_identifier> >	parent;
protected:
	const bool	 absolute;
public:
	/**
		Constructor (implicit) that take a plain qualified_id.  
		Works because qualified_id is a subclass of
		list<excl_ptr<const token_identifier> >.  
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
#endif

//-----------------------------------------------------------------------------
#if 0
/// keyword version of token_string class, not necessarily an expr
class token_keyword : public token_string {
public:
	token_keyword(const char* s);
virtual	~token_keyword();

virtual	ostream& what(ostream& o) const;
};	// end class token_keyword
#endif

//-----------------------------------------------------------------------------
/// class for expression keywords, which happen to be only bools
class token_bool : public token_keyword, public expr {
public:
	token_bool(const char* tf);
	~token_bool();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_ptr<const object> check_build(never_ptr<context> c) const;
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
	never_ptr<const object> check_build(never_ptr<context> c) const;
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
	never_ptr<const object> check_build(never_ptr<context> c) const;
};	// end class token_quoted_string

//=============================================================================
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
virtual	never_ptr<const object> check_build(never_ptr<context> c) const = 0;
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
virtual	never_ptr<const object> check_build(never_ptr<context> c) const = 0;
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
	never_ptr<const object> check_build(never_ptr<context> c) const;
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
	never_ptr<const object> check_build(never_ptr<context> c) const;
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
virtual	never_ptr<const object> check_build(never_ptr<context> c) const = 0;
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
	never_ptr<const object> check_build(never_ptr<context> c) const;
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
	never_ptr<const object> check_build(never_ptr<context> c) const;
};	// end class token_pint_type

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_TOKEN_H__

