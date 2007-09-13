/**
	\file "main/guile.cc"
	Main module for new CHPSIM.
	$Id: guile.cc,v 1.6 2007/09/13 01:14:15 fang Exp $
 */

#define	ENABLE_STACKTRACE			0
#include "util/static_trace.h"

DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
#include "main/guile.h"
#include "main/program_registry.h"	// to register with hackt's dispatcher
#include "main/main_funcs.h"		// for save/load_module()
#include "main/options_modifier.tcc"
#include "main/compile_options.h"
#include "main/global_options.h"
#include "guile/libhackt-wrap.h"
#include "util/getopt_mapped.h"		// for getopt()
#include "util/libguile.h"

namespace HAC {
#include "util/using_ostream.h"
using guile_wrap::obj_module;		// global module pointer

//=============================================================================
template class options_modifier_policy<guile_options>;

//=============================================================================
/**
	Has a lot in common with guile_options (main/prsim.cc)
 */
struct guile_options {
	bool			interactive;
	bool			help_only;
	compile_options		comp_opt;
	guile_options() : interactive(true), help_only(false) { }
};	// end class guile_options

//=============================================================================
// class guile static initializers

const char
guile::name[] = "guile";

const char
guile::brief_str[] = "embedded guile interpreter preloaded with hackt libs";

#ifndef	WITH_MAIN
const size_t
guile::program_id = register_hackt_program_class<guile>();
#endif

//=============================================================================
// class guile member definitions

guile::guile() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
__guile_main(int argc, char* argv[]) {
#if 0
	libhackt_guile_init();
#else
	// load definitions into a guile module, and load the module
	scm_init_hackt_libhackt_primitives_module();
	scm_c_use_module("hackt hackt-primitives");
#endif
	scm_shell(argc, argv);	// read-eval-print
	// never returns :S
	// how the f--- am I supposed to clean up memory?
	// A: atexit()
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
guile::main_interactive(void* closure, int argc, char** argv) {
	cout << "Welcome to hackt-guile!" << endl;
	scm_c_eval_string("(set-repl-prompt! \"hacguile> \")");
	__guile_main(argc, argv);
	// never returns :S
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Just expect redirected input from stdin.
 */
void
guile::main_script(void* closure, int argc, char** argv) {
	scm_c_eval_string("(set-repl-prompt! \"\")");	// disable-prompt
	libhackt_guile_init();
	__guile_main(argc, argv);
	// never returns :S
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Add your own getopt(), parse_command_options()...
 */
int
guile::main(int argc, char* argv[], const global_options&) {
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
		usage();
		return 0;
	}
	if (optind+1 != argc) {
		cerr << "Error: Exactly one non-option argument "
			"allowed/required." << endl;
		usage();
		return 1;
	}
	const char* const ofn = argv[optind];
if (opt.comp_opt.compile_input) {
	obj_module = parse_and_check(ofn, opt.comp_opt);
} else {
	if (!check_object_loadable(ofn).good)
		return 1;
	obj_module = load_module(ofn);
	// load_module_debug(ofn);
}
	if (!obj_module)
		return 1;

	// got error message?
//	obj_module->dump(cerr);
	// automatically allocate as needed
	if (!obj_module->is_allocated()) {
		if (!obj_module->allocate_unique().good) {
			cerr << "ERROR in allocating.  Aborting." << endl;
			return 1;
		}
	}
	if (opt.interactive) {
		scm_boot_guile(argc -optind, argv +optind,
			main_interactive, NULL);
	} else {
		scm_boot_guile(argc -optind, argv +optind,
			main_script, NULL);
	}
	// never returns...
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: use getopt_mapped(), see cflat for example.
 */
int
guile::parse_command_options(const int argc, char* argv[], options& o) {
	static const char optstring[] = "+bcC:hi";
	int c;
while((c = getopt(argc, argv, optstring)) != -1) {
switch (c) {
	case 'b':
		o.interactive = false;
		break;
	case 'c':
	case 'C': {
		const int r = parse_create_flag(c, o.comp_opt);
		if (r) return r;
	}
		break;
#if 0
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
#endif
	case 'h':
		o.help_only = true;
		break;
	case 'i':
		o.interactive = true;
#if 0
	case 'I':
		o.source_paths.push_back(optarg);
#endif
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
guile::usage(void) {
	cerr << "usage: " << name << " <hackt-obj-file>" << endl;
	cerr << "options:" << endl;
	cerr << "\t-b : batch-mode, non-interactive (promptless)" << endl;
	cerr << "\t-c : input file is a source, not an object" << endl;
	cerr << "\t-C <opts> : forward flags to compiler-driver" << endl;
//	cerr << "\t-f <flag> : general options modifiers (listed below)" << endl;
	cerr << "\t-h : print this help and exit" << endl;
	cerr << "\t-i : interactive (default)" << endl;
//	cerr << "\t-I <path> : include path for scripts (repeatable)" << endl;
//	cerr << "\t-O <0..1> : expression optimization level" << endl;
#if 0
        const size_t flags = options_modifier_map.size();
	if (flags) {
		cerr << "flags (" << flags << " total):" << endl;
		dump_options_briefs(cerr);
	}
#endif
}

//=============================================================================
}	// end namespace HAC

#ifdef	WITH_MAIN
/**
	Assumes no global hackt options.  
 */
int
main(const int argc, char* argv[]) {
	const HAC::global_options g;
	return HAC::guile::main(argc, argv, g);
}
#endif	// WITH_MAIN

DEFAULT_STATIC_TRACE_END

