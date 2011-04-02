/**
	\file "Object/ref/instance_reference_datatype.cc"
	Method definitions for datatype instance reference classes.
	This file was reincarnated from "Object/art_object_inst_ref_data.cc".
	$Id: instance_reference_node.cc,v 1.3 2011/04/02 01:46:07 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include "Object/ref/simple_meta_dummy_reference.tcc"
#include "Object/inst/node_instance_collection.h"
#include "Object/expr/const_range.h"
#include "Object/devel_switches.h"
#include "Object/persistent_type_hash.h"
#include "util/persistent_object_manager.tcc"
#include "Object/traits/node_traits.h"
#include "Object/inst/null_collection_type_manager.h"	// need?

namespace util {

SPECIALIZE_UTIL_WHAT(
	HAC::entity::simple_node_meta_instance_reference, "node-inst-ref")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::simple_node_meta_instance_reference, 
		SIMPLE_NODE_META_INSTANCE_REFERENCE_TYPE_KEY, 0)

namespace memory {
// explicit template instantiations
using HAC::entity::simple_node_meta_instance_reference;
template class count_ptr<const simple_node_meta_instance_reference>;
}	// end namespace memory
}	// end namespace util

namespace HAC {
namespace entity {
//=============================================================================
// explicit template instantiations

// template class meta_instance_reference<node_tag>;
template class simple_meta_dummy_reference<node_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

