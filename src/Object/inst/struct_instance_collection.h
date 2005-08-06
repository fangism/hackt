/**
	\file "Object/inst/struct_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_struct.h"
		in a previous life.  
	$Id: struct_instance_collection.h,v 1.2.4.1 2005/08/06 01:32:21 fang Exp $
 */

#ifndef	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_STRUCT_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/traits/struct_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of a struct.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
struct struct_instance {
private:
	typedef	struct_instance			this_type;
	typedef	datastruct_instance_alias_info	alias_info_type;
	// need back-reference(s) to owner(s) or hierarchical keys?
	never_ptr<const alias_info_type>	back_ref;
public:
	struct_instance();

	explicit
	struct_instance(const alias_info_type&);

	~struct_instance();

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

