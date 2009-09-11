/**
	\file "AST/definition.h"
	Definition-related parser classes for HAC.  
	$Id: definition.h,v 1.5.34.1 2009/09/11 23:53:35 fang Exp $
	This file used to be the following before it was renamed:
	Id: art_parser_definition.h,v 1.17.40.1 2005/12/11 00:45:04 fang Exp
 */

#ifndef __HAC_AST_DEFINITION_H__
#define __HAC_AST_DEFINITION_H__

#include "AST/namespace.h"
#include "AST/type.h"
#include "AST/definition_item.h"

namespace HAC {
namespace entity {
	class definition_base;
	class process_definition;
}
namespace parser {
namespace CHP {
	class body;
}
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

	PURE_VIRTUAL_NODE_METHODS
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
class signature_base {
protected:
	excl_ptr<const template_formal_decl_list_pair>	temp_spec;
	excl_ptr<const token_identifier>		id;
public:
	signature_base(const template_formal_decl_list_pair* tf, 
		const token_identifier* i);

protected:
	// ownership transferring copy-constructor
	explicit
	signature_base(signature_base&);

private:
	// no default copy-ctor
	signature_base(const signature_base&);

public:
virtual	~signature_base();

	PURE_VIRTUAL_NODE_METHODS
};	// end class signature_base

//=============================================================================
/**
	Abstract base class for basic process information
	(prototype, basically).  
	This class need not be a node or root_item because it's never 
	constructed on the symbol stack.  
 */
class process_signature : public signature_base {
public:
	typedef	never_ptr<const object>			return_type;
protected:
	excl_ptr<const generic_keyword_type>	exp;	///< export keyword
	excl_ptr<const generic_keyword_type>	def;	///< definition keyword
		// should never be NULL, could be const reference?
	/**
		Optional port formal list.  
		Never NULL, but may be empty.  
	 */
	excl_ptr<const port_formal_decl_list>	ports;
	/**
		This is now needed because process-definitions are overloaded
		for use with datatype/struct and channel types.  
	 */
	unsigned char				meta_type;

	/// transferring copy-ctor
	explicit
	process_signature(process_signature&);
public:
	process_signature(
		const generic_keyword_type* e, 
		const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* d, const token_identifier* i, 
		const port_formal_decl_list* p, 
		const unsigned char m);

	// need not be virtual since never use pointers of this type...
virtual	~process_signature();

	return_type
	check_signature(context& c) const;
};	// end class process_signature

//-----------------------------------------------------------------------------
/// process prototype declaration
class process_prototype : public prototype, public process_signature {
public:
	process_prototype(
		const generic_keyword_type* e, 
		const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* d, const token_identifier* i, 
		const port_formal_decl_list* p, 
		const unsigned char);

	~process_prototype();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class process_prototype

//-----------------------------------------------------------------------------
/**
	Process definition, syntax structure.  
 */
class process_def : public definition, public process_signature {
protected:
//	const excl_ptr<const generic_keyword_type>	def;	//  inherited
//	const excl_ptr<const concrete_type_ref>		idt;	//  inherited
//	const excl_ptr<const port_formal_decl_list>	ports;	//  inherited
	const excl_ptr<const definition_body>		body;	///< definition body
public:
	process_def(process_signature& s, const definition_body* b, 
		const unsigned char);

	~process_def();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class process_def

//=============================================================================
/// user-defined data type
class user_data_type_signature : public signature_base {
protected:
	typedef	never_ptr<const object>			return_type;
protected:
	const excl_ptr<const generic_keyword_type>	def;	///< "deftype" keyword
	const excl_ptr<const string_punctuation_type>	dop;	///< <: operator
	const excl_ptr<const concrete_type_ref>	bdt;	///< the represented type
	const excl_ptr<const data_param_decl_list>
						params;	///< implementation type

protected:
	user_data_type_signature(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, 
		const concrete_type_ref* b, 	// or concrete_datatype_ref
		const data_param_decl_list* p);

virtual	~user_data_type_signature();

	return_type
	check_signature(context& c) const;
};	// end class user_data_type_signature

//-----------------------------------------------------------------------------
/// user-defined data type prototype declaration
class user_data_type_prototype : public prototype, 
		public user_data_type_signature {
public:
	user_data_type_prototype(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p);

	~user_data_type_prototype();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class user_data_type_prototype

//-----------------------------------------------------------------------------
/// user-defined data type (is not a type_base)
class user_data_type_def : public definition, public user_data_type_signature {
protected:
	typedef	char_punctuation_type		brace_type;
//	const excl_ptr<const generic_keyword_type>	def;	// inherited
//	const excl_ptr<const token_identifier>	name;	// inherited
//	const excl_ptr<const token_string>	dop;	// inherited
//	const excl_ptr<const concrete_type_ref>	bdt;	// inherited
//	const excl_ptr<const data_param_decl_list>	params;	// inherited
	const excl_ptr<const brace_type>	lb;	///< left brace
	const excl_ptr<const definition_body>	body;
	const excl_ptr<const CHP::body>	setb;	///< set body
	const excl_ptr<const CHP::body>	getb;	///< get body
	const excl_ptr<const brace_type>	rb;	///< right brace
public:
	user_data_type_def(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p, 
		const brace_type* l,
		const definition_body*, 
		const CHP::body* s,
		const CHP::body* g, const brace_type* r);

	~user_data_type_def();

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
};	// end class user_data_type_def

//-----------------------------------------------------------------------------
/**
	Common field data to enumeration declaration and definitions.  
 */
class enum_signature : public signature_base {
protected:
//	const excl_ptr<const token_identifier>	id;	// inherited
	/** "enum" keyword */
	const excl_ptr<const generic_keyword_type>	en;
public:
	enum_signature(const generic_keyword_type* e, const token_identifier* i);

virtual	~enum_signature();

virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class enum_signature

//-----------------------------------------------------------------------------
/**
	Forward declaration of an enum type.  
	Not really a prototype, but classified as such for convenience.  
 */
class enum_prototype : public prototype, public enum_signature {
public:
	enum_prototype(const generic_keyword_type* e,
		const token_identifier* i);

	~enum_prototype();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class enum_prototype

//-----------------------------------------------------------------------------
typedef	node_list<const token_identifier>	enum_member_list_base;

/**
	Specialized list of identifiers for enumeration members.  
 */
class enum_member_list : public enum_member_list_base {
protected:	// no new members
	typedef	enum_member_list_base		parent;
public:
	explicit
	enum_member_list(const token_identifier* i);

	~enum_member_list();

	/** overrides default */
	never_ptr<const object>
	check_build(context& c) const;
};	// end enum_member_list

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Definition of an enumeration data type.
 */
class enum_def : public definition, public enum_signature {
protected:
	/** names of the enumerated members */
	const excl_ptr<const enum_member_list>	members;
public:
	enum_def(const generic_keyword_type* e, const token_identifier* i,      
		const enum_member_list*	m);

	~enum_def();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class enum_def

//=============================================================================
/// user-defined channel type signature
class user_chan_type_signature : public signature_base {
public:
	typedef	never_ptr<const object>			return_type;
protected:
	const excl_ptr<const generic_keyword_type>	def;	///< "defchan" keyword
	const excl_ptr<const string_punctuation_type>	dop;	///< <: operator
	const excl_ptr<const chan_type>		bct;	///< the represented type
	const excl_ptr<const data_param_decl_list>
						params;	///< the implementation type
public:
	user_chan_type_signature(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const chan_type* b, 
		const data_param_decl_list* p);

virtual	~user_chan_type_signature();

	return_type
	check_signature(context& c) const;
};	// end class user_data_type_signature

//-----------------------------------------------------------------------------
/// user-defined channel type prototype
class user_chan_type_prototype : public prototype, 
		public user_chan_type_signature {
public:
	user_chan_type_prototype(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const chan_type* b, 
		const data_param_decl_list* p);
	~user_chan_type_prototype();

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
};	// end class user_chan_type_prototype

//-----------------------------------------------------------------------------
/// user-defined channel type definition
class user_chan_type_def : public definition, public user_chan_type_signature {
protected:
//	const excl_ptr<const generic_keyword_type>	def;	// inherited
//	const excl_ptr<const token_identifier>	name;	// inherited
//	const excl_ptr<const token_string>	dop;	// inherited
//	const excl_ptr<const chan_type>		bct;	// inherited
//	const excl_ptr<const data_param_decl_list>	params;	// inherited
	const excl_ptr<const char_punctuation_type>	lb;	///< left brace
	const excl_ptr<const definition_body>	body;
	const excl_ptr<const CHP::body>		sendb;	///< set body
	const excl_ptr<const CHP::body>		recvb;	///< get body
	const excl_ptr<const char_punctuation_type>	rb;	///< right brace
public:
	user_chan_type_def(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const chan_type* b, 
		const data_param_decl_list* p, 
		const char_punctuation_type* l,
		const definition_body*, 
		const CHP::body* s, 
		const CHP::body* g, const char_punctuation_type* r);

	~user_chan_type_def();

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
};	// end class user_chan_type_def

//=============================================================================
/**
	Typedef statement.  
 */
class typedef_alias : public def_body_item {
protected:
	const excl_ptr<const template_formal_decl_list_pair>	temp_spec;
	const excl_ptr<const generic_keyword_type>		td;
	const excl_ptr<const concrete_type_ref>		base;
	const excl_ptr<const token_identifier>		id;
public:
	typedef_alias(const template_formal_decl_list_pair* t, 
		const generic_keyword_type* k, const concrete_type_ref* b, 
		const token_identifier* i);

	~typedef_alias();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	never_ptr<const object>
	check_build(context& c) const;
};	// end class typedef_alias

//=============================================================================

}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_DEFINITION_H__

