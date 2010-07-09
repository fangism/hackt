/**
	\file "Object/unroll/meta_conditional.tcc"
	Helper functions for repetitive conditional constructs.  
	$Id: meta_conditional.tcc,v 1.4 2010/07/09 02:14:14 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_META_CONDITIONAL_TCC__
#define	__HAC_OBJECT_UNROLL_META_CONDITIONAL_TCC__

#include <iostream>
#include <algorithm>
#include "Object/unroll/meta_conditional.h"
#include "Object/expr/pbool_const.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/unroll/unroll_context.h"
#include "Object/common/dump_flags.h"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/indent.h"
#include "util/stacktrace.h"
#include "util/IO_utils.h"

namespace HAC {
namespace entity {
using std::for_each;
using util::auto_indent;
#include "util/using_ostream.h"
using PRS::rule_dump_context;
using util::read_value;
using util::write_value;

//=============================================================================
// class directives_conditional method definitions
// mostly ripped from "Object/lang/PRS.cc":rule_conditional

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if ALL clauses are empty.  
 */
template <class T>
bool
meta_conditional<T>::empty(const T& mc) {
	clause_iterator ci(mc.clauses.begin()), ce(mc.clauses.end());
	for ( ; ci!=ce; ++ci) {
		if (!ci->empty())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
template <class C>
ostream&
meta_conditional<T>::dump(const T& mc, ostream& o, const C& c, 
		ostream& (T::clause_list_type::value_type::*dumper)(ostream&, const C&) const) {
	INVARIANT(mc.guards.size());
	INVARIANT(mc.guards.size() == mc.clauses.size());
	clause_iterator ci(mc.clauses.begin()), ce(mc.clauses.end());
	guard_iterator gi(mc.guards_begin()), ge(mc.guards_end());
	const entity::expr_dump_context edc(c);
	NEVER_NULL(*gi);
	(*gi)->dump(o << "[ ", edc) << " ->" << endl;
	{
		INDENT_SECTION(o);
		(*ci.*dumper)(o, c);
	}
	for (++gi, ++ci; ci!=ce; ++gi, ++ci) {
		o << auto_indent << "[] ";
		if (*gi) {
			(*gi)->dump(o, edc);
		} else {
			o << "else";
		}
		o << " ->" << endl;
		INDENT_SECTION(o);
		(*ci.*dumper)(o, c);
	}
	return o << auto_indent << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unrolls the directives in the body of guard evaluates true.  
	\param F the footprint type (unroll target)
	\param str the string associated with the host type for diagnostics.  
 */
template <class T>
good_bool
meta_conditional<T>::unroll(const T& mc,
		const unroll_context& c,
		const char* str) {
	clause_iterator ci(mc.clauses.begin()), ce(mc.clauses.end());
	guard_iterator gi(mc.guards_begin()), ge(mc.guards_end());
for ( ; ci!=ce; ++ci, ++gi) {
	const meta_conditional_base::guard_ptr_type& guard(*gi);
	// guards may be NULL-terminated with else clause
	if (!guard) {
		if (!ci->unroll(c).good) {
			cerr << "Error encountered in conditional " << str
				<< " else-clause." << endl;
			return good_bool(false);
		}
		return good_bool(true);
	}
	const count_ptr<const pbool_const>
		g(guard->__unroll_resolve_rvalue(c, guard));
	if (!g) {
		cerr << "Error evaluating guard expression of conditional "
			<< str << "." << endl;
		return good_bool(false);
	}
	// no change in context necessary
	if (g->static_constant_value()) {
		const element_type& if_clause(*ci);
		if (!if_clause.unroll(c).good) {
			cerr << "Error encountered in conditional " << str
				<< " if-clause." << endl;
			return good_bool(false);
		}
		return good_bool(true);
	}
}	// end for
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
meta_conditional<T>::append_guarded_clause(T& mc,
		const meta_conditional_base::guard_ptr_type& g) {
	mc.guards.push_back(g);
	mc.clauses.push_back(element_type());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
meta_conditional<T>::collect_transient_info(const T& mc, 
		persistent_object_manager& m) {
if (!m.register_transient_object(&mc, util::persistent_traits<T>::type_key)) {
	mc.meta_conditional_base::collect_transient_info_base(m);
	for_each(mc.clauses.begin(), mc.clauses.end(),
		util::persistent_collector_ref(m));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
meta_conditional<T>::write_object(const T& mc, 
		const persistent_object_manager& m,
		ostream& o) {
	mc.meta_conditional_base::write_object_base(m, o);
	const size_t s = mc.clauses.size();
	util::write_value(o, s);
	for_each(mc.clauses.begin(), mc.clauses.end(),
		util::persistent_writer<element_type>(
			&element_type::write_object_base, m, o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
void
meta_conditional<T>::load_object(T& mc, 
		const persistent_object_manager& m, istream& i) {
	mc.meta_conditional_base::load_object_base(m, i);
	size_t s;
	util::read_value(i, s);
	mc.clauses.resize(s);
	for_each(mc.clauses.begin(), mc.clauses.end(),
		util::persistent_loader<element_type>(
			&element_type::load_object_base, m, i));
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_META_CONDITIONAL_TCC__

