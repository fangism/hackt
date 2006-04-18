/**
	\file "Object/lang/PRS_attribute_registry.cc"
	This defines the attribute actions for the cflat visitor.  
	$Id: PRS_attribute_registry.cc,v 1.8 2006/04/18 18:42:40 fang Exp $
 */

#include <iostream>
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/cflat_printer.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/expr_dump_context.h"
#include "main/cflat_options.h"
#include "util/qmap.tcc"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.h"
using util::qmap;

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
namespace __attributes__ {

/**
	Macro for declaring attribute classes.  
	Here, the vistor_type is cflat_prs_printer.
	TODO: These classes should have hidden visibility.  
 */
#define	DECLARE_PRS_ATTRIBUTE_CLASS(class_name, att_name)		\
struct class_name {							\
	typedef	cflat_attribute_definition_entry::visitor_type		\
							visitor_type;	\
	typedef	cflat_attribute_definition_entry::values_type		\
							values_type;	\
	typedef	values_type::value_type			value_type;	\
public:									\
	static const char				name[];		\
	static void main(visitor_type&, const values_type&);		\
	static good_bool check_vals(const values_type&);		\
private:								\
	static const size_t				id;		\
};									\
const char class_name::name[] = att_name;				\
const size_t class_name::id = register_cflat_attribute_class<class_name>();

//-----------------------------------------------------------------------------
DECLARE_PRS_ATTRIBUTE_CLASS(After, "after")

/**
	Prints out "after x" before a rule in cflat.  
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

good_bool
After::check_vals(const values_type& v) {
	if (v.size() != 1 || !v[0].is_a<const pint_const>()) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one pint (integer) expression argument." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//-----------------------------------------------------------------------------
DECLARE_PRS_ATTRIBUTE_CLASS(Weak, "weak")

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

good_bool
Weak::check_vals(const values_type& v) {
	if (v.size() != 1 || !v[0].is_a<const pint_const>()) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one pint (integer) expression argument." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//=============================================================================
}	// end namespace __attributes__

#undef	DECLARE_PRS_ATTRIBUTE_CLASS

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

