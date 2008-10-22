/**
	\file "Object/inst/substructure_alias_base.cc"
	$Id: substructure_alias_base.cc,v 1.16 2008/10/22 22:16:59 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/instance_collection_base.h"
#include "Object/port_context.h"
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
#include "Object/common/dump_flags.h"
#endif
#include "common/ICE.h"
#include "util/macros.h"
#include "util/stacktrace.h"

namespace HAC {
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
	Virtually pure virtual.
 */
size_t
substructure_alias::hierarchical_depth(void) const {
	ICE_NEVER_CALL(cerr);
	return 0;
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
#if ENABLE_RELAXED_TEMPLATE_PARAMETERS
	try {
		this->finalize_find(c);
	} catch (...) {
		cerr << "Error trying to connect ports: incomplete type for `";
		this->dump_hierarchical_name(cerr, dump_flags::default_value);
		cerr << "\'." << endl;
		return good_bool(false);
	}
#endif
	return parent_type::__connect_ports(cr, c);
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
/**
	Once the children pointers have been loaded, this can
	go in and restore the link to the parents (this).  
 */
void
substructure_alias::load_object_base(const footprint& m, istream& i) {
	parent_type::load_object_base(m, i);
	relink_super_instance_alias(*this);	// restore_parent_child_links
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

