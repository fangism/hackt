/**
	\file "main/lexyacc.cc"
	Test program using unmangled, unhacked flex/yacc code.
	$Id: lexyacc.cc,v 1.1 2009/02/20 16:40:14 fang Exp $
 */

#include "main/main_funcs.h"
#include "parser/hackt-parse-real.h"	// for YYSTYPE
#include "AST/root.h"
#include "util/memory/count_ptr.h"

using util::memory::count_ptr;
using HAC::parser::root_body;

extern
root_body*
global_hackt_lval;

extern
int
hackt_parse(void);

int
main(int argc, char* argv[]) {
	count_ptr<root_body> p;
	try {
		hackt_parse();
		p = count_ptr<root_body>(global_hackt_lval);
	} catch (...) {
		return 1;
	}
	return 0;
}

