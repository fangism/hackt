/**
	\file "Object/inst/process_instance_collection.h"
	Class declarations for process instance and collections.  
	This file originated from "Object/art_object_instance_proc.h"
		in a previous life.
	$Id: process_instance_collection.h,v 1.2.4.2 2005/08/06 15:42:29 fang Exp $
 */

#ifndef	__OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_PROCESS_INSTANCE_COLLECTION_H__

#include "Object/inst/physical_instance_collection.h"
#include "Object/traits/proc_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/state_instance.h"

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
#if 0
class process_instance {
private:
	typedef	process_instance		this_type;
	typedef	process_instance_alias_info	alias_info_type;
private:
	// need back-reference(s) to owner(s) or hierarchical keys?
	never_ptr<const alias_info_type>	back_ref;

	// TODO: contain a vector of pointers to sub-structures
	// concrete definition map will map member names to index/offsets
	//	per-class structure layout...
	// empty for now

public:
	process_instance();

	explicit
	process_instance(const alias_info_type&);

	~process_instance();

	ostream&
	what(ostream&) const;

#if 0
	PERSISTENT_METHODS_DECLARATIONS
#else
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
#endif
};	// end class proc_instance
#else
/**
	Unique process state.
 */
class class_traits<process_tag>::state_instance_base {
protected:

};	// end class state_instance_base
#endif

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

