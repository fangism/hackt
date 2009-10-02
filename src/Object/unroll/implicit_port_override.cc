/**
	\file "Object/unroll/implicit_port_override.cc"
	$Id: implicit_port_override.cc,v 1.2 2009/10/02 01:57:17 fang Exp $
 */

#define	ENABLE_STACKTRACE			0

#include <iostream>
#include "Object/unroll/implicit_port_override.h"
#include "Object/ref/meta_instance_reference_subtypes.h"
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/traits/bool_traits.h"
#include "Object/traits/proc_traits.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/inst/substructure_alias_base.h"
#include "Object/persistent_type_hash.h"
#include "util/memory/count_ptr.tcc"
#include "util/what.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.tcc"

namespace util {
SPECIALIZE_UTIL_WHAT(HAC::entity::implicit_port_override,
		"implicit_port_override")

SPECIALIZE_PERSISTENT_TRAITS_FULL_DEFINITION(
	HAC::entity::implicit_port_override, IMPLICIT_PORT_OVERRIDE_TYPE_KEY, 0)
}	// end namespace util

//=============================================================================
namespace HAC {
namespace entity {
#include "util/using_ostream.h"

// class implicit_port_override method definitions
/**
	Exmpt ctor only used by persistent object reconstruction.  
 */
implicit_port_override::implicit_port_override() :
		parent_type(), inst() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
implicit_port_override::implicit_port_override(const instance_ptr_type& p) :
		parent_type(), inst(p) {
	NEVER_NULL(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
implicit_port_override::~implicit_port_override() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PERSISTENT_WHAT_DEFAULT_IMPLEMENTATION(implicit_port_override)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
implicit_port_override::dump(ostream& o, const expr_dump_context& c) const {
	NEVER_NULL(inst);
	inst->dump(o, c) << " $(";
	parent_type::dump_ports(o, c);
	return o << ");";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
implicit_port_override::append_bool_port(const port_ptr_type& p) {
	STACKTRACE_VERBOSE;
	inst_list.push_back(p);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
good_bool
implicit_port_override::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
	const never_ptr<substructure_alias>
		parent_instance(inst->unroll_scalar_substructure_reference(c));
	if (!parent_instance) {
		cerr <<
"ERROR: resolving process instance of implicit port override: ";
		inst->dump(cerr, expr_dump_context::default_value) << endl;
		return good_bool(false);
	}
	return parent_instance->connect_implicit_ports(inst_list, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
implicit_port_override::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this,
		util::persistent_traits<this_type>::type_key)) {
	NEVER_NULL(inst);
	inst->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
implicit_port_override::write_object(const persistent_object_manager& m, 
		ostream& o) const {
	m.write_pointer(o, inst);
	parent_type::write_object_base(m, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
implicit_port_override::load_object(const persistent_object_manager& m, 
		istream& i) {
	m.read_pointer(i, inst);
	parent_type::load_object_base(m, i);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

