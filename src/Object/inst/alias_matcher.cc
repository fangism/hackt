/**
	\file "Object/inst/alias_matcher.cc"
	$Id: alias_matcher.cc,v 1.1.2.1 2006/04/07 22:54:29 fang Exp $
 */

#include "Object/inst/alias_matcher.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/global_entry.h"
#include "Object/state_manager.h"
#include "Object/common/dump_flags.h"
#include "util/sstream.h"
#include "util/stacktrace.h"
#include "util/type_traits.h"
#include "util/attributes.h"

namespace HAC {
namespace entity {
using std::ostringstream;
using util::is_same;
//=============================================================================

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
	 */
	template <class AliasType, class MatcherType>
	void
	operator () (const AliasType& a, MatcherType& m) const {
	}
};

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
	 */
	template <class AliasType, class MatcherType>
	void
	operator () (const AliasType& a, MatcherType& v) const {
		typedef	typename AliasType::traits_type::tag_type	Tag;
		STACKTRACE_VERBOSE;
		INVARIANT(a.valid());
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
#if 0
		v.prefix += local_name;
		const global_entry<Tag>& e(gp[gindex]);
		a.__match_aliases(v, e, gindex);
#endif
		// recursion or termination
	}
	}
};	// end struct match_aliases_implementation_policy

//=============================================================================
/**
	Aggregates aliases for a particular canonical instance reference.  
	\param Tag is the type of reference we're trying to find, 
	so we only recurse if 1) this has substructure, and
	2) Tag's meta-class type can possibly contain Tag2's type.  
 */
INSTANCE_ALIAS_INFO_TEMPLATE_SIGNATURE
template <class Tag2>
void
INSTANCE_ALIAS_INFO_CLASS::match_aliases(alias_matcher<Tag2>& m) const {
	typedef	match_aliases_implementation_policy<is_same<Tag,Tag2>::value>
					implementation_policy;
	implementation_policy()(*this, m);
}

//=============================================================================
// class alias_matcher method definitions

/**
	Wrapped dispatcher to instance_alias_info::match_aliases. 
 */
template <class Tag>
template <class Tag2>
void
alias_matcher<Tag>::__visit(const instance_alias_info<Tag2>& a) {
	a.match_aliases(*this);
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

