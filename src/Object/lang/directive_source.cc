/**
	\file "Object/lang/directive_source.cc"
	$Id: directive_source.cc,v 1.11 2011/02/02 23:54:22 fang Exp $
 */

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <algorithm>
#include <iterator>
#include <set>
#include <functional>

#include "Object/lang/directive_source.h"
#include "Object/lang/PRS_literal_unroller.h"	// for PRS::literal
#include "Object/expr/param_expr_functor.h"
#include "Object/expr/const_param.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/persistent_type_hash.h"
#include "Object/traits/bool_traits.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"

#include "common/TODO.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/reserve.h"
#include "util/indent.h"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
using std::for_each;
using std::back_inserter;
using std::transform;
using util::auto_indent;
#include "util/using_ostream.h"
using PRS::rule_dump_context;
using util::read_value;
using util::write_value;
using std::distance;
using std::find;
using std::find_if;
using std::mem_fun_ref;

//=============================================================================
// class directive_source_common method definitions

directive_source_common::directive_source_common() :
		name(), params() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive_source_common::directive_source_common(const string& n) :
		name(n), params() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive_source_common::~directive_source_common() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped off of PRS::macro::unroll().
	\return 1-indexed index of first error, else 0 to signal no error. 
 */
size_t
directive_source_common::unroll_params(const params_type& s,
	const unroll_context& c, 
		unrolled_params_type& p) {
	STACKTRACE_VERBOSE;
	transform(s.begin(), s.end(),
		back_inserter(p), param_expr::unroller(c));
	typedef	unrolled_params_type::const_iterator	const_iterator;
	typedef	unrolled_params_type::value_type	value_type;
	// look for error
	const const_iterator b(p.begin()), e(p.end());
	const const_iterator n(find(b, e, value_type(NULL)));
	if (n != e)
		return distance(b, n) +1;
	else	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
directive_source_common::unroll_params(const unroll_context& c, 
		unrolled_params_type& p) const {
	return unroll_params(params, c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints without enclosing angle-brackets.
 */
ostream&
directive_source_common::dump_params_bare(const params_type& params,
		ostream& o, 
		const expr_dump_context& edc) {
	typedef params_type::const_iterator	const_iterator;
	const_iterator i(params.begin());
	const const_iterator e(params.end());
if (i!=e) {
	(*i)->dump(o, edc);
	for (++i; i!=e; ++i) {
		(*i)->dump(o << ',', edc);
	}
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints with enclosing angle brackets.
 */
ostream&
directive_source_common::dump_params(const params_type& params, ostream& o, 
		const expr_dump_context& edc) {
if (!params.empty()) {
	o << '<';
	dump_params_bare(params, o, edc);
	o << '>';
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects pointers.  
 */
void
directive_source_common::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(params);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use string dictionary to save space.  
 */
void
directive_source_common::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	write_value(o, name);
	m.write_pointer_list(o, params);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_source_common::load_object_base(const persistent_object_manager& m,
		istream& i) {
	read_value(i, name);
	m.read_pointer_list(i, params);
}

//=============================================================================
// helper class definitions

/**
	ostream binding functor.  
 */
template <class L>
struct directive_source<L>::dumper {
	ostream&				os;
	const rule_dump_context&		rdc;

	dumper(ostream& o, const rule_dump_context& r) : os(o), rdc(r) { }

	template <class P>
	void
	operator () (const P& p) {
		NEVER_NULL(p);
		p->dump(os << auto_indent, rdc) << endl;
	}
};	// end struct directive_source<L>::dumper

//=============================================================================
// class directive method definitions

template <class L>
directive_source<L>::directive_source() :
		directive_source_common(), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class L>
directive_source<L>::directive_source(const string& n) :
		directive_source_common(n), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class L>
directive_source<L>::~directive_source() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class L>
ostream&
directive_source<L>::dump_group(const typename nodes_type::value_type& n,
		ostream& o, 
		const PRS::rule_dump_context& c) {
if (n.size() > 1) {
	typedef typename nodes_type::value_type::const_iterator	const_iterator;
	const_iterator i(n.begin());
	const const_iterator e(n.end());
	i->dump(o << '{', c);
	for (++i; i!=e; ++i) {
		i->dump(o << ',', c);
	}
	o << '}';
} else {
	INVARIANT(n.size());
	n.begin()->dump(o, c);
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class L>
ostream&
directive_source<L>::dump_nodes(ostream& o,
		const PRS::rule_dump_context& c) const {
	o << '(';
	typedef typename nodes_type::const_iterator	const_iterator;
if (nodes.size()) {
	const_iterator i(nodes.begin());
	const const_iterator e(nodes.end());
	dump_group(*i, o, c);
	for (++i; i!=e; ++i) {
		dump_group(*i, o << ',', c);
	}
}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off PRS::macro::dump().
	TODO: params
 */
template <class L>
ostream&
directive_source<L>::dump(ostream& o, const PRS::rule_dump_context& c) const {
	o << name;
	const entity::expr_dump_context edc(c);
	dump_params(params, o, edc);
	return dump_nodes(o, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped off of PRS::macro::unroll().
	\return 1-indexed index of first error, else 0 to signal no error. 
 */
template <class L>
size_t
directive_source<L>::unroll_nodes(const nodes_type& s, const unroll_context& c, 
		unrolled_nodes_type& p) {
	STACKTRACE_VERBOSE;
#if 0
	transform(s.begin(), s.end(),
		back_inserter(p), literal_type::unroller(c));
#else
{
	// not a clean transform algorithm
	// TODO: cleaner error handling
	typedef	typename nodes_type::const_iterator	source_iterator;
	typedef	unrolled_nodes_type::iterator		dest_iterator;
	source_iterator si(s.begin()), se(s.end());
	p.resize(s.size());
	dest_iterator di(p.begin());
	for ( ; si!=se; ++si, ++di) {
		typedef	typename nodes_type::value_type::const_iterator
						inner_source_iterator;
		inner_source_iterator ii(si->begin()), ie(si->end());
		typename literal_type::group_type temp;
		for ( ; ii!=ie; ++ii) {
			if (!ii->unroll_group(c, temp).good) {
				return distance(s.begin(), si) +1;
			}
		}
		INVARIANT(temp.size());
		// temp is an array of node IDs which may not be unique
		// we keep only unique ones.  
		// inserter iterator not quite appropriate...
		// a set_inserter would be ideal
		typename literal_type::group_type::const_iterator
			ti(temp.begin()), te(temp.end());
		for ( ; ti!=te; ++ti) {
			di->insert(*ti);
			// don't bother warning about ignored duplicates
		}
		INVARIANT(di->size());
	}
}
#endif
	typedef	unrolled_nodes_type::const_iterator	const_iterator;
	typedef	unrolled_nodes_type::value_type	value_type;
	// look for error
	const const_iterator b(p.begin()), e(p.end());
	const const_iterator n(find_if(b, e, mem_fun_ref(&value_type::empty)));
	if (n != e)
		return distance(b, n) +1;
	else	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped off of PRS::macro::unroll().
 */
template <class L>
size_t
directive_source<L>::unroll_nodes(const unroll_context& c, 
		unrolled_nodes_type& n) const {
	return unroll_nodes(nodes, c, n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects pointers.  
 */
template <class L>
void
directive_source<L>::collect_transient_info_base(
		persistent_object_manager& m) const {
	directive_source_common::collect_transient_info_base(m);
	for_each(nodes.begin(), nodes.end(),
		util::persistent_sequence_collector_ref(m)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use string dictionary to save space.  
 */
template <class L>
void
directive_source<L>::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	directive_source_common::write_object_base(m, o);
{
	typedef	typename nodes_type::const_iterator	const_iterator;
	write_value(o, nodes.size());
	const_iterator i(nodes.begin()), e(nodes.end());
	for ( ; i!=e; ++i) {
		util::write_persistent_sequence(m, o, *i);
	}
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class L>
void
directive_source<L>::load_object_base(const persistent_object_manager& m,
		istream& i) {
	directive_source_common::load_object_base(m, i);
{
	size_t s;
	read_value(i, s);
	util::reserve(nodes, s);
	for ( ; s; --s) {
		this->nodes.push_back(typename nodes_type::value_type());
		util::read_persistent_sequence_resize(m, i, nodes.back());
	}
}
}

//=============================================================================
// explicit template instantiations
template class directive_source<bool_literal>;
template class directive_source<proc_literal>;

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

