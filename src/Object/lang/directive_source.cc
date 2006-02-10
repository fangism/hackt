/**
	\file "Object/lang/directive_source.cc"
	$Id: directive_source.cc,v 1.1.2.2 2006/02/10 08:09:52 fang Exp $
 */

#include <iostream>
#include <algorithm>
#include <iterator>
#include "Object/lang/directive_source.h"
#include "Object/lang/PRS_literal_unroller.h"	// for PRS::literal
#include "Object/expr/param_expr.h"
#include "Object/expr/const_param.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/persistent_type_hash.h"
#include "Object/traits/bool_traits.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
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

//=============================================================================
// helper class definitions

/**
	ostream binding functor.  
 */
struct directive_source::dumper {
	ostream&				os;
	const rule_dump_context&		rdc;

	dumper(ostream& o, const rule_dump_context& r) : os(o), rdc(r) { }

	template <class P>
	void
	operator () (const P& p) {
		NEVER_NULL(p);
		p->dump(os << auto_indent, rdc) << endl;
	}
};	// end struct directive_source::dumper

//=============================================================================
// class directive method definitions

directive_source::directive_source() :
		name(), params(), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive_source::directive_source(const string& n) :
		name(n), params(), nodes() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
directive_source::~directive_source() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
directive_source::dump_params(const params_type& params, ostream& o, 
		const expr_dump_context& edc) {
if (!params.empty()) {
	typedef params_type::const_iterator	const_iterator;
	o << '<';
	INVARIANT(params.size());
	const_iterator i(params.begin());
	const const_iterator e(params.end());
	(*i)->dump(o, edc);
	for (++i; i!=e; ++i) {
		(*i)->dump(o << ',', edc);
	}
	o << '>';
}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Ripped off PRS::macro::dump().
	TODO: params
 */
ostream&
directive_source::dump(ostream& o, const PRS::rule_dump_context& c) const {
	o << name;
	o << '(';
{
	const entity::expr_dump_context edc(c);
	dump_params(params, o, edc);
}
{
	typedef nodes_type::const_iterator	const_iterator;
	INVARIANT(nodes.size());
	const_iterator i(nodes.begin());
	const const_iterator e(nodes.end());
	i->dump(o, c);
	for (++i; i!=e; ++i) {
		i->dump(o << ',', c);
	}
}
	return o << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped off of PRS::macro::unroll().
	\return 1-indexed index of first error, else 0 to signal no error. 
 */
size_t
directive_source::unroll_params(const params_type& s, const unroll_context& c, 
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
directive_source::unroll_params(const unroll_context& c, 
		unrolled_params_type& p) const {
	return unroll_params(params, c, p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped off of PRS::macro::unroll().
	\return 1-indexed index of first error, else 0 to signal no error. 
 */
size_t
directive_source::unroll_nodes(const nodes_type& s, const unroll_context& c, 
		unrolled_nodes_type& p) {
	STACKTRACE_VERBOSE;
	transform(s.begin(), s.end(),
		back_inserter(p), bool_literal::unroller(c));
	typedef	unrolled_nodes_type::const_iterator	const_iterator;
	typedef	unrolled_nodes_type::value_type	value_type;
	// look for error
	const const_iterator b(p.begin()), e(p.end());
	const const_iterator n(find(b, e, value_type(NULL)));
	if (n != e)
		return distance(b, n) +1;
	else	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implementation ripped off of PRS::macro::unroll().
 */
size_t
directive_source::unroll_nodes(const unroll_context& c, 
		unrolled_nodes_type& n) const {
	return unroll_nodes(nodes, c, n);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Collects pointers.  
 */
void
directive_source::collect_transient_info_base(
		persistent_object_manager& m) const {
	m.collect_pointer_list(params);
	for_each(nodes.begin(), nodes.end(),
		util::persistent_collector_ref(m)
	);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_source::write_object_base(const persistent_object_manager& m,
		ostream& o) const {
	write_value(o, name);
	m.write_pointer_list(o, params);
	util::write_persistent_sequence(m, o, nodes);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
directive_source::load_object_base(const persistent_object_manager& m,
		istream& i) {
	read_value(i, name);
	m.read_pointer_list(i, params);
	util::read_persistent_sequence_resize(m, i, nodes);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

