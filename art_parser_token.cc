// "art_parser_token.cc"
// class method definitions for ART::parser, related to terminal tokens

// template instantiations are at the end of the file

#include <iostream>
#include <stdio.h>		// for sprintf
#include <string.h>		// for a few C-string functions

#include "art_macros.h"		// actually not needed
#include "art_switches.h"
#include "art_parser_debug.h"
#include "art_parser.h"
// #include "art_parser_template_methods.h"	// no templates here
#include "art_symbol_table.h"
#include "art_object.h"
#include "art_object_expr.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

namespace ART {
namespace parser {

//=============================================================================
// class terminal definitions

CONSTRUCTOR_INLINE
terminal::terminal() : node(), pos(current) {
	// "current" is defined in "art_switches.h"
}

DESTRUCTOR_INLINE
terminal::~terminal() { }

/// returns the position in file where this node starts
inline
line_position
terminal::leftmost(void) const {
	return pos;
}

/// returns the position in file where this node ends
inline
line_position
terminal::rightmost(void) const {
	return pos;
}

//=============================================================================
/*** not used
// class token_EOF method definitions

CONSTRUCTOR_INLINE
token_EOF::token_EOF() : terminal() { }

DESTRUCTOR_INLINE
token_EOF::~token_EOF() { }

ostream&
token_EOF::what(ostream& o) const {
	return o << "END-OF-FILE";
}

int
token_EOF::string_compare(const char* d) const {
	return -1;
}
***/
//=============================================================================
// class token_char method definitions

CONSTRUCTOR_INLINE
token_char::token_char(const int i) : terminal(), c(i) { }

DESTRUCTOR_INLINE
token_char::~token_char() { }

/**
	Performs string comparison for a character token.
	\param d string to match against
	\return 0 if match
 */
int
token_char::string_compare(const char* d) const {
	char cs[2] = { c, 0 }; return strcmp(cs,d);
}

ostream&
token_char::what(ostream& o) const {
	return o << (char) c;
}

//=============================================================================
// class token_int method definitions

CONSTRUCTOR_INLINE
token_int::token_int(const long v) : terminal(), expr(), val(v) { }

DESTRUCTOR_INLINE
token_int::~token_int() { }

/**
	Performs string comparison for an integer token.
	\param d string to match against
	\return 0 if match
 */
int
token_int::string_compare(const char* d) const {
	char n[64];
	sprintf(n, "%ld", val);
	return strcmp(n,d);
}

ostream&
token_int::what(ostream& o) const {
	return o << "int: " << val;
}

line_position
token_int::leftmost(void) const {
	return terminal::leftmost();
}

line_position
token_int::rightmost(void) const {
	return terminal::rightmost();
}

const object*
token_int::check_build(context* c) const {
	cerr << "token_int::check_build(): not quite done yet!" << endl;
	return new param_const_int(val);
}


//=============================================================================
// class token_float method definitions

CONSTRUCTOR_INLINE
token_float::token_float(const double v) : terminal(), expr(), val(v) { }

DESTRUCTOR_INLINE
token_float::~token_float() { }


/**
	Performs string comparison for a floating-point token.
	\param d string to match against
	\return 0 if match
 */
int
token_float::string_compare(const char* d) const {
	char n[64];
	sprintf(n, "%f", val);
	return strcmp(n,d);
}

ostream&
token_float::what(ostream& o) const { return o << "float: " << val; }

line_position
token_float::leftmost(void) const {
	return terminal::leftmost();
}

line_position
token_float::rightmost(void) const {
	return terminal::rightmost();
}

/**
	Need built-in float type first.  
 */
const object*
token_float::check_build(context* c) const {
	cerr << "token_float::check_build(): not quite done yet!" << endl;
	return NULL;
}

//=============================================================================
// class token_string method definitions

CONSTRUCTOR_INLINE
token_string::token_string(const char* s) : string(s), terminal() { }

DESTRUCTOR_INLINE
token_string::~token_string() { }

int
token_string::string_compare(const char* d) const { return compare(d); }

ostream&
token_string::what(ostream& o) const {
	return o << "token: " << AS_A(const string&, *this);
}

line_position
token_string::rightmost(void) const {
	return line_position(pos.line, pos.col +length() -1);
}

//=============================================================================
// class token_identifier method definitions

CONSTRUCTOR_INLINE
token_identifier::token_identifier(const char* s) : token_string(s), expr() { }

DESTRUCTOR_INLINE
token_identifier::~token_identifier() { }

ostream&
token_identifier::what(ostream& o) const {
	return o << "identifier: " << AS_A(const string&, *this);
}

line_position
token_identifier::leftmost(void) const {
	return terminal::leftmost();
}

line_position
token_identifier::rightmost(void) const {
	return token_string::rightmost();
}

/**
	MESS ALERT:
	Type-checking for expression literals, 
	not to be used for instantiation and declaration.  
	The identifier must have been instantiated or declared formally
	to pass type-check.  
	Not intended for use of user-defined type identifiers... yet.
	SOLUTION: reserve token_identifier for ONLY instantiations
		and definitions, whereas
		relative and absolute (qualified) identifiers
		should be used in the grammar for all *references*
		to instances.  
	\param c the context of the current position in the syntax tree.  
	\return pointer to the instance named if found, else NULL.  
 */
const object*
token_identifier::check_build(context* c) const {
//	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_identifier::check_build(...): FINISH ME!";
//	)

	// don't look up, instantiate (checked) in the context's current scope!
	return c->lookup_instance(*this).unprotected_const_ptr();
//	return c->lookup_instance(*this);
}

//=============================================================================
// class token_keyword method definitions

CONSTRUCTOR_INLINE
token_keyword::token_keyword(const char* s) : token_string(s) { }

DESTRUCTOR_INLINE
token_keyword::~token_keyword() { }

ostream& 
token_keyword::what(ostream& o) const {
	return o << "keyword: " << AS_A(const string&, *this);
}

//=============================================================================
// class token_bool method definitions

/**
	Constructor.  
	\param tf is either "true" or "false"
 */
CONSTRUCTOR_INLINE
token_bool::
token_bool(const char* tf) : token_keyword(tf), expr() {
	assert(!strcmp(tf,"true") || !strcmp(tf,"false"));
} 

DESTRUCTOR_INLINE
token_bool:: ~token_bool() { }

ostream&
token_bool::what(ostream& o) const {
	return o << "bool: " << AS_A(const string&, *this);
}

line_position
token_bool::leftmost(void) const {
	return terminal::leftmost();
}

line_position
token_bool::rightmost(void) const {
	return token_string::rightmost();
}
const object*
token_bool::check_build(context* c) const {
	cerr << "token_bool::check_build(): not quite done yet!" << endl;
	return new param_const_bool(strcmp(c_str(),"true") == 0);
}


//=============================================================================
// class token_else method definitions

/**
	Constructor.  
	\param e is either "true" or "false"
 */
CONSTRUCTOR_INLINE
token_else::
token_else(const char* e) : token_keyword(e), expr() {
	assert(!strcmp(e,"else"));
} 

DESTRUCTOR_INLINE
token_else:: ~token_else() { }

ostream&
token_else::what(ostream& o) const {
	return token_keyword::what(o);
}

line_position
token_else::leftmost(void) const {
	return terminal::leftmost();
}

line_position
token_else::rightmost(void) const {
	return token_string::rightmost();
}

const object*
token_else::check_build(context* c) const {
	cerr << "token_else::check_build(): Don't call me!";
	return NULL;
}

//=============================================================================
// class token_quoted_string method definitions

CONSTRUCTOR_INLINE
token_quoted_string::token_quoted_string(const char* s) :
	token_string(s), expr() { }

DESTRUCTOR_INLINE
token_quoted_string::~token_quoted_string() { }

ostream&
token_quoted_string::what(ostream& o) const {
	// punt: handle special characters later...
	return ((const token_string*) this)->what(o << "string: \"")
		<< "\"";
}

line_position
token_quoted_string::leftmost(void) const {
	return terminal::leftmost();
}

line_position
token_quoted_string::rightmost(void) const {
	return terminal::rightmost();
}

/**
	Can't do this until we have a built-in type for strings.  
 */
const object*
token_quoted_string::check_build(context* c) const {
	cerr << "token_quoted_string::check_build(): FINISH ME!" << endl;
	return NULL;
}

//=============================================================================
// class token_type method definitions

CONSTRUCTOR_INLINE
token_type::token_type(const char* tf) : token_keyword(tf), type_base() { }

DESTRUCTOR_INLINE
token_type::~token_type() { }

line_position
token_type::leftmost(void) const {
	return token_keyword::leftmost();
}

line_position
token_type::rightmost(void) const {
	return token_keyword::rightmost();
}

//=============================================================================
// class token_datatype method definitions

CONSTRUCTOR_INLINE
token_datatype::token_datatype(const char* dt) : token_type(dt) { }

DESTRUCTOR_INLINE
token_datatype::~token_datatype() { }

ostream&
token_datatype::what(ostream& o) const {
	return o << "datatype: " << AS_A(const string&, *this);
}

const object*
token_datatype::check_build(context* c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_datatype::check_build(...): ";
	)
	return c->set_datatype_def(*this).unprotected_const_ptr();
//	return c->set_datatype_def(*this);
}

//=============================================================================
// class token_paramtype method definitions

CONSTRUCTOR_INLINE
token_paramtype::token_paramtype(const char* dt) : token_type(dt) { }

DESTRUCTOR_INLINE
token_paramtype::~token_paramtype() { }

ostream&
token_paramtype::what(ostream& o) const {
	return o << "paramtype: " << AS_A(const string&, *this);
}

const object*
token_paramtype::check_build(context* c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_paramtype::check_build(...): ";
	)
	return c->set_param_def(*this).unprotected_const_ptr();
//	return c->set_param_def(*this);
}

//=============================================================================
};	// end namespace parser
};	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


