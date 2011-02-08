/**
	\file "AST/token_string.h"
	Base set of classes for the HAC parser.  
	These classes are implemented in "AST/token.cc"
	$Id: token_string.h,v 1.4 2011/02/08 02:06:47 fang Exp $
	This file used to be the following before it was renamed:
	$Id: token_string.h,v 1.4 2011/02/08 02:06:47 fang Exp $
 */

#ifndef __HAC_AST_TOKEN_STRING_H__
#define __HAC_AST_TOKEN_STRING_H__

#include "AST/terminal.h"
#include "AST/expr_base.h"
#include "util/memory/chunk_map_pool_fwd.h"

//=============================================================================
namespace HAC {
namespace parser {
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
	explicit
	token_string(const string& s) : string(s), terminal() { }

#if 0
	/// default copy-constructor
	token_string(const token_string& s) : string(s), terminal() { }
#endif

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
class token_identifier : public token_string, public inst_ref_expr {
	typedef	token_identifier	this_type;
public:
	explicit
	token_identifier(const char* s) : token_string(s), inst_ref_expr() { }

	token_identifier(const token_identifier& i) :
		token_string(i), inst_ref_expr() { }

	~token_identifier() { }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_REFERENCE_PROTO;
	CHECK_NONMETA_REFERENCE_PROTO;
	EXPAND_CONST_REFERENCE_PROTO;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
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

virtual ostream&
	dump(ostream&) const;

};	// end class token_keyword

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_TOKEN_STRING_H__

