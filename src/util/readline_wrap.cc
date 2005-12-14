/**
	\file "util/readline_wrap.cc"
	Simplified wrapper implementation for readline.  
	$Id: readline_wrap.cc,v 1.1.4.1 2005/12/14 05:16:53 fang Exp $
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
	NOTE: some ancient version of readline haas prototype without
	'const' in ANY of the arguments (v. 4.1, ca. 2002).  
	That is utterly unaacceptable.  We provide an internal workaround.  
	TODO: handle line continuations (ending with '\')
	TODO: parse semicolons (see sh behavior)
	\return allocated line.  
 */
char*
readline_wrapper::gets(void) {
	// this will automatically release the last line's memory
#if	defined(HAVE_GNUREADLINE)  || defined(HAVE_BSDEDITLINE)
	// NOTE: some ASS version of readline accepts a char* 
	// for the prompt argument and trips an error here.  
	hold_line = hold_line_type(readline(
#if defined(READLINE_PROMPT_CONST) && !READLINE_PROMPT_CONST
		// your readline line header sucks
		const_cast<char*>
#endif
		(prompt.c_str())));
	if (hold_line && *hold_line) {
#if !defined(READLINE_PROMPT_CONST) || READLINE_PROMPT_CONST
		const
#endif
			char* cursor = &*hold_line;
		cursor = eat_whitespace(cursor);
		// this will internally strcpy the string
		add_history(cursor);
	}
	return (hold_line ? &*hold_line : NULL);
#else
	cout << prompt;
	hold_line = hold_line_type(static_cast<char*>(
		malloc(sizeof(char) *READLINE_BUFFER_SIZE)));
	return fgets(&*hold_line, READLINE_BUFFER_SIZE, stdin);
#endif
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

