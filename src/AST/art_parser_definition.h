/**
	\file "art_parser_definition.h"
	Definition-related parser classes for ART.  
 */

#ifndef __ART_PARSER_DEFINITION_H__
#define __ART_PARSER_DEFINITION_H__

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
	class concrete_type_ref;	// defined here
	class context;		// defined in "art_symbol_table.h"

	// defined in "art_parser_formal.h"
	class data_param_decl_list;
	class template_formal_decl_list;
	class port_formal_decl_list;

//=============================================================================
/**
	Abstract base class for definitions of complex types, 
	including processes, user-defined channels and data-types.  
	All definitions are templatable!  
 */
class definition : public root_item {
protected:
public:
	definition();
virtual	~definition();

virtual	ostream& what(ostream& o) const = 0;
virtual	line_position leftmost(void) const = 0;
virtual	line_position rightmost(void) const = 0;
};	// end class definition

//-----------------------------------------------------------------------------
/// abstract base class for prototypes
class prototype : public root_item {
public:
	prototype();
virtual	~prototype();

// don't bother re-declaring virtual methods
};	// end class prototype

//=============================================================================
/**
	Abstract base class for any definition or declaration.
	All definitions and prototypes may potentially contain a
	template signature.  
 */
class signature_base : virtual public node {
protected:
	const excl_const_ptr<template_formal_decl_list>	temp_spec;
	const excl_const_ptr<token_identifier>		id;
public:
	signature_base(const template_formal_decl_list* tf, 
		const token_identifier* i) :
		node(), temp_spec(tf), id(i) { }
virtual	~signature_base();

virtual	never_const_ptr<object> check_build(never_ptr<context> c) const = 0;
};	// end class signature_base

//=============================================================================
/**
	Abstract base class for basic process information
	(prototype, basically).  
	This class need not be a node or root_item because it's never 
	constructed on the symbol stack.  
 */
class process_signature : public signature_base {
protected:
	const excl_const_ptr<token_keyword>	def;	///< definition keyword
		// should never be NULL, could be const reference?
	/**
		Optional port formal list.  
		Never NULL, but may be empty.  
	 */
	const excl_const_ptr<port_formal_decl_list>	ports;
public:
	process_signature(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p);
virtual	~process_signature();

// note: non-virtual
	const token_identifier& get_name(void) const;
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class process_signature

//-----------------------------------------------------------------------------
/// process prototype declaration
class process_prototype : public prototype, public process_signature {
protected:
	const excl_const_ptr<token_char>	semi;	///< semicolon token
public:
	process_prototype(const template_formal_decl_list* tf, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p, const token_char* s);
	~process_prototype();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class process_prototype

//-----------------------------------------------------------------------------
/**
	Process definition, syntax structure.  
 */
class process_def : public definition, public process_signature {
protected:
//	const excl_const_ptr<token_keyword>		def;	//  inherited
//	const excl_const_ptr<concrete_type_ref>		idt;	//  inherited
//	const excl_const_ptr<port_formal_decl_list>	ports;	//  inherited
	const excl_const_ptr<definition_body>		body;	///< definition body
public:
	process_def(const template_formal_decl_list*, 
		const token_keyword* d, const token_identifier* i, 
		const port_formal_decl_list* p, const definition_body* b);
	~process_def();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class process_def

//=============================================================================
/// user-defined data type
class user_data_type_signature : public signature_base {
protected:
	const excl_const_ptr<token_keyword>	def;	///< "deftype" keyword
	const excl_const_ptr<token_string>	dop;	///< <: operator
	const excl_const_ptr<concrete_type_ref>	bdt;	///< the represented type
	const excl_const_ptr<data_param_decl_list>
						params;	///< implementation type
public:
	user_data_type_signature(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, 
		const concrete_type_ref* b, 	// or concrete_datatype_ref
		const data_param_decl_list* p);
virtual	~user_data_type_signature();
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class user_data_type_signature

//-----------------------------------------------------------------------------
/// user-defined data type prototype declaration
class user_data_type_prototype : public prototype, 
		public user_data_type_signature {
protected:
	const excl_const_ptr<token_char>	semi;	///< semicolon
public:
	user_data_type_prototype(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p, const token_char* s);
	~user_data_type_prototype();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class user_data_type_prototype

//-----------------------------------------------------------------------------
/// user-defined data type (is not a type_base)
class user_data_type_def : public definition, public user_data_type_signature {
protected:
//	const excl_const_ptr<token_keyword>	def;	// inherited
//	const excl_const_ptr<token_identifier>	name;	// inherited
//	const excl_const_ptr<token_string>	dop;	// inherited
//	const excl_const_ptr<concrete_type_ref>	bdt;	// inherited
//	const excl_const_ptr<data_param_decl_list>	params;	// inherited
	const excl_const_ptr<token_char>	lb;	///< left brace
	const excl_const_ptr<language_body>	setb;	///< set body
	const excl_const_ptr<language_body>	getb;	///< get body
	const excl_const_ptr<token_char>	rb;	///< right brace
public:
	user_data_type_def(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p, 
		const token_char* l, const language_body* s,
		const language_body* g, const token_char* r);
	~user_data_type_def();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class user_data_type_def

//-----------------------------------------------------------------------------
/**
	Common field data to enumeration declaration and definitions.  
 */
class enum_signature : public signature_base {
protected:
//	const excl_const_ptr<token_identifier>	id;	// inherited
	/** "enum" keyword */
	const excl_const_ptr<token_keyword>	en;
public:
	enum_signature(const token_keyword* e, const token_identifier* i);
virtual	~enum_signature();

virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class enum_signature

//-----------------------------------------------------------------------------
/**
	Forward declaration of an enum type.  
	Not really a prototype, but classified as such for convenience.  
 */
class enum_prototype : public prototype, public enum_signature {
protected:
	/** semicolon token (optional) */
	const excl_const_ptr<token_char>	semi;
public:
	enum_prototype(const token_keyword* e, const token_identifier* i, 
		const token_char* s);
	~enum_prototype();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class enum_prototype

//-----------------------------------------------------------------------------
typedef	node_list<token_identifier,comma>	enum_member_list_base;

/**
	Specialized list of identifiers for enumeration members.  
 */
class enum_member_list : public enum_member_list_base {
protected:	// no new members
	typedef	enum_member_list_base		parent;
public:
	enum_member_list(const token_identifier* i);
	~enum_member_list();

	/** overrides default */
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end enum_member_list

#define enum_member_list_wrap(b,l,e)					\
	IS_A(enum_member_list*, l->wrap(b,e))
#define enum_member_list_append(l,d,n)					\
	IS_A(enum_member_list*, l->append(d,n))

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Definition of an enumeration data type.
 */
class enum_def : public definition, public enum_signature {
protected:
	/** names of the enumerated members */
	const excl_const_ptr<enum_member_list>	members;
public:
	enum_def(const token_keyword* e, const token_identifier* i,      
		const enum_member_list*	m);
	~enum_def();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class enum_def

//=============================================================================
/// user-defined channel type signature
class user_chan_type_signature : public signature_base {
protected:
	const excl_const_ptr<token_keyword>	def;	///< "defchan" keyword
	const excl_const_ptr<token_string>	dop;	///< <: operator
	const excl_const_ptr<chan_type>		bct;	///< the represented type
	const excl_const_ptr<data_param_decl_list>
						params;	///< the implementation type
public:
	user_chan_type_signature(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p);
virtual	~user_chan_type_signature();
virtual	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class user_data_type_signature

//-----------------------------------------------------------------------------
/// user-defined channel type prototype
class user_chan_type_prototype : public prototype, 
		public user_chan_type_signature {
protected:
	const excl_const_ptr<token_char>	semi;	///< semicolon
public:
	user_chan_type_prototype(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p, const token_char* s);
	~user_chan_type_prototype();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class user_chan_type_prototype

//-----------------------------------------------------------------------------
/// user-defined channel type definition
class user_chan_type_def : public definition, public user_chan_type_signature {
protected:
//	const excl_const_ptr<token_keyword>	def;	// inherited
//	const excl_const_ptr<token_identifier>	name;	// inherited
//	const excl_const_ptr<token_string>	dop;	// inherited
//	const excl_const_ptr<chan_type>		bct;	// inherited
//	const excl_const_ptr<data_param_decl_list>	params;	// inherited
	const excl_const_ptr<token_char>	lb;	///< left brace
	const excl_const_ptr<language_body>	sendb;	///< set body
	const excl_const_ptr<language_body>	recvb;	///< get body
	const excl_const_ptr<token_char>	rb;	///< right brace
public:
	user_chan_type_def(const template_formal_decl_list* tf, 
		const token_keyword* df, const token_identifier* n, 
		const token_string* dp, const chan_type* b, 
		const data_param_decl_list* p, 
		const token_char* l, const language_body* s, 
		const language_body* g, const token_char* r);
	~user_chan_type_def();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
};	// end class user_chan_type_def

//=============================================================================

class typedef_alias : public def_body_item, public root_item {
protected:
	const excl_const_ptr<template_formal_decl_list>	temp_spec;
	const excl_const_ptr<token_keyword>		td;
	const excl_const_ptr<concrete_type_ref>		base;
	const excl_const_ptr<token_identifier>		id;
	const excl_const_ptr<token_char>		semi;
public:
	typedef_alias(const template_formal_decl_list* t, 
		const token_keyword* k, const concrete_type_ref* b, 
		const token_identifier* i, const token_char* s);
	~typedef_alias();

	ostream& what(ostream& o) const;
	line_position leftmost(void) const;
	line_position rightmost(void) const;
	never_const_ptr<object> check_build(never_ptr<context> c) const;
};	// end class typedef_alias

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_DEFINITION_H__

