/**
	\file "main/ipple.cc"
	Main program for Fang's Interactive Physics-driven Placement Engine
	Could be named "fipple"?
	$Id: ipple.cc,v 1.2 2011/05/03 19:20:53 fang Exp $
 */

#include <iostream>
#include <list>
#include "PR/placement_engine.hh"
#include "PR/pr-command.hh"
#include "sim/command_common.hh"
#include "common/config.hh"
#include "util/getopt_mapped.hh"

#include "util/using_ostream.hh"
using std::cin;
using HAC::PR::CommandRegistry;
using std::string;

//=============================================================================
namespace HAC {
namespace PR {
struct ipple_options {
	bool				help_only;
	bool				command_help_only;
	bool				interactive;
	bool				autosave;
	size_t				dimensions;
	string				autosave_name;
	typedef	std::list<string>	source_paths_type;
	/// include search paths for sources
	source_paths_type	source_paths;
	// output
	ipple_options() : help_only(false), 
		command_help_only(false),
		interactive(true),
		autosave(false),
		dimensions(PR_DIMENSIONS),
		autosave_name(), 
		source_paths() { }
};	// end struct ipple_options
}	// end namespace PR

//=============================================================================
// could go to "main/ippl.h" in the future
class ipple {
public:
	typedef	PR::ipple_options		options;

	static const char			name[];
	static const char			brief[];

	static
	int
	main(const int, char*[]);	// const global_options&

private:
	static
	void
	usage(void);

	static
	int
	parse_command_options(const int, char*[], options&);

};	// end class ipple
}	// end namespace HAC

//=============================================================================
namespace HAC {
const char
ipple::name[] = "ipple";

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
ipple::usage(void) {
	cerr << "usage: " << name << " [options]" << endl;
	cerr << "An Interactive Physics-driven Placement Engine" << endl;
	cerr << "options:\n"
"\t-a <file> : auto-save session upon exit\n"
"\t-b : batch-mode, non-interactive (promptless)\n"
"\t-d <int> : set number of dimensions of space (default: "
		<< PR::PR_DIMENSIONS << ")\n"
// "\t-f <flag> : general options modifiers (listed below)\n"
"\t-h : print usage help and exit (objfile optional)\n"
"\t-H : print in-program command help and exit\n"
"\t-i : interactive (default)\n"
"\t-I <path> : include path for scripts (repeatable)\n"
"\t-v : print version and exit" << endl;
// TODO: links to documentation pages
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
ipple::parse_command_options(const int argc, char* argv[], options& o) {
	static const char optstring[] = "+a:bd:hHiI:v";
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
		o.interactive = false;
		break;
/***
@texinfo opt/option-d.texi
@defopt -d N
Not yet supported.
Simulate in @var{N}-dimension space.
Current fixed to 3D only.  
@end defopt
@end texinfo
***/
	case 'd':
		cerr << "Warning: number of dimensions fixed to " <<
			o.dimensions << ", option ignored."  << endl;
		break;
/***
@texinfo opt/option-h.texi
@defopt -h
Print command-line options help and exit.
@end defopt
@end texinfo
***/
	case 'h':
		o.help_only = true;
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

}	// end switch
}	// end while
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
ipple::main(const int argc, char* argv[]) {
	options opt;
	// parse options
	if (parse_command_options(argc, argv, opt)) {
		cerr << "Error in command invocation." << endl;
		usage();
		return 1;
	}
	if (opt.help_only) {
		usage();
		return 0;
	}
	if (opt.command_help_only) {
		util::string_list args;
		args.push_back("help");
		args.push_back("all");
		PR::Help::main(args);
		return 0;
	}
	// output file name?

	PR::placement_engine P(opt.dimensions);	// fixed: 3D
	if (opt.autosave) {
		P.autosave(opt.autosave_name);
	}
	P.import_source_paths(opt.source_paths);
	CommandRegistry::prompt = "ipple> ";
	const CommandRegistry::readline_init __rl__;
	const int ret = CommandRegistry::interpret(P, cin, opt.interactive);
	if (ret) {
		return 1;
	}
	return 0;
}

//=============================================================================
}	// end namespace HAC

//=============================================================================
int
main(const int argc, char* argv[]) {
	return HAC::ipple::main(argc, argv);
}

