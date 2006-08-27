/**
	\file "Object/inst/physical_instance_collection.h"
	Instance collection classes for HAC.  
	This file came from "Object/art_object_instance.h" in a previous life.  
	$Id: physical_instance_collection.h,v 1.14.8.1 2006/08/27 07:52:02 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_H__
#define	__HAC_OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_H__

#include "Object/inst/instance_collection_base.h"
// #include "Object/inst/alias_visitee.h"

namespace HAC {
class cflat_options;
namespace entity {
class port_alias_tracker;
class state_manager;
class footprint_frame;
class port_collection_context;
struct alias_visitor;
struct dump_flags;

//=============================================================================
/**
	Base class for physical entity collections, 
	as opposed to value collections.  
	We don't bother deriving from alias_visitee because the traversal
	is not polymorphic, yet we implement the require accept() interface.  
 */
class physical_instance_collection : public instance_collection_base
	// , public alias_visitee	// not necessary
	{
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
#if USE_INSTANCE_PLACEHOLDERS
	physical_instance_collection() : parent_type() { }
#else
	explicit
	physical_instance_collection(const size_t d) : parent_type(d) { }

	physical_instance_collection(const scopespace& o, const string& n, 
		const size_t d);

	physical_instance_collection(const this_type& t, const footprint& f) :
		parent_type(t, f) { }
#endif

private:
#if !USE_INSTANCE_PLACEHOLDERS
virtual	MAKE_INSTANCE_COLLECTION_FOOTPRINT_COPY_PROTO = 0;
#endif

public:
virtual	~physical_instance_collection();

private:
	using parent_type::dump;	// don't intend to use directly

public:
	ostream&
	dump(ostream&, const dump_flags&) const;

virtual	ostream&
	dump_formal(ostream&) const = 0;

#define	UNROLL_PORT_ONLY_PROTO						\
	count_ptr<physical_instance_collection>				\
	unroll_port_only(const unroll_context&) const

virtual	UNROLL_PORT_ONLY_PROTO = 0;

virtual bool
	is_partially_unrolled(void) const = 0;

virtual ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const = 0;

#define	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO				\
	good_bool							\
	allocate_local_instance_ids(footprint&)

virtual	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO = 0;

#define	CONNECT_PORT_ALIASES_RECURSIVE_PROTO				\
	good_bool							\
	connect_port_aliases_recursive(physical_instance_collection&)

virtual	CONNECT_PORT_ALIASES_RECURSIVE_PROTO = 0;

#define	CREATE_DEPENDENT_TYPES_PROTO					\
	good_bool							\
	create_dependent_types(void)

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

virtual	void
	accept(alias_visitor&) const = 0;

virtual	count_ptr<meta_instance_reference_base>
	make_meta_instance_reference(void) const = 0;

protected:	// propagate to children
	using parent_type::collect_transient_info_base;
	using parent_type::write_object_base;
	using parent_type::load_object_base;
};	// end class physical_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_H__

