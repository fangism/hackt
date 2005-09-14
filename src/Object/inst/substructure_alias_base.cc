/**
	\file "Object/inst/substructure_alias_base.cc"
	$Id: substructure_alias_base.cc,v 1.6 2005/09/14 15:30:32 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/instance_collection_base.h"
#include "Object/port_context.h"
#include "common/ICE.h"
#include "util/macros.h"
#include "util/stacktrace.h"

namespace ART {
namespace entity {
#include "util/using_ostream.h"

//=============================================================================
// class substructure_alias_base<false> method definitions

//=============================================================================
// class substructure_alias_base<true> method definitions

/**
	Virtually pure virtual.
 */
ostream&
substructure_alias::dump_hierarchical_name(ostream& o,
		const dump_flags&) const {
	ICE_NEVER_CALL(cerr);
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implemented for real in instance_alias_info.  
 */
size_t
substructure_alias::allocate_state(footprint&) const {
	ICE_NEVER_CALL(cerr);
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
subinstance_manager::value_type
substructure_alias::lookup_port_instance(const port_type& inst) const {
	return subinstances.lookup_port_instance(inst);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
never_ptr<const physical_instance_collection>
substructure_alias::get_container_base(void) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
substructure_alias&
substructure_alias::__trace_alias_base(const this_type&) const {
	ICE_NEVER_CALL(cerr);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param cr sequence of meta_instance_reference_base to connect
		to ports in order.
 */
good_bool
substructure_alias::connect_ports(const connection_references_type& cr, 
		const unroll_context& c) {
	return subinstances.connect_ports(cr, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
substructure_alias::collect_port_aliases(port_alias_tracker& p) const {
	return subinstances.collect_port_aliases(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
substructure_alias::allocate_subinstances(footprint& f) {
	return subinstances.allocate(f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
substructure_alias::replay_substructure_aliases(void) const {
	return subinstances.replay_internal_aliases();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/**
	TODO: finish me.
	NOTE: subinstance_manager only has public ports.  
		Somewhere else will take care of private subinstances.  
	First, construct ports.  
 */
good_bool
substructure_alias::__allocate_subinstance_footprint(footprint_frame& ff, 
		state_manager& sm) const {
	STACKTRACE_VERBOSE;
	port_member_context pmc;
	subinstances.construct_port_context(pmc, ff, sm);
	// HERE
	// pass port context to allocator for this instance.  (?)
	return good_bool(true);
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
substructure_alias::__assign_footprint_frame(footprint_frame& ff, 
		const port_member_context& pmc) const {
	subinstances.assign_footprint_frame(ff, pmc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Called recursively through substrcuture hierarchy.  
 */
void
substructure_alias::__construct_port_context(port_member_context& pmc, 
		const footprint_frame& ff) const {
	subinstances.construct_port_context(pmc, ff);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
substructure_alias::collect_transient_info_base(
		persistent_object_manager& m) const {
	subinstances.collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
substructure_alias::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	subinstances.write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Once the children pointers have been loaded, this can
	go in and restore the link to the parents (this).  
 */
void
substructure_alias::load_object_base(const persistent_object_manager& m,
		istream& i) {
	subinstances.load_object_base(m, i);
	subinstances.relink_super_instance_alias(*this);
}

//=============================================================================
}	// end namespace entity
}	// end namespace ART

