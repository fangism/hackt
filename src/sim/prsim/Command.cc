/**
	\file "sim/prsim/Command.cc"
	All built-in commands should be statically registered 
	in this translation unit to guarantee proper static object
	initialization ordering; the command_map must be constructed
	before entries are registered.  
	$Id: Command.cc,v 1.1.2.1 2006/01/12 06:13:08 fang Exp $
 */

#include "sim/prsim/Command.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include "sim/prsim/State.h"
#include "util/qmap.tcc"
#include "util/readline_wrap.h"
#include "util/libc.h"
#include "util/memory/excl_malloc_ptr.h"
#include "util/tokenize.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using std::copy;
using std::ostream_iterator;
using util::readline_wrapper;
using util::tokenize;
using util::excl_malloc_ptr;
//=============================================================================
// class Command method definitions

/**
	\return 0 on success.  
 */
int
Command::main(State& s, const string_list& args) const {
	if (_main) {
		return (*_main)(s, args);
	} else {
		cerr << "command is undefined." << endl;
		return -1;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
Command::usage(ostream& o) const {
	if (_usage) {
		(*_usage)(o);
	} else {
		o << "help/usage unavailable." << endl;
	}
}

//=============================================================================
// class CommandRegistry member/method definitions

CommandRegistry::command_map_type
CommandRegistry::command_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommandRegistry::command_entry::command_entry(const string& s, 
		const main_ptr_type m, const usage_ptr_type u) {
	typedef	command_map_type::value_type		value_type;
	typedef	command_map_type::mapped_type		mapped_type;
	typedef	command_map_type::const_iterator	const_iterator;
	mapped_type& probe(command_map[s]);
	if (probe) {
		cerr << "command \'" << s << "\' has already been registered."
			<< endl;
		THROW_EXIT;
	} else {
		probe = Command(m, u);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
CommandRegistry::list(ostream& o) {
	typedef	command_map_type::const_iterator	const_iterator;
	o << "available commands: " << endl;
	const_iterator i(command_map.begin());
	const const_iterator e(command_map.end());
	for ( ; i!=e; ++i) {
		o << '\t' << i->first << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param args the tokenized command line, including the "help" command
		as the first token.
 */
int
CommandRegistry::_help_main(State&, const string_list& args) {
	const size_t s = args.size();
	INVARIANT(s);
	const string_list::const_iterator argi(++args.begin());
	// skip first, which is 'help'
	if (argi != args.end()) {
		// take the first argument
		const command_map_type::const_iterator
			p(command_map.find(*argi));
		if (p != command_map.end()) {
			p->second.usage(cerr);
		} else {
			cerr << "Unknown command: " << args.front() << endl;
		}
	} else {
		// no argument, just list available commands
		list(cerr);
		_help_usage(cerr);
	}
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
CommandRegistry::_help_usage(ostream& o) {
	o << "help: lists available commands" << endl;
	o << "to get usage for a specific command, run: help <command>" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Echoes the argument list space-delimited back to stdout.  
 */
int
CommandRegistry::_echo_main(State&, const string_list& args) {
	INVARIANT(!args.empty());
	ostream_iterator<string> osi(cout, " ");
	copy(++args.begin(), args.end(), osi);
	cout << endl;
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
CommandRegistry::_echo_usage(ostream& o) {
	o << "echo: repeats arguments to stdout, space-delimited" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
CommandRegistry::_comment_main(State&, const string_list&) {
	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
CommandRegistry::_comment_usage(ostream& o) {
	o << "# or \'comment\' ignores the whole line." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	The most important built-in command.
 */
const CommandRegistry::command_entry
CommandRegistry::help_command("help", _help_main, _help_usage),
CommandRegistry::echo_command("echo", _echo_main, _echo_usage),
CommandRegistry::comment_command("comment", _comment_main, _comment_usage),
CommandRegistry::comment_command2("#", _comment_main, _comment_usage);

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Input terminate on end-of-file.  
	Non-interactive errors should result in termination.  
	TODO: error handling? convention?
		propose:
		< 0 aborting error,
		0 success,
		> 0 non-aborting error.
	\param s the simulator state object.
	\param interactive whether or not the command is interactive
		or from some source script.  
 */
int
CommandRegistry::interpret(State& s, const bool interactive) {
	static const char prompt[] = "prsim> ";
	readline_wrapper rl(prompt);
	// do NOT delete this line, it is already managed.
	const char* line = NULL;
	int status = 0;
	size_t lineno = 1;
	do {
		line = rl.gets();	// already eaten leading whitespace
	if (line) {
		string_list toks;
		tokenize(line, toks);
		status = execute(s, toks);
		++lineno;
	}
	} while (line && (!status || interactive));
	// end-line for neatness
	if (!line)	cout << endl;
	// not sure if the following is good idea
	if (status && !interactive) {
		// useful for tracing errors in sourced files
		cerr << "Error detected at line " << lineno <<
			", aborting commands." << endl;
		return status;
	} else	return 0;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
CommandRegistry::execute(State& st, const string_list& args) {
	const size_t s = args.size();
if (s) {
	const string_list::const_iterator argi(args.begin());
	const command_map_type::const_iterator
		p(command_map.find(*argi));
	if (p != command_map.end()) {
		return p->second.main(st, args);
	} else {
		cerr << "Unknown command: " << *argi << endl;
		return 1;
	}
} else	return 0;
}

//=============================================================================

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

