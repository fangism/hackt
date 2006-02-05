/**
	\file "Object/inst/physical_instance_collection.h"
	Instance collection classes for HAC.  
	This file came from "Object/art_object_instance.h" in a previous life.  
	$Id: physical_instance_collection.h,v 1.10 2006/02/05 19:45:07 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_H__

#include "Object/inst/instance_collection_base.h"
#include "Object/devel_switches.h"

namespace HAC {
class cflat_options;
namespace entity {
class port_alias_tracker;
class state_manager;
class footprint_frame;
class port_collection_context;
struct cflat_aliases_arg_type;
struct dump_flags;

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

	physical_instance_collection(const this_type& t, const footprint& f) :
		parent_type(t, f) { }

private:
virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;

public:
virtual	~physical_instance_collection();

private:
	using parent_type::dump;	// don't intend to use directly

public:
	ostream&
	dump(ostream&, const dump_flags&) const;

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
#define	UNROLL_PORT_ONLY_PROTO						\
	count_ptr<physical_instance_collection>				\
	unroll_port_only(const unroll_context&) const

virtual	UNROLL_PORT_ONLY_PROTO = 0;

virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const = 0;

virtual	good_bool
	allocate_state(footprint&) = 0;

virtual	good_bool
	merge_created_state(this_type&, footprint&) = 0;

virtual	void
	inherit_created_state(const this_type&, const footprint&) = 0;

virtual	good_bool
	synchronize_actuals(this_type&) = 0;

#define	CREATE_DEPENDENT_TYPES_PROTO					\
	good_bool							\
	create_dependent_types(void) const

virtual	CREATE_DEPENDENT_TYPES_PROTO = 0;

#define	COLLECT_PORT_ALIASES_PROTO					\
	void								\
	collect_port_aliases(port_alias_tracker&) const

virtual	COLLECT_PORT_ALIASES_PROTO = 0;

#define	CONSTRUCT_PORT_CONTEXT_PROTO					\
	void								\
	construct_port_context(port_collection_context&, 		\
		const footprint_frame&) const

virtual	CONSTRUCT_PORT_CONTEXT_PROTO = 0;

#define	ASSIGN_FOOTPRINT_FRAME_PROTO					\
	void								\
	assign_footprint_frame(footprint_frame&,			\
		const port_collection_context&) const

virtual	ASSIGN_FOOTPRINT_FRAME_PROTO = 0;

#define	CFLAT_ALIASES_PROTO						\
	void								\
	cflat_aliases(const cflat_aliases_arg_type&) const

virtual	CFLAT_ALIASES_PROTO = 0;

#if INSTANCE_POOL_ALLOW_DEALLOCATION_FREELIST
#define	HACK_REMAP_INDICES_PROTO					\
	void								\
	hack_remap_indices(footprint&)

virtual	HACK_REMAP_INDICES_PROTO = 0;

#endif

protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class physical_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_H__

