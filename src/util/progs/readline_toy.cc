/**
	\file "util/progs/readline_toy.cc"
	Just a testing tool for readline behavior.  
	$Id: readline_toy.cc,v 1.4 2009/02/19 02:58:39 fang Exp $
 */

#include <iostream>
#include <cstdlib>		// for system
#include "util/readline_wrap.h"
#include "util/using_ostream.h"
#include "util/string.h"

using util::readline_wrapper;
using util::strings::eat_whitespace;

int
main(int, char*[]) {
	// command-line parsing:
	static const char prompt[] = "> ";
	readline_wrapper rl(prompt);
	// do NOT delete this line, it's already managed.  
	const char* line;
	do {
		line = rl.gets();
		// this already eats leading whitespace
	if (line) {
		const char* cursor = line;
		// check for special case: shell escape
		if (*cursor == '!') {
			cursor++;
			eat_whitespace(cursor);
			const int es = system(cursor);
			if (es) {
				cerr << "*** Exit " << es << endl;
			}
		} else if (strlen(cursor)) {
			cout << "You wrote: " << cursor << endl;
		}
	}
	} while (line);
	cout << endl;
	return 0;
}

