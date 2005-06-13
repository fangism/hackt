/**
	\file "Object/art_object_instance_proc.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_proc.cc,v 1.13.10.1 2005/06/13 17:52:12 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_PROC_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_PROC_CC__

#define	ENABLE_STACKTRACE		0

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_proc.h"
#include "Object/art_object_inst_stmt_base.h"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_expr_const.h"
#include "Object/art_object_type_hash.h"

#include "Object/art_object_classification_details.h"

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"

#include "Object/art_object_instance_collection.tcc"


namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::process_instance_collection,
		"process_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_scalar, "process_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_1D, "process_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_2D, "process_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_3D, "process_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::process_array_4D, "process_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_instance, UNIQUE_PROCESS_INSTANCE_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_scalar, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_array_1D, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_array_2D, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_array_3D, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::process_array_4D, PROCESS_INSTANCE_COLLECTION_TYPE_KEY, 4)

}	// end namespace util

namespace ART {
namespace entity {

//=============================================================================
template <>
struct collection_type_manager<process_tag> {
	typedef class_traits<process_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<process_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef class_traits<process_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	struct dumper {
		ostream& os;
		dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const instance_collection_generic_type& c) {
			os << "process ";
			const instance_collection_parameter_type&
				tr = c.get_type_parameter();
			INVARIANT(tr);
			tr->dump(os) << '^' << c.get_dimensions();
			return os;
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

	static
	type_ref_ptr_type
	get_type(const instance_collection_generic_type& c) {
		return c.type_parameter;
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
		//      in case type is typedef!
		// this really should be statically type-checked
		// until we allow templates to include type parameters.  

		// only needs to be "collectibly" type equivalent, 
		// not necessarily "connectible".
		if (c.type_parameter)
			return bad_bool(
				!c.type_parameter->must_be_type_equivalent(*t));
		else {
			c.type_parameter = t;
			return bad_bool(false);
		}
	}
};	// end struct collection_type_manager

//=============================================================================
// class proc_instance method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance::process_instance() : back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
process_instance::~process_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
process_instance::what(ostream& o) const {
	return o << "process_instance";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	// walk vector of pointers...
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
process_instance::write_object(const persistent_object_manager& m, 
		ostream& f) const {
#if 0
	write_value(f, state);
	// write pointer sequence...
#endif
}

void
process_instance::load_object(const persistent_object_manager& m, istream& f) {
#if 0
	read_value(f, state);
	// read pointer sequence...
#endif
}

//=============================================================================
// explicit template clas instantiations

template class instance_collection<process_tag>;
template class instance_array<process_tag, 0>;
template class instance_array<process_tag, 1>;
template class instance_array<process_tag, 2>;
template class instance_array<process_tag, 3>;
template class instance_array<process_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#undef	ENABLE_STACKTRACE

#endif	// __OBJECT_ART_OBJECT_INSTANCE_PROC_CC__

