/**
	\file "Object/def/user_def_datatype.hh"
	Definition-related HAC object classes.  
	This file came from "Object/art_object_definition_data.h".
	$Id: user_def_datatype.hh,v 1.14 2010/04/02 22:18:17 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_USER_DEF_DATATYPE_H__
#define	__HAC_OBJECT_DEF_USER_DEF_DATATYPE_H__

#include "Object/def/datatype_definition_base.hh"
#include "Object/unroll/sequential_scope.hh"
#include "Object/common/scopespace.hh"
#include "Object/def/port_formals_manager.hh"
#include "Object/lang/CHP.hh"
#include "Object/def/footprint_manager.hh"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Generalizable user-defined data type, which can (eventually) 
	build upon other user-defined data types.  
 */
class user_def_datatype : public datatype_definition_base, public scopespace, 
		public sequential_scope {
private:
	typedef	user_def_datatype		this_type;
protected:
	const string				key;
	const never_ptr<const name_space>	parent;
	count_ptr<const data_type_reference>	base_type;
	port_formals_manager			port_formals;
	CHP::action_sequence			set_chp;
	CHP::action_sequence			get_chp;
	mutable footprint_manager		footprint_map;
private:
	user_def_datatype();
public:
	user_def_datatype(const never_ptr<const name_space> o, 
		const string& name);
	~user_def_datatype();

	const string&
	get_key(void) const;

	string
	get_qualified_name(void) const;

	ostream&
	dump_qualified_name(ostream&, const dump_flags&) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const scopespace>
	get_scopespace(void) const;

	const port_formals_manager&
	get_port_formals(void) const { return port_formals; }

	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const;

	never_ptr<const object>
	lookup_member(const string& id) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	commit_arity(void);

	DEFINITION_ADD_PORT_FORMAL_PROTO;

	void
	attach_base_data_type(const count_ptr<const data_type_reference>&);

	CHP::action_sequence&
	get_set_body(void) { return set_chp; }

	// yeah, I know: funny name
	CHP::action_sequence&
	get_get_body(void) { return get_chp; }

	never_ptr<const port_formals_manager>
	get_port_formals_manager(void) const;

	good_bool
	require_signature_match(const never_ptr<const definition_base>) const
		{ return good_bool(false); }	// temporary

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_DATA_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_DATA_TYPE_PROTO;

	const footprint&
	get_footprint(const count_ptr<const const_param_expr_list>&) const;

	CERTIFY_PORT_ACTUALS_PROTO;
	REGISTER_COMPLETE_TYPE_PROTO;
private:
	good_bool
	__unroll_complete_type(const count_ptr<const const_param_expr_list>&, 
		const footprint&) const;
public:
	CREATE_COMPLETE_TYPE_PROTO;

	void
	count_channel_member(fundamental_channel_footprint&) const;

	DUMP_CHANNEL_FIELD_ITERATE_PROTO;
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);

	meta_type_tag_enum
	get_meta_type(void) const;

};	// end class user_def_datatype

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_USER_DEF_DATATYPE_H__

