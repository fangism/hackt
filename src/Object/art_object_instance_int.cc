/**
	\file "art_object_instance_int.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_int.cc,v 1.12.2.5.2.6 2005/02/27 01:09:31 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_INT_CC__
#define	__ART_OBJECT_INSTANCE_INT_CC__

#define	ENABLE_STACKTRACE		0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_int.h"
#include "art_object_inst_ref_data.h"
#include "art_object_member_inst_ref.h"
#include "art_object_expr_const.h"
#include "art_object_connect.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_type_hash.h"
#include "art_built_ins.h"

#include "art_object_instance_collection.tcc"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"


//=============================================================================
STATIC_TRACE_BEGIN("instance-int")

//=============================================================================
// module-local specializations

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::int_instance_collection, 
		DINT_INSTANCE_COLLECTION_TYPE_KEY)
	SPECIALIZE_UTIL_WHAT(ART::entity::int_instance_collection,
		"int_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_scalar, "int_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_1D, "int_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_2D, "int_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_3D, "int_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::int_array_4D, "int_array_4D")
}	// end namespace util


namespace ART {
namespace entity {

//=============================================================================
// functor specializations
template <>
struct type_dumper<int_tag> {
	typedef class_traits<int_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	ostream& os;
	type_dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << "int<" << c.get_type_parameter() <<
			">^" << c.get_dimensions();
	}
};      // end struct type_dumper<int_tag>

//-----------------------------------------------------------------------------
template <>
struct collection_parameter_persistence<int_tag> {
	typedef class_traits<int_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<int_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;

	static
	void
	collect(persistent_object_manager& m, 
		const instance_collection_generic_type& c) {
		// c.type_parameter contains no pointers
	}

	static
	void
	write(const persistent_object_manager&, ostream& o,
		const instance_collection_generic_type& c) {
		// parameter is just an int
		write_value(o, c.type_parameter);
	}

	static
	void
	load(const persistent_object_manager&, istream& i,
		instance_collection_generic_type& c) {
		// parameter is just an int
		read_value(i, c.type_parameter);
	}

};      // end struct collection_parameter_persistence

//-----------------------------------------------------------------------------

template <>
struct collection_type_committer<int_tag> {
	typedef class_traits<int_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<int_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	/**
		During unroll phase, this commits the type of the collection.  
		\param t the data integer type reference, containing width, 
			must already be resolved to a const_param_expr_list.  
		\return false on success, true on error.  
		\post the integer width is fixed for the rest of the program.  
	 */
	bool
	operator () (instance_collection_generic_type& c,
		const type_ref_ptr_type& t) const {
		INVARIANT(t->get_base_def() == &int_def);
		const never_ptr<const param_expr_list>
			params(t->get_template_params());
		NEVER_NULL(params);
		// extract first and only parameter, the integer width
		const never_ptr<const const_param_expr_list>
			cparams(params.is_a<const const_param_expr_list>());
		NEVER_NULL(cparams);
		INVARIANT(cparams->size() == 1); 
		const count_ptr<const const_param>&
			param1(cparams->front());
		NEVER_NULL(param1);
		const count_ptr<const pint_const>
			pwidth(param1.is_a<const pint_const>());
		NEVER_NULL(pwidth);
		const pint_value_type new_width = pwidth->static_constant_int();
		INVARIANT(new_width);
		if (c.is_partially_unrolled()) {
			INVARIANT(c.type_parameter);
			return (new_width != c.type_parameter);
		} else { 
			c.type_parameter = new_width;
			return false;
		}
	}
};      // end struct collection_type_committer

//=============================================================================
// typedef int_instance_alias_base function definitions

ostream&
operator << (ostream& o, const int_instance_alias_base& i) {
	return o << "int-alias @ " << &i;
}

//=============================================================================
// explicit template instantiations

template class instance_collection<int_tag>;
template class instance_array<int_tag, 0>;
template class instance_array<int_tag, 1>;
template class instance_array<int_tag, 2>;
template class instance_array<int_tag, 3>;
template class instance_array<int_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

STATIC_TRACE_END("instance-int")

#endif	// __ART_OBJECT_INSTANCE_INT_CC__

