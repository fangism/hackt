/**
	\file "art_parser_definition.h"
	Definition-related parser classes for ART.  
	$Id: art_parser_definition.h,v 1.11 2005/05/04 17:54:10 fang Exp $
 */

#ifndef __ART_PARSER_DEFINITION_H__
#define __ART_PARSER_DEFINITION_H__

#include "art_parser_root_item.h"
#include "art_parser_type.h"
#include "art_parser_definition_item.h"

namespace ART {
namespace parser {
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

virtual	ostream&
	what(ostream& o) const = 0;

virtual	line_position
	leftmost(void) const = 0;

virtual	line_position
	rightmost(void) const = 0;
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
	const excl_ptr<const template_formal_decl_list_pair>	temp_spec;
	const excl_ptr<const token_identifier>		id;
public:
	signature_base(const template_formal_decl_list_pair* tf, 
		const token_identifier* i) : temp_spec(tf), id(i) { }

virtual	~signature_base();

PURE_VIRTUAL_NODE_METHODS

virtual	never_ptr<const object>
	check_build(context& c) const = 0;
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
	const excl_ptr<const generic_keyword_type>	def;	///< definition keyword
		// should never be NULL, could be const reference?
	/**
		Optional port formal list.  
		Never NULL, but may be empty.  
	 */
	const excl_ptr<const port_formal_decl_list>	ports;
public:
	process_signature(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* d, const token_identifier* i, 
		const port_formal_decl_list* p);

virtual	~process_signature();

// note: non-virtual
	const token_identifier&
	get_name(void) const;

virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class process_signature

//-----------------------------------------------------------------------------
/// process prototype declaration
class process_prototype : public prototype, public process_signature {
protected:
//	const excl_ptr<const char_punctuation_type>	semi;	///< semicolon token
public:
	process_prototype(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* d, const token_identifier* i, 
		const port_formal_decl_list* p);

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
	process_def(const template_formal_decl_list_pair*, 
		const generic_keyword_type* d, const token_identifier* i, 
		const port_formal_decl_list* p, const definition_body* b);

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
	const excl_ptr<const generic_keyword_type>	def;	///< "deftype" keyword
	const excl_ptr<const string_punctuation_type>	dop;	///< <: operator
	const excl_ptr<const concrete_type_ref>	bdt;	///< the represented type
	const excl_ptr<const data_param_decl_list>
						params;	///< implementation type
public:
	user_data_type_signature(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, 
		const concrete_type_ref* b, 	// or concrete_datatype_ref
		const data_param_decl_list* p);

virtual	~user_data_type_signature();

virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class user_data_type_signature

//-----------------------------------------------------------------------------
/// user-defined data type prototype declaration
class user_data_type_prototype : public prototype, 
		public user_data_type_signature {
protected:
//	const excl_ptr<const char_punctuation_type>	semi;	///< semicolon
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
	const excl_ptr<const char_punctuation_type>	lb;	///< left brace
	const excl_ptr<const language_body>	setb;	///< set body
	const excl_ptr<const language_body>	getb;	///< get body
	const excl_ptr<const char_punctuation_type>	rb;	///< right brace
public:
	user_data_type_def(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const concrete_type_ref* b, 
		const data_param_decl_list* p, 
		const brace_type* l, const language_body* s,
		const language_body* g, const brace_type* r);

	~user_data_type_def();

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 0
	using user_data_type_signature::check_build;
#else
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
protected:
	/** semicolon token (optional) */
//	const excl_ptr<const char_punctuation_type>	semi;
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

virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class user_data_type_signature

//-----------------------------------------------------------------------------
/// user-defined channel type prototype
class user_chan_type_prototype : public prototype, 
		public user_chan_type_signature {
protected:
//	const excl_ptr<const char_punctuation_type>	semi;	///< semicolon
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
	const excl_ptr<const language_body>	sendb;	///< set body
	const excl_ptr<const language_body>	recvb;	///< get body
	const excl_ptr<const char_punctuation_type>	rb;	///< right brace
public:
	user_chan_type_def(const template_formal_decl_list_pair* tf, 
		const generic_keyword_type* df, const token_identifier* n, 
		const string_punctuation_type* dp, const chan_type* b, 
		const data_param_decl_list* p, 
		const char_punctuation_type* l, const language_body* s, 
		const language_body* g, const char_punctuation_type* r);

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

class typedef_alias : public def_body_item, public root_item {
protected:
	const excl_ptr<const template_formal_decl_list_pair>	temp_spec;
	const excl_ptr<const generic_keyword_type>		td;
	const excl_ptr<const concrete_type_ref>		base;
	const excl_ptr<const token_identifier>		id;
//	const excl_ptr<const char_punctuation_type>		semi;
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
}	// end namespace ART

#endif	// __ART_PARSER_DEFINITION_H__

