/**
	\file "Object/lang/PRS_attribute_registry.cc"
	This defines the attribute actions for the cflat visitor.  
	$Id: PRS_attribute_registry.cc,v 1.12 2007/10/08 01:21:21 fang Exp $
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/cflat_printer.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/lang/PRS_attribute_common.h"
#include "main/cflat_options.h"
#include "common/TODO.h"
#include "util/qmap.tcc"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.h"

//-----------------------------------------------------------------------------
// global initializers

const cflat_attribute_registry_type
cflat_attribute_registry;

static
cflat_attribute_registry_type&
__cflat_attribute_registry(
	const_cast<cflat_attribute_registry_type&>(cflat_attribute_registry));

//=============================================================================
// class attribute_definition_entry method definitions

//=============================================================================
/**
	Utility function for registering an attribute class.  
 */
template <class T>
static
size_t
register_cflat_attribute_class(void) {
	typedef	cflat_attribute_registry_type::iterator		iterator;
	typedef	cflat_attribute_registry_type::mapped_type	mapped_type;
	const string k(T::name);
	mapped_type& m(__cflat_attribute_registry[k]);
	if (m) {
		cerr << "Error: PRS attribute by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = cflat_attribute_definition_entry(k, &T::main, &T::check_vals);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n
		__ATTRIBUTE_UNUSED_CTOR__((cflat_attribute_registry[k]));
	INVARIANT(n);
	return cflat_attribute_registry.size();
}

//=============================================================================
/**
	Convenient home namespace for user-defined PRS rule attributes.  
	Each class in this namespace represents an attribute.  
 */
namespace cflat_rule_attributes {

/**
	Macro for declaring and defining attribute classes.  
	Here, the vistor_type is cflat_prs_printer.
	TODO: These classes should have hidden visibility.  
	TODO: could also push name[] into the base class, but would we be 
		creating an initialization order dependence?
 */
#define	DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(class_name, att_name) \
	DECLARE_PRS_RULE_ATTRIBUTE_CLASS(class_name, cflat_prs_printer)	\
	DEFINE_PRS_RULE_ATTRIBUTE_CLASS(class_name, att_name,		\
		register_cflat_attribute_class)

//-----------------------------------------------------------------------------
/***
@texinfo prs/attribute-after.texi
@defmac after d
Applies a fixed delay @var{d} to a single rule.
Affects @command{hflat} output and @command{hacprsim} operation.  
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(After, "after")

/**
	Prints out "after x" before a rule in cflat.  
	TODO: allow real-values.
 */
void
After::main(visitor_type& p, const values_type& v) {
if (p.cfopts.primary_tool == cflat_options::TOOL_PRSIM) {
	ostream& o(p.os);
	o << "after ";
	v.at(0).is_a<const pint_const>()->dump(o,
		entity::expr_dump_context::default_value) << '\t';
}
}

//-----------------------------------------------------------------------------
/***
@texinfo prs/attribute-weak.texi
@defmac weak b
If @var{b} is true (1), rule is considered weak, e.g. feedback, 
and may be overpowered by non-weak rules.  
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(Weak, "weak")

/**
	Prints out "weak" before a rule in cflat.  
 */
void
Weak::main(visitor_type& p, const values_type& v) {
if (p.cfopts.primary_tool == cflat_options::TOOL_PRSIM) {
	const pint_const& pi(*v[0].is_a<const pint_const>());
	if (pi.static_constant_value()) {
		ostream& o(p.os);
		o << "weak\t";
	}
}
}

//-----------------------------------------------------------------------------
/***
@texinfo prs/attribute-unstab.texi
@defmac unstab b
If @var{b} is true (1), rule is allowed to be unstable, as an exception.
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(Unstab, "unstab")

/**
      Prints out "unstab" before a rule in cflat.  
 */
void
Unstab::main(visitor_type& p, const values_type& v) {
if (p.cfopts.primary_tool == cflat_options::TOOL_PRSIM) {
      const pint_const& pi(*v[0].is_a<const pint_const>());
      if (pi.static_constant_value()) {
              ostream& o(p.os);
              o << "unstab\t";
      }
}
}

//-----------------------------------------------------------------------------
/***
@texinfo prs/attribute-comb.texi
@defmac comb b
If @var{b} is true (1), use combinational feedback.
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(Comb, "comb")

/**
      Prints out "comb" before a rule in cflat.  
 */
void
Comb::main(visitor_type& p, const values_type& v) {
#if 0
if (p.cfopts.primary_tool == cflat_options::TOOL_PRSIM) {
      const pint_const& pi(*v[0].is_a<const pint_const>());
      if (pi.static_constant_value()) {
              ostream& o(p.os);
              o << "comb\t";
      }
}
#else
	// do nothing yet
#endif
}

//-----------------------------------------------------------------------------
/***
@texinfo prs/attribute-keeper.texi
@defmac keeper b
If @var{b} is true (1), staticize (explicitly).
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(Keeper, "keeper")

/**
      Prints out "comb" before a rule in cflat.  
 */
void
Keeper::main(visitor_type& p, const values_type& v) {
#if 0
if (p.cfopts.primary_tool == cflat_options::TOOL_PRSIM) {
      const pint_const& pi(*v[0].is_a<const pint_const>());
      if (pi.static_constant_value()) {
              ostream& o(p.os);
              o << "keeper\t";
      }
}
#else
	// do nothing yet
#endif
}

//-----------------------------------------------------------------------------
/***
@texinfo prs/attribute-output.texi
@defmac keeper b
If @var{b} is true (1), staticize (explicitly).
Q: should this really be a rule-attribute? better off as node-attribute?
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(Output, "output")

/**
      Prints out "comb" before a rule in cflat.  
 */
void
Output::main(visitor_type& p, const values_type& v) {
#if 0
if (p.cfopts.primary_tool == cflat_options::TOOL_PRSIM) {
      const pint_const& pi(*v[0].is_a<const pint_const>());
      if (pi.static_constant_value()) {
              ostream& o(p.os);
              o << "output\t";
      }
}
#else
	FINISH_ME(Fang);
#endif
}

//-----------------------------------------------------------------------------
/***
@texinfo prs/attribute-loadcap.texi
@defmac loadcap C
Use @var{C} as load capacitance instead of inferring from configuration. 
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(LoadCap, "loadcap")

/**
      Prints out "comb" before a rule in cflat.  
 */
void
LoadCap::main(visitor_type& p, const values_type& v) {
#if 0
if (p.cfopts.primary_tool == cflat_options::TOOL_PRSIM) {
	// use real-value
      const pint_const& pi(*v[0].is_a<const pint_const>());
      if (pi.static_constant_value()) {
              ostream& o(p.os);
              o << "keeper\t";
      }
}
#else
	FINISH_ME(Fang);
#endif
}


#undef	DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS
//=============================================================================
}	// end namespace cflat_rule_attributes

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

