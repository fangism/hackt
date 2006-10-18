/**
	\file "Object/inst/collection_fwd.h"
	Forward declarations for all instance and value collection classes.  
	$Id: collection_fwd.h,v 1.3 2006/10/18 01:19:27 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_COLLECTION_FWD_H__
#define	__HAC_OBJECT_INST_COLLECTION_FWD_H__

#include "util/size_t.h"
#include "Object/traits/classification_tags_fwd.h"
#include "Object/devel_switches.h"

namespace HAC {
namespace entity {

class instance_collection_base;
class physical_instance_collection;
class datatype_instance_collection;	// exists?
#if USE_INSTANCE_PLACEHOLDERS
class instance_placeholder_base;
class physical_instance_placeholder;
class datatype_instance_placeholder;	// exists?
#endif

template <class>
class instance_collection;
#if USE_INSTANCE_PLACEHOLDERS
template <class>
class instance_placeholder;
#endif

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

#if USE_INSTANCE_PLACEHOLDERS
typedef instance_placeholder<bool_tag>
	bool_instance_placeholder;
typedef instance_placeholder<int_tag>
	int_instance_placeholder;
typedef instance_placeholder<enum_tag>
	enum_instance_placeholder;
typedef instance_placeholder<datastruct_tag>
	struct_instance_placeholder;
typedef instance_placeholder<channel_tag>
	channel_instance_placeholder;
typedef instance_placeholder<process_tag>
	process_instance_placeholder;
#endif

// TODO: rename to *_value_collection
template <class>
class value_collection;
class param_value_collection;
typedef value_collection<pint_tag>
	pint_instance_collection;
typedef value_collection<pbool_tag>
	pbool_instance_collection;
typedef value_collection<preal_tag>
	preal_instance_collection;

#if USE_INSTANCE_PLACEHOLDERS
template <class>
class value_placeholder;
class param_value_placeholder;
typedef value_placeholder<pint_tag>
	pint_value_placeholder;
typedef value_placeholder<pbool_tag>
	pbool_value_placeholder;
typedef value_placeholder<preal_tag>
	preal_value_placeholder;
#endif

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_COLLECTION_FWD_H__

