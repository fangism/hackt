/**
	\file "art_parser_token.h"
	Token-specific parser classes for ART.  
	$Id: art_parser_token.h,v 1.6 2004/12/07 02:22:03 fang Exp $
 */

#ifndef __ART_PARSER_TOKEN_H__
#define __ART_PARSER_TOKEN_H__

#include <iosfwd>
#include <string>

#include "art_parser_base.h"
#include "art_parser_expr.h"

namespace ART {

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

namespace parser {
// forward declarations in this namespace
class expr;			// family defined in "art_parser_expr.h"
class id_expr;
class token_char;		// defined here
class token_string;		// defined here
class qualified_id;		// defined here
class concrete_type_ref;	// defined here
class context;			// defined in "art_symbol_table.h"

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
explicit token_int(const long v) : terminal(), expr(), val(v) { }
/// standard virtual destructor
	~token_int() { }

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
explicit token_float(const double v) : terminal(), expr(), val(v) { }
/// standard virtual destructor
	~token_float() { }

	int string_compare(const char* d) const;
	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_ptr<const object> check_build(never_ptr<context> c) const;
};	// end class token_float

//=============================================================================
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

