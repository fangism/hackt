/**
	\file "AST/globals.cc"
	Shared AST objects.  
	$Id: globals.cc,v 1.4 2010/07/01 20:20:23 fang Exp $
 */

#define	ENABLE_STATIC_TRACE			0
#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE			0

#include "AST/globals.h"
#include "AST/formal.h"
#include "AST/instance.h"
#include "AST/reference.h"
#include "AST/type.h"
#include "AST/attribute.h"
#include "AST/identifier.h"
#include "AST/token.h"
#include "AST/token_string.h"
#include "util/stacktrace.h"
#include "util/memory/count_ptr.tcc"

REQUIRES_STACKTRACE_STATIC_INIT

namespace HAC {
namespace parser {
//=============================================================================
// static global vars

// this can and should be shared, nevermind the token positions recorded
static
const count_ptr<const token_identifier>&
__Vdd(void) {
	// function-local static, on-demand
	static const count_ptr<const token_identifier>
		ret(new token_identifier("!Vdd"));
	NEVER_NULL(ret);
	return ret;
}

static
const count_ptr<const token_identifier>&
__GND(void) {
	// function-local static, on-demand
	static const count_ptr<const token_identifier>
		ret(new token_identifier("!GND"));
	NEVER_NULL(ret);
	return ret;
}

static
const count_ptr<const generic_type_ref>&
__bool_scalar(void) {
	static const count_ptr<const generic_type_ref>
		ret(new generic_type_ref(new token_bool_type("bool"), NULL));
	NEVER_NULL(ret);
	return ret;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Wish I could just insert a scheme/lisp structure...
	Ordering must be kept consistent with GND_index and Vdd_index 
	in "net/netgraph.cc", and auto-connection in "Object/def/footprint.cc".
 */
const count_ptr<const port_formal_decl>&
get_implicit_ports(void) {
	STACKTRACE_VERBOSE;
	static count_ptr<const port_formal_decl> __implicit_ports;
if (!__implicit_ports) {
	// automatic Vdd! and GND! ports
	// as if: "bool GND!, Vdd!"
	port_formal_id_list* g = new port_formal_id_list;
	g->push_front(new port_formal_id(__Vdd()));
	g->push_front(new port_formal_id(__GND()));
	__implicit_ports = count_ptr<const port_formal_decl>(
		new port_formal_decl(__bool_scalar(), g));
}
	NEVER_NULL(__implicit_ports);
	return __implicit_ports;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Implicit global declarations for top-level module, global scope.
	Ordering must be kept consistent with GND_index and Vdd_index 
	in "net/netgraph.cc", and auto-connection in "Object/def/footprint.cc".
 */
const count_ptr<const instance_declaration>&
get_implicit_globals(void) {
	STACKTRACE_VERBOSE;
	static count_ptr<const instance_declaration> __implicit_globals;
if (!__implicit_globals) {
	instance_id_list* g = new instance_id_list;
	g->push_front(new instance_base(__Vdd()));
	g->push_front(new instance_base(__GND()));
	__implicit_globals = count_ptr<const instance_declaration>(
		new instance_declaration(__bool_scalar(), g));
}
	NEVER_NULL(__implicit_globals);
	return __implicit_globals;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// and attributes on the implicit supplies
// to return an 'instance_attribute_statement' [hackt-parse.yy.in]

static
const count_ptr<const token_identifier>
__supply_tok(void) {
	static const count_ptr<const token_identifier>
		ret(new token_identifier("supply"));
	NEVER_NULL(ret);
	return ret;
}

// effectively: !GND @ [supply=0];
const count_ptr<const type_completion_statement>
get_GND_attributes(void) {
	static const count_ptr<const type_completion_statement>
	__GND_attributes(new type_completion_statement(
		new id_expr(new qualified_id(__GND())),
		NULL,
		new generic_attribute_list(new generic_attribute(
			__supply_tok(), new expr_list(new token_int(0))))));
	return __GND_attributes;
}

// effectively: !Vdd @ [supply=1];
const count_ptr<const type_completion_statement>
get_Vdd_attributes(void) {
	static const count_ptr<const type_completion_statement>
	__Vdd_attributes(new type_completion_statement(
		new id_expr(new qualified_id(__Vdd())),
		NULL,
		new generic_attribute_list(new generic_attribute(
			__supply_tok(), new expr_list(new token_int(1))))));
	return __Vdd_attributes;
}

//=============================================================================
}	// end namespace parser
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

