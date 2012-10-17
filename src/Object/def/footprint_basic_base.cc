/**
	\file "Object/inst/footprint_basic_base.cc"
	Explicit template instantiations of footprint_bases
	that manage non-structured meta-types.  
	$Id: footprint_basic_base.cc,v 1.3 2011/02/04 02:23:35 fang Exp $
 */


#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include "Object/def/footprint_basic_base.hh"
#include "Object/def/footprint_base.tcc"
#include "Object/def/user_def_datatype.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/datatype_instance_placeholder.hh"
#include "Object/inst/datatype_instance_collection.hh"
#include "Object/inst/parameterless_collection_type_manager.hh"
#include "Object/inst/int_collection_type_manager.hh"
#include "Object/inst/null_collection_type_manager.hh"
#include "Object/inst/enum_instance.hh"
#include "Object/inst/int_instance.hh"
#include "Object/inst/bool_instance.hh"
#include "Object/inst/instance_scalar.hh"
#include "Object/inst/instance_array.hh"
#include "Object/inst/instance_collection_pool_bundle.tcc"
#include "Object/traits/instance_traits.hh"

namespace HAC {
namespace entity {

#if 0
template struct instance_collection_pool_bundle<bool_tag>;
template struct instance_collection_pool_bundle<int_tag>;
template struct instance_collection_pool_bundle<enum_tag>;
#endif

template class footprint_base<bool_tag>;
template class footprint_base<int_tag>;
template class footprint_base<enum_tag>;

}	// end namespace entity
}	// end namespace HAC

