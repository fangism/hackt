/**
	\file "Object/inst/bool_instance_collection.h"
	Class declarations for built-in boolean data instances
	and instance collections.  
	This file was "Object/art_object_instance_bool.h" in a previous life.  
	$Id: bool_instance_collection.h,v 1.2.4.4 2005/08/06 15:42:27 fang Exp $
 */

#ifndef	__OBJECT_INST_BOOL_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_BOOL_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/traits/bool_traits.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/state_instance.h"
// #include "util/memory/chunk_map_pool_fwd.h"

namespace ART {
namespace entity {

//=============================================================================
// class datatype_instance_collection declared in "art_object_instance.h"

//=============================================================================

ostream&
operator << (ostream&, const bool_instance_alias_base&);

//-----------------------------------------------------------------------------
/**
	An actual instantiated instance of a bool, 
	what used to be called "node".
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
	This is like PrsNode from prsim -- it contains simulation state.  

	Really don't want this to be persistently allocated...
	Consider using a non-allocator pool, one whose members
		need not be deallocated until the end of the program.  
		That would save from having to reference-count.
	Is *very expensive* to keep each instance in separate
		entry per instance persistently.  
	Save this idea for later.  
	Replace reference-count pointer with index (ID#) to static pool.
		Such a scheme would have non-reclaimable memory (not leak).  
		Consider using list_vector.
		Is saving/restoring state complicated?

	Should be pool allocated for efficiency.  
 */
#if 0
class bool_instance {
	typedef	bool_instance				this_type;
	typedef	bool_instance_alias_info		alias_info_type;
private:
	// need one back-reference to one alias (connected in a ring)
	never_ptr<const alias_info_type>		back_ref;
	/**
		Boolean state information.  
		Consider pointing to another general structure.  
		See old prsim's prs_node.  
		Not used yet.  
	 */
	int						state;
public:
	bool_instance();

	explicit
	bool_instance(const alias_info_type&);
	~bool_instance();

#if 0
	ostream&
	what(ostream&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
	CHUNK_MAP_POOL_DEFAULT_STATIC_DECLARATIONS(64)
#else
	void
	collect_transient_info_base(persistent_object_manager&) const;

	void
	write_object_base(const persistent_object_manager&, ostream&) const;

	void
	load_object_base(const persistent_object_manager&, istream&);
#endif
public:
	static
	instance_pool<this_type>			pool;

};	// end class bool_instance
#else
class class_traits<bool_tag>::state_instance_base {
protected:
	int						state;
// persistence methods:
};	// end class state_instance_base
#endif

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

