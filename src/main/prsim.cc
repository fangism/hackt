/**
	\file "main/prsim.cc"
	Traditional production rule simulator. 

	$Id: prsim.cc,v 1.1.2.1 2006/01/04 08:42:10 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "util/static_trace.h"

#include <iostream>
#include <cstring>

#include "main/prsim.h"
#include "main/program_registry.h"
#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "main/options_modifier.tcc"
#include "main/simple_options.tcc"
#include "util/persistent_object_manager.h"
#include "sim/prsim/State.h"

namespace HAC {
using SIM::PRSIM::State;
using util::persistent;
using util::persistent_object_manager;

#include "util/using_ostream.h"

//=============================================================================
// explicit instantiation for proper initialization ordering
// (options modifier registry map initialized before use)
template class prsim::options_modifier_policy;

//=============================================================================
class prsim_options {
public:
	// none
	/// whether or not to show result of expression allocation
	bool			dump_expr_alloc;
	/// whether or not to run the simulation or just terminate after setup
	bool			run;

	prsim_options() : dump_expr_alloc(false), run(true) { }
};	// end class options

//=============================================================================
// class alloc static initializers

const char
prsim::name[] = "prsim";

const char
prsim::brief_str[] = "A simple production rule simulator, based an on old one.";

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

#if 0
	const count_ptr<module> the_module(load_module(argv[1]).release());
#else
	excl_ptr<module> the_module = load_module(ofn);
#endif
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

	State sim_state(*the_module);
	if (opt.dump_expr_alloc)
		sim_state.dump_struct(cout) << endl;
	// optimize expression allocation?
	if (opt.run) {

	}
	// else just exit
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
prsim::parse_command_options(const int argc, char* argv[], options& o) {
	// using simple template function
	return parse_simple_command_options(
		argc, argv, o, options_modifier_map);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
prsim::usage(void) {
	cerr << "usage: " << name << " [options] <hackt-obj-infile>" << endl;
	cerr << "options: -f <flag>" << endl;
        const size_t flags = options_modifier_map.size();
	if (flags) {
		cerr << "flags (" << flags << " total):" << endl;
		dump_options_briefs(cerr);
	}
}

//-----------------------------------------------------------------------------
static void __prsim_default(prsim_options&) { }
static void __prsim_run(prsim_options& o) { o.run = true; }
static void __prsim_no_run(prsim_options& o) { o.run = false; }
static void __prsim_dump_expr_alloc(prsim_options& o)
	{ o.dump_expr_alloc = true; }
static void __prsim_no_dump_expr_alloc(prsim_options& o)
	{ o.dump_expr_alloc = false; }

const prsim::register_options_modifier
	prsim::_default("default", &__prsim_default, "default options"), 
	prsim::_run("run", &__prsim_run, "enable simulation run (default)"), 
	prsim::_no_run("no-run", &__prsim_no_run, "disable simulation run"), 
	prsim::_dump_expr_alloc("dump-expr-alloc", &__prsim_dump_expr_alloc,
		"show result of expression allocation"), 
	prsim::_no_dump_expr_alloc("no-dump-expr-alloc",
		&__prsim_no_dump_expr_alloc,
		"suppress result of expression allocation");

//=============================================================================
}	// end namespace HAC

