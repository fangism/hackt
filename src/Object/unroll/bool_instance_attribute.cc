/**
	\file "Object/unroll/bool_instance_attribute.cc"
	The home of instance attribute functions and classes, 
	which are applied at create-time.  
	Because these attributes are applied at create-time, 
	they are back-end independent.  
	$Id: bool_instance_attribute.cc,v 1.2 2008/10/11 22:49:10 fang Exp $
 */

#include "Object/unroll/instance_attribute_registry.h"
#include "Object/lang/bool_attribute_common.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/pint_const.h"
#include "Object/traits/bool_traits.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/connection_policy.h"

namespace HAC {
namespace entity {

INSTANTIATE_INSTANCE_ATTRIBUTE_REGISTRY(bool_tag)

namespace bool_attributes_impl {

#define	DECLARE_BOOL_INSTANCE_ATTRIBUTE_CLASS(class_name) \
	DECLARE_INSTANCE_ATTRIBUTE_CLASS(class_name, \
		entity::bool_attributes::class_name, \
		instance_alias_info<bool_tag>)

#define	DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(class_name, att_name) \
	DEFINE_INSTANCE_ATTRIBUTE_CLASS(class_name, att_name, \
		instance_attribute<bool_tag>::register_attribute_functions)

#define	DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(a, b)		\
	DECLARE_BOOL_INSTANCE_ATTRIBUTE_CLASS(a)			\
	DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(a, b)

//=============================================================================
// bool attributes

DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(IsComb, "iscomb")

/***
@texinfo attrib/bool-iscomb.texi
@defmac iscomb b
Nodes are initially @t{iscomb=false}.
If @var{b} is true, node is marked as being driven combinationally, 
@emph{regardless} of the actual fanin rule of the node.  
This can be used to tell other tools to not expect a staticizer on this node.
If attribute value is unspecified, default value is true.
@end defmac
@end texinfo
***/
void
IsComb::main(visitor_type& a, const values_type& v) {
	pint_value_type b = 1;		// default unspecified value
	if (v.size()) {
		const pint_const& pi(*v[0].is_a<const pint_const>());
		b = pi.static_constant_value();
	}
	if (b) {
		a.set_is_comb();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(AutoKeeper, "autokeeper")

/***
@texinfo attrib/bool-autokeeper.texi
@defmac autokeeper b
Nodes are initially @t{iscomb=true}.
If @var{b} is false, direct other back-end tools to not automatically
staticize this node for simulation or netlist generation purposes.  
If unspecified, default value is true.
@end defmac
@end texinfo
***/
void
AutoKeeper::main(visitor_type& a, const values_type& v) {
	pint_value_type b = 1;		// default unspecified value
	if (v.size()) {
		const pint_const& pi(*v[0].is_a<const pint_const>());
		b = pi.static_constant_value();
	}
	if (!b) {
		a.set_no_autokeeper();
	}
}

//=============================================================================
}	// end namespace bool_attributes_impl
}	// end namespace entity
}	// end namespace HAC

