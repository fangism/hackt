/**
	\file "Object/inst/substructure_alias_base.tcc"
	$Id: substructure_alias_base.tcc,v 1.5.16.1 2006/02/19 03:53:05 fang Exp $
 */

#ifndef	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_TCC__
#define	__HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_TCC__

#include <iostream>
#include "Object/inst/substructure_alias_base.h"
#include "Object/common/cflat_args.h"
#include "Object/common/alias_string_cache.h"
#include "Object/global_entry.h"
#include "Object/def/footprint.h"
#include "Object/traits/class_traits_fwd.h"
#include "main/cflat.h"			// for cflat::print_alias
#include "main/cflat_options.h"
#include "util/sstream.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
using std::ostringstream;
//=============================================================================
/**
	Recursive.  
	\param c the bundle of cflat arguments.  
	\param e the globally allocated entry representing the canonical
		instance (and its name)
	\param gi the global index with which the global entry was looked up.
 */
template <class Tag>
void
substructure_alias_base<true>::__cflat_aliases(cflat_aliases_arg_type& c,
		const global_entry<Tag>& e, const size_t gi) const {
	STACKTRACE_VERBOSE;
	c.fpf = &e._frame;
	NEVER_NULL(c.fpf);
	NEVER_NULL(c.fpf->_footprint);
	c.fpf->_footprint->cflat_aliases(c);
}

//=============================================================================
/**
	Terminal.  
	Copied from old alias_string_set::__dump_aliases().
	\param c the bundle of cflat arguments.  
	\param e the globally allocated entry representing the canonical
		instance (and its name)
	\param gi the global index with which the global entry was looked up.
 */
template <class Tag>
void
substructure_alias_base<false>::__cflat_aliases(cflat_aliases_arg_type& c,
		const global_entry<Tag>& e, const size_t gi) const {
	typedef	class_traits<Tag>		traits_type;
	STACKTRACE_VERBOSE;
if (traits_type::print_cflat_leaf) {
	ostringstream os;
	e.dump_canonical_name(os, c.topfp, c.sm);
	const string& canonical(os.str());
if (!c.cf.check_prs) {
	if (!c.cf.wire_mode) {
		cflat::print_alias(c.o, canonical, c.prefix, c.cf);
	} else if (canonical != c.prefix) {
		// this should only be done for bool_tag!
		INVARIANT(gi < c.wires.size());
		c.wires[gi].strings.push_back(c.prefix);
	}
}
}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_TCC__

