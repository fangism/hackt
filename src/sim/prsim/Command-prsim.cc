/**
	\file "sim/prsim/Command-prsim.cc"
	All built-in commands should be statically registered 
	in this translation unit to guarantee proper static object
	initialization ordering; the command_map must be constructed
	before entries are registered.  

	TODO: consider using some form of auto-indent
		in the help-system.  

	$Id: Command-prsim.cc,v 1.4.2.4 2008/01/22 23:05:18 fang Exp $

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
	channel("channel", "channel commands"),
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
***/
typedef	stateless_command_wrapper<Exit, State>		Exit;
typedef	stateless_command_wrapper<Quit, State>		Quit;

INITIALIZE_STATELESS_COMMAND_CLASS(PRSIM::Exit,
	"exit", PRSIM::builtin, "exits simulator")
INITIALIZE_STATELESS_COMMAND_CLASS(PRSIM::Quit,
	"quit", PRSIM::builtin, "exits simulator")

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
/**
	Command class for stepping through one event at a time from
	the event queue. 
@texinfo cmd/step.texi
@deffn Command step [n]
Advances the simulation by @var{n} steps (events).  
Without @var{n}, takes only a single step.  
@end deffn
@end texinfo
 */
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
	"step through event")

/**
	Like process_step() from original prsim.  
 */
int
Step::main(State& s, const string_list& a) {
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
	} catch (const State::excl_exception& exex) {
		s.inspect_excl_exception(exex, cerr);
		return Command::FATAL;
	}	// no other exceptions
	return Command::NORMAL;
}
}	// end Step::main()

void
Step::usage(ostream& o) {
	o << "step [#steps]" << endl;
	o <<
"Advances the simulation by a number of steps (default: 1).\n"
"Simulation will stop prematurely if any event violations are encountered."
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
	TODO: handle breakpoint.
	TODO: implement no/globaltime policy for resetting.  
	Isn't the while-loop made redundant by State::cycle()?
 */
int
Cycle::main(State& s, const string_list & a) {
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
	} catch (const State::excl_exception& exex) {
		s.inspect_excl_exception(exex, cerr);
		return Command::FATAL;
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
	const char val = node_type::string_to_value(_val);
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
	const char val = node_type::string_to_value(_val);
	if (!node_type::is_valid_value(val)) {
		cerr << "Invalid logic value: " << _val << endl;
		return Command::SYNTAX;
	}
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		const int err = s.set_node_after(ni, val,
			State::random_delay(), force);
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
Print all equivalent aliases of instance @var{name}.  
@end deffn
@end texinfo
***/
typedef	Who<State>				Who;
CATEGORIZE_COMMON_COMMAND_CLASS(PRSIM::Who, PRSIM::info)

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
Print all nodes whose current value is @var{value}.  
Frequently used to find nodes that are in an unknown ('X') state.  
Valid @var{value} arguments are [0fF] for logic-low, [1tT] for logic-high,
[xXuU] for unknown value.  
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
	const char v = node_type::string_to_value(a.back());
	if (node_type::is_valid_value(v)) {
		s.status_nodes(cout, v);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unknown-inputs.texi
@deffn Command unknown-inputs
Print all nodes with value X that have no fanins, i.e. input-only nodes.  
Great for debugging forgotten environment inputs and connections!
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
	s.dump_dangling_unknown_nodes(cout);
	return Command::NORMAL;
}
}

void
UnknownInputs::usage(ostream& o) {
	o << name << " -- list all nodes at value X with no fanin." << endl;
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
		const char val = node_type::string_to_value(_val);
		if (!node_type::is_valid_value(val)) {
			cerr << "Invalid logic value: " << _val << endl;
			return Command::SYNTAX;
		}
		const char actual = n.current_value();
		if (actual != val) {
			cout << "assert failed: expecting node `" << objname <<
				"\' at " <<
				node_type::value_to_char[size_t(val)] <<
				", but got ";
			n.dump_value(cout) << "." << endl;
			return Command::FATAL;
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
		const char val = node_type::string_to_value(_val);
		if (!node_type::is_valid_value(val)) {
			cerr << "Invalid logic value: " << _val << endl;
			return Command::SYNTAX;
		}
		const char actual = n.current_value();
		if (actual == val) {
			cout << "assert failed: expecting node `" << objname <<
				"\' not at " <<
				node_type::value_to_char[size_t(val)] <<
				", but got ";
			n.dump_value(cout) << "." << endl;
			return Command::FATAL;
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
@texinfo cmd/assert-queue.texi
@deffn Command assert-queue
Error out if event queue is empty.  
Useful for checking for deadlock.  
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
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(Confirm, "confirm", info, 
//	"confirm assertions verbosely")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoConfirm, "noconfirm", info, 
//	"confirm assertions silently")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
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

#endif	// PRSIM_SEPARATE_CAUSE_NODE_DIRECTION

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
	o << "tcounts" << endl;
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
	o << "notcounts" << endl;
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
@deffn Command weak-rules [on|off]
Simulation mode switch which globally enables or disables (ignores)
weak-rules.  
Weak-rules can only take effect when normal rules pulling a node are off.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WeakRules, "weak-rules", modes, 
	"enable/disable weak rules")

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
	cout << endl;
	return Command::NORMAL;
}
case 2: {
	const string& arg(a.back());
	if (arg == "on") {
		s.enable_weak_rules();
	} else if (arg == "off") {
		s.disable_weak_rules();
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
	o << "weak-rules [on|off]" << endl;
	o <<
"Simulate or ignore weak-rules, which can drive nodes, but are overpowered "
"by normal rules." << endl;
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
		State::error_policy_string(s.get_##func_name##_policy()) \
		<< endl;						\
	return Command::SYNTAX;						\
} else {								\
	const string& m(a.back());					\
	State::error_policy_enum e = State::string_to_error_policy(m);	\
	if (State::valid_error_policy(e)) {				\
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

#undef	DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/mode.texi
@deffn Command mode [md]
Without arguments, reports the current simulation policies on logic violations.
With argument @var{md}, @t{run} is the default set of policies, 
@t{reset} is only different in that @t{weak-unstable} is @t{ignore}d.  
@t{reset} is useful during the initalization phase, when some rules
may transiently and weakly interfere, as they come out of unknown state.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(SetMode, "mode", 
	modes, "enable/disable weak-interference warnings")

int
SetMode::main(State& s, const string_list& a) {
if (a.size() == 1) {
	s.dump_mode(cout);
} else if (a.size() == 2) {
	static const string reset("reset");
	static const string run("run");
	const string& m(a.back());
	if (m == reset) {
		s.set_mode_reset();
	} else if (m == run) {
		s.set_mode_run();
	} else {
		usage(cerr << "usage: ");
		return Command::BADARG;
	}
} else {
	usage(cerr << "usage: ");
	return Command::BADARG;
}
	return Command::NORMAL;
}

void
SetMode::usage(ostream& o) {
	o << "mode [reset|run]\n"
"\t\'reset\' disables weak-interference warnings, useful during initialization\n"
"\t\'run\' (default) enables weak-interference warnings" << endl;
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

//=============================================================================
#undef	DECLARE_AND_INITIALIZE_COMMAND_CLASS
//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

