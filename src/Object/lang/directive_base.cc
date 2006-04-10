/**
	\file "Object/lang/directive_base.cc"
	$Id: directive_base.cc,v 1.2.16.1 2006/04/10 23:21:31 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <algorithm>
#include "Object/lang/directive_base.h"
#include "Object/expr/const_param.h"
#include "Object/common/dump_flags.h"
#include "Object/inst/instance_pool.h"
#include "Object/inst/bool_instance.h"
#include "Object/inst/instance_alias_info.h"
#include "Object/inst/alias_empty.h"
#include "util/IO_utils.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"
#if GROUPED_DIRECTIVE_ARGUMENTS
#include <set>
#include <functional>
#include "util/IO_utils.tcc"
#endif

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

directive_base::directive_base() : name(), params(), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive_base::directive_base(const string& k) :
		name(k), params(), nodes() { }

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
directive_base::first_node_error(void) const {
	const size_t s = nodes.size();
	if (s) {
		typedef	nodes_type::const_iterator	const_iterator;
		typedef	nodes_type::value_type		value_type;
		const const_iterator i(nodes.begin()), e(nodes.end());
#if GROUPED_DIRECTIVE_ARGUMENTS
		// when an error occurs, we empty the group/set
		const const_iterator z(find_if(i, e,
			mem_fun_ref(&value_type::empty)));
#else
		const const_iterator z(find(i, e, value_type(NULL)));
#endif
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
directive_base::dump_params(const params_type& p, ostream& o) {
if (p.size()) {
	typedef	params_type::const_iterator	const_iterator;
	const_iterator i(p.begin());
	const const_iterator e(p.end());
	o << '<';
	NEVER_NULL(*i);
	(*i)->dump(o);
	for (++i; i!=e; ++i) {
		NEVER_NULL(*i);
		(*i)->dump(o << ',');
	}
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
#if GROUPED_DIRECTIVE_ARGUMENTS
ostream&
directive_base::dump_node_group(const directive_node_group_type& g,
		ostream& o, const node_pool_type& np) {
	if (g.size() > 1) {
		typedef directive_node_group_type::const_iterator
							const_iterator;
		const_iterator i(g.begin());
		const const_iterator e(g.end());
		o << '{';
		np[*i].get_back_ref()->dump_hierarchical_name(o,
			dump_flags::no_definition_owner);
		for (++i; i!=e; ++i) {
			np[*i].get_back_ref()->dump_hierarchical_name(
				o << ',', dump_flags::no_definition_owner);
		}
		o << '}';
	} else {
		INVARIANT(g.size() == 1);
		np[*g.begin()].get_back_ref()->dump_hierarchical_name(o,
			dump_flags::no_definition_owner);
	}
	return o;
}
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
#if GROUPED_DIRECTIVE_ARGUMENTS
{
	write_value(o, nodes.size());
	typedef	nodes_type::const_iterator	const_iterator;
	const_iterator i(nodes.begin()), e(nodes.end());
	for ( ; i!=e; ++i) {
		util::write_sequence(o, *i);	// works for sets
	}
}
#else
	util::write_sequence(o, nodes);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_base::load_object_base(const persistent_object_manager& m, 
		istream& i) {
	read_value(i, name);
	INVARIANT(name.length());
	m.read_pointer_list(i, params);
#if GROUPED_DIRECTIVE_ARGUMENTS
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
#else
	util::read_sequence_resize(i, nodes);
#endif
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

