/**
	\file "Object/art_object_instance_enum.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	$Id: art_object_instance_enum.h,v 1.11.14.2 2005/07/15 03:49:11 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_ENUM_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_ENUM_H__

#include "Object/art_object_instance.h"
#include "Object/traits/enum_traits.h"
#include "Object/art_object_instance_collection.h"
#include "Object/art_object_instance_alias.h"


namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of an enum.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class enum_instance : public persistent {
	// need back-reference(s) to owner(s) or hierarchical keys?
	never_ptr<const enum_instance_alias_base>	back_ref;
public:
	enum_instance();

	~enum_instance();

	PERSISTENT_METHODS_DECLARATIONS
};	// end class enum_instance

//=============================================================================
// convenient typedefs

typedef	instance_array<enum_tag, 0>	enum_scalar;
typedef	instance_array<enum_tag, 1>	enum_array_1D;
typedef	instance_array<enum_tag, 2>	enum_array_2D;
typedef	instance_array<enum_tag, 3>	enum_array_3D;
typedef	instance_array<enum_tag, 4>	enum_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_ENUM_H__

