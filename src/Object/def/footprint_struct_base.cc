/**
	\file "Object/inst/footprint_struct_base.cc"
	Explicit template instantiations of footprint_bases
	that manage (sub)structured meta-types.  
	$Id: footprint_struct_base.cc,v 1.2.8.1 2006/12/25 03:27:36 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include "Object/def/footprint_struct_base.h"
#include "Object/def/footprint_base.tcc"
#include "Object/def/user_def_datatype.h"
#include "Object/def/process_definition.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/general_collection_type_manager.h"
#include "Object/inst/channel_instance_collection.h"
#include "Object/inst/process_instance.h"
#include "Object/inst/channel_instance.h"
#include "Object/inst/struct_instance.h"
#include "Object/inst/instance_scalar.h"
#include "Object/inst/instance_array.h"
#include "Object/inst/instance_collection_pool_bundle.tcc"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {

#if 0
template class instance_collection_pool_bundle<process_tag>;
template class instance_collection_pool_bundle<channel_tag>;
template class instance_collection_pool_bundle<datastruct_tag>;
#endif

template class footprint_base<process_tag>;
template class footprint_base<channel_tag>;
#if ENABLE_DATASTRUCTS
template class footprint_base<datastruct_tag>;
#endif

}	// end namespace entity
}	// end namespace HAC

