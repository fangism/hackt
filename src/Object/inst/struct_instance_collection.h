/**
	\file "Object/inst/struct_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_struct.h"
		in a previous life.  
	$Id: struct_instance_collection.h,v 1.1.2.1 2005/07/22 00:25:15 fang Exp $
 */

#ifndef	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/traits/struct_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of a struct.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
struct struct_instance : public persistent {
	// need back-reference(s) to owner(s) or hierarchical keys?
	never_ptr<const struct_instance_alias_base>	back_ref;
public:
	struct_instance();

	~struct_instance();

	PERSISTENT_METHODS_DECLARATIONS
};	// end class struct_instance

//=============================================================================
// convenient typedefs

typedef	instance_array<datastruct_tag, 0>	struct_scalar;
typedef	instance_array<datastruct_tag, 1>	struct_array_1D;
typedef	instance_array<datastruct_tag, 2>	struct_array_2D;
typedef	instance_array<datastruct_tag, 3>	struct_array_3D;
typedef	instance_array<datastruct_tag, 4>	struct_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__

