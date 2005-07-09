/**
	\file "Object/art_object_instance_struct.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_struct.cc,v 1.16.2.6 2005/07/09 23:13:19 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_STRUCT_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_STRUCT_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_struct.h"
#include "Object/art_object_instance_alias_actuals.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/expr/struct_expr.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_definition_data.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_object_nonmeta_value_reference.h"
#include "Object/art_object_classification_details.h"

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"

#include "Object/art_object_instance_collection.tcc"

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_instance_collection,
		"struct_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_scalar, "struct_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_1D, "struct_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_2D, "struct_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_3D, "struct_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_4D, "struct_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_scalar, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_array_1D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_array_2D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_array_3D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_array_4D, STRUCT_INSTANCE_COLLECTION_TYPE_KEY, 4)

}	// end namespace util

namespace ART {
namespace entity {

//=============================================================================
template <>
struct collection_type_manager<datastruct_tag> {
	typedef class_traits<datastruct_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<datastruct_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef class_traits<datastruct_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	struct dumper {
		ostream& os;
		dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const instance_collection_generic_type& c) {
			return os << "struct " <<
				c.get_base_def()->get_qualified_name() <<
				'^' << c.get_dimensions();
		}
	};	// end struct dumper

	static
	void
	collect(persistent_object_manager& m, 
		const instance_collection_generic_type& c) {
		if (c.type_parameter)
			c.type_parameter->collect_transient_info(m);
	}

	static
	void
	write(const persistent_object_manager& m, ostream& o,
		const instance_collection_generic_type& c) {
		m.write_pointer(o, c.type_parameter);
	}

	static
	void
	load(const persistent_object_manager& m, istream& i,
		instance_collection_generic_type& c) {
		m.read_pointer(i, c.type_parameter);
	}

	/**
		TODO: what if type_parameter is not already set
			because it is template-dependent and unresolved?
		Then return the template-dependent type.  
		Consumer is responsible to testing template-dependence. 
	 */
	static
	type_ref_ptr_type
	get_type(const instance_collection_generic_type& e) {
		return e.type_parameter;
	}

	static
	bool
	is_relaxed_type(const instance_collection_generic_type& c) {
		return c.type_parameter->is_relaxed();
	}

	/**
		NOTE: called during connection checking.  
	 */ 
	static
	bool
	must_match_type(const instance_collection_generic_type& l, 
		const instance_collection_generic_type& r) {
		// must be const-resolved!
		return l.type_parameter->must_be_collectibly_type_equivalent(
			*r.type_parameter);
	}


	/**
		During unroll phase, this commits the type of the collection.  
		TODO: handle strict and relaxed template actuals appropriately
		\param t the data integer type reference, containing width, 
			must already be resolved to a const_param_expr_list.  
		\return false on success, true on error.  
		\post the integer width is fixed for the rest of the program.  
	 */
	static
	bad_bool
	commit_type(const instance_collection_generic_type& c,
		const type_ref_ptr_type& t) {
#if 0
		// make sure this is the canonical definition
		//      in case type is typedef!
		// this really should be statically type-checked
		// until we allow templates to include type parameters.  

		// only needs to be "collectibly" type equivalent, 
		// not necessarily "connectible".
		if (c.type_parameter)
			return bad_bool(
				!c.type_parameter->must_be_collectibly_type_equivalent(*t)
			);
		else {
			c.type_parameter = t;
			return bad_bool(false);
		}
#else
		INVARIANT(c.type_parameter);
		return bad_bool(
			!c.type_parameter->must_be_collectibly_type_equivalent(*t)
		);
#endif
	}

	static
	void
	commit_type_first_time(instance_collection_generic_type& c, 
		const type_ref_ptr_type& t) {
		INVARIANT(!c.type_parameter);
		c.type_parameter = t;
	}

};	// end struct collection_type_manager

//=============================================================================
// class struct_instance method definitions

struct_instance::struct_instance() : back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
struct_instance::~struct_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
struct_instance::collect_transient_info(persistent_object_manager& m) const {
	// collect pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
struct_instance::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	// write me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
struct_instance::load_object(const persistent_object_manager& m, 
		istream& i) {
	// write me!
}

//=============================================================================
// explicit template class instantiations

template class instance_alias_info<datastruct_tag>;
template class instance_collection<datastruct_tag>;
template class instance_array<datastruct_tag, 0>;
template class instance_array<datastruct_tag, 1>;
template class instance_array<datastruct_tag, 2>;
template class instance_array<datastruct_tag, 3>;
template class instance_array<datastruct_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_STRUCT_CC__

