/**
	\file "Object/art_object_definition_data.h"
	Definition-related ART object classes.  
	$Id: art_object_definition_data.h,v 1.1 2005/06/22 22:13:34 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DEFINITION_DATA_H__
#define	__OBJECT_ART_OBJECT_DEFINITION_DATA_H__

#include "Object/art_object_definition.h"
#include "Object/art_object_port_formals_manager.h"
#include "Object/art_object_CHP.h"

namespace ART {
//=============================================================================
namespace entity {
class data_type_reference;

//=============================================================================
/**
	Base class interface for data type definitions.  
 */
class datatype_definition_base : virtual public definition_base {
private:
	typedef	definition_base				parent_type;
public:
	datatype_definition_base() : definition_base() { }

virtual	~datatype_definition_base();

	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const;

virtual	ostream&
	what(ostream& o) const = 0;

	using parent_type::dump;

	/**
		Resolves the underlying type, without regard to the 
		template arguments of any typedefs, for the sake of 
		resolving the category: built-in, enum, or struct.
	 */
virtual	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const = 0;

virtual	count_ptr<const fundamental_type_reference>
	make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const = 0;

virtual	good_bool
	require_signature_match(
		const never_ptr<const definition_base> d) const = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class datatype_definition_base

//=============================================================================
/**
	Reserved for special built-in fundamental data types.  
	All user-defined data types will boil down to these types.  
	Final class.  
	Need to derive from scopespace because, built-in data type
	int's template parameter, pint, needs a valid owner scopespace.  
 */
class built_in_datatype_def : public datatype_definition_base, 
		public scopespace {
private:
	typedef	built_in_datatype_def			this_type;
protected:
	const string					key;
	const never_ptr<const name_space>		parent;
public:
	built_in_datatype_def(never_ptr<const name_space> o, const string& n);

	~built_in_datatype_def();

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	const string&
	get_key(void) const;

	string
	get_qualified_name(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const;

	count_ptr<const fundamental_type_reference>
	make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const;
	// overrides definition_base's, exception to rule
	// because this is not a scopespace
	// ah, but it is now!

	never_ptr<const instance_collection_base>
	add_template_formal(excl_ptr<instance_collection_base>& f);

	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const
		{ assert(d); return good_bool(key == d->get_name()); }
		// really, this should never be called...
public:
	// actually going to de/serialize built-in type, only to be
	// intercepted and replaced by data_type_reference::load_object
	PERSISTENT_METHODS_DECLARATIONS
	// thus we need only collect and write...
private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
};	// end class built_in_datatype_def

//-----------------------------------------------------------------------------
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

	const string&
	get_key(void) const;

	string
	get_qualified_name(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const;

	count_ptr<const fundamental_type_reference>
	make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const;

	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const;

	bool
	add_member(const token_identifier& em);
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);

};	// end class enum_datatype_def

//-----------------------------------------------------------------------------
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

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const;

	never_ptr<const object>
	lookup_object_here(const string& id) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	DEFINITION_ADD_PORT_FORMAL_PROTO;

	void
	attach_base_data_type(const count_ptr<const data_type_reference>&);

	CHP::action_sequence&
	get_set_body(void) { return set_chp; }

	// yeah, I know: funny name
	CHP::action_sequence&
	get_get_body(void) { return get_chp; }

	never_ptr<const instance_collection_base>
	lookup_port_formal(const string&) const;

	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const
		{ return good_bool(false); }	// temporary

	count_ptr<const fundamental_type_reference>
	make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const;

#if 0
	good_bool
	certify_port_actuals(const object_list& ol) const;
#endif
public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);
};	// end class user_def_datatype

//-----------------------------------------------------------------------------
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

	never_ptr<const datatype_definition_base>
	resolve_canonical_datatype_definition(void) const;

	bool
	assign_typedef(excl_ptr<const fundamental_type_reference>& f);

	count_ptr<const fundamental_type_reference>
	make_fundamental_type_reference(
		excl_ptr<dynamic_param_expr_list>& ta) const;

	good_bool
	require_signature_match(const never_ptr<const definition_base> d) const;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);

};	// end class datatype_definition_alias

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DEFINITION_DATA_H__

