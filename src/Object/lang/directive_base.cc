/**
	\file "Object/lang/directive_base.cc"
	$Id: directive_base.cc,v 1.11 2011/02/02 23:54:22 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <algorithm>
#include <set>
#include <functional>

#include "Object/lang/directive_base.hh"
#include "Object/expr/const_param.hh"
#if PRIVATE_MEMBER_REFERENCES
#include "Object/def/footprint.hh"
#endif
#include "Object/common/dump_flags.hh"
#include "Object/inst/instance_pool.hh"
#include "Object/inst/bool_instance.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/traits/bool_traits.hh"
#include "Object/traits/proc_traits.hh"

#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.hh"
#include "util/IO_utils.tcc"

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::write_value;
using util::read_value;
using std::find;
using std::find_if;
using std::distance;
using std::mem_fun_ref;

//=============================================================================
// class SPEC::directive_base method definitions

directive_base::directive_base() : name(), params() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive_base::directive_base(const string& k) :
		name(k), params() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive_base::~directive_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 1-indexed offset of first error if found, else 0.  
 */
size_t
directive_base::first_param_error(void) const {
	const size_t s = params.size();
	if (s) {
		typedef	params_type::const_iterator	const_iterator;
		typedef	params_type::value_type		value_type;
		const const_iterator i(params.begin()), e(params.end());
		const const_iterator z(find(i, e, value_type(0)));
		if (z != e) {
			const size_t d = distance(i, z) +1;
			cerr << "Error resolving expression " << d <<
				"." << endl;
			return d;
		}
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 1-indexed offset of first error if found, else 0.  
 */
size_t
generic_directive_base::first_node_error(void) const {
	const size_t s = nodes.size();
	if (s) {
		typedef	nodes_type::const_iterator	const_iterator;
		typedef	nodes_type::value_type		value_type;
		const const_iterator i(nodes.begin()), e(nodes.end());
		// when an error occurs, we empty the group/set
		const const_iterator z(find_if(i, e,
			mem_fun_ref(&value_type::empty)));
		if (z != e) {
			const size_t d = distance(i, z) +1;
			cerr << "Error resolving literal " << d <<
				"." << endl;
			return d;
		}
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directive_base::dump_params_bare(const params_type& p, ostream& o) {
if (p.size()) {
	typedef	params_type::const_iterator	const_iterator;
	const_iterator i(p.begin());
	const const_iterator e(p.end());
	NEVER_NULL(*i);
	(*i)->dump(o);
	for (++i; i!=e; ++i) {
		NEVER_NULL(*i);
		(*i)->dump(o << ',');
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directive_base::dump_params(const params_type& p, ostream& o) {
if (p.size()) {
	o << '<';
	dump_params_bare(p, o);
	o << '>';
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directive_base::dump_params(ostream& o) const {
	return dump_params(params, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This variant is limited to locally publicly accessible instances.
 */
template <class PTag>
ostream&
generic_directive_base::dump_group(const directive_node_group_type& g,
		ostream& o, const PTag& np) {
	const size_t lim = np.local_entries();
	const dump_flags& df(dump_flags::no_definition_owner);
	if (g.size() > 1) {
		typedef directive_node_group_type::const_iterator
							const_iterator;
		const_iterator i(g.begin());
		const const_iterator e(g.end());
		o << '{';
		INVARIANT(*i <= lim);
		size_t ni = *i -1;	// node-pool is 0-indexed
		np[ni].get_back_ref()->dump_hierarchical_name(o, df);
		for (++i; i!=e; ++i) {
			INVARIANT(*i <= lim);
			ni = *i -1;	// node-pool is 0-indexed
			np[ni].get_back_ref()->dump_hierarchical_name(
				o << ',', df);
		}
		o << '}';
	} else if (g.size() == 1) {
		const size_t nip1 = *g.begin();
		INVARIANT(nip1 <= lim);
		const size_t ni = nip1 -1;	// pool is 0-indexed
		np[ni].get_back_ref()->dump_hierarchical_name(o, df);
	} else {
		// during debugging, during reference resolution, 
		// might print a spec directive before its references 
		// have been resolved.
		o << '?';
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRIVATE_MEMBER_REFERENCES
/**
	This variant works for deep subinstance references.
 */
template <class Tag>
ostream&
generic_directive_base::dump_group(const directive_node_group_type& g,
		ostream& o, const footprint& f) {
	const dump_flags& df(dump_flags::no_definition_owner);
	const bool is_top = true;	// ?
#if 0
	o << '[';
	copy(g.begin(), g.end(), std::ostream_iterator<size_t>(o, ","));
	o << ']';
#endif
	if (g.size() > 1) {
		typedef directive_node_group_type::const_iterator
							const_iterator;
		const_iterator i(g.begin());
		const const_iterator e(g.end());
		o << '{';
		size_t ni = *i -1;	// node-pool is 0-indexed
		f.template dump_canonical_name<Tag>(o, ni, df, is_top);
		for (++i; i!=e; ++i) {
			ni = *i -1;	// node-pool is 0-indexed
			f.template dump_canonical_name<Tag>(
				o << ',', ni, df, is_top);
		}
		o << '}';
	} else if (g.size() == 1) {
		const size_t nip1 = *g.begin();
		const size_t ni = nip1 -1;	// pool is 0-indexed
		f.template dump_canonical_name<Tag>(o, ni, df, is_top);
	} else {
		// during debugging, during reference resolution, 
		// might print a spec directive before its references 
		// have been resolved.
		o << '?';
	}
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class PTag>
ostream&
generic_directive_base::dump_groups(ostream& o, const PTag& np) const {
{
	typedef nodes_type::const_iterator const_iterator;
	const_iterator i(nodes.begin());
	const const_iterator e(nodes.end());
if (i!=e) {
	dump_group(*i, o, np);
	for (++i; i!=e; ++i) {
		dump_group(*i, o << ',', np);
	}
}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRIVATE_MEMBER_REFERENCES
template <class Tag>
ostream&
generic_directive_base::dump_groups(ostream& o, const footprint& f) const {
{
	typedef nodes_type::const_iterator const_iterator;
	const_iterator i(nodes.begin());
	const const_iterator e(nodes.end());
if (i!=e) {
	dump_group<Tag>(*i, o, f);
	for (++i; i!=e; ++i) {
		dump_group<Tag>(*i, o << ',', f);
	}
}
}
	return o;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// explicit template instantiations
template 
ostream&
generic_directive_base::dump_groups(ostream&,
	const instance_pool<bool_instance>&) const;

template 
ostream&
generic_directive_base::dump_groups(ostream&,
	const instance_pool<process_instance>&) const;

#if PRIVATE_MEMBER_REFERENCES
template 
ostream&
generic_directive_base::dump_groups<bool_tag>(
	ostream&, const footprint&) const;

template 
ostream&
generic_directive_base::dump_groups<process_tag>(
	ostream&, const footprint&) const;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(params);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_base::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	INVARIANT(name.length());
	write_value(o, name);
	m.write_pointer_list(o, params);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	read_value(i, name);
	INVARIANT(name.length());
	m.read_pointer_list(i, params);
}

//-----------------------------------------------------------------------------
generic_directive_base::generic_directive_base() : directive_base(), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
generic_directive_base::generic_directive_base(const string& k) :
		directive_base(k), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
generic_directive_base::~generic_directive_base() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_directive_base::collect_transient_info_base(
		persistent_object_manager& m) const {
	directive_base::collect_transient_info_base(m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_directive_base::write_object_base(const persistent_object_manager& m, 
		ostream& o) const {
	directive_base::write_object_base(m, o);
{
	write_value(o, nodes.size());
	typedef	nodes_type::const_iterator	const_iterator;
	const_iterator i(nodes.begin()), e(nodes.end());
	for ( ; i!=e; ++i) {
		util::write_sequence(o, *i);	// works for sets
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
generic_directive_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	directive_base::load_object_base(m, i);
{
	size_t s;
	read_value(i, s);
	nodes.resize(s);
	typedef	nodes_type::iterator	iterator;
	iterator j(nodes.begin());
	for ( ; s; --s, ++j) {
		util::read_sequence_set_insert(i, *j);
	}
}
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

