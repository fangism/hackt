/**
	\file "AST/art_parser_type.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_type.h,v 1.7.2.4 2005/05/29 02:08:27 fang Exp $
 */

#ifndef __AST_ART_PARSER_TYPE_H__
#define __AST_ART_PARSER_TYPE_H__

#include "AST/art_parser_type_base.h"
#include "AST/art_parser_expr_list.h"
#include "util/memory/count_ptr.h"
#include "util/boolean_types.h"

namespace ART {
namespace entity {
	class fundamental_type_reference;
	class builtin_channel_type_reference;
}
namespace parser {
using util::good_bool;
using entity::fundamental_type_reference;
using entity::builtin_channel_type_reference;

//-----------------------------------------------------------------------------
/**
	Type identifier.
	Template parameters are separate.  
	Just wraps around id_expr;
	Can refer to a any user-defined data, channel, or process type.  
 */
class type_id : public type_base {
protected:
	const excl_ptr<const qualified_id>	base;
public:
	explicit
	type_id(const qualified_id* b);

virtual	~type_id();

virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

virtual	TYPE_BASE_CHECK_PROTO;

	friend
	ostream&
	operator << (ostream& o, const type_id& id);
};	// end class type_id

//-----------------------------------------------------------------------------
typedef node_list<const concrete_type_ref>		data_type_ref_list_base;
	// consider making concrete_datatype_ref sub-class
	// or overriding class's check_build

/// list of base data types
class data_type_ref_list : public data_type_ref_list_base {
public:
	typedef	count_ptr<const builtin_channel_type_reference>	return_type;
protected:
	typedef	data_type_ref_list_base			parent_type;
public:
	explicit
	data_type_ref_list(const concrete_type_ref* c);

	~data_type_ref_list();

#if 0
	good_bool
	check_data_types(context& c) const;
#else
	return_type
	check_builtin_channel_type(context&) const;
#endif

#if 0
	// functor for list transformation.  
	struct datatype_caster;
#endif

};	// end class data_type_ref_list

//=============================================================================
class concrete_type_ref {
public:
	typedef	count_ptr<const fundamental_type_reference>	return_type;

public:
	concrete_type_ref() { }
virtual	~concrete_type_ref() { }

	PURE_VIRTUAL_NODE_METHODS

virtual	return_type
	check_type(context&) const = 0;
};	// end class concrete_type_ref

//-----------------------------------------------------------------------------
/**
	Reference to a concrete type, i.e. definition with its
	template parameters specified (if applicable).

	NOTE (2005-05-12):
	Eventually this may be a valid template argument once we support
	template type arguments in addition to parameters.  
	Plan: derive this from expr, introduce new virtual functions
	to handle cases where subtypes are expected.  
 */
class generic_type_ref : public concrete_type_ref {
	typedef	concrete_type_ref			parent_type;
public:
	typedef	parent_type::return_type		return_type;
protected:
	/** definition name base */
	const excl_ptr<const type_base>			base;
	/**
		Optional template arguments.
		TODO: Needs to be split into strict and relaxed arguments.  
		(see "AST/art_parser_expr_list.h":template_argument_list_pair.)
	 */
	const excl_ptr<const expr_list>			temp_spec;
public:
	explicit
	generic_type_ref(const type_base* n, const expr_list* t = NULL);

	~generic_type_ref();

	never_ptr<const type_base>
	get_base_def(void) const;

	never_ptr<const expr_list>
	get_temp_spec(void) const;

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 0
	never_ptr<const object>
	check_build(context& c) const;
#endif

	return_type
	check_type(context&) const;

};	// end class concrete_type_ref

//-----------------------------------------------------------------------------
/**
	Built-in channel type reference.  
	Full base channel type, including base type list.
	So far, nothing derives from this...
 */
class chan_type : public concrete_type_ref {
	typedef	concrete_type_ref			parent_type;
public:
	typedef	parent_type::return_type		return_type;
protected:
	const excl_ptr<const generic_keyword_type>	chan;	///< keyword "channel"
	const excl_ptr<const char_punctuation_type>	dir;	///< port direction: in or out
	excl_ptr<const data_type_ref_list>	dtypes;	///< data types communicated
public:
	chan_type(const generic_keyword_type* c,
		const char_punctuation_type* d = NULL, 
		const data_type_ref_list* t = NULL);

	~chan_type();

	chan_type*
	attach_data_types(const data_type_ref_list* t);

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

#if 0
	TYPE_BASE_CHECK_PROTO;
#else
	return_type
	check_type(context&) const;
#endif

#if 0
	// dedicated for user_chan_type_signature
	good_bool
	check_base_chan_type(context&) const;
#endif
};	// end class chan_type

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __AST_ART_PARSER_TYPE_H__

