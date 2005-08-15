/**
	\file "Object/def/channel_definition_base.h"
	Channel definition-related ART object classes.  
	This file came from "Object/art_object_definition_chan.h".
	$Id: channel_definition_base.h,v 1.2.8.1 2005/08/15 21:12:07 fang Exp $
 */

#ifndef	__OBJECT_DEF_CHANNEL_DEFINITION_BASE_H__
#define	__OBJECT_DEF_CHANNEL_DEFINITION_BASE_H__

#include "Object/def/definition_base.h"
#include "Object/type/canonical_type_fwd.h"

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
public:
	typedef	channel_type_reference_base	type_reference_type;
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
	make_canonical_fundamental_type_reference(const template_actuals&) const

virtual	MAKE_CANONICAL_CHANNEL_TYPE_REFERENCE_PROTO = 0;

#define	MAKE_CANONICAL_CHANNEL_TYPE_PROTO			\
	canonical_generic_chan_type				\
	make_canonical_type(const template_actuals&) const

virtual	MAKE_CANONICAL_CHANNEL_TYPE_PROTO = 0;

protected:
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class channel_definition_base

//-----------------------------------------------------------------------------
#if 0
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
#endif

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_DEF_CHANNEL_DEFINITION_BASE_H__

