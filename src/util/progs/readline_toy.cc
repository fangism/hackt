/**
	\file "util/progs/readline_toy.cc"
	Just a testing tool for readline behavior.  
	$Id: readline_toy.cc,v 1.1.2.1 2006/01/16 22:28:00 fang Exp $
 */

#include <iostream>
#include "util/readline_wrap.h"
#include "util/using_ostream.h"
#include "util/string.h"

using util::readline_wrapper;
using util::strings::eat_whitespace;

int
main(int argc, char* argv[]) {
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

