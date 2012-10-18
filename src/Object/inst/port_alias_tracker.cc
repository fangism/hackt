/**
	\file "Object/inst/port_alias_tracker.cc"
	$Id: port_alias_tracker.cc,v 1.36 2011/03/23 00:36:11 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include <functional>
#include <algorithm>
#include <iterator>
#include <sstream>

#include "Object/inst/port_alias_tracker.tcc"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/global_channel_entry.hh"
#include "Object/inst/connection_policy.hh"
#include "Object/inst/substructure_alias_base.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/inst/state_instance.hh"
#include "Object/common/dump_flags.hh"
#include "Object/def/footprint.hh"
#include "Object/traits/proc_traits.hh"
#include "Object/traits/chan_traits.hh"
#include "Object/traits/struct_traits.hh"
#include "Object/traits/enum_traits.hh"
#include "Object/traits/int_traits.hh"
#include "Object/traits/bool_traits.hh"
#include "main/create_options.hh"

#include "util/persistent_object_manager.hh"
#include "util/copy_if.hh"
#include "util/STL/functional.hh"	// for _Select2nd
#include "util/IO_utils.hh"
#include "util/indent.hh"
#include "util/stacktrace.hh"
#include "util/sstream.hh"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::read_value;
using util::write_value;
using util::auto_indent;
USING_COPY_IF
using std::for_each;
using std::back_inserter;
using std::not1;
using std::ostringstream;

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
		a->dump_hierarchical_name(o, dump_flags::no_owners);
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
	Define to 1 to print ALL attributes for ALL aliases, 
	which *should* be redundant, as they should match.
	Really only useful for debugging.  
 */
#define	VERBOSE_ALIAS_ATTRIBUTES		0

/**
	Prints all equivalent aliases as determined by this set.  
 */
template <class Tag>
ostream&
alias_reference_set<Tag>::dump(ostream& o, const dump_flags& df) const {
	if (alias_array.size() > 1) {
		const_iterator i(alias_array.begin());
		const const_iterator e(alias_array.end());
		NEVER_NULL(*i);
		(*i)->dump_hierarchical_name(o, df);
#if VERBOSE_ALIAS_ATTRIBUTES
		(*i)->dump_attributes(o);	// check consistency
#endif
		for (++i; i!=e; ++i) {
			NEVER_NULL(*i);
			(*i)->dump_hierarchical_name(o << " = ", df);
#if VERBOSE_ALIAS_ATTRIBUTES
			(*i)->dump_attributes(o);	// check consistency
#endif
		}
	} else if (!alias_array.empty()) {
		alias_array.front()->dump_hierarchical_name(o, df);
#if VERBOSE_ALIAS_ATTRIBUTES
		alias_array.front()->dump_attributes(o);
#endif
	}
#if !VERBOSE_ALIAS_ATTRIBUTES
	alias_array.front()->dump_attributes(o);
#endif
	return o;
}

#undef VERBOSE_ALIAS_ATTRIBUTES

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if ANY alias in this set is a direct port alias, 
		or member thereof, return that pointer if true.  
	reminder: instance_alias_info::is_port_alias doesn't check
		equivalent aliases in the same union-find.  
 */
template <class Tag>
typename alias_reference_set<Tag>::alias_ptr_type
alias_reference_set<Tag>::is_aliased_to_port(void) const {
	const_iterator i(alias_array.begin());
	const const_iterator e(alias_array.end());
	for ( ; i!=e; ++i) {
		if ((*i)->is_port_alias())
			return *i;
	}
	return alias_ptr_type(NULL);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Overrides allocation-assigned ID with new value for all aliases
	in this set.
 */
template <class Tag>
void
alias_reference_set<Tag>::override_id(const size_t id) {
	iterator i(alias_array.begin());
	const iterator e(alias_array.end());
if ((*i)->instance_index != id) {
	// all-or-none: invariant: all instance IDs are the same
	for ( ; i!=e; ++i) {
		(*i)->instance_index = id;
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Swaps the alias arrays *without* updating their respective
	instance_index-s!  Use with caution!
 */
template <class Tag>
void
alias_reference_set<Tag>::bare_swap(this_type& t) {
	alias_array.swap(t.alias_array);
}

/**
	Swaps alias sets AND their aliases' ID numbers (locally allocated
	unique IDs)!
 */
template <class Tag>
void
alias_reference_set<Tag>::swap(this_type& t) {
	const size_t lid = alias_array.front()->instance_index;
	const size_t rid = t.alias_array.front()->instance_index;
	bare_swap(t);
	this->override_id(lid);
	t.override_id(rid);
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
	Copy the relaxed actuals and attribute flags (properties) of
	each alias (in this definition scope) to the corresponding alias 
	in the substructure (of the instance scope).  
 */
template <class Tag>
void
alias_reference_set<Tag>::export_alias_properties(substructure_alias& s) const {
	STACKTRACE_VERBOSE;
	INVARIANT(!alias_array.empty());
	// find a direct port alias in the set, if one exists
	const const_iterator e(alias_array.end());
	const const_iterator f(std::find_if(alias_array.begin(), e, 
		port_alias_predicate()));
if (f != e) {
	const alias_type& a(**f);
#if ENABLE_STACKTRACE
	a.dump_hierarchical_name(STACKTRACE_INDENT_PRINT("name: ")) << endl;
#endif
		STACKTRACE_INDENT_PRINT("is port alias" << endl);
		alias_type& _inst(a.trace_alias(s));
	// this call should NOT be recursive because iteration over ports
	// mitigates the need to recurse.
		// predicate is needed because this is called on the 
		// set of *scope* aliases which includes non-ports.
		_inst.import_properties(a);
		// FIXME: want directions initialized too!
} else { STACKTRACE_INDENT_PRINT("is not port alias" << endl); }
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Convenient alias collector.
	TODO: add predicated version
 */
template <class Tag>
void
alias_reference_set<Tag>::export_alias_strings(
		const dump_flags& df, 
		set<string>& aliases) const {
	const_iterator i(this->begin()), e(this->end());
	for ( ; i!=e; ++i) {
		const instance_alias_info<Tag>& a(**i);
		std::ostringstream alias;
		a.dump_hierarchical_name(alias, df);
			// was dump_flags::no_leading_scope
		aliases.insert(alias.str());
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return iterator pointing to first port alias found
 */
template <class Tag>
typename alias_reference_set<Tag>::const_iterator
alias_reference_set<Tag>::find_any_port_alias(void) const {
	const_iterator ai(this->begin()), ae(this->end());
	for ( ; ai!=ae; ++ai) {
		// just take the first one, arbitrary
		NEVER_NULL(*ai);
		if ((*ai)->is_port_alias()) {
			return ai;
		}
	}
//	INVARIANT(ai != ae);		// should not be reached!
	return ae;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Non-modifying, finds the shortest alias in the set.  
	The sorting policy is passed in through global_create_options 
	global variable.
	\param port_only true if only port aliases are desired.
 */
template <class Tag>
typename alias_reference_set<Tag>::const_iterator
alias_reference_set<Tag>::find_shortest_alias(const bool port_only) const {
	STACKTRACE_VERBOSE;
	INVARIANT(alias_array.size());
	const const_iterator b(alias_array.begin()), e(alias_array.end());
	const_iterator i(b);
	// accumulate to find the index of the shallowest alias
	const_iterator bi(b);
//	alias_ptr_type __shortest_alias(*i);
	size_t shortest_depth = (*i)->hierarchical_depth();
	bool shortest_is_port = (*i)->is_port_alias();
//	global_create_options.dump(cerr);
switch (global_create_options.canonicalize_mode) {
case SHORTEST_HIER_MIN_LENGTH: {
	size_t shortest_length;
	string best_name;
	static const dump_flags& df(dump_flags::no_definition_owner);
{
	ostringstream oss;
	(*i)->dump_hierarchical_name(oss, df);
	best_name = oss.str();
	shortest_length = best_name.length();
	// first one may not necessarily be a port!
}
	for (++i; i!=e; ++i) {
		const bool pa((*i)->is_port_alias());
	if (!port_only || pa) {
		size_t depth = (*i)->hierarchical_depth();
		string name;
		size_t length;
	{
		ostringstream oss;
		(*i)->dump_hierarchical_name(oss, df);
		name = oss.str();
		length = name.length();
	}
		STACKTRACE_INDENT_PRINT("shorten: " << best_name << " vs. " << name << endl);
		const bool z = port_only && !shortest_is_port && pa;
		if (z || (depth < shortest_depth || 
			(depth == shortest_depth && length < shortest_length))
			// && (*i != __shortest_alias)
			) {
			// __shortest_alias = *i;
			shortest_depth = depth;
			shortest_length = length;
			shortest_is_port = pa;
			best_name = name;
			bi = i;
		}
	}
	}
	STACKTRACE_INDENT_PRINT("BEST: " << best_name << endl);
	break;
}
// TODO: SHORTEST_EMULATE_ACT
case SHORTEST_HIER_NO_LENGTH:
default:
{
	for (++i; i!=e; ++i) {
		const bool pa((*i)->is_port_alias());
	if (!port_only || pa) {
		size_t depth = (*i)->hierarchical_depth();
		const bool z = port_only && !shortest_is_port && pa;
		if (z || (depth < shortest_depth)
			// && (*i != __shortest_alias)
			) {
			// __shortest_alias = *i;
			shortest_depth = depth;
			shortest_is_port = pa;
			bi = i;
		}
	}
	}
	break;
}
}	// end switch
	// if called on scope_alias set, may not necessarily find a port alias
	if (port_only && !shortest_is_port) {
		return e;
	}
	return bi;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Edits the canonical back reference to point to the 
	shallowest instance.  
	However, the alias list retains its original order!
	So the front is not necessarily canonical!
	Now this also flattens the union-find structure of each alias
	set by setting all aliases to point to the chosen canonical alias.
	\param bool slen set true to consider string-length as a tiebreaker.
	Alter the alias_array to move the shortest canonical node to front?
 */
template <class Tag>
typename alias_reference_set<Tag>::const_alias_ptr_type
alias_reference_set<Tag>::shortest_alias(void) {
	const const_iterator __shortest_i(this->find_shortest_alias(false));
	const alias_ptr_type __shortest_alias(*__shortest_i);

	// manually flatten the union-find structure
	// since this alters the union-find we must make sure that connection
	// flags are kept coherent, because normally they are only
	// maintained by the canonical alias of each set.
	// To accomplish this, we force a flag synchronization between
	// the new shortest alias and its canonical alias BEFORE
	// restructuring the union-find.
	__shortest_alias->update_direction_flags();

	const iterator b(alias_array.begin()), e(alias_array.end());
	iterator ii(b);
	for ( ; ii!=e; ++ii) {
		(*ii)->finalize_canonicalize(*__shortest_alias);
	}
	// after flattening array, and updating canonical flags
	// copy flags to all other aliases
	for (ii=b ; ii!=e; ++ii) {
		(*ii)->update_direction_flags();
	}
	// NOTE: this is currently called before all direction flags
	// have been updated by definition::unroll_lang() (prs, chp)

	// pardon the const_cast :S, we intend to modify, yes
	// consider making mutable...
	return __shortest_alias;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Makes all flags/attributes consistent among aliases within a set.
 */
template <class Tag>
void
alias_reference_set<Tag>::synchronize_flags(void) {
	// for_each(...);
	const iterator b(alias_array.begin()), e(alias_array.end());
	iterator i(b);
	for ( ; i!=e; ++i) {
		(*i)->update_direction_flags();
	}
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

	bool
	operator () (const bool b, const const_alias_ptr_type a) const {
		return b || a->is_port_alias();
		// should short-circuit
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
port_alias_tracker_base<Tag>::dump_map(ostream& o, const dump_flags& df) const {
	STACKTRACE_VERBOSE;
if (!_ids.empty()) {
	o << auto_indent << class_traits<Tag>::tag_name
		<< " port aliases:" << endl;
	// this could be "scope aliases" too...
	const_iterator i(_ids.begin());
	const const_iterator e(_ids.end());
	for ( ; i!=e; ++i) {
		i->second.dump(o << auto_indent << i->first << ": ", df)
			<< endl;
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
	for ( ; i!=e; ++i) {
		if (!i->second.replay_internal_aliases(s).good)
			return good_bool(false);
	}
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Tag>
void
port_alias_tracker_base<Tag>::__export_alias_properties(
		substructure_alias& s) const {
	STACKTRACE_VERBOSE;
	const_iterator i(_ids.begin());
	const const_iterator e(_ids.end());
	for ( ; i!=e; ++i) {
		i->second.export_alias_properties(s);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if alias is aliased to any public port (reachable).
 */
template <class Tag>
struct port_alias_tracker_base<Tag>::port_alias_predicate {
	typedef	typename map_type::mapped_type	mapped_type;
	typedef	mapped_type		argument_type;
	bool
	operator () (const mapped_type& a) const {
		return a.is_aliased_to_port();
	}

	typedef	typename map_type::value_type	value_type;
	bool
	operator () (const value_type& a) const {
		return a.second.is_aliased_to_port();
	}
};

/**
	Want to use the std::stable_partition algorithm, except that we need to
	actually modify the key (instance ID index).
 */
template <class Tag>
void
port_alias_tracker_base<Tag>::__sift_ports(void) {
	STACKTRACE_VERBOSE;
	typedef typename map_type::mapped_type	mapped_type;
	typedef	typename vector<mapped_type>::iterator	temp_iterator;
	const iterator fb(_ids.begin()), fe(_ids.end());
	iterator fi(fb);
#if 0
	// slow-but-stable partitioning, relative order preserved
	vector<mapped_type> tmp;
//	STACKTRACE_INDENT_PRINT("_ids.size() = " << _ids.size() << endl);
#if 0
	tmp.reserve(_ids.size());	// pre-allocate
	transform(fi, fe, back_inserter(tmp), std::_Select2nd<value_type>());
	const temp_iterator tb(tmp.begin()), te(tmp.end());
#else
	// must avoid expensive copy to save memory
	tmp.resize(_ids.size());	// default construct
	const temp_iterator tb(tmp.begin()), te(tmp.end());
	temp_iterator ti(tb);
	for ( ; fi!=fe; ++fi, ++ti) {
		// swap instead of copy to conserve memory
		fi->second.bare_swap(*ti);
	}
#endif
	std::stable_partition(tb, te, port_alias_predicate());
	// throws std::bad_alloc for sufficiently large arrays
	// or use std::mem_fun_ref(&mapped_type::is_aliased_to_port)
	for (fi = fb, ti = tb; fi!=fe; ++fi, ++ti) {
		fi->second.bare_swap(*ti);
		fi->second.override_id(fi->first);
	}
	cerr << "after array-swap" << endl;
#elif 1
	// manual stable-partition, less elegant, but memory efficient
	vector<mapped_type> p, np;
	// don't reserve, don't know breakdown a priori
	for ( ; fi!=fe; ++fi) {
		// move instead of copy
		if (fi->second.is_aliased_to_port()) {
			p.push_back(mapped_type());
			fi->second.bare_swap(p.back());
		} else {
			np.push_back(mapped_type());
			fi->second.bare_swap(np.back());
		}
	}
	_ids.clear();	// wipe and reconstruct
	size_t j = 1;
	temp_iterator ti(p.begin()), te(p.end());
	for ( ; ti!=te; ++ti, ++j) {
		mapped_type& m(_ids[j]);
		m.bare_swap(*ti);	// move instead of copy
		m.override_id(j);
	}
	ti = np.begin();
	te = np.end();
	for ( ; ti!=te; ++ti, ++j) {
		mapped_type& m(_ids[j]);
		m.bare_swap(*ti);	// move instead of copy
		m.override_id(j);
	}
#else
	// fast-but-unstable partitioning, relative order not preserved
	// this does not preserve relative order, hence 'unstable'
	typedef	typename map_type::reverse_iterator	reverse_iterator;
	reverse_iterator ri(_ids.rbegin());
	const reverse_iterator re(_ids.rend());
	fi = find_if(fi, fe, not1(port_alias_predicate()));	// forward seek
	ri = find_if(ri, re, port_alias_predicate());		// reverse seek
	while (fi != fe && ri != re && fi->first < ri->first) {
		// swap-eroo
		fi->second.swap(ri->second);
		fi = find_if(++fi, fe, not1(port_alias_predicate()));
		ri = find_if(++ri, re, port_alias_predicate());
	}
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For the complete set of scope aliases, this returns the index offset 
	of the first non-port alias.
 */
template <class Tag>
size_t
port_alias_tracker_base<Tag>::__port_offset(void) const {
	const const_iterator b(_ids.begin()), e(_ids.end());
#if 0
	// faster than find_first
	const const_iterator p(lower_bound(b, e,
		not1(port_alias_predicate())));
	if (p != e) {
		return p->second;
	} else {
		return _ids.size();
	}
#else
	return std::count_if(b, e, port_alias_predicate());
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param a is an alias of an enclosing substructure (process)
		representing the actual instance (context).  
 */
template <class Tag>
void
port_alias_tracker_base<Tag>::__assign_frame(
		const substructure_alias& p, 
		footprint_frame& ff) const {
	STACKTRACE_VERBOSE;
	footprint_frame_map_type& fm(ff.template get_frame_map<Tag>());
	const_iterator i(_ids.begin()), e(_ids.end());
	for ( ; i!=e; ++i) {
		const typename alias_set_type::alias_ptr_type
			ap(i->second.is_aliased_to_port());
		// now returns a direct port alias
	if (ap) {
		const size_t formal_index = i->first;
		INVARIANT(formal_index);
		const instance_alias_info<Tag>& f(*ap);
#if ENABLE_STACKTRACE
		f.dump_hierarchical_name(STACKTRACE_STREAM << "f: ") << endl;
#endif
		const instance_alias_info<Tag>& a(f.trace_alias(p));
		// assign actual index from context to 
		// corresponding slot in frame
		const size_t actual_index = a.instance_index;
		INVARIANT(actual_index);
		INVARIANT(formal_index <= fm.size());
		fm[formal_index -1] = actual_index;
	}
	}
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
//	STACKTRACE_INDENT_PRINT("p.size() = " << p.size() << endl);
	iterator i(_ids.begin());
	const iterator e(_ids.end());
	for ( ; i!=e; ++i) {
		typedef	typename alias_reference_set<Tag>::const_alias_ptr_type
						const_alias_ptr_type;
		const const_alias_ptr_type al(i->second.shortest_alias());
		INVARIANT(i->first);	// non-zero
		STACKTRACE_INDENT_PRINT("i->first = " << i->first << endl);
		BOUNDS_CHECK(i->first <= p.local_entries());
		p[i->first -1].set_back_ref(al);
		// 1-based to 0-based index, no dummy instance anymore
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Makes all flags/attributes consistent among aliases within each set.
 */
template <class Tag>
void
port_alias_tracker_base<Tag>::__synchronize_flags(void) {
	iterator i(_ids.begin());
	const iterator e(_ids.end());
	for ( ; i!=e; ++i) {
		i->second.synchronize_flags();
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This is only ever instantiated for channels and bools.
	Walks over all unique channel instances in scope, 
	and checks for dangling connections.  
 */
template <class Tag>
error_count
port_alias_tracker_base<Tag>::check_connections(void) const {
	STACKTRACE_VERBOSE;
	error_count ret;
	const_iterator i(_ids.begin()), e(_ids.end());
	for ( ; i!=e; ++i) {
		// grab the canonical alias from each set.
		INVARIANT(i->second.size());
		ret += i->second.front()->find()->check_connection();
		// already have diagnostic message
	}
	return ret;
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
/**
	Re-enumerates the alias indices so that aliases that are
	public ports have lower indices than those that are locally private.
	This is needed for efficient memory mapping.  
 */
void
port_alias_tracker::sift_ports(void) {
	STACKTRACE_VERBOSE;
	port_alias_tracker_base<process_tag>::__sift_ports();
	port_alias_tracker_base<channel_tag>::__sift_ports();
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::__sift_ports();
#endif
	port_alias_tracker_base<enum_tag>::__sift_ports();
	port_alias_tracker_base<int_tag>::__sift_ports();
	port_alias_tracker_base<bool_tag>::__sift_ports();
	// watch for std::bad_alloc, especially when calling stable_partition
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::export_alias_properties(substructure_alias& s) const {
	STACKTRACE_VERBOSE;
	port_alias_tracker_base<process_tag>::__export_alias_properties(s);
	port_alias_tracker_base<channel_tag>::__export_alias_properties(s);
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::__export_alias_properties(s);
#endif
	port_alias_tracker_base<enum_tag>::__export_alias_properties(s);
	port_alias_tracker_base<int_tag>::__export_alias_properties(s);
	port_alias_tracker_base<bool_tag>::__export_alias_properties(s);
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
void
port_alias_tracker::synchronize_flags(void) {
	STACKTRACE_VERBOSE;
if (has_internal_aliases) {
	port_alias_tracker_base<process_tag>::__synchronize_flags();
	port_alias_tracker_base<channel_tag>::__synchronize_flags();
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::__synchronize_flags();
#endif
	port_alias_tracker_base<enum_tag>::__synchronize_flags();
	port_alias_tracker_base<int_tag>::__synchronize_flags();
	port_alias_tracker_base<bool_tag>::__synchronize_flags();
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
error_count
port_alias_tracker::check_bool_connections(void) const {
	return port_alias_tracker_base<bool_tag>::check_connections();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
error_count
port_alias_tracker::check_process_connections(void) const {
	return port_alias_tracker_base<process_tag>::check_connections();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Only need to check channels, other meta-types don't have directions.  
	TODO: other things will need to be checked, like relaxed actual
		type completion.  
 */
error_count
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
	const dump_flags& df(dump_flags::default_value);
	port_alias_tracker_base<process_tag>::dump_map(o, df);
	port_alias_tracker_base<channel_tag>::dump_map(o, df);
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::dump_map(o, df);
#endif
	port_alias_tracker_base<enum_tag>::dump_map(o, df);
	port_alias_tracker_base<int_tag>::dump_map(o, df);
	port_alias_tracker_base<bool_tag>::dump_map(o, df);
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
port_alias_tracker::dump_local_bool_aliases(ostream& o) const {
	return port_alias_tracker_base<bool_tag>::dump_map(o,
		dump_flags::no_owners);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_alias_tracker::assign_alias_frame(const substructure_alias& a, 
		footprint_frame& ff) const {
	port_alias_tracker_base<process_tag>::__assign_frame(a, ff);
	port_alias_tracker_base<channel_tag>::__assign_frame(a, ff);
#if ENABLE_DATASTRUCTS
	port_alias_tracker_base<datastruct_tag>::__assign_frame(a, ff);
#endif
	port_alias_tracker_base<enum_tag>::__assign_frame(a, ff);
	port_alias_tracker_base<int_tag>::__assign_frame(a, ff);
	port_alias_tracker_base<bool_tag>::__assign_frame(a, ff);
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

#if 0
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
#else
template class alias_reference_set<process_tag>;
template class port_alias_tracker_base<process_tag>;
template class alias_reference_set<channel_tag>;
template class port_alias_tracker_base<channel_tag>;
#if ENABLE_DATASTRUCTS
template class alias_reference_set<datastruct_tag>;
template class port_alias_tracker_base<datastruct_tag>;
#endif
template class alias_reference_set<enum_tag>;
template class port_alias_tracker_base<enum_tag>;
template class alias_reference_set<int_tag>;
template class port_alias_tracker_base<int_tag>;
template class alias_reference_set<bool_tag>;
template class port_alias_tracker_base<bool_tag>;
#endif

#if USE_ALIAS_STRING_CACHE
template void alias_reference_set<process_tag>::refresh_string_cache() const;
template void alias_reference_set<bool_tag>::refresh_string_cache() const;
#endif

#undef	INSTANTIATE_ALIAS_REFERENCE_SET_PUSH_BACK

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

