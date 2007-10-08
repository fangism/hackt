/**
	\file "Object/ref/references_fwd.h"
	Foward declarations of all reference related classes, 
	instances, values, meta, nonmeta, simple, member, aggregate...
	$Id: references_fwd.h,v 1.6 2007/10/08 01:21:35 fang Exp $
 */

#ifndef	__HAC_OBJECT_REF_REFERENCES_FWD_H__
#define	__HAC_OBJECT_REF_REFERENCES_FWD_H__

#include "Object/traits/classification_tags_fwd.h"

namespace HAC {
namespace entity {

class meta_instance_reference_base;

// from "Object/ref/*nonmeta_instance_reference*.h"
// note there are generic (non-meta) abstract base classes from which
// the meta-versions are derived.
// defined in "Object/ref/nonmeta_instance_reference_subtypes.h"
class nonmeta_instance_reference_base;
// base class per meta-class, 
// defined in "Object/ref/nonmeta_instance_reference_subtypes.h"
template <class>
class nonmeta_instance_reference;
class data_nonmeta_instance_reference;

class simple_meta_indexed_reference_base;
class simple_nonmeta_instance_reference_base;
class aggregate_meta_value_reference_base;	// exists?
class aggregate_meta_instance_reference_base;	// exists?

template <class>
class meta_instance_reference;
	
typedef meta_instance_reference<channel_tag>
	channel_meta_instance_reference_base;
typedef meta_instance_reference<process_tag>
	process_meta_instance_reference_base;
typedef meta_instance_reference<int_tag>
	int_meta_instance_reference_base;
typedef meta_instance_reference<bool_tag>
	bool_meta_instance_reference_base;
typedef meta_instance_reference<enum_tag>
	enum_meta_instance_reference_base;
typedef meta_instance_reference<real_tag>
	real_meta_instance_reference_base;
typedef meta_instance_reference<datastruct_tag>
	struct_meta_instance_reference_base;
// base classes for meta_value_references
class meta_value_reference_base;
template <class> class meta_value_reference;
typedef meta_value_reference<pbool_tag>
	pbool_meta_value_reference_base;
typedef meta_value_reference<pint_tag>
	pint_meta_value_reference_base;
typedef meta_value_reference<preal_tag>
	preal_meta_value_reference_base;

template <class>
class simple_meta_instance_reference;
template <class>
class simple_meta_dummy_reference;
template <class>
class aggregate_meta_instance_reference;
template <class>
class aggregate_meta_value_reference;
template <class>
class simple_nonmeta_instance_reference;
template <class>
class simple_nonmeta_value_reference;

typedef simple_nonmeta_instance_reference<channel_tag>
	simple_channel_nonmeta_instance_reference;
typedef simple_nonmeta_instance_reference<process_tag>
	simple_process_nonmeta_instance_reference;

#if 1
// value reference this?
typedef simple_nonmeta_instance_reference<datatype_tag>
	simple_datatype_nonmeta_instance_reference;
#endif
// data: use value reference
typedef simple_nonmeta_value_reference<int_tag>
	simple_int_nonmeta_instance_reference;
typedef simple_nonmeta_value_reference<bool_tag>
	simple_bool_nonmeta_instance_reference;
typedef simple_nonmeta_value_reference<enum_tag>
	simple_enum_nonmeta_instance_reference;
typedef simple_nonmeta_value_reference<real_tag>
	simple_real_nonmeta_instance_reference;
typedef simple_nonmeta_value_reference<datastruct_tag>
	simple_datastruct_nonmeta_instance_reference;

typedef simple_nonmeta_value_reference<pbool_tag>
	simple_pbool_nonmeta_instance_reference;
typedef	simple_pbool_nonmeta_instance_reference
	simple_pbool_nonmeta_value_reference;
typedef simple_nonmeta_value_reference<pint_tag>
	simple_pint_nonmeta_instance_reference;
typedef	simple_pint_nonmeta_instance_reference
	simple_pint_nonmeta_value_reference;
// not supported officially:
typedef simple_nonmeta_value_reference<preal_tag>
	simple_preal_nonmeta_instance_reference;
typedef	simple_preal_nonmeta_instance_reference
	simple_preal_nonmeta_value_reference;

typedef simple_meta_instance_reference<channel_tag>
	simple_channel_meta_instance_reference;
typedef simple_meta_instance_reference<process_tag>
	simple_process_meta_instance_reference;
typedef simple_meta_instance_reference<bool_tag>
	simple_bool_meta_instance_reference;
typedef simple_meta_dummy_reference<node_tag>
	simple_node_meta_instance_reference;
typedef simple_meta_instance_reference<int_tag>
	simple_int_meta_instance_reference;
typedef simple_meta_instance_reference<enum_tag>
	simple_enum_meta_instance_reference;
typedef simple_meta_instance_reference<real_tag>
	simple_real_meta_instance_reference;
typedef simple_meta_instance_reference<datastruct_tag>
	simple_datastruct_meta_instance_reference;
	

typedef aggregate_meta_instance_reference<channel_tag>
	aggregate_channel_meta_instance_reference;
typedef aggregate_meta_instance_reference<process_tag>
	aggregate_process_meta_instance_reference;
typedef aggregate_meta_instance_reference<bool_tag>
	aggregate_bool_meta_instance_reference;
typedef aggregate_meta_instance_reference<int_tag>
	aggregate_int_meta_instance_reference;
typedef aggregate_meta_instance_reference<enum_tag>
	aggregate_enum_meta_instance_reference;
typedef aggregate_meta_instance_reference<real_tag>
	aggregate_real_meta_instance_reference;
typedef aggregate_meta_instance_reference<datastruct_tag>
	aggregate_datastruct_meta_instance_reference;
	
template <class>
class member_meta_instance_reference;

typedef member_meta_instance_reference<process_tag>
	process_member_meta_instance_reference;
typedef member_meta_instance_reference<channel_tag>
	channel_member_meta_instance_reference;
typedef member_meta_instance_reference<bool_tag>
	bool_member_meta_instance_reference;
typedef member_meta_instance_reference<int_tag>
	int_member_meta_instance_reference;
typedef member_meta_instance_reference<enum_tag>
	enum_member_meta_instance_reference;
typedef member_meta_instance_reference<real_tag>
	real_member_meta_instance_reference;
typedef member_meta_instance_reference<datastruct_tag>
	datastruct_member_meta_instance_reference;

// expressions and family
template <class>
class simple_meta_value_reference;
typedef simple_meta_value_reference<pint_tag>
	simple_pint_meta_value_reference;
typedef simple_meta_value_reference<pbool_tag>
	simple_pbool_meta_value_reference;
typedef simple_meta_value_reference<preal_tag>
	simple_preal_meta_value_reference;

typedef aggregate_meta_value_reference<pint_tag>
	aggregate_pint_meta_value_reference;
typedef aggregate_meta_value_reference<pbool_tag>
	aggregate_pbool_meta_value_reference;
typedef aggregate_meta_value_reference<preal_tag>
	aggregate_preal_meta_value_reference;

template <class>
class simple_nonmeta_value_reference;
typedef simple_nonmeta_value_reference<int_tag>
	simple_int_nonmeta_value_reference;
typedef simple_nonmeta_value_reference<bool_tag>
	simple_bool_nonmeta_value_reference;
typedef simple_nonmeta_value_reference<enum_tag>
	simple_enum_nonmeta_value_reference;
typedef simple_nonmeta_value_reference<real_tag>
	simple_real_nonmeta_value_reference;
typedef simple_nonmeta_value_reference<datastruct_tag>
	simple_struct_nonmeta_value_reference;

}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_REF_REFERENCES_FWD_H__

