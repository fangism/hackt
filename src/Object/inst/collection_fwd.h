/**
	\file "Object/inst/collection_fwd.h"
	Forward declarations for all instance and value collection classes.  
	$Id: collection_fwd.h,v 1.8.46.1 2010/09/08 21:14:25 fang Exp $
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
class instance_placeholder_base;
class physical_instance_placeholder;
class datatype_instance_placeholder;	// exists?

template <class>
class collection_interface;

template <class>
class instance_collection;
template <class>
class instance_placeholder;

template <class>
class dummy_collection;
template <class>
class dummy_placeholder;

template <class, size_t>
class instance_array;

// forward declare partial specialzation
template <class Tag>
class instance_array<Tag, 0>;

template <class, size_t>
class value_array;

// forward declare partial specialzation
template <class Tag>
class value_array<Tag, 0>;

template <class>
class port_formal_array;

template <class>
class port_actual_collection;

// subclasses of datatype_instance_collection
typedef instance_collection<bool_tag>
	bool_instance_collection;
typedef instance_collection<int_tag>
	int_instance_collection;
typedef instance_collection<enum_tag>
	enum_instance_collection;
typedef instance_collection<real_tag>
	real_instance_collection;
typedef instance_collection<datastruct_tag>
	struct_instance_collection;
typedef instance_collection<channel_tag>
	channel_instance_collection;
typedef instance_collection<process_tag>
	process_instance_collection;
typedef dummy_collection<node_tag>
	node_instance_collection;

typedef instance_placeholder<bool_tag>
	bool_instance_placeholder;
typedef instance_placeholder<int_tag>
	int_instance_placeholder;
typedef instance_placeholder<enum_tag>
	enum_instance_placeholder;
typedef instance_placeholder<real_tag>
	real_instance_placeholder;
typedef instance_placeholder<datastruct_tag>
	struct_instance_placeholder;
typedef instance_placeholder<channel_tag>
	channel_instance_placeholder;
typedef instance_placeholder<process_tag>
	process_instance_placeholder;
typedef dummy_placeholder<node_tag>
	node_instance_placeholder;

typedef port_formal_array<bool_tag>
	bool_port_formal_array;
typedef port_formal_array<int_tag>
	int_port_formal_array;
typedef port_formal_array<enum_tag>
	enum_port_formal_array;
typedef port_formal_array<real_tag>
	real_port_formal_array;
typedef port_formal_array<datastruct_tag>
	struct_port_formal_array;
typedef port_formal_array<channel_tag>
	channel_port_formal_array;
typedef port_formal_array<process_tag>
	process_port_formal_array;

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
typedef value_collection<pstring_tag>
	pstring_instance_collection;

template <class>
class value_placeholder;
class param_value_placeholder;
typedef value_placeholder<pint_tag>
	pint_value_placeholder;
typedef value_placeholder<pbool_tag>
	pbool_value_placeholder;
typedef value_placeholder<preal_tag>
	preal_value_placeholder;
typedef value_placeholder<pstring_tag>
	pstring_value_placeholder;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_COLLECTION_FWD_H__

