/**
	\file "Object/inst/footprint_value_base.cc"
	Explicit template instantiations of footprint_bases
	that manage value-collections.  
	$Id: footprint_value_base.cc,v 1.3.92.1 2010/09/08 21:14:17 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include "Object/def/footprint_value_base.h"
#include "Object/def/footprint.h"
#include "Object/def/footprint_base.tcc"
#include "Object/inst/value_collection_pool_bundle.tcc"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/inst/value_collection.h"
#include "Object/expr/const_collection.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/pint_const.h"
#include "Object/expr/preal_const.h"
#include "Object/expr/pstring_const.h"
#include "Object/inst/pbool_instance.h"
#include "Object/inst/pint_instance.h"
#include "Object/inst/preal_instance.h"
#include "Object/inst/pstring_instance.h"
#include "util/multikey_map.tcc"	// for destructor, when symbol optimized

namespace HAC {
namespace entity {

template class value_collection_pool_bundle<pbool_tag>;
template class value_collection_pool_bundle<pint_tag>;
template class value_collection_pool_bundle<preal_tag>;
template class value_collection_pool_bundle<pstring_tag>;

template class value_footprint_base<pbool_tag>;
template class value_footprint_base<pint_tag>;
template class value_footprint_base<preal_tag>;
template class value_footprint_base<pstring_tag>;

}	// end namespace entity
}	// end namespace HAC

