/**
	\file "sim/prsim/Command-prsim.cc"
	All built-in commands should be statically registered 
	in this translation unit to guarantee proper static object
	initialization ordering; the command_map must be constructed
	before entries are registered.  

	TODO: consider using some form of auto-indent
		in the help-system.  

	$Id: Command-prsim.cc,v 1.29 2008/12/18 21:00:03 fang Exp $

	NOTE: earlier version of this file was:
	Id: Command.cc,v 1.23 2007/02/14 04:57:25 fang Exp
	but had to be renamed to avoid base-object file name clashes
		which broke dyld on i686-apple-darwin, cause Command.o
		to fail to be loaded due to conflict with chpsim's.  
	Painful discontinuity in revision history...
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>

#include "sim/prsim/Command-prsim.h"
#include "sim/prsim/Command-prsim-export.h"
#include "sim/prsim/State-prsim.h"
#include "sim/command_base.tcc"
#include "sim/command_category.tcc"
#include "sim/command_registry.tcc"
#include "sim/command_builtin.tcc"
#include "sim/command_common.tcc"
#include "parser/instref.h"

#include "common/TODO.h"
#include "util/libc.h"
#include "util/memory/excl_malloc_ptr.h"

/**
	These commands are deprecated, but provided for backwards compatibility.
	Eventually disable this.  
	(NOTE: the alias command can always effectively re-enable it.)
 */
#define	WANT_OLD_RANDOM_COMMANDS			1

namespace HAC {
namespace SIM {

// must instantiate dependent class first!
template class command_registry<PRSIM::Command>;

namespace PRSIM {
#include "util/using_istream.h"
#include "util/using_ostream.h"
using std::ios_base;
using std::ifstream;
using std::copy;
using std::reverse_copy;
using std::ostream_iterator;
using std::front_inserter;
using util::excl_malloc_ptr;
using util::strings::string_to_num;
using parser::parse_node_to_index;
using parser::parse_process_to_index;
using parser::parse_name_to_what;
using parser::parse_name_to_aliases;
using parser::parse_name_to_get_subnodes;

//=============================================================================
// local static CommandCategories
// feel free to add categories here

static CommandCategory
	builtin("builtin", "built-in commands"),
	general("general", "general commands"),
	debug("debug", "debugging internals"),
	simulation("simulation", "simulation commands"),
	channels("channels", "channel commands"),
	info("info", "information about simulated circuit"),
	view("view", "instance to watch"),
	modes("modes", "timing model, error handling");

//=============================================================================
// local Command classes
// feel free to add commands here

/**
	Use this macro to declare ordinary commands.
	This includes static initializers for the non-function members.  
	\param _class the name of the class.
	\param _cmd the string-quoted name of the command.
	\param _category the cateory object with which to associate.  
	\param _brief one-line quoted-string description.
 */
#define	INITIALIZE_COMMAND_CLASS(_class, _cmd, _category, _brief)	\
const char _class::name[] = _cmd;					\
const char _class::brief[] = _brief;					\
CommandCategory& _class::category(_category);				\
const size_t _class::receipt_id = CommandRegistry::register_command<_class >();

#define	DECLARE_AND_INITIALIZE_COMMAND_CLASS(_class, _cmd, _category, _brief) \
	DECLARE_PRSIM_COMMAND_CLASS(_class)				\
	INITIALIZE_COMMAND_CLASS(_class, _cmd, _category, _brief)

//-----------------------------------------------------------------------------
/***
@texinfo cmd/echo.texi
@deffn Command echo args ...
Prints the arguments back to stdout.
@end deffn
@end texinfo
***/
typedef	stateless_command_wrapper<Echo, State>	Echo;
INITIALIZE_STATELESS_COMMAND_CLASS(PRSIM::Echo, "echo", PRSIM::builtin, 
	"prints arguments back to stdout, space-delimited")

//-----------------------------------------------------------------------------
/***
@texinfo cmd/help.texi
@deffn Command help cmd
Help on command or category @var{cmd}.
@samp{help all} gives a list of all commands available in all categories.
@samp{help help} tells you how to use @command{help}.
@end deffn
@end texinfo
***/
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Help, PRSIM::builtin)
}	// end namespace PRSIM

// template needs to be instantiated in correct namespace
template class Help<PRSIM::State>;

namespace PRSIM {
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/comment.texi
@deffn Command # ...
@deffnx Command comment ...
Whole line comment, ignored by interpreter.  
@end deffn
@end texinfo
***/
typedef	stateless_command_wrapper<CommentPound, State>		CommentPound;
typedef	stateless_command_wrapper<CommentComment, State>	CommentComment;

INITIALIZE_STATELESS_COMMAND_CLASS(PRSIM::CommentPound,
	"#", PRSIM::builtin, "comments are ignored")
INITIALIZE_STATELESS_COMMAND_CLASS(PRSIM::CommentComment,
	"comment", PRSIM::builtin, "comments are ignored")


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	All<State>					All;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::All, PRSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/exit.texi
@deffn Command exit
@deffnx Command quit
Exit the simulator.
@end deffn
@end texinfo

@texinfo cmd/abort.texi
@deffn Command abort
Exit the simulator with a fatal (non-zero) exit status.
@end deffn
@end texinfo
***/
typedef	stateless_command_wrapper<Exit, State>		Exit;
typedef	stateless_command_wrapper<Quit, State>		Quit;
typedef	stateless_command_wrapper<Abort, State>		Abort;

INITIALIZE_STATELESS_COMMAND_CLASS(PRSIM::Exit,
	"exit", PRSIM::builtin, "exits simulator")
INITIALIZE_STATELESS_COMMAND_CLASS(PRSIM::Quit,
	"quit", PRSIM::builtin, "exits simulator")
INITIALIZE_STATELESS_COMMAND_CLASS(PRSIM::Abort,
	"abort", PRSIM::builtin, "exits simulator with fatal status")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/alias.texi
@deffn Command alias cmd args
Defines an alias, whereby the interpreter expands @var{cmd} into
@var{args} before interpreting the command.
@var{args} may consist of multiple tokens.  
This is useful for shortening common commands.  
@end deffn
@end texinfo
***/
typedef	Alias<State>				Alias;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Alias, PRSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unalias.texi
@deffn Command unalias cmd
Undefines an existing alias @var{cmd}.
@end deffn
@end texinfo
***/
typedef	UnAlias<State>				UnAlias;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::UnAlias, PRSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unaliasall.texi
@deffn Command unaliasall
Undefines @emph{all} aliases.  
@end deffn
@end texinfo
***/
typedef	UnAliasAll<State>			UnAliasAll;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::UnAliasAll, PRSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/aliases.texi
@deffn Command aliases
Print a list of all known aliases registered with the interpreter.
@end deffn
@end texinfo
***/
typedef	Aliases<State>				Aliases;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Aliases, PRSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/interpret.texi
@deffn Command interpret
Open an interactive subshell of the interpreter, by re-opening
the standard input stream.  
This is useful when you want to break in the middle of a non-interactive
script and let the user take control temporarily before returning
control back to the script.  
The @command{exit} command or @kbd{Ctrl-D} sends the EOF signal to exit 
the current interactive level of input and return control to the parent.  
The level of shell is indicated by additional @t{>} characters in the prompt.
This works if @command{hacprsim} was originally launched interactively
and without redirecting a script through stdin.  

@example
$ @kbd{hacprsim foo.haco}
prsim> @kbd{!cat foo.prsimrc}
# foo.prsimrc
echo hello world
interpret
echo goodbye world
prsim> @kbd{source foo.prsimrc}
hello world
prsim>> @kbd{echo where am I?}
where am I?
prsim>> @kbd{exit}
goodbye world
prsim> @kbd{exit}
$
@end example
@end deffn
@end texinfo
***/
typedef	Interpret<State>			Interpret;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Interpret, PRSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/echo-commands.texi
@deffn Command echo-commands arg
Enables or disables echoing of each interpreted command and 
tracing through sourced script files.  
@var{arg} is either "on" or "off".  
Default off.
@end deffn
@end texinfo
***/
typedef	EchoCommands<State>				EchoCommands;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::EchoCommands, PRSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/source.texi
@deffn Command source script
@anchor{command-source}
Loads commands to the interpreter from the @var{script} file.
File is searched through include paths given by
the @ref{option-I,, @option{-I}} command-line option 
or the @ref{command-addpath,, @command{addpath}} command.  
@end deffn
@end texinfo
***/
typedef	Source<State>				Source;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Source, PRSIM::general)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/addpath.texi
@deffn Command addpath path
@anchor{command-addpath}
Appends @var{path} to the search path for sourcing scripts.
@end deffn
@end texinfo
***/
typedef	AddPath<State>				AddPath;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::AddPath, PRSIM::general)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/paths.texi
@deffn Command paths
Print the list of paths searched for source scripts.  
@end deffn
@end texinfo
***/
typedef	Paths<State>				Paths;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Paths, PRSIM::general)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/initialize.texi
@deffn Command initialize
Resets the variable state of the simulation (to unknown), 
while preserving other settings such as mode and breakpoints.  
@end deffn
@end texinfo
***/
typedef	Initialize<State>			Initialize;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Initialize, PRSIM::simulation)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/reset.texi
@deffn Command reset
Similar to @command{initialize}, but also resets all modes to their
default values.  
@end deffn
@end texinfo
***/
typedef	Reset<State>				Reset;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Reset, PRSIM::simulation)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
	Command class for stepping through one time-step at a time from
	the event queue. 
@texinfo cmd/step.texi
@deffn Command step [n]
Advances the simulation by @var{n} time steps.  
Without @var{n}, takes only a single step.  
Time steps may cover multiple events if they are at the exact same time.
To step by events count, use @command{step-event}.  
@end deffn
@end texinfo
***/
#if 0
struct Step {
public:
	static const char               name[];
	static const char               brief[];
	static CommandCategory&         category;
	static int      main(State&, const string_list&);
	static void     usage(ostream&);
private:
	static const size_t             receipt_id;
};      // end class Step
#endif

DECLARE_AND_INITIALIZE_COMMAND_CLASS(Step, "step", simulation,
	"step through event(s), by time increments")

/**
	Like process_step() from original prsim.  
 */
int
Step::main(State& s, const string_list& a) {
	CommandRegistry::forbid_cosimulation(a);
if (a.size() > 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::time_type		time_type;
	typedef	State::node_type		node_type;
	size_t i;		// the number of discrete time steps
		// not necessarily == the number of discrete events
	State::step_return_type ni;	// also stores the cause of the event
	if (a.size() == 2) {
		if (string_to_num(a.back(), i)) {
			cerr << "Error parsing #steps." << endl;
			// usage()?
			return Command::BADARG;
		}
	} else {
		i = 1;
	}
	s.resume();
	time_type time = s.time();
	// could check s.pending_events()
	try {
	while (!s.stopped() && i && GET_NODE((ni = s.step()))) {
		// if time actually advanced, decrement steps-remaining
		// NB: may need specialization for real-valued (float) time.  
		const time_type ct(s.time());
		if (time != ct) {
			--i;
			time = ct;
		}
		const node_type& n(s.get_node(GET_NODE(ni)));
		/***
			The following code should be consistent with
			Cycle::main() and Advance::main().
			tracing stuff here later...
		***/
		if (s.watching_all_nodes()) {
			print_watched_node(cout << '\t' << ct << '\t', s, ni);
		}
		if (n.is_breakpoint()) {
			// this includes watchpoints
			const bool w = s.is_watching_node(GET_NODE(ni));
			const string nodename(
				s.get_node_canonical_name(GET_NODE(ni)));
			if (w) {
			if (!s.watching_all_nodes()) {
				print_watched_node(cout << '\t' << ct << '\t',
					s, ni);
			}	// else already have message from before
			}
			// channel support
			if (!w) {
				// node is plain breakpoint
				cout << "\t*** break, " << i <<
					" steps left: `" << nodename <<
					"\' became ";
				n.dump_value(cout) << endl;
				return Command::NORMAL;
				// or Command::BREAK; ?
			}
		}
	}	// end while
	} catch (const step_exception& exex) {
		return error_policy_to_status(exex.inspect(s, cerr));
	}	// no other exceptions
	return Command::NORMAL;
}
}	// end Step::main()

void
Step::usage(ostream& o) {
	o << "step [#timesteps]" << endl;
	o <<
"Advances the simulation by a number of steps (default: 1).\n"
"Simulation will stop prematurely if any event violations are encountered.\n"
"Note: the argument is the number of time *increments* to simulate, not the\n"
"number of events (see step-event)."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
	Command class for stepping through one event at a time from
	the event queue. 
@texinfo cmd/step-event.texi
@deffn Command step-event [n]
Advances the simulation by @var{n} events.  
Without @var{n}, takes only a single event.  
A single event is not necessarily guaranteed to advance the time, 
if multiple events are enqueued at the same time.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(StepEvent, "step-event", simulation,
	"step through event(s), by event count")

/**
	\param i the maximum number of steps to execute
 */
static
int
step_event_main(State& s, size_t i) {
	s.resume();
	// could check s.pending_events()
	try {
	State::step_return_type ni;	// also stores the cause of the event
	while (!s.stopped() && i && GET_NODE((ni = s.step()))) {
		--i;
		// NB: may need specialization for real-valued (float) time.  
		const time_type ct(s.time());
		const node_type& n(s.get_node(GET_NODE(ni)));
		/***
			The following code should be consistent with
			Cycle::main() and Advance::main().
			tracing stuff here later...
		***/
		if (s.watching_all_nodes()) {
			print_watched_node(cout << '\t' << ct << '\t', s, ni);
		}
		if (n.is_breakpoint()) {
			// this includes watchpoints
			const bool w = s.is_watching_node(GET_NODE(ni));
			const string nodename(
				s.get_node_canonical_name(GET_NODE(ni)));
			if (w) {
			if (!s.watching_all_nodes()) {
				print_watched_node(cout << '\t' << ct << '\t',
					s, ni);
			}	// else already have message from before
			}
			// channel support
			if (!w) {
				// node is plain breakpoint
				cout << "\t*** break, " << i <<
					" steps left: `" << nodename <<
					"\' became ";
				n.dump_value(cout) << endl;
				return Command::NORMAL;
				// or Command::BREAK; ?
			}
		}
	}	// end while
	} catch (const step_exception& exex) {
		return error_policy_to_status(exex.inspect(s, cerr));
	}	// no other exceptions
	return Command::NORMAL;
}

int
StepEvent::main(State& s, const string_list& a) {
	CommandRegistry::forbid_cosimulation(a);
if (a.size() > 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::time_type		time_type;
	typedef	State::node_type		node_type;
	size_t i;		// the number of discrete time steps
		// not necessarily == the number of discrete events
	if (a.size() == 2) {
		if (string_to_num(a.back(), i)) {
			cerr << "Error parsing #steps." << endl;
			// usage()?
			return Command::BADARG;
		}
	} else {
		i = 1;
	}
	return step_event_main(s, i);
}
}	// end StepEvent::main

void
StepEvent::usage(ostream& o) {
	o << "step-event [#events]" << endl;
	o <<
"Advances the simulation by a number of events (default: 1).\n"
"Simulation will stop prematurely if any event violations are encountered.\n"
"Note: the argument is the number of discrete events to simulate, not the\n"
"number of time steps (see step)."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/advance.texi
@deffn Command advance delay
Advances the simulation @var{delay} units of time.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Advance, "advance", simulation,
	"advance the simulation in time units")

int
Advance::main(State& s, const string_list& a) {
	CommandRegistry::forbid_cosimulation(a);
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::time_type		time_type;
	typedef	State::node_type		node_type;
	time_type add;		// time to add
	if (string_to_num(a.back(), add)) {
		cerr << "Error parsing time." << endl;
		return Command::BADARG;
	} else if (add < 0) {
		cerr << "Error: time must be non-negative." << endl;
		return Command::BADARG;
	}
	const time_type stop_time = s.time() +add;
	return prsim_advance(s, stop_time, true);
}
}	// end Advance::main()

void
Advance::usage(ostream& o) {
	o << "advance <time>" << endl;
	o <<
"Advances the simulation by the given amount of time.\n"
"Simulation will stop prematurely if any event violations are encountered."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/cycle.texi
@deffn Command cycle
Execute steps until the event queue is exhausted (if ever).  
Can be interrupted by @kbd{Ctrl-C} or a @t{SIGINT} signal.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Cycle, "cycle", simulation,
 	"run until event queue empty or breakpoint")

/**
	TODO: implement no/globaltime policy for resetting.  
	Isn't the while-loop made redundant by State::cycle()?
 */
int
Cycle::main(State& s, const string_list& a) {
	CommandRegistry::forbid_cosimulation(a);
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	State::step_return_type ni;
	s.resume();	// clear STOP flag
	try {
	while (!s.stopped() && GET_NODE((ni = s.step()))) {
		if (!GET_NODE(ni))
			return Command::NORMAL;
		const node_type& n(s.get_node(GET_NODE(ni)));
		/***
			The following code should be consistent with
			Step::main() and Advance::main().
		***/
		if (s.watching_all_nodes()) {
			print_watched_node(cout << '\t' << s.time() <<
				'\t', s, ni);
		}
		if (n.is_breakpoint()) {
			// this includes watchpoints
			const bool w = s.is_watching_node(GET_NODE(ni));
			const string nodename(s.get_node_canonical_name(
				GET_NODE(ni)));
			if (w) {
			if (!s.watching_all_nodes()) {
				print_watched_node(cout << '\t' <<
					s.time() << '\t', s, ni);
			}	// else already have message from before
			}
			// channel support
			if (!w) {
				// node is plain breakpoint
				cout << "\t*** break, `" << nodename <<
					"\' became ";
				n.dump_value(cout) << endl;
				return Command::NORMAL;
				// or Command::BREAK; ?
			}
		}
	}	// end while (!s.stopped())
	} catch (const step_exception& exex) {
		return error_policy_to_status(exex.inspect(s, cerr));
	}	// no other exceptions
	return Command::NORMAL;
}	// end if
}	// end Cycle::main()

void
Cycle::usage(ostream& o) {
	o << "cycle" << endl;
	o << "Runs until event queue is empty.\n"
"Simulation will stop prematurely if any event violations are encountered."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/queue.texi
@deffn Command queue
Print the event queue.  
@end deffn
@end texinfo
***/
typedef	Queue<State>				Queue;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Queue, PRSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/set.texi
@deffn Command set node val [delay]
Set @var{node} to @var{val}.  
If @var{delay} is omitted, the set event is inserted `now' at 
the current time, at the head of the event queue.
If @var{delay} is given with a @t{+} prefix, time is added relative to
`now', otherwise it is scheduled at an absolute time @var{delay}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Set, "set", simulation,
	"set node immediately, or after delay")

/**
	Do we need an optional time argument?
	\param force whether or not this 'set' has precedence over 
		pending events in queue.  
 */
static
int
__set_main(State& s, const string_list& a, const bool force, 
		void (*usage)(ostream&)) {
	const size_t asz = a.size();
if (asz < 3 || asz > 4) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	typedef	State::event_type		event_type;
	typedef	State::event_placeholder_type	event_placeholder_type;
	// now I'm wishing string_list was string_vector... :) can do!
	string_list::const_iterator ai(++a.begin());
	const string& objname(*ai++);	// node name
	const string& _val(*ai++);	// node value
	// valid values are 0, 1, 2(X)
	const value_enum val = node_type::string_to_value(_val);
	if (!node_type::is_valid_value(val)) {
		cerr << "Invalid logic value: " << _val << endl;
		return Command::SYNTAX;
	}
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		int err;
		if (ai != a.end()) {
			// extract time
			const string& d(*ai);
			// cout << "d = " << d << endl;
			State::time_type t;
			if (string_to_num(d, t)) {
				cerr << "Error parsing delay value." << endl;
				return Command::BADARG;
			}
			if (d[0] == '+') {
				// relative delay into the future
				if (t < 0) {
					cerr << "Error: delay must be non-negative." << endl;
					return Command::BADARG;
				}
				err = s.set_node_after(ni, val, t, force);
			} else {
				// schedule at absolute time
				if (t < s.time()) {
					cerr << "Error: time cannot be in the past." << endl;
					return Command::BADARG;
				}
				err = s.set_node_time(ni, val, t, force);
			}
		} else {
			err = s.set_node(ni, val, force);
		}
		return (err < 1) ? Command::NORMAL : Command::BADARG;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}	// end Set::main

int
Set::main(State& s, const string_list& a) {
	return __set_main(s, a, false, &Set::usage);
}

void
Set::usage(ostream& o) {
	o << "set <node> <0|F|1|T|X|U> [+delay | time]" << endl;
	o <<
"\tWithout delay, node is set immediately.  Relative delay into future\n"
"\tis given by +delay, whereas an absolute time is given without \'+\'.\n"
"\tIf there is a pending event on the node, this command is ignored with a\n"
"\twarning.  See also \'setf\'."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/setf.texi
@deffn Command setf node val [delay]
Set forcefully.  
Same as the @command{set} command, but this overrides any pending 
events on @var{node}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(SetF, "setf", simulation,
	"same as set, but overriding pending events")

int
SetF::main(State& s, const string_list& a) {
	return __set_main(s, a, true, &SetF::usage);
}

void
SetF::usage(ostream& o) {
	o << "setf <node> <0|F|1|T|X|U> [+delay | time]" << endl;
	o <<
"\tWithout delay, node is set immediately.  Relative delay into future\n"
"\tis given by +delay, whereas an absolute time is given without \'+\'.\n"
"\tThis overrides a previous pending event on the node, if any."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unset.texi
@deffn Command unset node
Cancel any pending @command{set} commands on @var{node}.  
This effectively causes a node to be re-evaluated based on the 
state of its fanin.  
If the evaluation results in a change of value, a firing 
is scheduled in the event queue.  
This command may be useful in releasing nodes from a stuck state caused by
a coercive @command{set}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnSet, "unset", simulation,
	"force re-evaluation of node\'s input state, may cancel setf")

int
UnSet::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		s.unset_node(ni);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
UnSet::usage(ostream& o) {
	o << "unset <node>" << endl;
	o <<
"\tUnset causes a node to be re-evaluated in terms of the pull-state of\n"
"\tits inputs.  A node can become stuck as a result of a coerced set\n"
"\t(setf) that overrode a pending event; \'unset\' undoes such effects\n"
"\tand re-inserts pending events into the event queue where appropriate.\n"
"\tSee also \'unsetall\'."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unsetall.texi
@deffn Command unsetall
Clears all coercive @command{set} commands, and re-evaluates
@emph{all} nodes in terms of their fanins.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnSetAll, "unsetall", simulation,
	"force re-evaluation of all nodes\' inputs, cancelling setf")

int
UnSetAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.unset_all_nodes();
	return Command::NORMAL;
}
}

void
UnSetAll::usage(ostream& o) {
	o << "unsetall" << endl;
o <<
"\tUnsetall causes all nodes to be re-evaluated, and clears the effects\n"
"\tof any former coreced setf commands.  Corrected nodes will spawn\n"
"\tnew events in the event queue.  See also \'unset\'."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/setr.texi
@deffn Command setr node val
Same as the @command{set} command, but using a random delay into the future.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Setr, "setr", simulation,
	"set node to value after random delay")

/**
	NOTE: the random delay has the same distribution as the 
	random delays in the simulator in random mode.  
	\param force whether or not new set event has precedence over pending.  
 */
static
int
__setrf_main(State& s, const string_list& a, const bool force, 
		void (*usage)(ostream&)) {
	const size_t asz = a.size();
if (asz != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	typedef	State::event_type		event_type;
	typedef	State::event_placeholder_type	event_placeholder_type;
	// now I'm wishing string_list was string_vector... :) can do!
	string_list::const_iterator ai(++a.begin());
	const string& objname(*ai++);	// node name
	const string& _val(*ai++);	// node value
	// valid values are 0, 1, 2(X)
	const value_enum val = node_type::string_to_value(_val);
	if (!node_type::is_valid_value(val)) {
		cerr << "Invalid logic value: " << _val << endl;
		return Command::SYNTAX;
	}
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		const int err = s.set_node_after(ni, val,
			State::exponential_random_delay(), force);
		return (err < 1) ? Command::NORMAL : Command::BADARG;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}	// end Setr::main

int
Setr::main(State& s, const string_list& a) {
	return __setrf_main(s, a, false, &Setr::usage);
}

void
Setr::usage(ostream& o) {
	o << "setr <node> <0|F|1|T|X|U>" << endl;
	o << "\tsets node at a random time in the future.\n"
"\tNodes with already pending events in queue retain their former event.\n"
"\tThis variation yields to pending events on the node, if any."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/setrf.texi
@deffn Command setrf node val
Same as @command{setf} and @command{setr} combined; 
forcefully set @var{node} to @var{val} at random time in future, 
overriding any pending events.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(SetrF, "setrf", simulation,
	"set node to value after random delay, overriding pending events")

int
SetrF::main(State& s, const string_list& a) {
	return __setrf_main(s, a, true, &SetrF::usage);
}

void
SetrF::usage(ostream& o) {
	o << "setr <node> <0|F|1|T|X|U>" << endl;
	o << "\tsets node at a random time in the future.\n"
"\tNodes with already pending events in queue retain their former event.\n"
"\tThis variation overrides pending events on the node, if any."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(Setrwhen, "setrwhen", simulation,
//	"set node with random delay after event")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/reschedule.texi
@deffn Command reschedule node time
@deffnx Command reschedule-from-now node time
@deffnx Command reschedule-relative node time
@deffnx Command reschedule-now node
If there is a pending event on @var{node} in the event queue, 
reschedule it as follows:
@command{reschedule} interprets @var{time} as an absolute time.
@command{reschedule-from-now} interprets @var{time} relative 
to the current time.
@command{reschedule-relative} interprets @var{time} relative to the 
pending event's presently scheduled time.  
The resulting rescheduled time cannot be in the past; 
it must be greater than or equal to the current time.  
Tie-breakers: given a group of events with the same time, 
a newly rescheduled event at that time will be *last* among them.
@command{reschedule-now}, however, will guarantee that the 
rescheduled event occurs next at the current time.  
Return with error status if there is no pending event on @var{node}.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Reschedule, "reschedule", simulation,
	"reschedule event on node to absolute time")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RescheduleNow, "reschedule-now",
	simulation,
	"reschedule event at current time")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RescheduleFromNow, "reschedule-from-now", 
	simulation,
	"reschedule event on node to future time")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RescheduleRelative, "reschedule-relative", 
	simulation,
	"reschedule event relative to pending event time")

static
int
reschedule_main(State& s, const string_list& a,
		bool (State::*smf)(const node_index_type, const time_type), 
		void (usage)(ostream&)) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(*++a.begin());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
if (ni) {
	time_type t;
	if (string_to_num(a.back(), t)) {
		cerr << "Error: invalid time argument." << endl;
		return Command::BADARG;
	}
	if ((s.*smf)(ni, t)) {
		// already have error message
		return Command::BADARG;
	}
	return Command::NORMAL;
} else {
	cerr << "No such node found: " << objname << endl;
	return Command::BADARG;
}
}
}

int
RescheduleNow::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
if (ni) {
	if (s.reschedule_event_now(ni)) {
		// already have error message
		return Command::BADARG;
	}
	return Command::NORMAL;
} else {
	cerr << "No such node found: " << objname << endl;
	return Command::BADARG;
}
}
}

int
Reschedule::main(State& s, const string_list& a) {
	return reschedule_main(s, a, &State::reschedule_event, usage);
}

int
RescheduleFromNow::main(State& s, const string_list& a) {
	return reschedule_main(s, a, &State::reschedule_event_future, usage);
}

int
RescheduleRelative::main(State& s, const string_list& a) {
	return reschedule_main(s, a, &State::reschedule_event_relative, usage);
}

void
RescheduleNow::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << "Reschedules a pending event on node to current time, *now*."
		<< endl;
}

void
Reschedule::usage(ostream& o) {
	o << name << " <node> <time>" << endl;
	o << "Reschedules a pending event on node at an absolute time." << endl;
}

void
RescheduleFromNow::usage(ostream& o) {
	o << name << " <node> <time>" << endl;
	o << "Reschedules a pending event on node at a time in future." << endl;
}

void
RescheduleRelative::usage(ostream& o) {
	o << name << " <node> <time>" << endl;
	o <<
"Reschedules a pending event on node relative to its currently scheduled time."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/execute.texi
@deffn Command execute node
Reschedules a pending event to the current time and executes it immediately.
Equivalent to @command{reschedule-now node}, followed by @command{step-event}.
@end deffn
@end texinfo
***/

DECLARE_AND_INITIALIZE_COMMAND_CLASS(Execute, "execute", simulation,
	"execute a pending event now")

int
Execute::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
if (ni) {
	// RescheduleNow::main
	if (s.reschedule_event_now(ni)) {
		// already have error message
		return Command::BADARG;
	}
	// StepEvent::main
	return step_event_main(s, 1);
} else {
	cerr << "No such node found: " << objname << endl;
	return Command::BADARG;
}
}
}

void
Execute::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << "Execute a pending event on the node at the current time." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/breakpt.texi
@deffn Command breakpt node
Set a breakpoint on @var{node}.  
When @var{node} changes value, interrupt the simulation
(during @command{cycle}, @command{advance}, or @command{step}), 
and return control back to the interpreter.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(BreakPt, "breakpt", simulation,
	"set breakpoint on node")	// no vector support yet

int
BreakPt::main(State& s, const string_list& a) {
	CommandRegistry::forbid_cosimulation(a);
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& objname(*i);
		const node_index_type ni =
			parse_node_to_index(objname, s.get_module());
		if (ni) {
			s.set_node_breakpoint(ni);
		} else {
			cerr << "No such node found: " << objname << endl;
			badarg = true;
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
BreakPt::usage(ostream& o) {
	o << "breakpt <nodes>" << endl;
	o << "causes simulation to stop upon any transition of the named nodes"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/nobreakpt.texi
@deffn Command nobreakpt node
@deffnx Command unbreak node
Removes breakpoint on @var{node}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoBreakPt, "nobreakpt", simulation,
	"remove breakpoint on node")	// no vector support yet

int
NoBreakPt::main(State& s, const string_list& a) {
	CommandRegistry::forbid_cosimulation(a);
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& objname(*i);
		const node_index_type ni =
			parse_node_to_index(objname, s.get_module());
		if (ni) {
			s.clear_node_breakpoint(ni);
		} else {
			cerr << "No such node found: " << objname << endl;
			badarg = true;
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
NoBreakPt::usage(ostream& o) {
	o << "nobreakpt <nodes>" << endl;
	o << "removes named node from breakpoint list" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnBreak, "unbreak", simulation,
	"alias for \'nobreakpt\'")	// no vector support yet

int
UnBreak::main(State& s, const string_list& a) {
	return NoBreakPt::main(s, a);
}

void
UnBreak::usage(ostream& o) {
	NoBreakPt::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/nobreakptall.texi
@deffn Command nobreakptall
@deffnx Command unbreakall
Removes all breakpoints.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoBreakPtAll, "nobreakptall", simulation,
	"remove all breakpoints")

int
NoBreakPtAll::main(State& s, const string_list& a) {
	CommandRegistry::forbid_cosimulation(a);
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.clear_all_breakpoints();
	return Command::NORMAL;
}
}

void
NoBreakPtAll::usage(ostream& o) {
	o << "nobreakptall" << endl;
	o << "clears all breakpoints" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnBreakAll, "unbreakall", simulation,
	"alias for \'nobreakptall\'")

/**
	Just an alias to nobreakptall.
 */
int
UnBreakAll::main(State& s, const string_list& a) {
	return NoBreakPtAll::main(s, a);
}

void
UnBreakAll::usage(ostream& o) {
	NoBreakPtAll::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/breaks.texi
@deffn Command breaks
Show all breakpoints (nodes).  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Breaks, "breaks", simulation,
	"list all nodes that are breakpoints")

int
Breaks::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_breakpoints(cout);
	return Command::NORMAL;
}
}

void
Breaks::usage(ostream& o) {
	o << "breaks";
	o << "lists all breakpoint nodes" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/save.texi
@deffn Command save ckpt
Saves the current state of the production rules and nodes into 
a checkpoint file @var{ckpt}.  The checkpoint file can be loaded to 
resume or replay a simulation later.  
@end deffn
@end texinfo
***/
typedef	Save<State>				Save;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Save, PRSIM::simulation)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/load.texi
@deffn Command load ckpt
Loads a @command{hacprsim} checkpoint file into the simulator state.
@end deffn
@end texinfo
***/
typedef	Load<State>				Load;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Load, PRSIM::simulation)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/autosave.texi
@deffn Command autosave [on|off]
Automatically save checkpoint upon end of simulation, 
regardless of exit status.
@end deffn
@end texinfo
***/
typedef	AutoSave<State>				AutoSave;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::AutoSave, PRSIM::simulation)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/ls.texi
@deffn Command ls name
List immediate subinstances of the instance named @var{name}.  
@end deffn
@end texinfo
***/
typedef	LS<State>				LS;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::LS, PRSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/what.texi
@deffn Command what name
Print the type of the instance named @var{name}.  
@end deffn
@end texinfo
***/
typedef	What<State>				What;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::What, PRSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/who.texi
@deffn Command who name
@deffnx Command who-newline name
Print all equivalent aliases of instance @var{name}.  
The @option{-newline} variant separates names by line instead of spaces
for improved readability.  
@end deffn
@end texinfo
***/
typedef	Who<State>				Who;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Who, PRSIM::info)
typedef	WhoNewline<State>			WhoNewline;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::WhoNewline, PRSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/pending.texi
@deffn Command pending node
@deffnx Command pending-debug node
Shows pending event on node (if any).
The debug variant shows the internal event index number, 
which should really never be exposed to the public API.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Pending, "pending", info,
	"print any pending event on node")

int
Pending::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		// we have ni = the canonically allocated index of the bool node
		// just look it up in the node_pool
		s.dump_node_pending(cout, ni, false);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
Pending::usage(ostream& o) {
	o << "pending <node>" << endl;
	o << "prints pending even on node if there is one" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(PendingDebug, "pending-debug", debug,
	"print any pending event on node (with index)")

int
PendingDebug::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		s.dump_node_pending(cout, ni, true);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
PendingDebug::usage(ostream& o) {
	o << "pending-debug <node>" << endl;
	o << "prints pending even on node if there is one" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/check-structure.texi
@deffn Command check-structure
Check internal graph/tree/map data structures for consistency and coherence.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(CheckStructure, "check-structure", debug,
	"check internal data structures")

int
CheckStructure::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	try {
		s.check_structure();
		// will ISE (crash) on inconsistency (intentional)
	} catch (...) {
		return Command::FATAL;
	}
	return Command::NORMAL;
}
}

void
CheckStructure::usage(ostream& o) {
	o << "check-structure" << endl;
	o << "Halt on error if any internal data structure "
		"inconsistencies are found." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/check-queue.texi
@deffn Command check-queue
Check internal event queue (as seen by @command{queue}) for
inconsistencies, such as missing back-links, multiply referenced nodes...
(Because the simulator was not perfect.)
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(CheckQueue, "check-queue", debug,
	"check for inconsistencies in event-queue")

int
CheckQueue::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	try {
		s.check_event_queue();
		// will ISE (crash) on inconsistency (intentional)
	} catch (...) {
		return Command::FATAL;
	}
	return Command::NORMAL;
}
}

void
CheckQueue::usage(ostream& o) {
	o << "check-queue" << endl;
	o << "Halt on error if any event-node inconsistencies are found."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/get.texi
@deffn Command get node
Print the current value of @var{node}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Get, "get", info,
	"print value of node/vector")

/**
	Prints current value of the named node.  
 */
int
Get::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		// we have ni = the canonically allocated index of the bool node
		// just look it up in the node_pool
		print_watched_node(cout, s, ni, objname);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
Get::usage(ostream& o) {
	o << "get <node>" << endl;
	o <<
"Print the current value of the node and, if applicable, the last arriving\n"
"signal transition that caused it to become its current value."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/getall.texi
@deffn Command getall struct
Print the state of all subnodes of @var{struct}.  
Useful for observing channels and processes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(GetAll, "getall", info,
	"print values of all subnodes")

int
GetAll::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	vector<node_index_type>		nodes_id_list_type;
	const string& objname(a.back());
	nodes_id_list_type nodes;
	if (parse_name_to_get_subnodes(cout, objname, s.get_module(), nodes)) {
		// already got error message?
		return Command::BADARG;
	} else {
		typedef	nodes_id_list_type::const_iterator	const_iterator;
		cout << "All subnodes of \'" << objname << "\':" << endl;
		const_iterator i(nodes.begin()), e(nodes.end());
		for ( ; i!=e; ++i) {
			s.dump_node_value(cout, *i) << endl;
		}
		return Command::NORMAL;
	}
}
}

void
GetAll::usage(ostream& o) {
	o << "getall <name>" << endl;
	o << "prints the current values of all subnodes of the named structure"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/status.texi
@deffn Command status value
@deffnx Command status-newline value
Print all nodes whose current value is @var{value}.  
Frequently used to find nodes that are in an unknown ('X') state.  
Valid @var{value} arguments are [0fF] for logic-low, [1tT] for logic-high,
[xXuU] for unknown value.  
The @command{-newline} variant prints each node on a separate line
for readability.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Status, "status", info, 
	"show all nodes matching a state value")

int
Status::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	const value_enum v = node_type::string_to_value(a.back());
	if (node_type::is_valid_value(v)) {
		s.status_nodes(cout, v, false);
		return Command::NORMAL;
	} else {
		cerr << "Bad status value." << endl;
		usage(cerr);
		return Command::BADARG;
	}
}
}

void
Status::usage(ostream& o) {
	o << "status <[0fF1tTxXuU]>" << endl;
	o << "list all nodes with the matching current value" << endl;
}

DECLARE_AND_INITIALIZE_COMMAND_CLASS(StatusNewline, "status-newline", info, 
	"show all nodes matching a value, line separated")

int
StatusNewline::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	const value_enum v = node_type::string_to_value(a.back());
	if (node_type::is_valid_value(v)) {
		s.status_nodes(cout, v, true);
		return Command::NORMAL;
	} else {
		cerr << "Bad status value." << endl;
		usage(cerr);
		return Command::BADARG;
	}
}
}

void
StatusNewline::usage(ostream& o) {
	o << "status-newline <[0fF1tTxXuU]>" << endl;
	o << "list all nodes with the matching current value" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unknown-inputs.texi
@deffn Command unknown-inputs
Print all nodes with value X that have no fanins, i.e. input-only nodes.  
Connections to channel sinks or sources can count as inputs (fake fanin).
Great for debugging forgotten environment inputs and connections!
This variant includes X-nodes with no fanouts (unused nodes).  
@end deffn
@end texinfo
***/

DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnknownInputs, "unknown-inputs", info, 
	"list all nodes at value X with no fanin")

int
UnknownInputs::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_dangling_unknown_nodes(cout, true);
	return Command::NORMAL;
}
}

void
UnknownInputs::usage(ostream& o) {
	o << name <<
	" -- list all nodes at value X with no fanin (channels counted)."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unknown-inputs-fanout.texi
@deffn Command unknown-inputs-fanout
Print all nodes with value X that have no fanins, i.e. input-only nodes.  
This variant excludes X-nodes with no fanouts (unused nodes).  
@end deffn
@end texinfo
***/

DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnknownInputsFanout,
	"unknown-inputs-fanout", info, 
	"list all X nodes with no fanin, with fanout")

int
UnknownInputsFanout::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_dangling_unknown_nodes(cout, false);
	return Command::NORMAL;
}
}

void
UnknownInputsFanout::usage(ostream& o) {
	o << name <<
	" -- list X nodes with no fanin, with fanout (channels counted)."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unknown-outputs.texi
@deffn Command unknown-outputs
Print all nodes with value X that have no fanouts, i.e. output-only nodes.  
Connections to channel sinks and sources can counts as outputs (fake fanout).
This will not catch output nodes that are fed back into circuits.  
This variant excludes X-nodes with no fanouts (unused nodes).  
@end deffn
@end texinfo
***/

DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnknownOutputs,
	"unknown-outputs", info, 
	"list all X nodes with no fanout")

int
UnknownOutputs::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_output_unknown_nodes(cout);
	return Command::NORMAL;
}
}

void
UnknownOutputs::usage(ostream& o) {
	o << name << " -- list X nodes with no fanout (channels counted)."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/fanin.texi
@deffn Command fanin node
Print all production rules that can fire @var{NODE}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Fanin, "fanin", info, 
	"print rules that influence a node")

int
Fanin::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		// const State::node_type& n(s.get_node(ni));
		cout << "Fanins of node `" << objname << "\':" << endl;
		s.dump_node_fanin(cout, ni, false);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
Fanin::usage(ostream& o) {
	o << "fanin <node>" << endl;
	o << "print all rules and expressions that can affect this node"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/fanin-get.texi
@deffn Command fanin-get node
Print all production rules that can fire @var{NODE}.  
Also prints current values of all expression literals.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(FaninGet, "fanin-get", info, 
	"print rules that influence a node, with values")

int
FaninGet::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		// const State::node_type& n(s.get_node(ni));
		cout << "Fanins of node `" << objname << "\':" << endl;
		s.dump_node_fanin(cout, ni, true);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
FaninGet::usage(ostream& o) {
	o << "fanin-get <node>" << endl;
	o << "print all rules and expressions that can affect this node, "
		<< endl
	<< "also shows current values of expression literals."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/fanout.texi
@deffn Command fanout node
Print all production rules that @var{NODE} participates in.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Fanout, "fanout", info, 
	"print rules that a node influences")

int
Fanout::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		// const State::node_type& n(s.get_node(ni));
		cout << "Fanouts of node `" << objname << "\':" << endl;
		s.dump_node_fanout(cout, ni, false);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
Fanout::usage(ostream& o) {
	o << "fanout <node>" << endl;
	o << "print all rules affected by this node" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/fanout-get.texi
@deffn Command fanout-get node
Print all production rules that @var{NODE} participates in.  
Also prints current values of all expression literals.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(FanoutGet, "fanout-get", info, 
	"print rules that a node influences, with values")

int
FanoutGet::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		// const State::node_type& n(s.get_node(ni));
		cout << "Fanouts of node `" << objname << "\':" << endl;
		s.dump_node_fanout(cout, ni, true);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
FanoutGet::usage(ostream& o) {
	o << "fanout-get <node>" << endl;
	o << "print all rules affected by this node, " 
		<< endl
	<< "also shows current values of expression literals."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/rings-mk.texi
@deffn Command rings-mk node
Print forced exclusive high/low rings of which @var{node} is a member.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RingsMk, "rings-mk", info, 
	"print forced exclusive rings of which a node is a member")

int
RingsMk::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		s.dump_node_mk_excl_rings(cout, ni);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
RingsMk::usage(ostream& o) {
	o << "rings-mk <node>" << endl;
	o << "print all forced-exclusive rings of which this node is a member"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/allrings-mk.texi
@deffn Command allrings-mk
Print all forced exclusive high/low rings.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllRingsMk, "allrings-mk", info, 
	"dump all forced exclusive hi/lo rings")

int
AllRingsMk::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_mk_exclhi_rings(cout);
	s.dump_mk_excllo_rings(cout);
	return Command::NORMAL;
}
}

void
AllRingsMk::usage(ostream& o) {
	o << "allrings-mk" << endl;
	o << "print all forced-exclusive rings" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/rings-chk.texi
@deffn Command rings-chk node
Print all checked exclusive rings of which @var{node} is a member.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RingsChk, "rings-chk", info, 
	"print checked exclusive rings of which a node is a member")

int
RingsChk::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		s.dump_node_check_excl_rings(cout, ni);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
RingsChk::usage(ostream& o) {
	o << "rings-chk <node>" << endl;
	o << "print all checked-exclusive rings of which this node is a member"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/allrings-chk.texi
@deffn Command allrings-chk
Print all checked exclusive rings of nodes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllRingsChk, "allrings-chk", info, 
	"dump all checked exclusive hi/lo rings")

int
AllRingsChk::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_check_exclhi_rings(cout);
	s.dump_check_excllo_rings(cout);
	return Command::NORMAL;
}
}

void
AllRingsChk::usage(ostream& o) {
	o << "allrings-chk" << endl;
	o << "print all checked-exclusive rings" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// will category conflict with command?
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(Info, "info", info, 
//	"print information about a node/vector")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/assert.texi
@deffn Command assert node value
Error out if @var{node} is not at value @var{value}.  
The error-handling policy can actually be determined by
the @command{assert-fail} command.  
By default, such errors are fatal and cause the simulator to terminate
upon first error.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Assert, "assert", info, 
	"error if node is NOT expected value")

/**
	Checks expected value of node against actual value.  
	\return FATAL on assertion failure.  
 */
int
Assert::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	const string& objname(*++a.begin());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		const node_type& n(s.get_node(ni));
		const string& _val(a.back());	// node value
		// valid values are 0, 1, 2(X)
		const value_enum val = node_type::string_to_value(_val);
		if (!node_type::is_valid_value(val)) {
			cerr << "Invalid logic value: " << _val << endl;
			return Command::SYNTAX;
		}
		const value_enum actual = n.current_value();
		if (actual != val) {
			const error_policy_enum e(s.get_assert_fail_policy());
			if (e != ERROR_IGNORE) {
			cout << "assert failed: expecting node `" << objname <<
				"\' at " <<
				node_type::value_to_char[size_t(val)] <<
				", but got ";
			n.dump_value(cout) << "." << endl;
			}	// yes, actually allow suppression
			return error_policy_to_status(e);
		} else if (s.confirm_asserts()) {
			cout << "node `" << objname << "\' is " <<
				node_type::value_to_char[size_t(val)] <<
				", as expected." << endl;
		}
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
Assert::usage(ostream& o) {
	o << "assert <node> <value>" << endl;
	o << "signal an error and halt simulation if node is not at this value"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/assertn.texi
@deffn Command assertn node value
Error out if @var{node} @emph{is} at value @var{value}.  
Error handling policy can be set by the @command{assert-fail} command.
By default such errors are fatal.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AssertN, "assertn", info, 
	"error if node IS expected value")

/**
	Checks expected value of node against actual value.  
	\return FATAL on assertion failure.  
 */
int
AssertN::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	const string& objname(*++a.begin());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		const node_type& n(s.get_node(ni));
		const string& _val(a.back());	// node value
		// valid values are 0, 1, 2(X)
		const value_enum val = node_type::string_to_value(_val);
		if (!node_type::is_valid_value(val)) {
			cerr << "Invalid logic value: " << _val << endl;
			return Command::SYNTAX;
		}
		const value_enum actual = n.current_value();
		if (actual == val) {
			const error_policy_enum e(s.get_assert_fail_policy());
			if (e != ERROR_IGNORE) {
			cout << "assert failed: expecting node `" << objname <<
				"\' not at " <<
				node_type::value_to_char[size_t(val)] <<
				", but got ";
			n.dump_value(cout) << "." << endl;
			}
			return error_policy_to_status(e);
		} else if (s.confirm_asserts()) {
			cout << "node `" << objname << "\' is not " <<
				node_type::value_to_char[size_t(val)] <<
				", as expected." << endl;
		}
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
AssertN::usage(ostream& o) {
	o << "assertn <node> <value>" << endl;
	o << "signal an error and halt simulation if node is at this value"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/assert-pending.texi
@deffn Command assert-pending node
Error out if @var{node} does not have a pending event in queue.
The error handling policy is determined by the @command{assert-fail} command.
By default, such assertion failures are fatal.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AssertPending, "assert-pending", info, 
	"error if node does not have event in queue")


int
AssertPending::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	const string& objname(*++a.begin());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		const node_type& n(s.get_node(ni));
		if (!n.pending_event()) {
			const error_policy_enum e(s.get_assert_fail_policy());
			if (e != ERROR_IGNORE) {
			cout <<
			"assert failed: expecting pending event on node `"
				<< objname << "\', but none found." << endl;
			}
			return error_policy_to_status(e);
		} else if (s.confirm_asserts()) {
			cout << "node `" << objname <<
				"\' has a pending event, as expected." << endl;
		}
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
AssertPending::usage(ostream& o) {
	o << "assert-pending <node>" << endl;
	o << "signal an error and halt if node has pending event" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/assertn-pending.texi
@deffn Command assert-pending node
Error out if @var{node} does have a pending event in queue.
The error handling policy is determined by the @command{assert-fail} command.
By default, such assertion failures are fatal.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AssertNPending, "assertn-pending", info, 
	"error if node does have event in queue")

int
AssertNPending::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	const string& objname(*++a.begin());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		const node_type& n(s.get_node(ni));
		if (n.pending_event()) {
			const error_policy_enum e(s.get_assert_fail_policy());
			if (e != ERROR_IGNORE) {
			cout <<
			"assert failed: expecting no pending event on node `"
				<< objname << "\', but found one." << endl;
			}
			return error_policy_to_status(e);
		} else if (s.confirm_asserts()) {
			cout << "node `" << objname <<
				"\' has no pending event, as expected." << endl;
		}
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
AssertNPending::usage(ostream& o) {
	o << "assertn-pending <node>" << endl;
	o << "signal an error and halt if node has no pending event" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/assert-queue.texi
@deffn Command assert-queue
Error out if event queue is empty.  
Useful for checking for deadlock.  
The error handling policy is determined by the @command{assert-fail} command.
By default, such assertion failures are fatal.  
@end deffn
@end texinfo
***/
typedef	AssertQueue<State>			AssertQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::AssertQueue, PRSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/assertn-queue.texi
@deffn Command assertn-queue
Error out if event queue is not empty.  
Useful for checking for checking result of cycle.  
The error handling policy is determined by the @command{assert-fail} command.
By default, such assertion failures are fatal.  
@end deffn
@end texinfo
***/
typedef	AssertNQueue<State>			AssertNQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::AssertNQueue, PRSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/time.texi
@deffn Command time
What time is it (in the simulator)?
@end deffn
@end texinfo
***/
typedef	Time<State>				Time;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Time, PRSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/confirm.texi
@deffn Command confirm
@deffnx Command noconfirm
Controls whether or not correct assertions are reported.  
@end deffn
@end texinfo
***/
typedef	Confirm<State>				Confirm;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Confirm, PRSIM::info)
typedef	NoConfirm<State>			NoConfirm;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::NoConfirm, PRSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/backtrace.texi
@deffn Command backtrace node
Trace backwards through a history of last-arriving transitions on
node @var{node}, until a cycle is found.  
Useful for tracking down causes of instabilities, 
and identifying critical paths and cycle times.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(BackTrace, "backtrace", info, 
	"trace backwards partial event causality history")

int
BackTrace::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		s.backtrace_node(cout, ni);
		return Command::NORMAL;
	} else {
		return Command::BADARG;
	}
}
}

void
BackTrace::usage(ostream& o) {
	o << name << " <node>" << endl;
	o <<
"Traces back history of last-arriving events until cycle found.\n"
"This is useful for finding critical paths and diagnosing instabilities."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/why-x.texi
@deffn Command why-x node
@deffnx Command why-x-verbose node
@deffnx Command why-x-1 node
@deffnx Command why-x-1-verbose node
@deffnx Command why-x-N node maxdepth
@deffnx Command why-x-N-verbose node maxdepth
Print causality chain for why a particular node (at value X) remains X.  
In expressions, X nodes that are masked out (e.g. 1 | X or 0 & X) 
are not followed.  
The verbose variant prints more information about the subexpressions
visited (whether conjunctive or disjunctive), 
and pretty prints in tree-indent form.  
Recursion terminates on cycles and already-visited nodes.  
The @option{-1} variant only queries through depth 1, 
and the @option{-N} variant queries to a maximum depth of @var{maxdepth}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyX, "why-x", info, 
	"recursively trace cause of X value on node")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyXVerbose, "why-x-verbose", info, 
	"recursively trace cause of X value on node (verbose)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyXN, "why-x-N", info, 
	"recursively trace cause of X on node (N-level)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyXNVerbose, "why-x-N-verbose", info, 
	"recursively trace cause of X (N-level, verbose)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyX1, "why-x-1", info, 
	"trace cause of X on node (1-level)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyX1Verbose, "why-x-1-verbose", info, 
	"trace cause of X on node (1-level, verbose)")

static
int
__why_X_N_main(State& s, const node_index_type ni, 
		const size_t limit, const bool verbose) {
	if (ni) {
		s.dump_node_why_X(cout, ni, limit, verbose);
		return Command::NORMAL;
	} else {
		return Command::BADARG;
	}
}

static
int
why_X_N_main(State& s, const string_list& a, const bool verbose) {
	const string& objname(*++a.begin());
	const string& _limit(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	size_t limit;
	if (string_to_num(_limit, limit)) {
		cerr << "Error: expected integer depth-limit argument." << endl;
		return Command::BADARG;
	}
	return __why_X_N_main(s, ni, limit, verbose);
}

static
int
why_X_main(State& s, const string_list& a, 
		const size_t limit, const bool verbose) {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	return __why_X_N_main(s, ni, limit, verbose);
}

int
WhyX::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	return why_X_main(s, a, size_t(-1), false);
}
}

int
WhyXVerbose::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	return why_X_main(s, a, size_t(-1), true);
}
}

int
WhyX1::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	return why_X_main(s, a, 1, false);
}
}

int
WhyX1Verbose::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	return why_X_main(s, a, 1, true);
}
}

int
WhyXN::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	return why_X_N_main(s, a, false);
}
}

int
WhyXNVerbose::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	return why_X_N_main(s, a, true);
}
}

void
WhyX::usage(ostream& o) {
	o << name << " <node>" << endl;
	o <<
"Recursively finds the cause for the node being X through other X nodes.\n"
"Verbose variant prints more details about expression structures."
		<< endl;
}

void
WhyXVerbose::usage(ostream& o) {
	WhyX::usage(o);
}

void
WhyX1::usage(ostream& o) {
	o << name << " <node>" << endl;
	o <<
"Finds the cause for the node being X through other X nodes, depth 1." << endl;
}

void
WhyX1Verbose::usage(ostream& o) {
	WhyX1::usage(o);
}

void
WhyXN::usage(ostream& o) {
	o << name << " <node> <limit>" << endl;
	o <<
"Finds the cause for the node being X through other X nodes, depth N." << endl;
}

void
WhyXNVerbose::usage(ostream& o) {
	WhyXN::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/why.texi
@deffn Command why node [val]
@deffnx Command why-verbose node [val]
@deffnx Command why-1 node [val]
@deffnx Command why-1-verbose node [val]
@deffnx Command why-N node maxdepth [val]
@deffnx Command why-N-verbose node maxdepth [val]
Print reason for node being driven to a given value, 0 or 1.
X is not a valid value for this procedure.  
If @var{val} is not given, it is assumed to be the current value of the node.  
The algorithm examines each node's fanins and follows
nodes on paths where the subexpression is true 
(path through transistors is on).  
The analysis will terminate at state-holding nodes that are not being
driven to their current value.
This is an excellent aid in debugging unexpected values.  
The verbose variant prints expression types as it auto-indents, 
which is more informative but may appear more cluttered.  
The @option{-1} variant only queries through depth 1, 
and the @option{-N} variant queries to a maximum depth of @var{maxdepth}.  
@end deffn
@end texinfo
***/
/***
@texinfo cmd/why-not.texi
@deffn Command why-not node [val]
@deffnx Command why-not-verbose node [val]
@deffnx Command why-not-1 node [val]
@deffnx Command why-not-1-verbose node [val]
@deffnx Command why-not-N node maxdepth [val]
@deffnx Command why-not-N-verbose node maxdepth [val]
Print reason for node not being a given value, 0 or 1.
X is not a valid value for this procedure.  
If @var{val} is not given, it is assumed to be opposite of the
current value of the node.  
"Why isn't this node changing?"
The algorithm examines each node's fanins and follows
nodes that prevent the relevant expression from evaluating true.  
This is an excellent tool for debugging deadlocks.  
The verbose variant prints expression types as it auto-indents, 
which is more informative but may appear more cluttered.  
The @option{-1} variant only queries through depth 1, 
and the @option{-N} variant queries to a maximum depth of @var{maxdepth}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Why, "why", info, 
	"recursively trace why node is driven to value")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyVerbose, "why-verbose", info, 
	"recursively trace why node is driven to value (verbose)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Why1, "why-1", info, 
	"trace why node is driven to value")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Why1Verbose, "why-1-verbose", info, 
	"trace why node is driven to value (verbose)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyN, "why-N", info, 
	"recursively trace why node is driven to value")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyNVerbose, "why-N-verbose", info, 
	"recursively trace why node is driven to value (verbose)")

DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyNot, "why-not", info, 
	"recursively trace why node is not at value")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyNotVerbose, "why-not-verbose", info, 
	"recursively trace why node is not value (verbose)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyNot1, "why-not-1", info, 
	"trace why node is not at value")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyNot1Verbose, "why-not-1-verbose", info, 
	"trace why node is not at value (verbose)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyNotN, "why-not-N", info, 
	"recursively trace why node is not at value")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WhyNotNVerbose, "why-not-N-verbose", info, 
	"recursively trace why node is not value (verbose)")

/**
	Template procedure for invoking node_why[_not] with various options.
	\param b optional: value to query (or NULL -> automatic)
	\param why_not true if querying negatively
	\param verbose true if expression structure verbosity requested
 */
static
int
__why_not_N_main(State& s, const node_index_type ni, 
		const string* b,
		const size_t limit, const bool why_not, const bool verbose, 
		void (usage)(ostream&)) {
#if 0
const size_t a_s = a.size();
if (a_s < 2 || a_s > 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(*++a.begin());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
#endif
if (ni) {
	bool v;
	if (b) {
		if (*b == "0") {
			v = false;
		} else if (*b == "1") {
			v = true;
		} else {
			usage(cerr << "usage: ");
			return Command::BADARG;
		}
	} else {
		typedef	State::node_type	node_type;
		switch (s.get_node(ni).current_value()) {
		case LOGIC_LOW:
			v = why_not;
			break;
		case LOGIC_HIGH:
			v = !why_not;
			break;
		default:
			cerr << "why[-not] <node> X is not supported." << endl;
			return Command::BADARG;
		}
	}
	s.dump_node_why_not(cout, ni, limit, v, why_not, verbose);
	return Command::NORMAL;
} else {
	return Command::BADARG;
}
#if 0
}
#endif
}

static
int
why_not_main(State& s, const string_list& a, const size_t limit, 
		const bool why_not, const bool verbose, 
		void (usage)(ostream&)) {
const size_t a_s = a.size();
if (a_s < 2 || a_s > 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(*++a.begin());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	return __why_not_N_main(s, ni, (a_s == 3 ? &a.back() : NULL),
		limit, why_not, verbose, usage);
}
}

static
int
why_not_N_main(State& s, const string_list& a,
		const bool why_not, const bool verbose, 
		void (usage)(ostream&)) {
const size_t a_s = a.size();
if (a_s < 3 || a_s > 4) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(*++a.begin());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	const string& _limit(*++++a.begin());
	size_t limit;
	if (string_to_num(_limit, limit)) {
		cerr << "Error: expected integer depth-limit argument." << endl;
		return Command::BADARG;
	}
	return __why_not_N_main(s, ni, (a_s == 4 ? &a.back() : NULL),
		limit, why_not, verbose, usage);
}
}

int
Why::main(State& s, const string_list& a) {
	return why_not_main(s, a, size_t(-1), false, false, usage);
}

int
WhyVerbose::main(State& s, const string_list& a) {
	return why_not_main(s, a, size_t(-1), false, true, usage);
}

int
WhyNot::main(State& s, const string_list& a) {
	return why_not_main(s, a, size_t(-1), true, false, usage);
}

int
WhyNotVerbose::main(State& s, const string_list& a) {
	return why_not_main(s, a, size_t(-1), true, true, usage);
}

int
Why1::main(State& s, const string_list& a) {
	return why_not_main(s, a, 1, false, false, usage);
}

int
Why1Verbose::main(State& s, const string_list& a) {
	return why_not_main(s, a, 1, false, true, usage);
}

int
WhyNot1::main(State& s, const string_list& a) {
	return why_not_main(s, a, 1, true, false, usage);
}

int
WhyNot1Verbose::main(State& s, const string_list& a) {
	return why_not_main(s, a, 1, true, true, usage);
}

int
WhyN::main(State& s, const string_list& a) {
	return why_not_N_main(s, a, false, false, usage);
}

int
WhyNVerbose::main(State& s, const string_list& a) {
	return why_not_N_main(s, a, false, true, usage);
}

int
WhyNotN::main(State& s, const string_list& a) {
	return why_not_N_main(s, a, true, false, usage);
}

int
WhyNotNVerbose::main(State& s, const string_list& a) {
	return why_not_N_main(s, a, true, true, usage);
}

static
void
why_usage(ostream& o, const char* name) {
	o << name << " <node> [01]" << endl;
	o <<
"Recursively trace back reason why named node is driven to a given value.\n"
"Value must be 0 or 1.  If value is omitted, then assumes the current value.\n"
"This can be used to detect unexpected firings in circuits."
	<< endl;
}

static
void
why_not_usage(ostream& o, const char* name) {
	o << name << " <node> [01]" << endl;
	o <<
"Recursively trace back reason why named node has not become a given value.\n"
"Value must be 0 or 1.  If value is omitted, then assumes the opposite value.\n"
"This can be used to detect deadlock in circuits."
	<< endl;
}

static
void
why_1_usage(ostream& o, const char* name) {
	o << name << " <node> [01]" << endl;
	o <<
"Query why named node is driven to a given value.\n"
"Value must be 0 or 1.  If value is omitted, then assumes the current value.\n"
"This can be used to detect unexpected firings in circuits."
	<< endl;
}

static
void
why_not_1_usage(ostream& o, const char* name) {
	o << name << " <node> [01]" << endl;
	o <<
"Query why named node has not become a given value.\n"
"Value must be 0 or 1.  If value is omitted, then assumes the opposite value.\n"
"This can be used to detect deadlock in circuits."
	<< endl;
}

static
void
why_N_usage(ostream& o, const char* name) {
	o << name << " <node> <maxdepth> [01]" << endl;
	o <<
"Recursively trace back reason why named node is driven to a given value.\n"
"Value must be 0 or 1.  If value is omitted, then assumes the current value.\n"
"This can be used to detect unexpected firings in circuits.\n"
"Recursion depth is limited to maxdepth."
	<< endl;
}

static
void
why_not_N_usage(ostream& o, const char* name) {
	o << name << " <node> <maxdepth> [01]" << endl;
	o <<
"Recursively trace back reason why named node has not become a given value.\n"
"Value must be 0 or 1.  If value is omitted, then assumes the opposite value.\n"
"This can be used to detect deadlock in circuits.\n"
"Recursion depth is limited to maxdepth."
	<< endl;
}

void
Why::usage(ostream& o) {
	why_usage(o, name);
}

void
WhyVerbose::usage(ostream& o) {
	why_usage(o, name);
}

void
WhyNot::usage(ostream& o) {
	why_not_usage(o, name);
}

void
WhyNotVerbose::usage(ostream& o) {
	why_not_usage(o, name);
}

void
Why1::usage(ostream& o) {
	why_1_usage(o, name);
}

void
Why1Verbose::usage(ostream& o) {
	why_1_usage(o, name);
}

void
WhyNot1::usage(ostream& o) {
	why_not_1_usage(o, name);
}

void
WhyNot1Verbose::usage(ostream& o) {
	why_not_1_usage(o, name);
}

void
WhyN::usage(ostream& o) {
	why_N_usage(o, name);
}

void
WhyNVerbose::usage(ostream& o) {
	why_N_usage(o, name);
}

void
WhyNotN::usage(ostream& o) {
	why_not_N_usage(o, name);
}

void
WhyNotNVerbose::usage(ostream& o) {
	why_not_N_usage(o, name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/watch.texi
@deffn Command watch nodes...
Adds @var{nodes} to a watch-list of nodes to display when their values change,
much like @command{breakpt}, but doesn't interrupt simulation.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Watch, "watch", view, 
	"print activity on selected nodes")

/**
	Adds nodes to the watch list.  
	TODO: watch defined structures!
 */
int
Watch::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& objname(*i);
		const node_index_type ni =
			parse_node_to_index(objname, s.get_module());
		if (ni) {
			s.watch_node(ni);
		} else {
			cerr << "No such node found: " << objname << endl;
			badarg = true;
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
Watch::usage(ostream& o) {
	o << "watch <nodes>" << endl;
	o << "adds node(s) to watch-list.\n"
"Watched nodes print their transitions to stdout." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unwatch.texi
@deffn Command unwatch nodes...
Removes @var{nodes} from list of watched nodes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnWatch, "unwatch", view, 
	"silence activity reporting on selected nodes")

/**
	Removes a node from the watch list.  
 */
int
UnWatch::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& objname(*i);
		const node_index_type ni =
			parse_node_to_index(objname, s.get_module());
		if (ni) {
			s.unwatch_node(ni);
		} else {
			cerr << "No such node found: " << objname << endl;
			badarg = true;
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
UnWatch::usage(ostream& o) {
	o << "unwatch <nodes>" << endl;
	o << "removes nodes from watch-list" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/watchall.texi
@deffn Command watchall
@deffnx Command nowatchall
Display every node value change (regardless of present in watch-list).  
@command{nowatchall} restores the state where only explicitly
watched nodes are displayed on value changes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WatchAll, "watchall", view, 
	"print activity on all nodes (flag)")

/**
	Enables a watchall flag to report all transitions.  
 */
int
WatchAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.watch_all_nodes();
	return Command::NORMAL;
}
}

void
WatchAll::usage(ostream& o) {
	o << "watchall" << endl;
	o << "print all transitions to stdout." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnWatchAll, "unwatchall", view, 
	"remove all nodes from watchlist")

/**
	Enables a watchall flag to report all transitions.  
 */
int
UnWatchAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.unwatch_all_nodes();
	return Command::NORMAL;
}
}

void
UnWatchAll::usage(ostream& o) {
	o << "unwatchall" << endl;
	o << "clears the watch-list of nodes" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoWatchAll, "nowatchall", view, 
	"only print activity on explicitly watched nodes (flag)")

/**
	Disables global watchall, but still maintains explicitly
	watched nodes.  
 */
int
NoWatchAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.nowatch_all_nodes();
	return Command::NORMAL;
}
}

void
NoWatchAll::usage(ostream& o) {
	o << "nowatchall" << endl;
	o << "turns off watchall, printing only explicitly watched nodes"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/watches.texi
@deffn Command watches
Print list of all explicitly watched nodes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Watches, "watches", view,
	"list all nodes that are explicitly watched")

int
Watches::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_watched_nodes(cout);
	return Command::NORMAL;
}
}

void
Watches::usage(ostream& o) {
	o << "watches" << endl;
	o << "show list of explicitly watched nodes" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/watch-queue.texi
@deffn Command watch-queue
@deffnx Command nowatch-queue
Show changes to the event-queue as events on only watched nodes are scheduled.
Typically only used during debugging or detailed diagnostics.  
@end deffn
@end texinfo
***/
typedef	WatchQueue<State>			WatchQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::WatchQueue, PRSIM::view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	NoWatchQueue<State>			NoWatchQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::NoWatchQueue, PRSIM::view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/watchall-queue.texi
@deffn Command watchall-queue
@deffnx Command nowatchall-queue
Show changes to the event-queue as @emph{every} event is scheduled.
Typically only used during debugging or detailed diagnostics.  
@end deffn
@end texinfo
***/
typedef	WatchAllQueue<State>			WatchAllQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::WatchAllQueue, PRSIM::view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	NoWatchAllQueue<State>			NoWatchAllQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::NoWatchAllQueue, PRSIM::view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/cause.texi
@deffn Command cause
@deffnx Command nocause
@command{cause} displays causal information of nodes as they change value.  
@command{nocause} hides cause information, but silently keeps track.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Cause, "cause", view, 
	"show causal transitions on watched nodes")

int
Cause::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.set_show_cause();
	return Command::NORMAL;
}
}

void
Cause::usage(ostream& o) {
	o << name << endl;
	o << "report causal transitions of watched nodes" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoCause, "nocause", view, 
	"hide causal transitions on watched nodes")

int
NoCause::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.clear_show_cause();
	return Command::NORMAL;
}
}

void
NoCause::usage(ostream& o) {
	o << name << endl;
	o << "suppress causal transitions of watched nodes" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/tcounts.texi
@deffn Command tcounts
@deffnx Command notcounts
@command{tcounts} displays transition counts on nodes as they change value.  
@command{notcounts} hides transition count information.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(TCounts, "tcounts", view, 
	"show transition counts on watched nodes")

int
TCounts::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.set_show_tcounts();
	return Command::NORMAL;
}
}

void
TCounts::usage(ostream& o) {
	o << name<< endl;
	o << "report transition counts of watched nodes" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoTCounts, "notcounts", view, 
	"hide transition counts on watched nodes")

int
NoTCounts::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.clear_show_tcounts();
	return Command::NORMAL;
}
}

void
NoTCounts::usage(ostream& o) {
	o << name << endl;
	o << "suppress transition counts of watched nodes" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/zerotcounts.texi
@deffn Command zerotcounts
Reset all transition counts to 0.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ZeroTCounts, "zerotcounts", view, 
	"reset transition counts on all nodes")

int
ZeroTCounts::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.reset_tcounts();
	return Command::NORMAL;
}
}

void
ZeroTCounts::usage(ostream& o) {
	o << "zerotcounts" << endl;
	o << "reset transition counts of all nodes" << endl;
}

//-----------------------------------------------------------------------------
#if WANT_OLD_RANDOM_COMMANDS
/***
@texinfo cmd/random.texi
@deffn Command random
Deprecated, but retained for legacy compatibility.  
Synonymous with @samp{timing random}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Random, "random", modes, 
	"use random delays (deprecated)")

int
Random::main(State& s, const string_list& a) {
cerr << "WARNING: this command is deprecated, use \"timing random\" instead."
	<< endl;
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.randomize();
	return Command::NORMAL;
}
}

void
Random::usage(ostream& o) {
	o << "random (deprecated)";
	o << "same as: timing random" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/norandom.texi
@deffn Command norandom
Deprecated, but retained for legacy compatibility.  
Synonymous with @samp{timing uniform}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoRandom, "norandom", modes, 
	"use non-random delays (deprecated)")

int
NoRandom::main(State& s, const string_list& a) {
cerr << "WARNING: this command is deprecated, use \"timing ...\" instead."
	<< endl;
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.norandom();		// is this "uniform" or "after"?
	return Command::NORMAL;
}
}

void
NoRandom::usage(ostream& o) {
	o << "norandom (deprecated)";
	o << "disables random timing" << endl;
	o << "use: timing ..." << endl;
}

#endif	// WANT_OLD_RANDOM_COMMANDS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/eval-order.texi
@deffn Command eval-order [mode]
With no argument, reports the current evaluation ordering mode.
With @var{mode} (either @t{inorder} or @t{random}), 
fanouts are evaluated either in a pre-determined order, or in random order.
Random ordering is useful for emulating random arbitration among
fanouts of the same node.  
Default mode is @t{inorder}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(EvalOrder, "eval-order", modes, 
	"shuffle (or not) the evaluation ordering of fanouts")

int
EvalOrder::main(State& s, const string_list& a) {
if (a.size() == 1) {
	cout << "Fanout evaluation ordering is " <<
		(s.eval_ordering_is_random() ? "random" : "in-order") <<
		"." << endl;
	return Command::NORMAL;
} else if (a.size() == 2) {
	static const string random("random");
	static const string inorder("inorder");
	if (a.back() == random) {
		s.set_eval_ordering_random();
		return Command::NORMAL;
	} else if (a.back() == inorder) {
		s.set_eval_ordering_inorder();
		return Command::NORMAL;
	} else {
		usage(cerr << "usage: ");
		return Command::BADARG;
	}
} else {
	usage(cerr);
	return Command::SYNTAX;
}
}

void
EvalOrder::usage(ostream& o) {
	o << "eval-order [random|inorder]" << endl <<
"\tinorder - (default) evaluates fanout in sequential order of visit\n"
"\trandom - randomizes the evaluation ordering of fanouts, useful for \n"
"\t\temulating arbitration of forced exclusive rules with the same fanin."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_WEAK_RULES
/***
@texinfo cmd/weak-rules.texi
@deffn Command weak-rules [on|off|show|hide]
Simulation mode switch which globally enables or disables (ignores)
weak-rules.  
Weak-rules can only take effect when normal rules pulling a node are off.
The @t{hide} and @t{show} options control whether or not weak rules are 
displayed in rule queries, such as @command{fanin}, @command{fanout}, 
and @command{rules}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WeakRules, "weak-rules", modes, 
	"enable/disable, show/hide weak rules")

int
WeakRules::main(State& s, const string_list& a) {
switch (a.size()) {
case 1: {
	cout << "weak-rules ";
	if (s.weak_rules_enabled()) {
		cout << "on";
	} else {
		cout << "off";
	}
	if (!s.weak_rules_shown()) {
		cout << ", hidden";
	}
	cout << endl;
	return Command::NORMAL;
}
case 2: {
	const string& arg(a.back());
	if (arg == "on") {
		s.enable_weak_rules();
	} else if (arg == "off") {
		s.disable_weak_rules();
	} else if (arg == "show") {
		s.show_weak_rules();
	} else if (arg == "hide") {
		s.hide_weak_rules();
	} else {
		cerr << "Bad argument." << endl;
		usage(cerr);
		return Command::BADARG;
	}
	return Command::NORMAL;
}
default:
	usage(cerr << "usage: ");
	return Command::BADARG;
}
}

void
WeakRules::usage(ostream& o) {
	o << "weak-rules [on|off|show|hide]" << endl;
	o <<
"Simulate or ignore weak-rules, which can drive nodes, but are overpowered "
"by normal rules.  \'show\' and \'hide\' control whether or not weak rules "
"are displayed in command queries." << endl;
}
#endif	// PRSIM_WEAK_RULES

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/timing.texi
@deffn Command timing [mode] ...
Modes:
@samp{uniform} @var{delay} applies the same delay to @emph{all} rules.  
@t{uniform} is useful for getting quick transition counts.  
@samp{random} gives every event a different randomly assigned delay.  
@t{random} is most useful for detecting non-QDI logic violations.  
@samp{after} applies a different delay for each rule, as determined
by the @t{after} PRS rule attribute.  

The @t{after_min} and @t{after_max} rule attributes only have any effect
in random mode or on nodes marked @t{always_random}.  
In random-mode, @t{after_min} specifies a lower bound on delay, 
and @t{after_max} specifies an upper bound on delay.  
When no upper bound is specified, the delay distribution is an 
exponential variate; when an upper bound is specified, a delay 
is generated with uniform distribution between the bounds.  
If only a lower bound is specified, its value is added to the 
exponentially distribtued random delay.  

In the future, we may consider distributions that favor the bounds, 
(e.g. 50% chance of min or max) to stress the limits of the specified delays
in testing and timing closure.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Timing, "timing", modes, 
	"set/get timing mode")

int
Timing::main(State& s, const string_list& a) {
if (a.size() == 1) {
	s.dump_timing(cout);
	return Command::NORMAL;
} else {
	string_list b(a);
	b.pop_front();
	const string m(b.front());
	b.pop_front();
	if (s.set_timing(m, b)) {
		usage(cerr << "usage: ");
		return Command::BADARG;
	}
	return Command::NORMAL;
}
}

void
Timing::usage(ostream& o) {
	o << "timing [mode [args]]" << endl;
	o << "if no mode is given, just reports the current mode." << endl;
	State::help_timing(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/seed48.texi
@deffn Command seed48 [int int int]
Corresponds to libc's seed48 function.  
With no argument, print the current values of the internal random number seed.
With three (unsigned short) integers, sets the random number seed.
Note: the seed is automatically saved and restored in checkpoints.  
@end deffn
@end texinfo
***/
typedef	Seed48<State>			Seed48;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Seed48, PRSIM::modes)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/checkexcl.texi
@deffn Command checkexcl
@deffnx Command nocheckexcl
Enables mutual exclusion checking for checked exclusive node rings.  
Checking is enabled by default.  
Users of old @command{prsim} should replace uses of @t{CHECK_CHANNELS}
with these commands.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(CheckExcl, "checkexcl", modes, 
	"enable mutual exclusion checks")

int
CheckExcl::main(State& s, const string_list& a) {
if (a.size() > 1) {
	usage(cerr << "usage: ");
	return Command::BADARG;
} else {
	s.check_excl();
	return Command::NORMAL;
}
}

void
CheckExcl::usage(ostream& o) {
	o << "checkexcl" << endl;
	o << "Enables run-time mutual exclusion checks.  (default:on)"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoCheckExcl, "nocheckexcl", modes, 
	"disable mutual exclusion checks")

int
NoCheckExcl::main(State& s, const string_list& a) {
if (a.size() > 1) {
	usage(cerr << "usage: ");
	return Command::BADARG;
} else {
	s.nocheck_excl();
	return Command::NORMAL;
}
}

void
NoCheckExcl::usage(ostream& o) {
	o << "nocheckexcl" << endl;
	o << "Disables run-time mutual exclusion checks." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unstable-unknown.texi
@deffn Command unstable-unknown
When set, this causes unstable rules to be result in an unknown value
on the output node.  
The opposite effect is the @command{unstable-dequeue} command.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnstableUnknown, "unstable-unknown", 
	modes, "rule instabilities propagate unknowns (default)")

int
UnstableUnknown::main(State& s, const string_list& a) {
if (a.size() > 1) {
	usage(cerr << "usage: ");
	return Command::BADARG;
} else {
	s.dequeue_unstable_events(false);
	return Command::NORMAL;
}
}

void
UnstableUnknown::usage(ostream& o) {
	o << name << endl;
	o << "Unstable events propagate X\'s during run-time." << endl;
	o << "See also \'unstable-dequeue\'." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unstable-dequeue.texi
@deffn Command unstable-dequeue
When set, this causes unstable rules to be dequeued from the event queue.
The opposite effect is the @command{unstable-unknown} command.  
This option also allows events that drive a node to 'X' in the queue
to be @emph{overtaken} and replaced with known values if the fanin pull
of the node is resolved to a non-interfering direction @emph{before}
the 'X' event on the node is dequeued.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnstableDequeue, "unstable-dequeue", 
	modes, "rule instabilities dequeue events")

int
UnstableDequeue::main(State& s, const string_list& a) {
if (a.size() > 1) {
	usage(cerr << "usage: ");
	return Command::BADARG;
} else {
	s.dequeue_unstable_events(true);
	return Command::NORMAL;
}
}

void
UnstableDequeue::usage(ostream& o) {
	o << name << endl;
	o << "Unstable events are dequeued during run-time." << endl;
	o << "See also \'unstable-unknown\'." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
static const char
break_options[] = "[ignore|warn|notify|break]";

static const char
break_descriptions[] = 
"\tignore: silently ignores violation\n"
"\twarn: print warning without halting\n"
"\tnotify: (same as warn)\n"
"\tbreak: notify and halt";

#define	DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(class_name, command_key, \
		brief_str, usage_str, func_name) 			\
DECLARE_AND_INITIALIZE_COMMAND_CLASS(class_name, command_key, 		\
		modes, brief_str)					\
									\
int									\
class_name::main(State& s, const string_list& a) {			\
if (a.size() != 2) {							\
	usage(cerr << "usage: ");					\
	cerr << "current mode: " <<					\
		error_policy_string(s.get_##func_name##_policy()) \
		<< endl;						\
	return Command::SYNTAX;						\
} else {								\
	const string& m(a.back());					\
	const error_policy_enum e =	 				\
		string_to_error_policy(m);				\
	if (valid_error_policy(e)) {					\
		s.set_##func_name##_policy(e);				\
		return Command::NORMAL;					\
	} else {							\
		cerr << "bad mode: \'" << m << "\'." << endl;		\
		usage(cerr << "usage: ");				\
		return Command::BADARG;					\
	}								\
}									\
}									\
									\
void									\
class_name::usage(ostream& o) {						\
	o << name << " " << break_options << endl;			\
	o << usage_str << endl;						\
	o << break_descriptions << endl;				\
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/diagnostics.texi
@b{Diagnostic controls.}
The following commands control the simulation policy for run-time
logic violations.
Allowed arguments are: @t{ignore}, @t{warn}, @t{notify}, and @t{break}.
@t{ignore} silently ignores violations.
@t{notify} is the same as @t{warn}, which prints a diagnostic message
without interrupting the simulation.
@t{break} reports an error and stops the simulation.    
When no argument is given, just reports the current policy.  
@end texinfo
***/
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unstable.texi
@deffn Command unstable [mode]
Set the simulator policy in the event of an instability.
A rule is unstable when it is enqueued to fire, 
but a change in the input literal/expression stops the rule from firing.  
Stability is a requirement of quasi-delay insensitive circuits.  
Default mode is @t{break}.
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(Unstable, "unstable", 
	"alter simulation behavior on unstable", 
	"Alters simulator behavior on an instability violation.",
	unstable)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/weak-unstable.texi
@deffn Command weak-unstable [mode]
Set the simulator policy in the event of a weak-instability.
A rule is weakly-unstable when it is enqueued to fire, 
but a change in the input literal (to unknown) @emph{may} 
stop the rule from firing.  
Default mode is @t{warn}.
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(WeakUnstable, "weak-unstable", 
	"alter simulation behavior on weak-unstable",
	"Alters simulator behavior on a weak-instability violation.", 
	weak_unstable)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/interference.texi
@deffn Command interference [mode]
Set the simulator policy in the event of interference.
A rule is interfering when it is fighting an opposing (up/down) firing rule.  
Interference will always put the conflicting node into an unknown state.  
Non-interference is a requirement of quasi-delay insensitive circuits.  
Default mode is @t{break}.
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(Interference, "interference", 
	"alter simulation behavior on interference",
	"Alters simulator behavior on an interference violation.",
	interference)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/weak-interference.texi
@deffn Command weak-interference [mode]
Set the simulator policy in the event of weak-interference.
A rule is weakly interfering when @emph{may} fight 
an opposing (up/down) firing rule.  
Weak-interference will put the conflicting node into an unknown state.  
Default mode is @t{warn}.
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(WeakInterference, "weak-interference", 
	"alter simulation behavior on weak-interference",
	"Alters simulator behavior on a weak-interference violation.",
	weak_interference)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_INVARIANT_RULES
/***
@texinfo cmd/invariant-fail.texi
@deffn Command invariant-fail [mode]
Set the error-handling policy for certain invariant violations, 
when an invariant expression evaluates to @t{false}.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(InvariantFail, "invariant-fail", 
	"set error-handling policy on invariant failures",
	"Set error-handling policy on invariant violations.",
	invariant_fail)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/invariant-unknown.texi
@deffn Command invariant-unknown [mode]
Set the error-handling policy for possible invariant violations, i.e.
when an invariant expression evaluates to @t{X}.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(InvariantUnknown, "invariant-unknown", 
	"set error-handling of possible invariant failures",
	"Set error-handling policy on possible invariant violations.",
	invariant_unknown)
#endif	// PRSIM_INVARIANT_RULES

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/assert-fail.texi
@deffn Command assert-fail [mode]
Set the error-handling policy for when the @command{assert} command fails.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(AssertFail, "assert-fail", 
	"set error-handling of assert command failures",
	"Set error-handling policy on assert command failures.",
	assert_fail)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-expect-fail.texi
@deffn Command channel-expect-fail [mode]
Set the error-handling policy for when the a channel encounters
a value different from was expected.
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(ChannelExpectFail, 
	"channel-expect-fail", 
	"set error-handling of channel-expect failures",
	"Set error-handling policy on channel-expect failures.",
	channel_expect_fail)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/checkexcl-fail.texi
@deffn Command checkexcl-fail [mode]
Set the error-handling policy for when an exclusion check fails.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(CheckExclFail, 
	"checkexcl-fail", 
	"set error-handling of exclusion failures",
	"Set error-handling policy on exclusion failures.",
	excl_check_fail)

#undef	DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_INDIRECT_EXPRESSION_MAP
/***
@texinfo cmd/rules.texi
@deffn Command rules proc
@deffnx Command rules-verbose proc
Print all rules belonging to the named process @var{proc}.  
'@t{.}' can be used to refer to the top-level process.
The @t{-verbose} variant prints the state of each node and expression
appearing in each rule.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Rules, "rules", info, 
	"print rules belonging to a process")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RulesVerbose, "rules-verbose", info, 
	"print rules belonging to a process with values")

/***
@texinfo cmd/allrules.texi
@deffn Command allrules
@deffnx Command allrules-verbose
Print @emph{all} rules in the simulator, similar to @command{hflat}.  
The @t{-verbose} variant prints the state of each node and expression
appearing in each rule.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllRules, "allrules", info, 
	"print ALL rules being simulated")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllRulesVerbose, "allrules-verbose", info, 
	"print ALL rules being simulated with values")

static
int
rules_main(const State& s, const string_list& a, const bool verbose, 
		void (usage)(ostream&)) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const process_index_type pid =
		parse_process_to_index(a.back(), s.get_module());
	if (pid < s.get_num_processes()) {
		if (pid) {
			parse_name_to_what(cout, a.back(), s.get_module());
		} else {
			cout << "[top-level]" << endl;
		}
		cout << "rules:" << endl;
		s.dump_rules(cout, pid, verbose);
		return Command::NORMAL;
	} else {
		cerr << "Error: process not found." << endl;
		return Command::BADARG;
	}
}
}

static
int
all_rules_main(const State& s, const string_list& a, const bool verbose, 
		void (usage)(ostream&)) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_all_rules(cout, verbose);
	return Command::NORMAL;
}
}

static
void
rules_usage(ostream& o) {
	o << "rules[-verbose] process-name\n"
"Prints all rules that belong to the named process (\".\" is the top-level).\n"
"The -verbose variant prints values of each node and expression in the guards."
	<< endl;
}

static
void
all_rules_usage(ostream& o) {
	o << "allrules[-verbose]\n"
"Prints all rules in the simuation, like the \'hflat\' tool.\n"
"The -verbose variant prints values of each node and expression in the guards."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
Rules::main(State& s, const string_list& a) {
	return rules_main(s, a, false, usage);
}

void
Rules::usage(ostream& o) { rules_usage(o); }

int
RulesVerbose::main(State& s, const string_list& a) {
	return rules_main(s, a, true, usage);
}

void
RulesVerbose::usage(ostream& o) { rules_usage(o); }

int
AllRules::main(State& s, const string_list& a) {
	return all_rules_main(s, a, false, usage);
}

void
AllRules::usage(ostream& o) { all_rules_usage(o); }

int
AllRulesVerbose::main(State& s, const string_list& a) {
	return all_rules_main(s, a, true, usage);
}

void
AllRulesVerbose::usage(ostream& o) { all_rules_usage(o); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_INVARIANT_RULES
/***
@texinfo cmd/invariants.texi
@deffn Command invariants proc
@deffnx Command invariants-verbose proc
Print all invariants belonging to the named process @var{proc}.  
'@t{.}' can be used to refer to the top-level process.
The @t{-verbose} variant prints the state of each node and expression
appearing in each invariant.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Invariants, "invariants", info, 
	"print invariants belonging to a process")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(InvariantsVerbose, 
	"invariants-verbose", info, 
	"print invariants belonging to a process with values")

/***
@texinfo cmd/allinvariants.texi
@deffn Command allinvariants
@deffnx Command allinvariants-verbose
Print @emph{all} invariants in the simulator, similar to @command{hflat}.  
The @t{-verbose} variant prints the state of each node and expression
appearing in each invariant.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllInvariants, "allinvariants", info, 
	"print ALL invariants being simulated")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllInvariantsVerbose, 
	"allinvariants-verbose", info, 
	"print ALL invariants being simulated with values")

static
int
invariants_main(const State& s, const string_list& a, const bool verbose, 
		void (usage)(ostream&)) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const process_index_type pid =
		parse_process_to_index(a.back(), s.get_module());
	if (pid < s.get_num_processes()) {
		if (pid) {
			parse_name_to_what(cout, a.back(), s.get_module());
		} else {
			cout << "[top-level]" << endl;
		}
		cout << "invariants:" << endl;
		s.dump_invariants(cout, pid, verbose);
		return Command::NORMAL;
	} else {
		cerr << "Error: process not found." << endl;
		return Command::BADARG;
	}
}
}

static
int
all_invariants_main(const State& s, const string_list& a, const bool verbose, 
		void (usage)(ostream&)) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_all_invariants(cout, verbose);
	return Command::NORMAL;
}
}

static
void
invariants_usage(ostream& o) {
	o << "invariants[-verbose] process-name\n"
"Prints all invariants that belong to the named process (\".\" is the top-level).\n"
"The -verbose variant prints values of each node and expression."
	<< endl;
}

static
void
all_invariants_usage(ostream& o) {
	o << "allinvariants[-verbose]\n"
"Prints all invariants in the simuation, like the \'hflat\' tool.\n"
"The -verbose variant prints values of each node and expression."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
Invariants::main(State& s, const string_list& a) {
	return invariants_main(s, a, false, usage);
}

void
Invariants::usage(ostream& o) { invariants_usage(o); }

int
InvariantsVerbose::main(State& s, const string_list& a) {
	return invariants_main(s, a, true, usage);
}

void
InvariantsVerbose::usage(ostream& o) { invariants_usage(o); }

int
AllInvariants::main(State& s, const string_list& a) {
	return all_invariants_main(s, a, false, usage);
}

void
AllInvariants::usage(ostream& o) { all_invariants_usage(o); }

int
AllInvariantsVerbose::main(State& s, const string_list& a) {
	return all_invariants_main(s, a, true, usage);
}

void
AllInvariantsVerbose::usage(ostream& o) { all_invariants_usage(o); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/***
@texinfo cmd/check-invariants.texi
@deffn Command check-invariants
Checks every invariant expression in the design.  
Returns true if there were any certain violations, 
excluding possible violations of invariants.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(CheckInvariants, "check-invariants",
	info, "check all invariant expressions now")

int
CheckInvariants::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.check_all_invariants(cout)) {
		return Command::BADARG;	// appropriate?
	} else {
		return Command::NORMAL;
	}
}
}

void
CheckInvariants::usage(ostream& o) {
	o << name <<
" : checks all invariants in the design and reports all\n"
"and possible violations." << endl;
}
#endif	// PRSIM_INVARIANT_RULES
#endif	// PRSIM_INDIRECT_EXPRESSION_MAP

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/mode.texi
@deffn Command mode [md]
Without arguments, reports the current simulation policies on logic violations.
With argument @var{md}, @t{run} is the default set of policies, 
@t{reset} is only different in that @t{weak-unstable} is @t{ignore}d.  
@t{reset} is useful during the initalization phase, when some rules
may transiently and weakly interfere, as they come out of unknown state.  
@t{paranoid} causes the simulation to break on weak-instabilities
and weak-interferences, which is useful for debugging.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(SetMode, "mode", 
	modes, "set weak-unstable/interference policies (presets)")

int
SetMode::main(State& s, const string_list& a) {
if (a.size() == 1) {
	s.dump_mode(cout);
} else if (a.size() == 2) {
	static const string reset("reset");
	static const string run("run");
	static const string paranoid("paranoid");
	const string& m(a.back());
	if (m == reset) {
		s.set_mode_reset();
	} else if (m == run) {
		s.set_mode_run();
	} else if (m == paranoid) {
		s.set_mode_breakall();
	} else {
		usage(cerr << "usage: ");
		return Command::BADARG;
	}
} else {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
}
	return Command::NORMAL;
}

void
SetMode::usage(ostream& o) {
	o << "mode [reset|run|paranoid]\n"
"\t\'reset\' disables weak-interference warnings, useful during initialization\n"
"\t\'run\' (default) enables weak-interference warnings\n"
"\t\'paranoid\' also break on weak-instability and weak-interference" << endl;
	o <<
"Instabilities and interferences still cause simulations to halt, while \n"
"weak-instabilities trigger warnings." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/memstats.texi
@deffn Command memstats
Show memory usage breakdown of the simulator.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(MemStats, "memstats", 
	debug, "show memory usage statistics")

int
MemStats::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_memory_usage(cout);
	return Command::NORMAL;
}
}

void
MemStats::usage(ostream& o) {
	o << "memstats" << endl;
	o << "show memory usage details of simulator state" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TODO: texinfo documentation! oh, and implement these.
// TODO: support PxeMx1ofN (array source, auto-expand and decouple?)
// TODO: typedef-channel command

/***
@texinfo cmd/channel.texi
@deffn Command channel name type bundle rails
Registers a named channel (with constituents) in a separate namespace in
the simulator, typically used to drive or log the environment.
The @var{name} of the channel should match that of an instance 
(process or channel) in the source file.  
@itemize
@item @var{name} is the name of the new channel in the simulator's namespace
@item @var{type} is a regular expression of the form @t{[ae][nv]?:[01]}, where
@itemize
	@item @t{a} means active-high acknowledge
	@item @t{e} means active-low acknowledge (a.k.a. enable)
	@item @t{n} means active-low validity (a.k.a. neutrality)
	@item @t{v} means active-high validity.
	These are also the names of the channel signals.
	@item @t{[01]} is the initial value of the acknowledge during reset, 
		which is only relevant to channel sinks.  
@end itemize
@item @var{bundle} is the name of the data bundle (rail group) of the channel
	in the form @t{[name]:size}, where @var{size} is the number of 
	rail bundles (M in Mx1ofN).
	If there are no bundles, then leave the name blank,
		i.e. just write @t{:0}
	If there is only one bundle (1x1ofN), use @var{size} 0 to 
		indicate that named bundle is not an array.
@item @var{rails} (@t{rname:radix}) is the name and size of each bundle's 
	data rails, @var{rname} is the name of the data rail of the channel.
	@var{radix} is the number of data rails per bundle (N in Mx1ofN).
	Use @var{radix} 0 to indicate that rail is not an array (1of1).
@end itemize
For example, @t{channel e:0 :0 d:4} is a conventional e1of4 channel with
data rails @t{d[0..3]}, and an active-low acknowledge reset to 0, no bundles.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Channel, "channel", 
	channels, "declare a handshake channel from a group of nodes")

int
Channel::main(State& s, const string_list& a) {
if (a.size() != 5) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	const string& chan_name(*i);
	const string& ev_type(*++i);
	const string& bundle(*++i);
	const string& rail(*++i);
	// could confirm that 'name' exists as a process/channel/datatype?
#if PRSIM_ACKLESS_CHANNELS
	bool have_ack;
#endif
	bool ack_sense;
	bool ack_init;
	bool have_valid = false;
	bool valid_sense = false;
#if PRSIM_CHANNEL_RAILS_INVERTED
	bool data_sense = true;
#endif
	try {
		// parse ev-type
		const size_t evl = ev_type.length();
		if (evl < 3 || evl > 4) { THROW_EXIT; }
		switch (tolower(ev_type[0])) {
		case 'a': ack_sense = true; break;
		case 'e': ack_sense = false; break;
		default: THROW_EXIT;
		}
		size_t c = ev_type.find(':');
		if (c == string::npos) { THROW_EXIT; }
		if (evl == 4) {
			have_valid = true;
			switch (tolower(ev_type[c-1])) {
			case 'n': valid_sense = false; break;
			case 'v': valid_sense = true; break;
			default: THROW_EXIT;
			}
		}
		switch (*--ev_type.end()) {
		case '0': ack_init = false; break;
		case '1': ack_init = true; break;
		default: THROW_EXIT;
		}
	} catch (...) {
		cerr << "Error: invalid channel ev-type argument." << endl;
		cerr << "See \"help " << name << "\"." << endl;
		return Command::BADARG;
	}
	string bundle_name;	// default blank
	size_t bundle_size = 0;
	try {
		// parse bundle
		size_t c = bundle.find(':');
		if (c == string::npos || (c == bundle.length() -1)) {
			THROW_EXIT;
		}
		const string::const_iterator b(bundle.begin());
		bundle_name.assign(b, b+c);
		string v(b +c +1, bundle.end());
		if (string_to_num(v, bundle_size)) { THROW_EXIT; }
	} catch (...) {
		cerr << "Error: invalid channel bundle argument." << endl;
		cerr << "See \"help " << name << "\"." << endl;
		return Command::BADARG;
	}
	string rail_name;
	size_t rail_size = 0;
	try {
		// parse rail
		size_t c = rail.find(':');
		if (c == string::npos || (c == rail.length() -1)) {
			THROW_EXIT;
		}
#if PRSIM_CHANNEL_RAILS_INVERTED
		// TODO: finish me
#endif
		const string::const_iterator b(rail.begin());
		rail_name.assign(b, b+c);
		if (!rail_name.length()) { THROW_EXIT; }
		string v(b +c +1, rail.end());
		if (string_to_num(v, rail_size)) { THROW_EXIT; }
	} catch (...) {
		cerr << "Error: invalid channel rail argument." << endl;
		cerr << "See \"help " << name << "\"." << endl;
		return Command::BADARG;
	}
	channel_manager& cm(s.get_channel_manager());
	if (cm.new_channel(s, chan_name, bundle_name, bundle_size, 
			rail_name, rail_size) ||
			cm.set_channel_ack_valid(s, chan_name, ack_sense, 
				ack_init, have_valid, valid_sense)) {
		return Command::BADARG;
	}
	return Command::NORMAL;
}
}	// end Channel::main

void
Channel::usage(ostream& o) {
	o << name << " <name> <type:init> <bundle:size> <rail:radix>" << endl;
	o <<
"Registers a named channel (with constituents) in a separate namespace in \n"
"the simulator, typically used to drive or log the environment.\n"
"\'name\' is the name of the new channel in the simulator's namespace\n"
"\'type\' is a regular expression of the form [ae][nv]?, where \n"
	"\t\'a\' means active-high acknowledge\n"
	"\t\'e\' means active-low acknowledge (a.k.a. enable)\n"
	"\t\'n\' means active-low validity (a.k.a. neutrality)\n"
	"\t\'v\' means active-high validity.\n"
	"\tThese are also the names of the channel signals.\n"
"\'init\' is [01], the initial value of the acknowledge during reset\n"
"\'bundle\' is the name of the data bundle (rail group) of the channel.\n"
"\'size\' is the number of rail bundles (M in Mx1ofN)\n"
	"\tnote: if there are no bundles, then leave the name blank,\n"
		"\t\ti.e. just write \":0\"\n"
	"\tUse size 0 to indicate that bundle name is not an array.\n"
"\'rail\' is the name of the data rail of the channel.\n"
"\'radix\' is the number of data rails per bundle (N in Mx1ofN).\n"
	"\tUse radix 0 to indicate that rails are not an array (1of1).\n"
"For example, \"channel e:0 :0 d:4\", is a conventional e1of4 channel with\n"
"data rails d[0..3], and an active-low acknowledge reset to 0, no bundles."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// for finesse
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AutoChannel, "auto-channel", 
	channels, "register a channel based on internal type")
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-show.texi
@deffn Command channel-show chan
Print the current configuration and state of channel @var{chan}.
This also shows the sequence of values associated with sources and 
expectations with sequence position, if applicable.  
Looping values are indicated with @t{*}.  
This also shows the origin of the value sequence and the 
name of the current log file to which values are dumped, if enabled.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelShow, "channel-show", 
	channels, "show configuration of registered channel")

int
ChannelShow::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.dump_channel(cout, a.back())) {
		return Command::BADARG;
	}
	return Command::NORMAL;
}
}

void
ChannelShow::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-show-all.texi
@deffn Command channel-show-all
Print the current configuration for all registered channels.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelShowAll, "channel-show-all", 
	channels, "list configuration of all registered channels")

int
ChannelShowAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_channels(cout);
	return Command::NORMAL;
}
}

void
ChannelShowAll::usage(ostream& o) {
	o << name << endl;
o << "Print list of all registered channels with their type information."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// change loop configuration
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelLoop, "channel-loop", 
	channels, "cycle through source/expect values")

DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelLoop, "channel-unloop", 
	channels, "stop sourcing/expecting at end of values")
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-watch.texi
@deffn Command channel-watch chan
Report value of data rails when channel @var{chan} has valid data.  
Data validity is only determined by the state of the data rails, 
and not the acknowledge signal. 
An unstable channel (that can transiently take valid states)
will report @emph{every} transient value.
Channels in the stopped state will still be reported, 
make sure that they are resumed by @command{channel-release}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelWatch, "channel-watch", 
	channels, "report when specified channel changes state")

int
ChannelWatch::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().watch_channel(a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelWatch::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-unwatch.texi
@deffn Command channel-unwatch chan
Remove channel @var{chan} from watch list.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelUnWatch, "channel-unwatch", 
	channels, "ignore when spcified channel changes state")

int
ChannelUnWatch::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().unwatch_channel(a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelUnWatch::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-watchall.texi
@deffn Command channel-watchall
Report values on all channels when data rails become valid.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelWatchAll, "channel-watchall", 
	channels, "report when any channel changes state")

int
ChannelWatchAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.get_channel_manager().watch_all_channels();
	return Command::NORMAL;
}
}

void
ChannelWatchAll::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-unwatchall.texi
@deffn Command channel-unwatchall
Silence value-reporting on all channels.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelUnWatchAll, "channel-unwatchall", 
	channels, "ignore when any channel changes state")

int
ChannelUnWatchAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.get_channel_manager().unwatch_all_channels();
	return Command::NORMAL;
}
}

void
ChannelUnWatchAll::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-heed.texi
@deffn Command channel-heed chan
Take channel @var{chan} out of the ignored state.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelHeed, "channel-heed", 
	channels, "resume logging and checking channel values")

int
ChannelHeed::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().heed_channel(a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelHeed::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-ignore.texi
@deffn Command channel-ignore chan
Stop logging and checking expected values on channel @var{chan}. 
This can be useful for momentarily ignoring a sequence of values.  
An ignored channel will continue to respond to changes until it
is stopped, by a @command{stop} command.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelIgnore, "channel-ignore", 
	channels, "stop logging and checking channel values")

int
ChannelIgnore::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().ignore_channel(a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelIgnore::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-heed-all.texi
@deffn Command channel-heed-all
Continue logging and checking all channel values.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelHeedAll, "channel-heed-all", 
	channels, "resume logging and checking channel values")

int
ChannelHeedAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.get_channel_manager().heed_all_channels();
	return Command::NORMAL;
}
}

void
ChannelHeedAll::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-ignore-all.texi
@deffn Command channel-ignore-all
Stop logging and checking all channel values.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelIgnoreAll, "channel-ignore-all", 
	channels, "stop checking and logging all channel values")

int
ChannelIgnoreAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.get_channel_manager().ignore_all_channels();
	return Command::NORMAL;
}
}

void
ChannelIgnoreAll::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-reset.texi
@deffn Command channel-reset chan
Force a environment-configured channel into its reset state, i.e. 
a source will reset its data rails to neutral (ignoring state of acknowledge),
and a sink will set the acknowledge to the initial value (from configuration)
regardless of the data rails (and validity).  
@strong{IMPORTANT}: This command also freezes a channel in the stopped state, 
like @command{channel-stop} and will not respond to signal changes until 
resumed by @command{channel-release}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelReset, "channel-reset", 
	channels, "set a channel into its reset state")

int
ChannelReset::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.reset_channel(a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelReset::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << "Hold a source/sink channel in its reset state." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-reset-all.texi
@deffn Command channel-reset-all
Force all source- or sink- configured channels into their reset state, 
as done by @command{channel-reset}.
This is typically done at the same time as global reset initalization.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelResetAll, "channel-reset-all", 
	channels, "set all registered channel into reset state")

int
ChannelResetAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.reset_all_channels();
	return Command::NORMAL;
}
}

void
ChannelResetAll::usage(ostream& o) {
	o << name << endl;
	o << "Hold all registered source/sink channels into reset state."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-stop.texi
@deffn Command channel-stop chan
Freeze a source- or sink-configured channel so that it stops responding 
to signal transitions from the circuit.  
Stopped channels will not log data nor assert expected values
because they may be in a transient state.  
A channel can be unfrozen by @command{channel-release}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelStop, "channel-stop", 
	channels, "hold a channel in its current state")

int
ChannelStop::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().stop_channel(a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelStop::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << "Prevent a source/sink channel from operating (pause)." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-stop-all.texi
@deffn Command channel-stop-all
Applies @command{channel-stop} to all channels.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelStopAll,
	"channel-stop-all", channels,
	"hold all registered channels in current state")

int
ChannelStopAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.get_channel_manager().stop_all_channels();
	return Command::NORMAL;
}
}

void
ChannelStopAll::usage(ostream& o) {
	o << name << endl;
	o << "Prevent all source/sink channels from operating." << endl;
}

// could call these Resume...
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-release.texi
@deffn Command channel-release chan
Releases a source- or sink-configured channel from the stopped state, 
so that it begins to respond to circuit signal transitions
(and continue logging and expecting).  
Upon resuming, the channel evaluates its inputs and adds events
to the event queue as deemed appropriate.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelRelease, "channel-release", 
	channels, "release a channel from its reset or stopped state")

int
ChannelRelease::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.resume_channel(a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelRelease::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << "Release a source/sink channel from paused state." << endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-release-all.texi
@deffn Command channel-release-all
Applies @command{channel-release} to all channels.  
This is typically used at the end of a reset initialization sequence
as the circuit is brought out of the reset state.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelReleaseAll,
	"channel-release-all", channels,
	"channel-release all registered channels")

int
ChannelReleaseAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.resume_all_channels();
	return Command::NORMAL;
}
}

void
ChannelReleaseAll::usage(ostream& o) {
	o << name << endl;
	o << "Release all source/sink channels from reset/stopped state."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-close.texi
@deffn Command channel-close chan
Close any output file streams associated with channel @var{chan}.
This flush the current log file, closes the file, and stops logging.  
This does not affect source nor expect value sequences since those
files are read in their entirety upon configuration.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelClose, "channel-close", 
	channels, "close any files/streams associated with channel")

int
ChannelClose::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().close_channel(a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelClose::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << "Close streams associated with channel." << endl;
	o << "Note: this does not affect sources and expects." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-close-all.texi
@deffn Command channel-close-all
Apply @command{channel-close} to all channels.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelCloseAll, "channel-close-all", 
	channels, "close files/streams associated with any channel")

int
ChannelCloseAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.get_channel_manager().close_all_channels();
	return Command::NORMAL;
}
}

void
ChannelCloseAll::usage(ostream& o) {
	o << name << endl;
	o << "Close all streams associated with channels." << endl;
	o << "Note: this does not affect sources and expects." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-source.texi
@deffn Command channel-source-file chan file
@deffnx Command channel-source chan file
Configure channel @var{chan} to source values from the environment.
Values are take from @var{file} and read into an internal array.
Once values are exhausted, the channel stops sourcing.  
To repeat values, use @command{channel-source-file-loop}.  
A channel configured as a source should have the production rules
drive the acknowledge signal and no other rules driving the data rails
(otherwise the simulator will issue a warning).  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceFile, "channel-source-file", 
	channels, "source values on channel from file (once)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSource, "channel-source", 
	channels, "alias to channel-source (deprecated)")

int
ChannelSourceFile::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().source_channel_file(s, 
			*++a.begin(), a.back(), false))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelSourceFile::usage(ostream& o) {
	o << name << " <channel> <file>" << endl;
	// TODO optional start argument for offset
	o << "Source channel values from file.  \n"
		"Once values are exhausted, channel stops sourcing." << endl;
}

int
ChannelSource::main(State& s, const string_list& a) {
	return ChannelSourceFile::main(s, a);
}

void
ChannelSource::usage(ostream& o) {
	ChannelSourceFile::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-source-loop.texi
@deffn Command channel-source-file-loop chan file
@deffnx Command channel-source-loop chan file
Like @command{channel-source-file} except that value sequence is repeated
infintely.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceFileLoop, 
	"channel-source-file-loop", 
	channels, "source values on channel from file (loop)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceLoop, "channel-source-loop", 
	channels, "alias to channel-source-loop (deprecated)")

int
ChannelSourceFileLoop::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().source_channel_file(s, 
			*++a.begin(), a.back(), true))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelSourceFileLoop::usage(ostream& o) {
	o << name << " <channel> <file>" << endl;
	// TODO optional start argument for offset
	o << "Source channel values from file infinitely.  \n"
		"Once values are exhausted, sequence restarts." << endl;
}

int
ChannelSourceLoop::main(State& s, const string_list& a) {
	return ChannelSourceFileLoop::main(s, a);
}

void
ChannelSourceLoop::usage(ostream& o) {
	ChannelSourceFileLoop::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-source-args.texi
@deffn Command channel-source-args chan [values...]
Source values on channel @var{chan} using the @var{values} passed
on the command.  
Sourcing stops after last value is used. 
Legal values are integers and 'X' for random.  
If no values are given, then the channel will not souce any values, 
but it will still reset the data rails to neutral state.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceArgs,
	"channel-source-args", 
	channels, "source values on channel from arguments (once)")

int
ChannelSourceArgs::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list v;
	copy(++++a.begin(), a.end(), back_inserter(v));
	if (s.get_channel_manager().source_channel_args(s, 
			*++a.begin(), v, false))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelSourceArgs::usage(ostream& o) {
	o << name << " <channel> <values...>" << endl;
	o <<
"Sources a channel with finite sequence of values passed in arguments.\n"
"Argument values may be integer or \'X\' (don\'t care or random)."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-source-args-loop.texi
@deffn Command channel-source-args-loop chan [values...]
Source values on channel @var{chan} using the @var{values} passed
on the command.  
Value sequence is repeated infinitely.  
Legal values are integers and 'X' for random.  
If no values are given, then the channel will not souce any values, 
but it will still reset the data rails to neutral state.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceArgsLoop,
	"channel-source-args-loop", 
	channels, "source channel values from arguments (loop)")

int
ChannelSourceArgsLoop::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list v;
	copy(++++a.begin(), a.end(), back_inserter(v));
	if (s.get_channel_manager().source_channel_args(s, 
			*++a.begin(), v, true))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelSourceArgsLoop::usage(ostream& o) {
	o << name << " <channel> <values...>" << endl;
	o <<
"Sources a channel with repeating sequence of values passed in arguments.\n"
"Argument values may be integer or \'X\' (don\'t care or random)."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-rsource.texi
@deffn Command channel-rsource chan
Configures a channel to source random data values.  
This is useful for tests that do not depend on data values.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelRandomSource, "channel-rsource", 
	channels, "source random values on channel (infinite)")

int
ChannelRandomSource::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().rsource_channel(s, a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelRandomSource::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << "Source channel using random values." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-sink.texi
@deffn Command channel-sink chan
Configure a channel to consume all data values (infinitely).  
A sink-configured channel should have data rails driven by the production rules
and nothing else driving the acknowledge signal 
(simulator will issue warning otherwise).
A sink-configured channel can also log and expect values.  
Mmmmm... tokens!  Nom-nom-nom...
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSink, "channel-sink", 
	channels, "consume tokens infinitely on channel")

int
ChannelSink::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().sink_channel(s, a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelSink::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << "Sink channel tokens infinitely." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-log.texi
@deffn Command channel-log chan file
Record all valid data values on channel @var{chan} to output @var{file}.
File stream automatically closes upon end of simulation, 
or with an explicit @command{channel-close}.  
Channels in the stopped state will NOT be reported, 
make sure that they are resumed by @command{channel-release}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelLog, "channel-log", 
	channels, "log channel values to file")

int
ChannelLog::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.get_channel_manager().log_channel(*++a.begin(), a.back()))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelLog::usage(ostream& o) {
	o << name << " <channel> <file>" << endl;
	o << "Record channel values to file (non-append)." << endl;
	o << "Logging only passively observes the state of channel data, "
		"without controlling any handshake signals.  " << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-expect.texi
@deffn Command channel-expect-file chan file
@deffnx Command channel-expect chan file
Compare data values seen on channel @var{chan} against a sequence of
values from @var{file}.
Error out as soon as there is a value mismatch.  
In this variant, once value sequence is exhausted, 
no more comparisons are done, and channel values go unchecked.  
See also @command{channel-expect-file-loop}.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpectFile, "channel-expect-file", 
	channels, "assert values on channel from file (once)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpect, "channel-expect", 
	channels, "alias to channel-expect-file (deprecated)")

int
ChannelExpectFile::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& chan_name(*++a.begin());
	channel_manager& cm(s.get_channel_manager());
	if (cm.expect_channel_file(chan_name, a.back(), false))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelExpectFile::usage(ostream& o) {
	// TODO optional start argument for offset
	o << name << " <channel> <file>" << endl;
	o <<
"Assert that values observed on channel match expected values from file.\n"
"Expecting only passively observes the state of channel data, "
	"without controlling any handshake signals." << endl;
}

int
ChannelExpect::main(State& s, const string_list& a) {
	return ChannelExpectFile::main(s, a);
}

void
ChannelExpect::usage(ostream& o) {
	ChannelExpectFile::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-expect-loop.texi
@deffn Command channel-expect-file-loop chan file
@deffnx Command channel-expect-loop chan file
Like @command{channel-expect-file} but repeats value sequence infintely.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpectFileLoop,
	"channel-expect-file-loop", 
	channels, "assert values on channel from file (loop)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpectLoop, "channel-expect-loop", 
	channels, "alias to channel-expect-loop (deprecated)")

int
ChannelExpectFileLoop::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& chan_name(*++a.begin());
	channel_manager& cm(s.get_channel_manager());
	if (cm.expect_channel_file(chan_name, a.back(), true))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelExpectFileLoop::usage(ostream& o) {
	o << name << " <channel> <file>" << endl;
	o <<
"Assert that values observed on channel match expected values from file.\n"
"Expecting only passively observes the state of channel data, "
	"without controlling any handshake signals." << endl;
}

int
ChannelExpectLoop::main(State& s, const string_list& a) {
	return ChannelExpectFileLoop::main(s, a);
}

void
ChannelExpectLoop::usage(ostream& o) {
	ChannelExpectFileLoop::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-expect-args.texi
@deffn Command channel-expect-args chan values...
Tells a channel @var{chan} to expect @var{values} on data rails.
Stops checking values after last value is used.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpectArgs,
	"channel-expect-args", 
	channels, "assert values on channel from arguments (once)")

int
ChannelExpectArgs::main(State& s, const string_list& a) {
if (a.size() < 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& chan_name(*++a.begin());
	string_list v;
	copy(++++a.begin(), a.end(), back_inserter(v));
	channel_manager& cm(s.get_channel_manager());
	if (cm.expect_channel_args(chan_name, v, false))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelExpectArgs::usage(ostream& o) {
	o << name << " <channel> <values...>" << endl;
	o << 
"Assert that values seen on channel match those listed in argument values.\n"
"Stops checking after last value is used."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-expect-args-loop.texi
@deffn Command channel-expect-args-loop chan values...
Tells a channel @var{chan} to expect @var{values} on data rails.
Checks that value sequence repeats infintely.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpectArgsLoop,
	"channel-expect-args-loop", 
	channels, "assert channel values from arguments (loop)")

int
ChannelExpectArgsLoop::main(State& s, const string_list& a) {
if (a.size() < 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& chan_name(*++a.begin());
	string_list v;
	copy(++++a.begin(), a.end(), back_inserter(v));
	channel_manager& cm(s.get_channel_manager());
	if (cm.expect_channel_args(chan_name, v, true))
		return Command::BADARG;
	return Command::NORMAL;
}
}

void
ChannelExpectArgsLoop::usage(ostream& o) {
	o << name << " <channel> <values...>" << endl;
	o << 
"Assert that values seen on channel match those listed in argument values.\n"
"Checks that value sequence repeats infintely."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-assert-value-queue.texi
@deffn Command channel-assert-value-queue chan val
For channels that are sourcing or expecting values, 
assert the state of the channel value array being used.  
@var{val} is 1 to assert that values are still remaining, 
0 to assert that values are empty (channel is finished).
Looped sources and expects will never be empty.  
This is useful for checking that finite sequence tests
have actually completed.  
Exits fatally if assertion fails.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelAssertValueQueue,
	"channel-assert-value-queue", channels, 
	"Assert whether channel is out of source/expect values")

int
ChannelAssertValueQueue::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	const string& cn(*i);
	channel* const c = s.get_channel_manager().lookup(cn);
	if (c) {
		++i;
		const string& v(*i);
		int x;
		if (string_to_num(v, x)) {
			cerr << "Error: expecting 0 or 1 to assert." << endl;
			usage(cerr);
			return Command::BADARG;
		}
		if (c->have_value()) {
			if (!x) {
				cerr << "Expecting no more channel values on "
					<< cn << ", but found some." << endl;
				return Command::FATAL;
			} else if (s.confirm_asserts()) {
				cout << "channel " << cn <<
					" has no more values, as expected."
					<< endl;
			}
		} else {
			if (x) {
				cerr << "Expecting more channel values on "
					<< cn << ", but found none." << endl;
				return Command::FATAL;
			} else if (s.confirm_asserts()) {
				cout << "channel " << cn <<
					" has more values, as expected."
					<< endl;
			}
		}
		return Command::NORMAL;
	} else	return Command::BADARG;
}
}

void
ChannelAssertValueQueue::usage(ostream& o) {
o << name << " channel [01]\n";
o << "Checks the value status of a source/expecting channel.\n";
o << "If argument is 1, assert that channel still has values remaining.\n";
o << "If argument is 0, assert that channel still has no values remaining."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_TIMING
/***
@texinfo cmd/channel-timing.texi
@deffn Command channel-timing chan [mode [args]]
With no additional arguments, report the timing mode of channel @var{chan}.
Timing only applies to channels that are configured as a source or a sink.
Modes:
@itemize
@item @t{global} : use the global simulation-wide timing policy.
@item @t{after [delay]} : use a fixed delay.
@item @t{random [max]} : if @var{max} is specified, 
	use a uniform distribution delay bounded by @var{max}, 
	otherwise return an exponential variate delay.  
@end itemize
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelTiming, "channel-timing", 
	channels, "set/get per channel timing mode")

int
ChannelTiming::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else if (a.size() == 2) {
	const string& cn(*++a.begin());
	channel* const c = s.get_channel_manager().lookup(cn);
	if (c) {
		c->dump_timing(cout << "channel " << cn << ": ") << endl;
		return Command::NORMAL;
	} else	return Command::BADARG;	// have error message
} else {
	string_list b(a);
	b.pop_front();
	const string cn(b.front());
	b.pop_front();
	const string m(b.front());
	b.pop_front();
	channel* const c = s.get_channel_manager().lookup(cn);
	if (c && c->set_timing(m, b)) {
		usage(cerr << "usage: ");
		return Command::BADARG;
	}
	return Command::NORMAL;
}
}

void
ChannelTiming::usage(ostream& o) {
	o << name << " channel [mode [args]]" << endl;
	o << "if no mode is given, just reports the current mode." << endl;
	channel::help_timing(o);
}
#endif	// PRSIM_CHANNEL_TIMING

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelGet, "channel-get", 
	channels, "show current state of channel")

DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelAssert, "channel-assert", 
	channels, "assert value on channel immediately")

#endif

//=============================================================================
#undef	DECLARE_AND_INITIALIZE_COMMAND_CLASS
//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

