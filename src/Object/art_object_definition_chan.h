/**
	\file "Object/art_object_definition_chan.h"
	Definition-related ART object classes.  
	$Id: art_object_definition_chan.h,v 1.5.2.1.2.2 2005/07/15 03:18:38 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_DEFINITION_CHAN_H__
#define	__OBJECT_ART_OBJECT_DEFINITION_CHAN_H__

#include "Object/art_object_definition.h"
#include "Object/art_object_port_formals_manager.h"
#include "Object/art_object_CHP.h"

namespace ART {
namespace entity {
class builtin_channel_type_reference;
class channel_type_reference_base;
class channel_type_reference;

//=============================================================================
/// abstract base class for channels and their representations
class channel_definition_base : virtual public definition_base {
private:
	typedef	definition_base			parent_type;
protected:
	channel_definition_base() : parent_type() { }
public:
virtual	~channel_definition_base() { }

	excl_ptr<definition_base>
	make_typedef(never_ptr<const scopespace> s, 
		const token_identifier& id) const;

virtual	MAKE_FUNDAMENTAL_TYPE_REFERENCE_PROTO;

#define	MAKE_CANONICAL_CHANNEL_TYPE_REFERENCE_PROTO			\
	count_ptr<const channel_type_reference_base>			\
	make_canonical_type_reference(const template_actuals&) const

virtual	MAKE_CANONICAL_CHANNEL_TYPE_REFERENCE_PROTO = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class channel_definition_base

//-----------------------------------------------------------------------------
/**
	Generalizable user-defined channel type, which can (eventually) 
	build upon other user-defined channel types.  
 */
class user_def_chan : public channel_definition_base, public scopespace,
		public sequential_scope {
private:
	typedef	user_def_chan			this_type;
	typedef channel_definition_base		definition_parent_type;
	typedef	scopespace			scope_parent_type;
	typedef	sequential_scope		sequential_parent_type;
protected:
	// list of other type definitions
	const string				key;
	const never_ptr<const name_space>	parent;
	count_ptr<const builtin_channel_type_reference>
						base_chan_type_ref;
	port_formals_manager			port_formals;
	CHP::action_sequence			send_chp;
	CHP::action_sequence			recv_chp;
private:
	user_def_chan();
public:
	user_def_chan(const never_ptr<const name_space> o, const string& name);
	~user_def_chan();

	const string&
	get_key(void) const;

	string
	get_qualified_name(void) const;

	ostream&
	dump_qualified_name(ostream&) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const object>
	lookup_object_here(const string& id) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	const builtin_channel_type_reference&
	get_builtin_channel_type(void) const {
		return *base_chan_type_ref;
	}

	void
	attach_base_channel_type(
		const count_ptr<const builtin_channel_type_reference>&);

	CHP::action_sequence&
	get_send_body(void) { return send_chp; }

	CHP::action_sequence&
	get_recv_body(void) { return recv_chp; }

	// from class definition_base
	DEFINITION_ADD_PORT_FORMAL_PROTO;

	never_ptr<const instance_collection_base>
	lookup_port_formal(const string&) const;

	good_bool
	certify_port_actuals(const checked_refs_type&) const;

	MAKE_CANONICAL_CHANNEL_TYPE_REFERENCE_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);

};	// end class user_def_chan

//-----------------------------------------------------------------------------
/**
	Built-in channel definition type.  (commented 2005-05-27)
	How do these definitions get created? and who manages them?
	They are just referenced anoymously.  
	The datatypes carried are like template parameters, but not really.  
	Do we need a notion of built-in channel parameters?
	Do we need to keep a static repository of seen types?
	How will this be handled with respect to object persistence?
	Would it be easier to sub-type channel type references?
	Since built-in channel types are not templated, they can be handled
	as a special case of concrete_type_ref: built-in chan type ref.
 */
class built_in_channel_def : public channel_definition_base {
private:
public:
	built_in_channel_def();
	~built_in_channel_def();

};	// end class built_in_channel_def

//-----------------------------------------------------------------------------
/**
	Alias to a channel-type.  
 */
class channel_definition_alias : public channel_definition_base, 
		public typedef_base {
private:
	typedef	channel_definition_alias	this_type;
protected:
	const string				key;
	const never_ptr<const scopespace>	parent;
	/**
		Not channel_type_reference_base?
	 */
	excl_ptr<const channel_type_reference>	base;
private:
	channel_definition_alias();
public:
	channel_definition_alias(const string& n, 
		const never_ptr<const scopespace> p);

	~channel_definition_alias();

	ostream&
	what(ostream& o) const;

	const string&
	get_key(void) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const fundamental_type_reference>
	get_base_type_ref(void) const;

	bool
	assign_typedef(excl_ptr<const fundamental_type_reference>& f);

	MAKE_CANONICAL_CHANNEL_TYPE_REFERENCE_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);

};	// end class channel_definition_alias

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_DEFINITION_CHAN_H__

