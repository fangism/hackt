/**
	\file "Object/inst/substructure_alias_base.tcc"
	$Id: substructure_alias_base.tcc,v 1.6.10.4 2006/04/09 04:34:01 fang Exp $
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
#include "Object/inst/alias_printer.h"
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
	TODO: rename this to __accept()
	\param c the bundle of cflat arguments.  
	\param e the globally allocated entry representing the canonical
		instance (and its name)
	\param gi the global index with which the global entry was looked up.
 */
template <class Tag>
void
substructure_alias_base<true>::__cflat_aliases(alias_printer& c, 
		const global_entry<Tag>& e, const size_t gi) const {
	STACKTRACE_VERBOSE;
	// saves away current footprint frame on stack, and restores on return
	const alias_printer::save_frame save(c, &e._frame);
	NEVER_NULL(c.fpf);
	NEVER_NULL(c.fpf->_footprint);
	c.fpf->_footprint->accept(c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses the same visitor pattern code as __cflat_aliases.  
	The choice of function names is unfortunate...
	TODO: rename this shit...
 */
template <class Tag>
void
substructure_alias_base<true>::__match_aliases(alias_matcher_base& c, 
		const global_entry<Tag>& e, const size_t gi) const {
	STACKTRACE_VERBOSE;
	// saves away current footprint frame on stack, and restores on return
	const alias_printer::save_frame save(c, &e._frame);
	NEVER_NULL(c.fpf);
	NEVER_NULL(c.fpf->_footprint);
	c.fpf->_footprint->accept(c);
}

//=============================================================================
/**
	Terminal.  

	TODO: This functionality should really be pushed into the alias_printer
	visitor functor -- to facilitate generic visitor pattern.  

	Copied from old alias_string_set::__dump_aliases().
	\param c the bundle of cflat arguments.  
	\param e the globally allocated entry representing the canonical
		instance (and its name)
	\param gi the global index with which the global entry was looked up.
	\pre gi MUST correspond to e!
 */
template <class Tag>
void
substructure_alias_base<false>::__cflat_aliases(alias_printer& c, 
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: move this code to specific visitor.  
	TODO: rename this shit...
	Wait, there's nothing to do...
 */
template <class Tag>
void
substructure_alias_base<false>::__match_aliases(alias_matcher_base& c, 
		const global_entry<Tag>& e, const size_t gi) const {
#if 0
	typedef	class_traits<Tag>		traits_type;
	STACKTRACE_VERBOSE;
	ostringstream os;
	e.dump_canonical_name(os, c.topfp, c.sm);
	const string& canonical(os.str());
	// might as well inclde self too for convenience
//	if (canonical != c.prefix) {
		// this should only be done for bool_tag!
		INVARIANT(gi < c.wires.size());
		c.aliases.push_back(c.prefix);
//	}
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_INST_SUBSTRUCTURE_ALIAS_BASE_TCC__

