/**
	\file "Object/inst/footprint_value_base.cc"
	Explicit template instantiations of footprint_bases
	that manage value-collections.  
	$Id: footprint_value_base.cc,v 1.4 2010/09/21 00:18:11 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#define	STACKTRACE_PERSISTENTS			(0 && ENABLE_STACKTRACE)

#include "Object/def/footprint_value_base.hh"
#include "Object/def/footprint.hh"
#include "Object/def/footprint_base.tcc"
#include "Object/inst/value_collection_pool_bundle.tcc"
#include "Object/inst/null_collection_type_manager.hh"
#include "Object/inst/value_collection.hh"
#include "Object/expr/const_collection.hh"
#include "Object/expr/pbool_const.hh"
#include "Object/expr/pint_const.hh"
#include "Object/expr/preal_const.hh"
#include "Object/expr/pstring_const.hh"
#include "Object/inst/pbool_instance.hh"
#include "Object/inst/pint_instance.hh"
#include "Object/inst/preal_instance.hh"
#include "Object/inst/pstring_instance.hh"
#include "util/multikey_map.tcc"	// for destructor, when symbol optimized

namespace HAC {
namespace entity {

template struct value_collection_pool_bundle<pbool_tag>;
template struct value_collection_pool_bundle<pint_tag>;
template struct value_collection_pool_bundle<preal_tag>;
template struct value_collection_pool_bundle<pstring_tag>;

template class value_footprint_base<pbool_tag>;
template class value_footprint_base<pint_tag>;
template class value_footprint_base<preal_tag>;
template class value_footprint_base<pstring_tag>;

}	// end namespace entity
}	// end namespace HAC

