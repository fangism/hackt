/**
	\file "main/cflat.cc"
	cflat backwards compability module.  

	$Id: cflat.cc,v 1.4 2005/11/03 07:52:06 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

// #include "util/static_trace.h"

#include <iostream>
#include <string>

#include "main/cflat.h"
#include "main/cflat_options.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "util/stacktrace.h"
#include "util/persistent_object_manager.h"
#include "util/qmap.tcc"
#include "util/getopt_portable.h"

namespace ART {
using std::string;
using util::persistent;
using util::persistent_object_manager;
#include "util/using_ostream.h"

//=============================================================================
struct cflat::options_modifier_info {
	options_modifier		op;
	string				brief;

	options_modifier_info() : op(NULL), brief() { }

	options_modifier_info(const options_modifier o, const char* b) :
		op(o), brief(b) { }

	operator bool () const { return op; }

	void
	operator () (cflat_options& cf) const {
		NEVER_NULL(op);
		(*op)(cf);
	}
};	// end struct options_modifier_info

//=============================================================================
// class cflat static initializers

const char
cflat::name[] = "cflat";

const char
cflat::brief_str[] =
	"Flattens instantiations CAST-style (backward compatibility)";

const size_t
cflat::program_id = register_hackt_program_class<cflat>();

const cflat::options_modifier_map_type
cflat::options_modifier_map;

//=============================================================================
static const char default_options_brief[] = "(CAST cflat preset)";
/**
	CFLAT mode registration mechanism.  
 */
class cflat::register_options_modifier {
public:
	register_options_modifier(const string& Mode,
			const cflat::options_modifier COM, 
			const char* b = default_options_brief) {
		options_modifier_map_type&
			omm(const_cast<options_modifier_map_type&>(
				options_modifier_map));
		NEVER_NULL(COM);
		options_modifier_info& i(omm[Mode]);
		INVARIANT(!i);
		i.op = COM;
		i.brief = b;
	}
};	// end class register_options_modifier

//=============================================================================
// cflat::options_modifier declarations and definitions

/// Does absolutely nothing.  
static
void
__cflat_default(cflat::options&) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// preset modes
static
void
__cflat_prsim(cflat::options& cf) {
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__cflat_prlint(cflat::options& cf) {
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__cflat_connect(cflat::options& cf) {
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
}
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Also works for:
	aspice, Aspice
 */
static
void
__cflat_wire(cflat::options& cf) {
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__cflat_ADspice(cflat::options& cf) {
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just check, don't print.
 */
static
void
__cflat_check(cflat::options& cf) {
	cf.connect_style = cflat::options::CONNECT_STYLE_NONE;
	cf.include_prs = true;
	cf.check_prs = true;
	cf.wire_mode = false;
	cf.csim_style_prs = false;
	cf.dsim_prs = false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const cflat::register_options_modifier
	cflat::_prsim("prsim", &__cflat_prsim);

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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// for setting/unsetting individual flags

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

//	cf.dump_non_bools = false;
//	cf.namespace_policy = cflat::options::NAMESPACE_POLICY_NONE;

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
	const options_modifier_info om(options_modifier_map[mode]);
	if (!om) {
		cerr << "Invalid mode: " << mode << endl;
		usage();
		return 1;
	} else {
		om(cf);
	}

	// additional mode modifiers
	// shift arguments by one to pass over mode
	if (parse_command_options(argc-1, argv+1, cf)) {
		cerr << "Error in command invocation." << endl;
		usage();
		return 1;
	}
	// cerr << "optind = " << optind << endl;
	INVARIANT(optind+1 < argc);
	if (optind+2 < argc) {
		cerr << "Only one non-option argument allowed." << endl;
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
	static const char* optstring = "+f:";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
		case 'f': {
			const options_modifier_info
				om(options_modifier_map[optarg]);
			if (!om) {
				cerr << "Invalid mode: " << optarg << endl;
				return 1;
			} else {
				om(cf);
			}
			break;
		}
		case ':':
			cerr << "Expected but missing option-argument." << endl;
			return 1;
		case '?':
			unknown_option(optopt);
			return 1;
		default:
			abort();
	}	// end switch
	}	// end while
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
cflat::usage(void) {
	cerr << "Usage: " << name << " <mode> [options] <hackt-obj-infile>"
		<< endl;
	cerr << "Options: " << endl;
	cerr << "\t-f <mode> : applies mode-preset or individual flag modifier"
		" (repeatable)" << endl;
	// list modes
	const size_t modes = options_modifier_map.size();
if (modes) {
	cerr << "Modes and modifier-flags (" << modes << " total):" << endl;
	typedef	options_modifier_map_type::const_iterator	const_iterator;
	const_iterator i(options_modifier_map.begin());
	const const_iterator e(options_modifier_map.end());
	for ( ; i!=e; i++) {
		cerr << '\t' << i->first << " : " << i->second.brief << endl;
	}
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
}	// end namespace ART

