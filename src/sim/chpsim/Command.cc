/**
	\file "sim/chpsim/Command.cc"
	All built-in commands should be statically registered 
	in this translation unit to guarantee proper static object
	initialization ordering; the command_map must be constructed
	before entries are registered.  

	TODO: consider using some form of auto-indent
		in the help-system.  

	$Id: Command.cc,v 1.13.8.1 2008/01/17 01:32:01 fang Exp $
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <set>

#include "sim/chpsim/Command.h"
#include "sim/chpsim/State.h"
#include "sim/command_base.tcc"
#include "sim/command_builtin.tcc"
#include "sim/command_category.tcc"
#include "sim/command_registry.tcc"
#include "sim/command_common.tcc"
#include "parser/instref.h"
#include "sim/chpsim/Trace.h"

#include "common/TODO.h"
#include "util/libc.h"
#include "util/attributes.h"
#include "util/memory/excl_malloc_ptr.h"

namespace HAC {
namespace SIM {

// must instantiate dependent class first!
template class command_registry<CHPSIM::Command>;

namespace CHPSIM {
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
using entity::global_indexed_reference;
using parser::parse_global_reference;
using parser::parse_name_to_what;
using parser::parse_name_to_aliases;
using parser::parse_name_to_get_subnodes;

//=============================================================================
// local static CommandCategories
// feel free to add categories here

static CommandCategory
	builtin("builtin", "built-in commands"),
	general("general", "general commands"),
	simulation("simulation", "simulation commands"),
//	channel("channel", "channel commands"),
	info("info", "information about simulated circuit"),
	view("view", "instance to watch"),
	tracing("tracing", "trace and checkpoint commands"), 
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
	DECLARE_CHPSIM_COMMAND_CLASS(_class)				\
	INITIALIZE_COMMAND_CLASS(_class, _cmd, _category, _brief)

//-----------------------------------------------------------------------------
/***
@texinfo cmd/echo.texi
@deffn Command echo output
Print @var{output} to stdout.  
@b{Note:} multiple spaces in @var{output} are compacted into 
single spaces by the interpreter's tokenizer.  
@end deffn
@end texinfo
***/
typedef	stateless_command_wrapper<Echo, State>	Echo;
INITIALIZE_STATELESS_COMMAND_CLASS(CHPSIM::Echo, "echo", CHPSIM::builtin, 
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
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Help, CHPSIM::builtin)
}	// end namespace CHPSIM

// explicit instantiation in correct namespace
template int CHPSIM::Help::main(const string_list&);

// re-open namespace
namespace CHPSIM {
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

INITIALIZE_STATELESS_COMMAND_CLASS(CHPSIM::CommentPound,
	"#", CHPSIM::builtin, "comments are ignored")
INITIALIZE_STATELESS_COMMAND_CLASS(CHPSIM::CommentComment,
	"comment", CHPSIM::builtin, "comments are ignored")


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	All<State>					All;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::All, CHPSIM::builtin)

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

INITIALIZE_STATELESS_COMMAND_CLASS(CHPSIM::Exit,
	"exit", CHPSIM::builtin, "exits simulator")
INITIALIZE_STATELESS_COMMAND_CLASS(CHPSIM::Quit,
	"quit", CHPSIM::builtin, "exits simulator")

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
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Alias, CHPSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unalias.texi
@deffn Command unalias cmd
Undefines an existing alias @var{cmd}.
@end deffn
@end texinfo
***/
typedef	UnAlias<State>				UnAlias;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::UnAlias, CHPSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unaliasall.texi
@deffn Command unaliasall
Undefines @emph{all} aliases.  
@end deffn
@end texinfo
***/
typedef	UnAliasAll<State>			UnAliasAll;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::UnAliasAll, CHPSIM::builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/aliases.texi
@deffn Command aliases
Print a list of all known aliases registered with the interpreter.
@end deffn
@end texinfo
***/
typedef	Aliases<State>				Aliases;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Aliases, CHPSIM::builtin)

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
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::EchoCommands, CHPSIM::builtin)

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
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Source, CHPSIM::general)

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
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::AddPath, CHPSIM::general)

/***
@texinfo cmd/paths.texi
@deffn Command paths
Print the list of paths searched for source scripts.  
@end deffn
@end texinfo
***/
typedef	Paths<State>				Paths;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Paths, CHPSIM::general)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/dladdpath.texi
@deffn Command dladdpath paths ...
@anchor{command-dladdpath}
Append @var{paths} to the list of paths to search for 
opening shared library modules.  
This is useful if you simply forget (or are too lazy) to 
pass the corresponding paths on the command-line.  
See also @ref{option-L,, the @option{-L} option}.
@end deffn
@end texinfo
***/
typedef	DLAddPath<State>			DLAddPath;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::DLAddPath, CHPSIM::general)

/***
@texinfo cmd/dlpaths.texi
@deffn Command dlpaths
Prints the list of paths used in searching for dlopen-ing modules.  
@end deffn
@end texinfo
***/
typedef	DLPaths<State>				DLPaths;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::DLPaths, CHPSIM::general)

/***
@texinfo cmd/dlopen.texi
@cindex dlopen
@deffn Command dlopen lib
@anchor{command-dlopen}
Open shared library @var{lib} for loading external user-defined functions.  
Library is found by searching through user-specified load paths and 
the conventional library path environment variables.  
The command-line equivalent is the @ref{option-l,, @option{-l} option},
following the same naming guidelines.
@end deffn
@end texinfo
***/
typedef	DLOpen<State>				DLOpen;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::DLOpen, CHPSIM::general)

/***
@texinfo cmd/dlcheckfunc.texi
@deffn Command dlcheckfunc funcs ...
@anchor{command-dlcheckfunc}
For each function named in @var{funcs}, report whether or not
it has been bound to a symbol in a dynamically loaded module.
Never errors out.  
@xref{command-dlassertfunc,, command @command{dlassertfunc}}.
@end deffn
@end texinfo
***/
typedef	DLCheckFunc<State>			DLCheckFunc;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::DLCheckFunc, CHPSIM::general)

/***
@texinfo cmd/dlassertfunc.texi
@deffn Command dlassertfunc funcs ...
@anchor{command-dlassertfunc}
Error out if any function named in @var{funcs} is unbound 
to a module symbol.
Useful for making sure a set of symbols is resolved before 
any execution begins.  
@xref{command-dlcheckfunc,, command @command{dlcheckfunc}}.
@end deffn
@end texinfo
***/
typedef	DLAssertFunc<State>			DLAssertFunc;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::DLAssertFunc, CHPSIM::general)

/***
@texinfo cmd/dlfuncs.texi
@deffn Command dlfuncs
Print list of registered functions, from dlopened modules.
@end deffn
@end texinfo
***/
typedef	DLFuncs<State>				DLFuncs;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::DLFuncs, CHPSIM::general)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/initialize.texi
@deffn Command initialize
Resets the variable state of the simulation, while preserving
other settings such as mode and breakpoints.  
@end deffn
@end texinfo
***/
typedef	Initialize<State>			Initialize;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Initialize, CHPSIM::simulation)

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
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Reset, CHPSIM::simulation)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Command class for stepping through one event at a time from
	the event queue. 

@texinfo cmd/step.texi
@deffn Command step n
Advances the simulation by @var{n} steps.  
@end deffn
@end texinfo
 */
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

INITIALIZE_COMMAND_CLASS(Step, "step", simulation,
	"step through single event")

/**
	Command to advance one event in simulation.  
	TODO: return value support
	TODO: breakpoint, watchpoint support
 */
int
Step::main(State& s, const string_list& a) {
if (a.size() > 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::time_type		time_type;
	size_t i;		// the number of events to advance
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
	s.resume();
	// time_type time = s.time();	// unused
	// could check s.pending_events()
try {
	while (s.pending_events() && !s.stopped() && i) {
		const State::step_return_type brk = s.step();
		// time = s.time();	// unused
		if (brk) {
			// already printed diagnostics in step()
			cout << "\t*** break, " << i << " steps left." << endl;
			break;
		}
		--i;
	}	// end while
	return Command::NORMAL;
} catch (...) {
	cerr << "Caught run-time exception during execution.  Halting." << endl;
	return Command::FATAL;
}
}	// end else
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
	time_type add;		// time to add
	if (string_to_num(a.back(), add)) {
		cerr << "Error parsing time." << endl;
		return Command::BADARG;
	} else if (add < 0) {
		cerr << "Error: time must be non-negative." << endl;
		return Command::BADARG;
	}
	const time_type stop_time = s.time() +add;
	s.resume();
	try {
	while (s.pending_events() && !s.stopped() &&
			(s.next_event_time() < stop_time)) {
		const State::step_return_type brk = s.step();
		if (brk) {
			// already printed diagnostics in step()
			cout << "\t*** break, " << stop_time -s.time() <<
				" time left." << endl;
			break;
		}
	}	// end while
	} catch (...) {
		cerr << "Caught run-time exception during execution.  Halting."
			<< endl;
		return Command::FATAL;
	}	// no other exceptions
#if 0
	if (!s.stopped() && s.time() < stop_time) {
		s.update_time(stop_time);
	}
#endif
	// else leave the time at the time as of the last event
	return Command::NORMAL;
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
@texinfo cmd/advance-to.texi
@deffn Command advance-to t
Advances the simulation @emph{until} time @var{t}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AdvanceTo, "advance-to", simulation,
	"advance the simulation to time")

/** TODO: advance simulation time to value specified, 
 * would need to modify private variable in State.cc
 * Currently just advances as far as time of last event before time
 */

// copy-rip from Advance

int
AdvanceTo::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	State::time_type		time_type;
	time_type add;		// time to add
	if (string_to_num(a.back(), add)) {
		cerr << "Error parsing time." << endl;
		return Command::BADARG;
	} else if (add < 0) {
		cerr << "Error: time must be non-negative." << endl;
		return Command::BADARG;
	}
	const time_type stop_time = add;
	s.resume();
	try {
	while (s.pending_events() && !s.stopped() &&
			(s.next_event_time() <= stop_time)) {
		const State::step_return_type brk = s.step();
		if (brk) {
			// already printed diagnostics in step()
			cout << "\t*** break, " << stop_time -s.time() <<
				" time left." << endl;
			break;
		}
	}	// end while
	} catch (...) {
		cerr << "Caught run-time exception during execution.  Halting."
			<< endl;
		return Command::FATAL;
	}	// no other exceptions
#if 0
	if (!s.stopped() && s.time() < stop_time) {
		s.update_time(stop_time);
	}
#endif
	// else leave the time at the time as of the last event
	return Command::NORMAL;
}
}	// end AdvanceTo::main()

void
AdvanceTo::usage(ostream& o) {
	o << "advance-to <time>" << endl;
	o <<
"Advances the simulation until events up to <time> are executed.\n"
"Simulation will stop prematurely if any event violations are encountered."
	<< endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/run.texi
@deffn Command run
Runs the simulation until the event queue is empty, if ever.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Run, "run", simulation,
 	"run until event queue empty or breakpoint")

/**
	TODO: add cause tracing.
	TODO: handle breakpoint.
	TODO: implement no/globaltime policy for resetting.  
	Isn't the while-loop made redundant by State::cycle()?
 */
int
Run::main(State& s, const string_list & a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
//	typedef	State::node_type		node_type;
	typedef	State::time_type		time_type;
//	time_type time = s.time();		// unused
	s.resume();	// clear STOP flag
	try {
	while (s.pending_events() && !s.stopped()) {
		const State::step_return_type brk = s.step();
		// time = s.time();		// unused
		if (brk) {
			// already printed diagnostics in step()
			cout << "\t*** break, at time " <<
				s.time() << "." << endl;
			break;
		}
	}	// end while (!s.stopped())
	} catch (...) {
		cerr << "Caught run-time exception during execution.  Halting."
			<< endl;
		return Command::FATAL;
	}	// no other exceptions
	return Command::NORMAL;
}	// end if
}	// end Run::main()

void
Run::usage(ostream& o) {
	o << name << endl;
	o << "Runs until event queue is empty or breakpoint hit.\n"
"Simulation will stop prematurely if any event anomalies are encountered."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/queue.texi
@cindex event queue
@deffn Command queue
Print an ordered list of all events in the checking event queue and 
execution event queue.  
@end deffn
@end texinfo
***/
typedef	Queue<State>				Queue;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Queue, CHPSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(Setrwhen, "setrwhen", simulation,
//	"set node with random delay after event")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/break-event.texi
@deffn Command break-event event-id
Stop the simulation when event @var{event-id} executes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(BreakEvent, "break-event", simulation,
	"set breakpoint on event")

int
BreakEvent::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& eid(*i);
		event_index_type t;
		if (string_to_num(eid, t)) {
			cerr << "Error: non-numeric argument \"" 
				<< eid << "\"." << endl;
			badarg = true;
		} else {
			s.break_event(t);
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
BreakEvent::usage(ostream& o) {
	o << name << " <event-id ...>" << endl;
	o << "causes simulation to stop upon execution of listed events"<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unbreak-event.texi
@deffn Command unbreak-event event-id
Remove breakpoint on event @var{event-id}.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnBreakEvent, "unbreak-event", simulation,
	"remove breakpoint and watchpoint on event")

int
UnBreakEvent::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& eid(*i);
		event_index_type t;
		if (string_to_num(eid, t)) {
			cerr << "Error: non-numeric argument \"" 
				<< eid << "\"." << endl;
			badarg = true;
		} else {
			s.unbreak_event(t);
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
UnBreakEvent::usage(ostream& o) {
	o << name << " <event-id ...>" << endl;
	o << "removes events from breakpoint and watchpoint lists"<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unbreakall-events.texi
@deffn Command unbreakall-events
Removes all event breakpoints.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnBreakAllEvents, 
	"unbreakall-events", simulation, 
	"remove all breakpoint events")

int
UnBreakAllEvents::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.unbreak_all_events();
	return Command::NORMAL;
}
}

void
UnBreakAllEvents::usage(ostream& o) {
	o << name << endl;
	o << "Clears all event breakpoints." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/show-event-breaks.texi
@deffn Command show-event-breaks
List all event breakpoints.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ShowEventBreaks, 
	"show-event-breaks", simulation,
	"list all events that are breakpoints")

int
ShowEventBreaks::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_break_events(cout);
	return Command::NORMAL;
}
}

void
ShowEventBreaks::usage(ostream& o) {
	o << name << endl;
	o << "Lists all breakpoint events." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/break-value.texi
@deffn Command break-value inst
Stop the simulation when variable @var{inst} is written, 
event when its value does not change.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(BreakValue, "break-value", simulation, 
	"set breakpoint on selected variables")

/**
	Adds variables to the watch list.  
 */
int
BreakValue::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& objname(*i);
		const global_indexed_reference
			r(parse_global_reference(objname, s.get_module()));
		if (r.first) {
			s.break_value(r);
		} else {
			cerr << "No such instance found: " << objname << endl;
			badarg = true;
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
BreakValue::usage(ostream& o) {
	o << name << " <var-names ...>" << endl;
	o << "sets breakpoints on variable(s)." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unbreak-value.texi
@deffn Command unbreak-value inst
Remove breakpoint on variable @var{inst}.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnBreakValue, 
	"unbreak-value", simulation, 
	"remove breakpoint on selected variables")

/**
	Removes a variable from the breakpoint list.  
 */
int
UnBreakValue::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& objname(*i);
		const global_indexed_reference
			r(parse_global_reference(objname, s.get_module()));
		if (r.first) {
			s.unbreak_value(r);
		} else {
			cerr << "No such instance found: " << objname << endl;
			badarg = true;
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
UnBreakValue::usage(ostream& o) {
	o << name << " <nodes>" << endl;
	o << "removes non-breakpoint variables from watch-list" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unbreakall-values.texi
@deffn Command unbreakall-values
Removes all variable breakpoints.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnBreakAllValues,
	"unbreakall-values", simulation, 
	"clear all variable breakpoints")

int
UnBreakAllValues::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.unbreak_all_values();
	return Command::NORMAL;
}
}

void
UnBreakAllValues::usage(ostream& o) {
	o << name << endl;
	o << "removes all breakpoint variables." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/show-value-breaks.texi
@deffn Command show-value-breaks
List all variable breakpoints.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ShowValueBreaks,
	"show-value-breaks", simulation, 
	"list breakpoint variables with values")

int
ShowValueBreaks::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_break_values(cout);
	return Command::NORMAL;
}
}

void
ShowValueBreaks::usage(ostream& o) {
	o << name << endl;
	o << "Print all breakpoint variables with current values." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/save.texi
@deffn Command save ckpt
Saves the current simulator state to a checkpoint file @var{ckpt}
that can be restored later.  
Overwrites @var{ckpt} if it already exists.  
@end deffn
@end texinfo
***/
typedef	Save<State>				Save;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Save, CHPSIM::tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/load.texi
@deffn Command load ckpt
Restores the simulator state (variables and events) from a checkpoint
file @var{ckpt}.  
@end deffn
@end texinfo
***/
typedef	Load<State>				Load;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Load, CHPSIM::tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/dump-state.texi
@deffn Command dump-state
Print textual summary of entire state of simulation.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DumpState, "dump-state", info,
	"print entire state of the simulation")

int
DumpState::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_state(cout);
	return Command::NORMAL;
}
}

void
DumpState::usage(ostream& o) {
	o << name << endl;
	o <<
"Prints all the stateful information of variables, channels, and events that\n"
"would be recorded and restored by a checkpoint." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/what.texi
@deffn Command what inst
Prints the type of the named instance @var{inst}, 
along with its canonical name.  
@end deffn
@end texinfo
***/
typedef	What<State>				What;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::What, CHPSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/who.texi
@cindex aliases, instance
@deffn Command who inst
Print all equivalent aliases of the instance named @var{inst}.  
@end deffn
@end texinfo
***/
typedef	Who<State>				Who;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Who, CHPSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/get.texi
@cindex variable state
@deffn Command get inst
Print the state information about instance named @var{inst}.
The name @var{inst} need not be canonical.  
Information includes current run-time value, if applicable.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Get, "get", info,
	"print value of instance")

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
	const global_indexed_reference
		gr(parse_global_reference(objname, s.get_module()));
	if (gr.first) {
		cout << objname << " : ";
		s.print_instance_name_value(cout, gr) << endl;
		return Command::NORMAL;
	} else {
		cerr << "No such instance found." << endl;
		return Command::BADARG;
	}
}
}

void
Get::usage(ostream& o) {
	o << name << " <name>" << endl;
	o << "Print the current value of the instance." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/print-event-header.texi
@deffn Command print-event-header
Prints a table header suitable for interpreting printed event records.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(PrintEventHeader, 
	"print-event-header", info,
	"print the event table header")

int
PrintEventHeader::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	State::dump_event_table_header(cout);
	return Command::NORMAL;
}
}

void
PrintEventHeader::usage(ostream& o) {
	o << name << endl;
	o << "Prints out the event table header, for diagnostics." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/subscribers.texi
@deffn Command subscribers inst
@cindex subscribers
Print a list of all events currently subscribed to the value of 
variable @var{inst}.  Such events are alerted for rechecking
when value of @var{inst} changes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Subscribers, "subscribers", info,
	"list events subscribed to the named instance")

/**
	Prints current value of the named node.  
 */
int
Subscribers::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const global_indexed_reference
		gr(parse_global_reference(objname, s.get_module()));
	if (gr.first) {
		cout << "events subscribed to `" << objname << "\' : ";
		s.print_instance_name_subscribers(cout, gr);
		return Command::NORMAL;
	} else {
		cerr << "No such instance found." << endl;
		return Command::BADARG;
	}
}
}

void
Subscribers::usage(ostream& o) {
	o << name << " <name>" << endl;
	o << "Lists all events currently subscribed to the instance." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/subscribers-all.texi
@deffn Command subscribers-all
Print a list of all events currently subscribed to any variables.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(SubscribersAll, "subscribers-all", info,
	"list all events subscribed sorted by instance")

/**
	Print all subscriber events.
 */
int
SubscribersAll::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	cout << "All event subscriptions:" << endl;
	s.print_all_subscriptions(cout);
	return Command::NORMAL;
}
}

void
SubscribersAll::usage(ostream& o) {
	o << name << endl;
	o << "List all instances with subscribed events." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/dump-event.texi
@deffn Command dump-event event-id
Print status information about event number @var{event-id}.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DumpEvent, "dump-event", info,
	"print event information")

int
DumpEvent::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	size_t i;
	if (string_to_num(a.back(), i)) {
		cerr << "Error parsing event index number." << endl;
		return Command::BADARG;
	}
	const size_t z = s.event_pool_size();
	if (i >= z) {
		cerr << "Error: index out-of-range, must be < " <<
			z << "." << endl;
		return Command::BADARG;
	}
	// dump structural information
	s.dump_event(cout, i);
	// status: is it subscribed to its dependencies?
	s.dump_event_status(cout, i);
	return Command::NORMAL;
}
}

void
DumpEvent::usage(ostream& o) {
	o << name << " <index>" << endl;
	o << "Dumps information about the referenced event." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/dump-event-source.texi
@deffn Command dump-event-source event-id
Print full-context of the source in which event @var{event-id} occurs.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DumpEventSource, "dump-event-source", info,
	"print event with full source context")

int
DumpEventSource::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	size_t i;
	if (string_to_num(a.back(), i)) {
		cerr << "Error parsing event index number." << endl;
		return Command::BADARG;
	}
	const size_t z = s.event_pool_size();
	if (i >= z) {
		cerr << "Error: index out-of-range, must be < " <<
			z << "." << endl;
		return Command::BADARG;
	}
	// dump structural information
	s.dump_event_source(cout, i);
	return Command::NORMAL;
}
}

void
DumpEventSource::usage(ostream& o) {
	o << name << " <index>" << endl;
	o << "Dumps source of referenced event with full-context." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/dump-all-event-source.texi
@deffn Command dump-all-event-source
Print full-context of the source for all events.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DumpAllEventSource, 
	"dump-all-event-source", info,
	"print all event with full source context")

int
DumpAllEventSource::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_all_event_source(cout);
	return Command::NORMAL;
}
}

void
DumpAllEventSource::usage(ostream& o) {
	o << name << endl;
	o << "Dumps source of all events with full-context." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// will category conflict with command?
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(Info, "info", info, 
//	"print information about a node/vector")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/assert-queue.texi
@deffn Command assert-queue
Error out if the event queue is empty.
Useful as a quick check for deadlock.
@end deffn
@end texinfo
***/
typedef	AssertQueue<State>			AssertQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::AssertQueue, CHPSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/assertn-queue.texi
@deffn Command assertn-queue
Error out if the event queue is not empty.
@end deffn
@end texinfo
***/
typedef	AssertNQueue<State>			AssertNQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::AssertNQueue, CHPSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/time.texi
@deffn Command time
Print the current simulator time.
@end deffn
@end texinfo
***/
typedef	Time<State>				Time;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::Time, CHPSIM::info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(Confirm, "confirm", info, 
//	"confirm assertions verbosely")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoConfirm, "noconfirm", info, 
//	"confirm assertions silently")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/watch-event.texi
@deffn Command watch-event event-id
Watchpoint.  
Print event @var{event-id} each time it executes, without interrupting.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WatchEvent, "watch-event", view, 
	"print activity on selected events")

int
WatchEvent::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& eid(*i);
		event_index_type t;
		if (string_to_num(eid, t)) {
			cerr << "Error: non-numeric argument \"" 
				<< eid << "\"." << endl;
			badarg = true;
		} else {
			s.watch_event(t);
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
WatchEvent::usage(ostream& o) {
	o << name << "<event-id ...>";
	o << 
"Added events to the watch list, which are printed upon execution." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unwatch-event.texi
@deffn Command unwatch-event event-id
Remove watchpoint on event @var{event-id}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnWatchEvent, "unwatch-event", view, 
	"silence activity on selected events")

int
UnWatchEvent::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& eid(*i);
		event_index_type t;
		if (string_to_num(eid, t)) {
			cerr << "Error: non-numeric argument \"" 
				<< eid << "\"." << endl;
			badarg = true;
		} else {
			s.unwatch_event(t);
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
UnWatchEvent::usage(ostream& o) {
	o << name << "<event-id ...>";
	o << 
"Removes events from the watch list, which are printed upon execution.\n"
"However, events listed as breakpoints are NOT removed." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnWatchAllEvents, 
	"unwatchall-events", view, 
	"remove all non-breakpoint events from watch list")

int
UnWatchAllEvents::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.unwatch_all_events();
	return Command::NORMAL;
}
}

void
UnWatchAllEvents::usage(ostream& o) {
	o << name << endl;
	o << "Clears the watch-list of all events EXCEPT breakpoints." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/watch-value.texi
@deffn Command watch-value inst
Print events that write to @var{inst} as they execute.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WatchValue, "watch-value", view, 
	"print activity on selected variables")

/**
	Adds variables to the watch list.  
 */
int
WatchValue::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& objname(*i);
		const global_indexed_reference
			r(parse_global_reference(objname, s.get_module()));
		if (r.first) {
			s.watch_value(r);
		} else {
			cerr << "No such instance found: " << objname << endl;
			badarg = true;
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
WatchValue::usage(ostream& o) {
	o << name << " <var-names ...>" << endl;
	o << "adds variable(s) to watch-list.\n"
"Watched variables print their value transitions to stdout." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unwatch-value.texi
@deffn Command unwatch-value inst
Stop watching @var{inst}. 
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnWatchValue, "unwatch-value", view, 
	"silence activity reporting on selected variables")

/**
	Removes a variable from the watch list.  
 */
int
UnWatchValue::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef string_list::const_iterator	const_iterator;
	const_iterator i(++a.begin()), e(a.end());
	bool badarg = false;
	for ( ; i!=e; ++i) {
		const string& objname(*i);
		const global_indexed_reference
			r(parse_global_reference(objname, s.get_module()));
		if (r.first) {
			s.unwatch_value(r);
		} else {
			cerr << "No such instance found: " << objname << endl;
			badarg = true;
		}
	}
	return badarg ? Command::BADARG : Command::NORMAL;
}
}

void
UnWatchValue::usage(ostream& o) {
	o << name << " <nodes>" << endl;
	o << "removes non-breakpoint variables from watch-list" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnWatchAllValues,
	"unwatchall-values", view, 
	"silence activity reporting on all variables")

int
UnWatchAllValues::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.unwatch_all_values();
	return Command::NORMAL;
}
}

void
UnWatchAllValues::usage(ostream& o) {
	o << name << endl;
	o << "removes all non-breakpoint variables from watch-list" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/show-value-watches.texi
@deffn Command show-event-values
Print list of all watched variables.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ShowValueWatches,
	"show-value-watches", view, 
	"list watched variables with values")

int
ShowValueWatches::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_watch_values(cout);
	return Command::NORMAL;
}
}

void
ShowValueWatches::usage(ostream& o) {
	o << name << endl;
	o << "Print all watched variables with current values." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/watchall-events.texi
@deffn Command watchall-events
Print all events as they execute, @emph{regardless of whether or not they 
are explicitly watched}.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WatchAllEvents, "watchall-events", view, 
	"print activity of all events")

/**
	Enables a watchall flag to report all transitions.  
 */
int
WatchAllEvents::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.watch_all_events();
	return Command::NORMAL;
}
}

void
WatchAllEvents::usage(ostream& o) {
	o << name << endl;
	o << "print all events to stdout." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/nowatchall-events.texi
@deffn Command nowatchall-events
Stop printing all events, but keep printing events that are explicitly
listed watchpoints.  
This is particularly useful for temporarily watching all events in detail, 
and later restoring only explicitly watched events.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoWatchAllEvents,
	"nowatchall-events", view, 
	"only print activity on explicitly watched events")

/**
	Disables global watchall, but still maintains explicitly
	watched nodes.  
 */
int
NoWatchAllEvents::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.nowatch_all_events();
	return Command::NORMAL;
}
}

void
NoWatchAllEvents::usage(ostream& o) {
	o << name << endl;
o << "turns off watchall-events, printing only explicitly watched events"
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/show-event-watches.texi
@deffn Command show-event-watches
Print list of all watched events.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ShowEventWatches, 
	"show-event-watches", view,
	"list all events that are explicitly watched")

int
ShowEventWatches::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_watch_events(cout);
	return Command::NORMAL;
}
}

void
ShowEventWatches::usage(ostream& o) {
	o << name << endl;
	o << "Show list of explicitly watched events." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TODO: FINISH ME
#if 0
/***
@texinfo cmd/watch-queue.texi
@deffn Command watch-queue
Print events on watched events and values as they enter the event queue 
(either for checking or execution).
This is generally recommended for debugging, 
as it prints @emph{a lot} of information.  
@end deffn
@end texinfo
***/
typedef	WatchQueue<State>			WatchQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::WatchQueue, CHPSIM::view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/nowatch-queue.texi
@deffn Command nowatch-queue
Disables @command{watch-queue}.
@end deffn
@end texinfo
***/
typedef	NoWatchQueue<State>			NoWatchQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::NoWatchQueue, CHPSIM::view)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/watchall-queue.texi
@deffn Command watchall-queue
Print events as they enter the event queue (either for checking or execution).
This is generally recommended for debugging, 
as it prints @emph{a lot} of information.  
@end deffn
@end texinfo
***/
typedef	WatchAllQueue<State>			WatchAllQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::WatchAllQueue, CHPSIM::view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/nowatchall-queue.texi
@deffn Command nowatchall-queue
Disables @command{watchall-queue}.
@end deffn
@end texinfo
***/
typedef	NoWatchAllQueue<State>			NoWatchAllQueue;
CATEGORIZE_COMMON_COMMAND_CLASS(CHPSIM::NoWatchAllQueue, CHPSIM::view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/null-event-delay.texi
@deffn Command null-event-delay [delay]
Without the @var{delay} argument, prints the value of the delay used
for ``trivial'' events.  
With the @var{delay} argument, sets the said delay value.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NullEventDelay, "null-event-delay", modes, 
	"set/get the delay of trivial events")

int
NullEventDelay::main(State& s, const string_list& a) {
if (a.size() == 1) {
	cout << "null-event-delay = " << s.get_null_event_delay() << endl;
	return Command::NORMAL;
} else if (a.size() == 2) {
	State::time_type t;
	if (string_to_num(a.back(), t)) {
		cerr << "Error: invalid delay argument, must be real-valued."
			<< endl;
		return Command::BADARG;
	}
	s.set_null_event_delay(t);
	// confirmation:
	cout << "null-event-delay = " << s.get_null_event_delay() << endl;
	return Command::NORMAL;
} else {
	usage(cerr);
	return Command::SYNTAX;
}
}

void
NullEventDelay::usage(ostream& o) {
	o << name << " [delay]" << endl;
	o <<
"If no delay is given, just reports the current null-event delay.\n"
"NOTE: null-delay only takes effect when the timing *mode* is uniform."
	<< endl;
//	State::help_timing(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/uniform-delay.texi
@deffn Command uniform-delay [delay]
The uniform delay value only takes effect in the @t{uniform} timing mode.
Without the @var{delay} argument, prints the value of the delay.  
With the @var{delay} argument, sets the said delay value.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UniformDelay, "uniform-delay", modes, 
	"set/get the uniform delay of events")

int
UniformDelay::main(State& s, const string_list& a) {
if (a.size() == 1) {
	cout << "uniform-delay = " << s.get_uniform_delay() << endl;
	return Command::NORMAL;
} else if (a.size() == 2) {
	State::time_type t;
	if (string_to_num(a.back(), t)) {
		cerr << "Error: invalid delay argument, must be real-valued."
			<< endl;
		return Command::BADARG;
	}
	s.set_uniform_delay(t);
	// confirmation:
	cout << "uniform-delay = " << s.get_uniform_delay() << endl;
	return Command::NORMAL;
} else {
	usage(cerr);
	return Command::SYNTAX;
}
}

void
UniformDelay::usage(ostream& o) {
	o << name << " [delay]" << endl;
	o <<
"If no delay is given, just reports the current uniform delay.\n"
"NOTE: uniform delay only takes effect when the timing *mode* is uniform."
	<< endl;
//	State::help_timing(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/timing.texi
@deffn Command timing mode
Select timing mode for event delays.  
@var{mode} can be one of the following:
@table @option
@item uniform
Use the same delay for all events, set by @command{uniform-delay}.
@item random
Use a high-entropy random variable delay.
@item per-event
Use the delay specified by each individual event.
@end table
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
#if 0
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
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
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
#endif

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
#if 0
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(Unstable, "unstable", 
	"alter simulation behavior on unstable", 
	"Alters simulator behavior on an instability violation.",
	unstable)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(WeakUnstable, "weak-unstable", 
	"alter simulation behavior on weak-unstable",
	"Alters simulator behavior on a weak-instability violation.", 
	weak_unstable)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(Interference, "interference", 
	"alter simulation behavior on interference",
	"Alters simulator behavior on an interference violation.",
	interference)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(WeakInterference, "weak-interference", 
	"alter simulation behavior on weak-interference",
	"Alters simulator behavior on a weak-interference violation.",
	weak_interference)
#endif

#undef	DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/cause.texi
@cindex cause, events
@deffn Command cause
Show causes of events when events are printed.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Cause, "cause", view, 
	"include causality with event diagnostics")

int
Cause::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.show_cause();
	return Command::NORMAL;
}
}

void
Cause::usage(ostream& o) {
	o << name << endl;
	o <<
"Print the last event to have triggered this event in diagnostics." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - _
/***
@texinfo cmd/nocause.texi
@deffn Command nocause
Turn off @command{cause} in feedback.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoCause, "nocause", view, 
	"suppress causality of event diagnostics")

int
NoCause::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.no_show_cause();
	return Command::NORMAL;
}
}

void
NoCause::usage(ostream& o) {
	o << name << endl;
	o << "Suppress event causalities in diagnostics." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace.texi
@deffn Command trace file
Record events to tracefile @var{file}.  
Overwrites @var{file} if it already exists.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Trace, "trace", tracing, 
	"record trace of all events to file")

int
Trace::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.open_trace(a.back())) {
		// confirm message
		cout << "Writing simulation trace to \"" << a.back()
			<< "\"." << endl;
		return Command::NORMAL;
	} else {
		cout << "Error opening file \"" << a.back() <<
			"\" for trace recording." << endl;
		return Command::BADARG;
	}
}
}

void
Trace::usage(ostream& o) {
	o << name << " <file>" << endl;
	o << "Records all data and events to file for later analysis.\n"
"Trace-file is completed with a \'trace-close\' command, or automatically\n"
"upon termination of ths simulation." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-file.texi
@deffn Command trace-file
Print the name of the currently opened trace file.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(TraceFile, "trace-file", tracing, 
	"show the name of the active trace file")

int
TraceFile::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.is_tracing()) {
		cout << "Active trace file: " <<
			s.get_trace_manager()->get_trace_name() << endl;
	} else {
		cout << "No active trace file." << endl;
	}
	return Command::NORMAL;
}
}

void
TraceFile::usage(ostream& o) {
	o << name << endl;
	o << "Prints the name of the active trace file, if applicable." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-close.texi
@deffn Command trace-close
Finish writing the currently opened tracefile by flushing out the last epoch
and concatenating the header with the stream body.  
Trace is automatically closed when the simulator exits.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(TraceClose, "trace-close", tracing, 
	"close the active trace file")

int
TraceClose::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.close_trace();
	return Command::NORMAL;
}
}

void
TraceClose::usage(ostream& o) {
	o << name << endl;
	o << "Stops the active trace and writes it out to file." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-flush-notify.texi
@deffn Command trace-flush-notify [0|1]
Enable (1) or disable (0) notifications when trace epochs are flushed.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(TraceFlushNotify, 
	"trace-flush-notify", tracing, 
	"enable/disable trace flush notifications (debug)")
int
TraceFlushNotify::main(State&, const string_list& a) {
switch (a.size()) {
case 1:
	cout << "Trace flush notification is ";
	if (TraceManager::notify_flush) {
		cout << "enabled." << endl;
	} else {
		cout << "disabled." << endl;
	}
	return Command::NORMAL;
case 2:
	size_t i;
	if (string_to_num(a.back(), i)) {
		cerr << "Error parsing numeric argument." << endl;
		return Command::BADARG;
	}
	TraceManager::notify_flush = i;
	return Command::NORMAL;
default:
	usage(cerr << "usage: ");
	return Command::SYNTAX;
}
}

void
TraceFlushNotify::usage(ostream& o) {
	o << name << " [0|1]" << endl;
	o << 
"With argument, enables (1) or disables (0) trace flush notifications\n"
"Without argument, reports current policy." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-flush-interval.texi
@deffn Command trace-flush-interval steps
If @var{steps} is given, set the size of each epoch according to the
number of events executed, otherwise report the current epoch size.  
This regulates the granularity of saving traces in a space-time tradeoff.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(TraceFlushInterval, 
	"trace-flush-interval", tracing, 
	"set/get the current trace chunk granularity")

int
TraceFlushInterval::main(State& s, const string_list& a) {
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
		return Command::BADARG;
	}
	s.set_trace_flush_interval(i);
	break;
default:
	usage(cerr << "usage: ");
	return Command::SYNTAX;
}
	return Command::NORMAL;
}

void
TraceFlushInterval::usage(ostream& o) {
	o << name << " [interval]" << endl;
	o <<
"If argument is passed, then set the trace flush interval to it.\n"
"Otherwise, just report the current trace flush interval.\n"
"The interval is counted in number of events that execute." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-dump.texi
@deffn Command trace-dump file
Produce textual dump of trace file contents in @var{file}.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(TraceDump, 
	"trace-dump", tracing, 
	"spill a human-readable (?) text dump of a trace file")

int
TraceDump::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (TraceManager::text_dump(a.back(), cout)) {
		cerr << "Error opening trace file: " << a.back() << endl;
		return Command::BADARG;
	}
	return Command::NORMAL;
}
}

void
TraceDump::usage(ostream& o) {
	o << name << " <tracefile>" << endl;
	o << "Dumps contents of a trace file to stdout.\n"
"Future versions may require a proper object file to be attached." << endl;
}

//=============================================================================
#undef	DECLARE_AND_INITIALIZE_COMMAND_CLASS
//=============================================================================
}	// end namespace CHPSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

