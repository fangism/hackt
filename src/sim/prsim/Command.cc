/**
	\file "sim/prsim/Command.cc"
	All built-in commands should be statically registered 
	in this translation unit to guarantee proper static object
	initialization ordering; the command_map must be constructed
	before entries are registered.  

	TODO: consider using some form of auto-indent
		in the help-system.  

	$Id: Command.cc,v 1.18 2006/08/13 22:16:03 fang Exp $
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <set>

#include "sim/prsim/Command.h"
#include "sim/prsim/State.h"
#include "parser/instref.h"

#include "common/TODO.h"
#include "util/qmap.tcc"
#include "util/readline_wrap.h"
#include "util/libc.h"
#include "util/string.tcc"
#include "util/memory/excl_malloc_ptr.h"
#include "util/tokenize.h"

/**
	These commands are deprecated, but provided for backwards compatibility.
	Eventually disable this.  
	(NOTE: the alias command can always effectively re-enable it.)
 */
#define	WANT_OLD_RANDOM_COMMANDS			1

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_istream.h"
#include "util/using_ostream.h"
using std::ios_base;
using std::set;
using std::ifstream;
using std::copy;
using std::reverse_copy;
using std::ostream_iterator;
using std::front_inserter;
using util::readline_wrapper;
using util::tokenize;
using util::excl_malloc_ptr;
using util::strings::string_to_num;
using util::strings::eat_whitespace;
using parser::parse_node_to_index;
using parser::parse_name_to_what;
using parser::parse_name_to_aliases;
using parser::parse_name_to_get_subnodes;

//=============================================================================
// class Command method definitions

Command::Command() : _name(), _brief(), 
		_category(NULL), _main(NULL), _usage(NULL) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Command::Command(const string& _n, const string& _b, const category_ptr_type _c,
		const main_ptr_type _m, const usage_ptr_type _u) :
		_name(_n), _brief(_b), _category(_c), _main(_m), _usage(_u) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\return 0 on success.  
 */
int
Command::main(State& s, const string_list& args) const {
	if (_main) {
		return (*_main)(s, args);
	} else {
		cerr << "command is undefined." << endl;
		return Command::UNKNOWN;
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

/**
	Global static initialization of the command-map.
	This MUST be initialized before any commands are registered.
 */
CommandRegistry::command_map_type
CommandRegistry::command_map;

/**
	Global static initialization of the category-map.
	This MUST be initialized before any categories are registered.  
 */
CommandRegistry::category_map_type
CommandRegistry::category_map;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
size_t
CommandRegistry::register_category(CommandCategory& c) {
	typedef	category_map_type::mapped_type		mapped_type;
	mapped_type& probe(category_map[c.name()]);
	// can we be sure that the ptr is initially NULL?
	// perhaps only if we use a never_ptr.  
	if (probe) {
		cerr << "category \'" << c.name() <<
			"\' has already been registered." << endl;
		THROW_EXIT;
	} else {
		probe = &c;
	}
	return category_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Complete registration of command with category.  
 */
template <class C>
size_t
CommandRegistry::register_command(void) {
	typedef	C	command_type;
	const Command temp(command_type::name, command_type::brief,
		&command_type::category, &command_type::main, 
		&command_type::usage);
	typedef	command_map_type::mapped_type		mapped_type;
	const string& s(command_type::name);
	mapped_type& probe(command_map[s]);
	if (probe) {
		cerr << "command \'" << s << "\' has already been "
			"registered globally." << endl;
		THROW_EXIT;
	} else {
		probe = temp;
	}
	command_type::category.register_command(temp);
	return command_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints list of command categories.  
 */
void
CommandRegistry::list_categories(ostream& o) {
	o << "available categories: " << endl;
	category_iterator i(category_map.begin());
	const category_iterator e(category_map.end());
	for ( ; i!=e; ++i) {
		o << '\t' << i->first << " --- " << i->second->brief() << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints list of commands, arranged by category.
 */
void
CommandRegistry::list_commands(ostream& o) {
	typedef	category_map_type::const_iterator	const_iterator;
	o << "available commands, by category: " << endl;
	category_iterator i(category_map.begin());
	const category_iterator e(category_map.end());
	for ( ; i!=e; ++i) {
		i->second->list(o);
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interpreter termination condition.  
	TODO: Shouldn't interpreter halt on Command::FATAL (assert fail)?
 */
bool
CommandRegistry::continue_interpreter(const int _status, const bool ia) {
	return (_status != Command::END) && (!_status || ia);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// don't delete this yet...
class CommandRegistry::interactive_mode {
private:
	bool			save_int_mode;
public:
	/// save away current interactive mode
	explicit
	interactive_mode(const bool m) : save_int_mode(interactive) {
		interactive = m;
	}

	/// restore former interactive mode
	~interactive_mode() {
		interactive = save_int_mode;
	}

} __ATTRIBUTE_UNUSED__ ;	// end class interactive_mode
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Make sure name isn't already registered as a category, 
	standard command, or existing alias.  
 */
int
CommandRegistry::add_alias(const string& a, const string_list& c) {
	if (command_map.find(a) != command_map.end()) {
		cerr << "\'" << a << "\' is already a command, "
			"cannot be used to register an alias." << endl;
		return Command::BADARG;
	} else if (category_map.find(a) != category_map.end()) {
		cerr << "\'" << a << "\' is already a category, "
			"cannot be used to register an alias." << endl;
		return Command::BADARG;
	} else if (aliases.find(a) != aliases.end()) {
		cerr << "\'" << a << "\' is already an alias; "
			"you must unalias it before redefining it." << endl;
		return Command::BADARG;
	} else {
		aliases[a] = c;
		return Command::NORMAL;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unregisters an alias.  
	Doesn't check for former existence.  
 */
int
CommandRegistry::unalias(const string& a) {
	aliases.erase(a);
	return Command::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unregisters all aliases.  
 */
int
CommandRegistry::unalias_all(void) {
	aliases.clear();
	return Command::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
CommandRegistry::list_aliases(ostream& o) {
	o << "Command aliases:" << endl;
	alias_iterator i(aliases.begin()), e(aliases.end());
	for ( ; i!=e; ++i) {
		o << "\t" << i->first << " -> ";
		ostream_iterator<string> osi(o, " ");
		copy(i->second.begin(), i->second.end(), osi);
		o << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands prefix aliases, just like sh.  
	\pre there exist no cyclic aliases.  
	\pre no existing commands are overridden by aliases.  
 */
int
CommandRegistry::expand_aliases(string_list& c) {
if (c.size()) {
	set<string> seen;
	alias_iterator a(aliases.find(c.front()));
	while (a != aliases.end()) {
		const string_list& x(a->second);
		if (!seen.insert(a->first).second) {
			cerr << "Error: detected cyclic alias during expansion!"
				<< endl;
			return Command::BADARG;
		}
		c.pop_front();
		reverse_copy(x.begin(), x.end(), front_inserter(c));
		a = aliases.find(c.front());
	}
	// stops expanding as soon as non-alias is found.  
}
// else nothing to expand
	return Command::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interprets a single command line.  
	Capable of executing shell commands with '!' prefix.  
	Now capable of expanding alias commands.  
	\pre there are no cyclic aliases.  
 */
int
CommandRegistry::interpret_line(State& s, const string& line) {
	const char* cursor = line.c_str();
	if (*cursor == '!') {
		++cursor;
		eat_whitespace(cursor);
		const int es = system(cursor);
		// let status remain as is, for now
		// TODO: determine exit behavior
		if (es) {
			cerr << "*** Exit " << es << endl;
		}
		return Command::NORMAL;
	} else {
		string_list toks;
		tokenize(line, toks);
		// check if command is aliased :)
		if (expand_aliases(toks) != Command::NORMAL) {
			return Command::BADARG;
		} else {
			return execute(s, toks);
		}
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Reads commands from stdin/cin.
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
	static const char noprompt[] = "";
if (interactive) {
	readline_wrapper rl(interactive ? prompt : noprompt);
	// do NOT delete this line string, it is already managed.
	const char* line = NULL;
	int status = Command::NORMAL;
	size_t lineno = 0;
	do {
		line = rl.gets();	// already eaten leading whitespace
		// GOTCHA: readline eats '\t' characters!?
	if (line) {
#if 0
		cout << "echo: " << line << endl;
#endif
		status = interpret_line(s, line);
		++lineno;
	}
	} while (line && continue_interpreter(status, interactive));
	// end-line for neatness
	if (!line)	cout << endl;
	// not sure if the following is good idea
	if (status && !interactive) {
		// useful for tracing errors in sourced files
		cerr << "Error detected at line " << lineno <<
			", aborting commands." << endl;
		return status;
	} else	return Command::NORMAL;
} else {
	// non interactive, skip readline, preserves tab-characters
	return __source(cin, s);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param i the input stream, such as file or cin.
	\param s the simulation state.  
	\return interpreter's exit status.
 */
int
CommandRegistry::__source(istream& i, State& s) {
	int status = Command::NORMAL;
	// const interactive_mode tmp(false);
	string line;
	do {
		std::getline(i, line);
	if (i) {
#if 0
		cout << "echo: " << line << endl;
#endif
		status = interpret_line(s, line);
	}	// end if
	} while (i && continue_interpreter(status, false));
	return status;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param f the file name.  
	\param st the simulator state.  
	\return status of the interpreter.
	TODO: manage the interactive bit with an object/class, 
		for prope stack restoration with exception safety.  
	TODO: keep track of already opened files to prevent 
		cyclic sourcing...
 */
int
CommandRegistry::source(State& st, const string& f) {
	ifstream_manager::placeholder p(st.get_stream_manager(), f);
	// ifstream i(f.c_str());
if (p) {
	return __source(p.get_stream(), st);
	// return __source(i, st);
} else {
	cerr << "Error opening file: \"" << f << '\"' << endl;
	p.error_msg(cerr) << endl;
	return Command::BADFILE;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: this does not work with alias commands, aliases
	should be pre-expanded before passing to execute.  
 */
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
		return Command::SYNTAX;
	}
} else	return Command::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints the usage for a particular command.
	\param c the name of the command sought.
	\return true if a command was found
 */
bool
CommandRegistry::help_command(ostream& o, const string& c) {
	typedef	command_map_type::const_iterator	const_iterator;
	const_iterator probe(command_map.find(c));
	if (probe != command_map.end()) {
		probe->second.usage(o);
		return true;
	} else	return false;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints brief descriptions for all commands in a particular category.  
	\param c the name of the category.  
	\return true if category was found.  
 */
bool
CommandRegistry::help_category(ostream& o, const string& c) {
	typedef	category_map_type::const_iterator	const_iterator;
	const_iterator probe(category_map.find(c));
	if (probe != category_map.end()) {
		probe->second->list(o);
		return true;
	} else	return false;
}

//=============================================================================
// class CommandCategory method definitions

CommandCategory::CommandCategory() : _name(), _brief(), command_map() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
CommandCategory::CommandCategory(const string& _n, const string& _b) :
		_name(_n), _brief(_b), command_map() {
	CommandRegistry::register_category(*this);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Don't unregister because assignment is used to set
	a category.  
 */
CommandCategory::~CommandCategory() {
	// CommandCategory::unregister_category()?
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Registers a command with the local category.  
 */
size_t
CommandCategory::register_command(const Command& c) {
	typedef	command_map_type::mapped_type	mapped_type;
	mapped_type& probe(command_map[c.name()]);
	if (probe) {
		cerr << "command \'" << c.name() <<
			"\' has already been registered "
			"in this category (" << probe.name() << ")." << endl;
		THROW_EXIT;
	} else {
		probe = c;
	}
	return command_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
CommandCategory::list(ostream& o) const {
	o << "Commands available in the \'" << _name << "\' category:" << endl;
	const_iterator i(command_map.begin());
	const const_iterator e(command_map.end());
	for ( ; i!=e; i++) {
		o << '\t' << i->first << " -- " << i->second.brief() << endl;
	}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Looks up a command.  
	Pretty much only used by alias lookups.  
 */
CommandCategory::const_iterator
CommandCategory::lookup_command(const string& c) const {
	return command_map.find(c);
}

//=============================================================================
// local static CommandCategories
// feel free to add categories here

#if 0
CommandCategory
	CommandRegistry::aliases("aliases", "user-defined alias commands");
#else
/**
	Global static initialization of aliases.  
	Or should we have one per interpreter?
 */
CommandRegistry::aliases_map_type
CommandRegistry::aliases;
#endif

static CommandCategory
	builtin("builtin", "built-in commands"),
	general("general", "general commands"),
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
	DECLARE_COMMAND_CLASS(_class)					\
	INITIALIZE_COMMAND_CLASS(_class, _cmd, _category, _brief)

//-----------------------------------------------------------------------------
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Echo, "echo", builtin, 
	"prints arguments back to stdout, space-delimited")

int
Echo::main(State&, const string_list& args) {
	INVARIANT(!args.empty());
	ostream_iterator<string> osi(cout, " ");
	copy(++args.begin(), args.end(), osi);
	cout << endl;
	return Command::NORMAL;
}

void
Echo::usage(ostream& o) {
	o << "echo [args]: repeats arguments to stdout, space-delimited"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
INITIALIZE_COMMAND_CLASS(Help, "help", builtin,
	"show available commands and categories")

/**
	\param args the tokenized command line, including the "help" command
		as the first token.
 */
int
Help::main(const string_list& args) {
	INVARIANT(args.size());
	const string_list::const_iterator argi(++args.begin());
	// skip first, which is 'help'
	if (argi != args.end()) {
		if (!CommandRegistry::help_category(cout, *argi) &&
			!CommandRegistry::help_command(cout, *argi)) {
			cerr << "Unknown category/command: " << *argi<< endl;
		}
	} else {
		// no argument, just list available commands
		CommandRegistry::list_categories(cout);
		usage(cout);
	}
	return Command::NORMAL;
}

int
Help::main(State&, const string_list& args) {
	return main(args);
}

void
Help::usage(ostream& o) {
	o << "help: lists available commands and categories" << endl;
	o << "to get usage for a specific command or category, "
		"run: help <name>" << endl;
	o << "help all: lists all commands across all categories" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(CommentPound, "#", builtin,
	"comments are ignored")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(CommentComment, "comment", builtin,
	"comments are ignored")

int
CommentPound::main(State&, const string_list&) { return Command::NORMAL; }

void
CommentPound::usage(ostream& o) {
	o << "# or \'comment\' ignores the whole line." << endl;
}

int
CommentComment::main(State&, const string_list&) { return Command::NORMAL; }

void
CommentComment::usage(ostream& o) { CommentPound::usage(o); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(All, "all", builtin, "show all commands")

int
All::main(State&, const string_list&) {
	cout << "usage: help all" << endl;
	return Command::NORMAL;
}

void
All::usage(ostream& o) {
	CommandRegistry::list_commands(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Exit, "exit", builtin, "exits simulator")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Quit, "quit", builtin, "exits simulator")

int
Exit::main(State&, const string_list&) {
	return Command::END;
}

void
Exit::usage(ostream& o) {
	o << "exit: " << brief << endl;
}

int
Quit::main(State& s, const string_list& a) {
	return Exit::main(s, a);
}

void
Quit::usage(ostream& o) {
	Exit::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Alias, "alias", builtin,
	"defines alias to auto-expand by the interpreter")

/**
	TODO: Consider storing interpreter aliases in the State object, 
	instead of global registration.  
 */
int
Alias::main(State&, const string_list& a) {
if (a.size() < 3) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	typedef	string_list::const_iterator	const_iterator;
	string_list ac(a);
	ac.pop_front();		// drop the command
	const string al(ac.front());
	ac.pop_front();		// extract alias name, use the rest
	return CommandRegistry::add_alias(al, ac);
}
}

void
Alias::usage(ostream& o) {
	o << "alias <name> <command> [args...]" << endl;
	o <<
"defines an alias that expands into another command and args\n"
"Aliases may reference other aliases as long as no cycles are formed."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnAlias, "unalias", builtin,
	"undefines an alias command")

/**
	Un-aliases one command at a time.  
 */
int
UnAlias::main(State&, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	return CommandRegistry::unalias(a.back());
}
}

void
UnAlias::usage(ostream& o) {
	o << "unalias <name>" << endl;
	o << "undefines an existing command alias" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnAliasAll, "unaliasall", builtin,
	"undefines all alias commands")

/**
	Un-aliases all alias commands.  
 */
int
UnAliasAll::main(State&, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	return CommandRegistry::unalias_all();
}
}

void
UnAliasAll::usage(ostream& o) {
	o << "unaliasall" << endl;
	o << "undefines all existing command aliases" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Aliases, "aliases", builtin,
	"show all registered command aliases")

int
Aliases::main(State&, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	CommandRegistry::list_aliases(cout);
	return Command::NORMAL;
}
}

void
Aliases::usage(ostream& o) {
	o << "aliases" << endl;
	o << "lists all active defined command aliases" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Source, "source", general,
	"execute commands from script file(s)")

/**
	TODO: additional flags to determine error handling, verbosity...
	Include paths for source files?
 */
int
Source::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	string_list::const_iterator i(++a.begin());
	const string_list::const_iterator e(a.end());
	int status = Command::NORMAL;
	for ( ; i!=e && !status; ++i) {
		// termination will depend on error handling
		status = CommandRegistry::source(s, *i);
	}
	if (status) {
		--i;
		cerr << "Error encountered during source \"" <<
			*i << "\"." << endl;
	}
	return status;
}
}

void
Source::usage(ostream& o) {
	o << "source <file(s)>: " << brief << endl;
	o << "read in and run commands from the named file(s)" << endl;
	o << "sourcing terminates upon first error encountered." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddPath, "addpath", general,
	"add search paths for source scripts")

int
AddPath::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.add_source_path(a.back());
	return Command::NORMAL;
}
}

void
AddPath::usage(ostream& o) {
	o << "addpath <path>" << endl;
	o <<
"adds a path to the search-list used to locate sourced command files"
<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Paths, "paths", general,
	"show search paths for source scripts")

int
Paths::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_source_paths(cout);
	return Command::NORMAL;
}
}

void
Paths::usage(ostream& o) {
	o << "paths" << endl;
	o << "show current list of source search paths" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Initialize, "initialize", simulation,
	"resets simulator state and event queue, preserving modes")

int
Initialize::main(State& s, const string_list&) {
	s.initialize();
	return Command::NORMAL;
}

void
Initialize::usage(ostream& o) {
	o << "initialize: " << brief << endl;
	o << "set all nodes to unknown state and clear the event queue" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Reset, "reset", simulation, 
	"resets simulator state, queue, and modes (fresh start)")

int
Reset::main(State& s, const string_list&) {
	s.reset();
	return Command::NORMAL;
}

void
Reset::usage(ostream& o) {
	o << "reset: " << brief << endl;
	o << "restart the entire simulation as if it was just launched" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Command class for stepping through one event at a time from
	the event queue. 
 */
struct Step {
public:
	static const char               name[];
	static const char               brief[];
	static CommandCategory&         category;
	static int      main(State&, const string_list&);
	static void     usage(ostream&);
	static ostream& print_watched_node(ostream&, const State&, 
		const node_index_type, const string&);
	static ostream& print_watched_node(ostream&, const State&, 
		const State::step_return_type&);
	static ostream& print_watched_node(ostream&, const State&, 
		const State::step_return_type&, const string&);
private:
	static const size_t             receipt_id;
};      // end class Step

INITIALIZE_COMMAND_CLASS(Step, "step", simulation,
	"step through event")

static
inline
node_index_type
GET_NODE(const State::step_return_type& x) {
	return x.first;
}

static
inline
node_index_type
GET_CAUSE(const State::step_return_type& x) {
	return x.second;
}

/**
	Yeah, I know looking up already looked up node, but we don't
	care because printing and diagnostics are not performance-critical.  
	\param nodename the name to use for reporting, which need not be
		the canonical name of the node, but some equivalent.  
 */
ostream&
Step::print_watched_node(ostream& o, const State& s, 
		const State::step_return_type& r, const string& nodename) {
	const node_index_type ni = GET_NODE(r);
	// const string nodename(s.get_node_canonical_name(ni));
	const State::node_type& n(s.get_node(ni));
	n.dump_value(o << nodename << " : ");
	const node_index_type ci = GET_CAUSE(r);
	if (ci) {
		const string causename(s.get_node_canonical_name(ci));
		const State::node_type& c(s.get_node(ci));
		c.dump_value(o << "\t[by " << causename << ":=") << ']';
	}
	if (s.show_tcounts()) {
		o << "\t(" << n.tcount << " T)";
	}
	return o << endl;
}

/**
	This automatically uses the canonical name.  
 */
ostream&
Step::print_watched_node(ostream& o, const State& s, 
		const State::step_return_type& r) {
	const node_index_type ni = GET_NODE(r);
	const string nodename(s.get_node_canonical_name(ni));
	return print_watched_node(o, s, r, nodename);
}

/**
	This variation deduces the cause of the given node's last transition,
	the last arriving input to a firing rule.  
	\param nodename the name to use for reporting.  
 */
ostream&
Step::print_watched_node(ostream& o, const State& s, 
		const node_index_type ni, const string& nodename) {
	return print_watched_node(o, s,
		State::step_return_type(ni, s.get_node(ni).get_cause_node()), 
		nodename);
}

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
	} catch (State::excl_exception& exex) {
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
	State::step_return_type ni;
	s.resume();
	try {
	while (!s.stopped() && s.pending_events() &&
			(s.next_event_time() < stop_time) &&
			GET_NODE((ni = s.step()))) {
		// NB: may need specialization for real-valued (float) time.  

		// honor breakpoints?
		// tracing stuff here later...
		const node_type& n(s.get_node(GET_NODE(ni)));
		/***
			The following code should be consistent with
			Cycle::main() and Step::main().
			TODO: factor this out for maintainability.  
		***/
		if (s.watching_all_nodes()) {
			Step::print_watched_node(cout << '\t' << s.time() <<
				'\t', s, ni);
		}
		if (n.is_breakpoint()) {
			// this includes watchpoints
			const bool w = s.is_watching_node(GET_NODE(ni));
			const string nodename(s.get_node_canonical_name(
				GET_NODE(ni)));
			if (w) {
			if (!s.watching_all_nodes()) {
				Step::print_watched_node(cout << '\t' <<
					s.time() << '\t', s, ni);
			}	// else already have message from before
			}
			// channel support
			if (!w) {
				// node is plain breakpoint
				cout << "\t*** break, " <<
					stop_time -s.time() <<
					" time left: `" << nodename <<
					"\' became ";
				n.dump_value(cout) << endl;
				return Command::NORMAL;
				// or Command::BREAK; ?
			}
		}
	}	// end while
	} catch (State::excl_exception& exex) {
		s.inspect_excl_exception(exex, cerr);
		return Command::FATAL;
	}	// no other exceptions
	if (!s.stopped() && s.time() < stop_time) {
		s.update_time(stop_time);
	}
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
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Cycle, "cycle", simulation,
 	"run until event queue empty or breakpoint")

/**
	TODO: add cause tracing.
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
			Step::print_watched_node(cout << '\t' << s.time() <<
				'\t', s, ni);
		}
		if (n.is_breakpoint()) {
			// this includes watchpoints
			const bool w = s.is_watching_node(GET_NODE(ni));
			const string nodename(s.get_node_canonical_name(
				GET_NODE(ni)));
			if (w) {
			if (!s.watching_all_nodes()) {
				Step::print_watched_node(cout << '\t' <<
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
	} catch (State::excl_exception exex) {
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
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Queue, "queue", simulation,
	"show event queue")

int
Queue::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.dump_event_queue(cout);
	return Command::NORMAL;
}
}

void
Queue::usage(ostream& o) {
	o << "queue: " << brief << endl;
	o << "prints events pending in the event queue" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Save, "save", simulation, 
	"saves simulation state to a checkpoint")

int
Save::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& fname(a.back());
	std::ofstream f(fname.c_str(), ios_base::binary);
	if (!f) {
		cerr << "Error opening file \"" << fname <<
			"\" for writing." << endl;
		return Command::BADFILE;
	}
	if (s.save_checkpoint(f)) {
		// error-handling?
		cerr << "Error writing checkpoint!" << endl;
		return Command::UNKNOWN;
	}
	return Command::NORMAL;
}
}

void
Save::usage(ostream& o) {
	o << "save <file>" << endl;
	o << brief << endl;
	o << "(recommend some extension like .prsimckpt)" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Load, "load", simulation, 
	"loads simulation state from a checkpoint")

int
Load::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	const string& fname(a.back());
	std::ifstream f(fname.c_str(), ios_base::binary);
	if (!f) {
		cerr << "Error opening file \"" << fname <<
			"\" for reading." << endl;
		return Command::BADFILE;
	}
	if (s.load_checkpoint(f)) {
		// error-handling?
		cerr << "Error loading checkpoint!" << endl;
		return Command::UNKNOWN;
	}
	return Command::NORMAL;
}
}

void
Load::usage(ostream& o) {
	o << "load <file>" << endl;
	o << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(What, "what", info,
	"print type information of named entity")

int
What::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (parse_name_to_what(cout, a.back(), s.get_module()))
		return Command::BADARG;
	else	return Command::NORMAL;
}
}

void
What::usage(ostream& o) {
	o << "what <name>" << endl;
	o << "prints the type/size of the referenced instance(s)" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Who, "who", info,
	"print aliases of node or structure")

int
Who::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	cout << "aliases of \"" << a.back() << "\":" << endl;
	if (parse_name_to_aliases(cout, a.back(), s.get_module())) {
		return Command::BADARG;
	} else {
		cout << endl;
		return Command::NORMAL;
	}
}
}

void
Who::usage(ostream& o) {
	o << "who <name>" << endl;
	o << "prints all aliases (equivalent names) of the referenced instance"
		<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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
		Step::print_watched_node(cout, s, ni, objname);
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
		s.dump_node_fanin(cout, ni);
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
		s.dump_node_fanout(cout, ni);
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AssertQueue, "assert-queue", info, 
	"assert that the event queue is not empty")

int
AssertQueue::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	if (s.pending_events()) {
		return Command::NORMAL;
	} else {
		cout << "assert failed: expecting non-empty event queue."
			<< endl;
		return Command::FATAL;
	}
}
}

void
AssertQueue::usage(ostream& o) {
	o << "assert-queue" << endl;
	o << "signal an error if the event queue is empty" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Time, "time", info, 
	"display current simulation time")

/**
	TODO:
	Allow "time x" to manually set the time if the event queue is empty. 
	Useful for manually resetting the timer.  
	Allow "time +x" to advance by time, like step.  
		(or reserve for the advance command?)
 */
int
Time::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	cout << "time: " << s.time() << endl;
	return Command::NORMAL;
}
}

void
Time::usage(ostream& o) {
	o << "time" << endl;
	o << "shows the current time" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(Confirm, "confirm", info, 
//	"confirm assertions verbosely")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoConfirm, "noconfirm", info, 
//	"confirm assertions silently")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if PRSIM_SEPARATE_CAUSE_NODE_DIRECTION
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
		// Step::print_watched_node(cout, s, ni, objname);
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
	o << "watches";
	o << "show list of explicitly watched nodes" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//-----------------------------------------------------------------------------
#if WANT_OLD_RANDOM_COMMANDS
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

#undef	DECLARE_AND_DEFINE_ERROR_CONTROL_CLASS

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
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

//=============================================================================
#undef	DECLARE_AND_INITIALIZE_COMMAND_CLASS
//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

