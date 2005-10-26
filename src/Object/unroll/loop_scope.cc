/**
	\file "Object/unroll/loop_scope.cc"
	Control-flow related class method definitions.  
 	$Id: loop_scope.cc,v 1.2.22.1 2005/10/26 22:12:36 fang Exp $
 */

#ifndef	__OBJECT_UNROLL_LOOP_SCOPE_CC__
#define	__OBJECT_UNROLL_LOOP_SCOPE_CC__

#include "Object/unroll/loop_scope.h"
#include "Object/unroll/unroll_context.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/meta_range_expr.h"
#include "Object/inst/pint_value_collection.h"
#include "Object/persistent_type_hash.h"
#include "common/TODO.h"
#include "common/ICE.h"
#include "util/persistent_object_manager.tcc"
#include "util/indent.h"

namespace util {
	SPECIALIZE_UTIL_WHAT(ART::entity::loop_scope, "loop-scope")
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		ART::entity::loop_scope, LOOP_SCOPE_TYPE_KEY, 0)
}

namespace ART {
namespace entity {
#include "util/using_ostream.h"
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
// class loop_scope method definitions

// loop_scope::loop_scope(const never_ptr<const sequential_scope> p)
loop_scope::loop_scope() : interface_type(), parent_type(), meta_loop_base() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
loop_scope::loop_scope(const ind_var_ptr_type& i, const range_ptr_type& r) :
		interface_type(), parent_type(), meta_loop_base(i, r) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
loop_scope::~loop_scope() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(loop_scope)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: add context to instance_management dumps.
 */
ostream&
loop_scope::dump(ostream& o) const {
	NEVER_NULL(ind_var);
	NEVER_NULL(range);
	o << auto_indent << "(;" << ind_var->get_name() << ':';
	range->dump(o, expr_dump_context::default_value) << ':' << endl;
	{
		INDENT_SECTION(o);
		parent_type::dump(o);
	}
	return o << auto_indent << ')';
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
loop_scope::unroll(const unroll_context& c) const {
	FINISH_ME(Fang);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
loop_scope::create_unique(const unroll_context& c, footprint& f) const {
	FINISH_ME(Fang);
	return good_bool(false);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
loop_scope::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		persistent_traits<this_type>::type_key)) {
	meta_loop_base::collect_transient_info_base(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
loop_scope::write_object(const persistent_object_manager& m, ostream& o) const {
	meta_loop_base::write_object_base(m, o);
	parent_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
loop_scope::load_object(const persistent_object_manager& m, istream& i) {
	meta_loop_base::load_object_base(m, i);
	parent_type::load_object_base(m, i);
}

//=============================================================================
} 	// end namespace entity
}	// end namespace ART

#endif	// __OBJECT_UNROLL_LOOP_SCOPE_CC__

