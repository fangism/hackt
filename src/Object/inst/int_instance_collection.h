/**
	\file "Object/inst/int_instance_collection.h"
	Class declarations for built-in and user-defined data instances
	and instance collections.  
	This file was "Object/art_object_instance_int.h"
		in a previous life.  
	$Id: int_instance_collection.h,v 1.2.4.3 2005/08/06 15:42:29 fang Exp $
 */

#ifndef	__OBJECT_INST_INT_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_INT_INSTANCE_COLLECTION_H__

#include "Object/inst/datatype_instance_collection.h"
#include "Object/traits/int_traits.h"
#include "Object/inst/instance_collection.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/state_instance.h"
// #include "util/memory/chunk_map_pool_fwd.h"

namespace ART {
namespace entity {
//=============================================================================

ostream&
operator << (ostream&, const int_instance_alias_base&);

//-----------------------------------------------------------------------------
/**
	State information for an integer.  
 */
#if 0
class int_instance {
	typedef	int_instance				this_type;
	typedef	int_instance_alias_info			alias_info_type;
private:
	never_ptr<const alias_info_type>		back_ref;
	/**
		TODO: use a real integer type for the state.  
	 */
	int						state;
public:
	int_instance();

	explicit
	int_instance(const alias_info_type&);

	~int_instance();

#if 0
	ostream&
	what(ostream&) const;

public:
	PERSISTENT_METHODS_DECLARATIONS
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
};	// end class int_instance
#else
class class_traits<int_tag>::state_instance_base {
protected:
	int						state;
};	// end class state_instance_base
#endif

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

