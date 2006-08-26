/**
	\file "Object/inst/instance_placeholder.cc"
	Since these are all lightweight classes, 
	we instantiated them together.  
	$Id: instance_placeholder.cc,v 1.1.2.1 2006/08/26 22:05:12 fang Exp $
 */

#include "Object/inst/instance_placeholder.tcc"
#include "Object/inst/datatype_instance_placeholder.h"
#include "Object/inst/datatype_instance_collection.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/traits/data_traits.h"
#include "Object/inst/null_collection_type_manager.h"
#include "Object/unroll/datatype_instantiation_statement.h"
// #include "Object/type/data_type_reference.h"

namespace HAC {
namespace entity {

// explicit template instantiations

template class instance_placeholder<bool_tag>;
// template class instance_placeholder<int_tag>;
// template class instance_placeholder<enum_tag>;
// template class instance_placeholder<struct_tag>;
// template class instance_placeholder<channel_tag>;
// template class instance_placeholder<process_tag>;

}	// end namespace entity
}	// end namespace HAC

