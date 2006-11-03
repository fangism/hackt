/**
	\file "Object/inst/footprint_basic_base.cc"
	Explicit template instantiations of footprint_bases
	that manage non-structured meta-types.  
	$Id: footprint_basic_base.cc,v 1.1.2.1 2006/11/03 07:07:31 fang Exp $
 */


#define	ENABLE_STACKTRACE			1
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include "Object/def/footprint_basic_base.h"
#include "Object/def/footprint_base.tcc"
#include "Object/def/user_def_datatype.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/inst/parameterless_collection_type_manager.h"
#include "Object/inst/int_collection_type_manager.h"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/inst/enum_instance.h"
#include "Object/inst/int_instance.h"
#include "Object/inst/bool_instance.h"
#include "Object/inst/instance_scalar.h"
#include "Object/inst/instance_array.h"
#include "Object/inst/instance_collection_pool_bundle.tcc"

namespace HAC {
namespace entity {

template class footprint_base<bool_tag>;
template class footprint_base<int_tag>;
template class footprint_base<enum_tag>;

}	// end namespace entity
}	// end namespace HAC

