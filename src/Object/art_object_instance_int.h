/**
	\file "Object/art_object_instance_int.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	$Id: art_object_instance_int.h,v 1.11.14.1.4.1 2005/07/11 03:26:59 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_INT_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_INT_H__

#include "Object/art_object_instance.h"
#include "Object/traits/int_traits.h"
#include "Object/art_object_instance_collection.h"
#include "Object/art_object_instance_alias.h"

namespace ART {
namespace entity {
//=============================================================================

ostream&
operator << (ostream&, const int_instance_alias_base&);

//-----------------------------------------------------------------------------
class int_instance : public persistent {
	never_ptr<int_instance_alias_base>	back_ref;
public:
	int_instance();
	~int_instance();

public:
	PERSISTENT_METHODS_DECLARATIONS
};	// end class int_instance

//-----------------------------------------------------------------------------
// convenient typedefs

typedef	instance_array<int_tag, 0>	int_scalar;
typedef	instance_array<int_tag, 1>	int_array_1D;
typedef	instance_array<int_tag, 2>	int_array_2D;
typedef	instance_array<int_tag, 3>	int_array_3D;
typedef	instance_array<int_tag, 4>	int_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_INT_H__

