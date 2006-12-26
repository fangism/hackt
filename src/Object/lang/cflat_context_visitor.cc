/**
	\file "Object/lang/cflat_context_visitor.cc"
	Implementation of cflattening visitor.
	$Id: cflat_context_visitor.cc,v 1.5.8.1 2006/12/26 21:26:07 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <set>
#include "Object/lang/cflat_context_visitor.h"
#include "Object/lang/SPEC_footprint.h"
#include "Object/global_entry.tcc"
#include "Object/state_manager.h"
#include "Object/traits/bool_traits.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class cflat_context_visitor method definitions

/**
	Can also be pushed to parent class.  
	Frequently used, consider inlining.  
	The footprint_frame is NOT set in the case of top-level (new supported)
	visits (b/c type frame is not applicable), in which case, 
	the referenced node ID *IS* the global index.  
	\param lni is the *local* node index referenced by this
		literal reference in this process.
	\return The local node index is translated into a globally 
		allocated (bool) node index, using the 
		footprint_frame_map.  
 */
size_t
cflat_context_visitor::__lookup_global_bool_id(const size_t lni) const {
	STACKTRACE_INDENT_PRINT("lookup_global_bool_id: lni = " << lni << endl);
	INVARIANT(lni);
	// cerr << "lni = " << lni << endl;
	if (fpf) {
		// see also footprint_frame_transformer in global_entry.h
#if 0
		// equivalent
		return footprint_frame_transformer(
			fpf->get_frame_map<bool_tag>())(lni);
#else
		return fpf->get_frame_map<bool_tag>()[lni-1];
#endif
	} else {
		// is top-level footprint
		return lni;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ni the global node ID.  
 */
ostream&
cflat_context_visitor::__dump_resolved_canonical_literal(
		ostream& os, const size_t ni) const {
	return sm->get_pool<bool_tag>()[ni]
		.dump_canonical_name(os, *topfp, *sm);
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
	typedef	directive_node_group_type::const_iterator	const_iterator;
	STACKTRACE_BRIEF;
	const_iterator i(s.begin()), e(s.end());
	for ( ; i!=e; ++i) {
		const size_t j = __lookup_global_bool_id(*i);
		STACKTRACE_INDENT_PRINT("j = " << j << endl);
		INVARIANT(j);
		d.insert(j);
	}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

