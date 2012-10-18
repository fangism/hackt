/**
	\file "Object/unroll/enum_instance_attribute.cc"
	The home of instance attribute functions and classes, 
	which are applied at create-time.  
	Because these attributes are applied at create-time, 
	they are back-end independent.  
	$Id: enum_instance_attribute.cc,v 1.1 2008/10/07 03:22:30 fang Exp $
 */

#include "Object/unroll/instance_attribute_registry.hh"
// #include "Object/lang/enum_attribute_common.hh"
// #include "Object/expr/const_param_expr_list.hh"
// #include "Object/expr/pint_const.hh"
#include "Object/traits/enum_traits.hh"
// #include "Object/inst/instance_alias_info.hh"
// #include "Object/inst/alias_empty.hh"
// #include "Object/inst/connection_policy.hh"

namespace HAC {
namespace entity {

INSTANTIATE_INSTANCE_ATTRIBUTE_REGISTRY(enum_tag)

namespace enum_attributes_impl {

#define	DECLARE_ENUM_INSTANCE_ATTRIBUTE_CLASS(class_name) \
	DECLARE_INSTANCE_ATTRIBUTE_CLASS(class_name, \
		entity::enum_attributes::class_name, \
		instance_alias_info<enum_tag>)

#define	DEFINE_ENUM_INSTANCE_ATTRIBUTE_CLASS(class_name, att_name) \
	DEFINE_INSTANCE_ATTRIBUTE_CLASS(class_name, att_name, \
		instance_attribute<enum_tag>::register_attribute_functions)

#define	DECLARE_AND_DEFINE_ENUM_INSTANCE_ATTRIBUTE_CLASS(a, b)		\
	DECLARE_ENUM_INSTANCE_ATTRIBUTE_CLASS(a)			\
	DEFINE_ENUM_INSTANCE_ATTRIBUTE_CLASS(a, b)

//=============================================================================
// enum attributes


//=============================================================================
}	// end namespace enum_attributes_impl
}	// end namespace entity
}	// end namespace HAC

