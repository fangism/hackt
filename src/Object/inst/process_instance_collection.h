/**
	\file "Object/inst/process_instance_collection.h"
	Class declarations for process instance and collections.  
	This file originated from "Object/art_object_instance_proc.h"
		in a previous life.
	$Id: process_instance_collection.h,v 1.1.2.1 2005/07/22 00:25:14 fang Exp $
 */

#ifndef	__OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__

#include "Object/inst/physical_instance_collection.h"
#include "Object/traits/proc_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias.h"


namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of an process.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
	Needs to be pool allocated for efficient unique construction. 
	Derive from unique_instance_base.  
 */
class process_instance : public persistent {
	typedef	process_instance		this_type;
private:
	// need back-reference(s) to owner(s) or hierarchical keys?
	never_ptr<const process_instance_alias_base>	back_ref;

	// TODO: contain a vector of pointers to sub-structures
	// concrete definition map will map member names to index/offsets
	//	per-class structure layout...
	// empty for now

public:
	process_instance();
	~process_instance();

	ostream&
	what(ostream&) const;

	PERSISTENT_METHODS_DECLARATIONS

};	// end class proc_instance

//=============================================================================
// convenient typedefs

typedef	instance_array<process_tag, 0>	process_scalar;
typedef	instance_array<process_tag, 1>	process_array_1D;
typedef	instance_array<process_tag, 2>	process_array_2D;
typedef	instance_array<process_tag, 3>	process_array_3D;
typedef	instance_array<process_tag, 4>	process_array_4D;

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__

