/**
	\file "main/chpsim-guile.cc"
	Main module for new CHPSIM guile interface.
	$Id: chpsim-guile.cc,v 1.2.2.2 2007/03/25 02:25:40 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#include "util/static_trace.h"

DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include "main/chpsim-guile.h"
#include "main/chpsim.h"
#include "main/chpsim_options.h"
#include "main/program_registry.h"	// to register with hackt's dispatcher
#include "main/main_funcs.h"		// for save/load_module()
#include "main/global_options.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "sim/chpsim/State.h"
#include "sim/chpsim/graph_options.h"
#include "guile/libhackt-wrap.h"
#include "guile/chpsim-wrap.h"
#include "util/libguile.h"

namespace HAC {
#include "util/using_ostream.h"
using SIM::CHPSIM::State;
// using SIM::CHPSIM::CommandRegistry;
using SIM::CHPSIM::graph_options;
using entity::canonical_fundamental_chan_type_base;
using guile_wrap::obj_module;
using guile_wrap::chpsim_state;

//=============================================================================
// class chpsim_guile member definitions

chpsim_guile::chpsim_guile() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__guile_main(int argc, char* argv[]) {
#if 0
	libhacktsim_guile_init();
#else
	// definitions loaded into modules, then load the module
	scm_init_hackt_libhackt_primitives_module();
	scm_init_hackt_chpsim_primitives_module();
	scm_init_hackt_chpsim_trace_primitives_module();
	scm_c_use_module("hackt chpsim-trace-primitives");
#endif
	scm_shell(argc, argv);	// no-return
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
chpsim_guile::main_interactive(void* closure, int argc, char** argv) {
	cout << "Welcome to hackt-chpsim-guile!" << endl;
	scm_c_eval_string("(set-repl-prompt! \"hacchpsimguile> \")");
	__guile_main(argc, argv);	// no-return
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expect script from stdin only.  
 */
void
chpsim_guile::main_script(void* closure, int argc, char** argv) {
	// suppress prompt
	scm_c_eval_string("(set-repl-prompt! \"\")");
	__guile_main(argc, argv);	// no-return
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Add your own getopt(), parse_command_options()...
 */
int
chpsim_guile::main(int argc, char* argv[], const global_options&) {
	options opt;
	if (chpsim::parse_command_options(argc, argv, opt)) {
		cerr << "Error in command invocation." << endl;
		chpsim::usage();
		return 1;
	}
	if (opt.dump_checkpoint) {
		// dump checkpoint only, doesn't run
		return 0;
	}
#if 0
	if (opt.help_only) {
		util::string_list args;
		args.push_back("help");
		args.push_back("all");
		SIM::CHPSIM::Help::main(args);
		return 0;
	}
#endif
	if (optind+1 != argc) {
		cerr << "Error: Exactly one non-option argument "
			"allowed/required." << endl;
		chpsim::usage();
		return 1;
	}
	const char* const ofn = argv[optind];
	if (!check_object_loadable(ofn).good)
		return 1;
	obj_module = load_module(ofn);
		// load_module_debug(ofn);
	if (!obj_module)
		return 1;
//	the_module->dump(cerr);
	// automatically compile as far as needed:
	if (!obj_module->is_allocated()) {
		if (!obj_module->allocate_unique().good) {
			cerr << "ERROR in allocating.  Aborting." << endl;
			return 1;
		}
	}
try {
	// first, cache all built-in channel types' summaries
	canonical_fundamental_chan_type_base::refresh_all_footprints();
	chpsim_state = count_ptr<State>(new State(*obj_module));
		// may throw
	NEVER_NULL(chpsim_state);
	// install interrupt signal handler
	// do we really need this?
	const State::signal_handler int_handler(&*chpsim_state);
#if 0
	if (opt.dump_graph_alloc)
		sim_state.dump_struct(cout) << endl;
#endif
#if 0
	if (opt.check_structure)
		sim_state.check_structure();
#endif
#if 0
	if (opt.dump_dot_struct)
		sim_state.dump_struct_dot(cout, opt.graph_opts) << endl;
#endif
	// don't really need
	chpsim_state->import_source_paths(opt.source_paths);
	// don't run anything
#if 0
	if (opt.run) {
		sim_state.initialize();
		// run command interpreter
		// return error if necessary
		const int ret = CommandRegistry::interpret(sim_state, 
			opt.interactive);
		if (ret)	return ret;
	}
#endif
	// pass control over to guile
	if (opt.interactive) {
		scm_boot_guile(argc -optind, argv +optind,
			main_interactive, NULL);
	} else {
		scm_boot_guile(argc -optind, argv +optind,
			main_script, NULL);
	}
	// never returns...
} catch (...) {
	cerr << "Caught exception during construction of simulator state."
		<< endl;
	return 1;
}
	return 0;
}

//=============================================================================
}	// end namespace HAC

/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::chpsim_guile::main(argc, argv, g);
}

DEFAULT_STATIC_TRACE_END

