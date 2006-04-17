/**
	\file "Object/lang/cflat_context_visitor.cc"
	Implementation of cflattening visitor.
	$Id: cflat_context_visitor.cc,v 1.1.2.1 2006/04/17 03:04:07 fang Exp $
 */

#include <iostream>
#include <set>
#include "Object/lang/cflat_context_visitor.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/global_entry.h"
#include "Object/state_manager.h"
#include "Object/traits/bool_traits.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class cflat_context_visitor method definitions

/**
	\param lni is the *local* node index referenced by this
		literal reference in this process.
	\return The local node index is translated into a globally 
		allocated (bool) node index, using the 
		footprint_frame_map.  
 */
size_t
cflat_context_visitor::__lookup_global_bool_id(const size_t lni) const {
	INVARIANT(lni);
	return fpf->get_frame_map<bool_tag>()[lni-1];
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ni the global node ID.  
 */
ostream&
cflat_context_visitor::__dump_resolved_canonical_literal(
		ostream& os, const size_t ni) const {
	return sm->get_pool<bool_tag>()[ni].dump_canonical_name(os, *fp, *sm);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates local node reference to its canonical name.
	\param lni is the local node id, which needs to be resolved
		into the globally allocated id.  
 */
ostream&
cflat_context_visitor::__dump_canonical_literal(
		ostream& o, const size_t lni) const {
	return __dump_resolved_canonical_literal(
		o, __lookup_global_bool_id(lni));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates set of local node IDs into unique set of 
	global IDs which may result in fewer nodes because duplicate
	aliases are dropped.  
 */
void
cflat_context_visitor::__resolve_unique_literal_group(
		const directive_node_group_type& s,
		directive_node_group_type& d) const {
	typedef	directive_node_group_type::const_iterator
					const_iterator;
	const_iterator i(s.begin()), e(s.end());
	for ( ; i!=e; ++i) {
		d.insert(__lookup_global_bool_id(*i));
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

