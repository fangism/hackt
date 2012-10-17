/**
	\file "Object/unroll/bool_instance_attribute.cc"
	The home of instance attribute functions and classes, 
	which are applied at create-time.  
	Because these attributes are applied at create-time, 
	they are back-end independent.  
	$Id: bool_instance_attribute.cc,v 1.6 2010/08/24 22:52:05 fang Exp $
 */

#include "Object/unroll/instance_attribute_registry.hh"
#include "Object/lang/bool_attribute_common.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/expr/pint_const.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/connection_policy.hh"

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

/***
@texinfo attrib/bool-ignore_interfere.texi
@defmac ignore_interfere b
Diagnostic attribute.
If @var{b} is true, then suppress diagnostics about interference 
(opposing on-pulls) on this node.
In simulation, the behavior should remain, just silence warnings.  
When @var{b} is omitted, it is assumed to be true.
@end defmac
@end texinfo

@texinfo attrib/bool-ignore_weak_interfere.texi
@defmac ignore_weak_interfere b
Diagnostic attribute.
If @var{b} is true, then suppress diagnostics about weak-interference 
(on-pull vs. X-pull or X-pull vs. X-pull) on this node.
In simulation, the behavior should remain, just silence warnings.  
When @var{b} is omitted, it is assumed to be true.
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(AllowInterference, 
	"ignore_interference")
DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(AllowWeakInterference,
	"ignore_weak_interference")

void
AllowInterference::main(visitor_type& a, const values_type& v) {
	pint_value_type b = 1;		// default unspecified value
	if (v.size()) {
		const pint_const& pi(*v[0].is_a<const pint_const>());
		b = pi.static_constant_value();
	}
	if (b) {
		a.set_may_interfere();
	}
}

void
AllowWeakInterference::main(visitor_type& a, const values_type& v) {
	pint_value_type b = 1;		// default unspecified value
	if (v.size()) {
		const pint_const& pi(*v[0].is_a<const pint_const>());
		b = pi.static_constant_value();
	}
	if (b) {
		a.set_may_weak_interfere();
	}
}


//=============================================================================

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
// attributes added by request

DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(IsRVC1, "isrvc1")
DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(IsRVC2, "isrvc2")
DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(IsRVC3, "isrvc3")

/***
@texinfo attrib/bool-isrvc.texi
@defmac isrvc1 b
@defmacx isrvc2 b
@defmacx isrvc3 b
Nodes are initially @t{isrvc1=false}, @t{isrvc2=false}, @t{isrvc3=false}.
If @var{b} is true, label this node in a way meaningful for 
redundant keeper circuits.  
If unspecified, argument is implicitly true.  
Exclusivity between these attributes is not yet checked.
@end defmac
@end texinfo
***/
#define	DEFINE_ISRVC_ATTRIBUTE(class_name, setter)			\
void									\
class_name::main(visitor_type& a, const values_type& v) {		\
	pint_value_type b = 1;						\
	if (v.size()) {							\
		const pint_const& pi(*v[0].is_a<const pint_const>());	\
		b = pi.static_constant_value();				\
	}								\
	if (b) {							\
		a.setter();						\
	}								\
}

DEFINE_ISRVC_ATTRIBUTE(IsRVC1, set_is_rvc1)
DEFINE_ISRVC_ATTRIBUTE(IsRVC2, set_is_rvc2)
DEFINE_ISRVC_ATTRIBUTE(IsRVC3, set_is_rvc3)

#undef	DEFINE_ISRVC_ATTRIBUTE

//=============================================================================
/***
@texinfo attrib/bool-supply.texi
@defmac supply t
Declares that the said node is a supply node.
Supply nodes are treated specially in different back-end tools.  
@var{t}=0 indicates that supply is GND-like (low).
@var{t}=1 indicates that supply is Vdd-like (high).
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(Supply, "supply")
void
Supply::main(visitor_type& a, const values_type& v) {
	INVARIANT(v.size());
	const pint_const& pi(*v[0].is_a<const pint_const>());
	const pint_value_type b = pi.static_constant_value();
	a.set_supply(b);
}

//=============================================================================
/***
@texinfo attrib/bool-reset.texi
@defmac reset init
Declares that the said node is a reset node with initial value @var{init}.
Reset nodes are treated specially in different back-end tools.  
@var{init}=0 indicates that supply is active-low.
@var{init}=1 indicates that supply is active-high.
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_BOOL_INSTANCE_ATTRIBUTE_CLASS(Reset, "reset")
void
Reset::main(visitor_type& a, const values_type& v) {
	INVARIANT(v.size());
	const pint_const& pi(*v[0].is_a<const pint_const>());
	const pint_value_type b = pi.static_constant_value();
	a.set_reset(b);
}

//=============================================================================
}	// end namespace bool_attributes_impl
}	// end namespace entity
}	// end namespace HAC

