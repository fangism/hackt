// "art_parser_instance.h"
// parser classes for ART only!

#ifndef __ART_PARSER_INSTANCE_H__
#define __ART_PARSER_INSTANCE_H__

#include <iosfwd>
#include <string>

#include "art_parser_base.h"
// #include "art_parser_expr.h"

#if 0
#include "art_macros.h"
#include "art_utils.h"		// for token_position
#include "sublist.h"		// for efficient list slices
#include "ptrs.h"		// experimental pointer classes
#include "count_ptr.h"		// reference-counted pointers
#endif

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

using namespace std;
using namespace entity;
using namespace fang;		// for experimental pointer classes

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

	// defined in "art_parser_expr.*"
	class range;		
	class range_list;

//=============================================================================
/**
	An expression list specialized for port connection arguments.
 */
class connection_argument_list : public expr_list {
public:
	connection_argument_list(expr_list* e);
virtual ~connection_argument_list();

virtual ostream& what(ostream& o) const;
using expr_list::leftmost;
using expr_list::rightmost;
virtual never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class connection_argument_list

#define connection_argument_list_wrap(b,l,e)				\
	IS_A(connection_argument_list*, l->wrap(b,e))
#define connection_argument_list_append(l,d,n)				\
	IS_A(connection_argument_list*, l->append(d,n))

//=============================================================================
#if 0
typedef node_list<concrete_type_ref,comma>	data_type_ref_list_base;
	// consider making concrete_datatype_ref sub-class
	// or overriding class's check_build

/// list of base data types
class data_type_ref_list : public data_type_ref_list_base {
protected:
	typedef	data_type_ref_list_base			parent;
public:
	data_type_ref_list(const concrete_type_ref* c);
	~data_type_ref_list();
};

// construction macros
#define data_type_ref_list_wrap(b,l,e)					\
	IS_A(data_type_ref_list*, l->wrap(b,e))
#define data_type_ref_list_append(l,d,n)				\
	IS_A(data_type_ref_list*, l->append(d,n))

#endif

//=============================================================================
/**
	Base class for instance-related items, including declarations, 
	arrays, connections and aliases, conditionals, loops.  
 */
class instance_management : virtual public def_body_item, 
		virtual public root_item {
public:
	instance_management();
virtual	~instance_management();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};	// end class instance_management

//-----------------------------------------------------------------------------
/**
	A list of lvalue expressions aliased/connected together.  
 */
class alias_list : public instance_management, public node_list<expr,alias> {
private:
	typedef node_list<expr,alias>		alias_list_base;
public:
	alias_list(expr* e);
virtual	~alias_list();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class alias_list

#define alias_list_wrap(b,l,e)						\
	IS_A(alias_list*, l->wrap(b,e))
#define alias_list_append(l,d,n)					\
        IS_A(alias_list*, l->append(d,n))

//=============================================================================
/**
	Abstract base class for connection statements of instantiations.  
	Contains actuals list of arguments.  
 */
class actuals_base : virtual public instance_management {
protected:
	const excl_const_ptr<expr_list>		actuals;
public:
	actuals_base(const expr_list* l);
virtual	~actuals_base();

// same virtual methods
// virtual	ostream& what(ostream& o) const;
// virtual	line_position leftmost(void) const;
// virtual	line_position rightmost(void) const;
// virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class actuals_base

//=============================================================================
/**
	Basic instance identifier, no trimmings, just contains an identifier.  
 */
class instance_base : virtual public instance_management {
protected:
/**
	In instantiation context, id should only be a token_identifier, 
 */
	const excl_const_ptr<token_identifier>		id;
public:
	instance_base(const token_identifier* i);
virtual	~instance_base();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<instance_base,comma>	instance_id_list_base;

class instance_id_list : public instance_id_list_base {
protected:
	typedef	instance_id_list_base		parent;
public:
	instance_id_list(const instance_base* i);
	~instance_id_list();

};	// end class instance_id_list

#define instance_id_list_wrap(b,l,e)					\
	IS_A(instance_id_list*, l->wrap(b,e))
#define instance_id_list_append(l,d,n)					\
	IS_A(instance_id_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/// instance identifier with ranges
class instance_array : public instance_base {
protected:
	const excl_const_ptr<range_list>	ranges;	///< optional ranges
public:
	instance_array(const token_identifier* i, const range_list* rl);
virtual	~instance_array();

virtual	ostream& what(ostream& o) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance_array

//=============================================================================
/**
	A collection of instances of the same type. 
	Contains a list of instance identifiers.  
 */
class instance_declaration : public instance_management {
protected:
	/**
		The base type of the instantiations in this collection.  
	 */
	const excl_const_ptr<concrete_type_ref>		type;
	/**
		List of instance_base.  
	 */
	const excl_const_ptr<instance_id_list>		ids;
	const excl_const_ptr<terminal>			semi;
public:
	instance_declaration(const concrete_type_ref* t, 
		const instance_id_list* i, const terminal* s = NULL);
virtual	~instance_declaration();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance declaration

//=============================================================================
/// class for an or instance port connection or declaration connection
class instance_connection : public instance_base, public actuals_base {
protected:
//	const excl_const_ptr<token_identifier>	id;		// inherited
//	const excl_const_ptr<expr_list>		actuals;	// inherited
	const excl_const_ptr<terminal>		semi;	///< semicolon (optional)
public:
	instance_connection(const token_identifier* i, const expr_list* a, 
		const terminal* s = NULL);
virtual	~instance_connection();

// remember to check for declaration context when checking id

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance_connection

//-----------------------------------------------------------------------------
/**
	Statement that is just a connection of ports.  
	Resembles a plain function call.  
	Unlike instance_connection, this doesn't create any new 
	instantiations.  
 */
class connection_statement : public actuals_base {
protected:
//	const excl_const_ptr<expr_list>		actuals;	// inherited
	const excl_const_ptr<expr>		lvalue;
	const excl_const_ptr<terminal>		semi;
public:
	connection_statement(const expr* l, const expr_list* a, 
		const terminal* s = NULL);
virtual	~connection_statement();
virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
// virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class connection_statement

//-----------------------------------------------------------------------------
/**
	An alias statement without type identifier, 
	instance is declared at the same time.  
	Can also be a parameter instantiation initialized
	to other parameter expressions.  
	Contains a list of alias identifier expressions.  
 */
class instance_alias : public instance_base {
protected:
//	const excl_const_ptr<token_identifier>	id;	// inherited
	const excl_const_ptr<alias_list>	aliases;
	const excl_const_ptr<terminal>		semi;	///< semicolon
public:
	instance_alias(const token_identifier* i, alias_list* al, 
		const terminal* s = NULL);
virtual	~instance_alias();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class instance_alias

//=============================================================================
/// class for loop instantiations, to be unrolled in the build phase
class loop_instantiation : public instance_management {
protected:
	const excl_const_ptr<terminal>		lp;
	const excl_const_ptr<terminal>		delim;
	const excl_const_ptr<token_identifier>	index;
	const excl_const_ptr<terminal>		colon1;
	const excl_const_ptr<range>		rng;
	const excl_const_ptr<terminal>		colon2;
	const excl_const_ptr<definition_body>	body;
	const excl_const_ptr<terminal>		rp;
public:
	loop_instantiation(const terminal* l, const terminal* d, 
		const token_identifier* i, const terminal* c1, 
		const range* g, const terminal* c2, 
		const definition_body* b, const terminal* r);
virtual	~loop_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};	// end class loop_instantiation

//=============================================================================
/// conditional instantiations in definition body
class guarded_definition_body : public instance_management {
protected:
	const excl_const_ptr<expr>	guard;	///< condition expression
	const excl_const_ptr<terminal>		arrow;	///< right arrow
	const excl_const_ptr<definition_body>	body;
public:
	guarded_definition_body(const expr* e, const terminal* a, 
		const definition_body* b);
virtual	~guarded_definition_body();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};	// end class guarded_definition_body

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<guarded_definition_body,thickbar>
		guarded_definition_body_list_base;
class guarded_definition_body_list : public guarded_definition_body_list_base {
protected:
	typedef	guarded_definition_body_list_base		parent;
public:
	guarded_definition_body_list(const guarded_definition_body* g);
	~guarded_definition_body_list();
};	// end class guarded_definition_body_list

#define guarded_definition_body_list_wrap(b,l,e)			\
	IS_A(guarded_definition_body_list*, l->wrap(b,e))
#define guarded_definition_body_list_append(l,d,n)			\
	IS_A(guarded_definition_body_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/// wrapper class for conditional instantiations
class conditional_instantiation : public instance_management {
protected:
	const excl_const_ptr<guarded_definition_body_list>	gd;
public:
	conditional_instantiation(const guarded_definition_body_list* n);
virtual	~conditional_instantiation();

virtual	ostream& what(ostream& o) const;
virtual	line_position leftmost(void) const;
virtual	line_position rightmost(void) const;
};	// end class conditional_instantiation

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_INSTANCE_H__

