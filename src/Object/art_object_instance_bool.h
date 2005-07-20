/**
	\file "Object/art_object_instance_bool.h"
	Class declarations for built-in boolean data instances
	and instance collections.  
	$Id: art_object_instance_bool.h,v 1.12 2005/07/20 21:00:30 fang Exp $
 */

#ifndef	__OBJECT_ART_OBJECT_INSTANCE_BOOL_H__
#define	__OBJECT_ART_OBJECT_INSTANCE_BOOL_H__

#include "Object/art_object_instance.h"
#include "util/memory/list_vector_pool_fwd.h"
#include "Object/traits/bool_traits.h"

#include "Object/art_object_instance_alias.h"
#include "Object/art_object_instance_collection.h"

namespace ART {
namespace entity {

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

class bool_instance;

//=============================================================================

ostream&
operator << (ostream&, const bool_instance_alias_base&);

//-----------------------------------------------------------------------------
/**
	An actual instantiated instance of a bool, 
	what used to be called "node".
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
	This is like PrsNode from prsim.  

	Should be pool allocated for efficiency.  
 */
class bool_instance : public persistent {
	// need one back-reference to one alias (connected in a ring)
	never_ptr<const bool_instance_alias_base>	back_ref;
public:
	bool_instance();
	~bool_instance();

public:
	PERSISTENT_METHODS_DECLARATIONS

};	// end class bool_instance

//-----------------------------------------------------------------------------
// convenient typedefs

typedef	instance_array<bool_tag, 0>	bool_scalar;
typedef	instance_array<bool_tag, 1>	bool_array_1D;
typedef	instance_array<bool_tag, 2>	bool_array_2D;
typedef	instance_array<bool_tag, 3>	bool_array_3D;
typedef	instance_array<bool_tag, 4>	bool_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_ART_OBJECT_INSTANCE_BOOL_H__

