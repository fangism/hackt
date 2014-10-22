/**
	\file "Object/inst/physical_instance_collection.hh"
	Instance collection classes for HAC.  
	This file came from "Object/art_object_instance.h" in a previous life.  
	$Id: physical_instance_collection.hh,v 1.21 2010/04/07 00:12:42 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_HH__
#define	__HAC_OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_HH__

#include "Object/inst/instance_collection_base.hh"
#include "Object/unroll/target_context.hh"
#include "util/persistent_fwd.hh"

namespace HAC {
struct cflat_options;
namespace entity {
class port_alias_tracker;
class state_manager;
class footprint_frame;
class physical_instance_placeholder;
class alias_visitor;
struct dump_flags;
using util::persistent_object_manager;

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
	physical_instance_collection() : parent_type() { }

virtual	~physical_instance_collection();

private:
	using parent_type::dump;	// don't intend to use directly

public:
	ostream&
	dump(ostream&, const dump_flags&) const;

	size_t
	get_dimensions(void) const;

	parent_type::owner_ptr_type
	get_owner(void) const;

	const string&
	get_name(void) const;

virtual	never_ptr<const physical_instance_placeholder>
	get_placeholder_base(void) const = 0;

private:
	never_ptr<const instance_placeholder_base>
	__get_placeholder_base(void) const;

public:
virtual ostream&
	dump_unrolled_instances(ostream&, const dump_flags&) const = 0;

#define	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO				\
	good_bool							\
	allocate_local_instance_ids(footprint&)

virtual	ALLOCATE_LOCAL_INSTANCE_IDS_PROTO = 0;

#define	DEEP_COPY_STRUCTURE_PROTO					\
	never_ptr<physical_instance_collection>				\
	deep_copy(footprint&) const

virtual	DEEP_COPY_STRUCTURE_PROTO = 0;

#define	CONNECT_PORT_ALIASES_RECURSIVE_PROTO				\
	good_bool							\
	connect_port_aliases_recursive(physical_instance_collection&,	\
		target_context&)

virtual	CONNECT_PORT_ALIASES_RECURSIVE_PROTO = 0;

#define	RECONNECT_PORT_ALIASES_RECURSIVE_PROTO				\
	void								\
	reconnect_port_aliases_recursive(physical_instance_collection&)

virtual	RECONNECT_PORT_ALIASES_RECURSIVE_PROTO = 0;

#define	CREATE_DEPENDENT_TYPES_PROTO					\
	good_bool							\
	create_dependent_types(const footprint&)

virtual	CREATE_DEPENDENT_TYPES_PROTO = 0;

#define	COLLECT_PORT_ALIASES_PROTO					\
	void								\
	collect_port_aliases(port_alias_tracker&) const

virtual	COLLECT_PORT_ALIASES_PROTO = 0;

virtual	void
	accept(alias_visitor&) const = 0;

virtual	void
	collect_transient_info_base(persistent_object_manager&) const = 0;

virtual	void
	write_local_pointer(const footprint&, ostream&) const = 0;
};	// end class physical_instance_collection

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_PHYSICAL_INSTANCE_COLLECTION_HH__

