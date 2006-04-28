/**
	\file "main/cflat.cc"
	cflat backwards compability module.  

	$Id: cflat.cc,v 1.10.6.1 2006/04/28 03:08:16 fang Exp $
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
#include "main/simple_options.tcc"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.h"

namespace HAC {
using std::string;
using util::persistent;
using util::persistent_object_manager;
#include "util/using_ostream.h"

//=============================================================================
// explicit early class instantiation for proper static initializer ordering
// this guarantees the registry map is initialized before anything is registered

// preferred by g++-3.3, but rejected by g++-4
// template class cflat::options_modifier_policy_type;
// accepted by both :S (hate having to copy exact name...)
template class options_modifier_policy<cflat_options>;

//=============================================================================
// class cflat static initializers

const char
cflat::name[] = "cflat";

const char
cflat::brief_str[] =
	"Flattens instantiations CAST-style (backward compatibility)";

STATIC_TRACE_HERE("before cflat registry")

const size_t
cflat::program_id = register_hackt_program_class<cflat>();

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
	cf.size_prs = false;
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__cflat_connect(cflat::options& cf) {
	// cf.primary_tool = ?
	// cf.tool_options = cflat::options::TOOL_OPTIONS_DEFAULT;
	cf.connect_style = cflat::options::CONNECT_STYLE_CONNECT;
	cf.include_prs = false;
	cf.dump_self_connect = false;
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
}

// end of primary modes
//=============================================================================
STATIC_TRACE_HERE("before cflat option modifiers")
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const cflat::register_options_modifier
	cflat::_prsim("prsim", &__cflat_prsim);

STATIC_TRACE_HERE("after first cflat option modifier")

const cflat::register_options_modifier
	cflat::_lvs("lvs", &__cflat_lvs);

const cflat::register_options_modifier
	cflat::_LVS("LVS", &__cflat_lvs);		// re-use lvs

const cflat::register_options_modifier
	cflat::_ergen("ergen", &__cflat_lvs);		// re-use lvs

const cflat::register_options_modifier
	cflat::_alint("alint", &__cflat_lvs);		// re-use lvs

const cflat::register_options_modifier
	cflat::_prlint("prlint", &__cflat_prlint);

const cflat::register_options_modifier
	cflat::_prs2tau("prs2tau", &__cflat_lvs);	// re-use lvs

const cflat::register_options_modifier
	cflat::_connect("connect", &__cflat_connect);

const cflat::register_options_modifier
	cflat::_check("check", &__cflat_check);

const cflat::register_options_modifier
	cflat::_wire("wire", &__cflat_wire);

const cflat::register_options_modifier
	cflat::_aspice("aspice", &__cflat_wire);	// re-use wire

const cflat::register_options_modifier
	cflat::_Aspice("Aspice", &__cflat_wire);	// re-use wire

const cflat::register_options_modifier
	cflat::_ADspice("ADspice", &__cflat_ADspice);

const cflat::register_options_modifier
	cflat::_default("default", &__cflat_default);

//-----------------------------------------------------------------------------
// for setting/unsetting individual flags

// TODO: reperitive code should be templated or macro'd
// define for common patterns to reduce clutter.

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

static
void
__cflat_connect_equal(cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_EQUAL;
}
const cflat::register_options_modifier
	cflat::_connect_equal("connect-equal", &__cflat_connect_equal, 
		"alias-style: = x y");

static
void
__cflat_connect_connect(cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_CONNECT;
}
const cflat::register_options_modifier
	cflat::_connect_connect("connect-connect", &__cflat_connect_connect,
		"alias-style: connect x y");

static
void
__cflat_connect_wire(cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_WIRE;
}
const cflat::register_options_modifier
	cflat::_connect_wire("connect-wire", &__cflat_connect_wire,
		"alias-style: wire x y");

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

	if (!check_object_loadable(ofn).good)
		return 1;

	excl_ptr<module> the_module = load_module(ofn);
	if (!the_module)
		return 1;

	if (!the_module->allocate_unique().good) {
		cerr << "ERROR in allocating global state.  Aborting." << endl;
		return 1;
	}

	// cflat here!!!
	// based on mode, set the options to pass into the module.
	if (!the_module->cflat(cout, cf).good) {
		cerr << "Error during cflat." << endl;
		return 1;
	}

	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
cflat::parse_command_options(const int argc, char* argv[], options& cf) {
	// using simple template function
	return parse_simple_command_options(
		argc, argv, cf, options_modifier_map);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat::usage(void) {
	cerr << "usage: " << name << " <mode> [options] <hackt-obj-infile>"
		<< endl;
	cerr << "options: " << endl;
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
/**
	Prints an alias as specified by the flags.  
	Publicly accessible.  
 */
void
cflat::print_alias(ostream& o, const string& canonical, const string& alias,
		const options& cf) {
if (cf.dump_self_connect || alias != canonical) {
	switch (cf.connect_style) {
		case cflat_options::CONNECT_STYLE_CONNECT:
			o << "connect ";
			break;  
		case cflat_options::CONNECT_STYLE_EQUAL:
			o << "= ";
			break;  
		case cflat_options::CONNECT_STYLE_WIRE:
			o << "wire ";
			break;  
		default:
			o << "alias ";
			break;  
	}       
	if (cf.enquote_names) {
		o << '\"' << canonical << "\" \"" << alias << '\"';
	} else {
		o << canonical << ' ' << alias;
	}       
	o << endl;
}       
}

//=============================================================================
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

