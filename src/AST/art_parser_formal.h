/**
	\file "art_parser_formal.h"
	Definition-formal-related parser classes for ART.  
 */

#ifndef __ART_PARSER_FORMAL_H__
#define __ART_PARSER_FORMAL_H__

#include <iosfwd>
#include <string>

#include "art_parser_base.h"

/**
	This is the general namespace for all ART-related classes.  
 */
namespace ART {
//=============================================================================

// forward declaration of outside namespace and classes
namespace entity {
	// defined in "art_object.h"
	class object;
}

using namespace std;
using namespace entity;
using namespace PTRS_NAMESPACE;		// for experimental pointer classes

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
	class dense_range_list;	// defined in "art_parser_expr.*"

//=============================================================================
/**
	Class for port data (rather, members) of a user-defined channels.  
 */
class data_param_id : public node {
	// should be called data_port_id
protected:
	const excl_const_ptr<token_identifier>	id;
	const excl_const_ptr<dense_range_list>	dim;
public:
	data_param_id(const token_identifier* i, const dense_range_list* d);
	~data_param_id();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class data_param_id

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<data_param_id,comma>	data_param_id_list_base;

class data_param_id_list : public data_param_id_list_base {
protected:
	typedef	data_param_id_list_base			parent;
public:
	data_param_id_list(const data_param_id* d);
	~data_param_id_list();

};	// end class data_param_id_list

#define data_param_id_list_wrap(b,l,e)					\
	IS_A(data_param_id_list*, l->wrap(b,e))
#define data_param_id_list_append(l,d,n)				\
	IS_A(data_param_id_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/**
	Data parameter port declarations, grouped together by type.  
 */
class data_param_decl : public node {
protected:
	/**
		The base type of the data ports in this collection.  
	 */
	const excl_const_ptr<concrete_type_ref>		type;
	const excl_const_ptr<data_param_id_list>	ids;
public:
	data_param_decl(const concrete_type_ref* t, 
		const data_param_id_list* il);
	~data_param_decl();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class data_param_decl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<data_param_decl,semicolon>	data_param_decl_list_base;

class data_param_decl_list : public data_param_decl_list_base {
protected:
	typedef	data_param_decl_list_base		parent;
public:
	data_param_decl_list(const data_param_decl* d);
	~data_param_decl_list();

};	// end class data_param_decl_list

#define data_param_decl_list_wrap(b,l,e)				\
	IS_A(data_param_decl_list*, l->wrap(b,e))
#define data_param_decl_list_append(l,d,n)				\
	IS_A(data_param_decl_list*, l->append(d,n))

//=============================================================================
/// single port formal identifier, with optional dimension array specification
class port_formal_id : public node {
protected:
	const excl_const_ptr<token_identifier>	name;	///< formal name
	const excl_const_ptr<dense_range_list>	dim;	///< optional dimensions
public:
	port_formal_id(const token_identifier* n, const dense_range_list* d);
	~port_formal_id();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class port_formal_id

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<port_formal_id,comma>	port_formal_id_list_base;

/// list of port-formal identifiers (optional arrays)
class port_formal_id_list : public port_formal_id_list_base {
protected:
	typedef	port_formal_id_list_base		parent;
public:
	port_formal_id_list(const port_formal_id* p);
	~port_formal_id_list();

};	// end class port_formal_id_list

#define port_formal_id_list_wrap(b,l,e)					\
	IS_A(port_formal_id_list*, l->wrap(b,e))
#define port_formal_id_list_append(l,d,n)				\
	IS_A(port_formal_id_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/**
	Port formal declaration contains a type and identifier list.
 */
class port_formal_decl : public node {
protected:
	const excl_const_ptr<concrete_type_ref>		type;	///< formal base type
	const excl_const_ptr<port_formal_id_list>	ids;	///< identifier list
public:
	port_formal_decl(const concrete_type_ref* t, 
		const port_formal_id_list* i);
	~port_formal_decl();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class port_formal_decl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<port_formal_decl,semicolon>	port_formal_decl_list_base;

/**
	List of port-formal declarations.  
	Later: distinguish between empty ports and NULL (unspecified ports).  
 */
class port_formal_decl_list : public port_formal_decl_list_base {
protected:
	typedef	port_formal_decl_list_base		parent;
public:
	port_formal_decl_list();
	port_formal_decl_list(const port_formal_decl* p);
	~port_formal_decl_list();
};	// end class port_formal_decl_list

#define port_formal_decl_list_wrap(b,l,e)				\
	IS_A(port_formal_decl_list*, l->wrap(b,e))
#define port_formal_decl_list_append(l,d,n)				\
	IS_A(port_formal_decl_list*, l->append(d,n))

//=============================================================================
/**
	Single template formal identifier,
	with optional dimension array spec.  
	Now with optional default argument expression.  
 */
class template_formal_id : public node {
protected:
	const excl_const_ptr<token_identifier>	name;	///< formal name
	const excl_const_ptr<dense_range_list>	dim;	///< optional dimensions
	const excl_const_ptr<token_char>	eq;	///< '=' token
	const excl_const_ptr<expr>		dflt;	///< default value
public:
	template_formal_id(const token_identifier* n,
		const dense_range_list* d, 
		const token_char* e = NULL, const expr* v = NULL);
	~template_formal_id();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class template_formal_id

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<template_formal_id,comma>	template_formal_id_list_base;

/// list of template-formal identifiers (optional arrays)
class template_formal_id_list : public template_formal_id_list_base {
protected:
	typedef	template_formal_id_list_base		parent;
public:
	template_formal_id_list(const template_formal_id* t);
	~template_formal_id_list();
};	// end class template_formal_id_list

#define template_formal_id_list_wrap(b,l,e)				\
	IS_A(template_formal_id_list*, l->wrap(b,e))
#define template_formal_id_list_append(l,d,n)				\
	IS_A(template_formal_id_list*, l->append(d,n))

//-----------------------------------------------------------------------------
/**
	template formal declaration contains a type and identifier list
	Later: ADVANCED -- type can be a concrete_type_ref!
		Mr. Fancy-pants!
		Don't forget that its check build already
		sets current_fundamental_type
 */
class template_formal_decl : public node {
protected:
	const excl_const_ptr<token_paramtype>	type;	///< formal base type
		// why not concrete_type_ref?
	const excl_const_ptr<template_formal_id_list>	ids;	///< identifiers
public:
	template_formal_decl(const token_paramtype* t, 
		const template_formal_id_list* i);
	~template_formal_decl();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class template_formal_decl

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	node_list<template_formal_decl,semicolon>
					template_formal_decl_list_base;

class template_formal_decl_list : public template_formal_decl_list_base {
protected:
	typedef	template_formal_decl_list_base		parent;
public:
	template_formal_decl_list(const template_formal_decl* t);
	~template_formal_decl_list();
};	// end class template_formal_decl_list

#define template_formal_decl_list_wrap(b,l,e)				\
	IS_A(template_formal_decl_list*, l->wrap(b,e))
#define template_formal_decl_list_append(l,d,n)				\
	IS_A(template_formal_decl_list*, l->append(d,n))

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_FORMAL_H__

