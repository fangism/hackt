/**
	\file "main/chpsim.cc"
	Main module for new CHPSIM.
	This file is also processed with a script to extract 
	Texinfo documentation.
	This allows us to keep the documentation close to the source.
	$Id: chpsim.cc,v 1.15 2008/03/17 23:02:41 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#include "util/static_trace.h"

DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
#include <list>
#include "main/chpsim.h"
#include "main/chpsim_options.h"
#include "main/program_registry.h"	// to register with hackt's dispatcher
#include "main/main_funcs.h"		// for save/load_module()
#include "main/options_modifier.tcc"
#include "main/global_options.h"
#include "Object/type/canonical_fundamental_chan_type.h"
#include "Object/type/process_type_reference.h"
#include "Object/def/footprint.h"
#include "Object/lang/CHP_footprint.h"
#include "Object/expr/expr_dump_context.h"
#include "Object/expr/dlfunction.h"	// for ack_loaded_functions
#include "sim/chpsim/State.h"
#include "sim/chpsim/Command.h"
#include "sim/chpsim/graph_options.h"
#include "sim/command_common.h"
#include "util/getopt_mapped.h"		// for getopt()
#include "common/ltdl-wrap.h"
#include "common/TODO.h"
#include "install_paths.h"

namespace HAC {
#include "util/using_ostream.h"
using SIM::CHPSIM::State;
using SIM::CHPSIM::CommandRegistry;
using SIM::CHPSIM::graph_options;
using entity::canonical_fundamental_chan_type_base;
using entity::process_type_reference;
using entity::footprint;
using entity::CHP::local_event_footprint;

//=============================================================================
template class options_modifier_policy<chpsim_options>;

//=============================================================================
// class chpsim static initializers

const char
chpsim::name[] = "chpsim";

const char
chpsim::brief_str[] = "An event-driven CHP simulator";

#if	!defined(WITH_MAIN) && !defined(NO_REGISTER)
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
	const ltdl_token ltdl;	// dlinit/dlexit pair
	options opt;
	switch (parse_command_options(argc, argv, opt)) {
	case 0: break;
	// syntax error in command line arguments
	case 1:
		cerr << "Error in command invocation." << endl;
		usage();
		return 1;
	// some other error
	default:
		cerr << "Error in command invocation." << endl;
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
	count_ptr<module> the_module;
if (opt.comp_opt.compile_input) {
	the_module = parse_and_check(ofn, opt.comp_opt);
} else {
	if (!check_object_loadable(ofn).good)
		return 1;
	the_module = load_module(ofn);
	// load_module_debug(ofn);
}
	if (!the_module)
		return 1;
//	the_module->dump(cerr);
	static const char alloc_errstr[] =  "ERROR in allocating.  Aborting.";

	count_ptr<module> top_module;
	// when we want to print information for only a specific type
	const footprint* fp = NULL;
	if (opt.use_type) {
		// parse complete type
		const count_ptr<const process_type_reference>
			pt(parse_and_create_complete_process_type(
			opt.complete_type_name.c_str(), *the_module));
		if (!pt) {
			// already have error message
			return 1;
		}
		// resolve footprint
		fp = pt->lookup_footprint();
		NEVER_NULL(fp);
		// create fake top-level module from them (import)
		top_module = count_ptr<module>(new module("<auxiliary>"));
		NEVER_NULL(top_module);
		if (!top_module->allocate_unique_process_type(*pt,
				*the_module).good) {
			cerr << alloc_errstr << endl;
			return 1;
		}
		// if (opt.instantiate_type_recursively)
	} else {
		top_module = the_module;
	}

	// automatically compile as far as needed:
	if (!top_module->is_allocated()) {
		if (!top_module->allocate_unique().good) {
			cerr << alloc_errstr << endl;
			return 1;
		}
	}
try {
	// first, cache all built-in channel types' summaries
	canonical_fundamental_chan_type_base::refresh_all_footprints();
	State sim_state(*top_module);		// may throw
	// install interrupt signal handler
	const State::signal_handler int_handler(&sim_state);
	if (opt.dump_graph_alloc) {
	if (opt.use_type && !opt.instantiate_type_recursively) {
		NEVER_NULL(fp);
		const char* const null = NULL;
		fp->get_chp_event_footprint().dump(cout, 
			entity::expr_dump_context(null)) << endl;;
	} else {
		sim_state.dump_struct(cout) << endl;
	}
	}
#if 0
	if (opt.check_structure)
		sim_state.check_structure();
#endif
	if (opt.dump_dot_struct) {
	if (opt.use_type && !opt.instantiate_type_recursively) {
		NEVER_NULL(fp);
		const local_event_footprint::dot_graph_wrapper G(cout);
		fp->get_chp_event_footprint().dump_struct_dot(
			cout, opt.graph_opts) << endl;
		// this mode doesn't support channel edges (no deps analysis)
		// process clustering is also not applicable (is one process)
	} else {
		sim_state.dump_struct_dot(cout, opt.graph_opts) << endl;
	}
	}
	sim_state.import_source_paths(opt.source_paths);
	if (opt.run) {
		sim_state.initialize();
		// run command interpreter
		// return error if necessary
		const int ret = CommandRegistry::interpret(sim_state, 
			opt.interactive);
		if (ret) {
			// return value only has meaning to the interpreter
			if (CommandRegistry::autosave_on_exit) {
				std::ofstream ofs("autosave.chpsimckpt");
				if (ofs) {
					sim_state.save_checkpoint(ofs);
				} else {
					cerr <<
				"Error saving autosave.chpsimckpt" << endl;
				}
			}
			return 1;	// ret
		}
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
	MAINTAINENCE: keep this consistent and documented in
		$(top_srcdir)/dox/chpsim/usage.texi.
 */
int
chpsim::parse_command_options(const int argc, char* argv[], options& o) {
	static const char optstring[] = "+abcC:d:f:hiI:l:L:t:T:";
	int c;
while((c = getopt(argc, argv, optstring)) != -1) {
switch (c) {
/***
@texinfo options/option-a.texi
@cindex checkpoint
@cindex autosave
@defopt -a
Automatically save checkpoint "autosave.prsimckpt" upon exit, 
regardless of the exit status.
Useful for debugging and resuming simulations.  
@end defopt
@end texinfo
***/
	case 'a':
		CommandRegistry::autosave_on_exit = true;
		break;
/***
@texinfo options/option-b.texi
@cindex batch mode
@defopt -b
Batch mode, non-interactive, promptless.  
This is useful for running scripts or piping in commands 
while suppressing prompts.  
This mode also turns off tab-completion in the interpreter.  
The opposing option is @option{-i}.
@b{Note:} executables linked against @t{libeditline} may @emph{require} 
this option for processing scripts due to a mishandling of EOF.  
@end defopt
@end texinfo
***/
	case 'b':
		o.interactive = false;
		break;
/***
@texinfo options/option-c.texi
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
@texinfo options/option-C-upper.texi
@defopt -C options
When input is a source file, forward @var{options} to the compiler driver.  
@end defopt
@end texinfo
***/
	case 'C': {
		const int r = parse_create_flag(c, o.comp_opt);
		if (r) return r;
	}
		break;
/***
@texinfo options/option-d.texi
@cindex checkpoint
@defopt -d @var{checkpoint}
Produce a textual dump of a checkpoint binary.  
Exits without running the simulator.  
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
		State::dump_raw_checkpoint(cout, f);
		break;
	}
/***
@texinfo options/option-f.texi
@defopt -f @var{flag}
@xref{General Flags}.
@end defopt
@end texinfo
***/
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
/***
@texinfo options/option-h.texi
@defopt -h
Help.  Print list of all interpreter commands and exit.  
@end defopt
@end texinfo
***/
	case 'h':
		o.help_only = true;
		break;
/***
@texinfo options/option-i.texi
@cindex interactive
@cindex prompt
@defopt -i
Interactive, prompting.  This is the default mode.
The opposing option is @option{-b}.  
@end defopt
@end texinfo
***/
	case 'i':
		o.interactive = true;
		break;
/***
@texinfo options/option-I-upper.texi
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
@texinfo options/option-l.texi
@cindex module
@cindex loading
@defopt -l lib @r{(repeatable)}
@anchor{option-l}
Load the @var{lib} shared library module for registering user-defined
run-time functions.  
@var{lib} should be named @emph{without} its file extension, 
for the sake of portability.  
@c @var{lib}'s name should match that of the built library's base.
For example, @file{libcrunch.la} should be referenced as @samp{libcrunch}, 
and @file{chewy.so} should be referenced as @samp{chewy}.
The equivalent command in the interpreter is
@ref{command-dlopen,, @command{dlopen}}.
@end defopt
@end texinfo
***/
	case 'l':
		if (!HAC::ltdl_open_append(optarg)) {
			return 2;
		}
		break;
/***
@texinfo options/option-L-upper.texi
@cindex module paths
@cindex library paths
@defopt -L path @r{(repeatable)}
@anchor{option-L}
Append @var{path} to the list of paths to search for opening shared library
plug-ins (modules).  
The equivalent command in the interpreter is
@ref{command-dladdpath,, @command{dladdpath}}.
For more on building and loading shared-libraries, @xref{Extending simulation}.
@end defopt
@end texinfo
***/
	case 'L':
		lt_dladdsearchdir(optarg);
		break;
/***
@texinfo options/option-t.texi
@defopt -t type
Instead of expanding the whole top-level instances, only operate
on the given type @var{type}, i.e. instantiate one instance
of @var{type} as the top-level.  
This variation, however does @strong{not} expand subinstances recursively, 
like the @option{-T} option.  
This is particularly useful for examining the CHP event structure of
a particular definition.  
@end defopt
@end texinfo
***/
	case 't':
		o.use_type = true;
		o.instantiate_type_recursively = false;
		o.complete_type_name = optarg;
		break;
/***
@texinfo options/option-T-upper.texi
@defopt -T type
Instantiate one instance of type @var{type} as the top-level, 
ignoring all previous top-level instances in the object file.  
This variation @emph{does} recursively instantiate substructures.  
The ports of the instance of @var{type} (if any) will not be connected
to any other processes.  
This is particularly useful for selecting test structures
out of a collection of test structure definitions.  
@end defopt
@end texinfo
***/
	case 'T':
		o.use_type = true;
		o.instantiate_type_recursively = true;
		o.complete_type_name = optarg;
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
	cerr << "usage: " << name << " [options] <hackt-obj-file>" << endl;
	cerr << "options:\n"
"\t-a : auto-save checkpoint (autosave.chpsimckpt) upon exit\n"
"\t-b : batch-mode, non-interactive (promptless)\n"
"\t-d <checkpoint>: textual dump of checkpoint only\n"
"\t-f <flag> : general options modifiers (listed below)\n"
"\t-h : print commands help and exit (objfile optional)\n"
"\t-i : interactive (default)\n"
"\t-I <path> : include path for scripts (repeatable)\n"
"\t-L <path> : append load path for dlopening modules (repeatable)\n"
"\t-l <lib> : library to dlopen (NO file extension) (repeatable)\n"
"\t-t \"type\" : expand type non-recursively as top-level (recommend quotes)\n"
"\t-T \"type\" : expand type recursively as top-level (recommend quotes)"
	<< endl;
//	cerr << "\t-O <0..1> : expression optimization level" << endl;
        const size_t flags = options_modifier_map.size();
	if (flags) {
		cerr << "flags (" << flags << " total):" << endl;
		dump_options_briefs(cerr);
	}
	cerr << "To run a script, use shell redirection or pipes." << endl;
	cerr << "Additional documentation is installed in:\n"
	"\t`info hacchpsim' (finds " INFODIR "/hacchpsim.info)\n"
	"\tPDF: " PDFDIR "/hacchpsim.pdf\n"
	"\tPS: " PSDIR "/hacchpsim.ps\n"
	"\tHTML: " HTMLDIR "/hacchpsim.html/index.html" << endl;
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
static void __chpsim_show_channels(chpsim_options& o)
	{ o.graph_opts.show_channels = true; }
static void __chpsim_no_show_channels(chpsim_options& o)
	{ o.graph_opts.show_channels = false; }
static void __chpsim_show_delays(chpsim_options& o)
	{ o.graph_opts.show_delays = true; }
static void __chpsim_no_show_delays(chpsim_options& o)
	{ o.graph_opts.show_delays = false; }
static void __ack_loaded_functions(chpsim_options&)
	{ entity::ack_loaded_functions = true; }
static void __no_ack_loaded_functions(chpsim_options&)
	{ entity::ack_loaded_functions = false; }

const chpsim::register_options_modifier
/***
@texinfo options/default.texi
@defopt {-f default}
Resets to default flags.  Has no negation.  
@end defopt
@end texinfo
***/
	chpsim::_default(
		"default", &__chpsim_default,
		"default options"), 
/***
@texinfo options/run.texi
@defopt {-f run}
Actually run the simulator's interpreter.  Enabled by default.
@samp{-f no-run} is explicitly needed when all that is desired
are diagnostic dumps.  
@end defopt
@end texinfo
***/
	chpsim::_run(
		"run", &__chpsim_run,
		"enable simulation run (default)"), 
	chpsim::_no_run(
		"no-run", &__chpsim_no_run,
		"disable simulation run"), 
/***
@texinfo options/dump-graph-alloc.texi
@defopt {-f dump-graph-alloc}
Diagnostic tool.  
Produce a textual dump of expression allocation after the internal 
whole-program graph has been constructed.  
@end defopt
@end texinfo
***/
	chpsim::_dump_graph_alloc(
		"dump-graph-alloc", &__chpsim_dump_graph_alloc,
		"show result of expression allocation"), 
	chpsim::_no_dump_graph_alloc(
		"no-dump-graph-alloc", &__chpsim_no_dump_graph_alloc,
		"suppress result of expression allocation (default)"),
/***
@texinfo options/check-structure.texi
@defopt {-f check-structure}
Run additional internal graph (nodes and edges) consistency checks.
Enabled by default.
@end defopt
@end texinfo
***/
	chpsim::_check_structure(
		"check-structure", &__chpsim_check_structure,
		"checks expression/node structure consistency (default)"), 
	chpsim::_no_check_structure(
		"no-check-structure", &__chpsim_no_check_structure,
		"disable structural consistency checks"),
/***
@texinfo options/dump-dot-struct.texi
@defopt {-f dump-dot-struct}
@cindex dot
@cindex event-graph
@cindex whole-program graph
Produce a textual netlist of the whole-program event graph in 
@command{dot} format @footnote{@command{dot} 
is the name of a program (and its input language)
that is part of AT&T's GraphViz package (open-source).}.
A list of options that tune this output can be found in
@ref{Graph Generation}.
@end defopt
@end texinfo
***/
	chpsim::_dump_dot_struct(
		"dump-dot-struct", &__chpsim_dump_dot_struct,
		"print dot-formatted graph structure"), 
	chpsim::_no_dump_dot_struct(
		"no-dump-dot-struct", &__chpsim_no_dump_dot_struct,
		"suppress dot-formatted graph structure (default)"),
/***
@texinfo options/show-event-index.texi
@defopt {-f show-event-index}
Annotate event nodes with their globally allocated indices.  Default off.  
@end defopt
@end texinfo
***/
	chpsim::_show_event_index(
		"show-event-index", &__chpsim_show_event_index,
		"for dot-graphs: show event indices in graph"), 
	chpsim::_no_show_event_index(
		"no-show-event-index", &__chpsim_no_show_event_index,
		"for dot-graphs: hide event indices"),
/***
@texinfo options/show-instances.texi
@defopt {-f show-instances}
Also show allocated instances as nodes.  Default off.  
@end defopt
@end texinfo
***/
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
/***
@texinfo options/cluster-processes.texi
@cindex cluster
@defopt {-f cluster-processes}
Wrap process subgraphs into clusters, 
which are enveloped in rectangular outlines.  Default off.  
@end defopt
@end texinfo
***/
	chpsim::_process_clusters(
		"cluster-processes", &__chpsim_process_clusters,
		"for dot-graphs: wrap process subgraphs into clusters"), 
	chpsim::_no_process_clusters(
		"no-cluster-processes", &__chpsim_no_process_clusters,
		"for dot-graphs: un-clustered process subgraphs"),
/***
@texinfo options/show-channels.texi
@defopt {-f show-channels}
Label channel edges with their channel names.  Default off.  
@end defopt
@end texinfo
***/
	chpsim::_show_channels(
		"show-channels", &__chpsim_show_channels,
		"for dot-graphs: display channel communication event edges"), 
	chpsim::_no_show_channels(
		"no-show-channels", &__chpsim_no_show_channels,
		"for dot-graphs: suppress channel-event edges"),
/***
@texinfo options/show-delays.texi
@defopt {-f show-delays}
Annotate event nodes with their delay values.  Default off.  
@end defopt
@end texinfo
***/
	chpsim::_show_delays(
		"show-delays", &__chpsim_show_delays,
		"for dot-graphs: wrap process subgraphs into clusters"), 
	chpsim::_no_show_delays(
		"no-show-delays", &__chpsim_no_show_delays,
		"for dot-graphs: un-clustered process subgraphs"),
/***
@texinfo options/ack-loaded-fns.texi
@defopt {-f ack-loaded-fns}
Print names of functions as they are loaded from dlopened modules.  
Default on.
Mostly useful for diagnostics.  
@end defopt
@end texinfo
***/
	chpsim::_ack_loaded_functions(
		"ack-loaded-fns", &__ack_loaded_functions,
		"Print names of functions as they are loaded (default)."),
	chpsim::_no_ack_loaded_functions(
		"no-ack-loaded-fns", &__no_ack_loaded_functions,
		"Suppress names of functions as they are loaded.");

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

