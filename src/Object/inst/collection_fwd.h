/**
	\file "Object/inst/collection_fwd.h"
	Forward declarations for all instance and value collection classes.  
	$Id: collection_fwd.h,v 1.1.2.1 2006/04/10 23:21:27 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_COLLECTION_FWD_H__
#define	__HAC_OBJECT_INST_COLLECTION_FWD_H__

#include "util/size_t.h"
#include "Object/traits/classification_tags_fwd.h"

namespace HAC {
namespace entity {

class instance_collection_base;
class physical_instance_collection;
class datatype_instance_collection;	// exists?

template <class>
class instance_collection;

template <class, size_t>
class instance_array;
template <class, size_t>
class value_array;

// subclasses of datatype_instance_collection
typedef instance_collection<bool_tag>
	bool_instance_collection;
typedef instance_collection<int_tag>
	int_instance_collection;
typedef instance_collection<enum_tag>
	enum_instance_collection;
typedef instance_collection<datastruct_tag>
	struct_instance_collection;
typedef instance_collection<channel_tag>
	channel_instance_collection;
typedef instance_collection<process_tag>
	process_instance_collection;


template <class>
class value_collection;
class param_value_collection;
typedef value_collection<pint_tag>
	pint_instance_collection;
typedef value_collection<pbool_tag>
	pbool_instance_collection;
typedef value_collection<preal_tag>
	preal_instance_collection;


}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_COLLECTION_FWD_H__

