/**
	\file "Object/unroll/instance_attribute_registry.h"
	The home of instance attribute registries, a tcc file.
	By putting these definitions separate from "instance_attribute.tcc"
	we can control where these static map members are instantiated.  
	$Id: instance_attribute_registry.h,v 1.1 2008/10/07 03:22:32 fang Exp $
 */

#include <iostream>
#include "Object/unroll/instance_attribute.h"
// #include "Object/expr/const_param_expr_list.h"
// #include "Object/inst/instance_alias_info.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"

/**
	Macro to instantiate attribute registry map.  
 */
#define	INSTANTIATE_INSTANCE_ATTRIBUTE_REGISTRY(Tag)			\
template instance_attribute<Tag>::attribute_registry_type		\
instance_attribute<Tag>::attribute_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
typename INSTANCE_ATTRIBUTE_CLASS::attribute_registry_type
INSTANCE_ATTRIBUTE_CLASS::attribute_registry;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INSTANCE_ATTRIBUTE_TEMPLATE_SIGNATURE
template <class T>
size_t
INSTANCE_ATTRIBUTE_CLASS::register_attribute_functions(void) {
//	typedef	typename attribute_registry_type::iterator	iterator;
	typedef	typename attribute_registry_type::mapped_type	mapped_type;
	const string k(T::name);
	mapped_type& m(attribute_registry[k]);
	if (m) {
		cerr << "Error: " << traits_type::tag_name <<
			" instance attribute by the name \'" << k <<
			"\' has already been registered!" << endl;
		THROW_EXIT;
	}
	m = attribute_funcs_type(k, &T::main, &T::check_vals);
	// oddly, this is needed to force instantiation of the [] const operator
	const mapped_type& n
		__ATTRIBUTE_UNUSED_CTOR__((attribute_registry[k]));
	INVARIANT(n);
	return attribute_registry.size();
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

