/**
	\file "Object/inst/port_alias_tracker.cc"
	$Id: port_alias_tracker.cc,v 1.12.2.2 2006/10/24 04:24:35 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <functional>
#include "Object/inst/port_alias_tracker.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/common/dump_flags.h"

#include "Object/traits/proc_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"

#include "util/macros.h"
#include "util/persistent_object_manager.h"
#include "util/IO_utils.h"
#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/sstream.h"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::read_value;
using util::write_value;
using util::auto_indent;

//=============================================================================
#if 0
template <class P>
struct second_is_unique {
	bool
	operator () (const P& p) const { return p.second.is_unique(); }
};
#endif

//=============================================================================
// class alias_reference_set method definitions

template <class Tag>
alias_reference_set<Tag>::alias_reference_set() : alias_array()
#if USE_ALIAS_STRING_CACHE
	, cache()
#endif
	{
	alias_array.reserve(2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
alias_reference_set<Tag>::~alias_reference_set() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
alias_reference_set<Tag>::push_back(const alias_ptr_type a) {
	NEVER_NULL(a);
	alias_array.push_back(a);
#if USE_ALIAS_STRING_CACHE
	cache.valid = false;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if USE_ALIAS_STRING_CACHE
template <class Tag>
struct alias_reference_set<Tag>::alias_to_string_transformer :
		public std::unary_function<alias_ptr_type, string> {
	typedef std::unary_function<alias_ptr_type, string>	parent_type;
	typename parent_type::result_type
	operator () (const typename parent_type::argument_type a) const {
		INVARIANT(a);
		std::ostringstream o;
		a->dump_hierarchical_name(o, dump_flags::no_owner);
		return o.str();
	}
};      // end struct alias_to_string_transformer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
alias_reference_set<Tag>::refresh_string_cache(void) const {
	if (!cache.valid) {
		cache.strings.resize(alias_array.size());
		std::transform(alias_array.begin(), alias_array.end(), 
			cache.strings.begin(), alias_to_string_transformer()
		);
		cache.valid = true;
	}
	// else is already valid
}

#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints all equivalent aliases as determined by this set.  
 */
template <class Tag>
ostream&
alias_reference_set<Tag>::dump(ostream& o) const {
	if (alias_array.size() > 1) {
		const_iterator i(alias_array.begin());
		const const_iterator e(alias_array.end());
		NEVER_NULL(*i);
		(*i)->dump_hierarchical_name(o);
		for (i++; i!=e; i++) {
			NEVER_NULL(*i);
			(*i)->dump_hierarchical_name(o << " = ");
		}
	} else if (!alias_array.empty()) {
		alias_array.front()->dump_hierarchical_name(o);
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param s the structured instance in which to resolve and 
		connect internal aliases of this type.  
 */
template <class Tag>
good_bool
alias_reference_set<Tag>::replay_internal_aliases(substructure_alias& s) const {
	STACKTRACE_VERBOSE;
	INVARIANT(alias_array.size() > 1);
	// alias_type& _alias(IS_A(alias_type&, s));	// assert dynamic_cast
	const_iterator i(alias_array.begin());
	const const_iterator e(alias_array.end());
	// resolve each formal subinstance reference in the alias ring
	// to the corresponding actual instance reference 
	// in the substructure argument
	alias_type& head((*i)->trace_alias(s));
	for (i++; i!=e; i++) {
		alias_type& _inst((*i)->trace_alias(s));
		// symmetric connection
		if (!alias_type::checked_connect_port(head, _inst).good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Edits the canonical back reference to point to the 
	shallowest instance.  
	Now this also flattens the union-find structure of each alias
	set by setting all aliases to point to the chosen canonical alias.
 */
template <class Tag>
typename alias_reference_set<Tag>::const_alias_ptr_type
alias_reference_set<Tag>::shortest_alias(void) {
	// typedef	alias_array_type::iterator		iterator;
	INVARIANT(alias_array.size());
	const_iterator i(alias_array.begin());
	const const_iterator e(alias_array.end());
	// accumulate to find the index of the shallowest alias
	// alias_ptr_type __shortest_alias(&*(*i)->find());
	alias_ptr_type __shortest_alias(*i);
	size_t shortest_depth = __shortest_alias->hierarchical_depth();
	for (++i; i!=e; ++i) {
		size_t depth = (*i)->hierarchical_depth();
		if ((depth < shortest_depth) && (*i != __shortest_alias)) {
			__shortest_alias = *i;
			shortest_depth = depth;
		}
	}
{
	// manually flatten the union-find structure
	iterator ii(alias_array.begin());
	// const iterator ee(alias_array.end());
	for ( ; ii!=e; ++ii) {
		(*ii)->finalize_canonicalize(*__shortest_alias);
	}
}
	// pardon the const_cast :S, we intend to modify, yes
	// consider making mutable...
	return __shortest_alias;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
alias_reference_set<Tag>::collect_transient_info_base(
		persistent_object_manager& m) const {
	// shouldn't have to do anything
	// containers of aliases already belong to definition scopes
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
alias_reference_set<Tag>::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	const size_t s = alias_array.size();
	write_value(o, s);
	size_t i = 0;
	for ( ; i<s; i++) {
		alias_array[i]->write_next_connection(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
alias_reference_set<Tag>::load_object_base(
		const persistent_object_manager& m, istream& i) {
	size_t s;
	read_value(i, s);
	size_t j = 0;
	alias_array.reserve(s);
	for ( ; j<s; j++) {
		alias_array.push_back(alias_ptr_type(
			&alias_type::load_alias_reference(m, i)));
	}
}

//=============================================================================
// class port_alias_tracker_base method definitions

template <class Tag>
port_alias_tracker_base<Tag>::port_alias_tracker_base() : _ids() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
port_alias_tracker_base<Tag>::~port_alias_tracker_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Removes all unique entries of the alias map, i.e. all alias sets
	with only one alias.  
 */
template <class Tag>
void
port_alias_tracker_base<Tag>::filter_unique(void) {
	// iterate and erase
	iterator i(_ids.begin());
	const iterator e(_ids.end());
	for ( ; i!=e; ) {
		if (i->second.is_unique()) {
			iterator j(i);
			j++;
			_ids.erase(i);
			i = j;
		} else {
			i++;
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
ostream&
port_alias_tracker_base<Tag>::dump_map(ostream& o) const {
if (!_ids.empty()) {
	o << auto_indent << class_traits<Tag>::tag_name
		<< " port aliases:" << endl;
	const_iterator i(_ids.begin());
	const const_iterator e(_ids.end());
	for ( ; i!=e; i++) {
		i->second.dump(o << auto_indent << i->first << ": ") << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
good_bool
port_alias_tracker_base<Tag>::__replay_aliases(substructure_alias& s) const {
	STACKTRACE_VERBOSE;
	const_iterator i(_ids.begin());
	const const_iterator e(_ids.end());
	for ( ; i!=e; i++) {
		if (!i->second.replay_internal_aliases(s).good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-sets the back references of the unique aliases to
	be the 'shortest; for canonicalization.  
	TODO: in addition to setting the back-reference, 
		also restructure the union-finds.  
 */
template <class Tag>
void
port_alias_tracker_base<Tag>::__shorten_canonical_aliases(
		instance_pool<state_instance<Tag> >& p) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("p.size() = " << p.size() << endl);
	iterator i(_ids.begin());
	const iterator e(_ids.end());
	for ( ; i!=e; i++) {
		typedef	typename alias_reference_set<Tag>::const_alias_ptr_type
						const_alias_ptr_type;
		const const_alias_ptr_type al(i->second.shortest_alias());
		INVARIANT(i->first);	// non-zero
		STACKTRACE_INDENT_PRINT("i->first = " << i->first << endl);
		BOUNDS_CHECK(i->first < p.size());
		p[i->first].set_back_ref(al);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
port_alias_tracker_base<Tag>::collect_map(persistent_object_manager& m) const {
	const_iterator i(_ids.begin());
	const const_iterator e(_ids.end());
	for ( ; i!=e; i++) {
		i->second.collect_transient_info_base(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
port_alias_tracker_base<Tag>::write_map(const persistent_object_manager& m, 
		ostream& o) const {
	write_value(o, _ids.size());
	const_iterator i(_ids.begin());
	const const_iterator e(_ids.end());
	for ( ; i!=e; i++) {
		write_value(o, i->first);
		i->second.write_object_base(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
port_alias_tracker_base<Tag>::load_map(const persistent_object_manager& m, 
		istream& i) {
	size_t s;
	read_value(i, s);
	size_t j = 0;
	for ( ; j<s; j++) {
		typename map_type::key_type k;
		read_value(i, k);
		_ids[k].load_object_base(m, i);
	}
	INVARIANT(_ids.size() == s);
}

//=============================================================================
// class port_alias_tracker method definitions

port_alias_tracker::port_alias_tracker() :
		port_alias_tracker_base<process_tag>(),
		port_alias_tracker_base<channel_tag>(),
		port_alias_tracker_base<datastruct_tag>(),
		port_alias_tracker_base<enum_tag>(),
		port_alias_tracker_base<int_tag>(),
		port_alias_tracker_base<bool_tag>(),
		has_internal_aliases(true) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_alias_tracker::~port_alias_tracker() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Removes all unique aliases from all sets.  
 */
void
port_alias_tracker::filter_uniques(void) {
	port_alias_tracker_base<process_tag>::filter_unique();
	port_alias_tracker_base<channel_tag>::filter_unique();
	port_alias_tracker_base<datastruct_tag>::filter_unique();
	port_alias_tracker_base<enum_tag>::filter_unique();
	port_alias_tracker_base<int_tag>::filter_unique();
	port_alias_tracker_base<bool_tag>::filter_unique();
	has_internal_aliases =
		!port_alias_tracker_base<process_tag>::_ids.empty() ||
		!port_alias_tracker_base<channel_tag>::_ids.empty() ||
		!port_alias_tracker_base<datastruct_tag>::_ids.empty() ||
		!port_alias_tracker_base<enum_tag>::_ids.empty() ||
		!port_alias_tracker_base<int_tag>::_ids.empty() ||
		!port_alias_tracker_base<bool_tag>::_ids.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
port_alias_tracker::replay_internal_aliases(substructure_alias& s) const {
	STACKTRACE_VERBOSE;
if (has_internal_aliases) {
	return good_bool(
		port_alias_tracker_base<process_tag>::__replay_aliases(s).good &&
		port_alias_tracker_base<channel_tag>::__replay_aliases(s).good &&
		port_alias_tracker_base<datastruct_tag>::__replay_aliases(s).good &&
		port_alias_tracker_base<enum_tag>::__replay_aliases(s).good &&
		port_alias_tracker_base<int_tag>::__replay_aliases(s).good &&
		port_alias_tracker_base<bool_tag>::__replay_aliases(s).good
	);
} else {
	return good_bool(true);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::shorten_canonical_aliases(footprint& f) {
	STACKTRACE_VERBOSE;
if (has_internal_aliases) {
	port_alias_tracker_base<process_tag>::
		__shorten_canonical_aliases(f.get_pool<process_tag>());
	port_alias_tracker_base<channel_tag>::
		__shorten_canonical_aliases(f.get_pool<channel_tag>());
	port_alias_tracker_base<datastruct_tag>::
		__shorten_canonical_aliases(f.get_pool<datastruct_tag>());
	port_alias_tracker_base<enum_tag>::
		__shorten_canonical_aliases(f.get_pool<enum_tag>());
	port_alias_tracker_base<int_tag>::
		__shorten_canonical_aliases(f.get_pool<int_tag>());
	port_alias_tracker_base<bool_tag>::
		__shorten_canonical_aliases(f.get_pool<bool_tag>());
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_alias_tracker::dump(ostream& o) const {
if (has_internal_aliases) {
	port_alias_tracker_base<process_tag>::dump_map(o);
	port_alias_tracker_base<channel_tag>::dump_map(o);
	port_alias_tracker_base<datastruct_tag>::dump_map(o);
	port_alias_tracker_base<enum_tag>::dump_map(o);
	port_alias_tracker_base<int_tag>::dump_map(o);
	port_alias_tracker_base<bool_tag>::dump_map(o);
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::collect_transient_info_base(
		persistent_object_manager& m) const {
	// these are all no-ops
if (has_internal_aliases) {
	port_alias_tracker_base<process_tag>::collect_map(m);
	port_alias_tracker_base<channel_tag>::collect_map(m);
	port_alias_tracker_base<datastruct_tag>::collect_map(m);
	port_alias_tracker_base<enum_tag>::collect_map(m);
	port_alias_tracker_base<int_tag>::collect_map(m);
	port_alias_tracker_base<bool_tag>::collect_map(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, has_internal_aliases);
if (has_internal_aliases) {
	port_alias_tracker_base<process_tag>::write_map(m, o);
	port_alias_tracker_base<channel_tag>::write_map(m, o);
	port_alias_tracker_base<datastruct_tag>::write_map(m, o);
	port_alias_tracker_base<enum_tag>::write_map(m, o);
	port_alias_tracker_base<int_tag>::write_map(m, o);
	port_alias_tracker_base<bool_tag>::write_map(m, o);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::load_object_base(
		const persistent_object_manager& m, istream& i) {
	read_value(i, has_internal_aliases);
if (has_internal_aliases) {
	port_alias_tracker_base<process_tag>::load_map(m, i);
	port_alias_tracker_base<channel_tag>::load_map(m, i);
	port_alias_tracker_base<datastruct_tag>::load_map(m, i);
	port_alias_tracker_base<enum_tag>::load_map(m, i);
	port_alias_tracker_base<int_tag>::load_map(m, i);
	port_alias_tracker_base<bool_tag>::load_map(m, i);
}
}

//=============================================================================
// explicit template instantiations

#define	INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(Tag)			\
template void alias_reference_set<Tag>::push_back(			\
		never_ptr<instance_alias_info<Tag> >);

INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(process_tag)
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(channel_tag)
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(datastruct_tag)
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(enum_tag)
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(int_tag)
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(bool_tag)

#if USE_ALIAS_STRING_CACHE
template void alias_reference_set<process_tag>::refresh_string_cache() const;
template void alias_reference_set<bool_tag>::refresh_string_cache() const;
#endif

#undef	INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

