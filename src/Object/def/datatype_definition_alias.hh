/**
	\file "Object/def/datatype_definition_alias.hh"
	Definition-related HAC object classes.  
	This file came from "Object/art_object_definition_data.h".
	$Id: datatype_definition_alias.hh,v 1.11 2010/04/02 22:18:09 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_DATATYPE_DEFINITION_ALIAS_H__
#define	__HAC_OBJECT_DEF_DATATYPE_DEFINITION_ALIAS_H__

#include "Object/def/datatype_definition_base.hh"
#include "Object/def/typedef_base.hh"

namespace HAC {
namespace entity {
//=============================================================================
/**
	Data-type typedef.  
 */
class datatype_definition_alias : public datatype_definition_base, 
		public typedef_base {
private:
	typedef	datatype_definition_alias	this_type;
protected:
	const string				key;
	const never_ptr<const scopespace>	parent;
	// inherited template formals
	excl_ptr<const data_type_reference>	base;
private:
	datatype_definition_alias();

public:
	datatype_definition_alias(const string& n, 
		const never_ptr<const scopespace> p);

	~datatype_definition_alias();

	ostream&
	what(ostream& o) const;

	const string&
	get_key(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const fundamental_type_reference>
	get_base_type_ref(void) const;

	never_ptr<const scopespace>
	get_scopespace(void) const;

	/**
		Since user-def-data-types have port managers, we must resolve
		through the canonical definition.
	 */
	never_ptr<const port_formals_manager>
	get_port_formals_manager(void) const;

	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const;

	bool
	assign_typedef(excl_ptr<const fundamental_type_reference>& f);

	CERTIFY_PORT_ACTUALS_PROTO;

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_DATA_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_DATA_TYPE_PROTO;

	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const;

	REGISTER_COMPLETE_TYPE_PROTO;	// bogus
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

};	// end class datatype_definition_alias

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_DATATYPE_DEFINITION_ALIAS_H__

