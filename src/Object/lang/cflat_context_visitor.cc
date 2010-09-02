/**
	\file "Object/lang/cflat_context_visitor.cc"
	Implementation of cflattening visitor.
	$Id: cflat_context_visitor.cc,v 1.11 2010/09/02 00:34:39 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <set>
#include "Object/lang/cflat_context_visitor.h"
#include "Object/global_entry_context.tcc"
#include "Object/lang/SPEC_footprint.h"
#include "Object/global_entry.tcc"
#include "Object/traits/bool_traits.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
//=============================================================================
// class cflat_context_visitor method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ni the global node ID, 1-based.  
 */
template <class Tag>
ostream&
cflat_context_visitor::__dump_resolved_canonical_literal(
		ostream& os, const size_t ni, const dump_flags& df) const {
	// 0-based
	INVARIANT(ni);
	return topfp->template dump_canonical_name<Tag>(os, ni-1, df);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates set of local node IDs into unique set of 
	global IDs which may result in fewer nodes because duplicate
	aliases are dropped.  
	Can be replaced with transform : set_inserter
 */
template <class Tag>
void
cflat_context_visitor::__resolve_unique_literal_group(
		const directive_node_group_type& s,
		directive_node_group_type& d) const {
	typedef	directive_node_group_type::const_iterator	const_iterator;
	STACKTRACE_BRIEF;
	const_iterator i(s.begin()), e(s.end());
	for ( ; i!=e; ++i) {
		const size_t j = lookup_global_id<Tag>(*i);
		STACKTRACE_INDENT_PRINT("j = " << j << endl);
		INVARIANT(j);
		d.insert(j);
	}
}

//=============================================================================
// explicit template instantiations

template
ostream&
cflat_context_visitor::__dump_resolved_canonical_literal<bool_tag>(
		ostream&, const size_t, const dump_flags&) const;

template
ostream&
cflat_context_visitor::__dump_resolved_canonical_literal<process_tag>(
		ostream&, const size_t, const dump_flags&) const;

template
void
cflat_context_visitor::__resolve_unique_literal_group<bool_tag>(
		const directive_node_group_type&,
		directive_node_group_type&) const;

template
void
cflat_context_visitor::__resolve_unique_literal_group<process_tag>(
		const directive_node_group_type&,
		directive_node_group_type&) const;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

