/**
	\file "sim/prsim/Command.cc"
	All built-in commands should be statically registered 
	in this translation unit to guarantee proper static object
	initialization ordering; the command_map must be constructed
	before entries are registered.  

	TODO: consider using some form of auto-indent
		in the help-system.  

	$Id: Command.cc,v 1.3.8.1 2006/03/23 07:05:17 fang Exp $
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "sim/prsim/Command.h"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <iterator>
#include "sim/prsim/State.h"
#include "sim/prsim/Reference.h"
#include "util/qmap.tcc"
#include "util/readline_wrap.h"
#include "util/libc.h"
#include "util/memory/excl_malloc_ptr.h"
#include "util/tokenize.h"
#include "util/attributes.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
using std::ifstream;
using std::copy;
using std::ostream_iterator;
using util::readline_wrapper;
using util::tokenize;
using util::excl_malloc_ptr;
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
	readline_wrapper rl(prompt);
	// do NOT delete this line, it is already managed.
	const char* line = NULL;
	int status = Command::NORMAL;
	size_t lineno = 1;
	do {
		line = rl.gets();	// already eaten leading whitespace
	if (line) {
		string_list toks;
		tokenize(line, toks);
		status = execute(s, toks);
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	\param f the file name.  
	\return status of the interpreter.
	TODO: manage the interactive bit with an object/class, 
		for prope stack restoration with exception safety.  
	TODO: keep track of already opened files to prevent 
		cyclic sourcing...
 */
int
CommandRegistry::source(State& st, const string& f) {
	ifstream i(f.c_str());
if (i) {
	int status = Command::NORMAL;
	// const interactive_mode tmp(false);
	string line;
	do {
		std::getline(i, line);
	if (i) {
#if 0
		// echo-debugging
		cout << "echo: " << line << endl;
#endif
		string_list toks;
		tokenize(line, toks);
		status = execute(st, toks);
	}	// end if
	} while (i && continue_interpreter(status, false));
	return status;
} else {
	cerr << "Error opening file: " << f << endl;
	return Command::BADFILE;
}
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

//=============================================================================
// local static CommandCategories
// feel free to add categories here

static CommandCategory
	aliases("aliases", "user-defined alias commands"),
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
	\param class_name the name of the class.
	\param cmd_name the string-quoted name of the command.
	\param cmd_category the cateory object with which to associate.  
	\param brief_str one-line quoted-string description.
 */
#define	DECLARE_COMMAND_CLASS(class_name, cmd_name, cmd_category, brief_str) \
struct class_name {							\
public:									\
	static const char		name[];				\
	static const char		brief[];			\
	static CommandCategory&		category;			\
	static int	main(State&, const string_list&);		\
	static void	usage(ostream&);				\
private:								\
	static const size_t		receipt_id;			\
};									\
const char class_name::name[] = cmd_name;				\
const char class_name::brief[] = brief_str;				\
CommandCategory& class_name::category(cmd_category);			\
const size_t class_name::receipt_id =					\
	CommandRegistry::register_command<class_name>();

//-----------------------------------------------------------------------------
DECLARE_COMMAND_CLASS(Echo, "echo", builtin, 
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
DECLARE_COMMAND_CLASS(Help, "help", builtin,
	"show available commands and categories")

/**
	\param args the tokenized command line, including the "help" command
		as the first token.
 */
int
Help::main(State&, const string_list& args) {
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

void
Help::usage(ostream& o) {
	o << "help: lists available commands and categories" << endl;
	o << "to get usage for a specific command or category, "
		"run: help <name>" << endl;
	o << "help all: lists all commands across all categories" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_COMMAND_CLASS(CommentPound, "#", builtin, "comment")
DECLARE_COMMAND_CLASS(CommentComment, "comment", builtin, "comment")

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
DECLARE_COMMAND_CLASS(All, "all", builtin, "show all commands")

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
DECLARE_COMMAND_CLASS(Exit, "exit", builtin, "exits simulator")
DECLARE_COMMAND_CLASS(Quit, "quit", builtin, "exits simulator")

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
DECLARE_COMMAND_CLASS(Source, "source", simulation,
	"execute commands from script file(s)")

/**
	TODO: additional flags to determine error handling, verbosity...
	Include paths for source files?
 */
int
Source::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr);
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
		cerr << "Error encountered during source." << endl;
	}
	return status;
}
}

void
Source::usage(ostream& o) {
	o << "source <file(s)>: " << brief << endl;
	o << "sourcing terminates upon first error encountered." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_COMMAND_CLASS(Initialize, "initialize", simulation,
	"resets simulator state and event queue")

int
Initialize::main(State& s, const string_list&) {
	s.initialize();
	return Command::NORMAL;
}

void
Initialize::usage(ostream& o) {
	o << "initialize: " << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(Cycle, "cycle", simulation,
// 	"run until event queue empty")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(Step, "step", simulation, "step through event")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_COMMAND_CLASS(Queue, "queue", simulation, "show event queue")

int
Queue::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr);
	return Command::SYNTAX;
} else {
	s.dump_event_queue(cout);
	return Command::NORMAL;
}
}

void
Queue::usage(ostream& o) {
	o << "queue: " << brief << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_COMMAND_CLASS(Set, "set", simulation, "set node immediately")

/**
	Do we need an optional time argument?
 */
int
Set::main(State& s, const string_list& a) {
if (a.size() != 3) {
	usage(cerr);
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	typedef	State::event_type		event_type;
	typedef	State::event_placeholder_type	event_placeholder_type;
	// now I'm wishing string_list was string_vector... :) can do!
	const string& objname(*++a.begin());	// node name
	const string& _val(a.back());	// node value
	// valid values are 0, 1, 2(X)
	const char val = node_type::string_to_value(_val);
	if (val < 0) {
		return Command::SYNTAX;
	}
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		int err = s.set_node(ni, val);
		return (err < 1) ? Command::NORMAL : Command::BADARG;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}

void
Set::usage(ostream& o) {
	o << "set <node> <value 0|1|X>" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(Setr, "setr", simulation, "set node after random delay")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(Setrwhen, "setrwhen", simulation,
//	"set node with random delay after event")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(Breakpt, "breakpt", simulation,
//	"set breakpoint on node/vector")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(NoBreakpt, "nobreakpt", simulation,
//	"remove breakpoint on node/vector")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(NoBreakptAll, "nobreakptall", simulation,
//	"remove all breakpoints")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_COMMAND_CLASS(Get, "get", info, "print value of node/vector")

/**
	TODO: if instance referenced is an aggregate, 
		then print the values of all constituents.
		Reserve that for the getall command.
	Status: done.
	TODO: allow access to private members in Reference.cc.  
 */
int
Get::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr);
	return Command::SYNTAX;
} else {
	const string& objname(a.back());
	const node_index_type ni = parse_node_to_index(objname, s.get_module());
	if (ni) {
		// we have ni = the canonically allocated index of the bool node
		// just look it up in the node_pool
		// const state_manager& sm(s.get_module().get_state_manager());
		const State::node_type& n(s.get_node(ni));
		n.dump_value(cout << objname << " : ") << endl;
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(Fanin, "fanin", info, 
//	"print rules that influence a node")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(Fanout, "fanout", info, 
//	"print rules that influence a node")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// will category conflict with command?
// DECLARE_COMMAND_CLASS(Info, "info", info, 
//	"print information about a node/vector")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(Assert, "assert", info, 
//	"error if node is NOT expected value")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(AssertN, "assertn", info, 
//	"error if node IS expected value")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(Confirm, "confirm", info, 
//	"confirm assertions verbosely")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_COMMAND_CLASS(NoConfirm, "noconfirm", info, 
//	"confirm assertions silently")

#undef	DECLARE_COMMAND_CLASS
//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

