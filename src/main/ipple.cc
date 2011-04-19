/**
	\file "main/ipple.cc"
	Main program for Fang's Interactive Physics-driven Placement Engine
	Could be named "fipple"?
	$Id: ipple.cc,v 1.1.2.4 2011/04/19 01:08:43 fang Exp $
 */

#include <iostream>
#include <list>
#include "PR/placement_engine.h"
#include "PR/pr-command.h"
#include "sim/command_common.h"
#include "common/config.h"
#include "util/getopt_mapped.h"

#include "util/using_ostream.h"
using std::cin;
using PR::CommandRegistry;
using std::string;

//=============================================================================
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
namespace HAC {
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
	case 'a':
		o.autosave = true;
		o.autosave_name = optarg;
		break;
	case 'b':
		o.interactive = false;
		break;
	case 'd':
		cerr << "Warning: number of dimensions fixed to " <<
			o.dimensions << ", option ignored."  << endl;
		break;
	case 'h':
		o.help_only = true;
		break;
	case 'H':
		o.command_help_only = true;
		break;
	case 'i':
		o.interactive = true;
		break;
	case 'I':
		o.source_paths.push_back(optarg);
		break;
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

