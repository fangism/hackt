/**
	\file "art_parser_instance.h"
	Instance-related parser classes for ART.  
	$Id: art_parser_instance.h,v 1.8.40.1 2005/02/22 08:15:20 fang Exp $
 */

#ifndef __ART_PARSER_INSTANCE_H__
#define __ART_PARSER_INSTANCE_H__

#include "art_parser_expr_list.h"
#include "art_parser_root.h"
#include "art_parser_definition_item.h"

namespace ART {
namespace parser {
//=============================================================================
/**
	An expression list specialized for port connection arguments.
 */
class connection_argument_list : public expr_list {
public:
	explicit
	connection_argument_list(expr_list* e);

	~connection_argument_list();

	ostream&
	what(ostream& o) const;

using	expr_list::leftmost;
using	expr_list::rightmost;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class connection_argument_list

#define connection_argument_list_wrap(b,l,e)				\
	IS_A(connection_argument_list*, l->wrap(b,e))
#define connection_argument_list_append(l,d,n)				\
	IS_A(connection_argument_list*, l->append(d,n))

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
/**
	A list of lvalue expressions aliased/connected together.  
 */
class alias_list : public instance_management, 
		public node_list<const expr,alias> {
private:
	typedef node_list<const expr,alias>		alias_list_base;
public:
	explicit
	alias_list(const expr* e);

	~alias_list();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class alias_list

#define alias_list_wrap(b,l,e)						\
	IS_A(alias_list*, l->wrap(b,e))
#define alias_list_append(l,d,n)					\
        IS_A(alias_list*, l->append(d,n))

//=============================================================================
/**
	Abstract base class for connection statements of instantiations.  
	Contains actuals list of arguments, just wrapped around expr_list.  
 */
class actuals_base : virtual public instance_management {
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

virtual	never_ptr<const object>
	check_build(context& c) const;
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
typedef	node_list<const instance_base,comma>	instance_id_list_base;

/**
	Instance identifier list.  
 */
class instance_id_list : public instance_id_list_base {
protected:
	typedef	instance_id_list_base		parent;
public:
	explicit
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
	const excl_ptr<const terminal>			semi;
public:
	instance_declaration(const concrete_type_ref* t, 
		const instance_id_list* i, const terminal* s = NULL);

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
	const excl_ptr<const terminal>		semi;	///< semicolon (optional)
public:
	instance_connection(const token_identifier* i, const expr_list* a, 
		const terminal* s = NULL);

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
class connection_statement : public actuals_base {
protected:
//	const excl_ptr<const expr_list>		actuals;	// inherited
	/**
		Instance reference to connect, may be indexed,
		but must be scalar.  
	 */
	const excl_ptr<const expr>		lvalue;
	const excl_ptr<const terminal>		semi;
public:
	connection_statement(const expr* l, const expr_list* a, 
		const terminal* s = NULL);

	~connection_statement();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
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
	const excl_ptr<const terminal>		semi;	///< semicolon
public:
	instance_alias(const token_identifier* i, alias_list* al, 
		const terminal* s = NULL);

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
	const excl_ptr<const terminal>		lp;
	const excl_ptr<const terminal>		delim;
	const excl_ptr<const token_identifier>	index;
	const excl_ptr<const terminal>		colon1;
	const excl_ptr<const range>		rng;
	const excl_ptr<const terminal>		colon2;
	const excl_ptr<const definition_body>	body;
	const excl_ptr<const terminal>		rp;
public:
	loop_instantiation(const terminal* l, const terminal* d, 
		const token_identifier* i, const terminal* c1, 
		const range* g, const terminal* c2, 
		const definition_body* b, const terminal* r);

	~loop_instantiation();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;
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
};	// end class guarded_definition_body

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const guarded_definition_body,thickbar>
		guarded_definition_body_list_base;

/**
	Conditional (guarded) body inside definition.  
 */
class guarded_definition_body_list : public guarded_definition_body_list_base {
protected:
	typedef	guarded_definition_body_list_base		parent;
public:
	explicit
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
};	// end class conditional_instantiation

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_INSTANCE_H__

