// "art_parser_token.cc"
// class method definitions for ART::parser, related to terminal tokens

// template instantiations are at the end of the file

#include <iostream>
#include <stdio.h>		// for sprintf
#include <string.h>		// for a few C-string functions

#include "art_macros.h"		// actually not needed
#include "art_switches.h"
#include "art_parser.h"
// #include "art_parser_template_methods.h"	// no templates here

// will need these come time for type checking
// #include "art_symbol_table.h"
// #include "art_object.h"

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
	return o << "token: " << (const string&) (*this);
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
	return o << "identifier: " << (const string&) (*this);
}

line_position
token_identifier::leftmost(void) const {
	return terminal::leftmost();
}

line_position
token_identifier::rightmost(void) const {
	return token_string::rightmost();
}

//=============================================================================
// class token_keyword method definitions

CONSTRUCTOR_INLINE
token_keyword::token_keyword(const char* s) : token_string(s) { }

DESTRUCTOR_INLINE
token_keyword::~token_keyword() { }

ostream& 
token_keyword::what(ostream& o) const {
	return o << "keyword: " << *((const string*) this);
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
	return o << "bool: " << *((const string*) this);
}

line_position
token_bool::leftmost(void) const {
	return terminal::leftmost();
}

line_position
token_bool::rightmost(void) const {
	return token_string::rightmost();
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
	return o << "keyword: " << *((const string*) this);
}

line_position
token_else::leftmost(void) const {
	return terminal::leftmost();
}

line_position
token_else::rightmost(void) const {
	return token_string::rightmost();
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

//=============================================================================
// class token_type method definitions

CONSTRUCTOR_INLINE
token_type::token_type(const char* tf) : token_keyword(tf), type_base() { }

DESTRUCTOR_INLINE
token_type::~token_type() { }

ostream&
token_type::what(ostream& o) const {
	return o << "type: " << *((const string*) this);
}

line_position
token_type::leftmost(void) const {
	return token_keyword::leftmost();
}

line_position
token_type::rightmost(void) const {
	return token_keyword::rightmost();
}

//=============================================================================
};
};

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


