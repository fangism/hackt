/**
	\file "main/hacknet.cc"
	Traditional netlist generator.
	$Id: hacknet.cc,v 1.1.2.5 2009/08/27 20:38:44 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
// #include <cstring>
#include <list>

#include "net/netgraph.h"		// for netlist_generator
#include "main/hacknet.h"
#include "main/program_registry.h"
#include "main/main_funcs.h"
#if 0
#include "main/options_modifier.tcc"
#endif
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
using util::optparse;
using util::optparse_list;
using util::optparse_file;
using util::option_value_list;
#include "util/using_ostream.h"

//=============================================================================
#if 0
// explicit instantiation for proper initialization ordering
// (options modifier registry map initialized before use)
// accepted by g++-3.3, but not 4
template class options_modifier_policy<hacknet_options>;
#endif

//=============================================================================
class hacknet_options {
public:
	/**
		Un-processed option values list from file/command-line.
	 */
	option_value_list	raw_opts;
	/**
		Set to true to just exit after command parsing.  
	 */
	bool			help_only;
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
		The string of the complete process type to process
		in lieu of the top-level instance hierarchy.  
	 */
	string			named_process_type;
	/// compiler-driver flags
	compile_options		comp_opt;
	/// options for the netlist generator
	NET::netlist_options	net_opt;

	hacknet_options() :
		raw_opts(), 
		help_only(false),
		use_referenced_type_instead_of_top_level(false),
		named_process_type(),
		comp_opt(),
		net_opt()
		{ }

	bool
	finalize(void) {
		return net_opt.set(raw_opts);
	}

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
//	cerr << "options:\n" << opt.raw_opts << endl;	// DEBUG
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
	// process netlist options and check for errors
	if (opt.finalize()) {
		return 1;
	}

	static const char alloc_errstr[] = 
		"ERROR in allocating global state.  Aborting.";
	// inspired by hflat flag
	count_ptr<module> top_module;
if (opt.use_referenced_type_instead_of_top_level) {
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
	if (!top_module->allocate_unique_process_type(*rpt, *the_module).good) {
		cerr << alloc_errstr << endl;
		return 1;
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
	top_module->populate_top_footprint_frame();
	// the simulator state object, initialized with the module
try {
	const module& top_module_c(AS_A(const module&, *top_module));
	const entity::footprint& topfp(top_module_c.get_footprint());
	NET::netlist_generator n(top_module->get_state_manager(), topfp, 
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
	TODO: more options!
	TODO: use getopt_mapped(), see cflat for examples.  
 */
int
hacknet::parse_command_options(const int argc, char* argv[], options& o) {
	// now we're adding our own flags
	static const char optstring[] = "+c:f:hHt:";
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
@end defopt
@end texinfo
***/
		case 'c': {
			std::ifstream f(optarg);
			if (f) {
				const option_value_list t(optparse_file(f));
				o.raw_opts.insert(o.raw_opts.end(), 
					t.begin(), t.end());
				// any error handling here?
			} else {
				cerr << "Error opening options file \"" <<
					optarg << "\" for reading." << endl;
				return 2;
			}
			break;
		}
/***
@texinfo opt/option-f.texi
@defopt -f options...
Parse configuration options from @var{options}.
Options are of the same format as in the configuration
file, see option @option{-c}.  
Options are space-separated instead of newline-separated.  
This option is repeatable and cumulative.
@end defopt
@end texinfo
***/
		case 'f': {
			const option_value_list t(optparse_list(optarg));
			o.raw_opts.insert(o.raw_opts.end(), 
				t.begin(), t.end());
			// any error handling here?
			break;
		}
/***
@texinfo opt/option-h.texi
@defopt -h
Help.  Print usage and exit.
@end defopt
@end texinfo
***/
// TODO: option-help
		case 'h':
			o.help_only = true;
			usage();
			break;
/***
@texinfo opt/option-H-upper.texi
@defopt -H
Help.  Print usage and exit.
@end defopt
@end texinfo
***/
		case 'H':
			o.help_only = true;
			cerr << "TODO: print netlist_options help" << endl;
			break;
/***
@texinfo opt/option-t.texi
@defopt -t type
Instead of using the top-level instances in the source file, 
instantiate one instance of the named @var{type}, propagating its
ports as top-level globals.  
In other words, use the referenced type as the top-level scope, 
ignoring the source's top-level instances.  
Convenient takes palce of copy-propagating a single instance's ports.  
@end defopt
@end texinfo
***/
		case 't':
			if (o.use_referenced_type_instead_of_top_level) {
				cerr << "Cannot specify more than one type."
					<< endl;
				return 1;
			} else {
				o.use_referenced_type_instead_of_top_level = true;
				o.named_process_type = optarg;        // strcpy
			}
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
	cerr << "usage: " << name << " [options] <hackt-objfile>" << endl;
	cerr << "options:\n"
"\t-c file : process configuration options file (repeatable)\n"
"\t-f \"option=value ...\" : set option values (repeatable)\n"
"\t\tseparate multiple options with space\n"
"\t-h : print this usage help\n"
"\t-H : print options help\n"
"\t-t \"type\" : generate netlist for the named type,\n"
"\t\tignoring top-level instances (quotes recommended)."
	<< endl;
#if 0
        const size_t flags = options_modifier_map.size();
	if (flags) {
		cerr << "flags (" << flags << " total):" << endl;
		dump_options_briefs(cerr);
	}
#endif
	cerr << "Additional documentation is installed in:\n"
	"\t`info hacknet' (finds " INFODIR "/hacknet.info)\n"
	"\tPDF: " PDFDIR "/hacknet.pdf\n"
	"\tPS: " PSDIR "/hacknet.ps\n"
	"\tHTML: " HTMLDIR "/hacknet.html/index.html" << endl;
}

//-----------------------------------------------------------------------------
// hacknet_option modifier functions and their flag registrations
#if 0
static void __hacknet_default(hacknet_options& o) { o = hacknet_options(); }
#endif

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

