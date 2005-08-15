/**
	\file "Object/type/channel_type_reference.h"
	Type-reference classes of the ART language.  
	TODO: must pool-allocate these, they're created frequently!
	This file originated from "Object/art_object_type_ref.h"
		in a previous life.  
 	$Id: channel_type_reference.h,v 1.2.8.1 2005/08/15 21:12:23 fang Exp $
 */

#ifndef	__OBJECT_TYPE_CHANNEL_TYPE_REFERENCE_H__
#define	__OBJECT_TYPE_CHANNEL_TYPE_REFERENCE_H__

#include "Object/type/channel_type_reference_base.h"
#include "Object/expr/types.h"

namespace ART {
namespace entity {
class builtin_channel_type_reference;
class channel_definition_base;

//=============================================================================
/**
	Reference to a user-defined channel definition.  
	Includes optional template parameters.  
 */
class channel_type_reference : public channel_type_reference_base {
private:
	typedef	channel_type_reference			this_type;
	typedef	channel_type_reference_base		parent_type;
protected:
	typedef	parent_type::template_args_ptr_type	template_args_ptr_type;
	/**
		Can this possibly point to a typedef to a built-in 
		channel type?	(Type escape?)
		TODO: introduce a channel_alias_type_reference
			that resolves into built-in or user-defined.
			After that, then we can upgrade this
			to point to a user_def_chan.  
	 */
	never_ptr<const channel_definition_base>	base_chan_def;
private:
	channel_type_reference();
public:
	explicit
	channel_type_reference(
		const never_ptr<const channel_definition_base> td);

	channel_type_reference(
		const never_ptr<const channel_definition_base> td, 
		const template_actuals& pl);

	~channel_type_reference();

	ostream&
	what(ostream& o) const;

	// override grandparent's
	ostream&
	dump(ostream& o) const;

	never_ptr<const definition_base>
	get_base_def(void) const;

	never_ptr<const channel_definition_base>
	get_base_chan_def(void) const { return base_chan_def; }

	bool
	is_canonical(void) const;

	good_bool
	must_be_valid(void) const;

	count_ptr<const channel_type_reference_base>
	unroll_resolve(const unroll_context&) const;

	never_ptr<const builtin_channel_type_reference>
	resolve_builtin_channel_type(void) const;

private:
	struct canonical_compare_result_type;
public:
	TYPE_EQUIVALENT_PROTOS

	MERGE_RELAXED_ACTUALS_PROTO;

	UNROLL_PORT_INSTANCES_PROTO;

	// helper function
	count_ptr<const this_type>
	make_canonical_channel_type_reference(void) const;

	canonical_type<channel_definition_base>
	make_canonical_type(void) const;

	unroll_context
	make_unroll_context(void) const;
private:
	MAKE_INSTANTIATION_STATEMENT_PRIVATE_PROTO;
			
	MAKE_INSTANCE_COLLECTION_PROTO;

public:
	FRIEND_PERSISTENT_TRAITS
	PERSISTENT_METHODS_DECLARATIONS
};	// end class channel_type_reference

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_TYPE_CHANNEL_TYPE_REFERENCE_H__

