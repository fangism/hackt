/**
	\file "art_object_instance_chan.cc"
	Method definitions for integer data type instance classes.
	Hint: copied from the bool counterpart, and text substituted.  
	$Id: art_object_instance_chan.cc,v 1.8.2.6 2005/02/27 07:33:30 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_CHAN_CC__
#define	__ART_OBJECT_INSTANCE_CHAN_CC__

#include <exception>
#include <iostream>
#include <algorithm>

#include "art_object_instance_chan.h"
#include "art_object_inst_stmt_base.h"
#include "art_object_inst_ref.h"
#include "art_object_member_inst_ref.h"
#include "art_object_type_ref.h"
#include "art_object_connect.h"
#include "art_object_expr_const.h"
#include "art_object_type_hash.h"

#include "art_object_classification_details.h"

// experimental: suppressing automatic template instantiation
#include "art_object_extern_templates.h"

#include "art_object_instance_collection.tcc"

namespace util {
SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	ART::entity::channel_instance_collection, 
		CHANNEL_INSTANCE_COLLECTION_TYPE_KEY)

	SPECIALIZE_UTIL_WHAT(ART::entity::channel_instance_collection,
		"channel_instance_collection")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_scalar, "channel_scalar")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_1D, "channel_array_1D")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_2D, "channel_array_2D")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_3D, "channel_array_3D")
	SPECIALIZE_UTIL_WHAT(ART::entity::channel_array_4D, "channel_array_4D")
}	// end namespace util

namespace ART {
namespace entity {

//=============================================================================
template <>
struct type_dumper<channel_tag> {
	typedef class_traits<channel_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	ostream& os;
	type_dumper(ostream& o) : os(o) { }

	ostream&
	operator () (const instance_collection_generic_type& c) {
		return os << "channel " <<
			c.get_base_def()->get_qualified_name() <<
			'^' << c.get_dimensions();
	}
};      // end struct type_dumper<channel_tag>

//-----------------------------------------------------------------------------
template <>
struct collection_parameter_persistence<channel_tag> {
	typedef class_traits<channel_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<channel_tag>::instance_collection_parameter_type
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
struct collection_type_committer<channel_tag> {
	typedef class_traits<channel_tag>::instance_collection_generic_type
					instance_collection_generic_type;
	typedef class_traits<channel_tag>::type_ref_ptr_type
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

template class instance_collection<channel_tag>;
template class instance_array<channel_tag, 0>;
template class instance_array<channel_tag, 1>;
template class instance_array<channel_tag, 2>;
template class instance_array<channel_tag, 3>;
template class instance_array<channel_tag, 4>;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __ART_OBJECT_INSTANCE_CHAN_CC__

