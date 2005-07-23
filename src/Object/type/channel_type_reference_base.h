/**
	\file "Object/type/channel_type_reference_base.h"
	Type-reference classes of the ART language.  
	TODO: must pool-allocate these, they're created frequently!
	This file originated from "Object/art_object_type_ref.h"
		in a previous life.  
 	$Id: channel_type_reference_base.h,v 1.2 2005/07/23 06:52:53 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CHANNEL_TYPE_REFERENCE_BASE_H__
#define	__OBJECT_TYPE_CHANNEL_TYPE_REFERENCE_BASE_H__

#include "Object/type/fundamental_type_reference.h"

namespace ART {
namespace entity {
class unroll_context;
class builtin_channel_type_reference;
using std::ostream;

//=============================================================================
/**
	Abstract parent class for all channel types.  
 */
class channel_type_reference_base : public fundamental_type_reference {
	typedef	channel_type_reference_base		this_type;
protected:
	typedef	fundamental_type_reference		parent_type;
public:
#if 0
	typedef	enum {
		BIDIRECTIONAL, 
		SEND, 
		RECEIVE
	}	direction_type;
#endif
protected:
	/**
		Three possible values: '\0' means bidirections (unspecified), 
		'!' means send-only, '?' means receive-only.
	 */
	char						direction;
protected:
	channel_type_reference_base() : parent_type(), direction('\0') { }

	explicit
	channel_type_reference_base(const template_actuals&);

public:
virtual	~channel_type_reference_base() { }

virtual	ostream&
	dump(ostream&) const = 0;

	void
	set_direction(const char c) { direction = c; }

	char
	get_direction(void) const { return direction; }

	ostream&
	dump_direction(ostream&) const;

virtual	count_ptr<const this_type>
	unroll_resolve(const unroll_context&) const = 0;

virtual	UNROLL_PORT_INSTANCES_PROTO = 0;

virtual	never_ptr<const builtin_channel_type_reference>
	resolve_builtin_channel_type(void) const = 0;

virtual	unroll_context
	make_unroll_context(void) const = 0;
protected:
	using parent_type::collect_transient_info_base;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class channel_type_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_CHANNEL_TYPE_REFERENCE_BASE_H__

