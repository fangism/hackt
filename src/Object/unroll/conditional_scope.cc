/**
	\file "Object/unroll/conditional_scope.cc"
	Control-flow related class method definitions.  
 	$Id: conditional_scope.cc,v 1.7 2006/03/15 04:38:22 fang Exp $
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
		interface_type(), parent_type(), meta_conditional_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
conditional_scope::conditional_scope(const guard_ptr_type& g) :
		interface_type(), parent_type(), meta_conditional_base(g) {
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
	NEVER_NULL(guard);
	guard->dump(o << "[ ", dc) << " ->" << endl;
	{
		INDENT_SECTION(o);
		parent_type::dump(o, dc);
	}
	return o << auto_indent << ']';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	If the guard evaluate true, then unroll the body sequence.  
 */
good_bool
conditional_scope::unroll(const unroll_context& c) const {
	bool b;
	if (!guard->unroll_resolve_value(c, b).good) {
		cerr << "Error resolving guard expression: ";
		guard->dump(cerr, expr_dump_context::default_value) << endl;
		return good_bool(false);
	}
if (b) {
	if (!parent_type::unroll(c).good) {
		cerr << "Error resolving conditional-body:"
			<< endl;
		return good_bool(false);
	}
}
	// else don't bother expanding inside
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	meta_conditional_base::collect_transient_info_base(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::write_object(const persistent_object_manager& m,
		ostream& o) const {
	meta_conditional_base::write_object_base(m, o);
	parent_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
conditional_scope::load_object(const persistent_object_manager& m, istream& i) {
	meta_conditional_base::load_object_base(m, i);
	parent_type::load_object_base(m, i);
}

//=============================================================================
} 	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_CONDITIONAL_SCOPE_CC__

