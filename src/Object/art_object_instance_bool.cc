/**
	\file "Object/art_object_instance_bool.cc"
	Method definitions for boolean data type instance classes.
	$Id: art_object_instance_bool.cc,v 1.16.2.1 2005/06/14 05:38:31 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_BOOL_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_BOOL_CC__

#define	DEBUG_LIST_VECTOR_POOL				0
#define	DEBUG_LIST_VECTOR_POOL_USING_STACKTRACE		0
#define	ENABLE_STACKTRACE				0
#define	STACKTRACE_DESTRUCTORS		0 && ENABLE_STACKTRACE
#define	STACKTRACE_PERSISTENTS		0 && ENABLE_STACKTRACE

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_bool.h"
#include "Object/art_object_inst_ref_data.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/art_object_expr_const.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_definition.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_type_hash.h"
#include "Object/art_built_ins.h"
#include "Object/art_object_nonmeta_value_reference.h"
#include "Object/art_object_instance_collection.tcc"

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"

#include "util/memory/list_vector_pool.tcc"

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
	ART::entity::bool_scalar, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_array_1D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_array_2D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_array_3D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::bool_array_4D, DBOOL_INSTANCE_COLLECTION_TYPE_KEY, 4)

namespace memory {
	// can we still lazy destroy with instance aliases?
	// or will it contain pointers to other things later?  (instances)
	LIST_VECTOR_POOL_LAZY_DESTRUCTION(ART::entity::bool_scalar)
}	// end namespace memory
}	// end namespace util

//=============================================================================
namespace ART {
namespace entity {

//=============================================================================
// struct type_dumper specialization

template <>
struct collection_type_manager<bool_tag> {
	typedef	class_traits<bool_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<bool_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef class_traits<bool_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	struct dumper {
		ostream& os;
		dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const instance_collection_generic_type& c) {
			return os << "bool^" << c.get_dimensions();
		}
	};	// end struct dumper

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

	static
	type_ref_ptr_type
	get_type(const instance_collection_generic_type& i) {
		// just return built-in type
		return bool_type_ptr;
	}

	// return true on error, false on success
	static
	bad_bool
	commit_type(instance_collection_generic_type& c, 
		const type_ref_ptr_type& t) {
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

DEFAULT_STATIC_TRACE_END

#endif	// __OBJECT_ART_OBJECT_INSTANCE_BOOL_CC__

