/**
	\file "main/chpsim.cc"
	Main module for new CHPSIM.
	$Id: chpsim.cc,v 1.1.72.1 2006/12/05 01:49:24 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#include "util/static_trace.h"

DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <list>
#include "main/chpsim.h"
#include "main/program_registry.h"	// to register with hackt's dispatcher
#include "main/main_funcs.h"		// for save/load_module()
#include "main/options_modifier.tcc"
#include "sim/chpsim/State.h"
#include "util/getopt_mapped.h"		// for getopt()
#include "util/memory/excl_ptr.h"	// for never_ptr

namespace HAC {
#include "util/using_ostream.h"
using util::memory::excl_ptr;
using util::memory::never_ptr;
using SIM::CHPSIM::State;

//=============================================================================
/**
	Has a lot in common with prsim_options (main/prsim.cc)
 */
class chpsim_options {
public:
	/// just print help and exit
	bool				help_only;
	/// interactive vs. batch mode
	bool				interactive;
	/// whether or not to run, or just run some other diagnostics
	bool				run;

	/// list of paths to search for sourced scripts
	typedef	std::list<string>	source_paths_type;
	source_paths_type		source_paths;

	chpsim_options() : help_only(false), interactive(true), 
		run(true), source_paths() { }
};	// end class chpsim_options

//=============================================================================
// class chpsim static initializers

const char
chpsim::name[] = "chpsim";

const char
chpsim::brief_str[] = "An event-driven CHP simulator";

const size_t
chpsim::program_id = register_hackt_program_class<chpsim>();

//=============================================================================
// class chpsim member definitions

chpsim::chpsim() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Add your own getopt(), parse_command_options()...
 */
int
chpsim::main(int argc, char* argv[], const global_options&) {
	options opt;
	if (parse_command_options(argc, argv, opt)) {
		cerr << "Error in command invocation." << endl;
		usage();
		return 1;
	}
	if (opt.help_only) {
		cerr << "TODO: print chpsim interactive help." << endl;
		return 0;
	}
	if (optind+1 != argc) {
		cerr << "Error: Exactly one non-option argument "
			"allowed/required." << endl;
		usage();
		return 1;
	}
	const char* const ofn = argv[optind];
	if (!check_object_loadable(ofn).good)
		return 1;
	excl_ptr<module> the_module = load_module(ofn);
		// load_module_debug(ofn);
	if (!the_module)
		return 1;
//	the_module->dump(cerr);
	// automatically compile as far as needed:
	if (!the_module->is_allocated()) {
		if (!the_module->allocate_unique().good) {
			cerr << "ERROR in allocating.  Aborting." << endl;
			return 1;
		}
	}
try {
	State sim_state(*the_module);		// may throw
	// signal handler
	if (opt.run) {
		sim_state.initialize();
		// run command interpreter
		// return error if necessary
	}
} catch (...) {
	cerr << "Caught exception during construction of simulator state."
		<< endl;
	return 1;
}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use getopt_mapped(), see cflat for example.
 */
int
chpsim::parse_command_options(const int argc, char* argv[], options& o) {
	static const char optstring[] = "+bhi";
	int c;
while((c = getopt(argc, argv, optstring)) != -1) {
switch (c) {
	case 'b':
		o.interactive = false;
		break;
	case 'h':
		o.help_only = true;
		break;
	case 'i':
		o.interactive = true;
	case ':':
		cerr << "Expected but missing option-argument." << endl;
		return 1;
	case '?':
		util::unknown_option(cerr, optopt);
		return 1;
	default:
		THROW_EXIT;
}	// end switch
}	// end while
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chpsim::usage(void) {
	cerr << "usage: " << name << " <hackt-obj-file>" << endl;
}

//=============================================================================
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

