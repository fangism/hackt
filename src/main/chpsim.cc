/**
	\file "main/chpsim.cc"
	Main module for new CHPSIM.
	$Id: chpsim.cc,v 1.1.72.5 2006/12/11 00:40:06 fang Exp $
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
#include "sim/chpsim/Command.h"
#include "sim/command_common.h"
#include "util/getopt_mapped.h"		// for getopt()
#include "util/memory/excl_ptr.h"	// for never_ptr

namespace HAC {
#include "util/using_ostream.h"
using util::memory::excl_ptr;
using util::memory::never_ptr;
using SIM::CHPSIM::State;
using SIM::CHPSIM::CommandRegistry;

//=============================================================================
template class options_modifier_policy<chpsim_options>;

//=============================================================================
/**
	Has a lot in common with chpsim_options (main/prsim.cc)
 */
class chpsim_options {
public:
	/// just print help and exit
	bool				help_only;
	/// interactive vs. batch mode
	bool				interactive;
	/// whether or not to run, or just run some other diagnostics
	bool				run;
	/// show result of graph allocation
	bool				dump_graph_alloc;
	/// check structure
	bool				check_structure;
	/// whether or not to produce dot graph output before running
	bool				dump_dot_struct;
	/// list of paths to search for sourced scripts
	typedef	std::list<string>	source_paths_type;
	source_paths_type		source_paths;

	chpsim_options() : help_only(false), interactive(true), 
		run(true), dump_graph_alloc(false), check_structure(true),
		dump_dot_struct(false), source_paths() { }
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
#if 0
	if (opt.dump_checkpoint) {
		// dump checkpoint only, doesn't run
		return 0;
	}
#endif
	if (opt.help_only) {
		util::string_list args;
		args.push_back("help");
		args.push_back("all");
		SIM::CHPSIM::Help::main(args);
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
	const excl_ptr<module> the_module = load_module(ofn);
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
	// install interrupt signal handler
	const State::signal_handler int_handler(&sim_state);
	if (opt.dump_graph_alloc)
		sim_state.dump_struct(cout) << endl;
#if 0
	if (opt.check_structure)
		sim_state.check_structure();
	if (opt.dump_dot_struct)
		sim_state.dump_struct_dot(cout) << endl;
#endif
	sim_state.import_source_paths(opt.source_paths);
	if (opt.run) {
		sim_state.initialize();
		// run command interpreter
		// return error if necessary
		const int ret = CommandRegistry::interpret(sim_state, 
			opt.interactive);
		if (ret)	return ret;
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
	static const char optstring[] = "+bf:hiI:";
	int c;
while((c = getopt(argc, argv, optstring)) != -1) {
switch (c) {
	case 'b':
		o.interactive = false;
		break;
	case 'f': {
		const options_modifier_map_iterator
			mi(options_modifier_map.find(optarg));
		if (mi == options_modifier_map.end() || !mi->second) {
			cerr << "Invalid option argument: " << optarg << endl;
			return 1;
		} else {
			(mi->second)(o);
		}
		break;
	}
	case 'h':
		o.help_only = true;
		break;
	case 'i':
		o.interactive = true;
	case 'I':
		o.source_paths.push_back(optarg);
		break;
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
	cerr << "options:" << endl;
	cerr << "\t-b : batch-mode, non-interactive (promptless)" << endl;
//	cerr << "\t-d <checkpoint>: textual dump of checkpoint only" << endl;
	cerr << "\t-f <flag> : general options modifiers (listed below)" << endl;
	cerr << "\t-h : print commands help and exit (objfile optional)" << endl;
	cerr << "\t-i : interactive (default)" << endl;
	cerr << "\t-I <path> : include path for scripts (repeatable)" << endl;
//	cerr << "\t-O <0..1> : expression optimization level" << endl;
        const size_t flags = options_modifier_map.size();
	if (flags) {
		cerr << "flags (" << flags << " total):" << endl;
		dump_options_briefs(cerr);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static void __chpsim_default(chpsim_options& o) { o = chpsim_options(); }

static void __chpsim_run(chpsim_options& o) { o.run = true; }
static void __chpsim_no_run(chpsim_options& o) { o.run = false; }
static void __chpsim_dump_graph_alloc(chpsim_options& o)
	{ o.dump_graph_alloc = true; }
static void __chpsim_no_dump_graph_alloc(chpsim_options& o)
	{ o.dump_graph_alloc = false; }
static void __chpsim_check_structure(chpsim_options& o)
	{ o.check_structure = true; }
static void __chpsim_no_check_structure(chpsim_options& o)
	{ o.check_structure = false; }
static void __chpsim_dump_dot_struct(chpsim_options& o)
	{ o.dump_dot_struct = true; }
static void __chpsim_no_dump_dot_struct(chpsim_options& o)
	{ o.dump_dot_struct = false; }

const chpsim::register_options_modifier
	chpsim::_default(
		"default", &__chpsim_default,
		"default options"), 
	chpsim::_run(
		"run", &__chpsim_run,
		"enable simulation run (default)"), 
	chpsim::_no_run(
		"no-run", &__chpsim_no_run,
		"disable simulation run"), 
	chpsim::_dump_graph_alloc(
		"dump-graph-alloc", &__chpsim_dump_graph_alloc,
		"show result of expression allocation"), 
	chpsim::_no_dump_graph_alloc(
		"no-dump-graph-alloc", &__chpsim_no_dump_graph_alloc,
		"suppress result of expression allocation (default)"),
	chpsim::_check_structure(
		"check-structure", &__chpsim_check_structure,
		"checks expression/node structure consistency (default)"), 
	chpsim::_no_check_structure(
		"no-check-structure", &__chpsim_no_check_structure,
		"disable structural consistency checks"),
	chpsim::_dump_dot_struct(
		"dump-dot-struct", &__chpsim_dump_dot_struct,
		"print dot-formatted graph structure"), 
	chpsim::_no_dump_dot_struct(
		"no-dump-dot-struct", &__chpsim_no_dump_dot_struct,
		"suppress dot-formatted graph structure (default)");

//=============================================================================
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

