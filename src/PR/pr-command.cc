/**
	\file "PR/pr-command.cc"
	Command-line feature for PR simulator.
	TODO: scheme interface
	$Id: pr-command.cc,v 1.4 2011/05/17 21:19:54 fang Exp $
 */

#define	ENABLE_STATIC_TRACE		0
#define	ENABLE_STACKTRACE		0

#include "util/static_trace.hh"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
#include "PR/pr-command.hh"
#include "PR/placement_engine.hh"
#include "sim/command_base.tcc"
#include "sim/command_category.tcc"
#include "sim/command_registry.tcc"
#include "sim/command_macros.tcc"
#include "sim/command_common.tcc"

// clang-3.1 needs these points of instantiation/initialization earlier
namespace HAC {
namespace SIM {
DEFAULT_STATIC_TRACE
template class command_registry<PR::Command>;
}
}

#include "common/TODO.hh"
#include "util/optparse.hh"
#include "util/STL/container_iterator.hh"
#if PR_VARIABLE_DIMENSIONS
#include "util/STL/valarray_iterator.hh"
#endif

//=============================================================================
// explicit instantiations
namespace HAC {
using namespace HAC::SIM;
namespace PR {
using std::ofstream;

//=============================================================================
// name/index translation features?

// reference parsing features?

//=============================================================================
// command categories
#define	DECLARE_COMMAND_CATEGORY(x, y)					\
DECLARE_GENERIC_COMMAND_CATEGORY(CommandCategory, x, y)

DEFAULT_STATIC_TRACE
DECLARE_COMMAND_CATEGORY(builtin, "built-in commands")
DECLARE_COMMAND_CATEGORY(general, "general commands")
// DECLARE_COMMAND_CATEGORY(debug, "debugging internals")
DECLARE_COMMAND_CATEGORY(setup, "constructing the constrained system")
DECLARE_COMMAND_CATEGORY(simulation, "simulation commands")
// DECLARE_COMMAND_CATEGORY(objects, "object creation/manipulation commands")
DECLARE_COMMAND_CATEGORY(info, "information about objects")
DECLARE_COMMAND_CATEGORY(xport, "exporting positions to other formats")
// DECLARE_COMMAND_CATEGORY(physics, "physical properties of the system")
// DECLARE_COMMAND_CATEGORY(parameters, "simulation control parameters")
DECLARE_COMMAND_CATEGORY(tracing, "checkpointing and tracing features")
DEFAULT_STATIC_TRACE

#undef	DECLARE_COMMAND_CATEGORY

//=============================================================================
// command-line completion features

#if 0
/**
	Tell each command to override default completer.  
	\param _class can be any type name, such as a typedef.  
 */
#define	PR_OVERRIDE_DEFAULT_COMPLETER(_class, _func)			\
OVERRIDE_DEFAULT_COMPLETER(PR, _class, _func)

/**
	Same thing, but with forward declaration of class.  
 */
#define	PR_OVERRIDE_DEFAULT_COMPLETER_FWD(_class, _func)		\
class _class;								\
PR_OVERRIDE_DEFAULT_COMPLETER(_class, _func)

#define	PR_OVERRIDE_TEMPLATE_COMPLETER_FWD(_class, _func)		\
PR_OVERRIDE_DEFAULT_COMPLETER(_class, _func)
#endif

//=============================================================================
// command declaration macros

#define	PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(_class, _cat)		\
INSTANTIATE_TRIVIAL_COMMAND_CLASS_SIM(PR, _class, _cat)

#define	PR_INSTANTIATE_STATELESS_COMMAND_CLASS(_class, _cat)		\
typedef	stateless_command_wrapper<_class, State>	_class;		\
INSTANTIATE_COMMON_COMMAND_CLASS_SIM(PR, stateless_command_wrapper, _class, _cat)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
CommandCategory& (*_class::category)(void) = &__initialized_cat_ ## _category;	\
const size_t _class::receipt_id = CommandRegistry::register_command<_class >();

/**
	Combined macro for declaring and defining a local command class.
 */
#define	DECLARE_AND_INITIALIZE_COMMAND_CLASS(_class, _cmd, _category, _brief) \
	DECLARE_PR_COMMAND_CLASS(_class)				\
	INITIALIZE_COMMAND_CLASS(_class, _cmd, _category, _brief)

//=============================================================================
// the commands

/***
@texinfo cmd/echo.texi
@deffn Command echo args ...
Prints the arguments back to stdout.
@end deffn
@end texinfo
***/
PR_INSTANTIATE_STATELESS_COMMAND_CLASS(Echo, builtin)

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
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Help, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/comment.texi
@deffn Command # ...
@deffnx Command comment ...
Whole line comment, ignored by interpreter.  
@end deffn
@end texinfo
***/
PR_INSTANTIATE_STATELESS_COMMAND_CLASS(CommentPound, builtin)
PR_INSTANTIATE_STATELESS_COMMAND_CLASS(CommentComment, builtin)


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	All<State>					All;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(All, builtin)

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
PR_INSTANTIATE_STATELESS_COMMAND_CLASS(Exit, builtin)
PR_INSTANTIATE_STATELESS_COMMAND_CLASS(Quit, builtin)
PR_INSTANTIATE_STATELESS_COMMAND_CLASS(Abort, builtin)

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
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Alias, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unalias.texi
@deffn Command unalias cmd
Undefines an existing alias @var{cmd}.
@end deffn
@end texinfo
***/
typedef	UnAlias<State>				UnAlias;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(UnAlias, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unaliasall.texi
@deffn Command unaliasall
Undefines @emph{all} aliases.  
@end deffn
@end texinfo
***/
typedef	UnAliasAll<State>			UnAliasAll;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(UnAliasAll, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/aliases.texi
@deffn Command aliases
Print a list of all known aliases registered with the interpreter.
@end deffn
@end texinfo
***/
typedef	Aliases<State>				Aliases;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Aliases, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/repeat.texi
@deffn Command repeat n cmd...
Repeat a command @var{cmd} a fixed number of times, @var{n}.  
If there are any errors in during command processing, the loop
will terminate early with a diagnostic message.
@end deffn
@end texinfo
***/
typedef	Repeat<State>				Repeat;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Repeat, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/history.texi
@deffn Command history [start [end]]
Prints command history.
If no arguments given, then print entire command history.
If only @var{start} is given, print to the most recent line.
If @var{start} is negative, count backwards from last line.
If @var{end} is positive, count forward from @var{start}.
If @var{end} is negative, count backward from last line.
@end deffn

@deffn Command history-noninteractive [on|off]
Controls the recording of non-interactive commands in the history.
@end deffn

@deffn Command history-save file
Writes command line history to file @var{file}.
@end deffn

@deffn Command history-rerun start [end]
Reruns a set of previous commands.  
@var{start} is the first line to rerun.
If @var{end} is omitted, only one line is rerun.
If @var{end} is negative, count backwards from the most recent to
determine the last line to run in the range.
If @var{end} is positive, take that as the number of lines to
execute from @var{start}, inclusive.
@end deffn
@end texinfo
***/
typedef	History<State>				History;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(History, builtin)
typedef	HistoryNonInteractive<State>		HistoryNonInteractive;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(HistoryNonInteractive, builtin)
typedef	HistorySave<State>			HistorySave;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(HistorySave, builtin)
typedef	HistoryRerun<State>			HistoryRerun;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(HistoryRerun, builtin)

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
This works if @command{ipple} was originally launched interactively
and without redirecting a script through stdin.  

@example
$ @kbd{ipple}
ipple> @kbd{!cat foo.iplrc}
# foo.iplrc
echo hello world
interpret
echo goodbye world
ipple> @kbd{source foo.iplrc}
hello world
ipple>> @kbd{echo where am I?}
where am I?
ipple>> @kbd{exit}
goodbye world
ipple> @kbd{exit}
$
@end example
@end deffn
@end texinfo
***/
typedef	Interpret<State>			Interpret;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Interpret, builtin)

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
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(EchoCommands, builtin)

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
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Source, general)

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
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(AddPath, general)

//-----------------------------------------------------------------------------
// SIMULATION commands

#if 0
/***
@texinfo cmd/initialize.texi
@deffn Command initialize
Resets the variable state of the simulation (to unknown), 
while preserving other settings such as mode and breakpoints.  
The random number generator seed is untouched by this command.
@end deffn
@end texinfo
***/
typedef	Initialize<State>			Initialize;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Initialize, simulation)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/reset.texi
@deffn Command reset
Similar to @command{initialize}, but also resets all modes to their
default values.  
This command can be used to quickly bring the simulator to the initial
startup state, without having to exit and relaunch.  
This also resets the random number generator seed used with @command{seed48}.
@end deffn
@end texinfo
***/
typedef	Reset<State>				Reset;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Reset, simulation)
#endif

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
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Save, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/load.texi
@deffn Command load ckpt
Loads a @command{ipple} checkpoint file into the simulator state.
Loading a checkpoint will not overwrite the current status of
the auto-save file, the previous autosave command will keep effect.  
Loading a checkpoint, however, will close any open tracing streams.  
@end deffn
@end texinfo
***/
typedef	Load<State>				Load;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Load, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/***
@texinfo cmd/autosave.texi
@deffn Command autosave [on|off [file]]
Automatically save checkpoint upon end of simulation, 
regardless of exit status.
The @command{reset} command will turn off auto-save;
to re-enable it with the same file name, just @kbd{autosave on}.
The @option{-a} command line option is another way of enabling and specifying 
the autosave checkpoint name.  
@end deffn
@end texinfo
***/
typedef	AutoSave<State>				AutoSave;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(AutoSave, tracing)
#endif

//-----------------------------------------------------------------------------
#if 0
/***
@texinfo cmd/trace.texi
@deffn Command trace file
Record events to tracefile @var{file}.  
Overwrites @var{file} if it already exists.  
A trace stream is automatically closed when the @command{initialize}
or @command{reset} commands are invoked.  
See the @option{-r} option for starting up the simulator
with a newly opened trace stream.
@end deffn
@end texinfo
***/
typedef	Trace<State>				Trace;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Trace, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-file.texi
@deffn Command trace-file
Print the name of the currently opened trace file.  
@end deffn
@end texinfo
***/
typedef	TraceFile<State>			TraceFile;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceFile, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-close.texi
@deffn Command trace-close
Finish writing the currently opened trace file by flushing out the last epoch
and concatenating the header with the stream body.  
Trace is automatically closed when the simulator exits.  
@end deffn
@end texinfo
***/
typedef	TraceClose<State>			TraceClose;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceClose, tracing)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
/***
@texinfo cmd/trace-flush-notify.texi
@deffn Command trace-flush-notify [0|1]
Enable (1) or disable (0) notifications when trace epochs are flushed.  
@end deffn
@end texinfo
***/
typedef	TraceFlushNotify<State>			TraceFlushNotify;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceFlushNotify, tracing)

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
typedef	TraceFlushInterval<State>		TraceFlushInterval;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceFlushInterval, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-dump.texi
@deffn Command trace-dump file
Produce textual dump of trace file contents in @var{file}.
@end deffn
@end texinfo
***/
typedef	TraceDump<State>			TraceDump;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceDump, tracing)
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/seed48.texi
@deffn Command seed48 [int int int]
Corresponds to libc's seed48 function.  
With no argument, print the current values of the internal random number seed.
With three (unsigned short) integers, sets the random number seed.
Note: the seed is automatically saved and restored in checkpoints.  
The seed value is reset to 0 0 0 with the @command{reset} command, 
but not with the @command{initialize} command.
@end deffn
@end texinfo
***/
typedef	Seed48<State>			Seed48;
PR_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Seed48, simulation)

//=============================================================================
// handy macros

#define	REQUIRE_EXACT_ARGS(a, N)					\
if (a.size() != N) {							\
	usage(cerr << "usage: ");					\
	return Command::SYNTAX;						\
}

#define	REQUIRE_MIN_ARGS(a, N)						\
if (a.size() < N) {							\
	usage(cerr << "usage: ");					\
	return Command::SYNTAX;						\
}

#define	LEX_INDEX(i, ref)						\
	if (string_to_num(ref, i)) {					\
		cerr << "Error lexing object index." << endl;		\
		return Command::BADARG;					\
	}

#define	LEX_NUM(i, ref)							\
	if (string_to_num(ref, i)) {					\
		cerr << "Error lexing number." << endl;			\
		return Command::BADARG;					\
	}

#define	LEX_VECTOR(v, ref)						\
	if (parse_real_vector(ref, v)) {				\
		cerr << "Error parsing vector." << endl;		\
		return Command::BADARG;					\
	}

//-----------------------------------------------------------------------------
/**
	Takes single numeric parameter as an argument.
 */
template <class T>
static
int
simple_engine_command_1num(State& s, const string_list& a,
		void (State::*mf)(const T&), void usage(ostream&)) {
	REQUIRE_EXACT_ARGS(a, 2)
	real_type r;
	LEX_NUM(r, a.back());
	(s.*mf)(r);
	return Command::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
static
int
simple_engine_command_2num(State& s, const string_list& a,
		void (State::*mf)(const T&, const T&), void usage(ostream&)) {
	REQUIRE_EXACT_ARGS(a, 3)
	real_type r1, r2;
	string_list::const_iterator ai(++a.begin());
	LEX_NUM(r1, *ai);
	++ai;
	LEX_NUM(r2, *ai);
	(s.*mf)(r1, r2);
	return Command::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class T>
static
int
simple_engine_command_3num(State& s, const string_list& a,
		void (State::*mf)(const T&, const T&, const T&),
		void usage(ostream&)) {
	REQUIRE_EXACT_ARGS(a, 4)
	real_type r1, r2, r3;
	string_list::const_iterator ai(++a.begin());
	LEX_NUM(r1, *ai);
	++ai;
	LEX_NUM(r2, *ai);
	++ai;
	LEX_NUM(r3, *ai);
	(s.*mf)(r1, r2, r3);
	return Command::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Calls a function that takes no arguments.  
 */
static
int
simple_engine_command(State& s, const string_list& a,
		void (State::*mf)(void), void usage(ostream&)) {
	REQUIRE_EXACT_ARGS(a, 1)
	(s.*mf)();
	return Command::NORMAL;
}

//=============================================================================
// constraints commands

/***
@texinfo cmd/geometry.texi
@deffn Command geometry corner corner
Sets the bounds of the placement problem with two vectors that represent
the far opposite corners of the space.
Corners are formatted @t{<X,Y,Z>}, including the angle brackets.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Geometry, "geometry", setup,
        "sets coordinate bounds of the system")

int
Geometry::main(State& s, const string_list& a) {
	REQUIRE_EXACT_ARGS(a, 3)
	string_list::const_iterator i(++a.begin());
	const string& s1(*i++);
	const string& s2(*i++);
	real_vector v1, v2;
	LEX_VECTOR(v1, s1)
	LEX_VECTOR(v2, s2)
	// minswap elements
	util::vector_ops::min_swap_elements(v1, v2);
	s.opt.lower_corner = v1;
	s.opt.upper_corner = v2;
	return Command::NORMAL;
}

void
Geometry::usage(ostream& o) {
	o << name << " <VEC1> <VEC2>" << endl;
	o <<
"Sets the rectangular bounds of the space using coordinates of VEC1 and VEC2."
	<< endl;
}

//=============================================================================
// setup commands

/***
@texinfo cmd/object-type.texi
@deffn Command object-type attributes...
Declares an object type having the listed attributes.
Attributes are formatted as @t{key=value} pairs.
Each newly created object type is referenced by an index, 
in the order in which it was added.  
Upon startup, there exists an object type with default attributes
at index 0, so the first object type the user adds will be at index 1.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ObjectType, "object-type", setup,
        "declare an object type")

int
ObjectType::main(State& s, const string_list& a) {
	string_list::const_iterator i(++a.begin()), e(a.end());
	bool err = false;
	tile_type t;
	for ( ; i!=e; ++i) {
		if (t.parse_property(*i)) { err = true; }
	}
	if (err) {
		return Command::BADARG;
	}
	s.add_object_type(t);
	return Command::NORMAL;
}

void
ObjectType::usage(ostream& o) {
	o << name << " [KEY=VALUE ...]" << endl;
	o <<
"Defines an object type template with a specified set of attributes.\n"
"Unspecified attributes get assigned default values."
	<< endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/add-object.texi
@deffn Command add-object attributes...
Creates an object with the specified attributes.  
In addition, the user may reference an existing object type
by specifying @t{type=N} and acquire all the attributes from 
the indexed object type.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddObject, "add-object", setup,
        "adds an object (node)")

int
AddObject::main(State& s, const string_list& a) {
	string_list::const_iterator i(++a.begin()), e(a.end());
	bool err = false;
	tile_instance t;
//	tile_instance t(s.space.dimensions);
	for ( ; i!=e; ++i) {
		option_value v(optparse(*i));
		if (v.key == "type") {
			if (v.values.empty()) {
				cerr << "Error: expecting type index" << endl;
				err = true;
			}
			size_t j;
			if (string_to_num(v.values.front(), j)) {
				cerr << "Error lexing type index" << endl;
				err = true;
			}
			if (j >= s.num_object_types()) {
				cerr << "Error: invalid type index" << endl;
				err = true;
			}
			// inherit all attributes and properties
			t.properties = s.get_object_type(j);
		} else if (t.properties.parse_property(v)) { err = true; }
		// else no error, continue
	}
	if (err) {
		return Command::BADARG;
	}
	s.add_object(t);
	return Command::NORMAL;
}

void
AddObject::usage(ostream& o) {
	o << name << " [KEY=VALUE ...]" << endl;
	o <<
"Creates an object with the specified attributes.\n"
"If \'type=#' is specified as an attribute, inherit the properties from the \n"
"referenced type."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/add-pin.texi
@deffn Command add-pin attributes...
Same as @command{add-object} followed by @command{pin}ning the newly
created object.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddPin, "add-pin", setup,
        "adds an immovable object")

int
AddPin::main(State& s, const string_list& a) {
	const int ret = AddObject::main(s, a);
	if (ret) {
		return ret;
	}
	s.pin_object(s.num_objects() -1);	// never fails
	return Command::NORMAL;
}

void
AddPin::usage(ostream& o) {
	o <<
"same as 'add-object', also fixes the position of the new object"
	<< endl;
	
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/channel-type.texi
@deffn Command channel-type attributes...
Declares a channel type having the listed attributes.
Attributes are formatted as @t{key=value} pairs.
Each newly created channel type is referenced by an index, 
in the order in which it was added.  
Upon startup, there exists a channel type with default attributes
at index 0, so the first channel type the user adds will be at index 1.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelType, "channel-type", setup,
        "declare an channel type")

int
ChannelType::main(State& s, const string_list& a) {
	string_list::const_iterator i(++a.begin()), e(a.end());
	bool err = false;
	channel_type t;
	for ( ; i!=e; ++i) {
		if (t.parse_property(*i)) { err = true; }
	}
	if (err) {
		return Command::BADARG;
	}
	s.add_channel_type(t);
	return Command::NORMAL;
}

void
ChannelType::usage(ostream& o) {
	o << name << " [KEY=VALUE ...]" << endl;
	o <<
"Defines a channel type template with a specified set of attributes."
	<< endl;
}

//-----------------------------------------------------------------------------
#if 0
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NetType, "net-type", setup,
        "declare an multi-terminal net type")
#endif
//-----------------------------------------------------------------------------
/***
@texinfo cmd/add-channel.texi
@deffn Command add-channel src dst attributes...
Creates a channel (spring) connecting two terminals (objects)
with the specified attributes.  
In addition, the user may reference an existing channel type
by specifying @t{type=N} and acquire all the attributes from 
the indexed channel type.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddChannel, "add-channel", setup,
        "adds a channel (spring) object")

int
AddChannel::main(State& s, const string_list& a) {
	REQUIRE_MIN_ARGS(a, 3)
	string_list::const_iterator i(++a.begin()), e(a.end());
	channel_instance t;
	LEX_INDEX(t.source, *i)
	++i;
	LEX_INDEX(t.destination, *i)
	++i;
	bool err = false;
	for ( ; i!=e; ++i) {
		option_value v(optparse(*i));
		if (v.key == "type") {
			if (v.values.empty()) {
				cerr << "Error: expecting type index" << endl;
				err = true;
			}
			size_t j;
			if (string_to_num(v.values.front(), j)) {
				cerr << "Error lexing type index" << endl;
				err = true;
			}
			if (j >= s.num_channel_types()) {
				cerr << "Error: invalid type index" << endl;
				err = true;
			}
			// inherit all attributes and properties
			t.properties = s.get_channel_type(j);
		} else if (t.properties.parse_property(v)) { err = true; }
		// else no error, continue
	}
	if (err) {
		return Command::BADARG;
	}
	s.add_channel(t);
	return Command::NORMAL;
}

void
AddChannel::usage(ostream& o) {
	o << name << " SOURCE DESTINATION [KEY=VALUE ...]" << endl;
	o <<
"Creates a channel with the specified attributes.\n"
"If \'type=#' is specified as an attribute, inherit the properties from the \n"
"referenced type."
	<< endl;
}

//-----------------------------------------------------------------------------
#if 0
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddNet, "add-net", setup,
        "adds a multi-terminal net")
#endif

//-----------------------------------------------------------------------------
/***
@texinfo cmd/add-well.texi
@deffn Command add-x-well loc
@deffnx Command add-y-well loc
@deffnx Command add-z-well loc
Creates a gravity well for attracting objects to aligned to 
the specified @var{loc} coordinate in the x,y,z planes.  
Objects will be attracted to the nearest wells in each hyperplane.  
Users will typically want a z-well at 0 to encourage planar placement.  
@end deffn
@end texinfo
***/
/***
@texinfo cmd/add-wells.texi
@deffn Command add-x-wells min step max
@deffnx Command add-y-wells min step max
@deffnx Command add-z-wells min step max
Creates a series of gravity wells spaced apart by @var{step}
starting from @var{min} in the x,y,z planes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddXWell, "add-x-well", setup,
        "adds a single gravity well at x=k for alignment")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddXWells, "add-x-wells", setup,
        "adds a strided gravity wells on x-hyperplanes")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddYWell, "add-y-well", setup,
        "adds a single gravity well at y=k for alignment")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddYWells, "add-y-wells", setup,
        "adds a strided gravity wells on y-hyperplanes")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddZWell, "add-z-well", setup,
        "adds a single gravity well at z=k for alignment")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddZWells, "add-z-wells", setup,
        "adds a strided gravity wells on z-hyperplanes")
// likewise, remove-well commands

int
AddXWell::main(State& s, const string_list& a) {
	return simple_engine_command_1num(s, a, &State::add_x_well, usage);
}

void
AddXWell::usage(ostream& o) {
	o << name << " x-coord" << endl;
	o << "Adds a single gravity well along the plane x=k." << endl;
}

int
AddXWells::main(State& s, const string_list& a) {
	return simple_engine_command_3num(s, a, &State::add_x_wells, usage);
}

void
AddXWells::usage(ostream& o) {
	o << name << " x-min x-step x-max" << endl;
	o <<
"Adds a series of evenly spaced gravity planes from x=min to x=max."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
AddYWell::main(State& s, const string_list& a) {
	return simple_engine_command_1num(s, a, &State::add_y_well, usage);
}

void
AddYWell::usage(ostream& o) {
	o << name << " y-coord" << endl;
	o << "Adds a single gravity well along the plane y=k." << endl;
}

int
AddYWells::main(State& s, const string_list& a) {
	return simple_engine_command_3num(s, a, &State::add_y_wells, usage);
}

void
AddYWells::usage(ostream& o) {
	o << name << " y-min y-step y-max" << endl;
	o <<
"Adds a series of evenly spaced gravity planes from y=min to y=may."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
int
AddZWell::main(State& s, const string_list& a) {
	return simple_engine_command_1num(s, a, &State::add_z_well, usage);
}

void
AddZWell::usage(ostream& o) {
	o << name << " z-coord" << endl;
	o << "Adds a single gravity well along the plane z=k." << endl;
}

int
AddZWells::main(State& s, const string_list& a) {
	return simple_engine_command_3num(s, a, &State::add_z_wells, usage);
}

void
AddZWells::usage(ostream& o) {
	o << name << " z-min z-step z-max" << endl;
	o <<
"Adds a series of evenly spaced gravity planes from z=min to z=max."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/snap-gravity-wells.texi
@deffn Command snap-gravity-wells
Forces all objects to re-locate to the nearest gravity well in
each hyperplane.  
Typically, this is used for final placement legalization.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(SnapGravityWells, "snap-gravity-wells",
	simulation,
        "snap all objects to their nearest gravity well in each hyperplane")

int
SnapGravityWells::main(State& s, const string_list& a) {
	return simple_engine_command(s, a,
		&State::snap_to_gravity_wells, usage);
}

void
SnapGravityWells::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//=============================================================================
// physics commands

/***
@texinfo cmd/parameter.texi
@deffn Command parameter attributes...
Sets a global system parameter such as physical parameters
and simulation parameters.  
Each parameter is formatted @t{key=value}.
All parameters can be listed using the @command{dump-parameters} command.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Parameter, "parameter", setup,
        "sets/gets a physical parameter of the system")

int
Parameter::main(State& s, const string_list& a) {
	string_list::const_iterator i(++a.begin()), e(a.end());
	bool err = false;
	for ( ; i!=e; ++i) {
		if (s.parse_parameter(*i)) { err = true; }
	}
	return err ? Command::BADARG : Command::NORMAL;
}

// properties of placement_engine: temperature, friction
void
Parameter::usage(ostream& o) {
	o << name << " [KEY[=VALUE] ...]" << endl;
	o <<
"Controls parameters for the simulation, including physics.\n";
	placer_options::list_parameters(o) << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <bool placer_options::*bmem, bool V>
static
int
set_boolean_option(State& s, const string_list& a, void usage(ostream&)) {
	REQUIRE_EXACT_ARGS(a, 1)
	s.opt.*bmem = V;
	return Command::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/***
@texinfo cmd/watch-objects.texi
@deffn Command watch-objects
@deffnx Command nowatch-objects
When enabled, simulator will print out coordinates of all objects
as their locations are updated.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WatchObjects, "watch-objects", setup,
        "print coordinates of objects after each iteration")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoWatchObjects, "nowatch-objects", setup,
        "suppress coordinates of objects after each iteration")

int
WatchObjects::main(State& s, const string_list& a) {
	return set_boolean_option<&placer_options::watch_objects, true>
		(s, a, usage);
}

void
WatchObjects::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

int
NoWatchObjects::main(State& s, const string_list& a) {
	return set_boolean_option<&placer_options::watch_objects, false>
		(s, a, usage);
}

void
NoWatchObjects::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/***
@texinfo cmd/watch-deltas.texi
@deffn Command watch-deltas
@deffnx Command nowatch-deltas
OBSOLETE.
When enabled, simulator will print out changes in coordinates of all objects
as their locations are updated.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WatchDeltas, "watch-deltas", setup,
        "print change in position/velocity after each iteration")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoWatchDeltas, "nowatch-deltas", setup,
        "suppress change in position/velocity after each iteration")

int
WatchDeltas::main(State& s, const string_list& a) {
	return set_boolean_option<&placer_options::watch_deltas, true>
		(s, a, usage);
}

void
WatchDeltas::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

int
NoWatchDeltas::main(State& s, const string_list& a) {
	return set_boolean_option<&placer_options::watch_deltas, false>
		(s, a, usage);
}

void
NoWatchDeltas::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/***
@texinfo cmd/watch-energy.texi
@deffn Command watch-energy
@deffnx Command nowatch-energy
When enabled, simulator will print out kinetic and potential energy
with every step of simulation.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(WatchEnergy, "watch-energy", setup,
        "print energy after each iteration")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoWatchEnergy, "nowatch-energy", setup,
        "suppress energy after each iteration")

int
WatchEnergy::main(State& s, const string_list& a) {
	return set_boolean_option<&placer_options::watch_energy, true>
		(s, a, usage);
}

void
WatchEnergy::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

int
NoWatchEnergy::main(State& s, const string_list& a) {
	return set_boolean_option<&placer_options::watch_energy, false>
		(s, a, usage);
}

void
NoWatchEnergy::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/***
@texinfo cmd/report-iterations.texi
@deffn Command report-iterations
@deffnx Command noreport-iterations
When enabled, simulator will print out number of iterations executed per 
convergence loop.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ReportIterations, "report-iterations",
	setup,
        "print energy after each iteration")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoReportIterations, "noreport-iterations",
	setup,
        "suppress energy after each iteration")

int
ReportIterations::main(State& s, const string_list& a) {
	return set_boolean_option<&placer_options::report_iterations, true>
		(s, a, usage);
}

void
ReportIterations::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

int
NoReportIterations::main(State& s, const string_list& a) {
	return set_boolean_option<&placer_options::report_iterations, false>
		(s, a, usage);
}

void
NoReportIterations::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//=============================================================================
// info commands

/***
@texinfo cmd/dump-state.texi
@deffn Command dump-state
Print information for the entire state of the system: parameters, objects,
etc...
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DumpState, "dump-state", info,
        "print entire state of simulation")

int
DumpState::main(State& s, const string_list& a) {
	REQUIRE_EXACT_ARGS(a, 1)
	s.dump(cout);
	return Command::NORMAL;
}

void
DumpState::usage(ostream& o) {
	o << name << endl;
	o <<
"Prints out the state of the entire simulated system."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/dump-parameters.texi
@deffn Command dump-parameters
Print global system parameters.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DumpParameters, "dump-parameters", info,
        "print set of parameters")

int
DumpParameters::main(State& s, const string_list& a) {
	REQUIRE_EXACT_ARGS(a, 1)
	s.dump_parameters(cout);
	return Command::NORMAL;
}

void
DumpParameters::usage(ostream& o) {
	o << name << endl;
	o <<
"Prints out the current simulation and solver parameters."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/dump-objects.texi
@deffn Command dump-objects
Print state and location of all objects.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DumpObjects, "dump-objects", info,
        "print current coordinates and state of objects")

int
DumpObjects::main(State& s, const string_list& a) {
	REQUIRE_EXACT_ARGS(a, 1)
	s.dump_objects(cout);
	return Command::NORMAL;
}

void
DumpObjects::usage(ostream& o) {
	o << name << endl;
	o <<
"Prints out the current location and state of all objects."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/dump-positions.texi
@deffn Command dump-positions
Print location of all objects.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DumpPositions, "dump-positions", info,
        "print only current coordinates of objects")

int
DumpPositions::main(State& s, const string_list& a) {
	REQUIRE_EXACT_ARGS(a, 1)
	s.dump_positions(cout);
	return Command::NORMAL;
}

void
DumpPositions::usage(ostream& o) {
	o << name << endl;
	o <<
"Prints out the current location of all objects."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/dump-energy.texi
@deffn Command dump-energy
Print detailed kinetic and potential energy breakdown for the whole system.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DumpEnergy, "dump-energy", info,
        "update and print current energy of system")

int
DumpEnergy::main(State& s, const string_list& a) {
	REQUIRE_EXACT_ARGS(a, 1)
	s.dump_energy(cout);
	return Command::NORMAL;
}

void
DumpEnergy::usage(ostream& o) {
	o << name << endl;
	o <<
"Update and print out the current energy of the entire system."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/emit-dot.texi
@deffn Command emit-dot [file]
Print to stdout or @var{file} in the dot language (from graphviz), suitable
for generating a diagram of the current state of objects.
Recommendation: run the output through the 
@command{fdp} (force-directed placement) command:
@command{fdp -Tpdf input -o output.pdf}.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(EmitDot, "emit-dot", xport,
        "print system to graphviz output annotated with coordinates")

int
EmitDot::main(State& s, const string_list& a) {
const size_t asz = a.size();
if (asz > 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else if (asz == 2) {
	ofstream ofs(a.back().c_str());
	s.emit_dot(ofs);
} else {
	s.emit_dot(cout);
}
	return Command::NORMAL;
}

void
EmitDot::usage(ostream& o) {
	o << name << " [file]" << endl;
	o << brief << endl;
	o << "If file is omitted, result is printed to stdout." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/emit-fig.texi
@deffn Command emit-fig [file]
Print to stdout or @var{file} in the fig language (xfig), suitable
for generating a diagram of the current state of objects.
Recommendation: run the output through the 
@command{fig2dev} export command, such as:
@command{fig2dev -Lpdf input.fig -o output.pdf}.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(EmitFig, "emit-fig", xport,
        "print system to xfig output")

int
EmitFig::main(State& s, const string_list& a) {
const size_t asz = a.size();
if (asz > 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else if (asz == 2) {
	ofstream ofs(a.back().c_str());
	s.emit_fig(ofs);
} else {
	s.emit_fig(cout);
}
	return Command::NORMAL;
}

void
EmitFig::usage(ostream& o) {
	o << name << " [file]" << endl;
	o << brief << endl;
	o << "If file is omitted, result is printed to stdout." << endl;
}

//=============================================================================
// simulation commands

/***
@texinfo cmd/place.texi
@deffn Command place obj loc
Manually move object indexed @var{obj} to location @var{loc}, a 3D vector.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Place, "place", simulation,
        "manually place an object")

int
Place::main(State& s, const string_list& a) {
	REQUIRE_EXACT_ARGS(a, 3)
	string_list::const_iterator j(++a.begin());
	size_t i;
	LEX_INDEX(i, *j)
	real_vector v;
	++j;
	LEX_VECTOR(v, *j);
	if (s.place_object(i, v)) {
		return Command::BADARG;
	}
	return Command::NORMAL;
}

void
Place::usage(ostream& o) {
	o << name << " OBJECT <POSITION>" << endl;
o <<
"Manually relocates an object to the specified position.\n"
"NOTE: This does not fix the location of the object (see \'pin\' command)."
	<< endl;
}

//-----------------------------------------------------------------------------
#if 0
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Throw, "throw", simulation,
        "impart a impulse velocity on an object")
#endif

/***
@texinfo cmd/pin.texi
@deffn Command pin obj
@deffnx Command unpin obj
Fix object @var{obj} at its current location.
This is usually done for boundary terminals.  
@command{unpin} allows an object to move freely. 
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Pin, "pin", simulation,
        "fix location of object to present or specified location")

int
Pin::main(State& s, const string_list& a) {
	REQUIRE_EXACT_ARGS(a, 2)
	size_t i;
	LEX_INDEX(i, a.back())
	if (s.pin_object(i)) {
		return Command::BADARG;
	}
	return Command::NORMAL;
}

void
Pin::usage(ostream& o) {
//	o << name << " OBJECT [<VEC>]" << endl;
	o << name << " OBJECT" << endl;
	o << brief << endl;
}

//-----------------------------------------------------------------------------
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnPin, "unpin", simulation,
        "allow object to move freely")

int
UnPin::main(State& s, const string_list& a) {
	REQUIRE_EXACT_ARGS(a, 2)
	size_t i;
	LEX_INDEX(i, a.back())
	if (s.unpin_object(i)) {
		return Command::BADARG;
	}
	return Command::NORMAL;
}

void
UnPin::usage(ostream& o) {
	o << name << " OBJECT" << endl;
	o << brief << endl;
}

//=============================================================================
/***
@texinfo cmd/scatter.texi
@deffn Command scatter
Relocates every object to some random location within the bounding box.
This is often done at the start of the simulation.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Scatter, "scatter", simulation,
        "randomize location of all un-pinned objects")

int
Scatter::main(State& s, const string_list& a) {
	return simple_engine_command(s, a, &State::scatter, usage);
}

void
Scatter::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/step.texi
@deffn Command step
Advances the simulation one iteration using the 
current @t{time_step} (parameter).
Each iteration evaluates the forces (spring, repulsion, gravity)
acting upon all objects, and then updates the position and velocity
by numerical integration.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Step, "step", simulation,
        "advances simulation one iteration")

int
Step::main(State& s, const string_list& a) {
	return simple_engine_command(s, a, &State::iterate_no_delta, usage);
}

void
Step::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/simple-converge.texi
@deffn Command simple-converge
Runs @command{step} iteratively until some convergence criterion is met.  
The convergence criterion used here is when the relative maximum
change in position and velocity fall below the thresholds specified
in the global parameters, @t{position_tolerance} and 
@t{velocity_tolerance}.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(SimpleConverge, "simple-converge", 
	simulation,
        "iterate with constant time_step until convergence")

int
SimpleConverge::main(State& s, const string_list& a) {
	return simple_engine_command(s, a, &State::simple_converge, usage);
}

void
SimpleConverge::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
o <<
"Transient simulation converges when greatest change in position and velocity\n"
"fall below the respective tolerances in a given time-step."
	<< endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/descend-gradient.texi
@deffn Command descend-gradient
Accelerates all objects in a straight line 
(constant acceleration determined by an initial force calculation), 
until potential energy no longer decreases monotonically.  
At the start of this routine, all velocity/momentum is reset to 0.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DescendGradient, "descend-gradient", 
	simulation,
        "iterate linear direction until potential energy increases")

int
DescendGradient::main(State& s, const string_list& a) {
	return simple_engine_command(s, a,
		&State::gradient_slide, usage);
}

void
DescendGradient::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
o <<
"Initially resets velocities to zero, killing momentum.\n"
"Computes instantaneous force vector, and accelerates in that direction\n"
"(with constant acceleration) while potential energy monotonically decreases."
	<< endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/descend-gradient-converge.texi
@deffn Command descend-gradient-converge
Repeatedly runs @command{descend-gradient} until local minimum
in potential energy is found.  
This is very similar to the conjugate-gradient minimization method.  
@cindex conjugate gradient method
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DescendGradientConverge,
	"descend-gradient-converge", 
	simulation,
        "repeat descend-gradient (conjugate-gradient)")

int
DescendGradientConverge::main(State& s, const string_list& a) {
	return simple_engine_command(s, a,
		&State::repeat_gradient_slide, usage);
}

void
DescendGradientConverge::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
o <<
"Repeated descend-gradient while potential energy decreases monotonically."
	<< endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/descend-potential.texi
@deffn Command descend-potential
At the start of this routine, all velocity/momentum is reset to 0.  
Simulates transiently @command{step} until while potential energy
is monotonically decreasing.  
Unlike @command{descend-gradient} the force is continuously updated
with every iteration, so the paths taken by each object may curve.
This stops early to avoid overshooting a local minimum due to kinetic energy.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DescendPotential, "descend-potential", 
	simulation,
        "iterate with constant time_step until potential energy increases")

int
DescendPotential::main(State& s, const string_list& a) {
	return simple_engine_command(s, a,
		&State::descend_potential_energy, usage);
}

void
DescendPotential::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
o <<
"Initially sets velocities to zero, killing all momentum."
"Advances simulation with continuous force updates while potential energy\n"
"monotonically decreases."
	<< endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/descend-potential-converge.texi
@deffn Command descend-potential-converge
Repeatedly runs @command{descend-potential} until local minimum
in potential energy is found.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(DescendPotentialConverge,
	"descend-potential-converge", 
	simulation,
        "alternate descend-potential and kill-momentum until convergence")

int
DescendPotentialConverge::main(State& s, const string_list& a) {
	return simple_engine_command(s, a,
		&State::repeat_descend_potential_energy, usage);
}

void
DescendPotentialConverge::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
o <<
"Repeats descend-potential while potential energy montonically decreases."
	<< endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/kill-momentum.texi
@deffn Command kill-momentum
Resets velocities of all objects to 0.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(KillMomentum, "kill-momentum", 
	simulation,
        "zero out all object velocities, killing momentum")

int
KillMomentum::main(State& s, const string_list& a) {
	return simple_engine_command(s, a,
		&State::kill_momentum, usage);
}

void
KillMomentum::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//-----------------------------------------------------------------------------
/***
@texinfo cmd/shake-all.texi
@deffn Command shake-all [maxdist]
Randomly perturbs positions of all objects in random direction, 
uniformly random distance bounded by @var{maxdist}.
This helps the system get unstuck from some local minima.
If @var{maxdist} is omitted, the global @t{temperature} parameter
is used instead.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ShakeAll, "shake-all", 
	simulation,
        "perturb all objects in random direction by bounded random distance")

int
ShakeAll::main(State& s, const string_list& a) {
	const size_t asz = a.size();
	if (asz > 2) {
		usage(cerr << "usage: ");
		return Command::SYNTAX;
	}
	real_type dist = s.opt.temperature;
	if (asz == 2) {
		LEX_NUM(dist, a.back())
	}
	s.shake(dist);
	return Command::NORMAL;
}

void
ShakeAll::usage(ostream& o) {
	o << name << " [maxdist]" << endl;
	o << brief << endl;
o <<
"If maxdist is not supplied, the temperature parameter is used by default."
	<< endl;
}

//=============================================================================
// select functions
// operations for selecting and apply operations en mass to objects

static
vector<size_t> selected_objects;
static
vector<size_t> selected_channels;

/***
select - 
select-all - 
unselect -
unselect-all -
select-toggle -
***/

//=============================================================================
}	// end namespace PR
}	// end namespace HAC

DEFAULT_STATIC_TRACE_BEGIN

