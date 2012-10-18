/**
	\file "Object/inst/substructure_alias_base.cc"
	$Id: substructure_alias_base.cc,v 1.18 2010/04/07 00:12:46 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/inst/substructure_alias_base.hh"
#include "Object/inst/instance_collection_base.hh"
#include "Object/common/dump_flags.hh"
#include "common/ICE.hh"
#include "util/macros.h"
#include "util/stacktrace.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"

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
	try {
		this->finalize_find(c);
	} catch (...) {
		cerr << "Error trying to connect ports: incomplete type for `";
		this->dump_hierarchical_name(cerr, dump_flags::default_value);
		cerr << "\'." << endl;
		return good_bool(false);
	}
	return parent_type::__connect_ports(cr, c);
}

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

