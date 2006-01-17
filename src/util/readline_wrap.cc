/**
	\file "util/readline_wrap.cc"
	Simplified wrapper implementation for readline.  
	$Id: readline_wrap.cc,v 1.1.4.6 2006/01/17 04:41:08 fang Exp $
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_GETS			0 && ENABLE_STACKTRACE

#include <iostream>
#include "util/readline_wrap.h"
#include "util/readline.h"
#include "util/string.h"
#include "util/stacktrace.h"


/**
	Arbitrarily chosen buffer line size.  
	What's a good way of automating this?
 */
#ifndef	READLINE_BUFFER_SIZE
#define	READLINE_BUFFER_SIZE		16384
#endif

namespace util {
#include "util/using_ostream.h"
using namespace strings;		// for some utility functions

//=============================================================================
// class readline_wrapper method definitions

// TODO: do we need to initialize and free?

readline_wrapper::readline_wrapper() : hold_line(NULL), prompt(),
		_skip_blank_lines(true) { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
readline_wrapper::readline_wrapper(const string& s) :
		hold_line(NULL), prompt(s),
		_skip_blank_lines(true) {
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
readline_wrapper::~readline_wrapper() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the readline prompt.  
 */
void
readline_wrapper::set_prompt(const string& s) {
	prompt = s;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds line to history if readline is enabled.  
	Recommendation: trim whitespace before passing the string pointer.  
	\param hl the current hold line, must be non-NULL
		and point to a non-NUL character.  
 */
readline_wrapper::const_char_type*
readline_wrapper::__add_history(const_char_type* const hl) {
#if USE_READLINE
	const_char_type* cursor = hl;
	add_history(RL_CONST_CAST(cursor));
	return cursor;
#else
	history.push_back(string(hl));
	return hl;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Same as gets() but using the previously set prompt.  
 */
readline_wrapper::const_char_type*
readline_wrapper::gets(void) {
	return this->gets(prompt);
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: some ancient version of readline haas prototype without
	'const' in ANY of the arguments (v. 4.1, ca. 2002).  
	That is utterly unaacceptable.  We provide an internal workaround.  
	TODO: handle line continuations (ending with '\')
	TODO: parse semicolons (see sh behavior)
	\return allocated line, or NULL if EOF.  
 */
readline_wrapper::const_char_type*
readline_wrapper::gets(const string& _prompt) {
	// this will automatically release the last line's memory
	STACKTRACE_VERBOSE;
	// NOTE: NULL does not count as blank, and should be returned as such
	// BSD editline behavior is different
	// check with fgets as well
	bool is_blank;
	const char* cursor;
do {
#if USE_READLINE
	// (also covers editline)
	// NOTE: some ASS-version of readline accepts a char* 
	// for the prompt argument and trips an error here, 
	// hence the const_cast
	hold_line = hold_line_type(readline(
		RL_CONST_CAST(_prompt.c_str())));
#else
	cout << _prompt;
	get_line_type get_line(static_cast<char_type*>(
		malloc(sizeof(char_type) *READLINE_BUFFER_SIZE)));
	fgets(&*get_line, READLINE_BUFFER_SIZE, stdin);
	hold_line = get_line;	// transfer ownership
#endif
	// NOTE: BSD editline does not return NULL on EOF
	// it returns a string with strlen() == 0
	// therefore, we need one additional test in the condition check.  
	// NOTE: GNU readline returns a string of length 0 on an empty line
	// which should NOT be mistaken for EOF
#if DEBUG_GETS
	// echo debugging
	if (hold_line) {
		cout << "length: " << strlen(&*hold_line) << endl;
		cout << &*hold_line << endl;
	} else {
		cout << "NULL hold_line." << endl;
	}
#endif
	if (hold_line && !feof(stdin)) {
		cursor = &*hold_line;
		eat_whitespace(cursor);
#if DEBUG_GETS
		cout << "remaining length: " << strlen(cursor) << endl;
		cout << "remaining line:" << endl;
		cout << cursor << endl;
#endif
		// may have to eat newline
		is_blank = (!strlen(cursor) || (*cursor == '\n'));
	} else {
		cursor = NULL;
		is_blank = false;
	}
	// stop as soon as
	// 1) we have EOF (null cursor)
	// 2) or we have a non-blank-line
	// 3) or we're also accepting blank lines
} while (cursor && _skip_blank_lines && is_blank);
	if (cursor) {
		return __add_history(cursor);
	}
	return cursor;
}	// end readline_wrapper::gets()

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
ostream&
readline_wrapper::version_string(ostream& o) {
#if	defined(HAVE_GNUREADLINE)
	return o << "GNU readline " << rl_library_version;
#elif	defined(HAVE_BSDEDITLINE)
	return o << "BSD editline " << rl_library_version;
#else
	return o << "none";
#endif
}

//=============================================================================
}	// end namespace util

