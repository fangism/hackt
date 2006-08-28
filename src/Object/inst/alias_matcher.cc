/**
	\file "Object/inst/alias_matcher.cc"
	$Id: alias_matcher.cc,v 1.3 2006/08/28 20:22:38 fang Exp $
 */

#include "Object/inst/alias_matcher.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/def/footprint.h"
#include "Object/global_entry.h"
#include "Object/state_manager.h"
#include "Object/common/dump_flags.h"
#include "util/macros.h"
#include "util/sstream.h"
#include "util/stacktrace.h"
#include "util/type_traits.h"
#include "util/attributes.h"

namespace HAC {
namespace entity {
using std::ostringstream;
using util::is_same;
//=============================================================================
template <bool B>
struct alias_matcher_recursion_policy;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Has no substructure, does nothing.  
 */
template <>
struct __VISIBILITY_HIDDEN__ alias_matcher_recursion_policy<false> {
	template <class Tag>
	static
	void
	accept(alias_matcher_base&, const global_entry<Tag>&) { }
};	// end struct alias_matcher_recursion_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Has substructure, checks containership policy.  
	TODO: check containership policy.  
 */
template <>
struct __VISIBILITY_HIDDEN__ alias_matcher_recursion_policy<true> {
	template <class Tag>
	static
	void
	accept(alias_matcher_base& c, const global_entry<Tag>& e) {
		const alias_matcher_base::save_frame save(c, &e._frame);
		NEVER_NULL(c.fpf);
		NEVER_NULL(c.fpf->_footprint);
		c.fpf->_footprint->accept(c);
	}
};	// end struct alias_matcher_recursion_policy

//=============================================================================
/**
	\param B whether or not the meta-class tags match.  
		If so, then we check for aliases and aggregate, 
		otherwise, we just conditionally recurse.  
 */
template <bool B>
struct match_aliases_implementation_policy;

template <>
struct __VISIBILITY_HIDDEN__ match_aliases_implementation_policy<false> {
	/**
		Conditional recursion.  
		\pre alias must be valid and instantiated.
	 */
	template <class AliasType, class MatcherType>
	static
	void
	accept(const AliasType& a, MatcherType& v) {
		typedef	typename AliasType::traits_type		traits_type;
		typedef	typename traits_type::tag_type		Tag;
		STACKTRACE_VERBOSE;
		ostringstream os;
		a.dump_hierarchical_name(os, dump_flags::no_leading_scope);
		const string& local_name(os.str());
		// construct new prefix from os
		const typename MatcherType::save_prefix save(v);
		const global_entry_pool<Tag>& gp(v.sm.template get_pool<Tag>());
		size_t gindex;
	if (v.fpf) {
		v.prefix += ".";
		// this is not a top-level instance (from recursion)
		const size_t local_offset = a.instance_index -1;
		const footprint_frame_map_type&
			fm(v.fpf->template get_frame_map<Tag>());
		// footprint_frame yields the global offset
		gindex = fm[local_offset];
	} else {
		// footprint_frame is null, this is a top-level instance
		// the instance_index can be used directly as the offset into
		// the state_manager's member arrays
		BOUNDS_CHECK(a.instance_index && a.instance_index < gp.size());
		gindex = a.instance_index;
	}
	v.prefix += local_name;
	const global_entry<Tag>& e(gp[gindex]);
	alias_matcher_recursion_policy<traits_type::has_substructure>
		::accept(v, e);
	// recursion or termination
	}	// end method accept
};	// end struct match_aliases_implementation_policy

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <>
struct __VISIBILITY_HIDDEN__ match_aliases_implementation_policy<true> {
	/**
		Meta-types match, so we compare indices to see if we
		have an alias match.  If so, then we construct this
		non-canonical name from context and append it to
		the list of known aliases.  
		Rip some code off of instance_alias_info::cflat_aliases().
		TODO: isn't there a way to use the footprint's
		set of scope aliases for this?
		TODO: minor optimization, stop recursion when we find an
		alias because an instance cannot contain its own type
		as a subinstance.  
		\pre alias must be valid and instantiated.
	 */
	template <class AliasType, class MatcherType>
	static
	void
	accept(const AliasType& a, MatcherType& v) {
		typedef	typename AliasType::traits_type		traits_type;
		typedef	typename traits_type::tag_type		Tag;
		STACKTRACE_VERBOSE;
		ostringstream os;
		a.dump_hierarchical_name(os, dump_flags::no_leading_scope);
		const string& local_name(os.str());
		// construct new prefix from os
		const typename MatcherType::save_prefix save(v);
		const global_entry_pool<Tag>& gp(v.sm.template get_pool<Tag>());
		size_t gindex;
	if (v.fpf) {
		v.prefix += ".";
		// this is not a top-level instance (from recursion)
		const size_t local_offset = a.instance_index -1;
		const footprint_frame_map_type&
			fm(v.fpf->template get_frame_map<Tag>());
		// footprint_frame yields the global offset
		gindex = fm[local_offset];
	} else {
		// footprint_frame is null, this is a top-level instance
		// the instance_index can be used directly as the offset into
		// the state_manager's member arrays
		BOUNDS_CHECK(a.instance_index && a.instance_index < gp.size());
		gindex = a.instance_index;
	}
	if (gindex == v.index) {
		// we have a match!
		v.aliases.push_back(v.prefix +local_name);
		// no need to recurse because an instance cannot contain
		// a subinstance of its own type, just return.
	} else {
		// not matched... conditionally recurse
		v.prefix += local_name;
		const global_entry<Tag>& e(gp[gindex]);
		alias_matcher_recursion_policy<traits_type::has_substructure>
			::accept(v, e);
		// recursion or termination
	}
	}	// end method accept
};	// end struct match_aliases_implementation_policy

//=============================================================================
// class alias_matcher method definitions

/**
	Aggregates aliases for a particular canonical instance reference.  
	\param Tag is the type of reference we're trying to find, 
	so we only recurse if 1) this has substructure, and
	2) Tag's meta-class type can possibly contain Tag2's type.  
 */
template <class Tag>
template <class Tag2>
void
alias_matcher<Tag>::__visit(const instance_alias_info<Tag2>& a) {
	typedef	match_aliases_implementation_policy<is_same<Tag,Tag2>::value>
					implementation_policy;
if (a.valid()) {
	// because it may be conditionally instantiated
	implementation_policy::accept(a, *this);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define	DEFINE_INSTANCE_ALIAS_INFO_VISITOR(Tag2)			\
template <class Tag>							\
void									\
alias_matcher<Tag>::visit(const instance_alias_info<Tag2>& a) {		\
	this->__visit(a);						\
}

DEFINE_INSTANCE_ALIAS_INFO_VISITOR(bool_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(int_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(enum_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(datastruct_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(channel_tag)
DEFINE_INSTANCE_ALIAS_INFO_VISITOR(process_tag)

#undef	DEFINE_INSTANCE_ALIAS_INFO_VISITOR

//=============================================================================
// explicit template instantiations

template struct alias_matcher<bool_tag>;
template struct alias_matcher<int_tag>;
template struct alias_matcher<enum_tag>;
template struct alias_matcher<datastruct_tag>;
template struct alias_matcher<channel_tag>;
template struct alias_matcher<process_tag>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

