/**
	\file "art_object_instance_enum.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_enum.cc,v 1.13 2005/03/04 07:00:07 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_ENUM_CC__
#define	__ART_OBJECT_INSTANCE_ENUM_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_enum.h"
#include "art_object_inst_ref_data.h"
#include "art_object_member_inst_ref.h"
#include "art_object_expr_const.h"
#include "art_object_connect.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_type_hash.h"
#include "art_object_definition.h"

#include "art_object_classification_details.h"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

#include "art_object_instance_collection.tcc"

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
struct type_dumper<enum_tag> {
	typedef class_traits<enum_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	ostream& os;
	type_dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << "enum " <<
			c.get_base_def()->get_qualified_name() <<
			'^' << c.get_dimensions();
	}
};      // end struct type_dumper<enum_tag>

//-----------------------------------------------------------------------------
template <>
struct collection_parameter_persistence<enum_tag> {
	typedef class_traits<enum_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<enum_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;

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
};      // end struct collection_parameter_persistence

//-----------------------------------------------------------------------------

template <>
struct collection_type_committer<enum_tag> {
	typedef class_traits<enum_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<enum_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	/**
		During unroll phase, this commits the type of the collection.  
		\param t the data integer type reference, containing width, 
			must already be resolved to a const_param_expr_list.  
		\return false on success, true on error.  
		\post the integer width is fixed for the rest of the program.  
	 */
	bad_bool
	operator () (instance_collection_generic_type& c,
		const type_ref_ptr_type& t) const {
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
};

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

#endif	// __ART_OBJECT_INSTANCE_ENUM_CC__

