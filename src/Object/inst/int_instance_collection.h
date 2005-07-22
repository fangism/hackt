/**
	\file "Object/inst/int_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_int.h"
		in a previous life.  
	$Id: int_instance_collection.h,v 1.1.2.1 2005/07/22 00:25:12 fang Exp $
 */

#ifndef	__OBJECT_INST_INT_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_INT_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/traits/int_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias.h"

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

#endif	// __OBJECT_INST_INT_INSTANCE_COLLECTION_H__

