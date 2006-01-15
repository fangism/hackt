/**
	\file "util/readline_wrap.cc"
	Simplified wrapper implementation for readline.  
	$Id: readline_wrap.cc,v 1.1.4.3.2.3 2006/01/15 22:02:51 fang Exp $
 */

#include <iostream>
#include "util/readline_wrap.h"
#include "util/readline.h"
#include "util/string.h"

/**
	Arbitrarily chosen buffer line size.  
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

readline_wrapper::readline_wrapper() : hold_line(NULL), prompt() { }

//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
readline_wrapper::readline_wrapper(const string& s) :
		hold_line(NULL), prompt(s) {
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
	Automatically trims whitespace.  
	\param hl the current hold line, must be non-NULL
		and point to a non-NUL character.  
 */
readline_wrapper::const_char_type*
readline_wrapper::__add_history(const_char_type* const hl) const {
#if USE_READLINE
	const_char_type* cursor = hl;
	eat_whitespace(cursor);
	// this will internally strcpy the string
	add_history(RL_CONST_CAST(cursor));
	return cursor;
#else
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
#if 0
	// echo debugging
	if (hold_line) {
		cout << "length: " << strlen(&*hold_line) << endl;
		cout << &*hold_line << endl;
	} else {
		cout << "NULL hold_line." << endl;
	}
#endif
	if (hold_line
#if defined(HAVE_BSDEDITLINE)
		&& strlen(&*hold_line)
#endif
	) {
		if (*hold_line) {
			return __add_history(&*hold_line);
		} else {
			return &*hold_line;
		}
	} else {
		// EOF
		return NULL;
	}
}

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

