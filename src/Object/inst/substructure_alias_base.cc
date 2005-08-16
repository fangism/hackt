/**
	\file "Object/inst/substructure_alias_base.cc"
	$Id: substructure_alias_base.cc,v 1.4.4.1 2005/08/16 20:32:15 fang Exp $
 */

#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/instance_collection_base.h"
#include "util/macros.h"

namespace ART {
namespace entity {
//=============================================================================

/**
	Virtually pure virtual.
 */
ostream&
substructure_alias::dump_hierarchical_name(ostream& o) const {
	DIE;
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
subinstance_manager::value_type
substructure_alias::lookup_port_instance(
		const instance_collection_base& inst) const {
	return subinstances.lookup_port_instance(inst);
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
#if USE_MODULE_FOOTPRINT
void
substructure_alias::allocate_subinstances(footprint& f) {
	return subinstances.allocate(f);
}
#else
void
substructure_alias::allocate_subinstances(void) {
	return subinstances.allocate();
}
#endif

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

