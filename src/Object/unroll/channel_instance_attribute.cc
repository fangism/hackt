/**
	\file "Object/unroll/channel_instance_attribute.cc"
	The home of instance attribute functions and classes, 
	which are applied at create-time.  
	Because these attributes are applied at create-time, 
	they are back-end independent.  
	$Id: channel_instance_attribute.cc,v 1.1 2008/10/07 03:22:29 fang Exp $
 */

#include "Object/unroll/instance_attribute_registry.h"
// #include "Object/lang/channel_attribute_common.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/pint_const.h"
#include "Object/traits/chan_traits.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/connection_policy.h"

namespace HAC {
namespace entity {

INSTANTIATE_INSTANCE_ATTRIBUTE_REGISTRY(channel_tag)

namespace channel_attributes_impl {

#define	DECLARE_CHANNEL_INSTANCE_ATTRIBUTE_CLASS(class_name) \
	DECLARE_INSTANCE_ATTRIBUTE_CLASS(class_name, \
		entity::channel_attributes::class_name, \
		instance_alias_info<channel_tag>)

#define	DEFINE_CHANNEL_INSTANCE_ATTRIBUTE_CLASS(class_name, att_name) \
	DEFINE_INSTANCE_ATTRIBUTE_CLASS(class_name, att_name, \
		instance_attribute<channel_tag>::register_attribute_functions)

#define	DECLARE_AND_DEFINE_CHANNEL_INSTANCE_ATTRIBUTE_CLASS(a, b)	\
	DECLARE_CHANNEL_INSTANCE_ATTRIBUTE_CLASS(a)			\
	DEFINE_CHANNEL_INSTANCE_ATTRIBUTE_CLASS(a, b)

//=============================================================================
// channel attributes


//=============================================================================
}	// end namespace channel_attributes_impl
}	// end namespace entity
}	// end namespace HAC

