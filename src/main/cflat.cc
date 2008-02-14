/**
	\file "main/cflat.cc"
	cflat backwards compability module.  

	$Id: cflat.cc,v 1.19.14.2 2008/02/14 18:23:02 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	ENABLE_STATIC_TRACE		0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <string>

#include "main/cflat.h"
#include "main/cflat_options.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "main/options_modifier.tcc"
#include "main/global_options.h"

#include "Object/type/process_type_reference.h"
#include "common/TODO.h"

#include "util/getopt_mapped.tcc"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.h"

namespace HAC {
using std::string;
using util::persistent;
using util::persistent_object_manager;
#include "util/using_ostream.h"
using entity::process_type_reference;


//=============================================================================
// explicit early class instantiation for proper static initializer ordering
// this guarantees the registry map is initialized before anything is registered

// preferred by g++-3.3, but rejected by g++-4
// template class cflat::options_modifier_policy_type;
// accepted by both :S (hate having to copy exact name...)
template class options_modifier_policy<cflat_options>;

//=============================================================================
// class cflat static initializers

cflat::master_options_map_type
cflat::master_options;

const int
cflat::master_options_initialized = cflat::initialize_master_options_map();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const char
cflat::name[] = "cflat";

const char
cflat::brief_str[] =
	"Flattens instantiations CAST-style (backward compatibility)";

STATIC_TRACE_HERE("before cflat registry")

#ifndef	WITH_MAIN
const size_t
cflat::program_id = register_hackt_program_class<cflat>();
#endif

//=============================================================================
static const char default_options_brief[] = "(CAST cflat preset)";

/**
	CFLAT mode registration mechanism.  
 */
class cflat::register_options_modifier :
	public options_modifier_policy_type::register_options_modifier_base {
	typedef	options_modifier_policy_type::register_options_modifier_base
						base_type;
public:
	register_options_modifier(const string& Mode,
			const modifier_type COM, 
			const char* b = default_options_brief) :
			base_type(Mode, COM, b) {
	}

};	// end class register_options_modifier

//=============================================================================
// cflat::options_modifier declarations and definitions

/***
	Only the primary modes set the primary_tool field, 
	all other mode modifiers leave it untouched.  
***/

/// Does absolutely nothing.  
static
void
__cflat_default(cflat::options&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// preset modes
static
void
__cflat_prsim(cflat::options& cf) {
	cf.primary_tool = cflat::options::TOOL_PRSIM;
	cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_EQUAL;
	cf.include_prs = true;
	cf.dump_self_connect = false;
	cf.enquote_names = true;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
#if CFLAT_WITH_CONDUCTANCES
	cf.compute_conductances = false;
#endif
	cf.size_prs = false;
	cf.use_referenced_type_instead_of_top_level = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__cflat_prlint(cflat::options& cf) {
	cf.primary_tool = cflat::options::TOOL_PRLINT;
	cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_NONE;
	cf.include_prs = true;
	cf.dump_self_connect = false;
	cf.enquote_names = true;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
#if CFLAT_WITH_CONDUCTANCES
	cf.compute_conductances = false;
#endif
	cf.use_referenced_type_instead_of_top_level = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__cflat_connect(cflat::options& cf) {
	// cf.primary_tool = ?
	// cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_CONNECT;
	cf.include_prs = false;
	cf.dump_self_connect = true;
	cf.enquote_names = true;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
#if CFLAT_WITH_CONDUCTANCES
	cf.compute_conductances = false;
#endif
	cf.use_referenced_type_instead_of_top_level = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Also works for:
	ergen, prs2tau, LVS (what's the difference?), 
	alint
 */
static
void
__cflat_lvs(cflat::options& cf) {
	cf.primary_tool = cflat::options::TOOL_LVS;
	cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_CONNECT;
	cf.include_prs = true;
	cf.dump_self_connect = false;
	cf.enquote_names = true;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
#if CFLAT_WITH_CONDUCTANCES
	cf.compute_conductances = false;
#endif
	cf.use_referenced_type_instead_of_top_level = false;
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Also works for:
	aspice, Aspice
 */
static
void
__cflat_wire(cflat::options& cf) {
	// cf.primary_tool = ?
	cf.connect_style = cflat::options::CONNECT_STYLE_WIRE;
	cf.include_prs = false;
	cf.dump_self_connect = false;
	cf.enquote_names = false;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = true;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
#if CFLAT_WITH_CONDUCTANCES
	cf.compute_conductances = false;
#endif
	cf.use_referenced_type_instead_of_top_level = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__cflat_ADspice(cflat::options& cf) {
	// cf.primary_tool = ?
	// cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_WIRE;
	cf.include_prs = true;
	cf.dump_self_connect = false;
	cf.enquote_names = false;
	cf.dump_non_bools = false;
	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;
	cf.check_prs = false;
	cf.wire_mode = true;
	cf.csim_style_prs = false;
	cf.dsim_prs = true;
	cf.size_prs = false;
#if CFLAT_WITH_CONDUCTANCES
	cf.compute_conductances = false;
#endif
	cf.use_referenced_type_instead_of_top_level = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just check, don't print.
 */
static
void
__cflat_check(cflat::options& cf) {
	// cf.primary_tool = ?
	// cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_NONE;
	cf.include_prs = true;
	cf.check_prs = true;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
	cf.size_prs = false;
	cf.use_referenced_type_instead_of_top_level = false;
}

// end of primary modes
//=============================================================================
STATIC_TRACE_HERE("before cflat option modifiers")

const cflat::register_options_modifier
/***
@texinfo cflat/mode-prsim.texi
@defvr {@t{cflat} option} prsim
@command{prsim} output mode.
@end defvr
@end texinfo
***/
	cflat::_prsim("prsim", &__cflat_prsim),

/***
@texinfo cflat/mode-lvs.texi
@defvr {@t{cflat} option} lvs
@defvrx {@t{cflat} option} LVS
LVS output mode.
@end defvr
@end texinfo
***/
	cflat::_lvs("lvs", &__cflat_lvs),
	cflat::_LVS("LVS", &__cflat_lvs),		// re-use lvs

/***
@texinfo cflat/mode-ergen.texi
@defvr {@t{cflat} option} ergen
@command{ergen} output mode.
@end defvr
@end texinfo
***/
	cflat::_ergen("ergen", &__cflat_lvs),		// re-use lvs

/***
@texinfo cflat/mode-alint.texi
@defvr {@t{cflat} option} alint
@command{alint} output mode.
@end defvr
@end texinfo
***/
	cflat::_alint("alint", &__cflat_lvs),		// re-use lvs

/***
@texinfo cflat/mode-prlint.texi
@defvr {@t{cflat} option} prlint
@command{prlint} output mode.
@end defvr
@end texinfo
***/
	cflat::_prlint("prlint", &__cflat_prlint),

/***
@texinfo cflat/mode-prs2tau.texi
@defvr {@t{cflat} option} prs2tau
@command{prs2tau} output mode.
@end defvr
@end texinfo
***/
	cflat::_prs2tau("prs2tau", &__cflat_lvs),	// re-use lvs

/***
@texinfo cflat/mode-connect.texi
@defvr {@t{cflat} option} connect
@command{connect} output mode.
@end defvr
@end texinfo
***/
	cflat::_connect("connect", &__cflat_connect),

/***
@texinfo cflat/mode-check.texi
@defvr {@t{cflat} option} check
@command{check} output mode.
@end defvr
@end texinfo
***/
	cflat::_check("check", &__cflat_check),

/***
@texinfo cflat/mode-wire.texi
@defvr {@t{cflat} option} wire
@command{wire} output mode.
@end defvr
@end texinfo
***/
	cflat::_wire("wire", &__cflat_wire),

/***
@texinfo cflat/mode-aspice.texi
@defvr {@t{cflat} option} aspice
@defvrx {@t{cflat} option} Aspice
@command{aspice} output mode.
@end defvr
@end texinfo
***/
	cflat::_aspice("aspice", &__cflat_wire),	// re-use wire
	cflat::_Aspice("Aspice", &__cflat_wire),	// re-use wire

/***
@texinfo cflat/mode-ADspice.texi
@defvr {@t{cflat} option} ADspice
@command{ADspice} output mode.
@end defvr
@end texinfo
***/
	cflat::_ADspice("ADspice", &__cflat_ADspice),

/***
@texinfo cflat/mode-default.texi
@defvr {@t{cflat} option} default
@command{default} output mode.
@end defvr
@end texinfo
***/
	cflat::_default("default", &__cflat_default);

STATIC_TRACE_HERE("after cflat option modifiers")

//-----------------------------------------------------------------------------
// for setting/unsetting individual flags

// TODO: reperitive code should be templated or macro'd
// define for common patterns to reduce clutter.

/***
@texinfo cflat/opt-connect-none.texi
@defvr {@t{cflat -f} option} no-connect
@defvrx {@t{cflat -f} option} connect-none
Suppress printing of aliases.  
@end defvr
@end texinfo
***/
static
void
__cflat_connect_none(cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_NONE;
}
const cflat::register_options_modifier
	cflat::_connect_none("connect-none", &__cflat_connect_none, 
		"suppresses all printing of aliases"), 
	cflat::_no_connect("no-connect", &__cflat_connect_none, 
		"(connect-none)");

/***
@texinfo cflat/opt-connect-equal.texi
@defvr {@t{cflat -f} option} connect-equal
Print aliases with style: @samp{= x y}.
@end defvr
@end texinfo
***/
static
void
__cflat_connect_equal(cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_EQUAL;
}
const cflat::register_options_modifier
	cflat::_connect_equal("connect-equal", &__cflat_connect_equal, 
		"alias-style: = x y");

/***
@texinfo cflat/opt-connect-connect.texi
@defvr {@t{cflat -f} option} connect-connect
Print aliases with style: @samp{connect x y}.
@end defvr
@end texinfo
***/
static
void
__cflat_connect_connect(cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_CONNECT;
}
const cflat::register_options_modifier
	cflat::_connect_connect("connect-connect", &__cflat_connect_connect,
		"alias-style: connect x y");

/***
@texinfo cflat/opt-connect-wire.texi
@defvr {@t{cflat -f} option} connect-wire
Print aliases with style: @samp{wire x y}.
@end defvr
@end texinfo
***/
static
void
__cflat_connect_wire(cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_WIRE;
}
const cflat::register_options_modifier
	cflat::_connect_wire("connect-wire", &__cflat_connect_wire,
		"alias-style: wire x y");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-include-prs.texi
@defvr {@t{cflat -f} option} include-prs
@defvrx {@t{cflat -f} option} exclude-prs
@defvrx {@t{cflat -f} option} no-include-prs
@defvrx {@t{cflat -f} option} no-exclude-prs
Include or exclude production rules from output.  
@end defvr
@end texinfo
***/
static
void
__cflat_include_prs(cflat::options& cf) {
	cf.include_prs = true;
}
const cflat::register_options_modifier
	cflat::_include_prs("include-prs", &__cflat_include_prs, 
		"includes prs in output"),
	cflat::_no_exclude_prs("no-exclude-prs", &__cflat_include_prs, 
		"(include-prs)");

static
void
__cflat_exclude_prs(cflat::options& cf) {
	cf.include_prs = false;
}
const cflat::register_options_modifier
	cflat::_exclude_prs("exclude-prs", &__cflat_exclude_prs,
		"excludes prs from output"),
	cflat::_no_include_prs("no-include-prs", &__cflat_exclude_prs, 
		"(exclude-prs)");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-self-aliases.texi
@defvr {@t{cflat -f} option} self-aliases
@defvrx {@t{cflat -f} option} no-self-aliases
Includes or exclude aliases @samp{x = x}.
@end defvr
@end texinfo
***/
static
void
__cflat_self_aliases(cflat::options& cf) {
	cf.dump_self_connect = true;
}
const cflat::register_options_modifier
	cflat::_self_aliases("self-aliases", &__cflat_self_aliases, 
		"includes aliases x = x");

static
void
__cflat_no_self_aliases(cflat::options& cf) {
	cf.dump_self_connect = false;
}
const cflat::register_options_modifier
	cflat::_no_self_aliases("no-self-aliases", &__cflat_no_self_aliases,
		"excludes aliases x = x");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-quote-names.texi
@defvr {@t{cflat -f} option} quote-names
@defvrx {@t{cflat -f} option} no-quote-names
Wrap all node names in ``quotes''.  
@end defvr
@end texinfo
***/
static
void
__cflat_quote_names(cflat::options& cf) {
	cf.enquote_names = true;
}
const cflat::register_options_modifier
	cflat::_quote_names("quote-names", &__cflat_quote_names,
		"wraps all node names in \"quotes\"");

static
void
__cflat_no_quote_names(cflat::options& cf) {
	cf.enquote_names = false;
}
const cflat::register_options_modifier
	cflat::_no_quote_names("no-quote-names", &__cflat_no_quote_names, 
		"no quote around node names");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-SEU.texi
@defvr {@t{cflat -f} option} SEU
@defvrx {@t{cflat -f} option} no-SEU
Enable single-event-upset mode for selected tool.  
@end defvr
@end texinfo
***/
static
void
__cflat_SEU(cflat::options& cf) {
	cf.tool_options |= cflat::options::TOOL_OPTIONS_SEU;
}
const cflat::register_options_modifier
	cflat::_SEU("SEU", &__cflat_SEU,
		"enable single-event-upset mode for tool");

static
void
__cflat_no_SEU(cflat::options& cf) {
	cf.tool_options &= ~cflat::options::TOOL_OPTIONS_SEU;
}
const cflat::register_options_modifier
	cflat::_no_SEU("no-SEU", &__cflat_no_SEU, 
		"disable single-event-upset mode");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	cf.dump_non_bools = false;
//	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-check-mode.texi
@defvr {@t{cflat -f} option} check-mode
@defvrx {@t{cflat -f} option} no-check-mode
Silences @command{cflat} output while traversing hierarchy.  
Useful only as a diagnostic tool for debugging.  
@end defvr
@end texinfo
***/
static
void
__cflat_check_mode(cflat::options& cf) {
	cf.check_prs = true;
}
const cflat::register_options_modifier
	cflat::_check_mode("check-mode", &__cflat_check_mode, 
		"silences cflat output while traversing hierarchy");

static
void
__cflat_no_check_mode(cflat::options& cf) {
	cf.check_prs = false;
}
const cflat::register_options_modifier
	cflat::_no_check_mode("no-check-mode", &__cflat_no_check_mode, 
		"cflat output enabled");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-wire-mode.texi
@defvr {@t{cflat -f} option} wire-mode
@defvrx {@t{cflat -f} option} no-wire-mode
Accumulate aliases in the form: @samp{wire (x,y,...)}
@end defvr
@end texinfo
***/
static
void
__cflat_wire_mode(cflat::options& cf) {
	cf.wire_mode = true;
}
const cflat::register_options_modifier
	cflat::_wire_mode("wire-mode", &__cflat_wire_mode, 
		"accumulate aliases in the form: wire (x,y,...)");

static
void
__cflat_no_wire_mode(cflat::options& cf) {
	cf.wire_mode = false;
}
const cflat::register_options_modifier
	cflat::_no_wire_mode("no-wire-mode", &__cflat_no_wire_mode, 
		"use one of the connect-* modes of printing aliases");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-dsim-prs.texi
@defvr {@t{cflat -f} option} dsim-prs
@defvrx {@t{cflat -f} option} no-dsim-prs
Wraps prs in: @samp{dsim @{ ... @}}
@end defvr
@end texinfo
***/
static
void
__cflat_dsim_prs(cflat::options& cf) {
	cf.dsim_prs = true;
}
const cflat::register_options_modifier
	cflat::_dsim_prs("dsim-prs", &__cflat_dsim_prs, 
		"wraps prs in: dsim { ... }");

static
void
__cflat_no_dsim_prs(cflat::options& cf) {
	cf.dsim_prs = false;
}
const cflat::register_options_modifier
	cflat::_no_dsim_prs("no-dsim-prs", &__cflat_no_dsim_prs, 
		"not (dsim-prs)");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-sizes.texi
@defvr {@t{cflat -f} option} sizes
@defvrx {@t{cflat -f} option} no-sizes
Prints rule literals with @t{<size>} specifications.  
@end defvr
@end texinfo
***/
static
void
__cflat_size_prs(cflat::options& cf) {
	cf.size_prs = true;
}
const cflat::register_options_modifier
	cflat::_size_prs("sizes", &__cflat_size_prs, 
		"prints rule literals with <size> specifications");

static
void
__cflat_no_size_prs(cflat::options& cf) {
	cf.size_prs = false;
}
const cflat::register_options_modifier
	cflat::_no_size_prs("no-sizes", &__cflat_no_size_prs, 
		"not (size-prs)");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CFLAT_WITH_CONDUCTANCES
/***
@texinfo cflat/opt-strengths.texi
@defvr {@t{cflat -f} option} strengths
@defvrx {@t{cflat -f} option} no-strengths
Prints min/max drive strengths for each rule.
Strengths are computed as such:
the maximum strength assumes that every guard subexpression is true
and conducting to the supply;
the minimum strength assumes that a single weakest path is conducting, 
and 'one' value for the stronge single path.
CMOS implementability of rules is ignored.  
The strength unit is relative to a 1W/1L drive strength, 
and is not normalized with respect to NFET/PFET mobility. 
The default width is 5, and default length is 2.  
@end defvr
@end texinfo
***/
static
void
__cflat_strengths(cflat::options& cf) {
	cf.compute_conductances = true;
}
const cflat::register_options_modifier
	cflat::_strengths("strengths", &__cflat_strengths, 
		"prints min/max strengths for each rule");

static
void
__cflat_no_strengths(cflat::options& cf) {
	cf.compute_conductances = false;
}
const cflat::register_options_modifier
	cflat::_no_strengths("no-strengths", &__cflat_no_strengths, 
		"suppresses min/max strengths for each rule");
#endif	// CFLAT WITH_CONDUCTANCES

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
//	cf.csim_style_prs = false;

//=============================================================================
cflat::cflat() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	argv[1] is the cflat mode.
	argv[2] is the name of the input object file.
	Q: any need to parse command line options? later...
 */
int
cflat::main(const int argc, char* argv[], const global_options&) {
	cflat_options cf;
	// cerr << "argc = " << argc << endl;
	if (argc < 3) {
		// cerr << "Insufficient arguments." << endl;
		usage();
		return 1;
	}
	// cflat mode
	const char* const mode = argv[1];
	const options_modifier_map_iterator mi(options_modifier_map.find(mode));
	if (mi == options_modifier_map.end() || !mi->second) {
		cerr << "Invalid mode: " << mode << endl;
		usage();
		return 1;
	} else {
		(mi->second)(cf);
	}

	// additional mode modifiers
	// shift arguments by one to pass over mode
	if (parse_command_options(argc-1, argv+1, cf)) {
		cerr << "Error in command invocation." << endl;
		usage();
		return 1;
	}
	// cerr << "optind = " << optind << endl;
	if (optind+1 >= argc || optind+2 < argc) {
		cerr << "Exactly one non-option argument <objfile> required."
			<< endl;
		usage();
		return 1;
	}
	const char* const ofn = argv[optind+1];
	static const char alloc_errstr[] = 
		"ERROR in allocating global state.  Aborting.";
	count_ptr<module> the_module;
if (cf.comp_opt.compile_input) {
	the_module = parse_and_check(ofn, cf.comp_opt);
} else {
	if (!check_object_loadable(ofn).good)
		return 1;
	the_module = load_module(ofn);
	// load_module_debug(ofn);
}
	if (!the_module)
		return 1;
	count_ptr<module> top_module;	// use this module to cflat
if (cf.use_referenced_type_instead_of_top_level) {
	const count_ptr<const process_type_reference>
		rpt(parse_and_create_complete_process_type(
			cf.named_process_type.c_str(), *the_module));
	if (!rpt) {
		return 1;		// already have error message
	}
	top_module = count_ptr<module>(new module("<auxiliary>"));
	NEVER_NULL(top_module);
	if (!top_module->allocate_unique_process_type(*rpt).good) {
		cerr << alloc_errstr << endl;
		return 1;
	}
} else {
	if (!the_module->allocate_unique().good) {
		cerr << alloc_errstr << endl;
		return 1;
	}
	top_module = the_module;
}	// end if use_referenced_type_instead_of_top_level
	// based on mode, set the options to pass into the module.
	if (!top_module->cflat(cout, cf).good) {
		cerr << "Error during cflat." << endl;
		return 1;
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
cflat::parse_command_options(const int argc, char* argv[], options& cf) {
	// using simple template function
	return master_options(argc, argv, cf);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Parses -f <mode> options.
 */
void
cflat::getopt_f_options(options& opt, const char* optarg) {
	typedef options_modifier_map_type::mapped_type		mapped_type;
	typedef options_modifier_map_type::const_iterator	const_iterator;
	const const_iterator mi(options_modifier_map.find(optarg));
	if (mi == options_modifier_map.end() || !mi->second) {
		// cerr << "Invalid mode: " << optarg << endl;
		string err("Invalid mode: ");
		err += optarg;
		throw util::getopt_exception(1, err);
	} else {
		const mapped_type& om(mi->second);
		om(opt);	// process option
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets cflat mode to use single anonymous instance of type, 
	ignoring top-level instance hierarchy.  
 */
void
cflat::getopt_cflat_type_only(options& opt, const char* optarg) {
if (opt.use_referenced_type_instead_of_top_level) {
	throw util::getopt_exception(1, "Cannot specify more than one type.");
} else {
	opt.use_referenced_type_instead_of_top_level = true;
	opt.named_process_type = optarg;	// strcpy
	// cerr << "optarg = " << opt.named_process_type << endl;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-c.texi
@defopt -c
Indicate that input file is source, as opposed to an object file,
and needs to be compiled.
@end defopt
@end texinfo
***/
static
void
__cflat_getopt_c(cflat_options& opt) {
	parse_create_flag('c', opt.comp_opt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cflat/opt-C-upper.texi
@defopt -C opts
When compiling input source, forward options @var{opt} to the compiler driver.
@end defopt
@end texinfo
***/
static
void
__cflat_getopt_compile_flags(cflat_options& opt, const char* optarg) {
	parse_create_flag('C', opt.comp_opt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
cflat::initialize_master_options_map(void) {
	master_options.add_option('c', &__cflat_getopt_c);
	master_options.add_option('C', &__cflat_getopt_compile_flags);
	master_options.add_option('f', &getopt_f_options);
	master_options.add_option('t', &getopt_cflat_type_only);
//	master_options.add_option('p', &getopt_cflat_type_only);
//	*someone* else uses '-p' instead...
	return 1;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: automate help.  
 */
void
cflat::usage(void) {
	cerr << "usage: " << name << " <mode> [options] <hackt-obj-infile>"
		<< endl;
	cerr << "options: " << endl;
	cerr << "\t-c : input file is a source, to be compiled\n";
	cerr << "\t-C <opts> : flags to forward to compiler\n";
	cerr << "\t-t \"type\" : perform flattening on an instance of the named type,\n"
		"\t\tignoring top-level instances (quotes recommended)." << endl;
	cerr << "\t-f <mode> : applies mode-preset or individual flag modifier"
		" (repeatable)" << endl;
	// list modes
	const size_t modes = options_modifier_map.size();
if (modes) {
	cerr << "Modes and modifier-flags (" << modes << " total):" << endl;
#if 0
	typedef	options_modifier_map_type::const_iterator	const_iterator;
	const_iterator i(options_modifier_map.begin());
	const const_iterator e(options_modifier_map.end());
	for ( ; i!=e; i++) {
		cerr << '\t' << i->first << " : " << i->second.brief << endl;
	}
#else
	dump_options_briefs(cerr);
#endif
}
	// additional options summary
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// cflat::print_alias relocated to "Object/inst/alias_printer.cc"

//=============================================================================
}	// end namespace HAC

#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::cflat::main(argc, argv, g);
}
#endif	// WITH_MAIN

DEFAULT_STATIC_TRACE_END

