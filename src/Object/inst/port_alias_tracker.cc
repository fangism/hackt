/**
	\file "Object/inst/port_alias_tracker.cc"
	$Id: port_alias_tracker.cc,v 1.21 2008/10/21 00:24:31 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <functional>
#include <algorithm>
#include <iterator>

#include "Object/inst/port_alias_tracker.tcc"
#include "Object/inst/alias_actuals.h"
#include "Object/inst/alias_empty.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/common/dump_flags.h"
#include "Object/def/footprint.h"
#include "Object/traits/proc_traits.h"
#include "Object/traits/chan_traits.h"
#include "Object/traits/struct_traits.h"
#include "Object/traits/enum_traits.h"
#include "Object/traits/int_traits.h"
#include "Object/traits/bool_traits.h"

#include "util/persistent_object_manager.h"
#include "util/copy_if.h"
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
using util::copy_if;
using std::for_each;
using std::back_inserter;

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
	alias_array.front()->dump_attributes(o);	// show attributes
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
		if (!alias_type::replay_connect_port(head, _inst).good)
			return good_bool(false);
		// doesn't require unroll_context
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
	// since this alters the union-find we must make sure that connection
	// flags are kept coherent, because normally they are only
	// maintained by the canonical alias of each set.
	// To accomplish this, we force a flag synchronization between
	// the new shortest alias and its canonical alias BEFORE
	// restructuring the union-find.
	__shortest_alias->update_direction_flags();

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
/**
	Can this be replaced with mem_fun(&alias_type::is_port_alias())?
	Need compose with dereference to mem_fun_ref.
	Give me tr1::mem_fn!
 */
template <class Tag>
struct alias_reference_set<Tag>::port_alias_predicate {
	bool
	operator () (const const_alias_ptr_type a) const {
		const bool ret = a->is_port_alias();
#if ENABLE_STACKTRACE
		a->dump_hierarchical_name(STACKTRACE_INDENT) << " is ";
		if (!ret) cerr << "not ";
		cerr << "a formal alias." << endl;
#endif
		return ret;
	}
};	// end struct port_alias_predicate

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
alias_reference_set<Tag>::__import_port_aliases(const this_type& s) {
	STACKTRACE_VERBOSE;
	alias_array.clear();	// just in case
	copy_if(s.alias_array.begin(), s.alias_array.end(), 
		back_inserter(alias_array), port_alias_predicate());
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
#if 0
template <class Tag>
void
alias_reference_set<Tag>::collect_transient_info_base(
		persistent_object_manager& m) const {
	// shouldn't have to do anything
	// containers of aliases already belong to definition scopes
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
alias_reference_set<Tag>::write_object_base(
		const collection_pool_bundle_type& m, 
		ostream& o) const {
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
		const collection_pool_bundle_type& m, 
		istream& i) {
	size_t s;
	read_value(i, s);
	size_t j = 0;
	alias_array.reserve(s);
	for ( ; j<s; j++) {
		alias_array.push_back(alias_ptr_type(
			&alias_type::load_alias_reference(m, i)));
	}
}
#endif	// AUTO_CACHE_FOOTPRPINT_SCOPE_ALIASES

//=============================================================================
// class port_alias_tracker_base method definitions

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
/**
	This is only ever instantiated for channels.
	Walks over all unique channel instances in scope, 
	and checks for dangling connections.  
 */
template <class Tag>
good_bool
port_alias_tracker_base<Tag>::check_connections(void) const {
	bool good = true;
	const_iterator i(_ids.begin()), e(_ids.end());
	for ( ; i!=e; ++i) {
		// grab the canonical alias from each set.
		INVARIANT(i->second.size());
		if (!i->second.front()->find()->check_connection().good) {
			// already have diagnostic message
			good = false;
		}
	}
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Functor for importing port aliases.  
	TODO: mark as visibility hiddden, no need to export.
 */
template <class Tag>
struct port_alias_tracker_base<Tag>::port_alias_importer {
	port_alias_tracker_base<Tag>&		tracker;

	explicit
	port_alias_importer(port_alias_tracker_base<Tag>& t) :
		tracker(t) { }

	void
	operator () (const value_type& p) {
		STACKTRACE_VERBOSE;
		// minor optimization: use insert() to get an iterator
		// and use it to erase to avoid second tree lookup.
		typename map_type::mapped_type& m(tracker._ids[p.first]);
		m.__import_port_aliases(p.second);
		// discard if empty or only has unique alias (filter unique)
		if (m.is_unique()) {
			tracker._ids.erase(p.first);
		}
	}
};	// end struct port_alias_importer

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Filters port_aliases from a map of alias sets into another.  
	\param t the source of scope-aliases.  
 */
template <class Tag>
void
port_alias_tracker_base<Tag>::__import_port_aliases(const this_type& t) {
	STACKTRACE_VERBOSE;
	const_iterator i(t._ids.begin()), e(t._ids.end());
	for_each(i, e, port_alias_importer(*this));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
#if 0
template <class Tag>
void
port_alias_tracker_base<Tag>::collect_map(persistent_object_manager& m) const {
	const_iterator i(_ids.begin());
	const const_iterator e(_ids.end());
	for ( ; i!=e; i++) {
		i->second.collect_transient_info_base(m);
	}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
port_alias_tracker_base<Tag>::write_map(const footprint& f, 
		ostream& o) const {
	const collection_pool_bundle_type&
		m(f.template get_instance_collection_pool_bundle<Tag>());
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
port_alias_tracker_base<Tag>::load_map(const footprint& f, istream& i) {
	const collection_pool_bundle_type&
		m(f.template get_instance_collection_pool_bundle<Tag>());
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
#endif	// AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES

//=============================================================================
// class port_alias_tracker method definitions

port_alias_tracker::port_alias_tracker() :
		port_alias_tracker_base<process_tag>(),
		port_alias_tracker_base<channel_tag>(),
#if ENABLE_DATASTRUCTS
		port_alias_tracker_base<datastruct_tag>(),
#endif
		port_alias_tracker_base<enum_tag>(),
		port_alias_tracker_base<int_tag>(),
		port_alias_tracker_base<bool_tag>(),
		has_internal_aliases(true) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_alias_tracker::~port_alias_tracker() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
port_alias_tracker::replay_internal_aliases(substructure_alias& s) const {
	STACKTRACE_VERBOSE;
if (has_internal_aliases) {
	return good_bool(
		port_alias_tracker_base<process_tag>::__replay_aliases(s).good &&
		port_alias_tracker_base<channel_tag>::__replay_aliases(s).good &&
#if ENABLE_DATASTRUCTS
		port_alias_tracker_base<datastruct_tag>::__replay_aliases(s).good &&
#endif
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
		__shorten_canonical_aliases(f.get_instance_pool<process_tag>());
	port_alias_tracker_base<channel_tag>::
		__shorten_canonical_aliases(f.get_instance_pool<channel_tag>());
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::
		__shorten_canonical_aliases(f.get_instance_pool<datastruct_tag>());
#endif
	port_alias_tracker_base<enum_tag>::
		__shorten_canonical_aliases(f.get_instance_pool<enum_tag>());
	port_alias_tracker_base<int_tag>::
		__shorten_canonical_aliases(f.get_instance_pool<int_tag>());
	port_alias_tracker_base<bool_tag>::
		__shorten_canonical_aliases(f.get_instance_pool<bool_tag>());
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only need to check channels, other meta-types don't have directions.  
	TODO: other things will need to be checked, like relaxed actual
		type completion.  
 */
good_bool
port_alias_tracker::check_channel_connections(void) const {
	return port_alias_tracker_base<channel_tag>::check_connections();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This collects aliases that are only ports, and
	filters out unique sets.  
	For alias replay and reconstruction from callee to caller.  
 */
void
port_alias_tracker::import_port_aliases(const this_type& t) {
	STACKTRACE_VERBOSE;
	port_alias_tracker_base<process_tag>::__import_port_aliases(t);
	port_alias_tracker_base<channel_tag>::__import_port_aliases(t);
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::__import_port_aliases(t);
#endif
	port_alias_tracker_base<enum_tag>::__import_port_aliases(t);
	port_alias_tracker_base<int_tag>::__import_port_aliases(t);
	port_alias_tracker_base<bool_tag>::__import_port_aliases(t);
	has_internal_aliases =
		!port_alias_tracker_base<process_tag>::_ids.empty() ||
		!port_alias_tracker_base<channel_tag>::_ids.empty() ||
#if ENABLE_DATASTRUCTS
		!port_alias_tracker_base<datastruct_tag>::_ids.empty() ||
#endif
		!port_alias_tracker_base<enum_tag>::_ids.empty() ||
		!port_alias_tracker_base<int_tag>::_ids.empty() ||
		!port_alias_tracker_base<bool_tag>::_ids.empty();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_alias_tracker::dump(ostream& o) const {
if (has_internal_aliases) {
	port_alias_tracker_base<process_tag>::dump_map(o);
	port_alias_tracker_base<channel_tag>::dump_map(o);
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::dump_map(o);
#endif
	port_alias_tracker_base<enum_tag>::dump_map(o);
	port_alias_tracker_base<int_tag>::dump_map(o);
	port_alias_tracker_base<bool_tag>::dump_map(o);
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if !AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES
#if 0
void
port_alias_tracker::collect_transient_info_base(
		persistent_object_manager& m) const {
	// these are all no-ops
if (has_internal_aliases) {
#if 0
	port_alias_tracker_base<process_tag>::collect_map(m);
	port_alias_tracker_base<channel_tag>::collect_map(m);
	port_alias_tracker_base<datastruct_tag>::collect_map(m);
	port_alias_tracker_base<enum_tag>::collect_map(m);
	port_alias_tracker_base<int_tag>::collect_map(m);
	port_alias_tracker_base<bool_tag>::collect_map(m);
#endif
}
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::write_object_base(const footprint& m, ostream& o) const {
	write_value(o, has_internal_aliases);
if (has_internal_aliases) {
	port_alias_tracker_base<process_tag>::write_map(m, o);
	port_alias_tracker_base<channel_tag>::write_map(m, o);
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::write_map(m, o);
#endif
	port_alias_tracker_base<enum_tag>::write_map(m, o);
	port_alias_tracker_base<int_tag>::write_map(m, o);
	port_alias_tracker_base<bool_tag>::write_map(m, o);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::load_object_base(const footprint& m, istream& i) {
	read_value(i, has_internal_aliases);
if (has_internal_aliases) {
	port_alias_tracker_base<process_tag>::load_map(m, i);
	port_alias_tracker_base<channel_tag>::load_map(m, i);
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::load_map(m, i);
#endif
	port_alias_tracker_base<enum_tag>::load_map(m, i);
	port_alias_tracker_base<int_tag>::load_map(m, i);
	port_alias_tracker_base<bool_tag>::load_map(m, i);
}
}
#endif	// AUTO_CACHE_FOOTPRINT_SCOPE_ALIASES

//=============================================================================
// explicit template instantiations

#define	INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(Tag)			\
template void alias_reference_set<Tag>::push_back(			\
		never_ptr<instance_alias_info<Tag> >);

INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(process_tag)
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(channel_tag)
#if ENABLE_DATASTRUCTS
INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK(datastruct_tag)
#endif
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

