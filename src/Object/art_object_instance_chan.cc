/**
	\file "Object/art_object_instance_chan.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_chan.cc,v 1.14.4.7 2005/07/10 19:37:22 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_CHAN_CC__
#define	__OBJECT_ART_OBJECT_INSTANCE_CHAN_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "Object/art_object_instance_chan.h"
#include "Object/art_object_instance_alias_actuals.h"
#include "Object/art_object_inst_stmt_base.h"
#include "Object/art_object_inst_ref.h"
#include "Object/art_object_member_inst_ref.h"
#include "Object/art_object_definition_base.h"
#include "Object/art_object_type_ref.h"
#include "Object/art_object_connect.h"
#include "Object/art_object_type_hash.h"

#include "Object/art_object_classification_details.h"

// experimental: suppressing automatic template instantiation
#include "Object/art_object_extern_templates.h"

#include "Object/art_object_instance_collection.tcc"
#include "Object/inst/general_collection_type_manager.tcc"

namespace util {

	SPECIALIZE_UTIL_WHAT(ART::entity::channel_instance_collection,
		"channel_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_scalar, "channel_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_1D, "channel_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_2D, "channel_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_3D, "channel_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_4D, "channel_array_4D")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_scalar, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 0)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_array_1D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 1)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_array_2D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 2)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_array_3D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 3)
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_array_4D, CHANNEL_INSTANCE_COLLECTION_TYPE_KEY, 4)
}	// end namespace util

namespace ART {
namespace entity {

//=============================================================================
#if 0
template <>
struct collection_type_manager<channel_tag> {
	typedef class_traits<channel_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<channel_tag>::instance_collection_parameter_type
					instance_collection_parameter_type;
	typedef class_traits<channel_tag>::type_ref_ptr_type
					type_ref_ptr_type;

	struct dumper {
		ostream& os;
		dumper(ostream& o) : os(o) { }

		ostream&
		operator () (const instance_collection_generic_type& c) {
			return os << "channel " <<
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

	static
	type_ref_ptr_type
	get_type(const instance_collection_generic_type& c) {
		return c.type_parameter;
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
#if 0
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

};
#endif

//=============================================================================
// class channel_instance method definitions

channel_instance::channel_instance() : back_ref(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
channel_instance::~channel_instance() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance::collect_transient_info(persistent_object_manager& m) const {
	// collect pointers
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	// write me!
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
channel_instance::load_object(const persistent_object_manager& m, 
		istream& i) {
	// write me!
}

//=============================================================================
// explicit template class instantiations

template class instance_alias_info<channel_tag>;
template class instance_collection<channel_tag>;
template class instance_array<channel_tag, 0>;
template class instance_array<channel_tag, 1>;
template class instance_array<channel_tag, 2>;
template class instance_array<channel_tag, 3>;
template class instance_array<channel_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_CHAN_CC__

