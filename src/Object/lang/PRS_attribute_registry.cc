/**
	\file "Object/lang/PRS_attribute_registry.cc"
	$Id: PRS_attribute_registry.cc,v 1.4 2006/02/13 02:48:04 fang Exp $
 */

#include <iostream>
#include "Object/lang/PRS_attribute_registry.h"
#include "Object/lang/cflat_printer.h"
#include "Object/expr/const_param_expr_list.h"
#include "Object/expr/pint_const.h"
// #include "Object/expr/pbool_const.h"
#include "Object/expr/expr_dump_context.h"
#include "main/cflat_options.h"
#include "util/qmap.tcc"
#include "util/memory/count_ptr.tcc"

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.h"
using util::qmap;

//=============================================================================
// explicit template instantiations
template class qmap<string, attribute_definition_entry>;
// template attribute_registry_type;

//-----------------------------------------------------------------------------
// global initializers

const attribute_registry_type
attribute_registry;

static
attribute_registry_type&
__attribute_registry(const_cast<attribute_registry_type&>(attribute_registry));

//=============================================================================
// class attribute_definition_entry method definitions

void
attribute_definition_entry::main(cflat_prs_printer& p,
		const values_type& v) const {
	if (_main) {
		(*_main)(p, v);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
attribute_definition_entry::check_values(const values_type& v) const {
	if (_check_values) {
		return (_check_values)(v);
	} else	return good_bool(true);
}

//=============================================================================
/**
	Utility function for registering an attribute class.  
 */
template <class T>
static
size_t
register_attribute_class(void) {
	typedef	attribute_registry_type::iterator		iterator;
	typedef	attribute_registry_type::mapped_type		mapped_type;
	const string k(T::name);
	mapped_type& m = __attribute_registry[k];
	if (m) {
		cerr << "Error: PRS attribute by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = attribute_definition_entry(k, &T::main, &T::check_vals);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n __ATTRIBUTE_UNUSED__ = attribute_registry[k];
	INVARIANT(n);
	return attribute_registry.size();
}

//=============================================================================
/**
	Convenient home namespace for user-defined PRS rule attributes.  
	Each class in this namespace represents an attribute.  
 */
namespace __attributes__ {

/**
	Macro for declaring attribute classes.  
 */
#define	DECLARE_PRS_ATTRIBUTE_CLASS(class_name, att_name)		\
struct class_name {							\
	typedef	attribute_definition_entry::values_type	values_type;	\
	typedef	values_type::value_type			value_type;	\
public:									\
	static const char				name[];		\
	static void main(cflat_prs_printer&, const values_type&);	\
	static good_bool check_vals(const values_type&);		\
private:								\
	static const size_t				id;		\
};									\
const char class_name::name[] = att_name;				\
const size_t class_name::id = register_attribute_class<class_name>();

//-----------------------------------------------------------------------------
DECLARE_PRS_ATTRIBUTE_CLASS(After, "after")

/**
	Prints out "after x" before a rule in cflat.  
 */
void
After::main(cflat_prs_printer& p, const values_type& v) {
if (p.cfopts.primary_tool == cflat_options::TOOL_PRSIM) {
	ostream& o(p.os);
	o << "after ";
	v.at(0).is_a<const pint_const>()->dump(o,
		entity::expr_dump_context::default_value) << '\t';
}
}

good_bool
After::check_vals(const values_type& v) {
	if (v.size() != 1 || !v.at(0).is_a<const pint_const>()) {
		cerr << "The \'" << name << "\' attribute requires exactly "
			"one pint (integer) expression argument." << endl;
		return good_bool(false);
	} else	return good_bool(true);
}

//=============================================================================
}	// end namespace __attributes__

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

