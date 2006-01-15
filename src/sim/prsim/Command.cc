/**
	\file "sim/prsim/Command.cc"
	All built-in commands should be statically registered 
	in this translation unit to guarantee proper static object
	initialization ordering; the command_map must be constructed
	before entries are registered.  

	TODO: consider using some form of auto-indent
		in the help-system.  

	$Id: Command.cc,v 1.1.2.1.2.1 2006/01/15 22:02:50 fang Exp $
 */

#include "util/static_trace.h"
DEFAULT_STATIC_TRACE_BEGIN

#include "sim/prsim/Command.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <iterator>
#include "sim/prsim/State.h"
#include "util/qmap.tcc"
#include "util/readline_wrap.h"
#include "util/libc.h"
#include "util/memory/excl_malloc_ptr.h"
#include "util/tokenize.h"

namespace HAC {
namespace SIM {
namespace PRSIM {
#include "util/using_ostream.h"
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
		return -1;
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
 */
CommandRegistry::command_map_type
CommandRegistry::command_map;

/**
	Global static initialization of the category-map.
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
	int status = 0;
	size_t lineno = 1;
	do {
		line = rl.gets();	// already eaten leading whitespace
	if (line) {
		string_list toks;
		tokenize(line, toks);
		status = execute(s, toks);
		++lineno;
	}
	} while (line && (!status || interactive));
	// end-line for neatness
	if (!line)	cout << endl;
	// not sure if the following is good idea
	if (status && !interactive) {
		// useful for tracing errors in sourced files
		cerr << "Error detected at line " << lineno <<
			", aborting commands." << endl;
		return status;
	} else	return 0;
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

static CommandCategory builtin("builtin", "built-in commands");
static CommandCategory general("general", "general commands");
static CommandCategory simulation("simulation", "simulation commands");
static CommandCategory channel("channel", "channel commands");

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
	return 0;
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
	const size_t s = args.size();
	INVARIANT(s);
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
	return 0;
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

int
CommentPound::main(State&, const string_list&) { return 0; }

void
CommentPound::usage(ostream& o) {
	o << "# or \'comment\' ignores the whole line." << endl;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_COMMAND_CLASS(CommentComment, "comment", builtin, "comment")

int
CommentComment::main(State&, const string_list&) { return 0; }

void
CommentComment::usage(ostream& o) { CommentPound::usage(o); }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
DECLARE_COMMAND_CLASS(All, "all", builtin, "show all commands")

int
All::main(State&, const string_list&) {
	cout << "usage: help all" << endl;
}

void
All::usage(ostream& o) {
	CommandRegistry::list_commands(o);
}

//=============================================================================
}	// end namespace PRSIM
}	// end namespace SIM
}	// end namespace HAC

DEFAULT_STATIC_TRACE_END

