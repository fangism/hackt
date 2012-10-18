/**
	\file "Object/inst/footprint_struct_base.cc"
	Explicit template instantiations of footprint_bases
	that manage (sub)structured meta-types.  
	$Id: footprint_struct_base.cc,v 1.5 2010/04/07 00:12:36 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include "Object/def/footprint_struct_base.hh"
#include "Object/def/footprint_base.tcc"
#include "Object/def/user_def_datatype.hh"
#include "Object/def/process_definition.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/datatype_instance_collection.hh"
#include "Object/inst/general_collection_type_manager.hh"
#include "Object/inst/channel_instance_collection.hh"
#include "Object/inst/process_instance.hh"
#include "Object/inst/channel_instance.hh"
#include "Object/inst/struct_instance.hh"
#include "Object/inst/instance_scalar.hh"
#include "Object/inst/instance_array.hh"
#include "Object/inst/instance_collection_pool_bundle.tcc"
#include "Object/devel_switches.hh"
#include "Object/global_channel_entry.hh"

namespace HAC {
namespace entity {

#if 0
template struct instance_collection_pool_bundle<process_tag>;
template struct instance_collection_pool_bundle<channel_tag>;
template struct instance_collection_pool_bundle<datastruct_tag>;
#endif

template class footprint_base<process_tag>;
template class footprint_base<channel_tag>;
#if ENABLE_DATASTRUCTS
template class footprint_base<datastruct_tag>;
#endif

}	// end namespace entity
}	// end namespace HAC

