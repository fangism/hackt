/**
	\file "art_object_instance_struct.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_struct.cc,v 1.10 2005/02/27 22:54:17 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_STRUCT_CC__
#define	__ART_OBJECT_INSTANCE_STRUCT_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_struct.h"
#include "art_object_inst_ref_data.h"
#include "art_object_member_inst_ref.h"
#include "art_object_expr_const.h"
#include "art_object_connect.h"
#include "art_object_definition.h"
#include "art_object_type_ref.h"
#include "art_object_type_hash.h"

#include "art_object_classification_details.h"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

#include "art_object_instance_collection.tcc"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::struct_instance_collection, 
		STRUCT_INSTANCE_COLLECTION_TYPE_KEY)

	SPECIALIZE_UTIL_WHAT(ART::entity::struct_instance_collection,
		"struct_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_scalar, "struct_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_1D, "struct_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_2D, "struct_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_3D, "struct_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::struct_array_4D, "struct_array_4D")
}	// end namespace util

namespace ART {
namespace entity {

//=============================================================================
template <>
struct type_dumper<datastruct_tag> {
	typedef class_traits<datastruct_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	ostream& os;
	type_dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << "struct " <<
			c.get_base_def()->get_qualified_name() <<
			'^' << c.get_dimensions();
	}
};      // end struct type_dumper<datastruct_tag>

//-----------------------------------------------------------------------------
template <>
struct collection_parameter_persistence<datastruct_tag> {
	typedef class_traits<datastruct_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<datastruct_tag>::instance_collection_parameter_type
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
struct collection_type_committer<datastruct_tag> {
	typedef class_traits<datastruct_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<datastruct_tag>::type_ref_ptr_type
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
		// make sure this is the canonical definition
		//      in case type is typedef!
		// this really should be statically type-checked
		// until we allow templates to include type parameters.  

		// only needs to be "collectibly" type equivalent, 
		// not necessarily "connectible".
		if (c.type_parameter)
			return (!c.type_parameter->must_be_equivalent(*t));
		else
			c.type_parameter = t;
		return false;
	}
};

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

template class instance_collection<datastruct_tag>;
template class instance_array<datastruct_tag, 0>;
template class instance_array<datastruct_tag, 1>;
template class instance_array<datastruct_tag, 2>;
template class instance_array<datastruct_tag, 3>;
template class instance_array<datastruct_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_STRUCT_CC__

