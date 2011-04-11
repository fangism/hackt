/**
	\file "PR/pr-command.cc"
	Command-line feature for PR simulator.
	TODO: scheme interface
	$Id: pr-command.cc,v 1.1.2.1 2011/04/11 18:38:39 fang Exp $
 */

#include <iostream>
#include "PR/pr-command.h"
#include "PR/placement_engine.h"
#include "sim/command_base.tcc"
#include "sim/command_category.tcc"
#include "sim/command_registry.tcc"
#include "sim/command_macros.tcc"
#include "sim/command_common.h"
#include "util/optparse.h"

//=============================================================================
// explicit instantiations
namespace HAC {
namespace SIM {
template class command_registry<PR::Command>;
}
}

//=============================================================================
using namespace HAC::SIM;
namespace PR {

//=============================================================================
// name/index translation features?

// reference parsing features?

//=============================================================================
static
CommandCategory
	builtin("builtin", "built-in commands"),
	general("general", "general commands"),
	debug("debug", "debugging internals"),
	simulation("simulation", "simulation commands"),
	objects("objects", "object creation/manipulation commands"),
	info("info", "information about objects"),
	parameters("parameters", "physical parameters");

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
INSTANTIATE_TRIVIAL_COMMAND_CLASS(PR, _class, _cat)

#define	PR_INSTANTIATE_STATELESS_COMMAND_CLASS(_class, _cat)		\
typedef	stateless_command_wrapper<_class, State>	_class;		\
INSTANTIATE_COMMON_COMMAND_CLASS(PR, stateless_command_wrapper, _class, _cat)

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
CommandCategory& _class::category(_category);				\
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
typedef	Help<State>				Help;
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

//=============================================================================
}	// end namespace PR

