/**
	\file "sim/command_common.tcc"
	Library of template command implementations, re-usable with
	different state types.  
	$Id: command_common.tcc,v 1.24 2011/05/23 01:10:50 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_COMMON_TCC__
#define	__HAC_SIM_COMMAND_COMMON_TCC__

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <algorithm>
#include <functional>

#include "sim/command_common.hh"
#include "sim/command_base.hh"
#include "sim/command_category.hh"
#include "sim/command_registry.hh"
#include "sim/trace_common.hh"		// for trace_manager_base
#include "parser/instref.hh"
#include "Object/expr/dlfunction.hh"
#include "Object/module.hh"
#include "common/TODO.hh"
#include "util/compose.hh"
#include "util/string.tcc"
#include "util/utypes.h"
#include "util/timer.hh"
#include "util/stacktrace.hh"

namespace HAC {
namespace SIM {
using std::ios_base;
using std::ofstream;
using std::for_each;
using std::ptr_fun;
using std::mem_fun_ref;
using util::strings::string_to_num;
#include "util/using_ostream.hh"
USING_UTIL_COMPOSE

//=============================================================================
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Help, "help",
	"show available commands and categories")

/**
	\param args the tokenized command line, including the "help" command
		as the first token.
 */
template <class State>
int
Help<State>::main(const string_list& args) {
	INVARIANT(args.size());
	const string_list::const_iterator argi(++args.begin());
	// skip first, which is 'help'
	if (argi != args.end()) {
		if (!command_registry_type::help_category(cout, *argi) &&
			!command_registry_type::help_command(cout, *argi)) {
			cerr << "Unknown category/command: " << *argi<< endl;
		}
	} else {
		// no argument, just list available commands
		command_registry_type::list_categories(cout);
		usage(cout);
	}
	return command_type::NORMAL;
}

template <class State>
int
Help<State>::main(state_type&, const string_list& args) {
	return main(args);
}

template <class State>
void
Help<State>::usage(ostream& o) {
	o << "help: lists available commands and categories" << endl;
	o << "to get usage for a specific command or category, "
		"run: help <name>" << endl;
	o << "help all: lists all commands across all categories" << endl;
}

//-----------------------------------------------------------------------------
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(All, "all", "show all commands")

template <class State>
int
All<State>::main(state_type&, const string_list&) {
	cout << "usage: help all" << endl;
	return command_type::NORMAL;
}

template <class State>
void
All<State>::usage(ostream& o) {
	command_registry_type::list_commands(o);
}

//-----------------------------------------------------------------------------
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Alias, "alias",
	"defines alias to auto-expand by the interpreter")

/**
	TODO: Consider storing interpreter aliases in the State object, 
	instead of global registration.  
 */
template <class State>
int
Alias<State>::main(state_type&, const string_list& a) {
if (a.size() < 3) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	typedef	string_list::const_iterator	const_iterator;
	string_list ac(a);
	ac.pop_front();		// drop the command
	const string al(ac.front());
	ac.pop_front();		// extract alias name, use the rest
	return command_registry_type::add_alias(al, ac);
}
}

template <class State>
void
Alias<State>::usage(ostream& o) {
	o << "alias <name> <command> [args...]" << endl;
	o <<
"defines an alias that expands into another command and args\n"
"Aliases may reference other aliases as long as no cycles are formed."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(UnAlias, "unalias",
	"undefines an alias command")

/**
	Un-aliases one command at a time.  
 */
template <class State>
int
UnAlias<State>::main(state_type&, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	return command_registry_type::unalias(a.back());
}
}

template <class State>
void
UnAlias<State>::usage(ostream& o) {
	o << "unalias <name>" << endl;
	o << "undefines an existing command alias" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(UnAliasAll, "unaliasall",
	"undefines all alias commands")

/**
	Un-aliases all alias commands.  
 */
template <class State>
int
UnAliasAll<State>::main(state_type&, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	return command_registry_type::unalias_all();
}
}

template <class State>
void
UnAliasAll<State>::usage(ostream& o) {
	o << "unaliasall" << endl;
	o << "undefines all existing command aliases" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Aliases, "aliases",
	"show all registered command aliases")

template <class State>
int
Aliases<State>::main(state_type&, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	command_registry_type::list_aliases(cout);
	return command_type::NORMAL;
}
}

template <class State>
void
Aliases<State>::usage(ostream& o) {
	o << "aliases" << endl;
	o << "lists all active defined command aliases" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Interpret, "interpret",
	"re-open stdin interactively as a subshell")

template <class State>
int
Interpret<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	return command_registry_type::interpret_stdin(s);
}
}

template <class State>
void
Interpret<State>::usage(ostream& o) {
	o << name << endl;
	o << "Re-opens standard-input interactive as a subshell." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(EchoCommands, "echo-commands",
	"whether or not each command is echoed back")

template <class State>
int
EchoCommands<State>::main(state_type&, const string_list& a) {
switch (a.size()) {
case 1:
	cout << "echo-commands is " <<
		(command_registry_type::echo_commands ? "on" : "off") << endl;
	return command_type::NORMAL;
case 2: {
	const string& arg(a.back());
	if (arg == "on") {
		command_registry_type::echo_commands = true;
	} else if (arg == "off") {
		command_registry_type::echo_commands = false;
	} else {
		cerr << "Bad argument." << endl;
		usage(cerr);
		return command_type::BADARG;
	}
	return command_type::NORMAL;
}
default:
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
}

template <class State>
void
EchoCommands<State>::usage(ostream& o) {
	o << "echo-commands [on|off]" << endl;
	o << "Enable or disable printing of each command as it is interpreted."
 		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(MeasTime, "meas-time",
	"report time spent in command")

template <class State>
int
MeasTime<State>::main(state_type& s, const string_list& a) {
if (a.size() >= 2) {
	string_list::const_iterator i(++a.begin());
	string_list cmd(i, a.end());
	// taken from command_registry_type::interpret_line:
	// FIXME: this will not process escape-to-shell commands
	// expand aliases first
	if (command_registry_type::expand_aliases(cmd) != command_type::NORMAL) {
		return command_type::BADARG;
	}
	const string tag(cmd.front() + " ...");
	const util::timer T(true, cout, tag.c_str());
	int ret = command_registry_type::execute(s, cmd);
	if (ret == command_type::INTERACT) {
		ret = command_registry_type::interpret_stdin(s);
	}
	return ret;
} else {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
}

template <class State>
void
MeasTime<State>::usage(ostream& o) {
	o << name << " command...\n";
	o << "Reports time spent in a given command." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Repeat, "repeat",
	"execute a command several times")

template <class State>
int
Repeat<State>::main(state_type& s, const string_list& a) {
if (a.size() > 2) {
	string_list::const_iterator i(++a.begin());
	const string& cnt(*i);
	++i;
	string_list cmd(i, a.end());
	int n;
	if (string_to_num(cnt, n) || n < 0) {
		usage(cerr << "usage: ");
		return command_type::BADARG;
	}
	// taken from command_registry_type::interpret_line:
	// FIXME: this will not process escape-to-shell commands
	// expand aliases first
	if (command_registry_type::expand_aliases(cmd) != command_type::NORMAL) {
		return command_type::BADARG;
	}
	int j = 0;
	int ret = command_type::NORMAL;
	for ( ; j<n && ret == command_type::NORMAL; ++j) {
		ret = command_registry_type::execute(s, cmd);
		if (ret == command_type::INTERACT) {
			ret = command_registry_type::interpret_stdin(s);
		}
	}
	if (j < n) {
		cout << "*** Repeat-loop stopped after only " << j <<
			" of " << n << " iterations." << endl;
	}
	return ret;
} else {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
}

template <class State>
void
Repeat<State>::usage(ostream& o) {
	o << name << " <int> command...\n";
	o << "Repeats the given command a fixed number of times." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(History, "history", 
        "prints enumerated history of commands stdout")

template <class State>
int
History<State>::main(state_type&, const string_list& args) {
	const size_t a = args.size();
if (a > 3) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
	string_list::const_iterator i(++args.begin());
	int f = 0;
	int l = -1;
if (a > 1) {
	const string& lower(*i);
	if (string_to_num(lower, f)) {
		usage(cerr << "usage: ");
		return command_type::BADARG;
	}
if (a > 2) {
	++i;
	const string& upper(*i);
	if (string_to_num(upper, l)) {
		usage(cerr << "usage: ");
		return command_type::BADARG;
	}
}
}
	command_registry_type::dump_history(cout, f, l);
	return command_type::NORMAL;
}

template <class State>
void
History<State>::usage(ostream& o) {
	o << name << " [start [end]]:\n"
	"If no arguments given, then print entire command history.\n"
	"If only START is given, print to the end.\n"
	"If START is negative, count backwards from last line.\n"
	"If END is positive, count forward from START,\n"
	"If END is negative, count backward from last line."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(HistoryRerun, "history-rerun", 
        "shortcut for re-running previous commands")

template <class State>
int
HistoryRerun<State>::main(state_type& st, const string_list& args) {
	const size_t a = args.size();
	string_list::const_iterator i(++args.begin());
	int f = 0;
	int l = 1;	// default one line only
if (a < 2 || a > 3) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	INVARIANT(i != args.end());
	const string& lower(*i);
	if (string_to_num(lower, f)) {
		usage(cerr << "usage: ");
		return command_type::BADARG;
	}
if (a == 3) {
	++i;
	INVARIANT(i != args.end());
	const string& upper(*i);
	if (string_to_num(upper, l)) {
		usage(cerr << "usage: ");
		return command_type::BADARG;
	}
}
}
	return command_registry_type::rerun(st, f, l);
}

template <class State>
void
HistoryRerun<State>::usage(ostream& o) {
	o << name << " start [end]:\n"
	"Reruns partial command history.\n"
	"If START is non-negative, use START as the first line.\n"
	"If START is negative, count backwards from last line.\n"
	"If end is omitted, rerun line START only.\n"
	"If END is positive, count forward from START,\n"
	"If END is negative, count backward from last line."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(HistorySave,
	"history-save", "write command history to file")

template <class State>
int
HistorySave<State>::main(state_type&, const string_list& args) {
	const size_t s = args.size();
	string_list::const_iterator i(++args.begin());
if (s != 2) {
	// error
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
	return command_registry_type::write_history(*i);
}

template <class State>
void
HistorySave<State>::usage(ostream& o) {
	o << name << " FILE" << endl;
	o << "Writes command-line history to file." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(HistoryNonInteractive,
	"history-noninteractive",
	"keep history of non-interactive commands")

template <class State>
int
HistoryNonInteractive<State>::main(state_type&, const string_list& args) {
	const size_t s = args.size();
	string_list::const_iterator i(++args.begin());
if (s > 2) {
	// error
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
if (s > 1) {
	const string& a(*i);
	if (a == "on") {
		command_registry_type::keep_noninteractive_history = true;
	} else if (a == "off") {
		command_registry_type::keep_noninteractive_history = false;
	} else {
		usage(cerr << "usage: ");
		return command_type::BADARG;
	}
} else {
	// just show
	cout << "history-noninteractive: " <<
	(command_registry_type::keep_noninteractive_history ? "on" : "off")
		<< endl;
}
	return command_type::NORMAL;
}

template <class State>
void
HistoryNonInteractive<State>::usage(ostream& o) {
	o << name << " [on|off]" << endl;
	o << "Turns recording of non-interactive commands on or off." << endl;
	o << "When no argument is give, just reports the current mode." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Source, "source",
	"execute commands from script file(s)")

/**
	TODO: additional flags to determine error handling, verbosity...
	Include paths for source files?
 */
template <class State>
int
Source<State>::main(state_type& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	const string_list::const_iterator e(a.end());
	int status = command_type::NORMAL;
	for ( ; i!=e && !status; ++i) {
		// termination will depend on error handling
		status = command_registry_type::source(s, *i);
	}
	if (status) {
		--i;
		cerr << "Error encountered during source \"" <<
			*i << "\"." << endl;
	}
	return status;
}
}

template <class State>
void
Source<State>::usage(ostream& o) {
	o << "source <file(s)>: " << brief << endl;
	o << "read in and run commands from the named file(s)" << endl;
	o << "sourcing terminates upon first error encountered." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(AddPath, "addpath",
	"add search paths for source scripts")

template <class State>
int
AddPath<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.add_source_path(a.back());
	return command_type::NORMAL;
}
}

template <class State>
void
AddPath<State>::usage(ostream& o) {
	o << "addpath <path>" << endl;
	o <<
"adds a path to the search-list used to locate sourced command files"
<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Paths, "paths",
	"show search paths for source scripts")

template <class State>
int
Paths<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.dump_source_paths(cout);
	return command_type::NORMAL;
}
}

template <class State>
void
Paths<State>::usage(ostream& o) {
	o << "paths" << endl;
	o << "show current list of source search paths" << endl;
}

//-----------------------------------------------------------------------------
// directory commands

DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(ChangeDir, "cd",
	"change current working instance/directory")

template <class State>
int
ChangeDir<State>::main(state_type& s, const string_list& a) {
if (a.size() > 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	typedef	string_list::const_iterator	const_iterator;
	string_list ac(a);
	ac.pop_front();		// drop the command
	// verify that directory is valid, else undo cd
	const string save(command_registry_type::working_dir());
	if (a.size() == 2) {
		command_registry_type::change_dir(ac.back());
		const string newdir(command_registry_type::working_dir());
		// optional: confirm and print new working dir
		if (newdir != "" && 
			parser::parse_process_to_index(newdir,
				s.get_module().get_footprint()).index
					== INVALID_PROCESS_INDEX) {
			cerr << "Invalid process/directory: " << newdir << endl;
			command_registry_type::change_dir_abs(save);
			return command_type::BADARG;
		}
	} else {
		// go back to root
		command_registry_type::change_dir("");
	}
	return command_type::NORMAL;
}
}

template <class State>
void
ChangeDir<State>::usage(ostream& o) {
	o << "cd [name]" << endl;
	o <<
"Change current working scope (process hierarchy), with which all relative "
"references are prefixed.  With no arguments, changes to the root/top level.\n"
"Each ../ returns up a level of hierarchy.  Standard separator is . (dot)."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(PushDir, "pushd",
	"pushes new working instance/directory onto stack")

template <class State>
int
PushDir<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	typedef	string_list::const_iterator	const_iterator;
	string_list ac(a);
	ac.pop_front();		// drop the command
	// verify that directory is valid, else undo cd
	const string save(command_registry_type::working_dir());
	command_registry_type::push_dir(ac.back());
	const string newdir(command_registry_type::working_dir());
	if (newdir != "" && 
		parser::parse_process_to_index(newdir,
			s.get_module().get_footprint()).index
				== INVALID_PROCESS_INDEX) {
		cerr << "Invalid process/directory: " << newdir << endl;
		command_registry_type::pop_dir();
		return command_type::BADARG;
	}
	return command_type::NORMAL;
}
}

template <class State>
void
PushDir<State>::usage(ostream& o) {
	o << "pushd <name>" << endl;
	o <<
"Pushes working scope (process hierarchy), onto directory stack.\n"
"See also help for \'cd\'."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(PopDir, "popd",
	"pops last working instance/directory off stack")

template <class State>
int
PopDir<State>::main(state_type&, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	// check for underflow
	if (command_registry_type::pop_dir())
		return command_type::BADARG;
	else	return command_type::NORMAL;
}
}

template <class State>
void
PopDir<State>::usage(ostream& o) {
	o << "popd" << endl;
	o << "Pops working scope (process hierarchy) off of directory stack."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(WorkingDir, "pwd",
	"show current working instance/directory/scope")

template <class State>
int
WorkingDir<State>::main(state_type&, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	command_registry_type::show_working_dir(cout) << endl;
	return command_type::NORMAL;
}
}

template <class State>
void
WorkingDir<State>::usage(ostream& o) {
	o << "pwd" << endl;
	o << "Print current working scope (process hierarchy)." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Dirs, "dirs",
	"print entire instance/directory stack")

template <class State>
int
Dirs<State>::main(state_type&, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	// check for underflow
	command_registry_type::show_dirs(cout) << endl;
	return command_type::NORMAL;
}
}

template <class State>
void
Dirs<State>::usage(ostream& o) {
	o << "dirs" << endl;
	o << "Prints entire directory stack." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(LS, "ls",
	"list subinstances of the referenced instance")

template <class State>
int
LS<State>::main(state_type& s, const string_list& a) {
	const entity::footprint& f(s.get_module().get_footprint());
if (a.size() > 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	string t;
	if (a.size() == 1) {
		t = command_registry_type::working_dir();
	} else {
		t = command_registry_type::prepend_working_dir(a.back());
	}
	if (t.empty()) {
		t = ".";
	}
	if (parser::parse_name_to_members(cout, t, f))
		return command_type::BADARG;
	else	return command_type::NORMAL;
}
}

template <class State>
void
LS<State>::usage(ostream& o) {
	o << "ls [name]" << endl;
o << "prints list of subinstances of the referenced instance\n"
"The name argument is interpreted as relative to the current directory.\n"
"\"ls .\" or just \"ls\" lists instances in the current working directory"
<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(What, "what",
	"print type information of named entity")

template <class State>
int
What<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (parser::parse_name_to_what(cout,
			command_registry_type::prepend_working_dir(a.back()),
			s.get_module().get_footprint()))
		return command_type::BADARG;
	else	return command_type::NORMAL;
}
}

template <class State>
void
What<State>::usage(ostream& o) {
	o << "what <name>" << endl;
	o << "prints the type/size of the referenced instance(s)" << endl;
	o << "By default, references are relative to current working directory." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Who, "who",
	"print aliases of node or structure")

template <class State>
int
Who<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	const string r(command_registry_type::prepend_working_dir(a.back()));
	cout << "aliases of \"" << r << "\":" << endl;
	if (parser::parse_name_to_aliases(cout, r, 
			s.get_module().get_footprint(), s._dump_flags, " ")) {
		return command_type::BADARG;
	} else {
		cout << endl;
		return command_type::NORMAL;
	}
}
}

template <class State>
void
Who<State>::usage(ostream& o) {
	o << "who <name>" << endl;
	o << "prints all aliases (equivalent names) of the referenced instance"
		<< endl;
	o << "By default, references are relative to current working directory." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(WhoNewline, "who-newline",
	"print aliases of node or structure, newline separated")

template <class State>
int
WhoNewline<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	const string r(command_registry_type::prepend_working_dir(a.back()));
	cout << "aliases of \"" << r << "\":" << endl;
	if (parser::parse_name_to_aliases(cout, r,
			s.get_module().get_footprint(), 
			s._dump_flags, "\n")) {
		return command_type::BADARG;
	} else {
		cout << endl;
		return command_type::NORMAL;
	}
}
}

template <class State>
void
WhoNewline<State>::usage(ostream& o) {
	o << "who-newline <name>" << endl;
	o << "prints all aliases (equivalent names) of the referenced instance"
		<< endl;
	o << "By default, references are relative to current working directory." << endl;
}

//-----------------------------------------------------------------------------
// state initialization commands

DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Initialize, "initialize",
	"resets simulator state and event queue, preserving modes")

template <class State>
int
Initialize<State>::main(state_type& s, const string_list&) {
	s.initialize();
	return command_type::NORMAL;
}

template <class State>
void
Initialize<State>::usage(ostream& o) {
	o << "initialize: " << brief << endl;
	o << "set all nodes to unknown state and clear the event queue" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Reset, "reset",
	"resets simulator state, queue, and modes (fresh start)")

template <class State>
int
Reset<State>::main(state_type& s, const string_list&) {
	s.reset();
	command_registry_type::reset_dirs();
	return command_type::NORMAL;
}

template <class State>
void
Reset<State>::usage(ostream& o) {
	o << "reset: " << brief << endl;
	o << "restart the entire simulation as if it was just launched" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Queue, "queue", "show event queue")

template <class State>
int
Queue<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.dump_event_queue(cout);
	return command_type::NORMAL;
}
}

template <class State>
void
Queue<State>::usage(ostream& o) {
	o << "queue: " << brief << endl;
	o << "prints events pending in the event queue" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Seed48, "seed48", "set/get random number seed")

template <class State>
int
Seed48<State>::main(State&, const string_list& a) {
const size_t sz = a.size();
switch (sz) {
case 1: {
	ushort sd[3] = {0, 0, 0};
	// grab seed (destructive)
	const ushort* tmp = seed48(sd);
	sd[0] = tmp[0];
	sd[1] = tmp[1];
	sd[2] = tmp[2];
	cout << "seed48 = " << sd[0] << ' ' << sd[1] << ' ' << sd[2] << endl;
	// restore it
	seed48(sd);
	break;
}
case 4: {
	ushort sd[3];
	string_list::const_iterator i(a.begin());
	// pre-increment to skip first command token
	if (string_to_num(*++i, sd[0]) ||
		string_to_num(*++i, sd[1]) ||
		string_to_num(*++i, sd[2])) {
		usage(cerr << "usage: ");
		return command_type::BADARG;
	}
	seed48(sd);
	break;
}
default:
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
	return command_type::NORMAL;
}

template <class State>
void
Seed48<State>::usage(ostream& o) {
	o << "seed48 [int int int]" << endl;
	o <<
"Shows the current seed values used by the random number generator.\n"
"Setting the seed requires 3 (unsigned short) integers." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Save, "save",
	"saves simulation state to a checkpoint")

template <class State>
int
Save<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	const string& fname(a.back());
	std::ofstream f(fname.c_str(), ios_base::binary);
	if (!f) {
		cerr << "Error opening file \"" << fname <<
			"\" for writing." << endl;
		return command_type::BADFILE;
	}
	if (s.save_checkpoint(f)) {
		// error-handling?
		cerr << "Error writing checkpoint!" << endl;
		return command_type::UNKNOWN;
	}
	return command_type::NORMAL;
}
}

/**
	TODO: customize usage message (file extension) with template policy. 
 */
template <class State>
void
Save<State>::usage(ostream& o) {
	o << "save <file>" << endl;
	o << brief << endl;
	o << "(recommend some extension like .prsimckpt)" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Load, "load",
	"loads simulation state from a checkpoint")

template <class State>
int
Load<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	const string& fname(a.back());
	std::ifstream f(fname.c_str(), ios_base::binary);
	if (!f) {
		cerr << "Error opening file \"" << fname <<
			"\" for reading." << endl;
		return command_type::BADFILE;
	}
	if (s.load_checkpoint(f)) {
		// error-handling?
		cerr << "Error loading checkpoint!" << endl;
		return command_type::UNKNOWN;
	}
	return command_type::NORMAL;
}
}

template <class State>
void
Load<State>::usage(ostream& o) {
	o << "load <file>" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(AutoSave, "autosave",
	"automatically save checkpoint upon exit")

template <class State>
int
AutoSave<State>::main(state_type& s, const string_list& a) {
const size_t sz = a.size();
if ((sz != 2) && (sz != 3)) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
	static const string empty;
	const string& cname(sz == 3 ? a.back() : empty);
	const string& arg(*++a.begin());
	if (arg == "1" || (arg == "on") || (arg == "yes")) {
		s.autosave(true, cname);
	} else if (arg == "0" || (arg == "off") || (arg == "no")) {
		s.autosave(false, cname);
	} else {
		cerr << "Error: invalid argument." << endl;
		usage(cerr);
		return command_type::BADARG;
	}
	return command_type::NORMAL;
}

template <class State>
void
AutoSave<State>::usage(ostream& o) {
	o << name << " <on|off> [file]" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Confirm, "confirm", 
	"confirm assertions verbosely")

DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(NoConfirm, "noconfirm", 
	"confirm assertions silently (default)")

template <class State>
int
Confirm<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.confirm_asserts(true);
	return command_type::NORMAL;
}
}

template <class State>
int
NoConfirm<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.confirm_asserts(false);
	return command_type::NORMAL;
}
}

template <class State>
void
Confirm<State>::usage(ostream& o) { 
o << name << " : " << brief << endl;
}

template <class State>
void
NoConfirm<State>::usage(ostream& o) {
o << name << " : " << brief << endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(AssertQueue, "assert-queue",
	"assert that the event queue is not empty")

template <class State>
int
AssertQueue<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (s.pending_live_events()) {
		if (s.confirm_asserts()) {
			cout << "Event queue is non-empty, as expected."
				<< endl;
		}
		return command_type::NORMAL;
	} else {
		const error_policy_enum e(s.get_assert_fail_policy());
		if (e != ERROR_IGNORE) {
		cout << "assert failed: expecting non-empty event queue."
			<< endl;
		}
		return error_policy_to_status(e);
	}
}
}

template <class State>
void
AssertQueue<State>::usage(ostream& o) {
	o << "assert-queue" << endl;
	o << "signal an error if the event queue is empty" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(AssertNQueue, "assertn-queue",
	"assert that the event queue is empty")

template <class State>
int
AssertNQueue<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (!s.pending_live_events()) {
		if (s.confirm_asserts()) {
			cout << "Event queue is empty, as expected." << endl;
		}
		return command_type::NORMAL;
	} else {
		const error_policy_enum e(s.get_assert_fail_policy());
		if (e != ERROR_IGNORE) {
		cout << "assert failed: expecting empty event queue."
			<< endl;
		}
		return error_policy_to_status(e);
	}
}
}

template <class State>
void
AssertNQueue<State>::usage(ostream& o) {
	o << "assertn-queue" << endl;
	o << "signal an error if the event queue is not empty" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Time, "time",
	"display current simulation time")

/**
	TODO:
	Allow "time x" to manually set the time if the event queue is empty. 
	Useful for manually resetting the timer.  
	Allow "time +x" to advance by time, like step.  
		(or reserve for the advance command?)
 */
template <class State>
int
Time<State>::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	cout << "time: " << s.time() << endl;
	return command_type::NORMAL;
}
}

template <class State>
void
Time<State>::usage(ostream& o) {
	o << "time" << endl;
	o << "shows the current time" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(TimeFmt, "time-fmt",
	"display current simulation time")

/**
	TODO:
	Allow "time x" to manually set the time if the event queue is empty. 
	Useful for manually resetting the timer.  
	Allow "time +x" to advance by time, like step.  
		(or reserve for the advance command?)
 */
template <class State>
int
TimeFmt<State>::main(State& s, const string_list& a) {
if (a.size() == 1) {
	usage(cerr << "usage: ");
	s.time_fmt.describe(cout);
	return command_type::NORMAL;
} else {
	string_list::const_iterator i(a.begin()), e(a.end());
	for (++i; i!=e; ++i) {
		const string& arg(*i);
		if (arg == "fixed") {
			s.time_fmt.fmt |= std::ios_base::fixed;
		} else if (arg == "nofixed") {
			s.time_fmt.fmt &= ~std::ios_base::fixed;
		} else if (arg == "sci") {
			s.time_fmt.fmt |= std::ios_base::scientific;
		} else if (arg == "nosci") {
			s.time_fmt.fmt &= ~std::ios_base::scientific;
		} else {
			if (string_to_num(*i, s.time_fmt.precision)) {
				cerr << "Unrecognized option." << endl;
				return command_type::BADARG;
			}
		}
	}
	return command_type::NORMAL;
}
}

template <class State>
void
TimeFmt<State>::usage(ostream& o) {
	o << "time-fmt [fixed|nofixed|sci|nosci|INT]*" << endl;
	o << "sets the output formatting of time values.\n"
"  [no]fixed : use fixed-point\n"
"  [no]sci   : use scientific notation\n"
"  INT       : set precision" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(WatchQueue, "watch-queue",
	"print each event on watched nodes as it is enqueued")

template <class State>
int
WatchQueue<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.watch_event_queue();
	return command_type::NORMAL;
}
}

template <class State>
void
WatchQueue<State>::usage(ostream& o) {
	o << "watch-queue" << endl;
	o << "Print events on watched nodes only as they enter the event queue."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(NoWatchQueue, "nowatch-queue",
	"silence enqueuing into the event queue (default)")

template <class State>
int
NoWatchQueue<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.nowatch_event_queue();
	return command_type::NORMAL;
}
}

template <class State>
void
NoWatchQueue<State>::usage(ostream& o) {
	o << "nowatch-queue" << endl;
	o << "Silence events on watched nodes as they enter the event queue."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(WatchAllQueue, "watchall-queue",
	"print each event on all nodes as it is enqueued")

template <class State>
int
WatchAllQueue<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.watchall_event_queue();
	return command_type::NORMAL;
}
}

template <class State>
void
WatchAllQueue<State>::usage(ostream& o) {
	o << "watchall-queue" << endl;
	o << "Print all events as they are inserted into the event queue."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(NoWatchAllQueue, "nowatchall-queue",
	"silence enqueuing into the event queue (default)")

template <class State>
int
NoWatchAllQueue<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.nowatchall_event_queue();
	return command_type::NORMAL;
}
}

template <class State>
void
NoWatchAllQueue<State>::usage(ostream& o) {
	o << "nowatchall-queue" << endl;
	o << "Silence events as they are inserted into the event queue."
		<< endl;
}

//-----------------------------------------------------------------------------
// common trace file commands

DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(Trace, "trace", 
	"record trace of all events to file")

template <class State>
int
Trace<State>::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (s.open_trace(a.back())) {
		// confirm message
		cout << "Writing simulation trace to \"" << a.back()
			<< "\"." << endl;
		return command_type::NORMAL;
	} else {
		cout << "Error opening file \"" << a.back() <<
			"\" for trace recording." << endl;
		return command_type::BADARG;
	}
}
}

template <class State>
void
Trace<State>::usage(ostream& o) {
	o << name << " <file>" << endl;
	o << "Records all data and events to file for later analysis.\n"
"Trace-file is completed with a \'trace-close\' command, or automatically\n"
"upon termination of ths simulation." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(TraceFile, "trace-file", 
	"show the name of the active trace file")

template <class State>
int
TraceFile<State>::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (s.is_tracing()) {
		const string& tn(s.get_trace_manager()->get_trace_name());
		cout << "Active trace file: " << tn << endl;
	} else {
		cout << "No active trace file." << endl;
	}
	return command_type::NORMAL;
}
}

template <class State>
void
TraceFile<State>::usage(ostream& o) {
	o << name << endl;
	o << "Prints the name of the active trace file, if applicable." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(TraceClose, "trace-close", 
	"close the active trace file")

template <class State>
int
TraceClose<State>::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.close_trace();
	return command_type::NORMAL;
}
}

template <class State>
void
TraceClose<State>::usage(ostream& o) {
	o << name << endl;
	o << "Stops the active trace and writes it out to file." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(TraceFlushNotify, 
	"trace-flush-notify", 
	"enable/disable trace flush notifications (debug)")

template <class State>
int
TraceFlushNotify<State>::main(State&, const string_list& a) {
switch (a.size()) {
case 1:
	cout << "Trace flush notification is ";
	if (trace_manager_base::notify_flush) {
		cout << "enabled." << endl;
	} else {
		cout << "disabled." << endl;
	}
	return command_type::NORMAL;
case 2:
	size_t i;
	if (string_to_num(a.back(), i)) {
		cerr << "Error parsing numeric argument." << endl;
		return command_type::BADARG;
	}
	trace_manager_base::notify_flush = i;
	return command_type::NORMAL;
default:
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
}

template <class State>
void
TraceFlushNotify<State>::usage(ostream& o) {
	o << name << " [0|1]" << endl;
	o << 
"With argument, enables (1) or disables (0) trace flush notifications\n"
"Without argument, reports current policy." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(TraceFlushInterval, 
	"trace-flush-interval", 
	"set/get the current trace chunk granularity")

template <class State>
int
TraceFlushInterval<State>::main(State& s, const string_list& a) {
switch (a.size()) {
case 1:
	cout << "trace flush interval (events): " <<
		s.get_trace_flush_interval() << endl;
	break;
case 2:
	size_t i;
	if (string_to_num(a.back(), i)) {
		cerr << "Error parsing numeric interval argument." << endl;
		usage(cerr << "usage: ");
		return command_type::BADARG;
	}
	s.set_trace_flush_interval(i);
	break;
default:
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
	return command_type::NORMAL;
}

template <class State>
void
TraceFlushInterval<State>::usage(ostream& o) {
	o << name << " [interval]" << endl;
	o <<
"If argument is passed, then set the trace flush interval to it.\n"
"Otherwise, just report the current trace flush interval.\n"
"The interval is counted in number of events that execute." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(TraceDump, 
	"trace-dump", 
	"spill a human-readable (?) text dump of a trace file")

template <class State>
int
TraceDump<State>::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (State::trace_manager_type::text_dump(a.back(), cout, s)) {
		cerr << "Error opening trace file: " << a.back() << endl;
		return command_type::BADARG;
	}
	return command_type::NORMAL;
}
}

template <class State>
void
TraceDump<State>::usage(ostream& o) {
	o << name << " <tracefile>" << endl;
	o << "Dumps contents of a trace file to stdout.\n"
"Future versions may require a proper object file to be attached." << endl;
}

//-----------------------------------------------------------------------------
// common vcd trace file commands

DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(VCD, "vcd", 
	"record vector change dump trace")

template <class State>
int
VCD<State>::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (s.open_vcd(a.back())) {
		// confirm message
		cout << "Writing simulation vcd to \"" << a.back()
			<< "\"." << endl;
		return command_type::NORMAL;
	} else {
		cout << "Error opening file \"" << a.back() <<
			"\" for vcd recording." << endl;
		return command_type::BADARG;
	}
}
}

template <class State>
void
VCD<State>::usage(ostream& o) {
	o << name << " <file>" << endl;
	o << "Records vector-change-dump to file for later analysis.\n"
"The vcd file is completed with a \'vcd-close\' command, or automatically\n"
"upon termination of ths simulation." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(VCDFile, "vcd-file", 
	"show the name of the active vcd file")

template <class State>
int
VCDFile<State>::main(State& s, const string_list& a) {
	STACKTRACE_VERBOSE;
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	const util::memory::never_ptr<typename State::vcd_manager_type>
		vt(s.get_vcd_manager_if_tracing());
	if (vt) {
		STACKTRACE_INDENT_PRINT("is tracing");
		const string& tn(vt->get_trace_name());
		cout << "Active vcd file: " << tn << endl;
	} else {
		cout << "No active vcd file." << endl;
	}
	return command_type::NORMAL;
}
}

template <class State>
void
VCDFile<State>::usage(ostream& o) {
	o << name << endl;
	o << "Prints the name of the active vcd file, if applicable." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(VCDClose, "vcd-close", 
	"close the active vcd file")

template <class State>
int
VCDClose<State>::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	s.close_vcd();
	return command_type::NORMAL;
}
}

template <class State>
void
VCDClose<State>::usage(ostream& o) {
	o << name << endl;
	o << "Stops the active vcd and writes it out to file." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DESCRIBE_COMMON_COMMAND_CLASS_TEMPLATE(VCDTimeScale, 
	"vcd-time-scale", 
	"set/get the vcd time scale factor")

template <class State>
int
VCDTimeScale<State>::main(State& s, const string_list& a) {
switch (a.size()) {
case 1:
	cout << "vcd time scale (mult.): " <<
		s.get_vcd_timescale() << endl;
	break;
case 2:
	double i;
	if (string_to_num(a.back(), i)) {
		cerr << "Error parsing numeric interval argument." << endl;
		usage(cerr << "usage: ");
		return command_type::BADARG;
	} else if (i <= 0.0) {
		cerr << "Error: time scale must be positive." << endl;
		return command_type::BADARG;
	}
	s.set_vcd_timescale(i);
	break;
default:
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
}
	return command_type::NORMAL;
}

template <class State>
void
VCDTimeScale<State>::usage(ostream& o) {
	o << name << " [scale]" << endl;
	o <<
"If argument is passed, then set the vcd time scale factor to it.\n"
"Otherwise, just report the current vcd time scale factor.\n"
"This is sometimes needed because vcd files only accept integer times.\n"
"Default value: 1.0." << endl;
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_COMMON_TCC__

