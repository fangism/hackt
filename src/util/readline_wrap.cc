/**
	\file "util/readline_wrap.cc"
	Simplified wrapper implementation for readline.  
	$Id: readline_wrap.cc,v 1.6 2010/03/17 06:32:48 fang Exp $
	TODO: for editline/histedit, set H_SETUNIQUE flag.
 */

#define	ENABLE_STACKTRACE		0
#define	DEBUG_GETS			0 && ENABLE_STACKTRACE

#include <iostream>
#include <cstdio>			// for stdin, feof
#include "util/readline_wrap.hh"
#include "util/readline.h"
#include "util/string.hh"
#include "util/stacktrace.hh"
#include "util/value_saver.hh"

/**
	Arbitrarily chosen buffer line size.  
	What's a good way of automating this?
 */
#ifndef	READLINE_BUFFER_SIZE
#define	READLINE_BUFFER_SIZE		16384
#endif

namespace util {
#include "util/using_ostream.hh"
using namespace strings;		// for some utility functions

//=============================================================================
// class readline_wrapper method definitions

#ifdef	USE_HISTEDIT
string
readline_wrapper::current_prompt = string();

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
const readline_wrapper::char_type*
readline_wrapper::el_prompt(EditLine* e) {
	return current_prompt.c_str();
}
#endif

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// TODO: do we need to initialize and free?

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
readline_wrapper::readline_wrapper(const string& s) :
		hold_line(NULL),
#ifdef	USE_HISTEDIT
		_editline(el_init(PACKAGE, stdin, stdout, stderr)), 
		_el_history(history_init()), 
#else
		prompt(s),
#endif
		_skip_blank_lines(true) {
#ifdef	USE_HISTEDIT
	former_prompt = current_prompt;
	current_prompt = s;
	el_set(&*_editline, EL_PROMPT, &el_prompt);	// sticky
	// the rest is ripped from "tools/prsim/prsim.c"
	el_set(&*_editline, EL_EDITOR, "emacs");
	HistEvent ev;
	history(&*_el_history, &ev, H_SETSIZE, 1000);
	el_set(&*_editline, EL_HIST, &history, &*_el_history);
	el_set(&*_editline, EL_BIND, "^P", "ed-prev-history", NULL);
	el_set(&*_editline, EL_BIND, "^N", "ed-next-history", NULL);
	// el_set(&*_editline, EL_BIND, "^D", "ed-end-of-file", NULL);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	For histedit, restore the former prompt upon destruction.
 */
readline_wrapper::~readline_wrapper() {
#ifdef	USE_HISTEDIT
	current_prompt = former_prompt;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Sets the readline prompt.  
 */
void
readline_wrapper::set_prompt(const string& s) {
#ifdef	USE_HISTEDIT
	current_prompt = s;
#else
	prompt = s;
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Adds line to history if readline is enabled.  
	Recommendation: trim whitespace before passing the string pointer.  
	\param hl the current hold line, must be non-NULL
		and point to a non-NUL character.  
	\return same pointer to string added, else NULL if there is an error.  
 */
readline_wrapper::const_char_type*
readline_wrapper::__add_history(const_char_type* const hl) {
#ifdef	USE_READLINE
	const_char_type* cursor = hl;
#ifdef	USE_HISTEDIT
	// returns int signaling error...
	HistEvent hev;	// not used
	hev.str = NULL;
	if (history(&*_el_history, &hev, H_ENTER, cursor) < 0) {
		// is this possible?
		return NULL;
	}
#if 0
	if (hev.str) {
		// This always happens!  although the documentation
		// doesn't say what the hell this pointer points to, 
		// nor does it say to whom the memory belongs.  
		// All I know is that free bitches if you try to delete it, 
		// so we best leave it alone.  
		cerr << "Fang, I got a non-null string from editline\'s "
			"history(), and I don\'t know what to do!" << endl;
		cerr << "got: " << hev.str << endl;
		free(const_cast<char_type*>(hev.str));
	}
#endif
#else
	add_history(RL_CONST_CAST(cursor));
#endif
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
#ifdef	USE_HISTEDIT
	return this->gets(current_prompt);
#else
	return this->gets(prompt);
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	NOTE: some ancient version of readline haas prototype without
	'const' in ANY of the arguments (v. 4.1, ca. 2002).  
	That is utterly unaacceptable.  We provide an internal workaround.  
	Q: should the prompt be sticky or only for the duration of this call?
	A: right now, is only temporary effect.
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
#ifdef	USE_HISTEDIT
	const value_saver<string>
		_p(current_prompt, _prompt);	// temporary effect only
#endif
do {
#ifdef	USE_READLINE
#ifdef	USE_HISTEDIT
	// note: el_gets returns a char* that is owned (I think) by
	// the EditLine library, and thus should not be directly passed
	// to a manager pointer, but rather str-duplicated.
	// I deduced this because it sometimes returns the same buffer address.
	// Solution: change pointer type to never-own.
	int count;	// number of characters read by el_gets()
	hold_line = hold_line_type(el_gets(&*_editline, &count));
#else
	// (also covers editline)
	// NOTE: some ASS-version of readline accepts a char* 
	// for the prompt argument and trips an error here, 
	// hence the const_cast
	hold_line = hold_line_type(readline(
		RL_CONST_CAST(_prompt.c_str())));
#endif
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
	o << "GNU readline " << rl_library_version;
#elif	defined(HAVE_BSDEDITLINE)
#ifdef	USE_HISTEDIT
	o << "BSD EditLine (histedit interface)";
	#if	defined(LIBEDIT_MAJOR) && defined(LIBEDIT_MINOR)
	o << " ver. "  << LIBEDIT_MAJOR <<  '.' <<  LIBEDIT_MINOR;
	#endif
#else
	o << "BSD editline " << rl_library_version;
#endif
#else
	o << "none";
#endif
	return o;
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
/**
	Redraw the prompt and the current line buffer.
 */
void
readline_wrapper::refresh(void) {
#ifdef	HAVE_RL_FORCED_UPDATE_DISPLAY
	rl_forced_update_display();
#elif	defined(HAVE_RL_REDISPLAY)
	rl_redisplay();
#endif
}

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
void
readline_wrapper::display_match_list(char** matches, int len, int max) {
#ifdef	USE_READLINE
	rl_display_match_list(matches, len, max);
#endif
}

//=============================================================================
}	// end namespace util

