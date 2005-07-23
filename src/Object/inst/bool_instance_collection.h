/**
	\file "Object/inst/bool_instance_collection.h"
	Class declarations for built-in boolean data instances
	and instance collections.  
	This file was "Object/art_object_instance_bool.h" in a previous life.  
	$Id: bool_instance_collection.h,v 1.2 2005/07/23 06:52:34 fang Exp $
 */

#ifndef	__OBJECT_INST_BOOL_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_BOOL_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "util/memory/list_vector_pool_fwd.h"
#include "Object/traits/bool_traits.h"

#include "Object/inst/instance_alias.h"
#include "Object/inst/instance_collection.h"

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

#endif	// __OBJECT_INST_BOOL_INSTANCE_COLLECTION_H__

