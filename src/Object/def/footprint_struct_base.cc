/**
	\file "Object/inst/footprint_struct_base.cc"
	Explicit template instantiations of footprint_bases
	that manage (sub)structured meta-types.  
	$Id: footprint_struct_base.cc,v 1.1.2.1 2006/11/03 07:07:32 fang Exp $
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

namespace HAC {
namespace entity {

template class footprint_base<process_tag>;
template class footprint_base<channel_tag>;
template class footprint_base<datastruct_tag>;

}	// end namespace entity
}	// end namespace HAC

