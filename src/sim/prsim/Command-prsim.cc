/**
	\file "sim/prsim/Command-prsim.cc"
	All built-in commands should be statically registered 
	in this translation unit to guarantee proper static object
	initialization ordering; the command_map must be constructed
	before entries are registered.  

	TODO: consider using some form of auto-indent
		in the help-system.  

	$Id: Command-prsim.cc,v 1.92 2011/05/25 23:09:52 fang Exp $

	NOTE: earlier version of this file was:
	Id: Command.cc,v 1.23 2007/02/14 04:57:25 fang Exp
	but had to be renamed to avoid base-object file name clashes
		which broke dyld on i686-apple-darwin, cause Command.o
		to fail to be loaded due to conflict with chpsim's.  
	Painful discontinuity in revision history...
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#define	ENABLE_STACKTRACE				0

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>

#include "sim/prsim/Command-prsim.h"
#include "sim/prsim/Command-prsim-export.h"
#include "sim/prsim/State-prsim.tcc"
#if PRSIM_TRACE_GENERATION
#include "sim/prsim/Trace-prsim.h"
#endif
#if PRSIM_VCD_GENERATION
#include "sim/prsim/VCDManager.h"
#endif
#include "sim/command_base.tcc"
#include "sim/command_category.tcc"
#include "sim/command_registry.tcc"
#include "sim/command_macros.tcc"
#include "sim/command_common.tcc"
#include "parser/instref.h"
#include "Object/def/footprint.h"

#include "common/TODO.h"
#include "util/libc.h"
#include "util/memory/excl_malloc_ptr.h"
#include "util/stacktrace.h"

/**
	These commands are deprecated, but provided for backwards compatibility.
	Eventually disable this.  
	(NOTE: the alias command can always effectively re-enable it.)
 */
#define	WANT_OLD_RANDOM_COMMANDS			1

/**
	Define to 1 to allow some commands to take aggregate array
	references to nodes.  
	Goal: 1
	Status: buggy because dir-stack expansion transforms the 
		range operator ".." into a working directory/path.
		The path substitutor is not smart enough yet;
		needs to become a full path-reference parser.
		TODO: enhance instref parser to handle paths.
	Note: the reason why channels work is because they
		don't use the working directory (yet),
		known outstanding issue.
 */
#define	PRSIM_NODE_AGGREGATE_ARGUMENTS			1
/**
	Same idea for process references.
	Not worth the trouble.
 */
#define	PRSIM_PROCESS_AGGREGATE_ARGUMENTS		0

#if PRSIM_NODE_AGGREGATE_ARGUMENTS
#define	NODE_FOR_EACH(ref)						\
	vector<node_index_type> _tmp;					\
	if (parse_nodes_to_indices(ref, s.get_module(), _tmp)) {	\
		cerr << "No such node found: " << ref << endl;		\
		return Command::BADARG;					\
	}								\
	vector<node_index_type>::const_iterator				\
		niter(_tmp.begin()), nend(_tmp.end());			\
	for ( ; niter!=nend; ++niter)
#endif

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
using util::tokenize_char;
using util::excl_malloc_ptr;
using util::strings::string_to_num;
using entity::global_indexed_reference;
using entity::global_reference_array_type;
using entity::META_TYPE_PROCESS;
using entity::META_TYPE_BOOL;
using entity::META_TYPE_NONE;
using parser::process_index;
using parser::bool_index;

//=============================================================================
// directory features

/**
	Define to 1 to use directory scope features.
	TODO: these functions could be shared, in sim/directory.{h,cc}
 */
#define	AUTO_PREPEND_WORKING_DIR	1

static
#if AUTO_PREPEND_WORKING_DIR
string
#else
const string&
#endif
nonempty_abs_dir(const string& s) {
#if AUTO_PREPEND_WORKING_DIR
	string t(CommandRegistry::prepend_working_dir(s));
	if (t.empty()) {
		t = ".";
	}
	return t;
#else
	return s;
#endif
}

#if AUTO_PREPEND_WORKING_DIR
// wrap around definitions in "parser/instref.h"
static
// bool_index
node_index_type
parse_node_to_index(const string& s, const entity::module& m) {
	STACKTRACE_VERBOSE;
	STACKTRACE_INDENT_PRINT("Parsing node(s): " << s << endl);
	// automatically prepend working directory
	return parser::parse_node_to_index(
		CommandRegistry::prepend_working_dir(s), m).index;
}

#if PRSIM_NODE_AGGREGATE_ARGUMENTS
static
bool
parse_nodes_to_indices(const string& s, const entity::module& m, 
		vector<node_index_type>& r) {
	return parser::parse_nodes_to_indices(
		CommandRegistry::prepend_working_dir(s), m, r);
}
#endif

static
process_index
parse_process_to_index(const string& s, const entity::module& m) {
	STACKTRACE_VERBOSE;
	// automatically prepend working directory
	const string t(nonempty_abs_dir(s));
	return parser::parse_process_to_index(t, m);
}

#if PRSIM_PROCESS_AGGREGATE_ARGUMENTS
static
bool
parse_processes_to_indices(const string& s, const entity::module& m, 
		vector<process_index_type>& r) {
	return parser::parse_processes_to_indices(
		CommandRegistry::prepend_working_dir(s), m, r);
}
#endif

static
int
parse_name_to_what(ostream& o, const string& s, const entity::module& m) {
	STACKTRACE_VERBOSE;
	return parser::parse_name_to_what(o, 
		CommandRegistry::prepend_working_dir(s), m);
}

static
entity::global_indexed_reference
parse_global_reference(const string& s, const entity::module& m) {
	STACKTRACE_VERBOSE;
	return parser::parse_global_reference(
		CommandRegistry::prepend_working_dir(s), m);
}

#if 0
// use this for many commands!
static
int
parse_global_references(const string& s, const entity::module& m, 
		global_reference_array_type& a) {
	STACKTRACE_VERBOSE;
	return parser::parse_global_references(
		CommandRegistry::prepend_working_dir(s), m, a);
}
#endif

static
int
parse_name_to_get_subnodes(const string& s, const entity::module& m,
		vector<size_t>& v) {
	STACKTRACE_VERBOSE;
	const string t(nonempty_abs_dir(s));
	return parser::parse_name_to_get_subnodes(t, m, v);
}

static
int
parse_name_to_get_subnodes_local(const string& s, const entity::module& m,
		vector<size_t>& v) {
	STACKTRACE_VERBOSE;
//	const string t(nonempty_abs_dir(s));
	return parser::parse_name_to_get_subnodes_local(
		parse_process_to_index(s, m), m, v);
}

#if 0
static
int
parse_name_to_get_ports(const process_index& p, const entity::module& m,
		vector<size_t>& v, const vector<bool>* pred = NULL) {
	STACKTRACE_VERBOSE;
	return parser::parse_name_to_get_ports(p, m, v, pred);
}
#endif

static
int
parse_name_to_get_ports(const string& s, const entity::module& m,
		vector<size_t>& v, const vector<bool>* pred = NULL) {
	STACKTRACE_VERBOSE;
//	const string t(nonempty_abs_dir(s));
	return parser::parse_name_to_get_ports(
		parse_process_to_index(s, m), m, v, pred);
}

#else
// just use original functions
using parser::parse_node_to_index;
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
using parser::parse_nodes_to_indices;
#endif
using parser::parse_process_to_index;
#if PRSIM_PROCESS_AGGREGATE_ARGUMENTS
using parser::parse_processes_to_indices;
#endif
using parser::parse_global_reference;
using parser::parse_global_references;
using parser::parse_name_to_what;
using parser::parse_name_to_get_subnodes;
using parser::parse_name_to_get_subnodes_local;
using parser::parse_name_to_get_ports;
#endif

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
	tracing("tracing", "trace and checkpoint commands"), 
	modes("modes", "timing model, error handling");

//=============================================================================
// command completion facilities

/**
	Tell each command to override default completer.  
	\param _class can be any type name, such as a typedef.  
 */
#define	PRSIM_OVERRIDE_DEFAULT_COMPLETER(_class, _func)			\
OVERRIDE_DEFAULT_COMPLETER(PRSIM, _class, _func)

/**
	Same thing, but with forward declaration of class.  
 */
#define	PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(_class, _func)		\
class _class;								\
PRSIM_OVERRIDE_DEFAULT_COMPLETER(_class, _func)

#define	PRSIM_OVERRIDE_TEMPLATE_COMPLETER_FWD(_class, _func)		\
PRSIM_OVERRIDE_DEFAULT_COMPLETER(_class, _func)

//=============================================================================
// macros for re-using common command classes

#define	PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(_class, _cat)		\
INSTANTIATE_TRIVIAL_COMMAND_CLASS(PRSIM, _class, _cat)

#define	PRSIM_INSTANTIATE_STATELESS_COMMAND_CLASS(_class, _cat)		\
typedef	stateless_command_wrapper<_class, State>	_class;		\
INSTANTIATE_COMMON_COMMAND_CLASS(PRSIM, stateless_command_wrapper, _class, _cat)

#if 0
/**
	\param _compl is the overriding tab-completer function.
 */
#define	PRSIM_INSTANTIATE_MODULE_COMMAND_CLASS(_class, _cat, _compl)	\
typedef	module_command_wrapper<_class, State>		_class;		\
PRSIM_OVERRIDE_DEFAULT_COMPLETER(_class, _compl)			\
INSTANTIATE_COMMON_COMMAND_CLASS(PRSIM, module_command_wrapper, _class, _cat)
#endif

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

/**
	Combined macro for declaring and defining a local command class.
 */
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
PRSIM_INSTANTIATE_STATELESS_COMMAND_CLASS(Echo, builtin)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Help, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/comment.texi
@deffn Command # ...
@deffnx Command comment ...
Whole line comment, ignored by interpreter.  
@end deffn
@end texinfo
***/
PRSIM_INSTANTIATE_STATELESS_COMMAND_CLASS(CommentPound, builtin)
PRSIM_INSTANTIATE_STATELESS_COMMAND_CLASS(CommentComment, builtin)


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	All<State>					All;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(All, builtin)

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
PRSIM_INSTANTIATE_STATELESS_COMMAND_CLASS(Exit, builtin)
PRSIM_INSTANTIATE_STATELESS_COMMAND_CLASS(Quit, builtin)
PRSIM_INSTANTIATE_STATELESS_COMMAND_CLASS(Abort, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/precision.texi
@deffn Command precision [n]
Sets the precision of real-valued numbers to be printed.
Without an argument, this command just reports the current precision.
@end deffn
@end texinfo
***/
PRSIM_INSTANTIATE_STATELESS_COMMAND_CLASS(Precision, builtin)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Alias, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unalias.texi
@deffn Command unalias cmd
Undefines an existing alias @var{cmd}.
@end deffn
@end texinfo
***/
typedef	UnAlias<State>				UnAlias;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(UnAlias, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unaliasall.texi
@deffn Command unaliasall
Undefines @emph{all} aliases.  
@end deffn
@end texinfo
***/
typedef	UnAliasAll<State>			UnAliasAll;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(UnAliasAll, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/aliases.texi
@deffn Command aliases
Print a list of all known aliases registered with the interpreter.
@end deffn
@end texinfo
***/
typedef	Aliases<State>				Aliases;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Aliases, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/directories.texi
The following commands emulate a directory like interface for 
navigating the instance hierarchy, reminiscent of shells.  
By default, @emph{all instance references are relative to the current
working directory}, just like in a shell.
Prefix with @samp{::} to use absolute (from-the-top) reference.
Go up levels of hierarchy with @samp{../} prefix.
The hierarchy separator is @samp{.} (dot).

@deffn Command cd dir
Changes current working level of hierarchy.
@end deffn

@deffn Command pushd dir
Pushes new directory onto directory stack.
@end deffn

@deffn Command popd
Removes last entry on directory stack.
@end deffn

@deffn Command pwd
Prints current working directory.
@end deffn

@deffn Command dirs
Prints entire directory stack.
@end deffn
@end texinfo
***/
typedef	ChangeDir<State>			ChangeDir;
PRSIM_OVERRIDE_TEMPLATE_COMPLETER_FWD(ChangeDir, instance_completer)
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(ChangeDir, builtin)

typedef	PushDir<State>				PushDir;
PRSIM_OVERRIDE_TEMPLATE_COMPLETER_FWD(PushDir, instance_completer)
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(PushDir, builtin)

typedef	PopDir<State>				PopDir;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(PopDir, builtin)

typedef	WorkingDir<State>			WorkingDir;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(WorkingDir, builtin)

typedef	Dirs<State>				Dirs;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Dirs, builtin)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Repeat, builtin)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/meas-time.texi
@deffn Command meas-time cmd...
Reports time spent in a command.
@end deffn
@end texinfo
***/
typedef	MeasTime<State>				MeasTime;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(MeasTime, builtin)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(History, builtin)
typedef	HistoryNonInteractive<State>		HistoryNonInteractive;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(HistoryNonInteractive, builtin)
typedef	HistorySave<State>			HistorySave;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(HistorySave, builtin)
typedef	HistoryRerun<State>			HistoryRerun;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(HistoryRerun, builtin)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Interpret, builtin)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(EchoCommands, builtin)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Source, general)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(AddPath, general)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/paths.texi
@deffn Command paths
Print the list of paths searched for source scripts.  
@end deffn
@end texinfo
***/
typedef	Paths<State>				Paths;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Paths, general)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/process-member-separator.texi
@deffn process-member-separator [sep]
Sets the process hierarchy delimiter to @var{sep} instead of the
default '@t{.}' (period).
This can be useful for printing names for use with other tools.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ProcessMemberSep,
	"process-member-separator", general,
	"output separator for process members")

int
ProcessMemberSep::main(State& s, const string_list& a) {
if (a.size() > 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else if (a.size() == 2) {
	s._dump_flags.process_member_separator = a.back();
} else {
	// just print current separator
	cout << "process-member-separator: " <<
		s._dump_flags.process_member_separator << endl;
}
	return Command::NORMAL;
}

void
ProcessMemberSep::usage(ostream& o) {
	o << name << " [sep]" << endl;
	o <<
"Sets the process member separator used for printing hierarchical names."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Initialize, simulation)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Reset, simulation)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
/***
@texinfo cmd/x-all.texi
@deffn Command x-all
This resets the values of all nodes to @t{X}, and clears the event queue
and all other state except for the time, which is left as-is.
Trace files are kept @emph{open}, mode flags, and channel setups are retained.  
However, channel logs are closed.  
This is useful for executing multiple simulation runs in one long trace.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(XAll, "x-all", simulation,
	"set all nodes to X without restarting simulation")

int
XAll::main(State& s, const string_list& a) {
if (a.size() > 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.x_all();
	return Command::NORMAL;
}
}

void
XAll::usage(ostream& o) {
	o << name << endl;
o <<
"Resets all nodes to X, clears event queue and all state except time,\n"
"mode-flags, channel setup.  Active trace files remain open." << endl;
}

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
#if !PRSIM_AGGREGATE_EXCEPTIONS
	try {
#endif
	while (!s.stopped_or_fatal() && i && GET_NODE((ni = s.step()))) {
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
		if (s.watching_all_nodes()
#if USE_WATCHPOINT_FLAG
			|| n.is_watchpoint()
#endif
			) {
			print_watched_node(cout << '\t' << ct << '\t', s, ni);
		}
		if (n.is_breakpoint()) {
#if !USE_WATCHPOINT_FLAG
			// this includes watchpoints
			const bool w = s.is_watching_node(GET_NODE(ni));
			if (w) {
			if (!s.watching_all_nodes()) {
				print_watched_node(cout << '\t' << ct << '\t',
					s, ni);
			}	// else already have message from before
			}
			// channel support
			if (!w) {
#endif
				const string nodename(
					s.get_node_canonical_name(GET_NODE(ni)));
				// node is plain breakpoint
				cout << "\t*** break, " << i <<
					" steps left: `" << nodename <<
					"\' became ";
				n.dump_value(cout) <<
					" at time " << s.time() << endl;
				return Command::NORMAL;
				// or Command::BREAK; ?
#if !USE_WATCHPOINT_FLAG
			}
#endif
		}
	}	// end while
#if PRSIM_AGGREGATE_EXCEPTIONS
	if (s.is_fatal()) {
		return error_policy_to_status(s.inspect_exceptions());
	}
#else
	} catch (const step_exception& exex) {
		return error_policy_to_status(exex.inspect(s, cerr));
	}	// no other exceptions
#endif
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
#if !PRSIM_AGGREGATE_EXCEPTIONS
	try {
#endif
	State::step_return_type ni;	// also stores the cause of the event
	while (!s.stopped_or_fatal() && i && GET_NODE((ni = s.step()))) {
		--i;
		// NB: may need specialization for real-valued (float) time.  
		const time_type ct(s.time());
		const node_type& n(s.get_node(GET_NODE(ni)));
		/***
			The following code should be consistent with
			Cycle::main() and Advance::main().
			tracing stuff here later...
		***/
		if (s.watching_all_nodes()
#if USE_WATCHPOINT_FLAG
			|| n.is_watchpoint()
#endif
				) {
			print_watched_node(cout << '\t' << ct << '\t', s, ni);
		}
		if (n.is_breakpoint()) {
#if !USE_WATCHPOINT_FLAG
			// this includes watchpoints
			const bool w = s.is_watching_node(GET_NODE(ni));
			if (w) {
			if (!s.watching_all_nodes()) {
				print_watched_node(cout << '\t' << ct << '\t',
					s, ni);
			}	// else already have message from before
			}
			// channel support
			if (!w) {
#endif
				const string nodename(
					s.get_node_canonical_name(GET_NODE(ni)));
				// node is plain breakpoint
				cout << "\t*** break, " << i <<
					" steps left: `" << nodename <<
					"\' became ";
				n.dump_value(cout) <<
					" at time " << s.time() << endl;
				return Command::NORMAL;
				// or Command::BREAK; ?
#if !USE_WATCHPOINT_FLAG
			}
#endif
		}
	}	// end while
#if PRSIM_AGGREGATE_EXCEPTIONS
	if (s.is_fatal()) {
		return error_policy_to_status(s.inspect_exceptions());
	}
#else
	} catch (const step_exception& exex) {
		return error_policy_to_status(exex.inspect(s, cerr));
	}	// no other exceptions
#endif
	return Command::NORMAL;
}

int
StepEvent::main(State& s, const string_list& a) {
	CommandRegistry::forbid_cosimulation(a);
if (a.size() > 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
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
	State::step_return_type ni;
	s.resume();	// clear STOP flag
#if !PRSIM_AGGREGATE_EXCEPTIONS
	try {
#endif
	while (!s.stopped_or_fatal() && GET_NODE((ni = s.step()))) {
		if (!GET_NODE(ni))
			return Command::NORMAL;
		const node_type& n(s.get_node(GET_NODE(ni)));
		/***
			The following code should be consistent with
			Step::main() and Advance::main().
		***/
		if (s.watching_all_nodes()
#if USE_WATCHPOINT_FLAG
			|| n.is_watchpoint()
#endif
				) {
			print_watched_node(cout << '\t' << s.time() <<
				'\t', s, ni);
		}
		if (n.is_breakpoint()) {
#if !USE_WATCHPOINT_FLAG
			// this includes watchpoints
			const bool w = s.is_watching_node(GET_NODE(ni));
			if (w) {
			if (!s.watching_all_nodes()) {
				print_watched_node(cout << '\t' <<
					s.time() << '\t', s, ni);
			}	// else already have message from before
			}
			// channel support
			if (!w) {
#endif
				const string nodename(s.get_node_canonical_name(
					GET_NODE(ni)));
				// node is plain breakpoint
				cout << "\t*** break, `" << nodename <<
					"\' became ";
				n.dump_value(cout) <<
					" at time " << s.time() << endl;
				return Command::NORMAL;
				// or Command::BREAK; ?
#if !USE_WATCHPOINT_FLAG
			}
#endif
		}
	}	// end while (!s.stopped_or_fatal())
#if PRSIM_AGGREGATE_EXCEPTIONS
	if (s.is_fatal()) {
		return error_policy_to_status(s.inspect_exceptions());
	}
#else
	} catch (const step_exception& exex) {
		return error_policy_to_status(exex.inspect(s, cerr));
	}	// no other exceptions
#endif
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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Queue, info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/set.texi
@deffn Command set node val [delay]
Set @var{node} to @var{val}.  
@var{val} can be @t{0}, @t{1}, @t{X}, 
or @t{~} which means "opposite-of-the-current-value".
If @var{delay} is omitted, the set event is inserted `now' at 
the current time, at the head of the event queue.
If @var{delay} is given with a @t{+} prefix, time is added relative to
`now', otherwise it is scheduled at an absolute time @var{delay}.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Set, instance_completer)
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
	// now I'm wishing string_list was string_vector... :) can do!
	int err = 0;
	string_list::const_iterator ai(++a.begin());
	const string& objname(*ai++);	// node name
	const string& _val(*ai++);	// node value
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	} // else
#endif
		// valid values are 0, 1, 2(X)
		const value_enum val = s.node_to_value(_val, ni);
		if (!node_type::is_valid_value(val)) {
			cerr << "Invalid logic value: " << _val << endl;
			return Command::SYNTAX;
		}
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
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return (err < 1) ? Command::NORMAL : Command::BADARG;
}
}	// end Set::main

int
Set::main(State& s, const string_list& a) {
	return __set_main(s, a, false, &Set::usage);
}

void
Set::usage(ostream& o) {
	o << "set <node> <0|F|1|T|X|U|~|?> [+delay | time]" << endl;
	o <<
"\tWithout delay, node is set immediately.  Relative delay into future\n"
"\tis given by +delay, whereas an absolute time is given without \'+\'.\n"
"\tIf there is a pending event on the node, this command is ignored with a\n"
"\twarning.  The value '~' means opposite-of-the-current-value.\n"
"\tThe value '?' means any non-X value (0 or 1, random).\n"
"See also \'setf\'."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/set-pair-random.texi
@deffn Command set-pair-random node1 node2
Sets a pair of nodes to random, opposite values.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(SetPairRandom, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(SetPairRandom, "set-pair-random", 
	simulation, "sets a pair of nodes to random, opposite values")

int
SetPairRandom::main(State& s, const string_list& a) {
	const size_t asz = a.size();
if (asz != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator ai(++a.begin());
	const string& objname1(*ai++);	// node name
	const string& objname2(*ai++);	// node value
	const node_index_type ni1 =
		parse_node_to_index(objname1, s.get_module());
	const node_index_type ni2 =
		parse_node_to_index(objname2, s.get_module());
	if (!ni1) {
		cerr << "No such node found: " << objname1 << endl;
		return Command::BADARG;
	}
	if (!ni2) {
		cerr << "No such node found: " << objname2 << endl;
		return Command::BADARG;
	}
	const value_enum val = node_type::char_to_value('?');
	const value_enum nval = node_type::invert_value[val];
	const bool force = false;
	int err1 = s.set_node(ni1, val, force);
	int err2 = s.set_node(ni2, nval, force);
	return (err1 || err2) ? Command::BADARG : Command::NORMAL;
}
}

void
SetPairRandom::usage(ostream& o) {
	o << name << " <node1> <node2>" << endl;
	o << brief << endl;
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(SetF, instance_completer)
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(UnSet, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnSet, "unset", simulation,
	"force re-evaluation of node\'s input state, may cancel setf")

int
UnSet::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
#endif
	s.unset_node(ni);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Setr, instance_completer)
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
	// now I'm wishing string_list was string_vector... :) can do!
	string_list::const_iterator ai(++a.begin());
	const string& objname(*ai++);	// node name
	int err = 0;
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
#endif
		const string& _val(*ai++);	// node value
		// valid values are 0, 1, 2(X)
		const value_enum val = s.node_to_value(_val, ni);
		if (!node_type::is_valid_value(val)) {
			cerr << "Invalid logic value: " << _val << endl;
			return Command::SYNTAX;
		}
		err = s.set_node_after(ni, val,
			State::exponential_random_delay(), force);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return (err < 1) ? Command::NORMAL : Command::BADARG;
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
#if PRSIM_UPSET_NODES
/***
@texinfo cmd/freeze.texi
@deffn Command freeze node
Prevents @var{node} from switching cause by updates on its fanins.
Any pending events that are already in the event queue will remain
in the queue and fire when they reach the head.  
Q: How does this affect channels?
@end deffn

@deffn Command thaw node
Aliases to the @command{unset}, removes the frozen state to allow 
transition events, and also re-evaluates fanin to automatically 
enqueue an event when it should fire.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Freeze, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Thaw, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Freeze, "freeze", simulation,
	"freeze node, preventing further switching activity")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Thaw, "thaw", simulation,
	"allow node to resume switching (same as unset)")

int
Freeze::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		s.freeze_node(ni);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
}
}

void
Freeze::usage(ostream& o) {
	o << name << " node" << endl;
	o << "Prevent all switching activity on node.\n"
"Events that are already pending in the event queue will remain in the queue\n"
"until they are executed." << endl;
}

int
Thaw::main(State& s, const string_list& a) {
	return UnSet::main(s, a);
}

void
Thaw::usage(ostream& o) {
	UnSet::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/frozen-verbose.texi
@deffn Command frozen-verbose [01]
Controls verbosity of simulator events concerning frozen nodes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(FrozenVerbose, "frozen-verbose", view,
	"print additional information relevant to frozen nodes")

int
FrozenVerbose::main(State& s, const string_list& a) {
switch (a.size()) {
case 1: {
	cout << "frozen-verbose ";
	if (s.is_frozen_verbose()) {
		cout << "on";
	} else {
		cout << "off";
	}
	cout << endl;
	return Command::NORMAL;
}
case 2: {
	const string& arg(a.back());
	bool m = false;
	if (arg == "on") {
		m = true;
	} else if (arg == "off") {
		m = false;
	} else {
		cerr << "Bad argument." << endl;
		usage(cerr);
		return Command::BADARG;
	}
	s.set_frozen_verbose(m);
	return Command::NORMAL;
}
default:
	usage(cerr << "usage: ");
	return Command::BADARG;
}
}

void
FrozenVerbose::usage(ostream& o) {
	o << name << " [on|off]" << endl;
	o <<
"When enabled, prints additional information about events involving frozen "
"nodes." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/upset.texi
@deffn Command upset node [val]
Forces @var{node} to remain stuck at its current value or @var{val} (if given)
until it is explicitly restored by the @command{unset} command.
@command{upset} is a combination of @command{setf} and @command{freeze}.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Upset, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Upset, "upset", simulation,
	"freeze node to value until it is explicitly restored")

/**
	This should be equivalent to { setf; freeze; }.  
**/
int
Upset::main(State& s, const string_list& a) {
const size_t sz = a.size();
if (sz < 2 || sz > 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator ai(++a.begin());
	const string& objname(*ai++);	// node name
	string _val;
	if (sz == 3) {
		_val = *ai++;	// node value
	}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
#endif
		// default: use current value
		value_enum val = s.get_node(ni).current_value();
	if (sz == 3) {
		val = s.node_to_value(_val, ni);
		if (!node_type::is_valid_value(val)) {
			cerr << "Invalid logic value: " << _val << endl;
			return Command::SYNTAX;
		}
	}
		const int err = s.set_node(ni, val, true);	// forced
		if (err) return err;
		// freeze ...
		s.freeze_node(ni);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
}
}

void
Upset::usage(ostream& o) {
	o << name << " node [val]" << endl;
o <<
"Freezes node to a given value (or current value if non specified) until\n"
"node is explicitly unfrozen by an 'unset' command." << endl;
o << "A frozen node does not react to any fanin switching activity,\n"
"but may respond to user actions." << endl;

}
#endif	// PRSIM_UPSET_NODES

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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(SetrF, instance_completer)
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
@texinfo cmd/dequeue.texi
@deffn Command dequeue node
Cancels any pending event on @var{node} from the event queue.
This can result in the circuit getting stuck in a state until the
killed node is explicitly re-evaluated 
(e.g. with an @command{unset} command).
No error condition is returned if there is no pending event associated
with the named @var{node}.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Dequeue, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Dequeue, "dequeue", simulation,
	"cancel event on a node from the event queue")

int
Dequeue::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		s.deschedule_event(ni);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
}
}

void
Dequeue::usage(ostream& o) {
o << name << " node" << endl;
o << "Cancels any pending event on the given node." << endl;
}

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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Reschedule, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(RescheduleNow, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(RescheduleFromNow, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(RescheduleRelative, instance_completer)
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
	time_type t;
	if (string_to_num(a.back(), t)) {
		cerr << "Error: invalid time argument." << endl;
		return Command::BADARG;
	}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
if (!ni) {
	cerr << "No such node found: " << nonempty_abs_dir(objname) << endl;
	return Command::BADARG;
}
#endif
	if ((s.*smf)(ni, t)) {
		// already have error message
		return Command::BADARG;
	}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
}	// end for each node
#endif
	return Command::NORMAL;
}
}

int
RescheduleNow::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
if (!ni) {
	cerr << "No such node found: " << nonempty_abs_dir(objname) << endl;
	return Command::BADARG;
}
#endif
	if (s.reschedule_event_now(ni)) {
		// already have error message
		return Command::BADARG;
	}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
}	// end for each node
#endif
	return Command::NORMAL;
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Execute, instance_completer)
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
	cerr << "No such node found: " << nonempty_abs_dir(objname) << endl;
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(BreakPt, instance_completer)
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
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
		NODE_FOR_EACH(objname) {
			const node_index_type& ni(*niter);
#else
		const node_index_type ni =
			parse_node_to_index(objname, s.get_module());
		if (!ni) {
			cerr << "No such node found: " <<
				nonempty_abs_dir(objname) << endl;
			badarg = true;
		}
#endif
		s.set_node_breakpoint(ni);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
		}	// end node for each
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(NoBreakPt, instance_completer)
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
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
		NODE_FOR_EACH(objname) {
			const node_index_type& ni(*niter);
#else
		const node_index_type ni =
			parse_node_to_index(objname, s.get_module());
		if (!ni) {
			cerr << "No such node found: " <<
				nonempty_abs_dir(objname) << endl;
			badarg = true;
		}
#endif
			s.clear_node_breakpoint(ni);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
		}	// end for each node
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(UnBreak, instance_completer)
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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Save, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/load.texi
@deffn Command load ckpt
Loads a @command{hacprsim} checkpoint file into the simulator state.
Loading a checkpoint will not overwrite the current status of
the auto-save file, the previous autosave command will keep effect.  
Loading a checkpoint, however, will close any open tracing streams.  
@end deffn
@end texinfo
***/
typedef	Load<State>				Load;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Load, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(AutoSave, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/ls.texi
@deffn Command ls name
List immediate subinstances of the instance named @var{name}.  
@end deffn
@end texinfo
***/
typedef	LS<State>				LS;
PRSIM_OVERRIDE_TEMPLATE_COMPLETER_FWD(LS, instance_completer)
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(LS, info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/what.texi
@deffn Command what name
Print the type of the instance named @var{name}.  
@end deffn
@end texinfo
***/
typedef	What<State>				What;
PRSIM_OVERRIDE_TEMPLATE_COMPLETER_FWD(What, instance_completer)
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(What, info)

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
typedef	WhoNewline<State>			WhoNewline;
PRSIM_OVERRIDE_TEMPLATE_COMPLETER_FWD(Who, instance_completer)
PRSIM_OVERRIDE_TEMPLATE_COMPLETER_FWD(WhoNewline, instance_completer)
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Who, info)
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(WhoNewline, info)

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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Pending, instance_completer)
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
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(PendingDebug, instance_completer)
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
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
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
@texinfo cmd/process-id.texi
@deffn Command process-id name
Just prints the internal process ID referenced by @var{name}.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ProcessID, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ProcessID, "process-id", info,
	"print internal index of named process")
int
ProcessID::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const module& m(s.get_module());
	const process_index p(parse_process_to_index(objname, m));
	if (!p.valid()) {
		// already have error message
		return Command::BADARG;
	}
	cout << p.index << endl;
	return Command::NORMAL;
}
}

void
ProcessID::usage(ostream& o) {
	o << name << " process" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/node-id.texi
@deffn Command node-id name
Just prints the internal node ID referenced by @var{name}.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(NodeID, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NodeID, "node-id", info,
	"print internal index of named node")
int
NodeID::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	vector<node_index_type>		nodes_id_list_type;
	const string& objname(a.back());
	const module& m(s.get_module());
	const node_index_type p(parse_node_to_index(objname, m));
	if (!p) {
		// already have error message
		return Command::BADARG;
	}
	cout << p << endl;
	return Command::NORMAL;
}
}

void
NodeID::usage(ostream& o) {
	o << name << " node" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/get.texi
@deffn Command get node
Print the current value of @var{node}.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Get, instance_completer)
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
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		// we have ni = the canonically allocated index of the bool node
		// just look it up in the node_pool
		print_watched_node(cout, s, ni, objname);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
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
@texinfo cmd/get-driven.texi
@deffn Command get-driven node
Reports the drive state of pull-up/dn on a @var{node}.  
See also @command{fanin-get} for details.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(GetDriven, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(GetDriven, "get-driven", info,
	"print pull-up/pull-dn drive state on node")

/**
	Prints current value of the named node.  
 */
int
GetDriven::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		const node_type& n(s.get_node(ni));
		const pull_enum u = n.get_pull_struct(true, NORMAL_RULE).pull();
		const pull_enum d = n.get_pull_struct(false, NORMAL_RULE).pull();
#if PRSIM_WEAK_RULES
		const pull_enum wu = n.get_pull_struct(true, WEAK_RULE).pull();
		const pull_enum wd = n.get_pull_struct(false, WEAK_RULE).pull();
#endif
		cout << objname <<
			" pulled up:" << node_type::value_to_char[u] <<
			" dn:" << node_type::value_to_char[d] <<
#if PRSIM_WEAK_RULES
			" weak-up:" << node_type::value_to_char[wu] <<
			" weak-dn:" << node_type::value_to_char[wd] <<
#endif
				endl;
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
}
}

void
GetDriven::usage(ostream& o) {
	o << name << " <node>" << endl;
	o <<
"Print the current drive state of the node, pull in all directions.\n"
"See also 'fanin-get' for details."
	<< endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/getports.texi
@deffn Command getports struct
@deffnx Command getinports struct
@deffnx Command getoutports struct
Print the state of all port nodes of @var{struct}.  
Useful for observing boundaries of channels and processes.  
@command{getinports} and @command{getoutports} partition 
the set of ports into inputs and outputs.  
Directionality of inputs/outputs is inferred by the presence of
fanin local to the @var{struct} process instance (its type).  
@end deffn

@deffn Command getcommonports struct1 struct2
Prints the state of nodes that are common to @var{struct1} and @var{struct2}.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(GetPorts, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(GetPorts, "getports", info,
	"print values of ports of structure")

PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(GetInPorts, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(GetInPorts, "getinports", info,
	"print values of input ports of structure")

PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(GetOutPorts, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(GetOutPorts, "getoutports", info,
	"print values of output ports of structure")

PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(GetCommonPorts, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(GetCommonPorts, "getcommonports", info,
	"print values of ports shared by two structures")


int
GetPorts::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	vector<node_index_type>		nodes_id_list_type;
	const string& objname(a.back());
	nodes_id_list_type nodes;
	if (parse_name_to_get_ports(objname, s.get_module(), nodes)) {
		// already got error message?
		return Command::BADARG;
	} else {
		typedef	nodes_id_list_type::const_iterator	const_iterator;
		cout << "Ports subnodes of \'" << 
			nonempty_abs_dir(objname) << "\':" << endl;
		const_iterator i(nodes.begin()), e(nodes.end());
		for ( ; i!=e; ++i) {
			s.dump_node_value(cout, *i) << endl;
		}
		return Command::NORMAL;
	}
}
}

int
GetInPorts::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	vector<node_index_type>		nodes_id_list_type;
	const string& objname(a.back());
	const module& m(s.get_module());
	const process_index p(parse_process_to_index(objname, m));
	if (!p.valid()) {
		// already got error message?
		return Command::BADARG;
	}
	nodes_id_list_type nodes;
	vector<bool> input_mask;
//	s.get_process_state(p.index).type().has_not_local_fanin_map(input_mask);
	s.get_footprint_frame(p.index)._footprint
		->has_not_sub_fanin_map(input_mask);
	if (parser::parse_name_to_get_ports(p, m, nodes, &input_mask)) {
		return Command::BADARG;
	} else {
		typedef	nodes_id_list_type::const_iterator	const_iterator;
		cout << "Input ports of \'" << 
			nonempty_abs_dir(objname) << "\':" << endl;
		const_iterator i(nodes.begin()), e(nodes.end());
		for ( ; i!=e; ++i) {
			s.dump_node_value(cout, *i) << endl;
		}
		return Command::NORMAL;
	}
}
}

int
GetOutPorts::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	vector<node_index_type>		nodes_id_list_type;
	const string& objname(a.back());
	const module& m(s.get_module());
	const process_index p(parse_process_to_index(objname, m));
	if (!p.valid()) {
		// already got error message?
		return Command::BADARG;
	}
	nodes_id_list_type nodes;
	vector<bool> output_mask;
//	s.get_process_state(p.index).type().has_local_fanin_map(output_mask);
	s.get_footprint_frame(p.index)._footprint
		->has_sub_fanin_map(output_mask);
	if (parser::parse_name_to_get_ports(p, m, nodes, &output_mask)) {
		return Command::BADARG;
	} else {
		typedef	nodes_id_list_type::const_iterator	const_iterator;
		cout << "Output ports of \'" << 
			nonempty_abs_dir(objname) << "\':" << endl;
		const_iterator i(nodes.begin()), e(nodes.end());
		for ( ; i!=e; ++i) {
			s.dump_node_value(cout, *i) << endl;
		}
		return Command::NORMAL;
	}
}
}

int
GetCommonPorts::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	vector<node_index_type>		nodes_id_list_type;
	string_list::const_iterator ai(a.begin());
	const string& objname1(*++ai);
	const string& objname2(*++ai);
	nodes_id_list_type nodes1, nodes2;
	if (parse_name_to_get_ports(objname1, s.get_module(), nodes1)) {
		// already got error message?
		return Command::BADARG;
	}
	if (parse_name_to_get_ports(objname2, s.get_module(), nodes2)) {
		// already got error message?
		return Command::BADARG;
	}
	sort(nodes1.begin(), nodes1.end());
	sort(nodes2.begin(), nodes2.end());
	nodes_id_list_type common;
	common.reserve(std::min(nodes1.size(), nodes2.size()));
	set_intersection(nodes1.begin(), nodes1.end(), 
		nodes2.begin(), nodes2.end(), back_inserter(common));
	typedef	nodes_id_list_type::const_iterator	const_iterator;
	cout << "Ports common to \'" << 
		nonempty_abs_dir(objname1) << "\' and \'" <<
		nonempty_abs_dir(objname2) << "\':" << endl;
	const_iterator i(common.begin()), e(common.end());
	for ( ; i!=e; ++i) {
		s.dump_node_value(cout, *i) << endl;
	}
	return Command::NORMAL;
}
}

void
GetPorts::usage(ostream& o) {
	o << name << " <name>" << endl;
	o <<
"prints the current values of public ports of the structure"
		<< endl;
}

void
GetInPorts::usage(ostream& o) {
	o << name << " <name>" << endl;
	o <<
"prints the current values of input ports of the structure"
		<< endl;
}

void
GetOutPorts::usage(ostream& o) {
	o << name << " <name>" << endl;
	o <<
"prints the current values of output ports of the structure"
		<< endl;
}

void
GetCommonPorts::usage(ostream& o) {
	o << name << " <name1> <name2>" << endl;
	o <<
"prints the current values of ports common to both structures"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/getlocal.texi
@deffn Command getlocal struct
Print the state of all publicly reachable subnodes of @var{struct}.  
Recursive search does not visit private subnodes.  
Useful for observing channels and processes.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(GetLocal, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(GetLocal, "getlocal", info,
	"print values of public subnodes of structure")

int
GetLocal::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	vector<node_index_type>		nodes_id_list_type;
	const string& objname(a.back());
	nodes_id_list_type nodes;
	if (parse_name_to_get_subnodes_local(objname, s.get_module(), nodes)) {
		// already got error message?
		return Command::BADARG;
	} else {
		typedef	nodes_id_list_type::const_iterator	const_iterator;
		cout << "Public subnodes of \'" << 
			nonempty_abs_dir(objname) << "\':" << endl;
		const_iterator i(nodes.begin()), e(nodes.end());
		for ( ; i!=e; ++i) {
			s.dump_node_value(cout, *i) << endl;
		}
		return Command::NORMAL;
	}
}
}

void
GetLocal::usage(ostream& o) {
	o << name << " <name>" << endl;
	o <<
"prints the current values of publicly reachable subnodes of the structure"
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(GetAll, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(GetAll, "getall", info,
	"print values of all subnodes of structure")

int
GetAll::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	vector<node_index_type>		nodes_id_list_type;
	const string& objname(a.back());
	nodes_id_list_type nodes;
#if 0
// FIXED a while ago
try {	// temporary measure until bug ACX-PR-1456 is fixed
#endif
	if (parse_name_to_get_subnodes(objname, s.get_module(), nodes)) {
		// already got error message?
		return Command::BADARG;
	} else {
		typedef	nodes_id_list_type::const_iterator	const_iterator;
		const_iterator i(nodes.begin()), e(nodes.end());
#if ENABLE_STACKTRACE
		STACKTRACE_INDENT_PRINT("node-ids: ");
		copy(i, e, ostream_iterator<size_t>(STACKTRACE_STREAM, ","));
		STACKTRACE_STREAM << endl;
#endif
		cout << "All subnodes of \'" << 
			nonempty_abs_dir(objname) << "\':" << endl;
		for ( ; i!=e; ++i) {
			s.dump_node_value(cout, *i) << endl;
		}
		return Command::NORMAL;
	}
#if 0
} catch (...) {
	cerr << "... Saved from destruction!!!" << endl;
	cerr << "You have probably triggered known bug ACX-PR-1456." << endl;
	cerr << "Please nag Fang about fixing this.  You may resume." << endl;
	return Command::BADARG;
}
#endif
}
}

void
GetAll::usage(ostream& o) {
	o << name << " <name>" << endl;
	o << "prints the current values of all subnodes of the named structure"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_AGGREGATE_EXCEPTIONS
/***
@texinfo cmd/exceptions.texi
@deffn Command exceptions
Prints the information recorded in recently occurred exceptions.
The simulators exception list is cleared each time simulation is
advanced by any number of steps.  
This command is most useful immediately after halting on exceptions.
This is not fully implemented yet: exceptions are not saved in checkpoints yet. 
Noted as ACX-PR-6641.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Exceptions, "exceptions", info, 
	"show all recent simulation exceptions")

int
Exceptions::main(State& s, const string_list& a) {
if (a.size() > 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.inspect_exceptions();
	return Command::NORMAL;
}
}

void
Exceptions::usage(ostream& o) {
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/status.texi
@deffn Command status value [proc]
@deffnx Command status-newline value [proc]
Print all nodes whose current value is @var{value}.  
Frequently used to find nodes that are in an unknown ('X') state.  
Valid @var{value} arguments are [0fF] for logic-low, [1tT] for logic-high,
[xXuU] for unknown value.  
If @var{proc} is given, then restrict the scope of search to
only subnodes of that structure.
The @command{-newline} variant prints each node on a separate line
for readability.  
@end deffn
@end texinfo
***/
// PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Status, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Status, "status", info, 
	"show all nodes matching a state value")

static
int
__status_main(State& s, const string_list& a, void (usage)(ostream&), 
		const bool nl) {
const size_t sz = a.size();
if (sz != 2 && sz != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator ai(++a.begin());
	const value_enum v = node_type::string_to_value(*ai);
	if (!node_type::is_valid_value(v)) {
		cerr << "Bad status value." << endl;
		usage(cerr);
		return Command::BADARG;
	}
	if (sz == 2) {
		s.print_status_nodes(cout, v, nl);
		return Command::NORMAL;
	} else {
		++ai;
		const string& proc(*ai);
		vector<node_index_type> nodes;
		if (parse_name_to_get_subnodes(proc, s.get_module(), nodes)) {
			// already have error message
			return Command::BADARG;
		} else {
			s.filter_nodes(nodes,
				bind2nd(mem_fun_ref(&node_type::match_value),
					v));
			cout << node_type::value_to_char[size_t(v)] <<
				" nodes in " << proc << ':' << endl;
			s.print_nodes(cout, nodes, false, nl ? "\n" : " ");
			cout << endl;
		}
	}
	return Command::NORMAL;
}
}

int
Status::main(State& s, const string_list& a) {
	return __status_main(s, a, usage, false);
}

void
Status::usage(ostream& o) {
	o << name << "<[0fF1tTxXuU]> [proc]" << endl;
	o << "list all nodes with the matching current value" << endl;
	o << "If proc is given, restrict to nodes under that struct." << endl;
}

PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(StatusNewline, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(StatusNewline, "status-newline", info, 
	"show all nodes matching a value, line separated")

int
StatusNewline::main(State& s, const string_list& a) {
	return __status_main(s, a, usage, true);
}

void
StatusNewline::usage(ostream& o) {
	o << name << " <[0fF1tTxXuU]> [proc]" << endl;
	o << "list all nodes with the matching current value" << endl;
	o << "If proc is given, restrict to nodes under that struct." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/no-status.texi
@deffn Command no-status val
Asserts that there are no nodes at value @var{val}.
@end deffn
@end texinfo
***/
// PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(NoStatus, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoStatus, "no-status", info, 
	"assert that there are no nodes at a specified value")

// TODO: support for optional proc argument to limit scope
int
NoStatus::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	vector<node_index_type> nodes;
	const value_enum v = node_type::string_to_value(a.back());
	if (node_type::is_valid_value(v)) {
		s.status_nodes(v, nodes);
		if (nodes.empty()) {
			return Command::NORMAL;
		} else {
			cout << "Assertion failed: " <<
				"there are nodes at value " << a.back() << endl;
			s.print_nodes(cout, nodes, false, "\n") << std::flush;
			return Command::FATAL;
		}
	} else {
		cerr << "Bad status value." << endl;
		usage(cerr);
		return Command::BADARG;
	}
}
}


void
NoStatus::usage(ostream& o) {
	o << name << " [01X]" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/status-interference.texi
@deffn Command status-interference
@deffnx Command status-weak-interference
Print all nodes that have strongly interfering fanins, i.e.
the pull-up and pull-downs are on and causing shorts.  
@command{status-weak-interfere} reports possible interferences
where at least one direction is being pulled @t{X} (unknown).  
This command is useful for checking the safety of a particular 
state or snapshot of your circuit.  
@end deffn
@end texinfo
***/

DECLARE_AND_INITIALIZE_COMMAND_CLASS(StatusInterfere, "status-interference", info, 
	"show all nodes with currently interfering opposing rules")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(StatusWeakInterfere, "status-weak-interference", info, 
	"show all nodes with possibly interfering opposing rules")

int
StatusInterfere::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.print_status_interference(cout, false);
	return Command::NORMAL;
}
}

void
StatusInterfere::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

int
StatusWeakInterfere::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.print_status_interference(cout, true);
	return Command::NORMAL;
}
}

void
StatusWeakInterfere::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/no-status-interference.texi
@deffn Command no-status-interference
@deffnx Command no-status-weak-interference
Asserts that there are no nodes with interfering (or weak-interfering) fanins.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoStatusInterfere, "no-status-interfere",
	info, "assert that there are no nodes with interference")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoStatusWeakInterfere,
	"no-status-weak-interfere",
	info, "assert that there are no nodes with weak-interference")

int
NoStatusInterfere::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	vector<node_index_type> nodes;
	s.status_interference(false, nodes);
	if (nodes.empty()) {
		return Command::NORMAL;
	} else {
		cout << "Assertion failed: " <<
			"there are nodes with interference" << endl;
		s.print_nodes(cout, nodes, false, "\n") << std::flush;
		return Command::FATAL;
	}
}
}

int
NoStatusWeakInterfere::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	vector<node_index_type> nodes;
	s.status_interference(true, nodes);
	if (nodes.empty()) {
		return Command::NORMAL;
	} else {
		cout << "Assertion failed: " <<
			"there are nodes with weak-interference" << endl;
		s.print_nodes(cout, nodes, false, "\n") << std::flush;
		return Command::FATAL;
	}
}
}

void
NoStatusInterfere::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

void
NoStatusWeakInterfere::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/status-driven.texi
@deffn Command status-driven val
@deffnx Command status-driven-fanin val
@deffnx Command no-status-driven val
@deffnx Command no-status-driven-fanin val
Reports all nodes that are in a particular drive-state.
The drive-state of a node is the strongest pull in any direction.  
The current value of the node is not considered.
@var{val} is 0 for undriven nodes, X for X-driven nodes, and 1 for 
driven nodes (which may include interfering nodes).  
The @command{status-driven-fanin} variant filters out nodes with no
fanin (inputs), which are always undriven.  
The @command{no-} command variants assert that there are no nodes
that match the specified drive state.
For example, @command{no-status-driven-fanin 0} asserts that
no nodes with fanins are floating or in the high impedance state.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(StatusDriven, "status-driven", info, 
	"show all nodes that are currently driven or undriven")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(StatusDrivenFanin,
	"status-driven-fanin", info, 
	"show all nodes with fanin that match a drive-state")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoStatusDriven, "no-status-driven", info, 
	"assert that no nodes match the drive-state")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoStatusDrivenFanin,
	"no-status-driven-fanin", info, 
	"assert that no nodes with fanin match the drive-state")

static
int
__status_driven(State& s, const string_list& a,
		void (*usage)(ostream&), const bool fanin_only, 
		const bool assert_empty) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& v(a.back());
	pull_enum p = PULL_OFF;
	if (v == "0") {
		p = PULL_OFF;
	} else if (v == "1") {
		p = PULL_ON;
	} else if (v == "X" || v == "x") {
		p = PULL_WEAK;
	} else {
		cerr << "Error: invalid status argument, expecting [01X]."
			<< endl;
		return Command::BADARG;
	}
	if (s.print_status_driven(cout, p, fanin_only, assert_empty))
		return Command::FATAL;
	return Command::NORMAL;
}
}

int
StatusDriven::main(State& s, const string_list& a) {
	return __status_driven(s, a, usage, false, false);
}

int
StatusDrivenFanin::main(State& s, const string_list& a) {
	return __status_driven(s, a, usage, true, false);
}

int
NoStatusDriven::main(State& s, const string_list& a) {
	return __status_driven(s, a, usage, false, true);
}

int
NoStatusDrivenFanin::main(State& s, const string_list& a) {
	return __status_driven(s, a, usage, true, true);
}

static
void
__drive_values(ostream& o, const char* name) {
	o <<
name << " 0: reports nodes that are undriven\n" <<
name << " 1: reports nodes that are driven (including interference)\n" <<
name << " X: reports nodes that are only driven by X"
	<< endl;
}

void
StatusDriven::usage(ostream& o) {
	o << name << " [01Xx]" << endl;
	__drive_values(o, name);
}

void
NoStatusDriven::usage(ostream& o) {
	o << name << " [01Xx]" << endl;
	__drive_values(o, name);
}

void
StatusDrivenFanin::usage(ostream& o) {
	o << name << " [01Xx]" << endl;
	__drive_values(o, name);
}

void
NoStatusDrivenFanin::usage(ostream& o) {
	o << name << " [01Xx]" << endl;
	__drive_values(o, name);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Common function for similar print routines.  
	Extra bool parameter is verbosity flag, true for printing
	values with nodes.  
 */
static
int
default_print_0(const State& s, const string_list& a,
		ostream& (State::*memfn)(ostream&, const bool) const,
		const bool verbose,
		void (usage)(ostream&)) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	(s.*memfn)(cout, verbose);
	return Command::NORMAL;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_UPSET_NODES
/***
@texinfo cmd/status-frozen.texi
@deffn Command status-frozen
@deffnx Command status-frozen-get
Print all nodes that have been frozen (switching suppressed).
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(StatusFrozen, "status-frozen", info, 
	"show all nodes that are frozen from switching")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(StatusFrozenGet, "status-frozen-get", 
	info, 
	"show nodes that are frozen with values")

int
StatusFrozen::main(State& s, const string_list& a) {
	return default_print_0(s, a, &State::print_status_frozen, false, usage);
}

int
StatusFrozenGet::main(State& s, const string_list& a) {
	return default_print_0(s, a,
		&State::print_status_frozen, true, usage);
}

void
StatusFrozen::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}

void
StatusFrozenGet::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}
#endif	// PRSIM_UPSET_NODES

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unused-nodes.texi
@deffn Command unused-nodes
@deffnx Command unused-nodes-get
Print all nodes with no fanins and no fanouts, regardless of state.  
@end deffn
@end texinfo
***/
// TODO: what about invariant-only nodes?

DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnusedNodes, "unused-nodes", info, 
	"list all nodes with no fanin, no fanout")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnusedNodesGet, "unused-nodes-get", info, 
	"list nodes with no fanin, no fanout, with current value")

int
UnusedNodes::main(State& s, const string_list& a) {
	return default_print_0(s, a, &State::dump_unused_nodes, false, usage);
}

int
UnusedNodesGet::main(State& s, const string_list& a) {
	return default_print_0(s, a, &State::dump_unused_nodes, true, usage);
}

void
UnusedNodes::usage(ostream& o) {
	o << name << " -- " << brief << endl;
}

void
UnusedNodesGet::usage(ostream& o) {
	o << name << " -- " << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/no-unused-nodes.texi
@deffn Command no-unused-nodes
Assert that there are no unused nodes (those without fanout).
This is mostly useful for checking closed systems that do not
required interaction with any environment.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoUnusedNodes, "no-unused-nodes", info, 
	"assert that all nodes are used (have fanout)")
int
NoUnusedNodes::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	vector<node_index_type> nodes;
	s.unused_nodes(nodes);
	if (nodes.empty()) {
		return Command::NORMAL;
	} else {
		cout << "Assertion failed: there are unused nodes" << endl;
		s.print_nodes(cout, nodes, false, "\n") << std::flush;
		return Command::FATAL;
	}
}
}

void
NoUnusedNodes::usage(ostream& o) {
	o << name << " -- " << brief << endl;
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
@texinfo cmd/no-unknown-inputs.texi
@deffn Command no-unknown-inputs
Assert that there are no inputs nodes at X.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoUnknownInputs,
	"no-unknown-inputs", info, 
	"assert that no input nodes are X")
int
NoUnknownInputs::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	vector<node_index_type> nodes;
	s.dangling_unknown_nodes(true, nodes);
	if (nodes.empty()) {
		return Command::NORMAL;
	} else {
		cout << "Assertion failed: there are input nodes at X" << endl;
		s.print_nodes(cout, nodes, false, "\n") << std::flush;
		return Command::FATAL;
	}
}
}

void
NoUnknownInputs::usage(ostream& o) {
	o << name << " -- " << brief << endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/no-unknown-inputs-fanout.texi
@deffn Command no-unknown-inputs-fanout
Assert that there are no inputs nodes with fanout at X.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoUnknownInputsFanout,
	"no-unknown-inputs-fanout", info, 
	"assert that no input nodes with fanout are X")
int
NoUnknownInputsFanout::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	vector<node_index_type> nodes;
	s.dangling_unknown_nodes(false, nodes);
	if (nodes.empty()) {
		return Command::NORMAL;
	} else {
		cout << "Assertion failed: there are input nodes with fanout at X" << endl;
		s.print_nodes(cout, nodes, false, "\n") << std::flush;
		return Command::FATAL;
	}
}
}

void
NoUnknownInputsFanout::usage(ostream& o) {
	o << name << " -- " << brief << endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unknown-undriven-fanin.texi
@deffn Command unknown-undriven-fanin
Print all nodes with value X that have fanins, but are not being pulled.
These nodes are typically candidates for adding resets to fix.  
@end deffn
@end texinfo
***/

DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnknownUndrivenFanin,
	"unknown-undriven-fanin", info, 
	"list all X nodes with fanin, not being pulled")

int
UnknownUndrivenFanin::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_unknown_nodes_fanin_off(cout);
	return Command::NORMAL;
}
}

void
UnknownUndrivenFanin::usage(ostream& o) {
	o << name <<
	" -- list X nodes with no fanin, with fanout (channels counted)."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/no-unknown-undriven-fanin.texi
@deffn Command no-unknown-undriven-fanin
Assert that there are no nodes at X with fanins and are being undriven.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoUnknownUndrivenFanin,
	"no-unknown-undriven-fanin", info, 
	"assert that no undriven input nodes with fanin are X")
int
NoUnknownUndrivenFanin::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	vector<node_index_type> nodes;
	s.unknown_nodes_fanin_off(nodes);
	if (nodes.empty()) {
		return Command::NORMAL;
	} else {
		cout << "Assertion failed: there are input nodes at X that have fanin but are undriven" << endl;
		s.print_nodes(cout, nodes, false, "\n") << std::flush;
		return Command::FATAL;
	}
}
}

void
NoUnknownUndrivenFanin::usage(ostream& o) {
	o << name << " -- " << brief << endl;
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
@texinfo cmd/no-unknown-outputs.texi
@deffn Command no-unknown-outputs
Assert that there are no outputs nodes at X.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoUnknownOutputs,
	"no-unknown-outputs", info, 
	"assert that no output nodes are X")
int
NoUnknownOutputs::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	vector<node_index_type> nodes;
	s.output_unknown_nodes(nodes);
	if (nodes.empty()) {
		return Command::NORMAL;
	} else {
		cout << "Assertion failed: there are output nodes at X" << endl;
		s.print_nodes(cout, nodes, false, "\n") << std::flush;
		return Command::FATAL;
	}
}
}

void
NoUnknownOutputs::usage(ostream& o) {
	o << name << " -- " << brief << endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/unknown-fanout.texi
@deffn Command unknown-fanout
Print all nodes with value X that have fanouts.
Connections to channel sinks and sources can counts as outputs (fake fanout).
@end deffn
@end texinfo
***/

DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnknownFanout,
	"unknown-fanout", info, 
	"list all X nodes with fanout")

int
UnknownFanout::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_unknown_nodes_fanout(cout);
	return Command::NORMAL;
}
}

void
UnknownFanout::usage(ostream& o) {
	o << name << " -- list X nodes with fanout (channels counted)."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/no-unknown-fanout.texi
@deffn Command no-unknown-fanout
Assert that there are no nodes with fanout at X.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoUnknownFanout,
	"no-unknown-fanout", info, 
	"assert that no nodes with fanout are X")
int
NoUnknownFanout::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	vector<node_index_type> nodes;
	s.unknown_nodes_fanout(nodes);
	if (nodes.empty()) {
		return Command::NORMAL;
	} else {
		cout << "Assertion failed: there are nodes with fanout at X" << endl;
		s.print_nodes(cout, nodes, false, "\n") << std::flush;
		return Command::FATAL;
	}
}
}

void
NoUnknownFanout::usage(ostream& o) {
	o << name << " -- " << brief << endl;
}


//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/fanin.texi
@deffn Command fanin node
Print all production rules that can fire @var{NODE}.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Fanin, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Fanin, "fanin", info, 
	"print rules that influence a node")

static
int
default_print_nodeinfo_main(const State& s, const string_list& a, 
		ostream& (State::*memfn)(ostream&, const node_index_type,
			const bool) const,
		const char* msg,
		const bool v, 
		void (usage)(ostream&)) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
#endif
		// const State::node_type& n(s.get_node(ni));
		cout << msg << " `" << 
			nonempty_abs_dir(objname) << "\':" << endl;
		(s.*memfn)(cout, ni, v);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
}
}

int
Fanin::main(State& s, const string_list& a) {
	return default_print_nodeinfo_main(s, a, &State::dump_node_fanin,
		"Fanins of node", false, usage);
}

void
Fanin::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << "print all rules and expressions that can affect this node"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/fanin-get.texi
@deffn Command fanin-get node
Print all production rules that can fire @var{NODE}.  
Also prints current values of all expression literals as 'node:val'
and subexpressions as '(expr)<val>'.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(FaninGet, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(FaninGet, "fanin-get", info, 
	"print rules that influence a node, with values")

int
FaninGet::main(State& s, const string_list& a) {
	return default_print_nodeinfo_main(s, a, &State::dump_node_fanin,
		"Fanins of node", true, usage);
}

void
FaninGet::usage(ostream& o) {
	o << name << " <node>" << endl;
	o <<
"print all rules and expressions that can affect this node, also shows\n"
"current values of expression literals.  Nodes are formatted 'node:val',\n"
"and expressions are formatted '(expr)<val>'."
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Fanout, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Fanout, "fanout", info, 
	"print rules that a node influences")

int
Fanout::main(State& s, const string_list& a) {
	return default_print_nodeinfo_main(s, a, &State::dump_node_fanout_rules,
		"Fanouts of node", false, usage);
}

void
Fanout::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << "print all rules affected by this node" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/fanout-get.texi
@deffn Command fanout-get node
Print all production rules that @var{NODE} participates in.  
Also prints current values of all expression literals as 'node:val'
and subexpressions as '(expr)<val>'.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(FanoutGet, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(FanoutGet, "fanout-get", info, 
	"print rules that a node influences, with values")

int
FanoutGet::main(State& s, const string_list& a) {
	return default_print_nodeinfo_main(s, a, &State::dump_node_fanout_rules,
		"Fanouts of node", true, usage);
}

void
FanoutGet::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << "print all rules affected by this node, " 
		<< endl
	<< "also shows current values of expression literals."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/feedback.texi
@deffn Command feedback node
@deffnx Command feedback-get node
Print all nodes that @var{NODE} drives and is also driven-by.
The takes the intersection of fanout nodes and fanin nodes.
The @command{-get} variant also prints the current node values.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Feedback, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(FeedbackGet, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Feedback, "feedback", info, 
	"print feedback nodes of the given node")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(FeedbackGet, "feedback-get", info, 
	"print feedback nodes of the given node")

int
Feedback::main(State& s, const string_list& a) {
	return default_print_nodeinfo_main(s, a, &State::dump_node_feedback,
		"Feedback of node", false, usage);
}

int
FeedbackGet::main(State& s, const string_list& a) {
	return default_print_nodeinfo_main(s, a, &State::dump_node_feedback,
		"Feedback of node", true, usage);
}

void
Feedback::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << brief << endl;
}

void
FeedbackGet::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/rings-mk.texi
@deffn Command rings-mk node
@deffnx Command rings-mk-get node
Print forced exclusive high/low rings of which @var{node} is a member.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(RingsMk, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(RingsMkGet, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RingsMk, "rings-mk", info, 
	"print forced exclusive rings of which a node is a member")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RingsMkGet, "rings-mk-get", info, 
	"print forced exclusive rings of node, with value")

static
int
default_print_node_rings(const State& s, const string_list& a, 
	const bool verbose,
	ostream& (State::*memfn)(ostream&,
		const node_index_type, const bool) const, 
	void (usage)(ostream&)) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		(s.*memfn)(cout, ni, verbose);
		return Command::NORMAL;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

int
RingsMk::main(State& s, const string_list& a) {
	return default_print_node_rings(s, a, false,
		&State::dump_node_mk_excl_rings, usage);
}

int
RingsMkGet::main(State& s, const string_list& a) {
	return default_print_node_rings(s, a, true,
		&State::dump_node_mk_excl_rings, usage);
}

void
RingsMk::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << "print all forced-exclusive rings of which this node is a member"
		<< endl;
}
void
RingsMkGet::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << "print all forced-exclusive rings of which this node is a member, "
	"along with current value of each node."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/allrings-mk.texi
@deffn Command allrings-mk
@deffnx Command allrings-mk-get
Print all forced exclusive high/low rings.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllRingsMk, "allrings-mk", info, 
	"dump all forced exclusive hi/lo rings")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllRingsMkGet, "allrings-mk-get", info, 
	"dump all forced exclusive hi/lo rings, with value")

int
AllRingsMk::main(State& s, const string_list& a) {
	return default_print_0(s, a, &State::dump_mk_excl_rings, false, usage);
}

int
AllRingsMkGet::main(State& s, const string_list& a) {
	return default_print_0(s, a, &State::dump_mk_excl_rings, true, usage);
}

void
AllRingsMk::usage(ostream& o) {
	o << name << endl;
	o << "print all forced-exclusive rings" << endl;
}

void
AllRingsMkGet::usage(ostream& o) {
	o << name << endl;
	o << "print all forced-exclusive rings, with values" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/rings-chk.texi
@deffn Command rings-chk node
@deffnx Command rings-chk-get node
Print all checked exclusive rings of which @var{node} is a member.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(RingsChk, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(RingsChkGet, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RingsChk, "rings-chk", info, 
	"print checked exclusive rings of which a node is a member")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(RingsChkGet, "rings-chk-get", info, 
	"print checked exclusive rings with node, with value")

int
RingsChk::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		s.dump_node_check_excl_rings(cout, ni, false);
		return Command::NORMAL;
	} else {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
}
}

int
RingsChkGet::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		s.dump_node_check_excl_rings(cout, ni, true);
		return Command::NORMAL;
	} else {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
}
}

void
RingsChk::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << "print all checked-exclusive rings of which this node is a member"
		<< endl;
}

void
RingsChkGet::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << "print all checked-exclusive rings with this node, and value"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/allrings-chk.texi
@deffn Command allrings-chk
@deffnx Command allrings-chk-get
Print all checked exclusive rings of nodes.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllRingsChk, "allrings-chk", info, 
	"dump all checked exclusive hi/lo rings")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AllRingsChkGet, "allrings-chk-get", info, 
	"dump all checked exclusive hi/lo rings, with values")

int
AllRingsChk::main(State& s, const string_list& a) {
	return default_print_0(s, a, &State::dump_check_excl_rings,
		false, usage);
}

int
AllRingsChkGet::main(State& s, const string_list& a) {
	return default_print_0(s, a, &State::dump_check_excl_rings,
		true, usage);
}

void
AllRingsChk::usage(ostream& o) {
	o << name << endl;
	o << "print all checked-exclusive rings" << endl;
}

void
AllRingsChkGet::usage(ostream& o) {
	o << name << endl;
	o << "print all checked-exclusive rings, with node values" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// will category conflict with command?
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(Info, "info", info, 
//	"print information about a node/vector")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/attributes.texi
@deffn Command attributes node
Prints the list of attributes attached to the named @var{node}.
@end deffn
@end texinfo
TODO: extend to process attributes eventually
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Attributes, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Attributes, "attributes", info, 
	"prints node attributes")

int
Attributes::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		// we have ni = the canonically allocated index of the bool node
		// just look it up in the node_pool
		s.dump_node_canonical_name(cout << "Node ", ni)
			<< " attributes:";
		s.get_node(ni).dump_attributes(cout) << endl;
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
}
}

void
Attributes::usage(ostream& o) {
	o << name << " <node>" << endl;
	o << brief << endl;
}

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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Assert, instance_completer)
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
	const string& objname(*++a.begin());
	const string& _val(a.back());	// node value
		// valid values are 0, 1, 2(X)
	const value_enum val = node_type::string_to_value(_val);
	if (!node_type::is_valid_value(val)) {
		cerr << "Invalid logic value: " << _val << endl;
		return Command::SYNTAX;
	}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		const node_type& n(s.get_node(ni));
		const value_enum actual = n.current_value();
		if (actual != val) {
			const error_policy_enum e(s.get_assert_fail_policy());
			if (e != ERROR_IGNORE) {
			cout << "assert failed: expecting node `" << 
				nonempty_abs_dir(objname) <<
				"\' at " <<
				node_type::value_to_char[size_t(val)] <<
				", but got ";
			n.dump_value(cout) << "." << endl;
			}	// yes, actually allow suppression
			return error_policy_to_status(e);
			// stops on first error
		} else if (s.confirm_asserts()) {
			cout << "node `" << nonempty_abs_dir(objname)
				<< "\' is " <<
				node_type::value_to_char[size_t(val)] <<
				", as expected." << endl;
		}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(AssertN, instance_completer)
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
	const string& objname(*++a.begin());
	const string& _val(a.back());	// node value
	// valid values are 0, 1, 2(X)
	const value_enum val = node_type::string_to_value(_val);
	if (!node_type::is_valid_value(val)) {
		cerr << "Invalid logic value: " << _val << endl;
		return Command::SYNTAX;
	}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		const node_type& n(s.get_node(ni));
		const value_enum actual = n.current_value();
		if (actual == val) {
			const error_policy_enum e(s.get_assert_fail_policy());
			if (e != ERROR_IGNORE) {
			cout << "assert failed: expecting node `" << 
				nonempty_abs_dir(objname) <<
				"\' not at " <<
				node_type::value_to_char[size_t(val)] <<
				", but got ";
			n.dump_value(cout) << "." << endl;
			}
			return error_policy_to_status(e);
			// stops on first error
		} else if (s.confirm_asserts()) {
			cout << "node `" << nonempty_abs_dir(objname)
				<< "\' is not " <<
				node_type::value_to_char[size_t(val)] <<
				", as expected." << endl;
		}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
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
@texinfo cmd/assert-driven.texi
@deffn Command assert node value
Error out if @var{node} is driven with strength @var{value}.  
The error-handling policy can actually be determined by
the @command{assert-fail} command.  
By default, such errors are fatal and cause the simulator to terminate
upon first error.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(AssertDriven, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AssertDriven, "assert-driven", info, 
	"error if drive-state of node does not match expected")

/**
	Checks expected value of node against actual value.  
	\return FATAL on assertion failure.  
 */
int
AssertDriven::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(*++a.begin());
	const string& _val(a.back());	// node value
	// valid values are 0, 1, 2(X)
	const pull_enum val = node_type::string_to_pull(_val);
	if (!node_type::is_valid_pull(val)) {
		cerr << "Invalid pull value: " << _val << endl;
		return Command::SYNTAX;
	}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		const node_type& n(s.get_node(ni));
		const pull_enum actual = n.drive_state();
		if (actual != val) {
			const error_policy_enum e(s.get_assert_fail_policy());
			if (e != ERROR_IGNORE) {
			cout << "assert failed: expecting node `" << 
				nonempty_abs_dir(objname) <<
				"\' with drive-state " <<
				node_type::value_to_char[size_t(val)] <<
				", but got ";
			cout << node_type::value_to_char[size_t(actual)]
				<< "." << endl;
			}	// yes, actually allow suppression
			return error_policy_to_status(e);
		} else if (s.confirm_asserts()) {
			cout << "node `" << nonempty_abs_dir(objname)
				<< "\' has drive-state " <<
				node_type::value_to_char[size_t(val)] <<
				", as expected." << endl;
		}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
}
}

void
AssertDriven::usage(ostream& o) {
	o << name << " <node> <value>" << endl;
	o << "signal an error and halt simulation if node is not driven with the strength asserted."
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(AssertPending, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AssertPending, "assert-pending", info, 
	"error if node does not have event in queue")


int
AssertPending::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(*++a.begin());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		const node_type& n(s.get_node(ni));
		if (!n.pending_event()) {
			const error_policy_enum e(s.get_assert_fail_policy());
			if (e != ERROR_IGNORE) {
			cout <<
			"assert failed: expecting pending event on node `"
				<< nonempty_abs_dir(objname)
				<< "\', but none found." << endl;
			}
			return error_policy_to_status(e);
		} else if (s.confirm_asserts()) {
			cout << "node `" << nonempty_abs_dir(objname) <<
				"\' has a pending event, as expected." << endl;
		}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(AssertNPending, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AssertNPending, "assertn-pending", info, 
	"error if node does have event in queue")

int
AssertNPending::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(*++a.begin());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " <<
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
		const node_type& n(s.get_node(ni));
		if (n.pending_event()) {
			const error_policy_enum e(s.get_assert_fail_policy());
			if (e != ERROR_IGNORE) {
			cout <<
			"assert failed: expecting no pending event on node `"
				<< nonempty_abs_dir(objname)
				<< "\', but found one." << endl;
			}
			return error_policy_to_status(e);
		} else if (s.confirm_asserts()) {
			cout << "node `" << nonempty_abs_dir(objname) <<
				"\' has no pending event, as expected." << endl;
		}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(AssertQueue, info)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(AssertNQueue, info)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/time.texi
@deffn Command time
What time is it (in the simulator)?
@end deffn
@end texinfo
***/
typedef	Time<State>				Time;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Time, info)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Confirm, view)
typedef	NoConfirm<State>			NoConfirm;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(NoConfirm, view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/backtrace.texi
@deffn Command backtrace node [val]
Trace backwards through a history of last-arriving transitions on
node @var{node}, until a cycle is found.  
If @var{val} is omitted, the current value of the node is assumed.  
Useful for tracking down causes of instabilities, 
and identifying critical paths and cycle times.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(BackTrace, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(BackTrace, "backtrace", info, 
	"trace backwards partial event causality history")

int
BackTrace::main(State& s, const string_list& a) {
const size_t sz = a.size();
if (sz != 2 && sz != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator ai(++a.begin());
	const string& objname(*ai);
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		const node_type& n(s.get_node(ni));
		value_enum val = n.current_value();	// default current val
		++ai;
	if (ai != a.end()) {
		// valid values are 0, 1, 2(X)
		val = node_type::string_to_value(*ai);
		if (!node_type::is_valid_value(val)) {
			cerr << "Invalid logic value: " << *ai << endl;
			return Command::BADARG;
		}
	}
		s.backtrace_node(cout, ni, val);
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyX, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyXVerbose, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyXN, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyXNVerbose, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyX1, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyX1Verbose, instance_completer)
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Why, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyVerbose, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Why1, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Why1Verbose, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyN, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyNVerbose, instance_completer)
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

PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyNot, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyNotVerbose, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyNot1, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyNot1Verbose, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyNotN, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(WhyNotNVerbose, instance_completer)
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Watch, instance_completer)
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
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
		const node_index_type ni =
			parse_node_to_index(objname, s.get_module());
#endif
		if (!ni) {
			cerr << "No such node found: " << 
				nonempty_abs_dir(objname) << endl;
			badarg = true;
		} else {
			s.watch_node(ni);
		}
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
		}	// end for each node
#endif
	}	// end for each reference
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(UnWatch, instance_completer)
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
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
		const node_index_type ni =
			parse_node_to_index(objname, s.get_module());
		if (!ni) {
			cerr << "No such node found: " << 
				nonempty_abs_dir(objname) << endl;
			badarg = true;
		}
#endif
			s.unwatch_node(ni);
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
		}	// end for each node
#endif
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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(WatchQueue, view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	NoWatchQueue<State>			NoWatchQueue;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(NoWatchQueue, view)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(WatchAllQueue, view)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
typedef	NoWatchAllQueue<State>			NoWatchAllQueue;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(NoWatchAllQueue, view)

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
@texinfo cmd/tcount.texi
@deffn Command tcount node
@command{tcount} shows the number of non-X transitions that have
ever occurred on @var{node}.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(TCount, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(TCount, "tcount", info, 
	"show transition count on node")

int
TCount::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	NODE_FOR_EACH(objname) {
		const node_index_type& ni(*niter);
#else
	const node_index_type ni =
		parse_node_to_index(objname, s.get_module());
	if (!ni) {
		cerr << "No such node found: " << 
			nonempty_abs_dir(objname) << endl;
		return Command::BADARG;
	}
#endif
	cout << s.get_node_canonical_name(ni) << " : [" <<
		s.get_node(ni).tcount << " T]" << endl;
#if PRSIM_NODE_AGGREGATE_ARGUMENTS
	}	// end for each node
#endif
	return Command::NORMAL;
}
}

void
TCount::usage(ostream& o) {
	o << name << endl;
	o << "report transition counts of node" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/tcounts.texi
@deffn Command tcounts
@deffnx Command notcounts
@command{tcounts} displays transition counts on nodes as they change value.  
@command{notcounts} hides transition count information.  
Only transitions to 0 or 1 are counted; transitions to X are not counted.
Transitions are always counted, just not always displayed.
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
@deffn Command timing [mode] [args]
Modes:
@samp{uniform} @var{delay} applies the same delay to @emph{all} rules.  
@t{uniform} is useful for getting quick transition counts.  
@samp{random} gives every event a different randomly assigned delay.  
@t{random} is most useful for detecting non-QDI logic violations.  
@samp{after} applies a different delay for each rule, as determined
by the @t{after} PRS rule attribute.  
@t{binary} and @t{bounded} modes are most useful for testing
that certain timing assumptions (path races) are necessary.  

The @t{after_min} and @t{after_max} rule attributes only have any effect
in random mode or on nodes marked @t{always_random}.  
In random-mode, @t{after_min} specifies a lower bound on delay, 
and @t{after_max} specifies an upper bound on delay.  
When no upper bound is specified, the delay distribution is an 
exponential variate; when an upper bound is specified, a delay 
is generated with uniform distribution between the bounds.  
If only a lower bound is specified, its value is added to the 
exponentially distribtued random delay.  

Timing @option{random} also takes additional optional arguments for 
default min and max delays for @emph{unspecified rules};
user-written values from the source will always take precedence.
A max delay value of 0 is interpreted as being unbounded.
@itemize
@item @samp{timing random} preserves the default min/max delays
@item @samp{timing random :} will clear the default min/max delays
@item @samp{timing random X:} sets the default min delay
@item @samp{timing random :Y} sets the default max delay
@item @samp{timing random X:Y} sets the default min and max delays
@end itemize

Timing @option{binary} randomly chooses a min or max delay value
with a specified probability, like a skewed coin-flip.
Specifying both min and max values is required @t{X:Y}.
This mode completely @emph{disregards} any user-specified delay attributes
in the source, including delay @option{after=0}.

Timing @option{bounded} randomly chooses a min or max delay value
with a specified probability, like a skewed coin-flip.
Specifying both min and max values is required @t{X:Y}.
Unlike @t{binary} mode, @t{bounded} delays are overridden by user-specified 
delay attributes @option{after_min} and @option{after_max}.

@itemize
@item @samp{timing binary 10:90 0.5}
@item @samp{timing binary 10:50 0.95}
@item @samp{timing bounded 10:50 0.8}
@end itemize
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
The seed value is reset to 0 0 0 with the @command{reset} command, 
but not with the @command{initialize} command.
@end deffn
@end texinfo
***/
typedef	Seed48<State>			Seed48;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Seed48, modes)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if CACHE_GLOBAL_FOOTPRINT_FRAMES
/***
@texinfo cmd/frame-cache-half-life.texi
@deffn Command frame-cache-half-life [int]
Sets the period (in event count) at which the internal cache of
footprint frames (lookup-tables) should be aged.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(FrameCacheHalfLife,
	"frame-cache-half-life", modes, "lookup table cache age interval")

int
FrameCacheHalfLife::main(State& s, const string_list& a) {
switch (a.size()) {
case 0: cout << "frame-cache-half-life: " << s.cache_half_life << endl;
	cout << "frame-cache-countdown: " << s.get_cache_countdown() << endl;
	return Command::NORMAL;
case 1: {
	size_t x;
	if (string_to_num(a.front(), x)) {
		cerr << "Invalid interval value." << endl;
		return Command::BADARG;
	}
	s.cache_half_life = x;
	return Command::NORMAL;
}
default:
	usage(cerr << "usage: ");
	return Command::BADARG;
}
}

void
FrameCacheHalfLife::usage(ostream& o) {
	o << name << " [interval]" << endl;
	o <<
"Sets the number of events at which the frame cache should age (half-life)\n"
"by decaying all entry weights by half, and evict nodes that reach 0.\n"
"When no argument is given, just reports the current half-life and counter."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/frame-cache-halve.texi
@deffn Command frame-cache-halve
Manually age the cache, as if a half-life period elapsed.
One typically never needs to do this unless the memory usage
has gone out of hand.
The output reports the total amount of weight lost in the cache, 
which is meaningless unless you know how the cache works.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(FrameCacheHalve,
	"frame-cache-halve", modes, "downsize lookup table cache")

int
FrameCacheHalve::main(State& s, const string_list& a) {
if (a.size() > 1) {
	usage(cerr << "usage: ");
	return Command::BADARG;
} else {
	const global_entry_context::index_frame_cache_type&
		c(s.get_frame_cache());
	const size_t weight = c.weight();
	s.halve_cache();
	const size_t aft_weight = c.weight();
	cout << "frame cache weight reduced from " << weight << " to "
		<< aft_weight << endl;
	return Command::NORMAL;
}
}

void
FrameCacheHalve::usage(ostream& o) {
	o << name << endl;
	o <<
"Manually age the contents of the frame cache by one half-life."
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/frame-cache-dump.texi
@deffn Command frame-cache-dump
Print the contents of the global footprint-frame cache.
Really only intended for memory diagnostics.
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(FrameCacheDump,
	"frame-cache-dump", modes, "examine lookup table cache contents")

int
FrameCacheDump::main(State& s, const string_list& a) {
if (a.size() > 1) {
	usage(cerr << "usage: ");
	return Command::BADARG;
} else {
	s.dump_frame_cache(cout);
	return Command::NORMAL;
}
}

void
FrameCacheDump::usage(ostream& o) {
	o << name << endl;
	o << brief << endl;
}
#endif	// CACHE_GLOBAL_FOOTPRINT_FRAMES

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
break_options[] = "[ignore|warn|notify|break|interactive|fatal]";

static const char
break_descriptions[] = 
"\tignore: silently ignores violation\n"
"\twarn: print warning without halting\n"
"\tnotify: (same as warn)\n"
"\tbreak: notify and halt\n"
"\tinteractive: halt and start an interactive sub-shell (exit resumes)\n"
"\tfatal: immediately terminate simulator with nonzero exit status";

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
		error_policy_string(s.get_##func_name##_policy()) 	\
		<< endl;						\
	return Command::NORMAL;						\
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/keeper-check-fail.texi
@deffn Command keeper-check-fail [mode]
Set the error-handling policy for when an exclusion check fails.  
@end deffn
@end texinfo
***/
DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS(KeeperCheckFail, 
	"keeper-check-fail", 
	"set error-handling for missing keeper failures",
	"Set error-handling policy on missing keepers.",
	keeper_check_fail)

#undef	DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Rules, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(RulesVerbose, instance_completer)
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
		parse_process_to_index(a.back(), s.get_module()).index;
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
/***
@texinfo cmd/invariants.texi
@deffn Command invariants ref
@deffnx Command invariants-verbose ref
Print all invariants belonging to the named process @var{ref}, 
or all invariants that the node @var{ref} participates in.  
'@t{.}' can be used to refer to the top-level process.
The @t{-verbose} variant prints the state of each node and expression
appearing in each invariant.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Invariants, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(InvariantsVerbose, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Invariants, "invariants", info, 
	"print invariants belonging to a process or node")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(InvariantsVerbose, 
	"invariants-verbose", info, 
	"print invariants belonging to process with values")

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
	"print ALL invariants with values")

static
int
invariants_main(const State& s, const string_list& a, const bool verbose, 
		void (usage)(ostream&)) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& iname(a.back());
	const global_indexed_reference gref = (iname == ".") ?
			global_indexed_reference(META_TYPE_PROCESS, 0) :
			parse_global_reference(iname, s.get_module());
if (gref.first == META_TYPE_PROCESS) {
	const process_index_type& pid = gref.second;
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
} else if (gref.first == META_TYPE_BOOL) {
	const node_index_type& ni = gref.second;
	INVARIANT(ni);
	cout << "Invariants involving node `" << iname << "\':" << endl;
	s.dump_node_fanout(cout, ni, false, true, verbose);
	return Command::NORMAL;
} else if (gref.first == META_TYPE_NONE) {
	cerr << "Error: invalid reference: " << iname << endl;
	return Command::BADARG;
} else {
	cerr << "Error: referenced instance is neither a bool nor a process."
		<< endl;
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
@t{fatal} causes the simulation to exit immediate with non-zero exit status, 
which is useful for non-interactive batch testing.  
@strong{Caution:} @t{fatal} also causes the following diagnostic conditions to 
exit fatally:
	@i{invariant-fail},
	@i{invariant-unknown},
	@i{assert-fail},
	@i{channel-expect-fail},
	@i{excl-check-fail}

@multitable {channel-expect-fail} {@i{default}} {@t{reset}} {@t{run}} {@t{paranoid}} {@t{fatal}}
@headitem policy @tab @i{default} @tab @t{reset} @tab @t{run} @tab @t{paranoid} @tab @t{fatal}
@item interference
@tab break @tab break @tab break @tab break @tab fatal
@item weak-interference
@tab warn @tab ignore @tab warn @tab break @tab fatal
@item unstable
@tab break @tab break @tab break @tab break @tab fatal
@item weak-unstable
@tab warn @tab warn @tab warn @tab break @tab fatal
@item assert-fail
@tab fatal @tab - @tab - @tab - @tab fatal
@item excl-check-fail
@tab fatal @tab - @tab - @tab - @tab fatal
@item channel-expect-fail
@tab fatal @tab - @tab - @tab - @tab fatal
@item invariant-fail
@tab break @tab - @tab - @tab - @tab fatal
@item invariant-unknown
@tab warn @tab - @tab - @tab - @tab fatal
@end multitable
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
	static const string fatal("fatal");
	const string& m(a.back());
	if (m == reset) {
		s.set_mode_reset();
	} else if (m == run) {
		s.set_mode_run();
	} else if (m == paranoid) {
		s.set_mode_breakall();
	} else if (m == fatal) {
		s.set_mode_fatal();
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
	o << "mode [reset|run|paranoid|fatal]\n"
"\t\'reset\' disables weak-interference warnings, useful during initialization\n"
"\t\'run\' (default) enables weak-interference warnings\n"
"\t\'paranoid\' also break on weak-instability and weak-interference\n"
"\t\'fatal\' causes the simulator to exit with non-zero exit status" << endl;
	o <<
"By default, instabilities and interferences still cause simulations to halt,\n"
"while weak-instabilities trigger warnings." << endl;
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
@item @var{type} is a regular expression of the form @t{[ae]?[nv]?:[01]?}, where
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
@item @var{rails} (@t{[~]rname:radix}) is the name and size of each bundle's 
	data rails, @var{rname} is the name of the data rail of the channel.
	@var{radix} is the number of data rails per bundle (N in Mx1ofN).
	Use @var{radix} 0 to indicate that rail is not an array (1of1).
	If @var{rails} is prefixed with a @t{~}, then the data rails
	will be interpreted as active low.  
@end itemize
For example, @t{channel e:0 :0 d:4} is a conventional e1of4 channel with
active-high data rails @t{d[0..3]}, 
and an active-low acknowledge (enable) reset to 0, no bundles.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(Channel, instance_completer)
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
	bool have_ack = false;
	bool ack_sense = false;
	bool ack_init = false;
	bool have_valid = false;
	bool valid_sense = false;
	bool data_sense = false;
	try {
		// parse ev-type
		string::const_iterator si(ev_type.begin()), se(ev_type.end());
		// const size_t evl = ev_type.length();
		// if (evl < 3 || evl > 4) { THROW_EXIT; }
		bool have_colon = false;
		for ( ; si!=se && !have_colon; ++si) {
		switch (tolower(*si)) {
		case 'a': ack_sense = true; have_ack = true; break;
		case 'e': ack_sense = false; have_ack = true; break;
		case 'n': valid_sense = false; have_valid = true; break;
		case 'v': valid_sense = true; have_valid = true; break;
		case ':': have_colon = true; break;
		}	// end switch
		}	// end for
		if (have_ack) {
		for ( ; si!=se; ++si) {
		switch (*si) {
		case '0': ack_init = false; break;
		case '1': ack_init = true; break;
		default: THROW_EXIT;
		}	// end switch
		}	// end for
		}	// end if
		if (!have_colon) { THROW_EXIT; }
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
		data_sense = (rail[0] == '~');		// active low
#endif
		const string::const_iterator b(rail.begin());
		rail_name.assign(b +size_t(data_sense), b+c);
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
			rail_name, rail_size, data_sense, 
			have_ack, ack_sense, ack_init, 
			have_valid, valid_sense)) {
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
	"\tPrefix the data rail name with ~ to make it active-low.\n"
"\'radix\' is the number of data rails per bundle (N in Mx1ofN).\n"
	"\tUse radix 0 to indicate that rails are not an array (1of1).\n"
"For example, \"channel e:0 :0 d:4\", is a conventional e1of4 channel with\n"
"data rails d[0..3], and an active-low acknowledge reset to 0, no bundles."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-ledr.texi
@deffn Command channel-ledr name ack:init bundles:num data:init repeat:init
Registers a level-encoded dual-rail (LEDR) channel.
LEDR channels do not follow a return-to-null protocol;
there is exactly one transition per iteration on the forward path.
Currently, LEDR channels only encode 1-bit of information per channel.
The data rail represents the logic level, and the repeat rail is
toggled to communicate another token with the same value.  
The channel acknowledge (if present) also fires onces per handshake.
Together, they are used for 2-phase protocols.  
The @var{name} of the channel should match that of an instance 
(process or channel) in the source file.  
@itemize
@item @var{name} is the name of the new channel in the simulator's namespace
@item @var{ack} is a regular expression of the form @t{id:[01]}, where
@itemize
	@item if an identifier @t{id} is given before the @t{:},
	it is interpreted as the name of an acknowledge signal.  
	@item The value after the @t{:} (required)
	is interpreted as the initial state of the acknowledge wire.
	There is no need to express whether the acknowledge is active-high
	or active-low.
	@item @t{:} with no name represents a channel with no acknowledge.
@end itemize
@item @var{bundles} is the name of the data-repeat bundle followed by
	@t{:} and the number of bundles.  Pass just @t{:0} to indicate
	that there is only one data-repeat pair.  
@item @var{data} is the name of the data rail, interpreted with active-high
	logic levels.  The @var{init} value specifies the initial
	value of the data rail on an empty channel.
@item @var{repeat} is the name of the repeat rail.  @var{init} specifies the
	initial value of the repeat rail on an empty channel.
@end itemize
The initial values of the three rails determines the ``empty-parity'' of the
channel, the parity of the rails when the channel is in its empty state.
The initial values are used when the channels are connected up to 
driving environments such as sources and sinks.  
For bundled channels, the initial values of data and repeat apply
to all bundles.  
@example
@t{channel-ledr NAME e:0 :0 d:0 r:0}
@t{channel-ledr NAME e:1 :0 d:0 r:0}
@end example
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelLEDR, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelLEDR, "channel-ledr", 
	channels, "declare a handshake channel from a group of nodes")

int
ChannelLEDR::main(State& s, const string_list& a) {
if (a.size() != 6) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	const string& chan_name(*i);
	const string& ack(*++i);
	const string& bundle(*++i);
	const string& data(*++i);
	const string& repeat(*++i);
	// could confirm that 'name' exists as a process/channel/datatype?
	bool ack_init = false;
	bool data_init = false;
	bool data_sense = false;
	bool repeat_init = false;
	size_t num_bundles = 0;
	string ack_name, bundle_name, data_name, repeat_name;
	{
		// parse ack
		string_list tmp;
		tokenize_char(ack, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: ack must be of the form id:init."
				<< endl;
			return Command::SYNTAX;
		}
		if (tmp.front().length()) {	// if we have ack (name)...
		ack_name = tmp.front();
		if (string_to_num(tmp.back(), ack_init)) {
			cerr << "Error: parsing initial value of ack." << endl;
			return Command::SYNTAX;
		}
		}	// else skip
	}{
		// parse bundle
		string_list tmp;
		tokenize_char(bundle, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: bundle must be of the form id:num."
				<< endl;
			return Command::SYNTAX;
		}
		bundle_name = tmp.front();
		if (string_to_num(tmp.back(), num_bundles)) {
			cerr << "Error: parsing number of bundles." << endl;
			return Command::SYNTAX;
		}
	}{
		// parse data
		size_t c = data.find(':');
		if (c == string::npos || (c == data.length() -1)) {
			THROW_EXIT;
		}
		string_list tmp;
		tokenize_char(data, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: data must be of the form [~]id:init."
				<< endl;
			return Command::SYNTAX;
		}
		data_name = tmp.front();
		if (data_name.length()) {
#if PRSIM_CHANNEL_RAILS_INVERTED
		data_sense = (data_name[0] == '~');		// active low
#endif
		const string::const_iterator b(data_name.begin());
		data_name.assign(b +size_t(data_sense), b+c);
		if (string_to_num(tmp.back(), data_init)) {
			cerr << "Error: parsing initial value of data." << endl;
			return Command::SYNTAX;
		}
		}	// else is data-less, which we now support
	}{
		// parse repeat
		string_list tmp;
		tokenize_char(repeat, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: repeat must be of the form id:init."
				<< endl;
			return Command::SYNTAX;
		}
		repeat_name = tmp.front();
		if (string_to_num(tmp.back(), repeat_init)) {
			cerr << "Error: parsing initial value of repeat."
				<< endl;
			return Command::SYNTAX;
		}
	}
	channel_manager& cm(s.get_channel_manager());
	if (cm.new_channel_ledr(s, chan_name, ack_name, ack_init, 
			bundle_name, num_bundles, 
			data_name, data_init, data_sense, 
			repeat_name, repeat_init)) {
		return Command::BADARG;
	}
	return Command::NORMAL;
}
}

void
ChannelLEDR::usage(ostream& o) {
	o << name <<
" <name> <ack:init> <bundle:num> <data:init> <repeat:init>"
	<< endl;
	o <<
"Registers a named LEDR channel in a separate namespace in \n"
"the simulator, typically used to drive or log the environment.\n"
"\'name\' is the name of the new channel in the simulator's namespace\n"
"\'ack:init\' : ack is the name of the acknowledge wire, init is the initial\n"
	"\tvalue of this wire in an empty channel.\n"
	"\tIf the name is omitted, then the channel is acknowledgeless.\n"
"\'bundle:num\' : the name of data-repeat bundle array, if num is > 0\n"
	"\tor just :0 if channel contains no bundles.\n"
"\'data:init\' : data is the name of the data rail of the channel.\n"
	"\tinit is the initial value of this rail in an empty channel.\n"
"\'repeat:init\' : repeat is the name of the repeat rail of the channel.\n"
	"\tinit is the inital value of the repeat rail.\n"
"The XOR of the initial values of the rails defines the \'empty-parity\'"
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_BUNDLED_DATA
/***
@texinfo cmd/channel-bd-2p.texi
@deffn Command channel-bd-2p name ack:init req:init data:width
Registers a bundled-data (BD) channel, which consists of a bus, 
request rail, and acknowledge rail.
The request and acknowledge perform a two-phase handshake on every token;
these signals toggle once per handshake.
The @var{name} of the channel should match that of an instance 
(process or channel) in the source file.  
@itemize
@item @var{name} is the name of the new channel in the simulator's namespace
@item @var{ack} is a regular expression of the form @t{id:[01]}, where
@itemize
	@item @t{id} is the name of the acknowledge signal.  
	@item The value after the @t{:} (required)
	is interpreted as the initial state of the acknowledge wire, 
	if driven by sink.
@end itemize
@item @var{req} is the name of the request signal.
	The value given is the initial value of the request signal,
	if driven by a source.
	Together the XOR of the initial values of the acknowledge and request
	defines the @emph{empty-parity}.  
@item @var{data} is the name of the data rail(s), interpreted with active-high
	logic levels (prefix with @t{~} to make active-low).  
	The @var{num} value specifies the number of wires (bus width).
	If the channel is data-less (handshake only), then omit the 
	data rail name and just write @t{:}.
@end itemize
@example
@t{channel-bd-2p NAME e:1 v:1 d:0} -- this names the ack @t{e} and the 
	request @t{v}, and data is a single-wire bundled-data channel.
@t{channel-bd-2p NAME a:1 r:0 d:8} -- this names the ack @t{a} and the
	request @t{r}, and data is a 8-bit bundled-data channel.
@end example
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelBD2P, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelBD2P, "channel-bd-2p", 
	channels, "declare a bundled-data channel")

int
ChannelBD2P::main(State& s, const string_list& a) {
if (a.size() != 5) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	const string& chan_name(*i);
	const string& ack(*++i);
	const string& req(*++i);
	const string& data(*++i);
	// could confirm that 'name' exists as a process/channel/datatype?
	bool ack_init = false;
	bool req_init = false;
	bool data_sense = false;
	size_t num_rails = 0;
	string ack_name, req_name, data_name;
	{
		// parse ack
		string_list tmp;
		tokenize_char(ack, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: ack must be of the form id:init."
				<< endl;
			return Command::SYNTAX;
		}
		if (tmp.front().length()) {	// if we have ack (name)...
		ack_name = tmp.front();
		if (string_to_num(tmp.back(), ack_init)) {
			cerr << "Error: parsing initial value of ack." << endl;
			return Command::SYNTAX;
		}
		}	// else skip
	}{
		// parse req
		string_list tmp;
		tokenize_char(req, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: request must be of the form id:init."
				<< endl;
			return Command::SYNTAX;
		}
		req_name = tmp.front();
		if (string_to_num(tmp.back(), req_init)) {
			cerr << "Error: parsing initial value of request."
				<< endl;
			return Command::SYNTAX;
		}
	}{
		// parse data
		size_t c = data.find(':');
		if (c == string::npos || (c == data.length() -1)) {
			THROW_EXIT;
		}
		string_list tmp;
		tokenize_char(data, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: data must be of the form [~]id:init."
				<< endl;
			return Command::SYNTAX;
		}
		data_name = tmp.front();
		if (data_name.length()) {
#if PRSIM_CHANNEL_RAILS_INVERTED
		data_sense = (data_name[0] == '~');		// active low
#endif
		const string::const_iterator b(data_name.begin());
		data_name.assign(b +size_t(data_sense), b+c);
		if (string_to_num(tmp.back(), num_rails)) {
			cerr << "Error: parsing bus width." << endl;
			return Command::SYNTAX;
		}
		}	// else is data-less, which is now supported
	}
	channel_manager& cm(s.get_channel_manager());
	if (cm.new_channel_bd2p(s, chan_name, 
			ack_name, ack_init, req_name, req_init,
			data_name, num_rails, data_sense)) {
		return Command::BADARG;
	}
	return Command::NORMAL;
}
}

void
ChannelBD2P::usage(ostream& o) {
	o << name << " <name> <ack:init> <req:init> <data:num>" << endl;
	o <<
"Registers a named bundled-data channel (2-phase) in a separate namespace in \n"
"the simulator, typically used to drive or log the environment.\n"
"\'name\' is the name of the new channel in the simulator's namespace\n"
"\'ack:init\' : ack is the name of the acknowledge wire, init is the initial\n"
	"\tvalue of this wire if driven by sink.\n"
"\'req:init\' : req is the name of the request rail, init is the initial\n"
	"\tvalue of this wire if driven by source.\n"
"\'data:num\' : data is the name of the data rail(s) of the channel.\n"
	"\tnum is the number of rails (bus width).  Pass :0 if data-less.\n"
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/***
@texinfo cmd/channel-bd-4p.texi
@deffn Command channel-bd-4p name ack:init req:init data:width
Registers a bundled-data (BD) channel, which consists of a bus, 
request rail, and acknowledge rail.
The request and acknowledge perform a four-phase handshake on every token;
these signals toggle twice per handshake.
The @var{name} of the channel should match that of an instance 
(process or channel) in the source file.  
@itemize
@item @var{name} is the name of the new channel in the simulator's namespace
@item @var{ack} is a regular expression of the form @t{id:[01]}, where
@itemize
	@item @t{id} is the name of the acknowledge signal.  
	@item The value after the @t{:} (required)
	is interpreted as the initial state of the acknowledge wire, 
	if driven by sink.
	@item @t{a} denotes an active-high acknowledge, and 
	@t{e} denotes an active-low acknowledge, (same as 1ofN channels).
@end itemize
@item @var{req} is the name of the request signal.
	The value given is the initial value of the request signal,
	if driven by a source -- THIS IS IGNORED FOR NOW, 
	source channels will always drive this to inactive on reset.
@item @var{data} is the name of the data rail(s), interpreted with active-high
	logic levels (prefix with @t{~} to make active-low).  
	The @var{num} value specifies the number of wires (bus width).
	If the channel is data-less (handshake only), then omit the 
	data rail name and just write @t{:}.
@end itemize
@example
@t{channel-bd-4p NAME e:0 v:1 d:0} -- this declares an active-low acknowledge,
	active-high request, single-wire bundled-data channel.
@t{channel-bd-4p NAME a:1 n:0 d:8} -- this declares an active-high acknowledge,
	active-low request, 8-bit bundled-data channel.
@end example
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelBD4P, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelBD4P, "channel-bd-4p", 
	channels, "declare a bundled-data channel")

int
ChannelBD4P::main(State& s, const string_list& a) {
if (a.size() != 5) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	const string& chan_name(*i);
	const string& ack(*++i);
	const string& req(*++i);
	const string& data(*++i);
	// could confirm that 'name' exists as a process/channel/datatype?
	bool ack_sense = false;
	bool ack_init = false;
	bool req_sense = false;
//	bool req_init = false;		// not supported yet
	bool data_sense = false;
	size_t num_rails = 0;
	string ack_name, req_name, data_name;
	{
		// parse ack
		string_list tmp;
		tokenize_char(ack, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: ack must be of the form id:init."
				<< endl;
			return Command::SYNTAX;
		}
		ack_name = tmp.front();
		if (ack_name.length()) {	// if we have ack (name)...
		if (ack_name == "a") {
			ack_sense = true;
		} else if (ack_name == "e") {
			ack_sense = false;
		} else {
			cerr <<
"Error: for now, only \'a\' and \'e\' are supported acks." << endl;
			return Command::SYNTAX;
		}
		if (string_to_num(tmp.back(), ack_init)) {
			cerr << "Error: parsing initial value of ack." << endl;
			return Command::SYNTAX;
		}
		} else {
			cerr << "Error: ack must be of the form id:init."
				<< endl;
			return Command::SYNTAX;
		}
	}{
		// parse req
		string_list tmp;
		tokenize_char(req, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: req must be of the form id:init."
				<< endl;
			return Command::SYNTAX;
		}
		req_name = tmp.front();
		if (req_name.length()) {	// if we have ack (name)...
		if (req_name == "v") {
			req_sense = true;
		} else if (req_name == "n") {
			req_sense = false;
		} else {
			cerr <<
"Error: for now, only \'v\' and \'n\' are supported reqs." << endl;
			return Command::SYNTAX;
		}
#if 0
		// req initial value is ignored, always neutral for now
		if (string_to_num(tmp.back(), req_init)) {
			cerr << "Error: parsing initial value of req." << endl;
			return Command::SYNTAX;
		}
#endif
		} else {
			cerr << "Error: req must be of the form id:init."
				<< endl;
			return Command::SYNTAX;
		}
	}{
		// parse data
		size_t c = data.find(':');
		if (c == string::npos || (c == data.length() -1)) {
			THROW_EXIT;
		}
		string_list tmp;
		tokenize_char(data, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: data must be of the form [~]id:init."
				<< endl;
			return Command::SYNTAX;
		}
		data_name = tmp.front();
#if PRSIM_CHANNEL_RAILS_INVERTED
		data_sense = (data_name[0] == '~');		// active low
#endif
		const string::const_iterator b(data_name.begin());
		data_name.assign(b +size_t(data_sense), b+c);
		if (!data_name.length()) { THROW_EXIT; }
		if (string_to_num(tmp.back(), num_rails)) {
			cerr << "Error: parsing bus width." << endl;
			return Command::SYNTAX;
		}
	}
	channel_manager& cm(s.get_channel_manager());
	if (cm.new_channel_bd4p(s, chan_name, 
			ack_sense, ack_init, req_sense, // req_init,
			data_name, num_rails, data_sense)) {
		return Command::BADARG;
	}
	return Command::NORMAL;
}
}

void
ChannelBD4P::usage(ostream& o) {
	o << name << " <name> <ack:init> <req:init> <data:num>" << endl;
	o <<
"Registers a named bundled-data channel (4-phase) in a separate namespace in \n"
"the simulator, typically used to drive or log the environment.\n"
"\'name\' is the name of the new channel in the simulator's namespace\n"
"\'ack:init\' : ack is the name of the acknowledge wire [ae], init is the initial\n"
	"\tvalue of this wire if driven by sink.\n"
"\'req:init\' : req is the name of the request rail [nv], init is the initial\n"
	"\tvalue of this wire if driven by source.\n"
"\'data:num\' : data is the name of the data rail(s) of the channel.\n"
	"\tnum is the number of rails (bus width).  Pass :0 if data-less.\n"
	<< endl;
}
#endif	// PRSIM_CHANNEL_BUNDLED_DATA

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_SYNC
/***
@texinfo cmd/clock-source.texi
@deffn Command clock-source node N
Drives wire @var{node} with toggling values.
If @var{node} is prefixed with ~, then clock is active low (negative edge).
If @var{node} is prefixed with *, then clock is double-edged.
The reset value, init, is only relevant to double-edged clocks.  
With no prefix, the clock is active-high (positive edge).
@var{N} is the number of cycles, or * for infinite. 
For single-edged clocks, a rise and fall counts as one cycle.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ClockSource, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ClockSource, "clock-source", 
	channels, "drives a signal with clock")
int
ClockSource::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	string clk_name(*i++);
	const string& count(*i);
	// could confirm that 'name' exists as a process/channel/datatype?
	bool clk_2edge = false;
	bool clk_sense = true;
	bool clk_init = false;		// not supported yet
	int iter = 0;
	{
		// parse clk
		static const char clk_fmt_err[] = 
			"Error: clk must be of the form [~*]?id:init.";
		string_list tmp;
		tokenize_char(clk_name, tmp, ':');
		if (tmp.size() != 2) {
			cerr << clk_fmt_err << endl;
			return Command::SYNTAX;
		}
		clk_name = tmp.front();
		const string clk_name2(clk_name.substr(1));
		if (clk_name.length()) {
		switch (clk_name[0]) {
		case '~' : clk_sense = false; clk_name = clk_name2; break;
		case '*' : clk_2edge = true; clk_name = clk_name2; break;
		default: break;
		}
		// clk initial value is only meaningful for 2-edged clocks
		if (clk_2edge) {
		if (string_to_num(tmp.back(), clk_init)) {
			cerr << "Error: parsing initial value of clk." << endl;
			return Command::SYNTAX;
		}
		}
		} else {
			cerr << clk_fmt_err << endl;
			return Command::SYNTAX;
		}
	}{
		// parse num cycles
		if (count[0] == '*') {
			iter = -1;		// to mean infinite
		} else if (string_to_num(count, iter)) {
			cerr << "Error: parsing cycle count." << endl;
			return Command::SYNTAX;
		}
	}
	channel_manager& cm(s.get_channel_manager());
	if (cm.new_clock_source(s, clk_name,
			clk_2edge, clk_sense, clk_init, iter)) {
		return Command::BADARG;
	}
	return Command::NORMAL;
}
}

void
ClockSource::usage(ostream& o) {
	o << name << " <[~*]?node:init> <int|*>" << endl;
	o << "node is the name of a wire signal.\n"
"If node name is prefixed with ~, clock is active-lowa\n."
"If node name is prefixed with *, clock is double-edged.\n"
"For double-edged clocks, init is the reset value.\n"
"Number of cycles is an integer or * for infinite." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-clocked.texi
@deffn Command channel-clocked name clk:init data:width
Registers a synchronous (clocked) channel, which consists of a data bus
and a clock signal.
The @var{name} of the channel should match that of an instance 
(process or channel) in the source file.  
@itemize
@item @var{name} is the name of the new channel in the simulator's namespace
@item @var{clk} is the name of the clock signal.
	The value given is the initial value of the clock on reset,
	if driven by a source.
	If the name is prefixed by ~ then clock is active low (negative-edge).
	If the name is prefixed by * then clock is double-edged.
	Otherwise, clock is considered positive-edge only.
	The @var{init} initial value is only relevant for double-edged clocks.
@item @var{data} is the name of the data rail(s), interpreted with active-high
	logic levels (prefix with @t{~} to make active-low).  
	The @var{num} value specifies the number of wires (bus width).
	If the channel is data-less (handshake only), then omit the 
	data rail name and just write @t{:}.
@end itemize
@example
@t{channel-clocked NAME clk:0 d:0} -- this names the clock @t{clk}, 
	and data @var{d} is a single-wire channel.
	@var{clk} is pos-edge triggered only.
@t{channel-clocked NAME ~clk:0 d:8} -- this names the clock @t{clk}, 
	and data @var{d} is a 8-bit bundled-data channel.
	@var{clk} is neg-edge triggered only.
@t{channel-clocked NAME *clk:0 d:8} -- this names the clock @t{clk}, 
	and data @var{d} is a 8-bit bundled-data channel.
	@var{clk} is double-edge triggered.
@end example
	Sources do not actually drive the clock, they only setup the
	data during clock edges.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelClocked, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelClocked, "channel-clocked", 
	channels, "declare a clocked channel")

int
ChannelClocked::main(State& s, const string_list& a) {
if (a.size() != 4) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	const string& chan_name(*i);
	const string& clk(*++i);
	const string& data(*++i);
	// could confirm that 'name' exists as a process/channel/datatype?
	bool clk_2edge = false;
	bool clk_sense = true;
	bool clk_init = false;		// not supported yet
	bool data_sense = false;
	size_t num_rails = 0;
	string clk_name, data_name;
	{
		// parse clk
		static const char clk_fmt_err[] = 
			"Error: clk must be of the form [~*]?id:init.";
		string_list tmp;
		tokenize_char(clk, tmp, ':');
		if (tmp.size() != 2) {
			cerr << clk_fmt_err << endl;
			return Command::SYNTAX;
		}
		clk_name = tmp.front();
		const string clk_name2(clk_name.substr(1));
		if (clk_name.length()) {
		switch (clk_name[0]) {
		case '~' : clk_sense = false; clk_name = clk_name2; break;
		case '*' : clk_2edge = true; clk_name = clk_name2; break;
		default: break;
		}
		// clk initial value is only meaningful for 2-edged clocks
		if (clk_2edge) {
		if (string_to_num(tmp.back(), clk_init)) {
			cerr << "Error: parsing initial value of clk." << endl;
			return Command::SYNTAX;
		}
		}
		} else {
			cerr << clk_fmt_err << endl;
			return Command::SYNTAX;
		}
	}{
		// parse data
		size_t c = data.find(':');
		if (c == string::npos || (c == data.length() -1)) {
			THROW_EXIT;
		}
		string_list tmp;
		tokenize_char(data, tmp, ':');
		if (tmp.size() != 2) {
			cerr << "Error: data must be of the form [~]id:init."
				<< endl;
			return Command::SYNTAX;
		}
		data_name = tmp.front();
#if PRSIM_CHANNEL_RAILS_INVERTED
		data_sense = (data_name[0] == '~');		// active low
#endif
		const string::const_iterator b(data_name.begin());
		data_name.assign(b +size_t(data_sense), b+c);
		if (!data_name.length()) { THROW_EXIT; }
		if (string_to_num(tmp.back(), num_rails)) {
			cerr << "Error: parsing bus width." << endl;
			return Command::SYNTAX;
		}
	}
	channel_manager& cm(s.get_channel_manager());
	if (clk_2edge) {
	if (cm.new_channel_clocked_2edge(s, chan_name, 
			clk_name, clk_init,
			data_name, num_rails, data_sense)) {
		return Command::BADARG;
	}
	} else {
	if (cm.new_channel_clocked_1edge(s, chan_name, 
			clk_name, clk_sense,
			data_name, num_rails, data_sense)) {
		return Command::BADARG;
	}
	}
	return Command::NORMAL;
}
}

void
ChannelClocked::usage(ostream& o) {
	o << name << " <name> <clk:init> <data:num>" << endl;
	o <<
"Registers a named clocked channel in a separate namespace in \n"
"the simulator, typically used to drive or log the environment.\n"
"\'name\' is the name of the new channel in the simulator's namespace\n"
"\'clk:init\' : clk is the name of the clock signal, init is the initial\n"
	"\tvalue of this wire if configured as double-edged source.\n"
	"\tPrefix with ~ for neg-edge, prefix with * for double-edge.\n"
"\'data:num\' : data is the name of the data rail(s) of the channel.\n"
	"\tnum is the number of rails (bus width).  Pass :0 if scalar wire.\n"
	<< endl;
}
#endif	// PRSIM_CHANNEL_SYNC

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// for finesse
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AutoChannel, "auto-channel", 
	channels, "register a channel based on internal type")
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-report-time.texi
@deffn Command channel-report-time [on|off]
Set this switch on to show simulation timestamps when watched channels 
are printed or logged channels are written to file.  
Default: off
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelReportTime, "channel-report-time", 
	channels, "display timestamps with watched and logged channels")

int
ChannelReportTime::main(State& s, const string_list& a) {
	typedef	channel			channel_type;
switch (a.size()) {
case 1:
	cout << "channel report time: " <<
		(channel_type::report_time ? "on" : "off")
		<< endl;
	return Command::NORMAL;
case 2: {
	const string& arg(a.back());
	if (arg == "on") {
		channel_type::report_time = true;
	} else if (arg == "off") {
		channel_type::report_time = false;
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
ChannelReportTime::usage(ostream& o) {
	o << name << " [on|off]" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
/**
	Produce i as an iterator referring to a channel-pointer.
	\param type is channel or const channel.
	\param name is the name of the channel(s).
 */
#define	CHANNEL_FOR_EACH(T, name)					\
	channel_manager& cm(s.get_channel_manager());			\
	vector<T*> __tmp;						\
	if (cm.lookup_expand(name, s.get_module(), __tmp))		\
		{ return Command::BADARG; }				\
	vector<T*>::const_iterator					\
		i(__tmp.begin()), e(__tmp.end());			\
	for ( ; i!=e; ++i)
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
// TODO: prsim tab-completion on registered channel names
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelShow, instance_completer)
// PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelShow, prsim_channel_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelShow, "channel-show", 
	channels, "show configuration of registered channel")

int
ChannelShow::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(const channel, ref) {
		const channel& c(**i);
		// from channel_manager::__dump_channel
		c.dump(cout) << endl;
		c.dump_state(cout << '\t') << endl;
	}
#else
	if (s.dump_channel(cout, ref)) {
		return Command::BADARG;
	}
#endif
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
@texinfo cmd/channel-get.texi
@deffn Command channel-get chan
This prints the current handshake state of a channel, 
including the current value, if valid, 
and the expected activity (e.g., waiting for data from sender, 
or ack from receiver).
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelGet, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelGet, "channel-get", 
	channels, "print handshake state of channel")

int
ChannelGet::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(const channel, ref) {
		const channel& c(**i);
		c.dump_status(cout << "channel "
			<< c.get_name() << ": ", s) << endl;
	}
#else
	if (s.dump_channel_state(cout, ref)) {
		return Command::BADARG;
	}
#endif
	return Command::NORMAL;
}
}

void
ChannelGet::usage(ostream& o) {
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
/***
@texinfo cmd/channel-assert.texi
@deffn Command channel-assert chan args...
This asserts the current state of a channel.  
Legal values for arguments (in any order and combination):
@itemize
@item <int> the integer value of the data rails; passes only if data
	is valid and matches the expected value.  
@item @t{valid} (four-phase or two-phase) passes if the channel data
	rails are in the valid state, 
	or the validity signal (if any) is active, 
	or a two-phase channel is in the set-phase (full).  
@item @t{neutral} (four-phase or two-phase) passes if the channel data
	rails are all neutral/null.
@item @t{full} is synonymous with @t{valid}
@item @t{empty} is synonymous with @t{neutral}
@item @t{ack} (four-phase only) passes if the acknowledge is in
	the active state, whether the signal is active-high or active-low.
@item @t{neg-ack} (four-phase only) passes if the acknowledge is in
	the negative state.
@item @t{waiting-sender} (four-phase or two-phase) passes if the channel
	is in a state of the handshake that expects the next action
	from the sender of the channel.  
@item @t{waiting-receiver} (four-phase or two-phase) passes if the channel
	is in a state of the handshake that expects the next action
	from the receiver of the channel.  
@end itemize
The error-handling policy in the case of a failed assertion is controlled
by @command{channel-expect-fail}.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelAssert, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelAssert, "channel-assert", 
	channels, "asserts current state of channel")

int
ChannelAssert::main(State& s, const string_list& a) {
if (a.size() < 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	const string& n(*i);
	++i;
	const channel* const c = s.get_channel_manager().lookup(n);
	if (!c) {
		cerr << "No channel " << n << " found." << endl;
		return Command::BADARG;
	}
	// check all arguments
	error_policy_enum E = ERROR_NONE;
	for ( ; i!=a.end(); ++i) {
		const error_policy_enum R = c->assert_status(cout, s, *i);
		if (R > E) E = R;
	}
	return error_policy_to_status(E);
}
}

void
ChannelAssert::usage(ostream& o) {
	o << name << " <channel> {<int>|<keyword>}*" << endl;
// TODO: !summon info!
	o <<
"This asserts the current state of a channel.\n"
"Legal values for arguments (in any order and combination):\n"
"  <int> the integer value of the data rails; passes only if data\n"
"\tis valid and matches the expected value.\n"
"  \'valid\' (four-phase or two-phase) passes if the channel data\n"
"\trails are in the valid state, or the validity signal (if any) is active,\n"
"\tor a two-phase channel is in the set-phase (full).\n"
"  \'neutral\' (four-phase or two-phase) passes if the channel data\n"
"\trails are all neutral/null.\n"
"  \'full\' is synonymous with \'valid\'\n"
"  \'empty\' is synonymous with \'neutral\'\n"
"  \'ack\' (four-phase only) passes if acknowledge is in the active state,\n"
"\twhether the signal is active-high or active-low.\n"
"  \'neg-ack\' (four-phase only) passes if the acknowledge is in\n"
"\tthe negative state.\n"
"  \'waiting-sender\' (four-phase or two-phase) passes if the channel\n"
"\tis in a state of the handshake that expects the next action\n"
"\tfrom the sender of the channel.\n"
"  \'waiting-receiver\' (four-phase or two-phase) passes if the channel\n"
"\tis in a state of the handshake that expects the next action\n"
"\tfrom the receiver of the channel.  \n"
"The error-handling policy in the case of a failed assertion is controlled\n"
"by @command{channel-expect-fail}."
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
/**
	Standard channel command implementation.
	No additional arguments beyond the name of the channel.  
 */
static
int
standard_channel_command_main(State& s, const string_list& a, 
		void (channel::*memfn)(void), void (usage)(ostream&)) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, ref) {
		channel& c(**i);
		(c.*memfn)();
	}
	return Command::NORMAL;
#else
	return s.get_channel_manager().apply_one(ref, memfn) ?
		Command::BADARG : Command::NORMAL;
#endif
}
}

/**
	Standard apply-all channel command implementation.
	No arguments required.
 */
static
int
standard_channel_apply_all_main(State& s, const string_list& a, 
		void (channel::*memfn)(void), void (usage)(ostream&)) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.get_channel_manager().apply_all(memfn);
	return Command::NORMAL;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_CHANNEL_SIGNED
/***
@texinfo cmd/channel-signed.texi
@deffn Command channel-signed chan
@deffnx Command channel-unsigned chan
By default, bundled channels are interpreted as unsigned numbers.
@command{channel-signed} indicates that @var{chan} should be interpreted
and displayed as signed values.  
Signedness is only applicable to binary (radix-2) channels with more 
than one bit; 
non-radix-2 channels are always interpreted as unsigned, 
and single bit channels are always unsigned (0 or 1).  
It is generally recommended to declare the signedness of a channel
immedately after declaring it, and before any values are interpreted.
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelSigned, instance_completer)
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelUnsigned, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSigned, "channel-signed", 
	channels, "interpret data rails as signed (radix-2 only)")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelUnsigned, "channel-unsigned", 
	channels, "interpret data rails as unsigned")

int
ChannelSigned::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a,
		&channel::set_signed, usage);
}

int
ChannelUnsigned::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a,
		&channel::set_unsigned, usage);
}

void
ChannelSigned::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o <<
"Configures a channel to display and interpret data rails as signed binary \n"
"integer values.  Only applicable to radix-2 channels."
		<< endl;
}

void
ChannelUnsigned::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o <<
"Configures a channel to display and interpret data rails as unsigned binary \n"
"integer values."
		<< endl;
}
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelWatch, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelWatch, "channel-watch", 
	channels, "report when specified channel changes state")

int
ChannelWatch::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a, &channel::watch, usage);
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelUnWatch, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelUnWatch, "channel-unwatch", 
	channels, "ignore when spcified channel changes state")

int
ChannelUnWatch::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a, &channel::unwatch, usage);
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
	return standard_channel_apply_all_main(s, a, &channel::watch, usage);
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
	return standard_channel_apply_all_main(s, a, &channel::unwatch, usage);
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelHeed, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelHeed, "channel-heed", 
	channels, "resume logging and checking channel values")

int
ChannelHeed::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a, &channel::heed, usage);
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelIgnore, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelIgnore, "channel-ignore", 
	channels, "stop logging and checking channel values")

int
ChannelIgnore::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a, &channel::ignore, usage);
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
	return standard_channel_apply_all_main(s, a, &channel::heed, usage);
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
	return standard_channel_apply_all_main(s, a, &channel::ignore, usage);
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelReset, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelReset, "channel-reset", 
	channels, "set a channel into its reset state")

int
ChannelReset::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, ref) {
		channel& c(**i);
		s.reset_channel(c);	// includes flush_channel_events
	}
#else
	if (s.reset_channel(ref))
		return Command::BADARG;
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelStop, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelStop, "channel-stop", 
	channels, "hold a channel in its current state")

int
ChannelStop::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a, &channel::stop, usage);
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
	return standard_channel_apply_all_main(s, a, &channel::stop, usage);
}

void
ChannelStopAll::usage(ostream& o) {
	o << name << endl;
	o << "Prevent all source/sink channels from operating." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/channel-stop-on-empty.texi
@deffn Command channel-stop-on-empty chan
@deffnx Command channel-continue-on-empty chan
For channels that are sinking and expecting values (non-loop), 
stop sinking as soon as expected values are exhausted.  
The default behavior for a sink is to continue sinking regardless
of checking against expected values.  
@end deffn
@end texinfo
***/
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelStopOnEmpty,
	"channel-stop-on-empty", 
	channels, "stop channel sink after last expected value")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelContinueOnEmpty,
	"channel-continue-on-empty", 
	channels, "allow channel sink after last expected value")

int
ChannelStopOnEmpty::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a,
		&channel::stop_on_empty, usage);
}

int
ChannelContinueOnEmpty::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a,
		&channel::continue_on_empty, usage);
}

void
ChannelStopOnEmpty::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << brief << endl;
}

void
ChannelContinueOnEmpty::usage(ostream& o) {
	o << name << " <channel>" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// could call these Resume...
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelRelease, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelRelease, "channel-release", 
	channels, "release a channel from its reset or stopped state")

int
ChannelRelease::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, ref) {
		channel& c(**i);
		s.resume_channel(c);	// includes flush_channel_events
	}
#else
	if (s.resume_channel(ref))
		return Command::BADARG;
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelClose, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelClose, "channel-close", 
	channels, "close any files/streams associated with channel")

int
ChannelClose::main(State& s, const string_list& a) {
	return standard_channel_command_main(s, a, &channel::close_stream, 
		usage);
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
	return standard_channel_apply_all_main(s, a, &channel::close_stream, 
		usage);
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
// TODO: position-specific completion
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelSourceFile, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceFile, "channel-source-file", 
	channels, "source values on channel from file (once)")
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelSource, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSource, "channel-source", 
	channels, "alias to channel-source (deprecated)")

int
ChannelSourceFile::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(*++a.begin());
	const string& fn(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, ref) {
		channel& c(**i);
		// from channel_manager::source_channel_file
		if (c.set_source_file(s, fn, false) || cm.check_source(c))
			return Command::BADARG;
	}
#else
	if (s.get_channel_manager().source_channel_file(s, ref, fn, false))
		return Command::BADARG;
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelSourceFileLoop, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceFileLoop, 
	"channel-source-file-loop", 
	channels, "source values on channel from file (loop)")
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelSourceLoop, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceLoop, "channel-source-loop", 
	channels, "alias to channel-source-loop (deprecated)")

int
ChannelSourceFileLoop::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(*++a.begin());
	const string& fn(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, ref) {
		channel& c(**i);
		// from channel_manager::source_channel_file
		if (c.set_source_file(s, fn, true) || cm.check_source(c))
			return Command::BADARG;
	}
#else
	if (s.get_channel_manager().source_channel_file(s, ref, fn, true))
		return Command::BADARG;
#endif
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
If no values are given, then the channel will not source any values, 
but it will still reset the data rails to neutral state.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelSourceArgs, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceArgs,
	"channel-source-args", 
	channels, "source values on channel from arguments (once)")

int
ChannelSourceArgs::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(*++a.begin());
	string_list v;
	copy(++++a.begin(), a.end(), back_inserter(v));
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, ref) {
		channel& c(**i);
		// from channel_manager::source_channel_args
		if (c.set_source_args(s, v, false) || cm.check_source(c))
			return Command::BADARG;
	}
#else
	if (s.get_channel_manager().source_channel_args(s, ref, v, false))
		return Command::BADARG;
#endif
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
If no values are given, then the channel will not source any values, 
but it will still reset the data rails to neutral state.  
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelSourceArgsLoop, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSourceArgsLoop,
	"channel-source-args-loop", 
	channels, "source channel values from arguments (loop)")

int
ChannelSourceArgsLoop::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(*++a.begin());
	string_list v;
	copy(++++a.begin(), a.end(), back_inserter(v));
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, ref) {
		channel& c(**i);
		// from channel_manager::source_channel_args
		if (c.set_source_args(s, v, true) || cm.check_source(c))
			return Command::BADARG;
	}
#else
	if (s.get_channel_manager().source_channel_args(s, ref, v, true))
		return Command::BADARG;
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelRandomSource, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelRandomSource, "channel-rsource", 
	channels, "source random values on channel (infinite)")

int
ChannelRandomSource::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, ref) {
		channel& c(**i);
		// from channel_manager::rsource_channel
		if (c.set_rsource(s) || cm.check_source(c))
			return Command::BADARG;
	}
#else
	if (s.get_channel_manager().rsource_channel(s, ref))
		return Command::BADARG;
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelSink, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelSink, "channel-sink", 
	channels, "consume tokens infinitely on channel")

int
ChannelSink::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, ref) {
		channel& c(**i);
		if (c.set_sink(s) || cm.check_sink(c))
			return Command::BADARG;
	}
#else
	channel_manager& cm(s.get_channel_manager());
	if (cm.sink_channel(s, ref))
		return Command::BADARG;
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelLog, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelLog, "channel-log", 
	channels, "log channel values to file")

int
ChannelLog::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& ref(*++a.begin());
	const string& fn(a.back());
	if (s.get_channel_manager().log_channel(ref, fn))
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelExpectFile, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpectFile, "channel-expect-file", 
	channels, "assert values on channel from file (once)")
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelExpect, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpect, "channel-expect", 
	channels, "alias to channel-expect-file (deprecated)")

int
ChannelExpectFile::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& chan_name(*++a.begin());
	const string& fn(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, chan_name) {
		channel& c(**i);
		if (c.set_expect_file(fn, false))
			return Command::BADARG;
	}
#else
	channel_manager& cm(s.get_channel_manager());
	if (cm.expect_channel_file(chan_name, fn, false))
		return Command::BADARG;
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelExpectFileLoop, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpectFileLoop,
	"channel-expect-file-loop", 
	channels, "assert values on channel from file (loop)")
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelExpectLoop, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelExpectLoop, "channel-expect-loop", 
	channels, "alias to channel-expect-loop (deprecated)")

int
ChannelExpectFileLoop::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& chan_name(*++a.begin());
	const string& fn(a.back());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, chan_name) {
		channel& c(**i);
		if (c.set_expect_file(fn, true))
			return Command::BADARG;
	}
#else
	channel_manager& cm(s.get_channel_manager());
	if (cm.expect_channel_file(chan_name, fn, true))
		return Command::BADARG;
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelExpectArgs, instance_completer)
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
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, chan_name) {
		channel& c(**i);
		if (c.set_expect_args(v, false))
			return Command::BADARG;
	}
#else
	channel_manager& cm(s.get_channel_manager());
	if (cm.expect_channel_args(chan_name, v, false))
		return Command::BADARG;
#endif
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
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelExpectArgsLoop, instance_completer)
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
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, chan_name) {
		channel& c(**i);
		if (c.set_expect_args(v, true))
			return Command::BADARG;
	}
#else
	channel_manager& cm(s.get_channel_manager());
	if (cm.expect_channel_args(chan_name, v, true))
		return Command::BADARG;
#endif
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
	"assert whether channel has source/expect values left")

int
ChannelAssertValueQueue::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator ai(++a.begin());
	const string& cn(*ai);
	const string& v(*++ai);
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(const channel, cn) {
		const channel& c(**i);
#else
	const channel* const _c = s.get_channel_manager().lookup(cn);
	if (c) {
		const channel& c(*_c);
#endif
		int x;
		if (string_to_num(v, x)) {
			cerr << "Error: expecting 0 or 1 to assert." << endl;
			usage(cerr);
			return Command::BADARG;
		}
		// TODO: generalize argument to number of values remaining?
		if (c.have_value()) {
			if (!x) {
				cerr << "Expecting no more channel values on "
					<< cn << ", but found some." << endl;
				return Command::FATAL;
			} else if (s.confirm_asserts()) {
				cout << "channel " << c.get_name() <<
					" has more values, as expected."
					<< endl;
			}
		} else {
			if (x) {
				cerr << "Expecting more channel values on "
					<< cn << ", but found none." << endl;
				return Command::FATAL;
			} else if (s.confirm_asserts()) {
				cout << "channel " << c.get_name() <<
					" has no more values, as expected."
					<< endl;
			}
		}
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	}	// end for each
	return Command::NORMAL;
#else
		return Command::NORMAL;
	} else	return Command::BADARG;
#endif
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
@item @t{random [[min]:[max]]} : if @var{max} is specified, 
	use a uniform distribution delay bounded by @var{max}, 
	otherwise return an exponential variate delay with a minimum
	of @var{min}.  Unspecified @var{min} bounds defaults to 0.
	Unspecified @var{max} defaults to +INF.
@item @t{binary [min]:[max] prob} : chooses either the @var{min} or the 
	@var{max} value with @var{prob} probability of taking 
	the @var{min} value.
@end itemize
@end deffn
@end texinfo
***/
PRSIM_OVERRIDE_DEFAULT_COMPLETER_FWD(ChannelTiming, instance_completer)
DECLARE_AND_INITIALIZE_COMMAND_CLASS(ChannelTiming, "channel-timing", 
	channels, "set/get per channel timing mode")

int
ChannelTiming::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else if (a.size() == 2) {
	const string& cn(*++a.begin());
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(const channel, cn) {
		const channel& c(**i);
#else
	const channel* const _c = s.get_channel_manager().lookup(cn);
	if (c) {
		const channel& c(*_c);
#endif
		c.dump_timing(cout << "channel " << cn << ": ") << endl;
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	}	// end for each
	return Command::NORMAL;
#else
		return Command::NORMAL;
	} else	return Command::BADARG;	// have error message
#endif
} else {
	string_list b(a);
	b.pop_front();
	const string cn(b.front());
	b.pop_front();
	const string m(b.front());
	b.pop_front();
#if PRSIM_CHANNEL_AGGREGATE_ARGUMENTS
	CHANNEL_FOR_EACH(channel, cn) {
		channel& c(**i);
		if (c.set_timing(m, b)) {
			usage(cerr << "usage: ");
			return Command::BADARG;
		}
	}
#else
	channel* const c = s.get_channel_manager().lookup(cn);
	if (c && c->set_timing(m, b)) {
		usage(cerr << "usage: ");
		return Command::BADARG;
	}
#endif
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
#if PRSIM_TRACE_GENERATION
/***
@texinfo cmd/trace.texi
@deffn Command trace file
Record events to tracefile @var{file}.  
Overwrites @var{file} if it already exists.  
A trace stream is automatically closed when the @command{initialize}
or @command{reset} commands are invoked.  
See the @option{-r} option for starting up the simulator
with a newly opened trace stream.
The format of this trace file is unique to @command{hacprsim}.  
@end deffn
@end texinfo
***/
typedef	Trace<State>				Trace;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(Trace, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-file.texi
@deffn Command trace-file
Print the name of the currently opened trace file.  
@end deffn
@end texinfo
***/
typedef	TraceFile<State>			TraceFile;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceFile, tracing)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceClose, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-flush-notify.texi
@deffn Command trace-flush-notify [0|1]
Enable (1) or disable (0) notifications when trace epochs are flushed.  
@end deffn
@end texinfo
***/
typedef	TraceFlushNotify<State>			TraceFlushNotify;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceFlushNotify, tracing)

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
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceFlushInterval, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/trace-dump.texi
@deffn Command trace-dump file
Produce textual dump of trace file contents in @var{file}.
@end deffn
@end texinfo
***/
typedef	TraceDump<State>			TraceDump;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(TraceDump, tracing)
#endif	// PRSIM_TRACE_GENERATION

//=============================================================================
#if PRSIM_VCD_GENERATION
/***
@texinfo cmd/vcd.texi
@deffn Command vcd file
Record events to vcd @var{file}.  
Overwrites @var{file} if it already exists.  
A vector-change-dump (VCD) stream is automatically closed when the 
@command{initialize} or @command{reset} commands are invoked.  
See the @option{-r} option for starting up the simulator
with a newly opened trace stream.
@cindex vector-change-dump
@cindex VCD
@end deffn
@end texinfo
***/
typedef	VCD<State>				VCD;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(VCD, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/vcd-file.texi
@deffn Command vcd-file
Print the name of the currently opened vcd file.  
@end deffn
@end texinfo
***/
typedef	VCDFile<State>			VCDFile;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(VCDFile, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/vcd-timescale.texi
@deffn Command vcd-timescale [val]
Sets/gets the time scale by which real-valued actual times are
multiplied to get the output vcd timestamps.  
This is needed because vcd files don't necessary support
floating-point values, so a scale factor can be used
to select a suitable time granularity.
Default: 1.0
@end deffn
@end texinfo
***/
typedef	VCDTimeScale<State>			VCDTimeScale;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(VCDTimeScale, tracing)

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/***
@texinfo cmd/vcd-close.texi
@deffn Command vcd-close
Finish writing the currently opened vcd file by flushing out
buffered events to file.
VCD files are automatically flushed and closed when the simulator exits.  
@end deffn
@end texinfo
***/
typedef	VCDClose<State>			VCDClose;
PRSIM_INSTANTIATE_TRIVIAL_COMMAND_CLASS(VCDClose, tracing)
#endif	// PRSIM_VCD_GENERATION

//=============================================================================
#undef	DECLARE_AND_INITIALIZE_COMMAND_CLASS
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

