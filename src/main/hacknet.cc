/**
	\file "main/hacknet.cc"
	Traditional netlist generator.
	$Id: hacknet.cc,v 1.1.2.2 2009/08/10 22:31:23 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
#include <cstring>
#include <list>

#include "net/netgraph.h"		// for netlist_generator
#include "main/hacknet.h"
#include "main/program_registry.h"
#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "main/options_modifier.tcc"
#include "main/simple_options.tcc"
#include "main/global_options.h"
#include "main/compile_options.h"
#include "util/getopt_mapped.h"
#include "util/persistent_object_manager.h"
// #include "util/string.tcc"	// for string_to_num
#include "install_paths.h"

namespace HAC {
using std::string;
using util::persistent;
using util::persistent_object_manager;
// using util::strings::string_to_num;

#include "util/using_ostream.h"

//=============================================================================
// explicit instantiation for proper initialization ordering
// (options modifier registry map initialized before use)
// accepted by g++-3.3, but not 4
template class options_modifier_policy<hacknet_options>;

//=============================================================================
class hacknet_options {
public:
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
		help_only(false),
		use_referenced_type_instead_of_top_level(false),
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
	if (parse_command_options(argc, argv, opt)) {
		cerr << "Error in command invocation." << endl;
		usage();
		return 1;
	}
	if (opt.help_only) {
		// TODO: print help
		return 0;
	}
	if (optind+1 != argc) {
		cerr << "Error: Exactly one non-option argument "
			"allowed/required." << endl;
		usage();
		return 1;
	}
	const char* const ofn = argv[optind];
	// cerr << "loading " << ofn << endl;
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
if (opt.use_referenced_type_instead_of_top_level) {
	const count_ptr<const process_type_reference>
		rpt(parse_and_create_complete_process_type(
			opt.named_process_type.c_str(), *the_module));
	if (!rpt) {
		return 1;		// already have error message
	}
	top_module = count_ptr<module>(new module("<auxiliary>"));
	NEVER_NULL(top_module);
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
	static const char optstring[] = "+ct:";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
/***
@texinfo opt/option-c.texi
@defopt -c
bleh...
@end defopt
@end texinfo
***/
		case 'c':
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
	cerr << "options:\n";
        const size_t flags = options_modifier_map.size();
	if (flags) {
		cerr << "flags (" << flags << " total):" << endl;
		dump_options_briefs(cerr);
	}
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

