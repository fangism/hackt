/**
	\file "Object/inst/enum_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_enum.h"
		in a previous life.  
	$Id: enum_instance_collection.h,v 1.2.4.1 2005/08/06 01:32:20 fang Exp $
 */

#ifndef	__OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/traits/enum_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"

namespace ART {
namespace entity {
//=============================================================================
/**
	An actual instantiated instance of an enum.
	These are not constructed until after unrolling.  
	A final pass is required to construct the instances.  
 */
class enum_instance {
private:
	typedef	enum_instance_alias_info	alias_info_type;
	// need back-reference(s) to owner(s) or hierarchical keys?
	never_ptr<const alias_info_type>	back_ref;
public:
	enum_instance();

	explicit
	enum_instance(const alias_info_type&);

	~enum_instance();

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

#endif	// __OBJECT_INST_ENUM_INSTANCE_COLLECTION_H__

