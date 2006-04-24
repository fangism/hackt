/**
	\file "Object/unroll/port_connection.tcc"
 	$Id: port_connection.tcc,v 1.3 2006/04/24 00:28:08 fang Exp $
 */

#ifndef	__HAC_OBJECT_UNROLL_PORT_CONNECTION_TCC__
#define	__HAC_OBJECT_UNROLL_PORT_CONNECTION_TCC__

#include <iostream>

#include "Object/unroll/port_connection.h"
#include "Object/unroll/unroll_context.h"	// for debugging
#include "Object/ref/simple_meta_instance_reference.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/inst/substructure_alias_base.h"

#include "util/what.tcc"
#include "util/memory/count_ptr.tcc"
#include "util/persistent_object_manager.tcc"
#include "util/stacktrace.h"

//=============================================================================
namespace HAC {
namespace entity {
using util::persistent_traits;
#include "util/using_ostream.h"

//=============================================================================
// class port_connection method definitions

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Private empty constructor.
 */
PORT_CONNECTION_TEMPLATE_SIGNATURE
PORT_CONNECTION_CLASS::port_connection() :
		parent_type(), ported_inst(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Initial constructor for a port-connection.  
	\param i an instance of the definition that is to be connected.
 */
PORT_CONNECTION_TEMPLATE_SIGNATURE
PORT_CONNECTION_CLASS::port_connection(const ported_inst_ptr_type& i) :
		parent_type(), ported_inst(i) {
	NEVER_NULL(ported_inst);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Default destructor.
 */
PORT_CONNECTION_TEMPLATE_SIGNATURE
PORT_CONNECTION_CLASS::~port_connection() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_CONNECTION_TEMPLATE_SIGNATURE
ostream&
PORT_CONNECTION_CLASS::what(ostream& o) const {
	return o << util::what<this_type>::name();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_CONNECTION_TEMPLATE_SIGNATURE
ostream&
PORT_CONNECTION_CLASS::dump(ostream& o, const expr_dump_context& dc) const {
	NEVER_NULL(ported_inst);
	ported_inst->dump(o, dc) << " (";
	parent_type::dump_ports(o, dc);
	return o << ");";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands and finalizes the connection at unroll time.  
 */
PORT_CONNECTION_TEMPLATE_SIGNATURE
good_bool
PORT_CONNECTION_CLASS::unroll(const unroll_context& c) const {
	STACKTRACE_VERBOSE;
#if ENABLE_STACKTRACE
	STACKTRACE_INDENT_PRINT("context c @ " << &c << endl);
	c.dump(cerr) << endl;
#endif
	NEVER_NULL(ported_inst);
	const never_ptr<substructure_alias>
		parent_instance(
			ported_inst->unroll_scalar_substructure_reference(c));
	if (!parent_instance) {
		cerr << "ERROR: resolving super instance of port connection: ";
		ported_inst->dump(cerr, 
			expr_dump_context::default_value) << endl;
		return good_bool(false);
	}
	// iterators point to meta_instance_reference_base
	return parent_instance->connect_ports(inst_list, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_CONNECTION_TEMPLATE_SIGNATURE
void
PORT_CONNECTION_CLASS::collect_transient_info(
		persistent_object_manager& m) const {
if (!m.register_transient_object(this, 
		persistent_traits<this_type>::type_key)) {
	NEVER_NULL(this->ported_inst);
	ported_inst->collect_transient_info(m);
	parent_type::collect_transient_info_base(m);
}
// else already visited
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_CONNECTION_TEMPLATE_SIGNATURE
void
PORT_CONNECTION_CLASS::write_object(const persistent_object_manager& m, 
		ostream& f) const {
	m.write_pointer(f, this->ported_inst);
	parent_type::write_object_base(m, f);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
PORT_CONNECTION_TEMPLATE_SIGNATURE
void
PORT_CONNECTION_CLASS::load_object(const persistent_object_manager& m,
		istream& f) {
	m.read_pointer(f, this->ported_inst);
	parent_type::load_object_base(m, f);
}

//=============================================================================
}	// end namespace entity
}	// end namespace HAC

#endif	// __HAC_OBJECT_UNROLL_PORT_CONNECTION_TCC__

