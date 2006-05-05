/**
	\file "main/prsim.cc"
	Traditional production rule simulator. 

	$Id: prsim.cc,v 1.5.2.2 2006/05/05 04:55:31 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "util/static_trace.h"

#include <iostream>
#include <fstream>
#include <cstring>
#include <list>

#include "main/prsim.h"
#include "main/program_registry.h"
#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "main/options_modifier.tcc"
#include "main/simple_options.tcc"
#include "util/persistent_object_manager.h"
#include "sim/prsim/State.h"
#include "sim/prsim/Command.h"
#include "sim/prsim/ExprAllocFlags.h"
#include "util/string.tcc"	// for string_to_num

namespace HAC {
using SIM::PRSIM::State;
using SIM::PRSIM::CommandRegistry;
using SIM::PRSIM::ExprAllocFlags;
using util::persistent;
using util::persistent_object_manager;
using util::strings::string_to_num;

#include "util/using_ostream.h"

//=============================================================================
// explicit instantiation for proper initialization ordering
// (options modifier registry map initialized before use)
// accepted by g++-3.3, but not 4
// template class prsim::options_modifier_policy;
template class options_modifier_policy<prsim_options>;

//=============================================================================
class prsim_options {
public:
	// none
	/// just print help and exit
	bool			help_only;
	/// interactive (default true) vs. batch mode
	bool			interactive;
	/// whether or not to show result of expression allocation
	bool			dump_expr_alloc;
	/// whether or not to run the simulation or just terminate after setup
	bool			run;
	/// check structure
	bool			check_structure;
	/// whether or not to produce a dot-format structure dump before running
	bool			dump_dot_struct;
	/// whether or not checkpoint dump is requested
	bool			dump_checkpoint;
	ExprAllocFlags		expr_alloc_flags;

	typedef	std::list<string>	source_paths_type;
	/// include search paths for sources
	source_paths_type	source_paths;

	prsim_options() : help_only(false), interactive(true), 
		dump_expr_alloc(false), run(true),
		check_structure(true), dump_dot_struct(false), 
		dump_checkpoint(false),
		expr_alloc_flags(), 
		source_paths() { }

};	// end class options

//=============================================================================
// class alloc static initializers

const char
prsim::name[] = "prsim";

const char
prsim::brief_str[] = "A simple production rule simulator, based on an old one.";

const size_t
prsim::program_id = register_hackt_program_class<prsim>();

//=============================================================================
prsim::prsim() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
prsim::main(const int argc, char* argv[], const global_options&) {
	options opt;
	if (parse_command_options(argc, argv, opt)) {
		cerr << "Error in command invocation." << endl;
		usage();
		return 1;
	}
	if (opt.dump_checkpoint) {
		// dump of checkpoint only
		return 0;
	}
	if (opt.help_only) {
		util::string_list args;
		args.push_back("help");
		args.push_back("all");
		SIM::PRSIM::Help::main(args);
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
	if (!check_object_loadable(ofn).good)
		return 1;

	excl_ptr<module> the_module = load_module(ofn);
	if (!the_module)
		return 1;

//	the_module->dump(cerr);
	if (the_module->is_allocated()) {
		// cerr << "Module is already allocated, skipping..." << endl;
	} else {
		if (!the_module->allocate_unique().good) {
			cerr << "ERROR in allocating.  Aborting." << endl;
			return 1;
		}
//		the_module->dump(cerr);
	}

	// the simulator state object, initialized with the module
try {
	State sim_state(*the_module, opt.expr_alloc_flags);	// may throw
	const State::signal_handler int_handler(&sim_state);
	if (opt.dump_expr_alloc)
		sim_state.dump_struct(cout) << endl;
	if (opt.check_structure)
		sim_state.check_structure();
	// optimize expression allocation?

	if (opt.dump_dot_struct)
		sim_state.dump_struct_dot(cout) << endl;
	sim_state.import_source_paths(opt.source_paths);
	if (opt.run) {
		sim_state.initialize();
		// outermost level is interactive
		// until later, when we give a source file, or redirect in
		const int ret = CommandRegistry::interpret(sim_state,
			opt.interactive);
		if (ret)	return ret;
	}
} catch (...) {
	cerr << "Caught exception during construction of simulator state."
		<< endl;
	return 1;
}
	// else just exit
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: -I for search paths for recursive scripts.  
 */
int
prsim::parse_command_options(const int argc, char* argv[], options& o) {
#if 0
	// using simple template function for now
	return parse_simple_command_options(
		argc, argv, o, options_modifier_map);
#else
	// now we're adding our own flags
	static const char optstring[] = "+bd:f:hiI:O:";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
		case 'b':
			// batch-mode, non-interactive
			o.interactive = false;
			break;
		case 'd': {
			o.dump_checkpoint = true;
			std::ifstream f(optarg, std::ios_base::binary);
			if (!f) {
				cerr << "Error opening file \"" << optarg <<
					"\" for reading." << endl;
			}
			State::dump_checkpoint(cout, f);
			break;
		}
		case 'f': {
			const options_modifier_map_iterator
				mi(options_modifier_map.find(optarg));
			if (mi == options_modifier_map.end() || !mi->second) {
				cerr << "Invalid mode: " << optarg << endl;
				return 1;
			} else {
				(mi->second)(o);
			}
			break;
		}
		case 'h':
			o.help_only = true;
			// return 0
			break;
		case 'i':
			o.interactive = true;
			break;
		case 'I':
			o.source_paths.push_back(optarg);
			break;
		case 'O': {
			int opt_level;
			if (string_to_num(optarg, opt_level)) {
				cerr << "Invalid optimization level: " <<
					optarg << "." << endl;
				return 1;
			} else {
				o.expr_alloc_flags.optimize(opt_level);
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
			THROW_EXIT;
	}       // end switch
	}       // end while
	return 0;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
prsim::usage(void) {
	cerr << "usage: " << name << " [options] <hackt-obj-infile>" << endl;
	cerr << "options:" << endl;
	cerr << "\t-b : batch-mode, non-interactive (promptless)" << endl;
	cerr << "\t-d <checkpoint>: textual dump of checkpoint only" << endl;
	cerr << "\t-f <flag> : general options modifiers (listed below)" << endl;
	cerr << "\t-h : print commands help and exit (objfile optional)" << endl;
	cerr << "\t-i : interactive (default)" << endl;
	cerr << "\t-I <path> : include path for scripts (repeatable)" << endl;
	cerr << "\t-O <0..1> : expression optimization level" << endl;
        const size_t flags = options_modifier_map.size();
	if (flags) {
		cerr << "flags (" << flags << " total):" << endl;
		dump_options_briefs(cerr);
	}
}

//-----------------------------------------------------------------------------
// prsim_option modifier functions and their flag registrations
static void __prsim_default(prsim_options& o) { o = prsim_options(); }
static void __prsim_run(prsim_options& o) { o.run = true; }
static void __prsim_no_run(prsim_options& o) { o.run = false; }
static void __prsim_dump_expr_alloc(prsim_options& o)
	{ o.dump_expr_alloc = true; }
static void __prsim_no_dump_expr_alloc(prsim_options& o)
	{ o.dump_expr_alloc = false; }
static void __prsim_check_structure(prsim_options& o)
	{ o.check_structure = true; }
static void __prsim_no_check_structure(prsim_options& o)
	{ o.check_structure = false; }
static void __prsim_dump_dot_struct(prsim_options& o)
	{ o.dump_dot_struct = true; }
static void __prsim_no_dump_dot_struct(prsim_options& o)
	{ o.dump_dot_struct = false; }
static void __prsim_fold_literals(prsim_options& o)
	{ o.expr_alloc_flags.fold_literals(); }
static void __prsim_no_fold_literals(prsim_options& o)
	{ o.expr_alloc_flags.no_fold_literals(); }
static void __prsim_denormalize_negations(prsim_options& o)
	{ o.expr_alloc_flags.denormalize_negations(); }
static void __prsim_no_denormalize_negations(prsim_options& o)
	{ o.expr_alloc_flags.no_denormalize_negations(); }

const prsim::register_options_modifier
	prsim::_default(
		"default", &__prsim_default,
		"default options"), 
	prsim::_run(
		"run", &__prsim_run,
		"enable simulation run (default)"), 
	prsim::_no_run(
		"no-run", &__prsim_no_run,
		"disable simulation run"), 
	prsim::_dump_expr_alloc(
		"dump-expr-alloc", &__prsim_dump_expr_alloc,
		"show result of expression allocation"), 
	prsim::_no_dump_expr_alloc(
		"no-dump-expr-alloc", &__prsim_no_dump_expr_alloc,
		"suppress result of expression allocation (default)"),
	prsim::_check_structure(
		"check-structure", &__prsim_check_structure,
		"checks expression/node structure consistency (default)"), 
	prsim::_no_check_structure(
		"no-check-structure", &__prsim_no_check_structure,
		"disable structural consistency checks"),
	prsim::_dump_dot_struct(
		"dump-dot-struct", &__prsim_dump_dot_struct,
		"print dot-formatted graph structure"), 
	prsim::_no_dump_dot_struct(
		"no-dump-dot-struct", &__prsim_no_dump_dot_struct,
		"suppress dot-formatted graph structure (default)"),
	prsim::_fold_literals(
		"fold-literals", &__prsim_fold_literals,
		"[OPT] eliminate literal leaf nodes"), 
	prsim::_no_fold_literals(
		"no-fold-literals", &__prsim_no_fold_literals,
		"[OPT] disable fold-literals"), 
	prsim::_denormalize_negations(
		"denormalize-negations", &__prsim_denormalize_negations,
		"[OPT] apply DeMorgan\'s transformations"), 
	prsim::_no_denormalize_negations(
		"no-denormalize-negations", &__prsim_no_denormalize_negations,
		"[OPT] disable denormalize-negations");

//=============================================================================
}	// end namespace HAC

