/**
	\file "AST/token.hh"
	Token-specific parser classes for HAC.  
	$Id: token.hh,v 1.10 2011/02/08 02:06:46 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_token.h,v 1.17.34.1 2005/12/11 00:45:11 fang Exp
 */

#ifndef __HAC_AST_TOKEN_H__
#define __HAC_AST_TOKEN_H__

#include "AST/token_string.hh"
#include "AST/type_base.hh"
#include "Object/expr/types.hh"		// for typedefs
#include "util/memory/chunk_map_pool_fwd.hh"

namespace HAC {
namespace parser {
using entity::pint_value_type;
using entity::pbool_value_type;
using entity::preal_value_type;
using entity::pstring_value_type;

//=============================================================================
// forward declarations in this namespace appear in "AST/AST_fwd.hh"

// class terminal is defined in "AST/terminal.hh"
// class token_string is defined in "AST/token_string.hh"
// class token_identifier is defined in "AST/token_string.hh"
// class token_char is defined in "AST/token_char.hh"

//=============================================================================
/// stores an integer (long) in native form and retains position information
class token_int : public terminal, public expr {
	typedef	token_int		this_type;
protected:
	pint_value_type			val;	///< the value
public:
/// standard constructor
	explicit
	token_int(const pint_value_type v) : terminal(), expr(), val(v) { }

/// standard destructor
	~token_int() { }

	int
	string_compare(const char* d) const;

	pint_value_type
	value(void) const { return val; }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class token_int

//=============================================================================
/// stores an float (double) in native form and retains position information
class token_float : public terminal, public expr {
	typedef	token_float		this_type;
protected:
	preal_value_type		val;	///< the value
public:
/// standard constructor
	explicit
	token_float(const preal_value_type v) : terminal(), expr(), val(v) { }

/// standard destructor
	~token_float() { }

	int
	string_compare(const char* d) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(16)
};	// end class token_float

//=============================================================================
/// class for expression keywords, which happen to be only bools
class token_bool : public token_keyword, public expr {
	typedef	token_bool		this_type;
public:
	explicit
	token_bool(const char* tf);

	~token_bool();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	CHECK_META_EXPR_PROTO;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class token_bool

//-----------------------------------------------------------------------------
/// class for "else" keyword, which is a legitimate expr
class token_else : public token_keyword, public expr {
	typedef	token_else		this_type;
public:
	explicit
	token_else(const char* tf);

	~token_else();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
	
	never_ptr<const object>
	check_build(context& c) const;

	CHECK_META_EXPR_PROTO;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(8)
};	// end class token_else

//-----------------------------------------------------------------------------
/// quoted-string version of token_string class
class token_quoted_string : public token_string, public expr {
public:
	explicit
	token_quoted_string(const string& s);
	// pstring_value_type

	~token_quoted_string();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

// not until we have built-in type for string
	never_ptr<const object>
	check_build(context& c) const;

	CHECK_META_EXPR_PROTO;
	CHECK_NONMETA_EXPR_PROTO;

};	// end class token_quoted_string

//=============================================================================
/**
	Abstract base class for keywords that correspond to built-in types, 
	sub-classed into data-types and parameter types).  
 */
class token_type : public token_keyword {
public:
	explicit
	token_type(const char* tf);

virtual	~token_type();

// just use parent's
virtual	ostream&
	what(ostream& o) const = 0;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

};	// end class token_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "int" and "bool" data types.
 */
class token_datatype : public token_type, public type_base {
public:
	explicit
	token_datatype(const char* dt);

virtual	~token_datatype();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream&) const;

	line_position
	leftmost(void) const { return token_type::leftmost(); }

	line_position
	rightmost(void) const { return token_type::rightmost(); }

virtual	TYPE_BASE_CHECK_PROTO = 0;
};	// end class token_datatype

//-----------------------------------------------------------------------------
/**
	Class for built-in "int" data type.
 */
class token_int_type : public token_datatype {
	typedef	token_int_type		this_type;
	bool				atomic;
public:
	explicit
	token_int_type(const char* dt);

	~token_int_type();

	void
	flag_atomic(void) { atomic = true; }	// eint

	bool
	is_atomic(void) const { return atomic; }

	TYPE_BASE_CHECK_PROTO;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class token_int_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "bool" data type.
 */
class token_bool_type : public token_datatype {
	typedef	token_bool_type		this_type;
	bool				atomic;
public:
	explicit
	token_bool_type(const char* dt);

	~token_bool_type();

	void
	flag_atomic(void) { atomic = true; }	// ebool

	bool
	is_atomic(void) const { return atomic; }

	TYPE_BASE_CHECK_PROTO;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class token_bool_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "pint" and "pbool" and "preal" parameter types.
 */
class token_paramtype : public token_type, public concrete_type_ref {
public:
	explicit
	token_paramtype(const char* dt);

virtual	~token_paramtype();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const { return token_type::leftmost(); }

	line_position
	rightmost(void) const { return token_type::rightmost(); }

};	// end class token_paramtype

//-----------------------------------------------------------------------------
/**
	Class for built-in "pbool" parameter type.
 */
class token_pbool_type : public token_paramtype {
	typedef	token_pbool_type	this_type;
public:
	explicit
	token_pbool_type(const char* dt);

	~token_pbool_type();

	return_type
	check_type(const context&) const;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class token_pbool_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "pint" parameter type.
 */
class token_pint_type : public token_paramtype {
	typedef	token_pint_type		this_type;
public:
	explicit
	token_pint_type(const char* dt);

	~token_pint_type();

	return_type
	check_type(const context&) const;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class token_pint_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "preal" parameter type.
 */
class token_preal_type : public token_paramtype {
	typedef	token_preal_type		this_type;
public:
	explicit
	token_preal_type(const char* dt);

	~token_preal_type();

	return_type
	check_type(const context&) const;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class token_preal_type

//-----------------------------------------------------------------------------
/**
	Class for built-in "pstring" parameter type.
 */
class token_pstring_type : public token_paramtype {
	typedef	token_pstring_type		this_type;
public:
	explicit
	token_pstring_type(const char* dt);

	~token_pstring_type();

	return_type
	check_type(const context&) const;

	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(32)
};	// end class token_preal_type

//=============================================================================

}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_TOKEN_H__

