/**
	\file "main/shell.cc"
	Interactive shell for HACKT.  
	$Id: shell.cc,v 1.5 2005/12/13 04:15:48 fang Exp $
 */

#define	ENABLE_STACKTRACE		0

#include <iostream>
#include <cstring>
#include "main/shell.h"
#include "main/program_registry.h"
#include "util/stacktrace.h"
#include "main/main_funcs.h"
#include "util/persistent_object_manager.h"
#include "util/getopt_portable.h"
#include "util/libc.h"
#include "util/readline_wrap.h"

namespace HAC {
using util::persistent;
using util::persistent_object_manager;
using util::readline_wrapper;
#include "util/using_ostream.h"

//=============================================================================
class shell::options {
	// none
};	// end class options

//=============================================================================
// class shell static initializers

const char
shell::name[] = "shell";

const char
shell::brief_str[] = "Interactive environment shell";

const size_t
shell::program_id = register_hackt_program_class<shell>();

const char
shell::prompt[] = "hacksh> ";

//=============================================================================
shell::shell() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The main shell interpreter loop.  
	TODO: be able to source script files, etc...
	TODO: be able to set and maintain an include path.  
	TODO: sing a song.  "Daisy, daisy..."
	TODO: check for \ line continutation (change prompt)
	TODO: custom tab-completion
	TODO: load objects
	TODO: launch sub-shells
 */
int
shell::main(const int argc, char* argv[], const global_options&) {
	options opt;
	if (argc != 1) {
		usage();
		return 1;
	}
	if (parse_command_options(argc, argv, opt)) {
		cerr << "Error processing command-line options." << endl;
		return 1;
	}

	// cerr << "This doesn\'t do anything ... yet." << endl;
	// if banner is not suppressed...
	banner(cout);
{
	// command-line parsing:
	readline_wrapper rl(prompt);
	// do NOT delete this line, it's already managed.  
	const char* line;
	do {
		line = rl.gets();
	if (line) {
		// first, eat any leading whitespace
		const char* cursor = line;
		while (*cursor && isspace(*cursor)) cursor++;
		// check for special case: shell escape
		if (*cursor == '!') {
			cursor++;
			const int es = system(cursor);
			if (es) {
				cerr << "*** Exit " << es << endl;
			}
		} else {
		}
	}
	} while (line);
	cout << endl;
}
	farewell(cout);
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 0 on succsess, 1 on error.  
 */
int
shell::parse_command_options(const int argc, char* argv[], options& o) {
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
shell::usage(void) {
	cerr << "usage: " << name <<
		" (forthcoming...)" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	TODO: make the greeting language-specific, YEAH RIGHT!
 */
ostream&
shell::banner(ostream& o) {
	static const char wall[] =
		"\t#######+########=#######%#######%#######=#######+########";
	static const char side[] = "\t#\t\t\t\t\t\t\t#";
	return o << wall << endl << side << endl
		<< "\t#\t\tWelcome to the HACKT shell!\t\t#" << endl
		<< side << endl << wall << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The obligatory farewell quote.  
 */
ostream&
shell::farewell(ostream& o) {
#if 0
	return o << "Crawling out of the HACKT shell, farewell!" << endl;
#else
	return o <<
		"\t I\'m afraid.  I\'m afraid, Dave.  Dave, my mind is going."
		<< endl <<
		"\t   I can feel it.  I can feel it.  My mind is going..."
		<< endl;
        // "There is no question about it. I can feel it. I can feel it."
        // "I can feel it.  I\'m a... fraid. " << endl;
#endif
}

//=============================================================================
}	// end namespace HAC

