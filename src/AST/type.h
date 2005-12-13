/**
	\file "AST/type.h"
	Base set of classes for the HAC parser.  
	$Id: type.h,v 1.2 2005/12/13 04:15:14 fang Exp $
	This file used to be the following before it was renamed:
	$Id: type.h,v 1.2 2005/12/13 04:15:14 fang Exp $
 */

#ifndef __HAC_AST_TYPE_H__
#define __HAC_AST_TYPE_H__

#include "AST/type_base.h"
#include "AST/node_list.h"
#include "util/memory/excl_ptr.h"
#include "util/boolean_types.h"

namespace HAC {
namespace entity {
	class builtin_channel_type_reference;
}
namespace parser {
using util::good_bool;
using util::memory::excl_ptr;
using entity::builtin_channel_type_reference;
class expr_list;
class qualified_id;
class template_argument_list_pair;

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
	/// returns as non const so direction may be assigned
	typedef	count_ptr<builtin_channel_type_reference>	return_type;
protected:
	typedef	data_type_ref_list_base			parent_type;
public:
	explicit
	data_type_ref_list(const concrete_type_ref* c);

	~data_type_ref_list();

	return_type
	check_builtin_channel_type(context&) const;

};	// end class data_type_ref_list

//=============================================================================
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
//	typedef expr_list				template_args_type;
	typedef template_argument_list_pair		template_args_type;
public:
	typedef	parent_type::return_type		return_type;
protected:
	/** definition name base */
	const excl_ptr<const type_base>			base;
	/**
		Optional template arguments.
		TODO: Needs to be split into strict and relaxed arguments.  
		(see "AST/expr_list.h":template_argument_list_pair.)
	 */
	const excl_ptr<const template_args_type>		temp_spec;
	/**
		Optional channel direction, only applies to channels.  
	 */
	const excl_ptr<const char_punctuation_type>	chan_dir;
public:
	explicit
	generic_type_ref(const type_base* n,
		const template_args_type* t = NULL,
		const char_punctuation_type* d = NULL);

	~generic_type_ref();

	never_ptr<const type_base>
	get_base_def(void) const;

	never_ptr<const template_args_type>
	get_temp_spec(void) const;

	ostream&
	what(ostream& o) const;

	line_position
	leftmost(void) const;

	line_position
	rightmost(void) const;

	return_type
	check_type(context&) const;

};	// end class generic_type_ref

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

	return_type
	check_type(context&) const;

};	// end class chan_type

//=============================================================================

}	// end namespace parser
}	// end namespace HAC

#endif	// __HAC_AST_TYPE_H__

