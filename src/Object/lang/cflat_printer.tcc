/**
	\file "Object/lang/cflat_printer.tcc"
	Implementation of cflattening visitor.
	$Id: cflat_printer.tcc,v 1.2 2011/02/08 22:32:50 fang Exp $
 */

#ifndef	__HAC_OBJECT_LANG_CFLAT_PRINTER_TCC__
#define	__HAC_OBJECT_LANG_CFLAT_PRINTER_TCC__

#include <iostream>
#include <algorithm>
#include <set>
#include <sstream>
#include <numeric>
#include <vector>
#include "Object/lang/cflat_printer.hh"
#include "Object/def/footprint.hh"
#include "main/cflat_options.hh"

namespace HAC {
namespace entity {
namespace PRS {
#include "util/using_ostream.hh"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param ni the global node ID.  
 */
template <class Tag>
void
cflat_prs_printer::__dump_resolved_canonical_literal(ostream& o, 
		const size_t ni) const {
	if (cfopts.enquote_names) { o << '\"'; }
	parent_type::__dump_resolved_canonical_literal<Tag>(o, ni,
		cfopts.__dump_flags);
	if (cfopts.enquote_names) { o << '\"'; }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default, use the member output stream.
 */
template <class Tag>
void
cflat_prs_printer::__dump_resolved_canonical_literal(const size_t ni) const {
	return __dump_resolved_canonical_literal<Tag>(os, ni);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Translates local node reference to its canonical name.
	\param lni is the local node id, which needs to be resolved
		into the globally allocated id.  
 */
template <class Tag>
void
cflat_prs_printer::__dump_canonical_literal(ostream& o, 
		const size_t lni) const {
	__dump_resolved_canonical_literal<Tag>(o, 
		parent_type::lookup_global_id<Tag>(lni));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
cflat_prs_printer::__dump_canonical_literal(const size_t lni) const {
	__dump_resolved_canonical_literal<Tag>(os, 
		parent_type::lookup_global_id<Tag>(lni));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Print using default wrapper and delimiter.  
 */
template <class Tag>
void
cflat_prs_printer::__dump_canonical_literal_group(
		const directive_node_group_type& g) const {
	__dump_canonical_literal_group<Tag>(g, "{", ",", "}");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
cflat_prs_printer::__dump_resolved_literal_group(
		const directive_node_group_type& g) const {
	__dump_resolved_literal_group<Tag>(g, "{", ",", "}");
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The node list argument contains already unresolved local IDs.  
	\param g group argument
	\param l left wrapper, optional
	\param r right group wrapper, optional
	\param d group element delimiter
 */
template <class Tag>
void
cflat_prs_printer::__dump_canonical_literal_group(
		const directive_node_group_type& g, 
		const char* l, const char* d, const char* r) const {
	NEVER_NULL(d);
	// collect resolved (unique) node IDs here:
	directive_node_group_type s;
	__resolve_unique_literal_group<Tag>(g, s);
	__dump_resolved_literal_group<Tag>(s, l, d, r);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints a group of nodes.  
	Nodes in argument list are already resolved to global IDs.  
	By default, single nodes are not wrapped in braces, 
	and commas are used as delimiters.  
	Duplicate nodes are also eliminated.  
	\param g group argument
	\param l left wrapper, optional
	\param r right group wrapper, optional
	\param d group element delimiter
 */
template <class Tag>
void
cflat_prs_printer::__dump_resolved_literal_group(
		const directive_node_group_type& s, 
		const char* l, const char* d, const char* r) const {
	typedef	directive_node_group_type::const_iterator	const_iterator;
	if (s.size() > 1) {
		const_iterator i(s.begin()), e(s.end());
		if (l)	os << l;
		__dump_resolved_canonical_literal<Tag>(*i);
		for (++i; i!=e; ++i) {
			os << d;
			__dump_resolved_canonical_literal<Tag>(*i);
		}
		if (r)	os << r;
	} else {
		// only one element
		INVARIANT(!s.empty());
		__dump_resolved_canonical_literal<Tag>(*s.begin());
	}
}

//=============================================================================
}	// end namespace PRS
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_LANG_CFLAT_PRINTER_TCC__
