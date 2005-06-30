/**
	\file "AST/art_parser_token.cc"
	Class method definitions for ART::parser, related to terminal tokens.
	$Id: art_parser_token.cc,v 1.32.2.1 2005/06/30 23:22:13 fang Exp $
 */

#ifndef	__AST_ART_PARSER_TOKEN_CC__
#define	__AST_ART_PARSER_TOKEN_CC__

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <ostream>
#include <cstdio>		// for sprintf
#include <cstring>		// for a few C-string functions
#include <exception>

#include "AST/art_parser_token.h"
#include "AST/art_parser_token_char.h"
#include "AST/art_parser_token_string.h"

#include "Object/art_context.h"
#include "Object/art_object_definition_data.h"
#include "Object/art_object_instance_base.h"
#include "Object/art_object_inst_ref_base.h"
#include "Object/art_object_expr_const.h"
#include "Object/art_built_ins.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_pint_traits.h"
#include "Object/art_object_pbool_traits.h"

#include "util/what.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"
#include "util/memory/chunk_map_pool.tcc"

// enable or disable constructor inlining, undefined at the end of file
// leave blank do disable, define as inline to enable
#define	CONSTRUCTOR_INLINE		
#define	DESTRUCTOR_INLINE		

// for specializing util::what
#if 0
// not really needed in this module
namespace util {
// SPECIALIZE_UTIL_WHAT(ART::parser::token_EOF, "END-OF-FILE")
SPECIALIZE_UTIL_WHAT(ART::parser::token_char, "char")
SPECIALIZE_UTIL_WHAT(ART::parser::token_int, "int")
SPECIALIZE_UTIL_WHAT(ART::parser::token_float, "float")
SPECIALIZE_UTIL_WHAT(ART::parser::token_string, "token")
SPECIALIZE_UTIL_WHAT(ART::parser::token_identifier, "identifier")
SPECIALIZE_UTIL_WHAT(ART::parser::token_keyword, "keyword")
SPECIALIZE_UTIL_WHAT(ART::parser::token_bool, "bool")
SPECIALIZE_UTIL_WHAT(ART::parser::token_else, "else")
SPECIALIZE_UTIL_WHAT(ART::parser::token_quoted_string, "quoted-string")
SPECIALIZE_UTIL_WHAT(ART::parser::token_datatype, "datatype")
SPECIALIZE_UTIL_WHAT(ART::parser::token_paramtype, "paramtype")
}
#endif

#if 0
namespace util {
namespace memory {
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::parser::token_char)
}
}
#endif

namespace ART {
namespace parser {
#include "util/using_ostream.h"
USING_STACKTRACE
using entity::pint_const;
using entity::pbool_const;

// these are built-in instance references, not types.  
#if 0
using entity::pbool_def;
using entity::pint_def;
#else
using entity::class_traits;
using entity::pint_tag;
using entity::pbool_tag;
#endif
using entity::bool_def;
using entity::int_def;

//=============================================================================
// class terminal definitions

#if 0
CONSTRUCTOR_INLINE
terminal::terminal() : node(), pos(current) {
	// "current" is defined in "art_switches.h"
}

DESTRUCTOR_INLINE
terminal::~terminal() { }
#endif

void
terminal::bogus(void) const { }

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

inline
token_char::token_char() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
CONSTRUCTOR_INLINE
token_char::token_char(const int i) : terminal(), c(i) { }

DESTRUCTOR_INLINE
token_char::~token_char() { }
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_char)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Performs string comparison for a character token.
	\param d string to match against
	\return 0 if match
 */
int
token_char::string_compare(const char* d) const {
	const char cs[2] = { c, 0 };
	return strcmp(cs,d);
}

ostream&
token_char::what(ostream& o) const {
	return o << char(c);
}

//=============================================================================
// class token_int method definitions

#if 0
CONSTRUCTOR_INLINE
token_int::token_int(const long v) : terminal(), expr(), val(v) { }

DESTRUCTOR_INLINE
token_int::~token_int() { }
#endif

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_int)

/**
	Performs string comparison for an integer token.
	TODO: use ostringstream instead...
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return newly created constant integer value.  
 */
expr::meta_return_type
token_int::check_meta_expr(context& c) const {
	return expr::meta_return_type(new pint_const(val));
}

//=============================================================================
// class token_float method definitions

#if 0
CONSTRUCTOR_INLINE
token_float::token_float(const double v) : terminal(), expr(), val(v) { }

DESTRUCTOR_INLINE
token_float::~token_float() { }
#endif

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_float)

/**
	Performs string comparison for a floating-point token.
	TODO: use ostringstream instead...
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
token_float::what(ostream& o) const {
	return o << "float: " << val;
}

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
expr::meta_return_type
token_float::check_meta_expr(context& c) const {
	cerr << "token_float::check_meta_expr(): not quite done yet!" << endl;
	return expr::meta_return_type(NULL);
}

//=============================================================================
// class token_string method definitions

#if 0
CONSTRUCTOR_INLINE
token_string::token_string(const char* s) : string(s), terminal() { }

token_string::token_string(const token_string& s) :
		node(), string(s), terminal() {
}

DESTRUCTOR_INLINE
token_string::~token_string() { }
#endif

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

#if 0
CONSTRUCTOR_INLINE
token_identifier::token_identifier(const char* s) :
		token_string(s), inst_ref_expr() { }

/** default copy constructor */
token_identifier::token_identifier(const token_identifier& i) :
		token_string(i), inst_ref_expr() {
}

DESTRUCTOR_INLINE
token_identifier::~token_identifier() { }
#endif

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_identifier)

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is used specifically to return param_expr.  
	Another version will return meta_instance_references.  
	\param c the context of the current position in the syntax tree.  
	\return pointer to the instance named if found, else NULL.  
 */
inst_ref_expr::meta_return_type
token_identifier::check_meta_reference(context& c) const {
	typedef	inst_ref_expr::meta_return_type		return_type;
	STACKTRACE("token_identifier::check_meta_expr()");

	// don't look up, instantiate (checked) in the context's current scope!
	const never_ptr<const instance_collection_base>
		inst(c.lookup_instance(*this));
	// problem: stack is count_ptr, incompatible with never_ptr
	if (inst) {
		return inst->make_meta_instance_reference();
	} else {
		// better error handling later...
		what(cerr << "failed to find ") << endl;
		THROW_EXIT;		// temporary termination
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is used specifically to return a data_expr.  
	Another version will return meta_instance_references.  
	\param c the context of the current position in the syntax tree.  
	\return pointer to the instance named if found, else NULL.  
 */
inst_ref_expr::nonmeta_return_type
token_identifier::check_nonmeta_reference(context& c) const {
	typedef	inst_ref_expr::nonmeta_return_type	return_type;
	STACKTRACE("token_identifier::check_nonmeta_expr()");

	// don't look up, instantiate (checked) in the context's current scope!
	const never_ptr<const instance_collection_base>
		inst(c.lookup_instance(*this));
	// problem: stack is count_ptr, incompatible with never_ptr
	if (inst) {
		return inst->make_nonmeta_instance_reference();
	} else {
		// better error handling later...
		what(cerr << "failed to find ") << endl;
		THROW_EXIT;		// temporary termination
		return return_type(NULL);
	}
}

//=============================================================================
// class token_keyword method definitions

#if 0
CONSTRUCTOR_INLINE
token_keyword::token_keyword(const char* s) : token_string(s) { }

DESTRUCTOR_INLINE
token_keyword::~token_keyword() { }
#endif

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
	INVARIANT(!strcmp(tf,"true") || !strcmp(tf,"false"));
} 

DESTRUCTOR_INLINE
token_bool:: ~token_bool() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_bool)

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

expr::meta_return_type
token_bool::check_meta_expr(context& c) const {
	return expr::meta_return_type(new pbool_const(strcmp(c_str(),"true") == 0));
}

//=============================================================================
// class token_else method definitions

/**
	Constructor.  
	\param e is either "true" or "false"
 */
CONSTRUCTOR_INLINE
token_else::token_else(const char* e) : token_keyword(e), expr() {
	INVARIANT(!strcmp(e,"else"));
} 

DESTRUCTOR_INLINE
token_else:: ~token_else() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_else)

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

never_ptr<const object>
token_else::check_build(context& c) const {
	cerr << "token_else::check_build(): Don't call me!";
	return never_ptr<const object>(NULL);
}

expr::meta_return_type
token_else::check_meta_expr(context& c) const {
	cerr << "token_else::check_build(): Don't call me!";
	return expr::meta_return_type(NULL);
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
	return static_cast<const token_string*>(this)->what(o << "string: \"") << "\"";
//	return token_string::what(o << "string: \"") << "\"";
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
never_ptr<const object>
token_quoted_string::check_build(context& c) const {
	cerr << "token_quoted_string::check_build(): FINISH ME!" << endl;
	return never_ptr<const object>(NULL);
}

expr::meta_return_type
token_quoted_string::check_meta_expr(context& c) const {
	cerr << "token_quoted_string::check_meta_expr(): FINISH ME!" << endl;
	return expr::meta_return_type(NULL);
}

//=============================================================================
// class token_type method definitions

CONSTRUCTOR_INLINE
token_type::token_type(const char* tf) : token_keyword(tf) { }

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
token_datatype::token_datatype(const char* dt) :
		token_type(dt), type_base() { }

DESTRUCTOR_INLINE
token_datatype::~token_datatype() { }

ostream&
token_datatype::what(ostream& o) const {
	return o << "datatype: " << AS_A(const string&, *this);
}

//=============================================================================
// class token_paramtype method definitions

CONSTRUCTOR_INLINE
token_paramtype::token_paramtype(const char* dt) :
		token_type(dt), concrete_type_ref() { }

DESTRUCTOR_INLINE
token_paramtype::~token_paramtype() { }

ostream&
token_paramtype::what(ostream& o) const {
	return o << "paramtype: " << AS_A(const string&, *this);
}

//=============================================================================
// class token_bool_type method definitions

CONSTRUCTOR_INLINE
token_bool_type::token_bool_type(const char* dt) : token_datatype(dt) { }

DESTRUCTOR_INLINE
token_bool_type::~token_bool_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_bool_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
type_base::return_type
token_bool_type::check_definition(context& c) const {
	STACKTRACE("token_bool_type::check_build()");
	// bool_def declared in "art_built_ins.h"
	// safe to use never_ptr on address of statically allocated definition
	return type_base::return_type(&bool_def);
}

//=============================================================================
// class token_int_type method definitions

CONSTRUCTOR_INLINE
token_int_type::token_int_type(const char* dt) : token_datatype(dt) { }

DESTRUCTOR_INLINE
token_int_type::~token_int_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_int_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
type_base::return_type
token_int_type::check_definition(context& c) const {
	STACKTRACE("token_int_type::check_build()");
	// int_def declared in "art_built_ins.h"
	// safe to use never_ptr on address of statically allocated definition
	return type_base::return_type(&int_def);
}

//=============================================================================
// class token_pbool_type method definitions

CONSTRUCTOR_INLINE
token_pbool_type::token_pbool_type(const char* dt) : token_paramtype(dt) { }

DESTRUCTOR_INLINE
token_pbool_type::~token_pbool_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_pbool_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Return pointer to the definition, 
	the caller (concrete_type_ref) should convert it to the appropriate
	concrete type reference.  
	"pbool" is always used as a type_reference, and never refers
	to the definition.  
 */
type_base::return_type
token_pbool_type::check_definition(context& c) const {
	STACKTRACE("token_pbool_type::check_build()");
	// pbool_def declared in "art_built_ins.h"
	return type_base::return_type(&pbool_def);
}
#else

concrete_type_ref::return_type
token_pbool_type::check_type(context&) const {
	static const class_traits<pbool_tag>::type_ref_ptr_type&
		pbool_type_ptr(class_traits<pbool_tag>::built_in_type_ptr);
	return pbool_type_ptr;
}

#endif

//=============================================================================
// class token_pint_type method definitions

CONSTRUCTOR_INLINE
token_pint_type::token_pint_type(const char* dt) : token_paramtype(dt) { }

DESTRUCTOR_INLINE
token_pint_type::~token_pint_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_pint_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	Let caller resolve to concrete type reference with the definition.  
//	"pint" is always used as a type_reference, and never refers
//	to the definition.  
 */
type_base::return_type
token_pint_type::check_definition(context& c) const {
	STACKTRACE("token_pint_type::check_build()");
	// pint_def declared in "art_built_ins.h"
	return type_base::return_type(&pint_def);
}
#else

concrete_type_ref::return_type
token_pint_type::check_type(context&) const {
	static const class_traits<pint_tag>::type_ref_ptr_type&
		pint_type_ptr(class_traits<pint_tag>::built_in_type_ptr);
	return pint_type_ptr;
}

#endif

//=============================================================================
}	// end namespace parser
}	// end namespace ART

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

DEFAULT_STATIC_TRACE_END

#endif	// __AST_ART_PARSER_TOKEN_CC__

