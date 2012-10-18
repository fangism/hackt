/**
	\file "Object/unroll/port_scope.cc"
	Control-flow related class method definitions.  
 	$Id: port_scope.cc,v 1.2 2011/04/02 01:46:15 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include "Object/unroll/port_scope.hh"
#include "Object/def/template_formals_manager.hh"
#include "Object/expr/expr_dump_context.hh"
#include "Object/persistent_type_hash.hh"
#include "Object/inst/port_visitor.hh"
#include "Object/inst/alias_empty.hh"
#include "Object/inst/alias_actuals.hh"
#include "Object/inst/instance_alias_info.hh"
#include "Object/traits/instance_traits.hh"
#include "Object/unroll/unroll_context.hh"
#include "Object/def/footprint.hh"
#include "common/TODO.hh"
#include "common/ICE.hh"
#include "util/stacktrace.hh"
#include "util/indent.hh"
#include "util/persistent_object_manager.tcc"

namespace util {
	SPECIALIZE_UTIL_WHAT(HAC::entity::port_scope, "port-scope")
	SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
		HAC::entity::port_scope, PORT_SCOPE_TYPE_KEY, 0)
}

namespace HAC {
namespace entity {
#include "util/using_ostream.hh"
using util::auto_indent;
using util::persistent_traits;

//=============================================================================
/**
	DIRTY HACK: flag all bools in footprint (immediately after ports are 
	unrolled) as public ports.  
	The base class's interface was originally intended for 
	read-only access to instance aliases, but we use this for
	modification.  
 */
class port_flagger : public port_visitor {
public:
	port_flagger() : port_visitor() { }

	void
	visit(const instance_alias_info<bool_tag>&);

	using port_visitor::visit;
};	// end class port_flagger

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Pardon the const_cast... we need to modif
 */
#define	DEFINE_VISIT_FLAG_PORT(Tag)					\
void									\
port_flagger::visit(const instance_alias_info<Tag>& a) {		\
	STACKTRACE_VERBOSE;						\
	const_cast<instance_alias_info<Tag>&>(a).flag_port();		\
}

#if 0
	// debug
	STACKTRACE_INDENT_PRINT("bool: ");				\
	a.dump_hierarchical_name(cerr) << endl;				\
	a.dump_raw_attributes(STACKTRACE_INDENT_PRINT("flags = ")) << endl;
#endif

DEFINE_VISIT_FLAG_PORT(bool_tag)

#undef	DEFINE_VISIT_FLAG_PORT

//=============================================================================
// class port_scope method definitions

// port_scope::port_scope(const never_ptr<const sequential_scope> p)
port_scope::port_scope() : interface_type(), parent_type() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
port_scope::~port_scope() {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(port_scope)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: add context to instance_management dumps.
 */
ostream&
port_scope::dump(ostream& o, const expr_dump_context& dc) const {
#if 1
	if (!implementation_type::empty()) {
		o << "ports: (" << endl;
		{
		INDENT_SECTION(o);
		implementation_type::dump(o, dc);
		}
		o << auto_indent << ')';
	}
	return o;
#else
	// Don't bother with indenting scope, just make it transparent
	return implementation_type::dump(o, dc);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just a wrapped call to sequential_scope::unroll()
	but we can insert additional code here for special actions
	after ports are instantiated.
 */
good_bool
port_scope::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	if (!implementation_type::unroll(c).good) {
		return good_bool(false);
	}
	// TODO: do something after
	// visit all instances in current target_footprint
	// and flag as port (especially bools)
	port_flagger p;
	c.get_target_footprint().accept(p);
	return good_bool(true);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_scope::collect_transient_info(persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		util::persistent_traits<this_type>::type_key)) {
	implementation_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_scope::write_object(const persistent_object_manager& m, ostream& o) const {
	implementation_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
port_scope::load_object(const persistent_object_manager& m, istream& i) {
	implementation_type::load_object_base(m, i);
}

//=============================================================================
} 	// end namespace entity
}	// end namespace HAC

