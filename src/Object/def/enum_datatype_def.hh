/**
	\file "Object/def/enum_datatype_def.hh"
	Definition-related HAC object classes.  
	This file came from "Object/art_object_definition_data.h".
	$Id: enum_datatype_def.hh,v 1.12 2010/04/02 22:18:12 fang Exp $
 */

#ifndef	__HAC_OBJECT_DEF_ENUM_DATATYPE_DEF_H__
#define	__HAC_OBJECT_DEF_ENUM_DATATYPE_DEF_H__

#include "Object/def/datatype_definition_base.hh"
#include "Object/common/scopespace.hh"

namespace HAC {
namespace entity {

//=============================================================================
/**
	Member of an enumeration, just an identifier.  
	Kludge: derived from object, so it is storable as a used_id_map
	entity.  
	Need not be derived from persistent, since binary data
	consists of just a string, and need not be dynamically allocated.  
 */
class enum_member : public object {
private:
	typedef	enum_member		this_type;
protected:
	const string			id;
public:
	explicit
	enum_member(const string& n) : object(), id(n) { }

	~enum_member() { }

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;
};	// end class enum_member

//-----------------------------------------------------------------------------
/**
	Enumerations are special fundamental types of data, like int and bool.  
	There are no built in enumerations, all are user-defined.  
 */
class enum_datatype_def : public datatype_definition_base, public scopespace {
private:
	typedef	enum_datatype_def			this_type;
protected:
	const string					key;
	const never_ptr<const name_space>		parent;
	// realy this has no template arguments...
	// don't we need to track ordering of identifiers added?  later...
private:
	enum_datatype_def();
public:
	enum_datatype_def(never_ptr<const name_space> o, const string& n);
	~enum_datatype_def();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	meta_type_tag_enum
	get_meta_type(void) const;

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

	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const;

	CERTIFY_PORT_ACTUALS_PROTO;

	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_DATA_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_DATA_TYPE_PROTO;

	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const;

	bool
	add_member(const token_identifier& em);

	REGISTER_COMPLETE_TYPE_PROTO;
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

};	// end class enum_datatype_def

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_DEF_ENUM_DATATYPE_DEF_H__

