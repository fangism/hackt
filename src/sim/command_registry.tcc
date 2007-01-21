/**
	\file "sim/command_registry.tcc"
	$Id: command_registry.tcc,v 1.2 2007/01/21 06:00:29 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_REGISTRY_TCC__
#define	__HAC_SIM_COMMAND_REGISTRY_TCC__

#include <iostream>

#include "sim/command_registry.h"
#include "sim/command.h"
#include "sim/command_category.h"
#include "util/readline_wrap.h"
#include "util/named_ifstream_manager.h"
#include "util/tokenize.h"
#include "util/string.tcc"

namespace HAC {
namespace SIM {
using std::cin;
using std::istream;
using util::readline_wrapper;
using util::ifstream_manager;
using util::strings::eat_whitespace;
using util::tokenize;
#include "util/using_ostream.h"

//=============================================================================
// class command_registry member/method definitions

/**
	Global static initialization of the command-map.
	This MUST be initialized before any commands are registered.
 */
template <class Command>
typename command_registry<Command>::command_map_type
command_registry<Command>::command_map;

/**
	Global static initialization of the category-map.
	This MUST be initialized before any categories are registered.  
 */
template <class Command>
typename command_registry<Command>::category_map_type
command_registry<Command>::category_map;


/**
	Global static initialization of aliases.  
	Or should we have one per interpreter?
 */
template <class Command>
typename command_registry<Command>::aliases_map_type
command_registry<Command>::aliases;

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Command>
size_t
command_registry<Command>::register_category(command_category_type& c) {
	typedef	typename category_map_type::mapped_type		mapped_type;
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
template <class Command>
template <class C>
size_t
command_registry<Command>::register_command(void) {
	typedef	C	command_class;
	const Command temp(command_class::name, command_class::brief,
		&command_class::category, &command_class::main, 
		&command_class::usage);
	typedef	typename command_map_type::mapped_type		mapped_type;
	const string& s(command_class::name);
	mapped_type& probe(command_map[s]);
	if (probe) {
		cerr << "command \'" << s << "\' has already been "
			"registered globally." << endl;
		THROW_EXIT;
	} else {
		probe = temp;
	}
	command_class::category.register_command(temp);
	return command_map.size();
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Prints list of command categories.  
 */
template <class Command>
void
command_registry<Command>::list_categories(ostream& o) {
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
template <class Command>
void
command_registry<Command>::list_commands(ostream& o) {
	typedef	typename category_map_type::const_iterator	const_iterator;
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
template <class Command>
bool
command_registry<Command>::continue_interpreter(const int _status, const bool ia) {
	return (_status != Command::END) && (!_status || ia);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#if 0
// don't delete this yet...
class command_registry<Command>::interactive_mode {
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
template <class Command>
int
command_registry<Command>::add_alias(const string& a, const string_list& c) {
	if (command_map.find(a) != command_map.end()) {
		cerr << "\'" << a << "\' is already a command, "
			"cannot be used to register an alias." << endl;
		return Command::BADARG;
	} else if (category_map.find(a) != category_map.end()) {
		cerr << "\'" << a << "\' is already a category, "
			"cannot be used to register an alias." << endl;
		return Command::BADARG;
	}
	else return command_aliases_base::add_alias(aliases, a, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unregisters an alias.  
	Doesn't check for former existence.  
 */
template <class Command>
int
command_registry<Command>::unalias(const string& a) {
	return command_aliases_base::unalias(aliases, a);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Unregisters all aliases.  
 */
template <class Command>
int
command_registry<Command>::unalias_all(void) {
	return command_aliases_base::unalias_all(aliases);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Command>
void
command_registry<Command>::list_aliases(ostream& o) {
	command_aliases_base::list_aliases(aliases, o);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Expands prefix aliases, just like sh.  
	\pre there exist no cyclic aliases.  
	\pre no existing commands are overridden by aliases.  
 */
template <class Command>
int
command_registry<Command>::expand_aliases(string_list& c) {
	return command_aliases_base::expand_aliases(aliases, c);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Interprets a single command line.  
	Capable of executing shell commands with '!' prefix.  
	Now capable of expanding alias commands.  
	\pre there are no cyclic aliases.  
 */
template <class Command>
int
command_registry<Command>::interpret_line(state_type& s, const string& line) {
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
template <class Command>
int
command_registry<Command>::interpret(state_type& s, const bool interactive) {
	static const char noprompt[] = "";
if (interactive) {
	readline_wrapper rl(interactive ? s.get_prompt().c_str() : noprompt);
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
template <class Command>
int
command_registry<Command>::__source(istream& i, state_type& s) {
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
	Prerequisite: the state class must have a get_stream_manager interface.
	\param f the file name.  
	\param st the simulator state.  
	\return status of the interpreter.
	TODO: manage the interactive bit with an object/class, 
		for prope stack restoration with exception safety.  
	TODO: keep track of already opened files to prevent 
		cyclic sourcing...
 */
template <class Command>
int
command_registry<Command>::source(state_type& st, const string& f) {
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
template <class Command>
int
command_registry<Command>::execute(state_type& st, const string_list& args) {
	const size_t s = args.size();
if (s) {
	const string_list::const_iterator argi(args.begin());
	const typename command_map_type::const_iterator
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
template <class Command>
bool
command_registry<Command>::help_command(ostream& o, const string& c) {
	typedef	typename command_map_type::const_iterator	const_iterator;
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
template <class Command>
bool
command_registry<Command>::help_category(ostream& o, const string& c) {
	typedef	typename category_map_type::const_iterator	const_iterator;
	const_iterator probe(category_map.find(c));
	if (probe != category_map.end()) {
		probe->second->list(o);
		return true;
	} else	return false;
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_REGISTRY_TCC__

