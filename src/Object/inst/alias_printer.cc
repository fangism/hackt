/**
	\file "Object/inst/alias_printer.cc"
	$Id: alias_printer.cc,v 1.5 2007/01/21 05:59:08 fang Exp $
 */

#include "Object/inst/alias_printer.h"
#include "Object/devel_switches.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#if ENABLE_DATASTRUCTS
#include "Object/traits/struct_traits.h"
#endif
#if BUILTIN_CHANNEL_FOOTPRINTS
#include "Object/global_channel_entry.h"
#endif
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/def/footprint.h"
#include "Object/global_entry.tcc"
#include "Object/state_manager.h"
#include "Object/common/dump_flags.h"
#include "Object/common/alias_string_cache.h"
#include "main/cflat.h"
#include "main/cflat_options.h"
#include "util/sstream.h"
#include "util/macros.h"
#include "util/stacktrace.h"

namespace HAC {
namespace entity {
using std::ostringstream;
//=============================================================================
/**
	Defined by specialization only.  
	\param HasSubstructure whether or not the meta-class type
		has substructure.  
 */
template <bool HasSubstructure>
struct alias_printer_recursion_policy;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct alias_printer_recursion_policy<false> {
	template <class Tag>
	static
	void
	accept(alias_printer& c, const global_entry<Tag>& e, const size_t gi) {
		typedef class_traits<Tag>		traits_type;
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
};	// end struct alias_printer_recursion_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct alias_printer_recursion_policy<true> {
	template <class Tag>
	static
	void
	accept(alias_printer& c, const global_entry<Tag>& e, const size_t gi) {
		STACKTRACE_VERBOSE;
	STACKTRACE_VERBOSE;
	// saves away current footprint frame on stack, and restores on return
	const alias_printer::save_frame save(c, &e._frame);
	NEVER_NULL(c.fpf);
	NEVER_NULL(c.fpf->_footprint);
	c.fpf->_footprint->accept(c);
	}

};	// end struct alias_printer_recursion_policy

//=============================================================================
// class alias_printer method definitions

/**
	Rather than take a footprint argument, passed by the collection, 
	we get the footprint each time because collections with
	relaxed types may have different types per element.  
	This executes in a top-down traversal of the hierarchy.  
	NOTE: this code was relocated from instance_alias_info::cflat_aliases.
 */
template <class Tag>
void
alias_printer::__visit(const instance_alias_info<Tag>& a) {
	typedef class_traits<Tag>		traits_type;
	STACKTRACE_VERBOSE;
	// possibly uninstantiated because of conditionals
if (a.valid()) {
	ostringstream os;
	a.dump_hierarchical_name(os, dump_flags::no_leading_scope);
	const string& local_name(os.str());
	// construct new prefix from os
	const alias_printer::save_prefix save(*this);
	const global_entry_pool<Tag>& gp(this->sm.template get_pool<Tag>());
	size_t gindex;
if (this->fpf) {
	this->prefix += ".";
	// this is not a top-level instance (from recursion)
	const size_t local_offset = a.instance_index -1;
	const footprint_frame_map_type&
		fm(this->fpf->template get_frame_map<Tag>());
	// footprint_frame yields the global offset
	gindex = fm[local_offset];
} else {
	// footprint_frame is null, this is a top-level instance
	// the instance_index can be used directly as the offset into
	// the state_manager's member arrays
	BOUNDS_CHECK(a.instance_index && a.instance_index < gp.size());
	gindex = a.instance_index;
}
	this->prefix += local_name;
	const global_entry<Tag>& e(gp[gindex]);
	alias_printer_recursion_policy<traits_type::has_substructure>::accept(
		*this, e, gindex);
	// recursion or termination
}	// end if a.valid()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEFINE_INSTANCE_ALIAS_INFO_VISITOR(Tag)				\
void									\
alias_printer::visit(const instance_alias_info<Tag>& a) {		\
	__visit(a);							\
}

DEFINE_INSTANCE_ALIAS_INFO_VISITOR(bool_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(int_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(enum_tag)
#if ENABLE_DATASTRUCTS
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(datastruct_tag)
#endif
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(channel_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(process_tag)

#undef	DEFINE_INSTANCE_ALIAS_INFO_VISITOR

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

