/**
	\file "Object/type/channel_type_reference_base.hh"
	Type-reference classes of the HAC language.  
	TODO: must pool-allocate these, they're created frequently!
	This file originated from "Object/art_object_type_ref.h"
		in a previous life.  
 	$Id: channel_type_reference_base.hh,v 1.11 2011/03/23 00:36:19 fang Exp $
 */

#ifndef	__HAC_OBJECT_TYPE_CHANNEL_TYPE_REFERENCE_BASE_H__
#define	__HAC_OBJECT_TYPE_CHANNEL_TYPE_REFERENCE_BASE_H__

#include "Object/type/fundamental_type_reference.hh"
#include "Object/type/canonical_type_fwd.hh"
#include "Object/devel_switches.hh"
#include "Object/type/channel_direction_enum.hh"

namespace HAC {
namespace entity {
class unroll_context;
class channel_definition_base;
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
protected:
	/**
		Meaning is defined by the direction_type enumeration.  
	 */
	direction_type					direction;
protected:
	channel_type_reference_base() : 
		parent_type(), direction(CHANNEL_DIRECTION_DEFAULT) { }

	explicit
	channel_type_reference_base(const template_actuals&);

public:
virtual	~channel_type_reference_base() { }

virtual	ostream&
	dump(ostream&) const = 0;

	void
	set_direction(const direction_type c) { direction = c; }

	direction_type
	get_direction(void) const { return direction; }

	static
	ostream&
	dump_direction(ostream&, const direction_type);

	bool
	is_accepted_in_datatype(void) const;

	bool
	is_accepted_in_channel(void) const;

virtual	count_ptr<const this_type>
	unroll_resolve(const unroll_context&) const = 0;

virtual	never_ptr<const builtin_channel_type_reference>
	resolve_builtin_channel_type(void) const = 0;

	// should be pure virtual, just declare for now
#if !BUILTIN_CHANNEL_FOOTPRINTS
virtual	canonical_type<channel_definition_base>
	make_canonical_type(void) const = 0;
#endif

protected:
	using parent_type::collect_transient_info_base;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);

};	// end class channel_type_reference_base

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_TYPE_CHANNEL_TYPE_REFERENCE_BASE_H__

