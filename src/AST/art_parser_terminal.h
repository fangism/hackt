/**
	\file "art_parser_terminal.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_terminal.h,v 1.1.10.1 2005/03/06 04:19:31 fang Exp $
 */

#ifndef __ART_PARSER_TERMINAL_H__
#define __ART_PARSER_TERMINAL_H__

#include <string>
#include "art_lex.h"
#include "art_parser_expr_base.h"
// #include "memory/list_vector_pool_fwd.h"
#include "memory/chunk_map_pool_fwd.h"

//=============================================================================
namespace ART {
//=============================================================================
// global variable
namespace lexer {
extern	token_position current;
}

using lexer::current;			// current token position
using std::string;

namespace parser {
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

//	LIST_VECTOR_POOL_DEFAULT_STATIC_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
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
}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_TERMINAL_H__

