/**
	\file "art_object_instance_bool.cc"
	Method definitions for boolean data type instance classes.
	$Id: art_object_instance_bool.cc,v 1.10.2.1 2005/02/28 20:36:03 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_BOOL_CC__
#define	__ART_OBJECT_INSTANCE_BOOL_CC__

#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_bool.h"
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

#include "memory/list_vector_pool.tcc"


STATIC_TRACE_BEGIN("instance-bool")

//=============================================================================
// module-local specializations

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_instance_collection,
		"bool_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_scalar, "bool_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_1D, "bool_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_2D, "bool_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_3D, "bool_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::bool_array_4D, "bool_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_instance_collection, 
		DBOOL_INSTANCE_COLLECTION_TYPE_KEY)

namespace memory {
	// can we still lazy destroy with instance aliases?
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::bool_scalar)
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// struct type_dumper specialization

template <>
struct type_dumper<bool_tag> {
	typedef	class_traits<bool_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	ostream& os;
	type_dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << "bool^" << c.get_dimensions();
	}
};	// end struct type_dumper<bool_tag>

//-----------------------------------------------------------------------------
template <>
struct collection_parameter_persistence<bool_tag> {
	typedef	class_traits<bool_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<bool_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;

	static
	void
	collect(persistent_object_manager&,
		const instance_collection_generic_type&) {
		// do nothing! bool has no parameters!
	}

	static
	void
	write(const persistent_object_manager&, ostream&,
		const instance_collection_generic_type&) {
		// do nothing! bool has no parameters!
	}

	static
	void
	load(const persistent_object_manager&, istream&,
		instance_collection_generic_type&) {
		// do nothing! bool has no parameters!
	}
};	// end struct collection_parameter_persistence
//-----------------------------------------------------------------------------

template <>
struct collection_type_committer<bool_tag> {
	typedef class_traits<bool_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<bool_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	// return true on error, false on success
	bad_bool
	operator () (instance_collection_generic_type& c, 
		const type_ref_ptr_type& t) const {
		// INVARIANT(!is_partially_unrolled());
		INVARIANT(t->get_base_def() == &bool_def);
		// shouldn't have any parameters, NULL or empty list
		return bad_bool(false);
	}
};	// end struct collection_type_committer

//=============================================================================
// class bool_instance method definitions

bool_instance::bool_instance() : persistent(), back_ref() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
bool_instance::~bool_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance::collect_transient_info(persistent_object_manager& m) const {
	// register me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	// write me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
bool_instance::load_object(const persistent_object_manager& m, 
		istream& i) {
	// load me!
}

//=============================================================================
// typedef bool_instance_alias_base function definitions

ostream&
operator << (ostream& o, const bool_instance_alias_base& b) {
	return o << "bool-alias @ " << &b;
}

//=============================================================================
// explicit instantiations

template class instance_collection<bool_tag>;
template class instance_array<bool_tag, 0>;
template class instance_array<bool_tag, 1>;
template class instance_array<bool_tag, 2>;
template class instance_array<bool_tag, 3>;
template class instance_array<bool_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

STATIC_TRACE_END("instance-bool")

#endif	// __ART_OBJECT_INSTANCE_BOOL_CC__

