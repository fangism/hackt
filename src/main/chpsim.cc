/**
	\file "main/chpsim.cc"
	Main module for new CHPSIM.
	$Id: chpsim.cc,v 1.5 2007/03/16 07:07:21 fang Exp $
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
#include "main/global_options.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "sim/chpsim/State.h"
#include "sim/chpsim/Command.h"
#include "sim/chpsim/graph_options.h"
#include "sim/command_common.h"
#include "util/getopt_mapped.h"		// for getopt()

namespace HAC {
#include "util/using_ostream.h"
using SIM::CHPSIM::State;
using SIM::CHPSIM::CommandRegistry;
using SIM::CHPSIM::graph_options;
using entity::canonical_fundamental_chan_type_base;

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
	/// whether or not to print checkpoint dump
	bool				dump_checkpoint;
	/// list of paths to search for sourced scripts
	typedef	std::list<string>	source_paths_type;
	source_paths_type		source_paths;
	/// fine-tuning graph options
	graph_options			graph_opts;

	chpsim_options() : help_only(false), interactive(true), 
		run(true), dump_graph_alloc(false), check_structure(true),
		dump_dot_struct(false), dump_checkpoint(false), 
		source_paths() { }
};	// end class chpsim_options

//=============================================================================
// class chpsim static initializers

const char
chpsim::name[] = "chpsim";

const char
chpsim::brief_str[] = "An event-driven CHP simulator";

#ifndef	WITH_MAIN
const size_t
chpsim::program_id = register_hackt_program_class<chpsim>();
#endif

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
	if (opt.dump_checkpoint) {
		// dump checkpoint only, doesn't run
		return 0;
	}
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
	const count_ptr<module> the_module(load_module(ofn));
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
	// first, cache all built-in channel types' summaries
	canonical_fundamental_chan_type_base::refresh_all_footprints();
	State sim_state(*the_module);		// may throw
	// install interrupt signal handler
	const State::signal_handler int_handler(&sim_state);
	if (opt.dump_graph_alloc)
		sim_state.dump_struct(cout) << endl;
#if 0
	if (opt.check_structure)
		sim_state.check_structure();
#endif
	if (opt.dump_dot_struct)
		sim_state.dump_struct_dot(cout, opt.graph_opts) << endl;
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
	static const char optstring[] = "+bd:f:hiI:";
	int c;
while((c = getopt(argc, argv, optstring)) != -1) {
switch (c) {
	case 'b':
		o.interactive = false;
		break;
	case 'd': {
		o.dump_checkpoint = true;
		std::ifstream f(optarg, std::ios_base::binary);
		if (!f) {
			cerr << "Error opening file \"" << optarg <<
				"\" for reading." << endl;
		}
		State::dump_raw_checkpoint(cout, f);
		break;
	}
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
	cerr << "\t-d <checkpoint>: textual dump of checkpoint only" << endl;
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
static void __chpsim_show_event_index(chpsim_options& o)
	{ o.graph_opts.show_event_index = true; }
static void __chpsim_no_show_event_index(chpsim_options& o)
	{ o.graph_opts.show_event_index = false; }
static void __chpsim_show_instances(chpsim_options& o)
	{ o.graph_opts.show_instances = true;
#if CHPSIM_READ_WRITE_DEPENDENCIES
	  o.graph_opts.with_antidependencies = true;
#endif
	}
static void __chpsim_no_show_instances(chpsim_options& o)
	{ o.graph_opts.show_instances = false; }
#if CHPSIM_READ_WRITE_DEPENDENCIES
static void __chpsim_antidependencies(chpsim_options& o)
	{ o.graph_opts.with_antidependencies = true; }
static void __chpsim_no_antidependencies(chpsim_options& o)
	{ o.graph_opts.with_antidependencies = false; }
#endif
static void __chpsim_process_clusters(chpsim_options& o)
	{ o.graph_opts.process_event_clusters = true; }
static void __chpsim_no_process_clusters(chpsim_options& o)
	{ o.graph_opts.process_event_clusters = false; }
static void __chpsim_show_delays(chpsim_options& o)
	{ o.graph_opts.show_delays = true; }
static void __chpsim_no_show_delays(chpsim_options& o)
	{ o.graph_opts.show_delays = false; }

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
		"suppress dot-formatted graph structure (default)"),
	chpsim::_show_event_index(
		"show-event-index", &__chpsim_show_event_index,
		"for dot-graphs: show event indices in graph"), 
	chpsim::_no_show_event_index(
		"no-show-event-index", &__chpsim_no_show_event_index,
		"for dot-graphs: hide event indices"),
	chpsim::_show_instances(
		"show-instances", &__chpsim_show_instances,
		"for dot-graphs: show allocated instances as nodes"), 
	chpsim::_no_show_instances(
		"no-show-instances", &__chpsim_no_show_instances,
		"for dot-graphs: hide allocated instances"),
#if CHPSIM_READ_WRITE_DEPENDENCIES
	chpsim::_antidependencies(
		"antidependencies", &__chpsim_antidependencies,
		"for dot-graphs: show anti-dependence edges"), 
	chpsim::_no_antidependencies(
		"no-antidependencies", &__chpsim_no_antidependencies,
		"for dot-graphs: hide anti-dependence edges"),
#endif
	chpsim::_process_clusters(
		"cluster-processes", &__chpsim_process_clusters,
		"for dot-graphs: wrap process subgraphs into clusters"), 
	chpsim::_no_process_clusters(
		"no-cluster-processes", &__chpsim_no_process_clusters,
		"for dot-graphs: un-clustered process subgraphs"),
	chpsim::_show_delays(
		"show-delays", &__chpsim_show_delays,
		"for dot-graphs: wrap process subgraphs into clusters"), 
	chpsim::_no_show_delays(
		"no-show-delays", &__chpsim_no_show_delays,
		"for dot-graphs: un-clustered process subgraphs");

//=============================================================================
}	// end namespace HAC

#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::chpsim::main(argc, argv, g);
}
#endif	// WITH_MAIN

DEFAULT_STATIC_TRACE_END

