/**
	\file "util/readline_wrap.cc"
	Simplified wrapper implementation for readline.  
	$Id: readline_wrap.cc,v 1.1 2005/12/10 07:08:29 fang Exp $
 */

#include <iostream>
#include <cstdio>       // for FILE
#include "util/readline_wrap.h"

#if	defined(HAVE_GNUREADLINE)
#include <readline/readline.h>
#include <readline/history.h>
#elif	defined(HAVE_BSDEDITLINE)
#include <editline/readline.h>
#endif	/* HAVE_GNUREADLINE || HAVE_BSDEDITLINE */

/**
	Arbitrarily chosen buffer line size.  
 */
#define	READLINE_BUFFER_SIZE		16384

namespace util {
#include "util/using_ostream.h"

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
	\return allocated line.  
 */
char*
readline_wrapper::gets(void) {
	// this will automatically release the last line's memory
#if	defined(HAVE_GNUREADLINE)  || defined(HAVE_BSDEDITLINE)
	hold_line = hold_line_type(readline(prompt.c_str()));
	if (hold_line && *hold_line) {
		add_history(&*hold_line);
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

