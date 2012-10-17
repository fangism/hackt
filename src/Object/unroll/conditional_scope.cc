/**
	\file "Object/unroll/conditional_scope.cc"
	Control-flow related class method definitions.  
 	$Id: conditional_scope.cc,v 1.11 2011/04/02 01:46:11 fang Exp $
 */

#include "Object/unroll/conditional_scope.hh"
#include "Object/unroll/meta_conditional.tcc"
#include "Object/def/template_formals_manager.hh"
#include "Object/persistent_type_hash.hh"
#include "common/TODO.hh"
#include "common/ICE.hh"

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::conditional_scope,
		"conditional-scope")
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		HAC::entity::conditional_scope, CONDITIONAL_SCOPE_TYPE_KEY, 0)
}

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
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
	return meta_conditional_type::dump(*this, o, dc);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return true if all clauses are empty.
 */
bool
conditional_scope::empty(void) const {
	return meta_conditional_type::empty(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::append_guarded_clause(const guard_ptr_type& g) {
	meta_conditional_type::append_guarded_clause(*this, g);
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
	bool b;
	if (!guard) {
		b = true;
	} else if (!guard->unroll_resolve_value(c, b).good) {
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
	INVARIANT(gi == ge);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::collect_transient_info(persistent_object_manager& m) const {
	meta_conditional_type::collect_transient_info(*this, m);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_conditional_type::write_object(*this, m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::load_object(const persistent_object_manager& m, istream& i) {
	meta_conditional_type::load_object(*this, m, i);
}

//=============================================================================
} 	// end namespace entity
}	// end namespace HAC

