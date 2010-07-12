/**
	\file "Object/lang/directive_base.cc"
	$Id: directive_base.cc,v 1.7 2010/07/12 17:47:00 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <algorithm>
#include <set>
#include <functional>

#include "Object/lang/directive_base.h"
#include "Object/expr/const_param.h"
#include "Object/common/dump_flags.h"
#include "Object/inst/instance_pool.h"
#include "Object/inst/bool_instance.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"

#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#include "util/IO_utils.tcc"

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
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
ostream&
generic_directive_base::dump_node_group(const directive_node_group_type& g,
		ostream& o, const node_pool_type& np) {
	if (g.size() > 1) {
		typedef directive_node_group_type::const_iterator
							const_iterator;
		const_iterator i(g.begin());
		const const_iterator e(g.end());
		o << '{';
		size_t ni = *i -1;	// node-pool is 0-indexed
		np[ni].get_back_ref()->dump_hierarchical_name(o,
			dump_flags::no_definition_owner);
		for (++i; i!=e; ++i) {
			ni = *i -1;	// node-pool is 0-indexed
			np[ni].get_back_ref()->dump_hierarchical_name(
				o << ',', dump_flags::no_definition_owner);
		}
		o << '}';
	} else {
		INVARIANT(g.size() == 1);
		const size_t ni = *g.begin() -1;	// pool is 0-indexed
		np[ni].get_back_ref()->dump_hierarchical_name(o,
			dump_flags::no_definition_owner);
	}
	return o;
}

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

