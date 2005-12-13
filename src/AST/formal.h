/**
	\file "AST/formal.h"
	Definition-formal-related parser classes for HAC.  
	$Id: formal.h,v 1.2 2005/12/13 04:15:08 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_formal.h,v 1.14.40.1 2005/12/11 00:45:07 fang Exp
 */

#ifndef __HAC_AST_FORMAL_H__
#define __HAC_AST_FORMAL_H__

#include "AST/common.h"
#include "AST/node_list.h"
#include "util/boolean_types.h"

namespace HAC {
namespace parser {
using util::good_bool;
//=============================================================================
/**
	Class for port data (rather, members) of a user-defined channels.  
 */
class data_param_id {
	// should be called data_port_id
protected:
	const excl_ptr<const token_identifier>	id;
	const excl_ptr<const dense_range_list>	dim;
public:
	data_param_id(const token_identifier* i, const dense_range_list* d);

	~data_param_id();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& ) const;

};	// end class data_param_id

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const data_param_id>		data_param_id_list_base;

/**
	Data or parameter identifier list.  
 */
class data_param_id_list : public data_param_id_list_base {
protected:
	typedef	data_param_id_list_base			parent_type;
public:
	explicit
	data_param_id_list(const data_param_id* d);

	~data_param_id_list();

};	// end class data_param_id_list

//-----------------------------------------------------------------------------
/**
	Data parameter port declarations, grouped together by type.  
 */
class data_param_decl {
protected:
	/**
		The base type of the data ports in this collection.  
	 */
	const excl_ptr<const concrete_type_ref>		type;
	const excl_ptr<const data_param_id_list>	ids;
public:
	data_param_decl(const concrete_type_ref* t, 
		const data_param_id_list* il);

	~data_param_decl();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& ) const;

};	// end class data_param_decl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const data_param_decl>	data_param_decl_list_base;

/**
	Data or parameter declaration list.  
	Consider just typedef to _base class.  
 */
class data_param_decl_list : public data_param_decl_list_base {
protected:
	typedef	data_param_decl_list_base		parent_type;
public:
	explicit
	data_param_decl_list(const data_param_decl* d);

	~data_param_decl_list();

	good_bool
	check_chan_ports(context&) const;

	good_bool
	check_data_ports(context&) const;

};	// end class data_param_decl_list

//=============================================================================
/**
	Single port formal identifier, 
	with optional dimension array specification
 */
class port_formal_id {
protected:
	const excl_ptr<const token_identifier>	name;	///< formal name
	const excl_ptr<const dense_range_list>	dim;	///< optional dimensions
public:
	port_formal_id(const token_identifier* n, const dense_range_list* d);
	~port_formal_id();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class port_formal_id

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const port_formal_id>		port_formal_id_list_base;

/// list of port-formal identifiers (optional arrays)
class port_formal_id_list : public port_formal_id_list_base {
protected:
	typedef	port_formal_id_list_base		parent_type;
public:
	explicit
	port_formal_id_list(const port_formal_id* p);

	~port_formal_id_list();

};	// end class port_formal_id_list

//-----------------------------------------------------------------------------
/**
	Port formal declaration contains a type and identifier list.
 */
class port_formal_decl {
protected:
	const excl_ptr<const concrete_type_ref>		type;	///< formal base type
	const excl_ptr<const port_formal_id_list>	ids;	///< identifier list
public:
	port_formal_decl(const concrete_type_ref* t, 
		const port_formal_id_list* i);

	~port_formal_decl();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class port_formal_decl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const port_formal_decl>	port_formal_decl_list_base;

/**
	List of port-formal declarations.  
	Later: distinguish between empty ports and NULL (unspecified ports).  
 */
class port_formal_decl_list : public port_formal_decl_list_base {
protected:
	typedef	port_formal_decl_list_base		parent_type;
public:
	port_formal_decl_list();

	explicit
	port_formal_decl_list(const port_formal_decl* p);

	~port_formal_decl_list();
};	// end class port_formal_decl_list

//=============================================================================
/**
	Single template formal identifier,
	with optional dimension array spec.  
	Now with optional default argument expression.  
 */
class template_formal_id {
protected:
	const excl_ptr<const token_identifier>	name;	///< formal name
	const excl_ptr<const dense_range_list>	dim;	///< optional dimensions
	const excl_ptr<const char_punctuation_type>	eq;	///< '=' token
	const excl_ptr<const expr>		dflt;	///< default value
public:
	template_formal_id(const token_identifier* n,
		const dense_range_list* d, 
		const char_punctuation_type* e = NULL,
		const expr* v = NULL);

	~template_formal_id();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class template_formal_id

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const template_formal_id>	template_formal_id_list_base;

/// list of template-formal identifiers (optional arrays)
class template_formal_id_list : public template_formal_id_list_base {
protected:
	typedef	template_formal_id_list_base		parent_type;
public:
	explicit
	template_formal_id_list(const template_formal_id* t);

	~template_formal_id_list();
};	// end class template_formal_id_list

//-----------------------------------------------------------------------------
/**
	template formal declaration contains a type and identifier list
	Later: ADVANCED -- type can be a concrete_type_ref!
		Mr. Fancy-pants!
		Don't forget that its check build already
		sets current_fundamental_type
 */
class template_formal_decl {
protected:
	const excl_ptr<const token_paramtype>	type;	///< formal base type
		// why not concrete_type_ref?
	const excl_ptr<const template_formal_id_list>	ids;	///< identifiers
public:
	template_formal_decl(const token_paramtype* t, 
		const template_formal_id_list* i);

	~template_formal_decl();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class template_formal_decl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<const template_formal_decl>
					template_formal_decl_list_base;

/**
	Template formal declaration list.  
 */
class template_formal_decl_list : public template_formal_decl_list_base {
protected:
	typedef	template_formal_decl_list_base		parent_type;
public:
	template_formal_decl_list();

	explicit
	template_formal_decl_list(const template_formal_decl* t);

	~template_formal_decl_list();
};	// end class template_formal_decl_list

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pair of template formal lists, the first being for strict parameters, 
	the second list for relaxed parameters.  
 */
struct template_formal_decl_list_pair {
	const excl_ptr<const template_formal_decl_list>	first;
	const excl_ptr<const template_formal_decl_list>	second;

	template_formal_decl_list_pair(const template_formal_decl_list*,
		const template_formal_decl_list*);
	~template_formal_decl_list_pair();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class template_formal_decl_list_pair

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_FORMAL_H__

