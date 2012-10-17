/**
	\file "Object/lang/PRS_literal_attribute_registry.cc"
	This defines the literal attribute actions for the cflat visitor.  
	$Id: PRS_literal_attribute_registry.cc,v 1.3 2009/10/05 23:09:26 fang Exp $
 */

#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <map>
#include "Object/lang/PRS_literal_attribute_registry.hh"
#include "Object/lang/cflat_printer.hh"
#include "Object/expr/const_param_expr_list.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/string_expr.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/lang/PRS_literal_attribute_common.hh"
#include "main/cflat_options.hh"
#include "common/TODO.hh"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.hh"

//-----------------------------------------------------------------------------
// global initializers

/**
	Locally modifiable to this unit only.
 */
static
cflat_literal_attribute_registry_type
__cflat_literal_attribute_registry;

/**
	Public immutable reference
 */
const cflat_literal_attribute_registry_type&
cflat_literal_attribute_registry(__cflat_literal_attribute_registry);

//=============================================================================
// class attribute_definition_entry method definitions

//=============================================================================
/**
	Utility function for registering an attribute class.  
 */
template <class T>
static
size_t
register_cflat_literal_attribute_class(void) {
//	typedef	cflat_literal_attribute_registry_type::iterator		iterator;
	typedef	cflat_literal_attribute_registry_type::mapped_type	mapped_type;
	const string k(T::name);
	mapped_type& m(__cflat_literal_attribute_registry[k]);
	if (m) {
		cerr << "Error: PRS attribute by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = cflat_literal_attribute_definition_entry(k, &T::main, &T::check_vals);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n
		__ATTRIBUTE_UNUSED_CTOR__((cflat_literal_attribute_registry.find(k)->second));
	INVARIANT(n);
	return cflat_literal_attribute_registry.size();
}

//=============================================================================
/**
	Convenient home namespace for user-defined PRS rule attributes.  
	Each class in this namespace represents an attribute.  
 */
namespace cflat_literal_attributes {

/**
	Macro for declaring and defining attribute classes.  
	Here, the vistor_type is cflat_prs_printer.
	TODO: These classes should have hidden visibility.  
	TODO: could also push name[] into the base class, but would we be 
		creating an initialization order dependence?
 */
#define	DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(class_name, att_name) \
	DECLARE_PRS_LITERAL_ATTRIBUTE_CLASS(class_name, cflat_prs_printer) \
	DEFINE_PRS_LITERAL_ATTRIBUTE_CLASS(class_name, att_name,	\
		register_cflat_literal_attribute_class)

//-----------------------------------------------------------------------------
/***
@texinfo prs/literal-label.texi
@defmac label (string)
Labels the transistor represented by a PRS literal with a user-supplied name.
This is mostly useful during netlist generation.  
A shorthand notation for labels is just to pass the "@var{string}" value
without writing @samp{label="string"}.  
@example
prs @{
  x & y<;"this_one"> & z<;label="this_works_too"> -> _o-
@}
@end example
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(Label, "label")
void
Label::main(visitor_type& p, const values_type& v) {
if (p.cfopts.primary_tool == cflat_options::TOOL_LVS) {
	ostream& o(p.os);
	o << ";label=";
	v.front()->dump(o, entity::expr_dump_context::default_value);
}
}

//-----------------------------------------------------------------------------
/***
@texinfo prs/literal-vt.texi
@defmac lvt
Specifies that a transistor has low-Vt type, mainly for netlist 
generation and LVS checking.
@end defmac

@defmac svt
Specifies that a transistor has standard-Vt type, mainly for netlist 
generation and LVS checking.  This is also the default type, when
Vt is left unspecified.
@end defmac

@defmac hvt
Specifies that a transistor has high-Vt type, mainly for netlist 
generation and LVS checking.
@end defmac
@end texinfo
***/
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(LVT, "lvt")
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(SVT, "svt")
DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS(HVT, "hvt")

/**
	Currently, does nothing, ignores value.
 */
void
LVT::main(visitor_type& p, const values_type& v) {
if (p.cfopts.primary_tool == cflat_options::TOOL_LVS) {
	ostream& o(p.os);
	o << ";lvt";
}
}

void
SVT::main(visitor_type& p, const values_type& v) {
if (p.cfopts.primary_tool == cflat_options::TOOL_LVS) {
	ostream& o(p.os);
	o << ";svt";
}
}

void
HVT::main(visitor_type& p, const values_type& v) {
if (p.cfopts.primary_tool == cflat_options::TOOL_LVS) {
	ostream& o(p.os);
	o << ";hvt";
}
}

//-----------------------------------------------------------------------------

#undef	DECLARE_AND_DEFINE_CFLAT_PRS_ATTRIBUTE_CLASS
//=============================================================================
}	// end namespace cflat_literal_attributes

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

