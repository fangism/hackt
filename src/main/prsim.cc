/**
	\file "main/prsim.cc"
	Traditional production rule simulator. 
	This source file is processed by extract_texinfo.awk for 
	command-line option documentation.  
	$Id: prsim.cc,v 1.28 2010/04/05 20:10:57 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

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
#include "main/global_options.h"
#include "main/compile_options.h"
#include "common/config.h"
#include "util/getopt_mapped.h"
#include "util/persistent_object_manager.h"
#include "sim/prsim/State-prsim.h"
#include "sim/prsim/Command-prsim.h"
#include "sim/command_common.h"
#include "sim/prsim/ExprAllocFlags.h"
#include "util/string.tcc"	// for string_to_num
#include "install_paths.h"

namespace HAC {
using std::string;
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
	/// just print command help and exit
	bool			command_help_only;
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
	/// whether or not to automatically save
	bool			autosave;
	/// whether or not to startup tracing all events
	bool			autotrace;
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
	string			autosave_name;
	string			autotrace_name;
	ExprAllocFlags		expr_alloc_flags;
	/// compiler-driver flags
	compile_options		comp_opt;

	typedef	std::list<string>	source_paths_type;
	/// include search paths for sources
	source_paths_type	source_paths;

	prsim_options() : 
		help_only(false), command_help_only(false),
		interactive(true), 
		dump_expr_alloc(false), run(true),
		check_structure(true), dump_dot_struct(false), 
		dump_checkpoint(false),
		autosave(false),
		autotrace(false),
		use_referenced_type_instead_of_top_level(false),
		named_process_type(),
		autosave_name("autosave.prsimckpt"),
		autotrace_name("autotrace.prsimtrace"),
		expr_alloc_flags(), 
		comp_opt(),
		source_paths() { }

};	// end class prsim_options

//=============================================================================
// class alloc static initializers

const char
prsim::name[] = "prsim";

const char
prsim::brief_str[] = "A simple production rule simulator, based on an old one.";

#ifndef	WITH_MAIN
const size_t
prsim::program_id = register_hackt_program_class<prsim>();
#endif

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
	// TODO: -h should be for command-line usage
	// -H should be for in-program command help
	if (opt.help_only) {
		usage();
		return 0;
	}
	if (opt.command_help_only) {
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
	// normally, this would only be called if using the original top-level
	// workaround: (for missing global param bug)
	// just create the entire object hierarchy first
	// that way global params area already processed
	if (!the_module->allocate_unique().good) {
		cerr << alloc_errstr << endl;
		return 1;
	}
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
	top_module = the_module;
}
#if !MEMORY_MAPPED_GLOBAL_ALLOCATION
	top_module->populate_top_footprint_frame();
#endif
	// the simulator state object, initialized with the module
try {
	State sim_state(*top_module, opt.expr_alloc_flags);	// may throw
	const State::signal_handler_type int_handler(&sim_state);
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
		// autosave, autotrace
		if (opt.autosave) {
			sim_state.autosave(opt.autosave, opt.autosave_name);
		}
		if (opt.autotrace) {
			sim_state.open_trace(opt.autotrace_name);
		}
		CommandRegistry::prompt = sim_state.get_prompt();
		// initialize readline for tab-completion
		const CommandRegistry::readline_init __rl__(*top_module);
		// outermost level is interactive
		// until later, when we give a source file, or redirect in
		const int ret = CommandRegistry::interpret(sim_state, std::cin, 
			opt.interactive);
		if (ret) {
			// non-zero return value only has meaning 
			// to the interpreter
			// if autosave is on, destruction of the state
			// will save checkpoint for post-mortem analysis.
			return 1;	// ret;
		}
		// do we want to autosave when exit status is 0?
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
	TODO: more options!
	TODO: use getopt_mapped(), see cflat for examples.  
 */
int
prsim::parse_command_options(const int argc, char* argv[], options& o) {
	// now we're adding our own flags
	static const char optstring[] = "+a:bcC:d:D:f:hHiI:O:r:t:v";
	int c;
	while ((c = getopt(argc, argv, optstring)) != -1) {
	switch (c) {
/***
@texinfo opt/option-a.texi
@cindex checkpoint
@cindex autosave
@defopt -a file
Automatically save checkpoint @var{file} upon exit, 
regardless of the exit status.
Useful for debugging and resuming simulations.  
@end defopt
@end texinfo
***/
		case 'a':
			o.autosave = true;
			o.autosave_name = optarg;
			break;
/***
@texinfo opt/option-b.texi
@cindex batch mode
@defopt -b
Batch mode.  Run non-interactively, suppressing the prompt
and disabling tab-completion (from readline or editline).  
Useful for scripted jobs.  
Opposite of @option{-i}.  
@end defopt
@end texinfo
***/
		case 'b':
			// batch-mode, non-interactive
			o.interactive = false;
			break;
/***
@texinfo opt/option-c.texi
@defopt -c
Pass to indicate that input file is a source (to be compiled)
as opposed to an object file.
@end defopt
@end texinfo
***/
		case 'c':
			// just forward to a convenience function
			// fall-through
/***
@texinfo opt/option-C-upper.texi
@defopt -C options
When input is a source file, forward @var{options} to the compiler driver.  
@strong{NOTE:} This feature does not work yet, 
due to non-reentrant @command{getopt()}.  
@end defopt
@end texinfo
***/
		case 'C': {
			const int r = parse_create_flag(c, o.comp_opt);
			if (r) return r;
		}
			break;
/***
@texinfo opt/option-d.texi
@defopt -d ckpt
Print textual dump of prsim checkpoint file @var{ckpt}.  
@end defopt
@end texinfo
***/
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
/***
@texinfo opt/option-D-upper.texi
@defopt -D time
Override the default delay value applied to unspecified rules.  
@end defopt
@end texinfo
***/
		case 'D': {
			State::time_type t;
			if (string_to_num(optarg, t)) {
				cerr << "Invalid delay value: " <<
					optarg << "." << endl;
				return 1;
			}
			State::rule_type::default_unspecified_delay = t;
			break;
		}
/***
@texinfo opt/option-f.texi
@defopt -f @var{flag}
@xref{General Flags}.
@end defopt
@end texinfo
***/
		case 'f': {
			const options_modifier_map_iterator
				mi(options_modifier_map.find(optarg));
			if (mi == options_modifier_map.end() || !mi->second) {
				cerr << "Invalid option argument: " 
					<< optarg << endl;
				return 1;
			} else {
				(mi->second)(o);
			}
			break;
		}
/***
@texinfo opt/option-h.texi
@defopt -h
Print command-line options help and exit.
@end defopt
@end texinfo
***/
		case 'h':
			o.help_only = true;
			// return 0
			break;
/***
@texinfo opt/option-H-upper.texi
@defopt -H
Print list of all interpreter commands and exit.  
@end defopt
@end texinfo
***/
		case 'H':
			o.command_help_only = true;
			// return 0
			break;
/***
@texinfo opt/option-i.texi
@cindex interactive mode
@defopt -i
Interactive mode.  Show prompt before each command.
Enable tab-completion if built with readline/editline.  
Opposite of @option{-b}.  
@end defopt
@end texinfo
***/
		case 'i':
			o.interactive = true;
			break;
/***
@texinfo opt/option-I-upper.texi
@cindex source paths
@defopt -I path @r{(repeatable)}
@anchor{option-I}
Append @var{path} to the list of paths to search for sourcing other 
command scripts in the interpreter.  
@end defopt
@end texinfo
***/
		case 'I':
			o.source_paths.push_back(optarg);
			break;
/***
@texinfo opt/option-O-upper.texi
@cindex optimization
@defopt -O lvl
Optimize internal expanded representation of production rules.
Optimizations do not affect the event outcome of simulations.  
Current valid values of @var{lvl} are 0 (none) and 1.  
For more details, @xref{Optimization Flags}.
@end defopt
@end texinfo
***/
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
/***
@texinfo opt/option-r.texi
@defopt -r file
@cindex trace file
@cindex recording trace
Startup the simulation already recording a trace file of every event.  
Trace file is automatically close when simulation exits.  
This is equivalent issuing @command{trace} command at the beginning
of a simulation session.
@end defopt
@end texinfo
***/
		case 'r':
			o.autotrace = true;
			o.autotrace_name = optarg;
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
/***
@texinfo opt/option-v.texi
@defopt -v
Print version and exit.
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
prsim::usage(void) {
	cerr << "usage: " << name << " [options] <hackt-obj-infile>" << endl;
	cerr << "options:\n"
"\t-a <file> : auto-save checkpoint upon exit\n"
"\t-b : batch-mode, non-interactive (promptless)\n"
"\t-c : input file is source, not object, compile it\n"
"\t-C <opts> : forward options to compile driver\n"
"\t-d <checkpoint>: textual dump of checkpoint only\n"
"\t-f <flag> : general options modifiers (listed below)\n"
"\t-h : print usage help and exit (objfile optional)\n"
"\t-H : print in-program command help and exit (objfile optional)\n"
"\t-i : interactive (default)\n"
"\t-I <path> : include path for scripts (repeatable)\n"
"\t-O <0..1> : expression optimization level\n"
"\t-r <file> : record a trace of all events to file at startup\n"
"\t-t \"type\" : allocate one instance of the named type,\n"
	"\t\tignoring top-level instances (quotes recommended).\n";
        const size_t flags = options_modifier_map.size();
	if (flags) {
		cerr << "flags (" << flags << " total):" << endl;
		dump_options_briefs(cerr);
	}
	cerr << "Additional documentation is installed in:\n"
	"\t`info hacprsim' (finds " INFODIR "/hacprsim.info)\n"
	"\tPDF: " PDFDIR "/hacprsim.pdf\n"
	"\tPS: " PSDIR "/hacprsim.ps\n"
	"\tHTML: " HTMLDIR "/hacprsim.html/index.html" << endl;
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
/***
@texinfo opt/default.texi
@defopt {-f default}
Reset to the default set of configuration options.  
Not negatable.  
@end defopt
@end texinfo
***/
	prsim::_default(
		"default", &__prsim_default,
		"default options"), 
/***
@texinfo opt/run.texi
@defopt {-f run}
Actually run the simulator's interpreter.  Enabled by default.
@samp{-f no-run} is explicitly needed when all that is desired
are diagnostic dumps.  
@end defopt
@end texinfo
***/
	prsim::_run(
		"run", &__prsim_run,
		"enable simulation run (default)"), 
	prsim::_no_run(
		"no-run", &__prsim_no_run,
		"disable simulation run"), 
/***
@texinfo opt/dump-expr-alloc.texi
@defopt {-f dump-expr-alloc}
Diagnostic. 
Print result of expression allocation prior to execution of the simulator.  
@end defopt
@end texinfo
***/
	prsim::_dump_expr_alloc(
		"dump-expr-alloc", &__prsim_dump_expr_alloc,
		"show result of expression allocation"), 
	prsim::_no_dump_expr_alloc(
		"no-dump-expr-alloc", &__prsim_no_dump_expr_alloc,
		"suppress result of expression allocation (default)"),
/***
@texinfo opt/check-structure.texi
@defopt {-f check-structure}
Run some internal structural consistency checks 
on nodes and expressions prior to simulation.
Enabled by default.
@end defopt
@end texinfo
***/
	prsim::_check_structure(
		"check-structure", &__prsim_check_structure,
		"checks expression/node structure consistency (default)"), 
	prsim::_no_check_structure(
		"no-check-structure", &__prsim_no_check_structure,
		"disable structural consistency checks"),
/***
@texinfo opt/dump-dot-struct.texi
@defopt {-f dump-dot-struct}
Diagnostic.  
Print a @command{dot} format representation of the 
whole-program production rule graph.  
Recommend using with @option{-f no-run}.  
@end defopt
@end texinfo
***/
	prsim::_dump_dot_struct(
		"dump-dot-struct", &__prsim_dump_dot_struct,
		"print dot-formatted graph structure"), 
	prsim::_no_dump_dot_struct(
		"no-dump-dot-struct", &__prsim_no_dump_dot_struct,
		"suppress dot-formatted graph structure (default)"),
/***
@texinfo opt/fold-literals.texi
@defopt {-f fold-literals}
Collapse leaf nodes of literals directly into their parent expressions.  
Dramatically reduces the number of expression nodes allocated, 
and shortens propagation paths to output nodes.  
Enabled at level @option{-O 1} and above.  
@end defopt
@end texinfo
***/
	prsim::_fold_literals(
		"fold-literals", &__prsim_fold_literals,
		"[OPT] eliminate literal leaf nodes"), 
	prsim::_no_fold_literals(
		"no-fold-literals", &__prsim_no_fold_literals,
		"[OPT] disable fold-literals"), 
/***
@texinfo opt/denormalize-negations.texi
@defopt {-f denormalize-negations}
Apply DeMorgan's rules to transform expressions by pushing negations
as close each rule's root node as possible.  
Production rules are restructured into equivalent expressions.  
Reduces the number of negation expressions, 
enabling better folding of negated literals.  
Enabled at level @option{-O 1} and above.  
@end defopt
@end texinfo
***/
	prsim::_denormalize_negations(
		"denormalize-negations", &__prsim_denormalize_negations,
		"[OPT] apply DeMorgan\'s transformations"), 
	prsim::_no_denormalize_negations(
		"no-denormalize-negations", &__prsim_no_denormalize_negations,
		"[OPT] disable denormalize-negations");

//=============================================================================
}	// end namespace HAC

#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::prsim::main(argc, argv, g);
}
#endif	// WITH_MAIN

DEFAULT_STATIC_TRACE_END

