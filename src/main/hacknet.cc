/**
	\file "main/hacknet.cc"
	Traditional netlist generator.
	$Id: hacknet.cc,v 1.6.2.2 2010/01/13 17:43:41 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
#include <list>

#include "net/netlist_options.h"
#include "net/netlist_generator.h"
#include "common/config.h"
#include "main/hacknet.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#include "main/simple_options.tcc"
#include "main/global_options.h"
#include "main/compile_options.h"
#include "util/stacktrace.h"
#include "util/getopt_mapped.h"
#include "util/persistent_object_manager.h"
#include "util/optparse.h"
#include "install_paths.h"

namespace HAC {
using std::string;
using util::persistent;
using util::persistent_object_manager;
using util::option_value;
using NET::netlist_options;
#include "util/using_ostream.h"

//=============================================================================
class hacknet_options {
public:
	/**
		Set to true to just exit after command parsing.  
	 */
	bool			help_only;
	/**
		If true, print values of options.  
	 */
	bool			dump_config;
// many options copied from prsim.cc
	/**
		Copied from cflat_options.
		Ignore top-level instances and flatten one anonymous
		instance of a named complete process type.  
		Flag to do 'cast2lvs'-like behavior.  
		Tip: protect argument with "quotes" in command-line
			to protect shell-characters.  
	 */
	bool			use_referenced_type_instead_of_top_level;
	/**
		Use the named type, but instantiate the process, 
		not its contents into the top-level.  
		This option should be mutually exclusive with the previous.
		This also implies net_opt.emit_top=0.
	 */
	bool			instantiate_one_of_referenced_type;
	/**
		The string of the complete process type to process
		in lieu of the top-level instance hierarchy.  
	 */
	string			named_process_type;
	/// compiler-driver flags
	compile_options		comp_opt;
	/// options for the netlist generator
	netlist_options		net_opt;

	hacknet_options() :
		help_only(false),
		dump_config(false),
		use_referenced_type_instead_of_top_level(false),
		instantiate_one_of_referenced_type(false),
		named_process_type(),
		comp_opt(),
		net_opt()
		{ }

};	// end class hacknet_options

//=============================================================================
// class alloc static initializers

const char
hacknet::name[] = "hacknet";

const char
hacknet::brief_str[] = "SPICE netlist generator.";

#ifndef	WITH_MAIN
const size_t
hacknet::program_id = register_hackt_program_class<hacknet>();
#endif

//=============================================================================
hacknet::hacknet() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
hacknet::main(const int argc, char* argv[], const global_options&) {
	options opt;
	int opterr = parse_command_options(argc, argv, opt);
	if (opterr) {
		cerr << "Error in command invocation." << endl;
		if (opterr == 1) {
			usage();
		}
		// else is other error
		return opterr;
	}
	if (opt.dump_config) {
		opt.net_opt.dump(cout);
		return 0;
	}
	if (opt.help_only) {
		return 0;
	}
	if (optind+1 != argc) {
		cerr << "Error: Exactly one non-option argument "
			"allowed/required." << endl;
		usage();
		return 1;
	}
	const char* const ofn = argv[optind];
	count_ptr<module> the_module;
if (opt.comp_opt.compile_input) {
	the_module = parse_and_check(ofn, opt.comp_opt);
} else {
	if (!check_object_loadable(ofn).good)
		return 1;
	the_module = load_module(ofn);
}
	if (!the_module)
		return 1;

	static const char alloc_errstr[] = 
		"ERROR in allocating global state.  Aborting.";
	// inspired by hflat flag
	count_ptr<module> top_module;
if (opt.use_referenced_type_instead_of_top_level ||
	opt.instantiate_one_of_referenced_type) {
	const count_ptr<const process_type_reference>
		rpt(parse_and_create_complete_process_type(
			opt.named_process_type.c_str(), *the_module));
	if (!rpt) {
		return 1;		// already have error message
	}
	top_module = count_ptr<module>(new module("<auxiliary>"));
	NEVER_NULL(top_module);
	// TODO: this should only require create-phase, not global alloc
	// will save HUGE memory on full-hierarchy
	// this can only be done after folding state_manager locally
	// into footprint.
if (opt.use_referenced_type_instead_of_top_level) {
	if (!top_module->allocate_unique_process_type(*rpt, *the_module).good) {
		cerr << alloc_errstr << endl;
		return 1;
	}
} else { 	// (opt.instantiate_one_of_referenced_type)
	if (!top_module->allocate_single_process(rpt).good) {
		cerr << alloc_errstr << endl;
		return 1;
	}
}
} else {
//	the_module->dump(cerr);
	if (the_module->is_allocated()) {
		// cerr << "Module is already allocated, skipping..." << endl;
	} else {
		if (!the_module->allocate_unique().good) {
			cerr << alloc_errstr << endl;
			return 1;
		}
//		the_module->dump(cerr);
	}
	top_module = the_module;
}
	// is this needed? yes, we traverse top-down
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	top_module->populate_top_footprint_frame();
#endif
	// the simulator state object, initialized with the module
try {
	opt.net_opt.commit();		// commit options
	const module& top_module_c(AS_A(const module&, *top_module));
	const entity::footprint& topfp(top_module_c.get_footprint());
	NET::netlist_generator n(
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
		top_module->get_state_manager(), 
#endif
		topfp, 
		cout, opt.net_opt);
	// process global-scope instances and rules
	n();
} catch (...) {
	cerr << "Caught exception during netlist generation."
		<< endl;
	return 1;
}
	// else just exit
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Handle command-line options.
 */
int
hacknet::parse_command_options(const int argc, char* argv[], options& o) {
	// now we're adding our own flags
	static const char optstring[] = "+c:C:df:hHI:t:T:v";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
/***
@texinfo opt/option-c.texi
@defopt -c file
Parse configuration options from @var{file}.
Options are of the form @t{key=values} with 
no space characters separating the @t{=}.
Values may be singleton, comma-separated, or omitted.
The same options can also be passed in through the command-line 
via @option{-f}.   
This option is repeatable and cumulative.
@xref{Configuration Options}.
@end defopt
@end texinfo
***/
		case 'c': {
			option_value v;
			v.key = "config_file";	// doesn't really matter
			v.values.push_back(optarg);
			if (o.net_opt.open_config_file(v)) {
				// already have error message
				return 2;
			}
			break;
		}
/***
@texinfo opt/option-C-upper.texi
@defopt -C file
Backwards compatibility option.
Parse configuration options from @var{file}.
Options are of the form @t{type key value}.
@var{type} may be @option{int}, @option{real}, or @option{string}.
Values are only singleton.  
The same options can also be passed in through the command-line 
via @option{-F}.   
This option is repeatable and cumulative.
@xref{Configuration Options}.
@end defopt
@end texinfo
***/
		case 'C': {
			option_value v;
			v.key = "config_file_compat";	// doesn't really matter
			v.values.push_back(optarg);
			if (o.net_opt.open_config_file_compat(v)) {
				// already have error message
				return 2;
			}
			break;
		}
/***
@texinfo opt/option-d.texi
@defopt -d
Print the values of all configuration values to @file{stdout} and exits.
@end defopt
@end texinfo
***/
		case 'd':
			o.dump_config = true;
			break;
/***
@texinfo opt/option-f.texi
@defopt -f options...
Parse configuration options from @var{options}.
Options are of the same key-value format as in the configuration
file, see option @option{-c}.  
Options are space-separated instead of newline-separated.  
This option is repeatable and cumulative.
@xref{Configuration Options}.
@end defopt
@end texinfo
***/
		case 'f': {
			if (o.net_opt.set(util::optparse_list(optarg)))
				return 2;
			break;
		}
/***
@texinfo opt/option-F-upper.texi
@defopt -F option
Backwards compatibility option.
Parse configuration options from @var{options}.
Options are of the same key-value format as in the configuration
file, see option @option{-C}.  
Unlike @option{-f} option, only one parameter can be specified at a time.
This option is repeatable and cumulative.
@xref{Configuration Options}.
@end defopt
@end texinfo
***/
		case 'F': {
			if (o.net_opt.set(util::optparse_compat(optarg)))
				return 2;
			break;
		}
/***
@texinfo opt/option-h.texi
@defopt -h
Help.  Print usage and exit.
@end defopt
@end texinfo
***/
		case 'h':
			o.help_only = true;
			usage();
			break;
/***
@texinfo opt/option-H-upper.texi
@defopt -H
Describe all configuration options with default values and exit.  
@xref{Configuration Options}.
See also the installed documentation for @file{hacknet.info,html,pdf}.
@end defopt
@end texinfo
***/
		case 'H':
			o.help_only = true;
			netlist_options::help(cerr);
			break;
/***
@texinfo opt/option-I-upper.texi
@defopt -I path
Append @var{path} to the list of paths to search for including
and referencing other config files.  
See also the @option{config_path} configuration option.
@end defopt
@end texinfo
***/
		case 'I':
			o.net_opt.file_manager.add_path(optarg);
			break;
/***
@texinfo opt/option-t.texi
@defopt -t type
Instead of using the top-level instances in the source file, 
instantiate one instance of the named @var{type}, propagating its
ports as top-level globals.  
In other words, use the referenced type as the top-level scope, 
ignoring the source's top-level instances.  
Convenient takes place of copy-propagating a single instance's ports.  
See also the @option{-T} option.  
@end defopt
@end texinfo
***/
		case 't':
			if (o.named_process_type.length()) {
				cerr << "Cannot specify more than one type."
					<< endl;
				return 1;
			} else {
				o.use_referenced_type_instead_of_top_level = true;
				o.named_process_type = optarg;        // strcpy
			}
			break;
/***
@texinfo opt/option-T-upper.texi
@defopt -T type
Instead of using the top-level instances in the source file, 
instantiate one lone instance of the named @var{type}, 
at the top-level, ignoring the source's top-level instances.  
Unlike the @option{-t} option, the contents of named type @var{type}
do not get unrolled directly into the top-level scope.
This instance's ports will be unconnected, 
and the instance's name is unspecified (because you shouldn't care!).
This option also implies @option{emit_top=0}.  
Current limitation: can only specify one type for now.
@end defopt
@end texinfo
***/
		case 'T':
			if (o.named_process_type.length()) {
				cerr << "Cannot specify more than one type."
					<< endl;
				return 1;
			} else {
				o.instantiate_one_of_referenced_type = true;
				o.named_process_type = optarg;        // strcpy
			}
			o.net_opt.emit_top = false;
			break;
/***
@texinfo opt/option-v.texi
@defopt -v
Print version and build information and exit.
@end defopt
@end texinfo
***/
		case 'v':
			config::dump_all(cout);
			exit(0);
			break;
		case ':':
			cerr << "Expected but missing option-argument." << endl;
			return 1;
		case '?':
			util::unknown_option(cerr, optopt);
			return 1;
		default:
			THROW_EXIT;
	}       // end switch
	}       // end while
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
hacknet::usage(void) {
	ostream& o(cerr);
	o << "usage: " << name << " [options] <hackt-objfile>" << endl;
	o << "options:\n"
"\t-c file : process configuration options file (repeatable)\n"
"\t-C file : process configuration options file, old-style (repeatable)\n"
"\t-d : print configuration values and exit\n"
"\t-f \"option=value(s) ...\" : set option values (repeatable)\n"
"\t\tmultiple values may be comma-separated\n"
"\t\tmultiple options may be space-separated\n"
"\t-F \"type option value\" : set option value, old-style (repeatable)\n"
"\t\ttype is [int|real|string], and string values are \"quoted\"\n"
"\t\tonly one option at a time (per -F flag)\n"
"\t-h : print this usage help and exit\n"
"\t-H : print configuration options help and exit\n"
"\t-I path : append search path for configuration files\n"
"\t-t \"type\" : generate netlist using the contents of the named type,\n"
"\t\tignoring top-level instances (quotes recommended).\n"
"\t-T \"type\" : make subcircuit definition library for the named type,\n"
"\t\tignoring top-level instances (quotes recommended).\n"
	"Additional documentation is installed in:\n"
	"\t`info hacknet' (finds " INFODIR "/hacknet.info)\n"
	"\tPDF: " PDFDIR "/hacknet.pdf\n"
	"\tPS: " PSDIR "/hacknet.ps\n"
	"\tHTML: " HTMLDIR "/hacknet.html/index.html" << endl;
}

//=============================================================================
}	// end namespace HAC

#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::hacknet::main(argc, argv, g);
}
#endif	// WITH_MAIN

DEFAULT_STATIC_TRACE_END

