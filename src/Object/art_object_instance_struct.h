/**
	\file "art_object_instance_struct.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	$Id: art_object_instance_struct.h,v 1.10 2005/02/27 22:54:17 fang Exp $
 */

#ifndef	__ART_OBJECT_INSTANCE_STRUCT_H__
#define	__ART_OBJECT_INSTANCE_STRUCT_H__

#include "art_object_instance.h"
#include "art_object_classification_details.h"
#include "art_object_instance_collection.h"
#include "art_object_instance_alias.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of an enum.
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

#endif	// __ART_OBJECT_INSTANCE_STRUCT_H__

