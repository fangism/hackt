/**
	\file "util/readline_wrap.cc"
	Simplified wrapper implementation for readline.  
	$Id: readline_wrap.cc,v 1.1.4.2 2006/01/12 06:13:32 fang Exp $
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
	NOTE: some ancient version of readline haas prototype without
	'const' in ANY of the arguments (v. 4.1, ca. 2002).  
	That is utterly unaacceptable.  We provide an internal workaround.  
	TODO: handle line continuations (ending with '\')
	TODO: parse semicolons (see sh behavior)
	\return allocated line.  
 */
readline_wrapper::const_char_type*
readline_wrapper::gets(void) {
	// this will automatically release the last line's memory
#if USE_READLINE
	// NOTE: some ASS version of readline accepts a char* 
	// for the prompt argument and trips an error here.  
	hold_line = hold_line_type(readline(
		RL_CONST_CAST(prompt.c_str())));
#else
	cout << prompt;
	hold_line = hold_line_type(static_cast<char_type*>(
		malloc(sizeof(char_type) *READLINE_BUFFER_SIZE)));
	fgets(&*hold_line, READLINE_BUFFER_SIZE, stdin);
#endif

	if (hold_line && *hold_line) {
		return __add_history(&*hold_line);
	} else {
		return (hold_line ? &*hold_line : NULL);
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

