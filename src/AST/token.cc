/**
	\file "AST/token.cc"
	Class method definitions for HAC::parser, related to terminal tokens.
	$Id: token.cc,v 1.17 2011/02/08 02:06:46 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_token.cc,v 1.36.4.1 2005/12/11 00:45:11 fang Exp
 */

#ifndef	__HAC_AST_TOKEN_CC__
#define	__HAC_AST_TOKEN_CC__

#define	ENABLE_STACKTRACE		0
#define	ENABLE_STATIC_TRACE		0
// b/c this module has memory pools to initialize
#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <ostream>
#include <cstdio>		// for sprintf
#include <cstring>		// for a few C-string functions
#include <exception>

#include "AST/token.hh"
#include "AST/token_char.hh"
#include "AST/token_string.hh"
#include "AST/parse_context.hh"

#include "Object/def/built_in_datatype_def.hh"
#include "Object/ref/meta_instance_reference_base.hh"
#include "Object/inst/physical_instance_placeholder.hh"
#include "Object/inst/param_value_placeholder.hh"
#include "Object/ref/meta_reference_union.hh"
#include "Object/ref/nonmeta_instance_reference_base.hh"
#include "Object/common/namespace.hh"	// for dynamic_cast
#include "Object/expr/pint_const.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/pstring_const.hh"
#include "Object/type/param_type_reference.hh"
#include "Object/traits/value_traits.hh"
#include "Object/traits/int_traits.hh"
#include "Object/traits/bool_traits.hh"

#include "common/TODO.hh"
#include "util/what.hh"
#include "util/libc.h"
#include "util/sstream.hh"
#include "util/stacktrace.hh"
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
// SPECIALIZE_UTIL_WHAT(HAC::parser::token_EOF, "END-OF-FILE")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_char, "char")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_int, "int")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_float, "float")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_string, "token")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_identifier, "identifier")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_keyword, "keyword")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_bool, "bool")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_else, "else")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_quoted_string, "quoted-string")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_datatype, "datatype")
SPECIALIZE_UTIL_WHAT(HAC::parser::token_paramtype, "paramtype")
}
#endif

#if 0
namespace util {
namespace memory {
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(HAC::parser::token_char)
}
}
#endif
namespace util {
namespace memory {
// explicit template instantiation
template class count_ptr<const HAC::parser::token_identifier>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace parser {
#include "util/using_ostream.hh"
using std::ostringstream;
using entity::pint_const;
using entity::pbool_const;
using entity::preal_const;

// these are built-in instance references, not types.  
using entity::pbool_traits;
using entity::pint_traits;
using entity::preal_traits;
using entity::pstring_traits;
using entity::bool_traits;
using entity::int_traits;
using entity::physical_instance_placeholder;
using entity::param_value_placeholder;
using entity::pstring_const;

//=============================================================================
// class terminal definitions

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
CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_char)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Performs string comparison for a character token.
	\param d string to match against
	\return 0 if match
 */
int
token_char::string_compare(const char* d) const {
	const char cs[2] = { char(c), '\0' };
	return strcmp(cs,d);
}

ostream&
token_char::what(ostream& o) const {
	return o << char(c);
}

//=============================================================================
// class token_int method definitions

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_int)

/**
	Performs string comparison for an integer token.
	TODO: use ostringstream instead...
	\param d string to match against
	\return 0 if match
 */
int
token_int::string_compare(const char* d) const {
	ostringstream s;
	s << val;
	return strcmp(s.str().c_str(), d);
}

ostream&
token_int::what(ostream& o) const {
	return o << "int: " << val;
}

ostream&
token_int::dump(ostream& o) const {
	return o << val;
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
token_int::check_meta_expr(const context& c) const {
	return expr::meta_return_type(new pint_const(val));
}

//=============================================================================
// class token_float method definitions

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_float)

/**
	Performs string comparison for a floating-point token.
	TODO: use ostringstream instead...
	\param d string to match against
	\return 0 if match
 */
int
token_float::string_compare(const char* d) const {
	ostringstream s;
	s << val;
	return strcmp(s.str().c_str(), d);
}

ostream&
token_float::what(ostream& o) const {
	return o << "float: " << val;
}

ostream&
token_float::dump(ostream& o) const {
	return o << val;
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
token_float::check_meta_expr(const context& c) const {
	return expr::meta_return_type(new preal_const(val));
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

ostream&
token_identifier::dump(ostream& o) const {
	return o << AS_A(const string&, *this);
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
token_identifier::check_meta_reference(const context& c) const {
	typedef	inst_ref_expr::meta_return_type		return_type;
	STACKTRACE_VERBOSE;

	// don't look up, instantiate (checked) in the context's current scope!
	const context::placeholder_ptr_type
		inst(c.lookup_instance(*this));
	// NOTE: lookup up with unqualified identifier will check loop scope
	//	for loop varables.  
	// problem: stack is count_ptr, incompatible with never_ptr
	if (inst) {
		const never_ptr<const physical_instance_placeholder>
			pinst(inst.is_a<const physical_instance_placeholder>());
		if (pinst) {
			// catch references to 
			const never_ptr<const entity::scopespace>
				ssp(c.get_current_open_definition()
				.is_a<const entity::scopespace>());
			if (!c.at_top_level() &&
#if PROCESS_DEFINITION_IS_NAMESPACE
				pinst->get_owner() != ssp
#else
				pinst->get_owner()
				.is_a<const entity::name_space>()
#endif
				) {
				cerr <<
	"Error: cannot reference top-level instance from within a definition!  "
					<< where(*this) << endl;
				return return_type(NULL);
				// THROW_EXIT
			} else
			return pinst->make_meta_instance_reference();
		} else {
			return inst.is_a<const param_value_placeholder>()
				->make_meta_value_reference();
		}
	} else {
		// better error handling later...
		what(cerr << "failed to find ") <<
			" at " << where(*this) << endl;
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
token_identifier::check_nonmeta_reference(const context& c) const {
	typedef	inst_ref_expr::nonmeta_return_type	return_type;
	STACKTRACE_VERBOSE;

	// don't look up, instantiate (checked) in the context's current scope!
	const context::placeholder_ptr_type
		inst(c.lookup_instance(*this));
	// problem: stack is count_ptr, incompatible with never_ptr
	if (inst) {
		const never_ptr<const entity::scopespace>
			ssp(c.get_current_open_definition()
			.is_a<const entity::scopespace>());
		if (!c.at_top_level() &&
#if PROCESS_DEFINITION_IS_NAMESPACE
			inst->get_owner() != ssp
#else
			inst->get_owner()
			.is_a<const entity::name_space>()
#endif
			) {
			cerr <<
	"Error: cannot reference top-level instance from within a definition!  "
				<< where(*this) << endl;
			return return_type(NULL);
			// THROW_EXIT
		} else
		return inst->make_nonmeta_instance_reference();
	} else {
		// better error handling later...
		what(cerr << "failed to find ") <<
			" at " << where(*this) << endl;
		THROW_EXIT;		// temporary termination
		return return_type(NULL);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
token_identifier::expand_const_reference(
		const count_ptr<const inst_ref_expr>& _this, 
		reference_array_type& a) const {
	STACKTRACE_VERBOSE;
	INVARIANT(_this == this);
	INVARIANT(a.empty());
	a.push_back(_this);
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// non-member functions
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

ostream& 
token_keyword::dump(ostream& o) const {
	return o << AS_A(const string&, *this);
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

ostream&
token_bool::dump(ostream& o) const {
	return token_keyword::dump(o);
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
token_bool::check_meta_expr(const context& c) const {
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

ostream&
token_else::dump(ostream& o) const {
	FINISH_ME(Fang);
	return o;
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
token_else::check_meta_expr(const context& c) const {
	cerr << "token_else::check_build(): Don't call me!";
	return expr::meta_return_type(NULL);
}

//=============================================================================
// class token_quoted_string method definitions

CONSTRUCTOR_INLINE
token_quoted_string::token_quoted_string(const string& s) :
	token_string(s), expr() { }

DESTRUCTOR_INLINE
token_quoted_string::~token_quoted_string() { }

/**
	TODO: handle special characters later...
		how to print escape sequences?
 */
ostream&
token_quoted_string::what(ostream& o) const {
//	return static_cast<const token_string*>(this)->what(
//		o << "string: \"") << "\"";
//	return token_string::what(o << "string: \"") << "\"";
	o << "string: \"";
	o << AS_A(const string&, *this);
	return o << "\"";
}

ostream&
token_quoted_string::dump(ostream& o) const {
	return o << '\"' << AS_A(const string&, *this) << '\"';
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
	FINISH_ME_EXIT(Fang);
	return never_ptr<const object>(NULL);
}

expr::meta_return_type
token_quoted_string::check_meta_expr(const context& c) const {
	return meta_return_type(new pstring_const(*this));
}

expr::nonmeta_return_type
token_quoted_string::check_nonmeta_expr(const context& c) const {
	return nonmeta_return_type(new pstring_const(*this));
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
token_datatype::dump(ostream& o) const {
	return o << AS_A(const string&, *this);
}

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
token_bool_type::token_bool_type(const char* dt) : token_datatype(dt)
	, atomic(false)
	{ }

DESTRUCTOR_INLINE
token_bool_type::~token_bool_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_bool_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
type_base::return_type
token_bool_type::check_definition(const context&) const {
	STACKTRACE("token_bool_type::check_build()");
	// safe to use never_ptr on address of statically allocated definition
	return type_base::return_type(&bool_traits::built_in_definition);
}

//=============================================================================
// class token_int_type method definitions

CONSTRUCTOR_INLINE
token_int_type::token_int_type(const char* dt) : token_datatype(dt)
	, atomic(false)
	{ }

DESTRUCTOR_INLINE
token_int_type::~token_int_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_int_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
type_base::return_type
token_int_type::check_definition(const context&) const {
	STACKTRACE("token_int_type::check_build()");
	// safe to use never_ptr on address of statically allocated definition
#if 0
	if (atomic) {
		// TODO: implement eint like ebool
		FINISH_ME_EXIT(Fang);
	}
#endif
	return type_base::return_type(&int_traits::built_in_definition);
}

//=============================================================================
// class token_pbool_type method definitions

CONSTRUCTOR_INLINE
token_pbool_type::token_pbool_type(const char* dt) : token_paramtype(dt) { }

DESTRUCTOR_INLINE
token_pbool_type::~token_pbool_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_pbool_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Returns built-in parameter boolean type reference.  
 */
concrete_type_ref::return_type
token_pbool_type::check_type(const context&) const {
	return pbool_traits::built_in_type_ptr;
}

//=============================================================================
// class token_pint_type method definitions

CONSTRUCTOR_INLINE
token_pint_type::token_pint_type(const char* dt) : token_paramtype(dt) { }

DESTRUCTOR_INLINE
token_pint_type::~token_pint_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_pint_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Return built-in parameter integer type reference.
 */
concrete_type_ref::return_type
token_pint_type::check_type(const context&) const {
	return pint_traits::built_in_type_ptr;
}

//=============================================================================
// class token_preal_type method definitions

CONSTRUCTOR_INLINE
token_preal_type::token_preal_type(const char* dt) : token_paramtype(dt) { }

DESTRUCTOR_INLINE
token_preal_type::~token_preal_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_preal_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Return built-in parameter integer type reference.
 */
concrete_type_ref::return_type
token_preal_type::check_type(const context&) const {
	return preal_traits::built_in_type_ptr;
}

//=============================================================================
// class token_pstring_type method definitions

CONSTRUCTOR_INLINE
token_pstring_type::token_pstring_type(const char* dt) : token_paramtype(dt) { }

DESTRUCTOR_INLINE
token_pstring_type::~token_pstring_type() { }

CHUNK_MAP_POOL_DEFAULT_STATIC_DEFINITION(token_pstring_type)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Return built-in parameter integer type reference.
 */
concrete_type_ref::return_type
token_pstring_type::check_type(const context&) const {
	return pstring_traits::built_in_type_ptr;
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#undef	CONSTRUCTOR_INLINE
#undef	DESTRUCTOR_INLINE

DEFAULT_STATIC_TRACE_END

#endif	// __HAC_AST_TOKEN_CC__

