/**
	\file "sim/command_common.tcc"
	Library of template command implementations, re-usable with
	different state types.  
	$Id: command_common.tcc,v 1.10.4.1 2008/11/25 08:36:39 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_COMMON_TCC__
#define	__HAC_SIM_COMMAND_COMMON_TCC__

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <algorithm>
#include <functional>

#include "sim/command_common.h"
#include "sim/command_base.h"
#include "sim/command_category.h"
#include "sim/command_registry.h"
#include "parser/instref.h"
#include "Object/expr/dlfunction.h"
#include "common/TODO.h"
#include "common/ltdl-wrap.h"
#include "util/compose.h"
#include "util/string.tcc"
#include "util/utypes.h"

namespace HAC {
namespace SIM {
using std::ios_base;
using std::ofstream;
using std::for_each;
using std::ptr_fun;
using std::mem_fun_ref;
using util::strings::string_to_num;
#include "util/using_ostream.h"
using parser::parse_name_to_what;
using parser::parse_name_to_members;
using parser::parse_name_to_aliases;
USING_UTIL_COMPOSE

//=============================================================================


/**
	Use this macro to name the common command classes.
	This includes static initializers for the non-function members.  
	\param _class the name of the class.
	\param _cmd the string-quoted name of the command.
	\param _category the cateory object with which to associate.  
	\param _brief one-line quoted-string description.
 */
#define	INITIALIZE_COMMON_COMMAND_CLASS(_class, _cmd, _brief)		\
template <class State>							\
const char _class<State>::name[] = _cmd;				\
template <class State>							\
const char _class<State>::brief[] = _brief;

/**
	This should be called by the user to register the command
	with the desired category.  
	Explicit instantiation not necessary, automated by receipt_id.
 */
#define	CATEGORIZE_COMMON_COMMAND_CLASS(_class, _category)		\
template <>								\
_class::command_category_type&						\
_class::category(_category);						\
template <>								\
const size_t _class::receipt_id =					\
_class::command_registry_type::register_command<_class >();


//=============================================================================
INITIALIZE_COMMON_COMMAND_CLASS(Help, "help",
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
INITIALIZE_COMMON_COMMAND_CLASS(All, "all", "show all commands")

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
INITIALIZE_COMMON_COMMAND_CLASS(Alias, "alias",
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
INITIALIZE_COMMON_COMMAND_CLASS(UnAlias, "unalias",
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
INITIALIZE_COMMON_COMMAND_CLASS(UnAliasAll, "unaliasall",
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
INITIALIZE_COMMON_COMMAND_CLASS(Aliases, "aliases",
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
INITIALIZE_COMMON_COMMAND_CLASS(Interpret, "interpret",
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
	o << "interpret" << endl;
	o << "Re-opens standard-input interactive as a subshell." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(EchoCommands, "echo-commands",
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
INITIALIZE_COMMON_COMMAND_CLASS(Source, "source",
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
INITIALIZE_COMMON_COMMAND_CLASS(AddPath, "addpath",
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
INITIALIZE_COMMON_COMMAND_CLASS(Paths, "paths",
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(Initialize, "initialize",
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
INITIALIZE_COMMON_COMMAND_CLASS(Reset, "reset",
	"resets simulator state, queue, and modes (fresh start)")

template <class State>
int
Reset<State>::main(state_type& s, const string_list&) {
	s.reset();
	return command_type::NORMAL;
}

template <class State>
void
Reset<State>::usage(ostream& o) {
	o << "reset: " << brief << endl;
	o << "restart the entire simulation as if it was just launched" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(Queue, "queue", "show event queue")

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
INITIALIZE_COMMON_COMMAND_CLASS(Seed48, "seed48", "set/get random number seed")

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
INITIALIZE_COMMON_COMMAND_CLASS(Save, "save",
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
INITIALIZE_COMMON_COMMAND_CLASS(Load, "load",
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
INITIALIZE_COMMON_COMMAND_CLASS(AutoSave, "autosave",
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
INITIALIZE_COMMON_COMMAND_CLASS(LS, "ls",
	"list subinstances of the referenced instance")

template <class State>
int
LS<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (parse_name_to_members(cout, a.back(), s.get_module()))
		return command_type::BADARG;
	else	return command_type::NORMAL;
}
}

template <class State>
void
LS<State>::usage(ostream& o) {
	o << "ls <name>" << endl;
	o << "prints list of subinstances of the referenced instance" << endl;
	o << "\"ls .\" lists top-level instances" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(What, "what",
	"print type information of named entity")

template <class State>
int
What<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (parse_name_to_what(cout, a.back(), s.get_module()))
		return command_type::BADARG;
	else	return command_type::NORMAL;
}
}

template <class State>
void
What<State>::usage(ostream& o) {
	o << "what <name>" << endl;
	o << "prints the type/size of the referenced instance(s)" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(Who, "who",
	"print aliases of node or structure")

template <class State>
int
Who<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	cout << "aliases of \"" << a.back() << "\":" << endl;
	if (parse_name_to_aliases(cout, a.back(), s.get_module(), " ")) {
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(WhoNewline, "who-newline",
	"print aliases of node or structure, newline separated")

template <class State>
int
WhoNewline<State>::main(state_type& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	cout << "aliases of \"" << a.back() << "\":" << endl;
	if (parse_name_to_aliases(cout, a.back(), s.get_module(), "\n")) {
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(AssertQueue, "assert-queue",
	"assert that the event queue is not empty")

template <class State>
int
AssertQueue<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (s.pending_events()) {
		return command_type::NORMAL;
	} else {
		cout << "assert failed: expecting non-empty event queue."
			<< endl;
		return command_type::FATAL;
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
INITIALIZE_COMMON_COMMAND_CLASS(AssertNQueue, "assertn-queue",
	"assert that the event queue is empty")

template <class State>
int
AssertNQueue<State>::main(state_type& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (!s.pending_events()) {
		return command_type::NORMAL;
	} else {
		cout << "assert failed: expecting empty event queue."
			<< endl;
		return command_type::FATAL;
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
INITIALIZE_COMMON_COMMAND_CLASS(Time, "time",
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
INITIALIZE_COMMON_COMMAND_CLASS(WatchQueue, "watch-queue",
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
INITIALIZE_COMMON_COMMAND_CLASS(NoWatchQueue, "nowatch-queue",
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
INITIALIZE_COMMON_COMMAND_CLASS(WatchAllQueue, "watchall-queue",
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
INITIALIZE_COMMON_COMMAND_CLASS(NoWatchAllQueue, "nowatchall-queue",
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
INITIALIZE_COMMON_COMMAND_CLASS(DLAddPath, "dladdpath", 
	"Append search paths for dlopening modules")

template <class State>
int
DLAddPath<State>::main(state_type&, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	for_each(++a.begin(), a.end(), 
		unary_compose(ptr_fun(&lt_dladdsearchdir),
			mem_fun_ref(&string::c_str)));
	return command_type::NORMAL;
}
}

template <class State>
void
DLAddPath<State>::usage(ostream& o) {
	o << name << " <paths...>" << endl;
	o << "Loads an external library, file extension should be omitted.\n"
"Searches user-defined path (`dlpath\') before system paths.\n"
"Module should automatically register functions during static initization."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(DLPaths, "dlpaths", 
	"List search paths for dlopening modules")

template <class State>
int
DLPaths<State>::main(state_type&, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	const char* const p = lt_dlgetsearchpath();
	cout << "dlopen search paths: ";
	if (p)
		cout << lt_dlgetsearchpath();
	else	cout << "<empty>";
	cout << endl;
	return command_type::NORMAL;
}
}

template <class State>
void
DLPaths<State>::usage(ostream& o) {
	o << name << endl;
	o << "Prints current list of search paths used for dlopening." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(DLOpen, "dlopen", 
	"load a module (dynamically shared library)")

template <class State>
int
DLOpen<State>::main(state_type&, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	if (!ltdl_open_append(a.back())) {
		return command_type::BADARG;
	} else	return command_type::NORMAL;
}
}

template <class State>
void
DLOpen<State>::usage(ostream& o) {
	o << name << " <library>" << endl;
	o << "Loads an external library, file extension should be omitted.\n"
"Searches user-defined path (`dlpath') before system paths.\n"
"Module should automatically register functions during static initization."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(DLCheckFunc, "dlcheckfunc", 
	"query whether a nonmeta function symbol is bound")

template <class State>
int
DLCheckFunc<State>::main(state_type& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	// ignore assert's return value
	DLAssertFunc<State>::main(s, a);
	return command_type::NORMAL;
}
}

template <class State>
void
DLCheckFunc<State>::usage(ostream& o) {
	o << name << " <funcs ...>" << endl;
	o <<
"Reports whether or not each function is already bound to a symbol from a \n"
"loaded dynamic shared library or module.  Does not error out.\n"
"See also `dlassertfunc\'."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(DLAssertFunc, "dlassertfunc", 
	"assert-fail if a nonmeta function symbol is unbound")

template <class State>
int
DLAssertFunc<State>::main(state_type&, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	bool good = true;
	string_list::const_iterator i(++a.begin()), e(a.end());
	for ( ; i!=e; ++i) {
		if (entity::lookup_chpsim_function(*i)) {
			cout << "function `" << *i << "\': bound." << endl;
		} else {
			cout << "function `" << *i << "\': unbound." << endl;
			good = false;
		}
	}
	return good ? command_type::NORMAL : command_type::BADARG;
}
}

template <class State>
void
DLAssertFunc<State>::usage(ostream& o) {
	o << name << " <funcs ...>" << endl;
	o <<
"Reports whether or not each function is already bound to a symbol from a \n"
"loaded dynamic shared library or module.  Errors out if any are unbound.\n"
"See also `dlcheckfunc\'."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMON_COMMAND_CLASS(DLFuncs, "dlfuncs", 
	"List names of all dynamically bound functions")

template <class State>
int
DLFuncs<State>::main(state_type&, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return command_type::SYNTAX;
} else {
	entity::list_chpsim_functions(cout);
	return command_type::NORMAL;
}
}

template <class State>
void
DLFuncs<State>::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_COMMON_TCC__

