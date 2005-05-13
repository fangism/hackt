/**
	\file "AST/art_parser_instance.h"
	Instance-related parser classes for ART.  
	$Id: art_parser_instance.h,v 1.13 2005/05/13 21:24:28 fang Exp $
 */

#ifndef __AST_ART_PARSER_INSTANCE_H__
#define __AST_ART_PARSER_INSTANCE_H__

#include "AST/art_parser_expr_list.h"
#include "AST/art_parser_root.h"
#include "AST/art_parser_definition_item.h"
#include "util/STL/vector_fwd.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
	class param_expression_assignment;
	class aliases_connection_base;
	class port_connection;
	class simple_instance_reference;
}
namespace parser {
using util::good_bool;
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

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;
};	// end class instance_management

//-----------------------------------------------------------------------------
typedef	expr_list			alias_list_base;

/**
	A list of lvalue expressions aliased/connected together.  
	NOTE: this doubles as a connection and assignment!
	They are syntactically indistinguishable without context.  
 */
class alias_list : public instance_management, public alias_list_base {
private:
	typedef alias_list_base			parent_type;
public:
	explicit
	alias_list(const expr* e);

	~alias_list();

	using parent_type::size;
	using parent_type::push_front;

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;

private:
	typedef	parent_type::checked_generic_type	checked_generic_type;
	typedef	parent_type::checked_exprs_type		checked_exprs_type;
	typedef	parent_type::checked_refs_type		checked_refs_type;

	static
	excl_ptr<const entity::param_expression_assignment>
	make_param_assignment(const checked_exprs_type&);

	static
	excl_ptr<const entity::aliases_connection_base>
	make_alias_connection(const checked_refs_type&);
};	// end class alias_list

//=============================================================================
/**
	Abstract base class for connection statements of instantiations.  
	Contains actuals list of arguments, just wrapped around expr_list.  
 */
class actuals_base {
protected:
	const excl_ptr<const expr_list>		actuals;
public:
	explicit
	actuals_base(const expr_list* l);

virtual	~actuals_base();

// same virtual methods
// virtual	ostream& what(ostream& o) const;
virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

	good_bool
	check_actuals(expr_list::checked_refs_type&, context& c) const;
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
	const excl_ptr<const token_identifier>		id;
public:
	explicit
	instance_base(const token_identifier* i);

virtual	~instance_base();

virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class instance_base

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const instance_base>		instance_id_list_base;

/**
	Instance identifier list.  
 */
class instance_id_list : public instance_id_list_base {
protected:
	typedef	instance_id_list_base		parent_type;
public:
	explicit
	instance_id_list(const instance_base* i);

	~instance_id_list();

};	// end class instance_id_list

//-----------------------------------------------------------------------------
/// instance identifier with ranges
class instance_array : public instance_base {
protected:
	const excl_ptr<const range_list>	ranges;	///< optional ranges
public:
	instance_array(const token_identifier* i, const range_list* rl);

	~instance_array();

	ostream&
	what(ostream& o) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
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
	const excl_ptr<const concrete_type_ref>		type;
	/**
		List of instance_base.  
	 */
	const excl_ptr<const instance_id_list>		ids;
public:
	instance_declaration(const concrete_type_ref* t, 
		const instance_id_list* i);

	~instance_declaration();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class instance_declaration

//=============================================================================
/**
	Class for an or instance port connection or declaration connection.
	Declarator cannot be an array or indexed, may only be single instance.  
 */
class instance_connection : public instance_base, public actuals_base {
protected:
//	const excl_ptr<const token_identifier>	id;		// inherited
//	const excl_ptr<const expr_list>		actuals;	// inherited
public:
	instance_connection(const token_identifier* i, const expr_list* a);

	~instance_connection();

// remember to check for declaration context when checking id
	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class instance_connection

//-----------------------------------------------------------------------------
/**
	Statement that is just a connection of ports.  
	Resembles a plain function call.  
	Unlike instance_connection, this doesn't create any new 
	instantiations.  
 */
class connection_statement : public instance_management, public actuals_base {
protected:
//	const excl_ptr<const expr_list>		actuals;	// inherited
	/**
		Instance reference to connect, may be indexed,
		but must be scalar.  
	 */
	const excl_ptr<const inst_ref_expr>		lvalue;
public:
	connection_statement(const inst_ref_expr* l, const expr_list* a);

	~connection_statement();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;

	static
	excl_ptr<const entity::port_connection>
	make_port_connection(const expr_list::checked_refs_type&, 
                const count_ptr<const entity::simple_instance_reference>& ir);

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
//	const excl_ptr<const token_identifier>	id;	// inherited
	const excl_ptr<const alias_list>	aliases;
public:
	instance_alias(const token_identifier* i, alias_list* al);

	~instance_alias();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class instance_alias

//=============================================================================
/// class for loop instantiations, to be unrolled in the build phase
class loop_instantiation : public instance_management {
protected:
	const excl_ptr<const char_punctuation_type>	lp;
	const excl_ptr<const token_identifier>		index;
	const excl_ptr<const range>			rng;
	const excl_ptr<const definition_body>		body;
	const excl_ptr<const char_punctuation_type>	rp;
public:
	loop_instantiation(const char_punctuation_type* l,
		const token_identifier* i, const range* g,
		const definition_body* b, const char_punctuation_type* r);

	~loop_instantiation();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class loop_instantiation

//=============================================================================
/// conditional instantiations in definition body
class guarded_definition_body : public instance_management {
protected:
	const excl_ptr<const expr>	guard;	///< condition expression
	const excl_ptr<const terminal>		arrow;	///< right arrow
	const excl_ptr<const definition_body>	body;
public:
	guarded_definition_body(const expr* e, const terminal* a, 
		const definition_body* b);

	~guarded_definition_body();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class guarded_definition_body

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const guarded_definition_body>
		guarded_definition_body_list_base;

/**
	Conditional (guarded) body inside definition.  
 */
class guarded_definition_body_list : public guarded_definition_body_list_base {
protected:
	typedef	guarded_definition_body_list_base		parent_type;
public:
	explicit
	guarded_definition_body_list(const guarded_definition_body* g);

	~guarded_definition_body_list();
};	// end class guarded_definition_body_list

//-----------------------------------------------------------------------------
/// wrapper class for conditional instantiations
class conditional_instantiation : public instance_management {
protected:
	const excl_ptr<const guarded_definition_body_list>	gd;
public:
	explicit
	conditional_instantiation(const guarded_definition_body_list* n);

	~conditional_instantiation();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 1
	never_ptr<const object>
	check_build(context& ) const;
#endif
};	// end class conditional_instantiation

//=============================================================================
/**
	Statement that completes the type of an indexed range of instances
	with relaxed template arguments.  
 */
class type_completion_statement : virtual public instance_management {
protected:
	const excl_ptr<const index_expr>	inst_ref;
	const excl_ptr<const expr_list>		args;
public:
	type_completion_statement(const index_expr*, const expr_list*);
virtual	~type_completion_statement();

virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

virtual	never_ptr<const object>
	check_build(context& ) const;

};	// end class type_completion_statement

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as type_completion_statement, but this also includes
	port connections.  
	Consider deriving from type_completion_statement.  
 */
class type_completion_connection_statement :
		public type_completion_statement, public actuals_base {
public:
	type_completion_connection_statement(const index_expr*,
		const expr_list*, const expr_list*);
	~type_completion_connection_statement();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& ) const;

};	// end class type_completion_statement

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_INSTANCE_H__

