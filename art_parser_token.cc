// "art_parser_token.cc"
// class method definitions for ART::parser, related to terminal tokens

// template instantiations are at the end of the file

#include <iostream>
#include <stdio.h>		// for sprintf
#include <string.h>		// for a few C-string functions

// #include "art_macros.h"		// actually not needed
#include "art_switches.h"
#include "art_parser_debug.h"

#include "art_parser_token.h"

#include "art_symbol_table.h"
#include "art_object_expr.h"
#include "art_built_ins.h"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		


namespace ART {
using namespace entity;

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
#if 0
not used
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
#endif

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

/**
	Pushes a parameter expression (constant int) onto the 
	context's expression stack.  
	\return NULL, useless.  
 */
never_const_ptr<object>
token_int::check_build(never_ptr<context> c) const {
	count_ptr<pint_const> pe(new pint_const(val));
	assert(pe);
	c->push_object_stack(pe);
	return never_const_ptr<object>(NULL);
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
never_const_ptr<object>
token_float::check_build(never_ptr<context> c) const {
	cerr << "token_float::check_build(): not quite done yet!" << endl;
	return never_const_ptr<object>(NULL);
}

//=============================================================================
// class token_string method definitions

CONSTRUCTOR_INLINE
token_string::token_string(const char* s) : string(s), terminal() { }

token_string::token_string(const token_string& s) :
		node(), string(s), terminal() {
}

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

/** default copy constructor */
token_identifier::token_identifier(const token_identifier& i) :
		node(), token_string(i), expr() {
}

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
	Type-checking for expression literals and instance references, 
	not to be called from declarations.  

	(Later use "new_identifier" for declarations and definitions.)
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
never_const_ptr<object>
token_identifier::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_identifier::check_build(...)";
	)

	// don't look up, instantiate (checked) in the context's current scope!
	never_const_ptr<instantiation_base> inst(c->lookup_instance(*this));
	// problem: stack is count_ptr, incompatible with never_ptr
	if (inst) {
		// we will then make an instance_reference
		// what about indexed instance references?
		inst->make_instance_reference(*c);
		// already pushes onto context's object_stack
		// useless return value, always NULL
	} else {
		// push a NULL placeholder
		c->push_object_stack(count_ptr<object>(NULL));
		// better error handling later...
		what(cerr << "failed to find ") << endl;
		exit(1);		// temporary termination
	}
	return inst;
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

never_const_ptr<object>
token_bool::check_build(never_ptr<context> c) const {
	count_ptr<param_expr> pe(
		new pbool_const(strcmp(c_str(),"true") == 0));
	c->push_object_stack(pe);
	return never_const_ptr<object>(NULL);
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

never_const_ptr<object>
token_else::check_build(never_ptr<context> c) const {
	cerr << "token_else::check_build(): Don't call me!";
	return never_const_ptr<object>(NULL);
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
never_const_ptr<object>
token_quoted_string::check_build(never_ptr<context> c) const {
	cerr << "token_quoted_string::check_build(): FINISH ME!" << endl;
	return never_const_ptr<object>(NULL);
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

#if 0
never_const_ptr<object>
token_datatype::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_datatype::check_build(...): ";
	)
	return c->set_datatype_def(*this);
}
#endif

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

#if 0
never_const_ptr<object>
token_paramtype::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_paramtype::check_build(...): ";
	)
	return c->set_param_def(*this);
}
#endif

//=============================================================================
// class token_bool_type method definitions

CONSTRUCTOR_INLINE
token_bool_type::token_bool_type(const char* dt) : token_datatype(dt) { }

DESTRUCTOR_INLINE
token_bool_type::~token_bool_type() { }

#if 0
ostream&
token_bool_type::what(ostream& o) const {
	return o << "bool_type: " << AS_A(const string&, *this);
}
#endif

never_const_ptr<object>
token_bool_type::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_bool_type::check_build(...): ";
	)
	// bool_def declared in "art_built_ins.h"
	return c->set_current_definition_reference(bool_def);
}

//=============================================================================
// class token_int_type method definitions

CONSTRUCTOR_INLINE
token_int_type::token_int_type(const char* dt) : token_datatype(dt) { }

DESTRUCTOR_INLINE
token_int_type::~token_int_type() { }

#if 0
ostream&
token_int_type::what(ostream& o) const {
	return o << "int_type: " << AS_A(const string&, *this);
}
#endif

never_const_ptr<object>
token_int_type::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_int_type::check_build(...): ";
	)
	// int_def declared in "art_built_ins.h"
	return c->set_current_definition_reference(int_def);
}

//=============================================================================
// class token_pbool_type method definitions

CONSTRUCTOR_INLINE
token_pbool_type::token_pbool_type(const char* dt) : token_paramtype(dt) { }

DESTRUCTOR_INLINE
token_pbool_type::~token_pbool_type() { }

#if 0
ostream&
token_pbool_type::what(ostream& o) const {
	return o << "pbool_type: " << AS_A(const string&, *this);
}
#endif

/**
	Return pointer to the definition, 
	the caller (concrete_type_ref) should convert it to the appropriate
	concrete type reference.  
	"pbool" is always used as a type_reference, and never refers
	to the definition.  
 */
never_const_ptr<object>
token_pbool_type::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_pbool_type::check_build(...): ";
	)
	// pbool_def declared in "art_built_ins.h"
//	return c->set_current_fundamental_type(pbool_type);
	return c->set_current_definition_reference(pbool_def);
}

//=============================================================================
// class token_pint_type method definitions

CONSTRUCTOR_INLINE
token_pint_type::token_pint_type(const char* dt) : token_paramtype(dt) { }

DESTRUCTOR_INLINE
token_pint_type::~token_pint_type() { }

#if 0
ostream&
token_pint_type::what(ostream& o) const {
	return o << "pint_type: " << AS_A(const string&, *this);
}
#endif

/**
	Let caller resolve to concrete type reference with the definition.  
//	"pint" is always used as a type_reference, and never refers
//	to the definition.  
 */
never_const_ptr<object>
token_pint_type::check_build(never_ptr<context> c) const {
	TRACE_CHECK_BUILD(
		what(cerr << c->auto_indent())
			<< "token_pint_type::check_build(...): ";
	)
	// pint_def declared in "art_built_ins.h"
//	return c->set_current_fundamental_type(pint_type);
	return c->set_current_definition_reference(pint_def);
}

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE


