/**
	\file "Object/inst/port_alias_tracker.cc"
	$Id: port_alias_tracker.cc,v 1.1.2.2 2005/09/04 06:23:01 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/inst/port_alias_tracker.h"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/substructure_alias_base.h"

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

namespace ART {
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
alias_reference_set<Tag>::alias_reference_set() : alias_array() {
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
// class port_alias_tracker method definitions

port_alias_tracker::port_alias_tracker() :
		process_ids(), channel_ids(), struct_ids(), 
		enum_ids(), int_ids(), bool_ids(), 
		has_internal_aliases(true) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_alias_tracker::~port_alias_tracker() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Removes all unique entries of the alias map, i.e. all alias sets
	with only one alias.  
	\param M map type to filter.
 */
template <class M>
void
port_alias_tracker::filter_unique(M& m) {
#if 0
	// can't do this because remove_copy_if requires assignability
	std::remove_if(m.begin(), m.end(),
		second_is_unique<typename M::value_type>()
	);
#else
	// iterate and erase
	typedef	typename M::iterator	iterator;
	iterator i(m.begin());
	const iterator e(m.end());
	for ( ; i!=e; ) {
		if (i->second.is_unique()) {
			iterator j(i);
			j++;
			m.erase(i);
			i = j;
		} else {
			i++;
		}
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class M>
ostream&
port_alias_tracker::dump_map(const M& m, ostream& o) {
if (!m.empty()) {
	typedef	typename M::const_iterator	const_iterator;
	typedef	typename M::mapped_type::tag_type	tag_type;
	o << auto_indent << class_traits<tag_type>::tag_name
		<< " port aliases:" << endl;
	const_iterator i(m.begin());
	const const_iterator e(m.end());
	for ( ; i!=e; i++) {
		i->second.dump(o << auto_indent << i->first << ": ") << endl;
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class M>
good_bool
port_alias_tracker::__replay_aliases(const M& m, substructure_alias& s) {
	typedef	typename M::const_iterator	const_iterator;
	typedef	typename M::mapped_type::tag_type	tag_type;
	STACKTRACE_VERBOSE;
	const_iterator i(m.begin());
	const const_iterator e(m.end());
	for ( ; i!=e; i++) {
		if (!i->second.replay_internal_aliases(s).good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Removes all unique aliases from all sets.  
 */
void
port_alias_tracker::filter_uniques(void) {
	filter_unique(process_ids);
	filter_unique(channel_ids);
	filter_unique(struct_ids);
	filter_unique(enum_ids);
	filter_unique(int_ids);
	filter_unique(bool_ids);
	has_internal_aliases =
		!process_ids.empty() ||
		!channel_ids.empty() ||
		!struct_ids.empty() ||
		!enum_ids.empty() ||
		!int_ids.empty() ||
		!bool_ids.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
port_alias_tracker::replay_internal_aliases(substructure_alias& s) const {
	STACKTRACE_VERBOSE;
	if (has_internal_aliases) {
		return __replay_aliases(process_ids, s) &&
			__replay_aliases(channel_ids, s) &&
			__replay_aliases(struct_ids, s) &&
			__replay_aliases(enum_ids, s) &&
			__replay_aliases(int_ids, s) &&
			__replay_aliases(bool_ids, s);
	} else {
		return good_bool(true);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_alias_tracker::dump(ostream& o) const {
if (has_internal_aliases) {
	dump_map(process_ids, o);
	dump_map(channel_ids, o);
	dump_map(struct_ids, o);
	dump_map(enum_ids, o);
	dump_map(int_ids, o);
	dump_map(bool_ids, o);
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class M>
void
port_alias_tracker::collect_map(const M& am, persistent_object_manager& m) {
	typedef	typename M::const_iterator	const_iterator;
	const_iterator i(am.begin());
	const const_iterator e(am.end());
	for ( ; i!=e; i++) {
		i->second.collect_transient_info_base(m);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class M>
void
port_alias_tracker::write_map(const M& am, const persistent_object_manager& m, 
		ostream& o) {
	typedef	typename M::const_iterator	const_iterator;
	write_value(o, am.size());
	const_iterator i(am.begin());
	const const_iterator e(am.end());
	for ( ; i!=e; i++) {
		write_value(o, i->first);
		i->second.write_object_base(m, o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class M>
void
port_alias_tracker::load_map(M& am, const persistent_object_manager& m, 
		istream& i) {
	size_t s;
	read_value(i, s);
	size_t j = 0;
	for ( ; j<s; j++) {
		typename M::key_type k;
		read_value(i, k);
		am[k].load_object_base(m, i);
	}
	INVARIANT(am.size() == s);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::collect_transient_info_base(
		persistent_object_manager& m) const {
	// these are all no-ops
if (has_internal_aliases) {
	collect_map(process_ids, m);
	collect_map(channel_ids, m);
	collect_map(struct_ids, m);
	collect_map(enum_ids, m);
	collect_map(int_ids, m);
	collect_map(bool_ids, m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::write_object_base(
		const persistent_object_manager& m, ostream& o) const {
	write_value(o, has_internal_aliases);
if (has_internal_aliases) {
	write_map(process_ids, m, o);
	write_map(channel_ids, m, o);
	write_map(struct_ids, m, o);
	write_map(enum_ids, m, o);
	write_map(int_ids, m, o);
	write_map(bool_ids, m, o);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::load_object_base(
		const persistent_object_manager& m, istream& i) {
	read_value(i, has_internal_aliases);
if (has_internal_aliases) {
	load_map(process_ids, m, i);
	load_map(channel_ids, m, i);
	load_map(struct_ids, m, i);
	load_map(enum_ids, m, i);
	load_map(int_ids, m, i);
	load_map(bool_ids, m, i);
}
}

//=============================================================================
// explicit template instantiations

#define	INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(Tag)			\
template void alias_reference_set<Tag>::push_back(			\
		never_ptr<const instance_alias_info<Tag> >);

INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(process_tag);
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(channel_tag);
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(datastruct_tag);
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(enum_tag);
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(int_tag);
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(bool_tag);

//=============================================================================
}	// end namespace entity
}	// end namespace ART

