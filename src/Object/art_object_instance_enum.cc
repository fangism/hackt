/**
	\file "Object/art_object_instance_enum.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_enum.cc,v 1.14.10.2 2005/06/13 17:52:11 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_ENUM_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_ENUM_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_enum.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/art_object_expr_const.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_definition.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_object_definition.h"
#include "Object/art_object_nonmeta_value_reference.h"
#include "Object/art_object_classification_details.h"

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"

#include "Object/art_object_instance_collection.tcc"

namespace util {

	SPECIALIZE_UTIL_WHAT(ART::entity::enum_instance_collection,
		"enum_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_scalar, "enum_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_array_1D, "enum_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_array_2D, "enum_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_array_3D, "enum_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::enum_array_4D, "enum_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_scalar, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_array_1D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_array_2D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_array_3D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::enum_array_4D, ENUM_INSTANCE_COLLECTION_TYPE_KEY, 4)
}	// end namespace util

namespace ART {
namespace entity {

//=============================================================================
template <>
struct collection_type_manager<enum_tag> {
	typedef class_traits<enum_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<enum_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef class_traits<enum_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	struct dumper {
		ostream& os;
		dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const instance_collection_generic_type& c) {
			return os << "enum " <<
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
		return type_ref_ptr_type(new data_type_reference(
			// want get_base_def_subtype!!!
			e.get_base_def()
			.is_a<const datatype_definition_base>()));
	}

	/**
		During unroll phase, this commits the type of the collection.  
		\param t the data integer type reference, containing width, 
			must already be resolved to a const_param_expr_list.  
		\return false on success, true on error.  
		\post the integer width is fixed for the rest of the program.  
	 */
	static
	bad_bool
	commit_type(instance_collection_generic_type& c,
		const type_ref_ptr_type& t) {
		// make sure this is the canonical definition
		//	in case type is typedef!
		// this really should be statically type-checked
		// until we allow templates to include type parameters.  
		if (c.type_parameter)
			INVARIANT(t->get_base_def() == c.type_parameter);
		else
			c.type_parameter = t->get_base_def()
				.is_a<const enum_datatype_def>();
		return bad_bool(false);
	}
};	// end struct collection_type_manager

//=============================================================================
// class enum_instance method definitions

enum_instance::enum_instance() : back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
enum_instance::~enum_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance::collect_transient_info(persistent_object_manager& m) const {
	// collect pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	// write me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
enum_instance::load_object(const persistent_object_manager& m, 
		istream& i) {
	// write me!
}

//=============================================================================
// explicit class instantiations

template class instance_collection<enum_tag>;
template class instance_array<enum_tag, 0>;
template class instance_array<enum_tag, 1>;
template class instance_array<enum_tag, 2>;
template class instance_array<enum_tag, 3>;
template class instance_array<enum_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_ENUM_CC__

