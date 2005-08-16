/**
	\file "Object/inst/physical_instance_collection.h"
	Instance collection classes for ART.  
	This file came from "Object/art_object_instance.h" in a previous life.  
	$Id: physical_instance_collection.h,v 1.3.4.1 2005/08/16 20:32:15 fang Exp $
 */

#ifndef	__OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_H__
#define	__OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_H__

#include "Object/inst/instance_collection_base.h"
#include "Object/devel_switches.h"

namespace ART {
namespace entity {
class footprint;
//=============================================================================
// class instance_collection_base defined in "Object/inst/instance_collection_base.h"

/**
	Base class for physical entity collections, 
	as opposed to value collections.  
 */
class physical_instance_collection : public instance_collection_base {
private:
	typedef	physical_instance_collection	this_type;
	typedef	instance_collection_base	parent_type;
protected:
	typedef	parent_type::inst_ref_ptr_type	inst_ref_ptr_type;
	typedef	parent_type::member_inst_ref_ptr_type	
						member_inst_ref_ptr_type;
	typedef	parent_type::instance_relaxed_actuals_type
						instance_relaxed_actuals_type;
protected:
	explicit
	physical_instance_collection(const size_t d) : parent_type(d) { }

	physical_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

public:

virtual	~physical_instance_collection();

	ostream&
	dump(ostream& o) const;

#if 0
	/** returns the type of the first instantiation statement */
	count_ptr<const fundamental_type_reference>
	get_type_ref(void) const;
#endif

#if 0
	// TODO: instance-references, simple and aggregate
	// unveil when the time is right
	good_bool
	may_check_reference_dimensions(...) const;

	good_bool
	must_check_reference_dimensions(...) const;
#endif

// macro co-defined in "Object/art_object_instance_collection.h"
#ifndef	UNROLL_PORT_ONLY_PROTO
#define	UNROLL_PORT_ONLY_PROTO						\
	count_ptr<physical_instance_collection>				\
	unroll_port_only(const unroll_context&) const
#endif

virtual	UNROLL_PORT_ONLY_PROTO = 0;

virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream& o) const = 0;

#if USE_MODULE_FOOTPRINT
virtual	void
	allocate_state(footprint&) = 0;

virtual	void
	merge_created_state(this_type&, footprint&) = 0;

virtual	void
	inherit_created_state(const this_type&, const footprint&) = 0;
#else
virtual	void
	allocate_state(void) = 0;

virtual	void
	merge_created_state(this_type&) = 0;

virtual	void
	inherit_created_state(const this_type&) = 0;
#endif

protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class physical_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_H__

