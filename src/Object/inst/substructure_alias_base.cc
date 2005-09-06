/**
	\file "Object/inst/substructure_alias_base.cc"
	$Id: substructure_alias_base.cc,v 1.5.2.1 2005/09/06 05:56:48 fang Exp $
 */

#include <iostream>
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/instance_collection_base.h"
#include "common/ICE.h"
#include "util/macros.h"

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
substructure_alias::dump_hierarchical_name(ostream& o) const {
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
/**
	TODO: finish me.
 */
good_bool
substructure_alias::allocate_subinstance_footprint(footprint& f) const {
	return good_bool(true);
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

