/**
	\file "sim/prsim/Command.cc"
	All built-in commands should be statically registered 
	in this translation unit to guarantee proper static object
	initialization ordering; the command_map must be constructed
	before entries are registered.  

	TODO: consider using some form of auto-indent
		in the help-system.  

	$Id: Command.cc,v 1.4.2.2 2006/04/06 18:42:11 fang Exp $
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
#include "common/TODO.h"
#include "util/qmap.tcc"
#include "util/readline_wrap.h"
#include "util/libc.h"
#include "util/string.tcc"
#include "util/memory/excl_malloc_ptr.h"
#include "util/tokenize.h"
#include "util/attributes.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_istream.h"
#include "util/using_ostream.h"
using std::ifstream;
using std::copy;
using std::ostream_iterator;
using util::readline_wrapper;
using util::tokenize;
using util::excl_malloc_ptr;
using util::strings::string_to_num;

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
		string_list toks;
		tokenize(line, toks);
		status = execute(s, toks);
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
DECLARE_AND_INITIALIZE_COMMAND_CLASS(CommentPound, "#", builtin, "comment")
DECLARE_AND_INITIALIZE_COMMAND_CLASS(CommentComment, "comment", builtin, "comment")

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
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Source, "source", general,
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
	o << "sourcing terminates upon first error encountered." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(AddPath, "addpath", general,
	"add search paths for source scripts")

int
AddPath::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr);
	return Command::SYNTAX;
} else {
	s.add_source_path(a.back());
	return Command::NORMAL;
}
}

void
AddPath::usage(ostream& o) {
	o << "addpath <path>" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Paths, "paths", general,
	"show search paths for source scripts")

int
Paths::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr);
	return Command::SYNTAX;
} else {
	s.dump_source_paths(cout);
	return Command::NORMAL;
}
}

void
Paths::usage(ostream& o) {
	o << "paths" << endl;
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
private:
	static const size_t             receipt_id;
};      // end class Step

INITIALIZE_COMMAND_CLASS(Step, "step", simulation,
	"step through event")

#if 0
/**
	Like prs_step() from original prsim.
	\return index of the affected node.  
 */
node_index_type
Step::main(State& s) {
	return s.step();
}
#endif

/**
	Like process_step() from original prsim.  
 */
int
Step::main(State& s, const string_list& a) {
if (a.size() > 2) {
	usage(cerr);
	return Command::SYNTAX;
} else {
	typedef	State::time_type		time_type;
	typedef	State::node_type		node_type;
	size_t i;		// the number of discrete time steps
		// not necessarily == the number of discrete events
	node_index_type ni;
	if (a.size() == 2) {
		if (string_to_num(a.back(), i)) {
			cerr << "Error parsing #steps." << endl;
			return Command::BADARG;
		}
	} else {
		i = 1;
	}
	s.resume();
	time_type tm = s.time();
	// could check s.pending_events()
	while (!s.stopped() && i && (ni = s.step())) {
		// if time actually advanced, decrement steps-remaining
		// NB: may need specialization for real-valued (float) time.  
		const time_type ct(s.time());
		if (tm != ct) {
			--i;
			tm = ct;
		}
		const node_type& n(s.get_node(ni));
		/***
			The following code should be consistent with
			Cycle::main() and Advance::main().
			tracing stuff here later...
		***/
		if (s.watching_all_nodes()) {
			const string nodename(s.get_node_canonical_name(ni));
			cout << '\t' << ct << '\t';
			n.dump_value(cout << nodename << " : ") << endl;
			// possibly add cause information too
		}
		if (n.is_breakpoint()) {
			// this includes watchpoints
			const bool w = s.is_watching_node(ni);
			const string nodename(s.get_node_canonical_name(ni));
			if (w) {
			if (!s.watching_all_nodes()) {
				cout << '\t' << ct << '\t';
				n.dump_value(cout << nodename << " : ") << endl;
				// possibly add cause information too
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
	return Command::NORMAL;
}
}	// end Step::main()

void
Step::usage(ostream& o) {
	o << "step [#steps]" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Advance, "advance", simulation,
	"advance the simulation in time units")

int
Advance::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr);
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
	node_index_type ni;
	s.resume();
	while (!s.stopped() && s.pending_events() &&
			(s.next_event_time() < stop_time) && (ni = s.step())) {
		// NB: may need specialization for real-valued (float) time.  

		// honor breakpoints?
		// tracing stuff here later...
		const node_type& n(s.get_node(ni));
		/***
			The following code should be consistent with
			Cycle::main() and Step::main().
			TODO: factor this out for maintainability.  
		***/
		if (s.watching_all_nodes()) {
			const string nodename(s.get_node_canonical_name(ni));
			cout << '\t' << s.time() << '\t';
			n.dump_value(cout << nodename << " : ") << endl;
			// possibly add cause information too
		}
		if (n.is_breakpoint()) {
			// this includes watchpoints
			const bool w = s.is_watching_node(ni);
			const string nodename(s.get_node_canonical_name(ni));
			if (w) {
			if (!s.watching_all_nodes()) {
				cout << '\t' << s.time() << '\t';
				n.dump_value(cout << nodename << " : ") << endl;
				// possibly add cause information too
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Cycle, "cycle", simulation,
 	"run until event queue empty or breakpoint")

/**
	TODO: add cause tracing.
	TODO: handle breakpoint.
	TODO: implement no/globaltime policy for resetting.  
 */
int
Cycle::main(State& s, const string_list & a) {
if (a.size() != 1) {
	usage(cerr);
	return Command::SYNTAX;
} else {
	typedef	State::node_type		node_type;
	node_index_type ni;
	s.resume();	// clear STOP flag
	while (!s.stopped() && (ni = s.step())) {
		if (!ni)	return Command::NORMAL;
		const node_type& n(s.get_node(ni));
		/***
			The following code should be consistent with
			Step::main() and Advance::main().
		***/
		if (s.watching_all_nodes()) {
			const string nodename(s.get_node_canonical_name(ni));
			cout << '\t' << s.time() << '\t';
			n.dump_value(cout << nodename << " : ") << endl;
			// possibly add cause information too
		}
		if (n.is_breakpoint()) {
			// this includes watchpoints
			const bool w = s.is_watching_node(ni);
			const string nodename(s.get_node_canonical_name(ni));
			if (w) {
			if (!s.watching_all_nodes()) {
				cout << '\t' << s.time() << '\t';
				n.dump_value(cout << nodename << " : ") << endl;
				// possibly add cause information too
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
	return Command::NORMAL;
}	// end if
}	// end Cycle::main()

void
Cycle::usage(ostream& o) {
	o << "cycle" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Queue, "queue", simulation,
	"show event queue")

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
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Set, "set", simulation,
	"set node immediately, or after delay")

/**
	Do we need an optional time argument?
 */
int
Set::main(State& s, const string_list& a) {
	const size_t asz = a.size();
if (asz < 3 || asz > 4) {
	usage(cerr);
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
				err = s.set_node_after(ni, val, t);
			} else {
				// schedule at absolute time
				if (t < s.time()) {
					cerr << "Error: time cannot be in the past." << endl;
					return Command::BADARG;
				}
				err = s.set_node_time(ni, val, t);
			}
		} else {
			err = s.set_node(ni, val);
		}
		return (err < 1) ? Command::NORMAL : Command::BADARG;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}	// end Set::main

void
Set::usage(ostream& o) {
	o << "set <node> <0|F|1|T|X|U> [+delay | time]" << endl;
	o <<
"\tWithout delay, node is set immediately.  Relative delay into future\n"
"\tis given by +delay, whereas an absolute time is given without \'+\'."
	<< endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Setr, "setr", simulation,
	"set node to value after random delay")

/**
	NOTE: the random delay has the same distribution as the 
	random delays in the simulator in random mode.  
 */
int
Setr::main(State& s, const string_list& a) {
	const size_t asz = a.size();
if (asz != 3) {
	usage(cerr);
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
			State::random_delay());
		return (err < 1) ? Command::NORMAL : Command::BADARG;
	} else {
		cerr << "No such node found." << endl;
		return Command::BADARG;
	}
}
}	// end Setr::main

void
Setr::usage(ostream& o) {
	o << "setr <node> <0|F|1|T|X|U>" << endl;
	o << "\tsets node at a random time in the future.\n"
"\tNodes with already pending events in queue retain their former event."
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
	usage(cerr);
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoBreakPt, "nobreakpt", simulation,
	"remove breakpoint on node")	// no vector support yet

int
NoBreakPt::main(State& s, const string_list& a) {
if (a.size() < 2) {
	usage(cerr);
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
	usage(cerr);
	return Command::SYNTAX;
} else {
	s.clear_all_breakpoints();
	return Command::NORMAL;
}
}

void
NoBreakPtAll::usage(ostream& o) {
	o << "nobreakptall" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(UnBreakAll, "unbreakall", simulation,
	"alias for \'nobreakptall\'")

int
UnBreakAll::main(State& s, const string_list& a) {
	return NoBreakPtAll::main(s, a);
}

void
UnBreakAll::usage(ostream& o) {
	NoBreakPtAll::usage(o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(What, "what", info,
	"print type information of named entity")

int
What::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr);
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Aliases, "aliases", info,
	"print aliases of node or structure")

int
Aliases::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr);
	return Command::SYNTAX;
} else {
	if (parse_name_to_aliases(cout, a.back(), s.get_module()))
		return Command::BADARG;
	else	return Command::NORMAL;
}
}

void
Aliases::usage(ostream& o) {
	o << "aliases <name>" << endl;
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Get, "get", info,
	"print value of node/vector")

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
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Status, "status", info, 
	"show all nodes matching a state value")

int
Status::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr);
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Fanin, "fanin", info, 
	"print rules that influence a node")

int
Fanin::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr);
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
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Fanout, "fanout", info, 
	"print rules that a node influences")

int
Fanout::main(State& s, const string_list& a) {
if (a.size() != 2) {
	usage(cerr);
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
	usage(cerr);
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
	usage(cerr);
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
	usage(cerr);
	return Command::SYNTAX;
} else {
	cout << "time: " << s.time() << endl;
	return Command::NORMAL;
}
}

void
Time::usage(ostream& o) {
	o << "time" << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(Confirm, "confirm", info, 
//	"confirm assertions verbosely")

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoConfirm, "noconfirm", info, 
//	"confirm assertions silently")

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
	usage(cerr);
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
	usage(cerr);
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
}

//-----------------------------------------------------------------------------
DECLARE_AND_INITIALIZE_COMMAND_CLASS(Random, "random", modes, 
	"use random delays")

int
Random::main(State& s, const string_list& a) {
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
	o << "random";
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_AND_INITIALIZE_COMMAND_CLASS(NoRandom, "norandom", modes, 
	"use non-random delays")

int
NoRandom::main(State& s, const string_list& a) {
if (a.size() != 1) {
	usage(cerr << "usage: ");
	return Command::SYNTAX;
} else {
	s.norandom();
	return Command::NORMAL;
}
}

void
NoRandom::usage(ostream& o) {
	o << "norandom";
}

//=============================================================================
#undef	DECLARE_AND_INITIALIZE_COMMAND_CLASS
//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

