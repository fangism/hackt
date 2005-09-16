/**
	\file "Object/def/user_def_chan.h"
	Definition-related ART object classes.  
	This file came from "Object/art_object_definition_chan.h". 
	$Id: user_def_chan.h,v 1.3.4.1 2005/09/16 07:19:39 fang Exp $
 */

#ifndef	__OBJECT_DEF_USER_DEF_CHAN_H__
#define	__OBJECT_DEF_USER_DEF_CHAN_H__

#include "Object/def/channel_definition_base.h"
#include "Object/common/scopespace.h"
#include "Object/unroll/sequential_scope.h"
#include "Object/def/port_formals_manager.h"
#include "Object/lang/CHP.h"

namespace ART {
namespace entity {
class channel_type_reference;
//=============================================================================
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
public:
	typedef	channel_type_reference		type_reference_type;
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
	dump_qualified_name(ostream&, const dump_flags&) const;

	never_ptr<const scopespace>
	get_parent(void) const;

	never_ptr<const object>
	lookup_object_here(const string& id) const;

	ostream&
	what(ostream& o) const;

	ostream&
	dump(ostream& o) const;

	void
	commit_arity(void);

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

	size_t
	lookup_port_formal_position(const instance_collection_base&) const;

	good_bool
	certify_port_actuals(const checked_refs_type&) const;

	MAKE_CANONICAL_CHANNEL_TYPE_REFERENCE_PROTO;

	MAKE_CANONICAL_CHANNEL_TYPE_PROTO;

	REGISTER_COMPLETE_TYPE_PROTO;
	UNROLL_COMPLETE_TYPE_PROTO;
	CREATE_COMPLETE_TYPE_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS

private:
	void
	load_used_id_map_object(excl_ptr<persistent>& o);

};	// end class user_def_chan

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_DEF_USER_DEF_CHAN_H__

