/**
	\file "sim/command_registry.tcc"
	$Id: command_registry.tcc,v 1.23 2011/05/23 01:10:51 fang Exp $
 */

#ifndef	__HAC_SIM_COMMAND_REGISTRY_TCC__
#define	__HAC_SIM_COMMAND_REGISTRY_TCC__

#include <iostream>
#include <iterator>
#include <fstream>

#include "sim/command_registry.hh"
#include "sim/command_base.hh"
#include "sim/command_category.hh"
#include "util/readline_wrap.hh"
#include "util/named_ifstream_manager.hh"
#include "util/tokenize.hh"
#include "util/string.tcc"
#include "util/value_saver.hh"
#include "util/stacktrace.hh"

// TODO: move library-dependent functionality into library
#include "util/readline.h"

namespace HAC {
namespace SIM {
using std::cin;
using std::istream;
using std::ifstream;
using std::ofstream;
using std::ostream_iterator;
using util::readline_wrapper;
using util::ifstream_manager;
using util::strings::eat_whitespace;
using util::tokenize;
using util::value_saver;
#include "util/using_ostream.hh"

REQUIRES_STACKTRACE_STATIC_INIT

//=============================================================================
// class command_registry member/method definitions

/**
	Show whem static members get instantiated.  Debugging only.
 */
template <class Command>
class command_registry<Command>::dummy_type {
#if 0
public:
	dummy_type() { cout << "Hello, world! -- command_registry" << endl; }
	~dummy_type() { cout << "Goodbye, world! -- command_registry" << endl; }
#endif
};

template <class Command>
typename command_registry<Command>::dummy_type
command_registry<Command>::dummy;

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
	Directory stack.
 */
template <class Command>
directory_stack
command_registry<Command>::dir_stack;

template <class Command>
type_scope_manager
command_registry<Command>::type_stack;

template <class Command>
typename command_registry<Command>::history_type
command_registry<Command>::history;

template <class Command>
bool
command_registry<Command>::keep_noninteractive_history = false;

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
	STACKTRACE_VERBOSE;
	typedef	typename category_map_type::mapped_type		mapped_type;
	typedef	typename category_map_type::value_type		value_type;
	typedef	typename category_map_type::iterator		iterator;
	const std::pair<iterator, bool>
		probe(category_map.insert(value_type(c.name(), &c)));
	if (!probe.second) {
		cerr << "category \'" << c.name() <<
			"\' has already been registered." << endl;
		THROW_EXIT;
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
	STACKTRACE_VERBOSE;
	typedef	C	command_class;
	command_category_type& cat((*command_class::category)());
	const Command temp(command_class::name, command_class::brief,
		&cat, &command_class::main, 
		&command_class::usage, &Completer<command_class>);
	typedef	typename command_map_type::mapped_type		mapped_type;
	typedef	typename command_map_type::value_type		value_type;
	typedef	typename command_map_type::iterator		iterator;
	const string& s(command_class::name);
	const std::pair<iterator, bool>
		probe(command_map.insert(value_type(s, temp)));
	if (!probe.second) {
		cerr << "command \'" << s << "\' has already been "
			"registered globally." << endl;
		THROW_EXIT;
	}
	cat.register_command(temp);
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
// directory commands

template <class Command>
void
command_registry<Command>::reset_dirs(void) {
	return dir_stack.reset();
}

template <class Command>
bool
command_registry<Command>::change_dir(const string& d) {
	return dir_stack.change_directory(d);
}

template <class Command>
bool
command_registry<Command>::change_dir_abs(const string& d) {
	return dir_stack.change_directory(dir_stack.absolute_prefix +d);
}

template <class Command>
bool
command_registry<Command>::push_dir(const string& d) {
	return dir_stack.push_directory(d);
}

template <class Command>
bool
command_registry<Command>::pop_dir(void) {
	return dir_stack.pop_directory();
}

template <class Command>
const string&
command_registry<Command>::working_dir(void) {
	return dir_stack.current_working_directory();
}

template <class Command>
string
command_registry<Command>::prepend_working_dir(const string& s) {
	return dir_stack.transform(s);
}

template <class Command>
ostream&
command_registry<Command>::show_working_dir(ostream& o) {
	return dir_stack.dump_working_directory(o);
}

template <class Command>
ostream&
command_registry<Command>::show_dirs(ostream& o) {
	return dir_stack.dump_stack(o);
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
command_registry<Command>::interpret_line(state_type& s, const string& line, 
		const bool record) {
	const char* cursor = line.c_str();
	if (*cursor == '!') {
		++cursor;
		eat_whitespace(cursor);
		if (record) {
			// record system commands too
			history.push_back(string("!") +cursor);
		}
		const int es = system(cursor);
		// let status remain as is, for now
		// TODO: determine exit behavior
		if (es) {
			cerr << "*** Exit " << es << endl;
		}
		return Command::NORMAL;
	} else if (*cursor == '#') {
		// this catches lines like "#blah"
		// don't record comments
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
		const bool alter_source_cmd = (front == "source") &&
			keep_noninteractive_history;
	if (!comment_level) {
		if (record) {
			// record before expanding alias
		if (front == "history-save") {
			// always ignore this one, as a safety check
			history.push_back(string("# ") +line);
		} else if (alter_source_cmd) {
			history.push_back(string("# BEGIN ") +line);
		} else {
			history.push_back(line);
		}
		}
		// check if command is aliased :)
		if (expand_aliases(toks) != Command::NORMAL) {
			return Command::BADARG;
		} else {
			// should be CommandStatus
			const int ret = execute(s, toks);
			if (record && alter_source_cmd) {
				history.push_back(string("# END ") +line);
			}
			if (ret == Command::INTERACT) {
				return interpret_stdin(s);
			}
			return ret;
		}
	}
		// crude comment detection
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
	// TODO: prompt to include new history line count?
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
		status = interpret_line(s, line,
			interactive || keep_noninteractive_history);
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
	const value_saver<string> p(prompt);
	prompt[prompt.size() -1] = '>';
	prompt += ' ';
	// re-open stdin, don't use cin
	ifstream i(DEV_STDIN);
	// WARNING: not necessarily portable
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
		status = interpret_line(s, line, keep_noninteractive_history);
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
	Return a pair of integers in the history range [0,end].
	Range is interpreted as half-open: [x,y) 
	If start is negative, count backwards from the end.
	If end is positive, it is relative to start.
	If end is negative, it is backwards from the end.
	Returning [x,x) is a null range.
 */
template <class Command>
std::pair<int, int>
command_registry<Command>::history_range(const int start, const int end) {
	const int s = int(history.size());
	if (start >= s) {
		cerr << "Warning: reference past end-of-history." << endl;
		return std::make_pair<int, int>(0, 0);	// don't care
	}
	if (s +start <= 0) {
		cerr << "Warning: reference past beginning-of-history." << endl;
		return std::make_pair<int, int>(0, 0);	// don't care
	}
	const int f = (start < 0 ? s +start -1 : // omit last command, 
		start < s ? start : s);
	const int l = (end < 0 ? s +end :
		(end && (f +end < s) ? f +end: s));
#if 0
	const bool r = (l < f);
	const int ll = r ? l : f;
	if (r) {
		cerr <<
"Warning: history-end-reference is before beginning, taking start." << endl;
		return std::make_pair(f, f+1);
	}
	INVARIANT(ll <= s);
	INVARIANT(f <= ll);
#endif
	INVARIANT(f >= 0);
	INVARIANT(l <= s);
	return std::make_pair(f, l);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Command>
ostream&
command_registry<Command>::dump_history(ostream& o, 
	const int start, const int end) {
	std::pair<int, int> p(history_range(start, end));
	int& i(p.first);
	int& l(p.second);
	o << "history[" << i << ".." << l-1 << "]:" << endl;
	for ( ; i<l; ++i) {
		// print 0-based index
		o << i << '\t' << history[i] << endl;
	}
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Command>
int
command_registry<Command>::write_history(const string& fn) {
	ofstream ofs(fn.c_str());
	if (ofs) {
		copy(history.begin(), history.end(), 
			ostream_iterator<string>(ofs, "\n"));
	} else {
		cerr << "Error opening file `" << fn << "' for writing." << endl;
		return Command::BADARG;
	}
	return Command::NORMAL;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Be sure to exclude self!
 */
template <class Command>
int
command_registry<Command>::rerun(state_type& s,
		const int start, const int end) {
	std::pair<int, int> p(history_range(start, end));
	int& i(p.first);
	const int& l(p.second);
	// protect against self recursion! 
	// with upper bound excluding this command!
//	cout << "history-rerun[" << i << ".." << l-1 << "]:" << endl;
	INVARIANT(i >= 0);
	INVARIANT(l <= int(history.size()));
	for ( ; i<l; ++i) {
		// don't re-record the rerun lines
		const int status = interpret_line(s, history[i], false);
		if (status) {
			cerr << "*** Stopped re-running after line " << i 
				<< " due to error." << endl;
			return status;
		}
	}
	return Command::NORMAL;
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
	This function is not re-entrant!
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
	NEVER_NULL(_text);
	if (!state) {
	if (_text[0]) {
		const string text(_text);
		string next(text);
		++next[next.length() -1];
		// for lexicographical bounding
		match_begin = command_map.lower_bound(text), 
		match_end = command_map.lower_bound(next);
	} else {
		// empty string, return all commands
		match_begin = command_map.begin();
		match_end = command_map.end();
	}
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
	TODO: context-sensitive completion (e.g. type-local scope)
	TODO: refactor this code into readline_wrapper library.
 */
template <class Command>
char**
command_registry<Command>::completion(const char* text, int start, int end) {
	typedef	typename command_map_type::const_iterator	const_iterator;
#if 0
	// for debugging/development
	cout << "[text=" << text << ", start=" << start <<
		", end=" << end << ']' << endl;
#endif
#ifdef	USE_READLINE
	// rl_attempted_completion_over = true;
	// don't fallback to readline's default completer even 
	// if this returns no matches

	// restore some default that may have been overridden
	rl_completion_append_character = ' ';
	rl_completion_display_matches_hook = NULL;

	// TODO: use rl_line_buffer to parse entire line
	// use tokenize
	// eat leading whitespace
	const char* buf = rl_line_buffer;
	eat_whitespace(buf);
	if (!start || (rl_line_buffer +start == buf)) {
		// beginning-of-line or leading whitespace: complete command
		return rl_completion_matches(text, command_generator);
	} else if (rl_line_buffer +start != buf) {
		// then we have at least one whole token for the command
		// attempt command-specific completion
		string_list toks;
		tokenize(buf, toks);
		const string& key(toks.front());
		if (key[0] == '!') {
			// shell command, fallback to filename completion
			return NULL;
		}
		const const_iterator f(command_map.find(key));
		if (f == command_map.end()) {
			// invalid command
			cerr << "\nNo such command: " << key << endl;
			return NULL;
		}
		const command_completer gen(f->second.completer());
		if (gen) {
			// TODO: be able to override readline hooks here
			// making local modifications
			// const value_saver<int>	// don't add space
			rl_completion_append_character = '\0';
#ifdef	HAVE_BSDEDITLINE
#define	VOID_FUNCTION_CAST(x)		reinterpret_cast<void (*)(void)>(x)
#else
// editline completely fubar'd this type
#define	VOID_FUNCTION_CAST(x)		x
#endif
			rl_completion_display_matches_hook =
			VOID_FUNCTION_CAST(display_hierarchical_matches_hook);
#undef	VOID_FUCNTION_CAST
			return rl_completion_matches(text, gen);
		}
	}
#endif
	return NULL;
}

//=============================================================================
#ifndef	USE_READLINE
// dummy function pointer, for when we are not using any readline
static
char** (*__dummy_completion)(const char*, int, int) = NULL;
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Without any hierarchical instance completion module.
 */
template <class Command>
command_registry<Command>::readline_init::readline_init() :
		_compl(
#ifdef	USE_READLINE
			rl_attempted_completion_function,
#else
			__dummy_completion, 
#endif
			completion), 
		_mod(instance_completion_module, NULL),
		_dirs(instance_completion_dirs, &dir_stack)
{
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Uses compiled module for instance hierarchy name completion.
 */
template <class Command>
command_registry<Command>::readline_init::readline_init(const module& m) :
		_compl(
#ifdef	USE_READLINE
			rl_attempted_completion_function,
#else
			__dummy_completion, 
#endif
			completion), 
		_mod(instance_completion_module, &m),
		// this may conflict if we ever instantiate
		// multiple simulators' directory stacks simultanously 
		// (conflict)
		_dirs(instance_completion_dirs, &dir_stack)
{
#if 0
	// doesn't do what I want...
	static const char wb[] = " \t\n\"\\'`@$><=;|&{(.";	// added '.'
	rl_completer_word_break_characters = wb;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
template <class Command>
command_registry<Command>::readline_init::~readline_init() { }

//=============================================================================
}	// end namespace SIM
}	// end namespace HAC

#endif	// __HAC_SIM_COMMAND_REGISTRY_TCC__

