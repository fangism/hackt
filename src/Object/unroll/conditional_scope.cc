/**
	\file "Object/unroll/conditional_scope.cc"
	Control-flow related class method definitions.  
 	$Id: conditional_scope.cc,v 1.8 2007/11/26 08:27:43 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_CONDITIONAL_SCOPE_CC__
#define	__HAC_OBJECT_UNROLL_CONDITIONAL_SCOPE_CC__

#include "Object/unroll/conditional_scope.h"
#include "Object/unroll/unroll_context.h"
#include "Object/def/template_formals_manager.h"
#include "Object/expr/pbool_expr.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/persistent_type_hash.h"
#include "common/TODO.h"
#include "common/ICE.h"
#include "util/persistent_object_manager.tcc"
#include "util/persistent_functor.tcc"
#include "util/indent.h"

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::conditional_scope,
		"conditional-scope")
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		HAC::entity::conditional_scope, CONDITIONAL_SCOPE_TYPE_KEY, 0)
}

namespace HAC {
namespace entity {
#include "util/using_ostream.h"
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// class conditional_scope method definitions

// conditional_scope::conditional_scope(const never_ptr<const sequential_scope> p)
conditional_scope::conditional_scope() :
		interface_type(), 
		meta_conditional_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
conditional_scope::~conditional_scope() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(conditional_scope)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: add context to instance_management dumps.
 */
ostream&
conditional_scope::dump(ostream& o, const expr_dump_context& dc) const {
if (guards.empty()) {
	// should only occur during debugging
	return o << "[EMPTY]";
}
	INVARIANT(guards.size() == clauses.size());
	typedef	clause_list_type::const_iterator	clause_iterator;
	typedef	meta_conditional_base::const_iterator	guard_iterator;
	clause_iterator ci(clauses.begin()), ce(clauses.end());
	guard_iterator gi(guards.begin()), ge(guards.end());
	NEVER_NULL(*gi);
	(*gi)->dump(o << "[ ", dc) << " -> " << endl;
	{
		INDENT_SECTION(o);
		ci->dump(o, dc);	// << endl;
	}
	for (++ci, ++gi; ci!=ce; ++ci, ++gi) {
		o << auto_indent << "[] ";
		if (*gi) {
			(*gi)->dump(o, dc);
		} else {
			o << "else";
		}
		o << " -> " << endl;
		{
			INDENT_SECTION(o);
			ci->dump(o, dc);	// << endl;
		}
	}
	return o << auto_indent << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if all clauses are empty.
 */
bool
conditional_scope::empty(void) const {
	typedef	clause_list_type::const_iterator	clause_iterator;
	clause_iterator ci(clauses.begin()), ce(clauses.end());
	for ( ; ci!=ce; ++ci) {
		if (!ci->empty())
			return false;
	}
	return true;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::append_guarded_clause(const guard_ptr_type& g) {
	guards.push_back(g);
	clauses.push_back(sequential_scope());
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sequential if-else: evaluate the clause of the first true guard only.
	If none are true, just return.
 */
good_bool
conditional_scope::unroll(const unroll_context& c) const {
	typedef	clause_list_type::const_iterator	clause_iterator;
	typedef	meta_conditional_base::const_iterator	guard_iterator;
	clause_iterator ci(clauses.begin()), ce(clauses.end());
	guard_iterator gi(guards.begin()), ge(guards.end());
for ( ; ci!=ce; ++ci, ++gi) {
	const guard_ptr_type& guard(*gi);
	NEVER_NULL(guard);
	bool b;
	if (!guard->unroll_resolve_value(c, b).good) {
		cerr << "Error resolving guard expression: ";
		guard->dump(cerr, expr_dump_context::default_value) << endl;
		return good_bool(false);
	}
if (b) {
	if (!ci->unroll(c).good) {
		cerr << "Error resolving conditional-body:"
			<< endl;
		return good_bool(false);
	}
	return good_bool(true);
}	// end if bool true
	// else don't bother expanding inside
}	// end for-each guard-clause pair
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	meta_conditional_base::collect_transient_info_base(m);
	for_each(clauses.begin(), clauses.end(),
		util::persistent_collector_ref(m));
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_conditional_base::write_object_base(m, o);
	const size_t s = clauses.size();
	util::write_value(o, s);
	for_each(clauses.begin(), clauses.end(),
		util::persistent_writer<parent_type>(
			&parent_type::write_object_base, m, o));
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::load_object(const persistent_object_manager& m, istream& i) {
	meta_conditional_base::load_object_base(m, i);
	size_t s;
	util::read_value(i, s);
	clauses.resize(s);
	for_each(clauses.begin(), clauses.end(),
		util::persistent_loader<parent_type>(
			&parent_type::load_object_base, m, i));
}

//=============================================================================
} 	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_CONDITIONAL_SCOPE_CC__

