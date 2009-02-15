/**
	\file "sim/command_registry.tcc"
	$Id: command_registry.tcc,v 1.9 2009/02/15 23:07:00 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_REGISTRY_TCC__
#define	__HAC_SIM_COMMAND_REGISTRY_TCC__

#include <iostream>
#include <iterator>
#include <fstream>

#include "sim/command_registry.h"
#include "sim/command_base.h"
#include "sim/command_category.h"
#include "util/readline_wrap.h"
#include "util/named_ifstream_manager.h"
#include "util/tokenize.h"
#include "util/string.tcc"
#include "util/value_saver.h"

// TODO: move library-dependent functionality into library
#ifdef	USE_READLINE
#include <readline/readline.h>
#endif

namespace HAC {
namespace SIM {
using std::cin;
using std::istream;
using std::ifstream;
using std::ostream_iterator;
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

/**
	For block style comments. 
 */
template <class Command>
int
command_registry<Command>::comment_level = 0;

template <class Command>
int
command_registry<Command>::begin_outermost_comment = 0;

/**
	Switch to enable/disable echo-ing each interpreted command.
 */
template <class Command>
string
command_registry<Command>::prompt;

/**
	Switch to enable/disable echo-ing each interpreted command.
 */
template <class Command>
bool
command_registry<Command>::echo_commands = false;

template <class Command>
bool
command_registry<Command>::external_cosimulation = false;

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
	TODO: pass '$' lines to Scheme interpreter, or support Scheme mode
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
	} else if (*cursor == '#') {
		// this catches lines like "#blah"
		return Command::NORMAL;
	} else {
		// static const char begin_comment[] = "/*";
		// static const char end_comment[] = "*/";
		string_list toks;
		tokenize(line, toks);
	if (!toks.empty()) {
		// check for block comments
		const string& front(toks.front());
		const string& back(toks.back());
		const size_t fl = front.size();
		const size_t bl = back.size();
		if (fl >= 2 && front[0] == '/' && front[1] == '*') {
			++comment_level;
#if 0
			if (comment_level == 1) {
				begin_outermost_comment = lineno;
			}
#endif
		}
	if (!comment_level) {
		// check if command is aliased :)
		if (expand_aliases(toks) != Command::NORMAL) {
			return Command::BADARG;
		} else {
			// should be CommandStatus
			const int ret = execute(s, toks);
			if (ret == Command::INTERACT) {
				return interpret_stdin(s);
			}
			return ret;
		}
	}
		if (bl >= 2 && back[bl-2] == '*' && back[bl-1] == '/')  {
			if (comment_level) {
				--comment_level;
			} else {
cerr << "Error: encountered end-comment outside of comment block." << endl;
				return Command::FATAL;
			}
		}
	}	// end if !empty
		return Command::NORMAL;
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
command_registry<Command>::interpret(state_type& s, istream& _cin, 
		const bool interactive) {
	static const char noprompt[] = "";
if (interactive) {
	readline_wrapper rl(interactive ? prompt.c_str() : noprompt);
	// do NOT delete this line string, it is already managed.
	const char* line = NULL;
	int status = Command::NORMAL;
	size_t lineno = 0;
	do {
		++lineno;
		line = rl.gets();	// already eaten leading whitespace
		// GOTCHA: readline eats '\t' characters!?
	if (line) {
#if 0
		cout << "echo: " << line << endl;
#endif
		status = interpret_line(s, line);
		if (status != Command::NORMAL && status != Command::END) {
			cerr << "error at line " << lineno << endl;
		}
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
	// TODO: catch unterminated block comments?
} else {
	// non interactive, skip readline, preserves tab-characters
	return __source(_cin, s);
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Re-opens stdin in a subshell.
 */
template <class Command>
int
command_registry<Command>::interpret_stdin(state_type& s) {
	const util::value_saver<string> p(prompt);
	prompt[prompt.size() -1] = '>';
	prompt += ' ';
	// re-open stdin, don't use cin
	ifstream i(DEV_STDIN);
	return interpret(s, i, true);
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
	size_t lineno = 0;
	int status = Command::NORMAL;
	// const interactive_mode tmp(false);
	string line;
	do {
		++lineno;
		std::getline(i, line);
	if (i) {
#if 0
		cout << "echo: " << line << endl;
#endif
		status = interpret_line(s, line);
	}	// end if
	} while (i && continue_interpreter(status, false));
	if ((status == Command::NORMAL) && comment_level) {
		cerr << "Error: unterminated block comment in source." << endl;
#if 0
		cerr << "(comment block began at line " <<
			begin_outermost_comment << ")" << endl;
#endif
		status = Command::FATAL;
	}
	if (status != Command::NORMAL && status != Command::END) {
		cerr << "... at line " << lineno << endl;
	}
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
	ifstream_manager& ifm(st.get_stream_manager());
	ifstream_manager::placeholder p(ifm, f);
	// ifstream i(f.c_str());
if (p) {
	const auto_file_echo
		__(cout, echo_commands, ifm.top_named_ifstream_name());
	return __source(p.get_stream(), st);
} else {
	cerr << "Error opening file: \"" << f << '\"' << endl;
	p.error_msg(cerr) << endl;
	return Command::BADFILE;
}
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Throws fatal exception when attempting to use such a command
	in co-simulation mode.  
 */
template <class Command>
void
command_registry<Command>::forbid_cosimulation(const string_list& cmd) {
if (external_cosimulation) {
	cerr <<
"Error: attempt to use command that explicitly advances co-simulation!\n"
"cmd: ";
	copy(cmd.begin(), cmd.end(), ostream_iterator<string>(cerr, " "));
	cerr << endl;
	THROW_EXIT;
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
	const string& cmd(*argi);
	if (!cmd.size() || cmd[0] == '#') {
		return Command::NORMAL;
	}
	const typename command_map_type::const_iterator
		p(command_map.find(cmd));
	if (p != command_map.end()) {
		// skip # comments
		if (echo_commands && (args.front() != "#")) {
			cout << "# ";
			copy(argi, args.end(),
				ostream_iterator<string>(cout, " "));
			cout << endl;
		}
		return p->second.main(st, args);
	} else {
		cerr << "Unknown command: " << cmd << endl;
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

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	custom tab-completion for readline library, suitable for
		passing to rl_completion_matches.
	\param _text string to match.
	\param state is 0 to start from scratch.
	TODO: consider list of aliases for candidates.
	\return writeable malloc-allocated string for matching candidate,
		or NULL to signal end-of-list.
 */
template <class Command>
char*
command_registry<Command>::command_generator(const char* _text, int state) {
	typedef	typename command_map_type::const_iterator	const_iterator;
	static const_iterator match_begin, match_end;
#if 0
	cout << "text=" << _text << ", state=" << state << endl;
#endif
	if (!state) {
		const string text(_text);
		string next(text);
		++next[next.length() -1];
		// for lexicographical bounding
		match_begin = command_map.lower_bound(text), 
		match_end = command_map.lower_bound(next);
	}
	if (match_begin != match_end) {
		const const_iterator i(match_begin);
		++match_begin;
#if 0
		cout << " match=" << i->first;
#endif
		return strdup(i->first.c_str());	// malloc
	}	// match_begin == match_end
	return NULL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Master custom tab-completion for readline.
	TODO: refactor this code into readline_wrapper library.
 */
template <class Command>
char**
command_registry<Command>::completion(const char* text, int start, int end) {
	char** matches = NULL;
#ifdef	USE_READLINE
	// TODO: use rl_line_buffer to parse entire line
	// use tokenize
	if (!start) {
		// beginning-of-line: complete command
		matches = rl_completion_matches(text, command_generator);
	}
	// else default to file completion
#endif
	return matches;
}

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_REGISTRY_TCC__

