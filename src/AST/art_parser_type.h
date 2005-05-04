/**
	\file "art_parser_type.h"
	Base set of classes for the ART parser.  
	$Id: art_parser_type.h,v 1.3.4.3 2005/05/04 17:23:20 fang Exp $
 */

#ifndef __ART_PARSER_TYPE_H__
#define __ART_PARSER_TYPE_H__

#include "art_parser_type_base.h"
#include "art_parser_expr_list.h"
#include "memory/count_ptr.h"

namespace ART {
namespace parser {
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

virtual	never_ptr<const object>
	check_build(context& c) const;

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
protected:
	typedef	data_type_ref_list_base			parent_type;
public:
	explicit
	data_type_ref_list(const concrete_type_ref* c);

	~data_type_ref_list();
};

//-----------------------------------------------------------------------------
/// full base channel type, including base type list
class chan_type : public type_base {
protected:
	const excl_ptr<const generic_keyword_type>	chan;	///< keyword "channel"
	const excl_ptr<const token_char>	dir;	///< port direction: in or out
	excl_ptr<const data_type_ref_list>	dtypes;	///< data types communicated
public:
	chan_type(const generic_keyword_type* c, const token_char* d = NULL, 
		const data_type_ref_list* t = NULL);

virtual	~chan_type();

	chan_type*
	attach_data_types(const data_type_ref_list* t);

virtual	ostream&
	what(ostream& o) const;

virtual	line_position
	leftmost(void) const;

virtual	line_position
	rightmost(void) const;

virtual	never_ptr<const object>
	check_build(context& c) const;
};	// end class chan_type

//=============================================================================
/**
	Reference to a concrete type, i.e. definition with its
	template parameters specified (if applicable).
 */
class concrete_type_ref {
protected:
	/** definition name base */
	const excl_ptr<const type_base>			base;
	/**
		Optional template arguments.
		TODO: Needs to be split into strict and relaxed arguments.  
	 */
	const excl_ptr<const expr_list>			temp_spec;
public:
	explicit
	concrete_type_ref(const type_base* n, const expr_list* t = NULL);

	~concrete_type_ref();

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

	never_ptr<const object>
	check_build(context& c) const;
};	// end class concrete_type_ref

//=============================================================================

}	// end namespace parser
}	// end namespace ART

#endif	// __ART_PARSER_TYPE_H__

