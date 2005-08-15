/**
	\file "Object/def/channel_definition_alias.h"
	Definition-related ART object classes.  
	This file came from "art_object_definition_chan.h".
	$Id: channel_definition_alias.h,v 1.2.8.1 2005/08/15 21:12:07 fang Exp $
 */

#ifndef	__OBJECT_DEF_CHANNEL_DEFINITION_ALIAS_H__
#define	__OBJECT_DEF_CHANNEL_DEFINITION_ALIAS_H__

#include "Object/def/channel_definition_base.h"
#include "Object/def/typedef_base.h"

namespace ART {
namespace entity {
//=============================================================================
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

	MAKE_CANONICAL_CHANNEL_TYPE_PROTO;

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

#endif	// __OBJECT_DEF_CHANNEL_DEFINITION_ALIAS_H__

