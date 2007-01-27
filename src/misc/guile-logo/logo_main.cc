/**
	\file "misc/guile-logo/logo_main.cc"
	$Id: logo_main.cc,v 1.1 2007/01/27 05:50:58 fang Exp $
 */

#include "config.h"
#include <iostream>
#include <fstream>
#include "tortoise.h"

// guile's <libguile/scmconfig.h> also defines this, arg...
#ifdef	SIZEOF_LONG_LONG
#undef	SIZEOF_LONG_LONG
#endif
#include <guile/gh.h>

// #include "tortoise_wrap.cxx"
#include "util/c_decl.h"
#include "util/getopt_portable.h"

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
BEGIN_C_DECLS
extern void SWIG_init(void);
END_C_DECLS

using std::ostream;
using std::ifstream;
using std::cout;
using std::cerr;
using std::endl;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Execution options
 */
struct options {
	bool			interactive;
	bool			help_only;

	options() : interactive(true), help_only(false) { }
};	// end struct options

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	This hands control over entirely to the guile interpreter, 
	which repeats its interactive read-eval-print-loop.  
 */
static
void
logo_main_interactive(void* closure, int argc, char **argv) {
	cout << "Welcome to guile-logo!" << endl;
	SWIG_init();		// registers procedures
	// gh_repl(argc, argv);	// read-eval-print-loop
	scm_shell(argc, argv);	// read-eval-print-loop
	// nothing after here is reached...
	cout << "Exiting guile-logo..." << endl;
	sleep(2);
	cout << "Come back soon!" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
logo_main_script(void* closure, int argc, char **argv) {
	const char* fname = argv[argc-1];
	// cout << "file: " << fname << endl;
	ifstream f(fname);
	if (!f) {
		cerr << "Error opening file: " << fname << endl;
		return;
	}
	f.close();
	SWIG_init();		// registers procedures
	// gh_eval_file(fname);
	scm_c_primitive_load(fname);
	sleep(2);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
void
usage(ostream& o) {
	o << "guile-logo [file]" << endl;
#if 0
	"options:\n"
	"\t-b : batch-mode, non-interactive\n"
	"\t-i : interactive mode" << endl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static
int
parse_command_options(const int argc, char* argv[], options& o) {
	static const char optstring[] = "+bhi";
	int c;
while ((c = getopt(argc, argv, optstring)) != -1) {
switch (c) {
	case 'b': o.interactive = false; break;
	case 'i': o.interactive = true; break;
	case 'h': o.help_only = true; break;
	case ':': cerr << "Missing option argument" << endl;
		return 1;
	case '?': cerr << "Unknown option." << endl;
		return 1;
	default: cerr << "FATAL ERROR." << endl;
		return 1;
}	// end switch
}	// end while
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
main(int argc, char *argv[]) {
	options opt;
	if (parse_command_options(argc, argv, opt)) {
		usage(cerr);
		return 1;
	}

	logo::tortoise_init_window();	// X window initialization
switch (argc -optind) {
case 0:
	// enter the interpreter in loop
	// gh_enter(argc, argv, logo_main_interactive);
	scm_boot_guile(argc, argv, logo_main_interactive, NULL);
	break;
case 1: {
	// gh_enter(argc, argv, logo_main_script);
	scm_boot_guile(argc, argv, logo_main_script, NULL);
	break;
}
default:
	usage(cerr);
	return 1;
}
	return 0;
}

